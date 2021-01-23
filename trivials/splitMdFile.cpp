#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>
#include <fcntl.h>

std::vector<std::string> splitString(std::string srcstr, const std::string &delimiter);

int fixread(int fd, char *buf, int len);
int fixwrite(int fd, char *buf, int len);
void setBlock(int fd, bool block);

struct PipeInfo
{
    int pid;
    int _pipe[2];
    PipeInfo()
    {
        memset(this, 0, sizeof(PipeInfo));
    }
    PipeInfo(int npid, int _npipe[2])
    {
        pid = npid;
        _pipe[0] = _npipe[0];
        _pipe[1] = _npipe[1];
    }
    PipeInfo(const PipeInfo &other)
    {
        pid = other.pid;
        _pipe[0] = other._pipe[0];
        _pipe[1] = other._pipe[1];
    }
};

std::unordered_map<std::string, PipeInfo> mapPipe;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Please set input csv file" << std::endl;
        return 0;
    }
    //open input file
    std::ifstream ifs(argv[1]);
    if (!ifs.is_open())
    {
        std::cout << "open input csv file error" << std::endl;
        return 0;
    }

    //make dst sub dir
    std::vector<std::string> fileVec = splitString(argv[1], ".");
    std::string subDir = "Split_" + fileVec[0];
    if (mkdir(subDir.c_str(), 0755) == -1)
    {
        if (errno != EEXIST)
        {
            std::cout << "Creat split dir error: " << strerror(errno) << std::endl;
            return 0;
        }
    }
    //read csv head line
    std::string csvHead;
    size_t headLen = 0;
    getline(ifs, csvHead);
    std::string line;
    std::vector<std::string> spltStrs;
    while (!ifs.eof())
    {
        getline(ifs, line);
        spltStrs = splitString(line, ",");
        std::string strIns = spltStrs[0];
        if (strIns.empty())
            continue;
        auto it = mapPipe.find(strIns);
        if (it == mapPipe.end())
        {
            int _pipe[2];
            int lineLen = line.length();
            int ret = pipe(_pipe);
            if (ret < 0)
            {
                perror("pipe\n");
            }
            else
            {
                mapPipe[strIns] = {0, _pipe};
                int pid = fork();
                if (pid == 0) //child
                {
                    close(_pipe[1]); //close write-end
                    setBlock(_pipe[0], false);
                    char rbuf[512] = {0};
                    std::ofstream dstFile((subDir + "/" + fileVec[0] + strIns + ".csv").c_str());
                    if (dstFile.is_open())
                    {
                        dstFile << csvHead << std::endl;
                        while (1)
                        {
                            memset(rbuf, 0, sizeof(rbuf));
                            int n;
                            if (fixread(_pipe[0], (char *)&n, sizeof(int)) < 1)
                                continue;
                            printf("%d: read length, %d\n", getpid(), n);
                            n = fixread(_pipe[0], rbuf, n);
                            if (n > 0)
                            {
                                // printf("%d: %s\n", getpid(), rbuf);
                                if (std::string(rbuf).find_first_of("end") != std::string::npos)
                                {
                                    printf("%d: read end, return\n", getpid());
                                    dstFile.close();
                                    return 0;
                                }
                                else
                                {
                                    dstFile << line << std::endl;
                                }
                            }
                            else if (n < 0)
                            {
                                if (errno == EAGAIN || errno == EWOULDBLOCK)
                                    continue;
                                // printf("%d: read error: %s, return\n", getpid(), strerror(errno));
                                dstFile.close();
                                return 0;
                            }
                            else
                            {
                                // printf("%d: read zero bytes\n", getpid());
                            }
                        }
                    }
                    else
                        return 0;
                }
                else //parent
                {
                    mapPipe[strIns].pid = pid;
                    close(_pipe[0]); //close read-end
                    setBlock(_pipe[1], false);
                    int n = line.length();
                    fixwrite(_pipe[1], (char *)&n, sizeof(int)); //
                    fixwrite(_pipe[1], (char *)line.c_str(), line.length());
                }
            }
        }
        else
        {
            int *_pipe = it->second._pipe;
            int n = line.length();
            fixwrite(_pipe[1], (char *)&n, sizeof(int)); //
            fixwrite(_pipe[1], (char *)line.c_str(), line.length());
        }
    }
    //write stop to child process
    for (auto it = mapPipe.begin(); it != mapPipe.end(); it++)
    {
        int *_pipe = it->second._pipe;
        // printf("pipe %s:%d,%d\n", it->first.c_str(), _pipe[0], _pipe[1]);
        int n = 3;
        fixwrite(_pipe[1], (char *)&n, sizeof(int)); //
        fixwrite(_pipe[1], (char *)"end", n);
    }
    // sleep(2);
    for (auto it = mapPipe.begin(); it != mapPipe.end(); it++)
    {
        close(it->second._pipe[1]);
        waitpid(it->second.pid, NULL, 0);
    }
    ifs.close();
    return 0;
}

std::vector<std::string> splitString(std::string srcstr, const std::string &delimiter)
{
    std::vector<std::string> ans;
    size_t lpos = 0, rpos = 0;
    while (rpos != std::string::npos)
    {
        rpos = srcstr.find_first_of(delimiter, lpos);
        ans.push_back(srcstr.substr(lpos, rpos == std::string::npos ? rpos : rpos - lpos));
        if (rpos != std::string::npos)
        {
            lpos = rpos + delimiter.length();
        }
    }
    return std::move(ans);
}

int fixread(int fd, char *buf, int len)
{
    int ntotal = 0, nread;
    while (ntotal != len)
    {
        nread = read(fd, buf, len - ntotal);
        if (nread == 0)
            return ntotal;
        if (nread < 0)
            return ntotal == 0 ? -1 : ntotal;
        ntotal += nread;
        buf += nread;
    }
    return ntotal;
}
int fixwrite(int fd, char *buf, int len)
{
    int ntotal = 0, nwrite;
    while (ntotal != len)
    {
        nwrite = write(fd, buf, len - ntotal);
        if (nwrite == 0)
            return ntotal;
        if (nwrite < 0)
            return ntotal == 0 ? -1 : ntotal;
        ntotal += nwrite;
        buf += nwrite;
    }
    return ntotal;
}

void setBlock(int fd, bool block)
{
    int flags = fcntl(fd, F_GETFL);
    if (block)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}
