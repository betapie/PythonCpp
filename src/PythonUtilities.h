#pragma once

/*
    Mostly wrappers around the C API functions that can use PythonObjects and throw on errors
*/

#ifndef PYTHON_UTILITIES_H
#define PYTHON_UTILITIES_H

#include "PythonObject.h"

namespace pycpp
{
    PythonObject ImportModule(const char* module_name);
}

#endif // PYTHON_UTILITIES_H