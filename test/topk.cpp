#include <iostream>
#include <vector>

using namespace std;

void topkhelper(vector<int> &input, int k, int left, int right)
{
    if (left < right)
    {
        int i = left, j = right;
        int p = input[i];
        while (i < j)
        {
            while (i < j && p <= input[j])
                j--;
            while (i < j && input[i] <= p)
                i++;
            swap(input[i], input[j]);
        }
        swap(input[left], input[i]);
        if (i < k)
            topkhelper(input, k, i + 1, right);
        else if (i > k)
            topkhelper(input, k, left, i - 1);
    }
}

vector<int> topK(vector<int> &input, int k)
{
    vector<int> ans;
    topkhelper(input, k - 1, 0, input.size() - 1);
    for (int i = 0; i < k; i++)
    {
        ans.push_back(input[i]);
    }
    return std::move(ans);
}

int main(int argc, char *argv[])
{
    vector<int> nums{3, 0, 1, 5, 4, 7, 2, 9};
    auto topVec = topK(nums, 5);
    for (size_t i = 0; i < topVec.size(); i++)
    {
        cout << topVec[i] << ",";
    }
    return 0;
}
