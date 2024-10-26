#pragma once

/*
    This is the header that should be included in your project. It makes sure that "Python.h" is included
    before any stl headers. This is important due to a few typedefs in "Python.h".
*/

#ifndef PYTHON_CPP_H
#define PYTHON_CPP_H

#include "Python.h"
#include "Object.h"
#include "Error.h"
#include "TypeTraits.h"
#include "Interpreter.h"
#include "Sys.h"
#include "List.h"
#include "Tuple.h"
#include "Callable.h"
#include "Utilities.h"

#endif // PYTHON_CPP_H
