#include "Sys.h"

void pycpp::Sys::AddToPath(const std::string& path)
{
    auto* py_path_list = PySys_GetObject("path");
    if (!py_path_list)
        throw Error();

    pycpp::Object py_path_string = PyUnicode_FromString(path.c_str());
    if (!py_path_string)
        throw Error();

    if (PyList_Append(py_path_list, py_path_string.get()) != 0)
        throw Error();
}

void pycpp::Sys::AddToPath(const char* path)
{
    auto* py_path_list = PySys_GetObject("path");
    if (!py_path_list)
        throw Error();

    pycpp::Object py_path_string = PyUnicode_FromString(path);
    if (!py_path_string)
        throw Error();

    if (PyList_Append(py_path_list, py_path_string.get()) != 0)
        throw Error();
}
