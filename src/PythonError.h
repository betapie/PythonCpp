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
        PythonError()
            : std::runtime_error(RetrievePyErrorString())
        {}

        explicit PythonError(const std::string& errMsg)
            : std::runtime_error(errMsg)
        {}

        explicit PythonError(const char* errMsg)
            : std::runtime_error(errMsg)
        {}

    public:
        static std::string RetrievePyErrorString()
        {
            PyObject* pyExcType;
            PyObject* pyExcValue;
            PyObject* pyExcTraceback;

            PyErr_Fetch(&pyExcType, &pyExcValue, &pyExcTraceback);
            PyErr_NormalizeException(&pyExcType, &pyExcValue, &pyExcTraceback);

            PythonObject pyExcTypeObj(pyExcType);
            PythonObject pyExcValueObj(pyExcValue);
            PythonObject pyExcTracebackObj(pyExcTraceback);

            const auto typeString = pyExcTypeObj.StringRepr();
            const auto valueString = pyExcValueObj.StringRepr();
            const auto traceString = pyExcTracebackObj.StringRepr();

            return valueString + traceString;
        }
    };
}

#endif //PYTHON_ERROR_H