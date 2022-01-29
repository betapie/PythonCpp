#pragma once

/*
    This is the header that should be included in your project. It makes sure that "Python.h" is included
    before any stl headers. This is important due to a few typedefs in "Python.h".
*/

#ifndef PYTHON_CPP_H
#define PYTHON_CPP_H

#include "Python.h"
#include "PythonObject.h"
#include "PythonError.h"
#include "PythonTypeTraits.h"
#include "PythonInterpreter.h"
#include "PythonSys.h"
#include "PythonList.h"
#include "PythonTuple.h"
#include "PythonCallable.h"
#include "PythonUtilities.h"

#endif // PYTHON_CPP_H