#pragma once

/*
    Mostly wrappers around the C API functions that can use PythonObjects and throw on errors
*/

#ifndef PYTHON_UTILITIES_H
#define PYTHON_UTILITIES_H

#include "PythonObject.h"
#include "PythonError.h"
#include <string>

namespace pycpp
{
    PythonObject ImportModule(const char* module_name);
    PythonObject ImportModule(const std::string& module_name);

    PythonObject GetAttributeString(PyObject* pObject, const char* attr_name);
    PythonObject GetAttributeString(const PythonObject& pObject, const char* attr_name);
    PythonObject GetAttributeString(PyObject* pObject, const std::string& attr_name);
    PythonObject GetAttributeString(const PythonObject& pObject, const std::string& attr_name);
}

#endif // PYTHON_UTILITIES_H