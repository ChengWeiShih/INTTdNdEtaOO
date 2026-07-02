#include <glob.h>
#include <iostream>
#include <string>

std::string GetSingleMatchedFile(const std::string& file_pattern)
{
    glob_t glob_result{};

    const int status = glob(file_pattern.c_str(), 0, nullptr, &glob_result);

    if (status != 0)
    {
        std::cerr << "Error: no file matches pattern: "
                  << file_pattern << std::endl;

        globfree(&glob_result);
        return "";
    }

    if (glob_result.gl_pathc != 1)
    {
        std::cerr << "Error: expected exactly one file matching: "
                  << file_pattern
                  << ", but found " << glob_result.gl_pathc
                  << std::endl;

        globfree(&glob_result);
        return "";
    }

    const std::string matched_file = glob_result.gl_pathv[0];

    globfree(&glob_result);

    return matched_file;
}