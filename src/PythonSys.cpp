#include "PythonSys.h"

void pycpp::PythonSys::AddToPath(const std::string& path)
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

void pycpp::PythonSys::AddToPath(const char* path)
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
