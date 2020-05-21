#include "PythonUtilities.h"
#include "PythonError.h"

pycpp::PythonObject pycpp::ImportModule(const char* module_name)
{
   PythonObject retVal = PyImport_ImportModule(module_name);
    if (!retVal)
        throw PythonError();
    return retVal;
}

pycpp::PythonObject pycpp::ImportModule(const std::string& module_name)
{
    return ImportModule(module_name.c_str());
}

pycpp::PythonObject pycpp::GetAttributeString(PyObject* pObject, const char* attr_name)
{
    PythonObject retVal = PyObject_GetAttrString(pObject, attr_name);
    if (!retVal)
        throw PythonError();
    return retVal;
}

pycpp::PythonObject pycpp::GetAttributeString(const PythonObject& pObject, const char* attr_name)
{
    return GetAttributeString(pObject.get(), attr_name);
}

pycpp::PythonObject pycpp::GetAttributeString(PyObject* pObject, const std::string& attr_name)
{
    return GetAttributeString(pObject, attr_name.c_str());
}

pycpp::PythonObject pycpp::GetAttributeString(const PythonObject& pObject, const std::string& attr_name)
{
    return GetAttributeString(pObject.get(), attr_name.c_str());
}

pycpp::PythonObject pycpp::CallObject(PyObject* pCallableObject, PyObject* arglist)
{
    PythonObject retVal = PyEval_CallObject(pCallableObject, arglist);
    if (!retVal)
        throw PythonError();
    return retVal;
}

pycpp::PythonObject pycpp::CallObject(const PythonObject& pCallableObject, PyObject* arglist)
{
    return CallObject(pCallableObject.get(), arglist);
}

pycpp::PythonObject pycpp::CallObject(PyObject* pCallableObject, const PythonObject& pArglist)
{
    return CallObject(pCallableObject, pArglist.get());
}

pycpp::PythonObject pycpp::CallObject(const PythonObject& pCallableObject, const PythonObject& pArglist)
{
    return CallObject(pCallableObject.get(), pArglist.get());
}
