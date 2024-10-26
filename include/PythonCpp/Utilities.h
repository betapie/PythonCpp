#pragma once
#ifndef PYCPP_UTILITIES_H
#define PYCPP_UTILITIES_H

/*
    Mostly wrappers around the C API functions that can use PythonObjects and throw on errors
*/

#include "Object.h"
#include "Error.h"
#include <string>

namespace pycpp
{
    PYCPP_API PythonObject ImportModule(const char* module_name);
    PYCPP_API PythonObject ImportModule(const std::string& module_name);

    PYCPP_API PythonObject GetAttributeString(PyObject* pObject, const char* attr_name);
    PYCPP_API PythonObject GetAttributeString(const PythonObject& pObject, const char* attr_name);
    PYCPP_API PythonObject GetAttributeString(PyObject* pObject, const std::string& attr_name);
    PYCPP_API PythonObject GetAttributeString(const PythonObject& pObject, const std::string& attr_name);
}

#endif // PYTHON_UTILITIES_H
