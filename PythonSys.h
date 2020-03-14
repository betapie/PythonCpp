#pragma once
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
        static void AddToSystemPath(const std::string& path)
        {
            auto* py_path_list = PySys_GetObject("path");
            if (!py_path_list)
                throw PythonError();

            pycpp::PythonObject py_path_string = PyUnicode_FromString(path.c_str());
            if (!py_path_string)
                throw PythonError();

            if (PyList_Append(py_path_list, py_path_string.get()) != 0)
                throw PythonError();
        }

        static void AddToSystemPath(const char* path)
        {
            auto* py_path_list = PySys_GetObject("path");
            if (!py_path_list)
                throw PythonError();

            pycpp::PythonObject py_path_string = PyUnicode_FromString(path);
            if (!py_path_string)
                throw PythonError();

            if (PyList_Append(py_path_list, py_path_string.get()) != 0)
                throw PythonError();
        }

    private:
    };
}