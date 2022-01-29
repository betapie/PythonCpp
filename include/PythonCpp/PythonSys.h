#pragma once
#ifndef PYTHONSYS_H
#define PYTHONSYS_H

#include <string>
#include "PythonObject.h"
#include "PythonError.h"

/*
    Very high-level API to the sys module. This will only feature basic
    utilities that enable quick use of the Python API
*/

namespace pycpp
{
    class PythonSys
    {
    public:
        static void AddToPath(const std::string& path);

        static void AddToPath(const char* path);

    private:
    };
}

#endif // PYTHONSYS_H