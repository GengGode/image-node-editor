#pragma once
#include <string>
#include <vector>
namespace utils
{
    std::vector<std::string> split_string(const std::string &str, const std::string &delimiter)
    {
        std::vector<std::string> result;
        size_t last = 0;
        size_t next = 0;
        while ((next = str.find(delimiter, last)) != std::string::npos)
        {
            result.push_back(str.substr(last, next - last));
            last = next + delimiter.size();
        }
        result.push_back(str.substr(last));
        return result;
    }

} // namespace utils
