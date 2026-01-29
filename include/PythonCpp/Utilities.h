#pragma once
#ifndef PYCPP_UTILITIES_H
#define PYCPP_UTILITIES_H

/*
    Mostly wrappers around the C API functions that can use Objects and throw on
   errors
*/

#include <PythonCpp/Error.h>

#include <string>

#include "Object.h"

namespace pycpp {
PYCPP_API Object ImportModule(const char* module_name);
PYCPP_API Object ImportModule(const std::string& module_name);

PYCPP_API Object GetAttributeString(PyObject* pObject, const char* attr_name);
PYCPP_API Object GetAttributeString(const Object& pObject, const char* attr_name);
PYCPP_API Object
GetAttributeString(PyObject* pObject, const std::string& attr_name);
PYCPP_API Object
GetAttributeString(const Object& pObject, const std::string& attr_name);
}  // namespace pycpp

#endif  // PYTHON_UTILITIES_H
