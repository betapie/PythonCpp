#pragma once

/*
    This is the header that should be included in your project. It makes sure that "Python.h" is included
    before any stl headers. This is important due to a few typedefs in "Python.h".
*/

#ifndef PYTHON_CPP_H
#define PYTHON_CPP_H

#include "Python.h"
#include <PythonCpp/Object.h>
#include <PythonCpp/Error.h>
#include <PythonCpp/TypeTraits.h>
#include <PythonCpp/Interpreter.h>
#include <PythonCpp/Sys.h>
#include <PythonCpp/List.h>
#include <PythonCpp/Tuple.h>
#include <PythonCpp/Callable.h>
#include <PythonCpp/Utilities.h>

#endif // PYTHON_CPP_H
