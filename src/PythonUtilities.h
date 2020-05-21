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

    template<typename... Args>
    PythonObject BuildValue(const char* format, Args... args)
    {
        PythonObject ret = Py_BuildValue(format, args...);
        if (!ret)
            throw PythonError();
        return ret;
    }

    PythonObject GetAttributeString(PyObject* pObject, const char* attr_name);
    PythonObject GetAttributeString(const PythonObject& pObject, const char* attr_name);
    PythonObject GetAttributeString(PyObject* pObject, const std::string& attr_name);
    PythonObject GetAttributeString(const PythonObject& pObject, const std::string& attr_name);

    PythonObject CallObject(PyObject* pCallableObject, PyObject* pArglist);
    PythonObject CallObject(const PythonObject& pCallableObject, PyObject* pArglist);
    PythonObject CallObject(PyObject* pCallableObject, const PythonObject& pArglist);
    PythonObject CallObject(const PythonObject& pCallableObject, const PythonObject& pArglist);
}

#endif // PYTHON_UTILITIES_H