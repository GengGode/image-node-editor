#ifndef __CONSOLE_STREAM_H_
#define __CONSOLE_STREAM_H_

#include "global.define.h"
#include "convert.string.h"

namespace utils
{

    bool cout_is_utf8()
    {
        auto command = "chcp";
        auto res = get_command_result(command);
        if (res.find("65001") != std::string::npos)
            return true;
        return false;
    }
    bool cout_is_gbk()
    {
        auto command = "chcp";
        auto res = get_command_result(command);
        if (res.find("936") != std::string::npos)
            return true;
        return false;
    }
}

#endif // __CONSOLE_STREAM_H_