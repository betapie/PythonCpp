#pragma once

#if defined _WIN32
    #ifdef PYCPP_EXPORTS
        #define PYCPP_API __declspec(dllexport)
    #else
        #define PYCPP_API __declspec(dllimport)
    #endif
#elif defined __GNUC__ && __GNUC__ >= 4 || defined(__APPLE__))
    #ifdef PYCPP_EXPORTS
        #define PYCPP_API __attribute__ ((visibility ("default")))
    #endif
#else
    #define PYCPP_API
#endif