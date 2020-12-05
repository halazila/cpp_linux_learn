#include <string>
#include "CommonStruct.h"

thread_local std::string StaticDefines::sqlite_error_msg = "";

const char *StaticDefines::AllTableNames[] = {
    "ManageUser",
    "ServerConfig",
    "StrategyConfig",
    "DeployConfig",
    "DeployGroup",
};