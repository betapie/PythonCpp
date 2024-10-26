#pragma once

#if defined _WIN32
    #ifdef PYCPP_EXPORTS
        #define PYCPP_API __declspec(dllexport)
    #else
        #define PYCPP_API __declspec(dllimport)
    #endif
#else
    #define PYCPP_API
#endif