#pragma once
#ifndef PYTHON_ERROR_H
#define PYTHON_ERROR_H

#include "Python.h"
#include <stdexcept>
#include <PythonCpp/Object.h>

namespace pycpp
{
    /*
        Error represents the type of exception that will be thrown if any call to a Python
        C API function returns an error value. It will fetch the error/exception details from Python
        and make them available by calling what();
        You can also provide a custom error message
    */

    class PYCPP_API Error : public std::runtime_error
    {
    public:
        Error();

        explicit Error(const std::string& errMsg);

        explicit Error(const char* errMsg);

    public:
        static std::string RetrievePyErrorString();
    };
}

#endif //PYTHON_ERROR_H
