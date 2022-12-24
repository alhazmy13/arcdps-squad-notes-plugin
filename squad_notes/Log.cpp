#include "Log.h"

#include <fstream>
#include <chrono>


std::string get_file_timestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream output_stream;

    struct tm time_info;
    const auto errno_value = localtime_s(&time_info, &in_time_t);
    if (errno_value != 0)
    {
        throw std::runtime_error("localtime_s() failed: " + std::to_string(errno_value));
    }

    output_stream << std::put_time(&time_info, "%Y-%m-%d.%H_%M_%S");
    return output_stream.str();
}
void Log::Logger(std::string pFunctionName, std::string logMsg) {
    try {
        std::string filePath = "addons\\logs\\arcdps_squad_notes.txt";
        std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
        ofs << get_file_timestamp() << '\t' << pFunctionName  << '\t' << logMsg << '\n';
        ofs.close();
    }
    catch (const std::exception& e) {
        // some exception was thrown!
    }
}