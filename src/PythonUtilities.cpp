#include "PythonUtilities.h"
#include "PythonError.h"

pycpp::PythonObject pycpp::ImportModule(const char* module_name)
{
   PythonObject ret = PyImport_ImportModule(module_name);
    if (!ret)
        throw PythonError();
    return ret;
}
