#include <iostream>
#include <string>
#include <cstring>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#endif

std::string GetExecutablePath()
{
    char path[1024];

#if defined(_WIN32)
    DWORD size = GetModuleFileNameA(NULL, path, sizeof(path));
    if (size == 0 || size == sizeof(path)) throw std::runtime_error("Failed to get executable path");
    return std::string(path, size);

#elif defined(__APPLE__)
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) throw std::runtime_error("Buffer too small");
    return std::string(path);

#elif defined(__linux__)
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
    if (count == -1 || count == sizeof(path)) throw std::runtime_error("Failed to get path");
    return std::string(path, count);

#else
    #error "Unsupported platform"
#endif
}

std::string GetExecutableDirectory()
{
    std::string exePath = GetExecutablePath();

    size_t pos = exePath.find_last_of("/\\");
    
    if (pos != std::string::npos)
    {
        return exePath.substr(0, pos + 1);
    } else {
        throw std::runtime_error("Unable to extract directory from executable path");
    }
}

#define SAVE_PATH (std::string(GetExecutableDirectory()) + "/Photos/")
