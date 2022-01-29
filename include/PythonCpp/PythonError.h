#pragma once
#ifndef PYTHON_ERROR_H
#define PYTHON_ERROR_H

#include "Python.h"
#include <stdexcept>
#include "PythonObject.h"

namespace pycpp
{
    /*
        PythonError represents the type of exception that will be thrown if any call to a Python
        C API function returns an error value. It will fetch the error/exception details from Python
        and make them available by calling what();
        You can also provide a custom error message
    */

    class PythonError : public std::runtime_error
    {
    public:
        PythonError();

        explicit PythonError(const std::string& errMsg);

        explicit PythonError(const char* errMsg);

    public:
        static std::string RetrievePyErrorString();
    };
}

#endif //PYTHON_ERROR_H