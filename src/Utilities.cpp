#include <PythonCpp/Utilities.h>
#include <PythonCpp/Error.h>

pycpp::Object pycpp::ImportModule(const char* module_name)
{
    Object retVal = PyImport_ImportModule(module_name);
    if (!retVal)
        throw Error();
    return retVal;
}

pycpp::Object pycpp::ImportModule(const std::string& module_name)
{
    return ImportModule(module_name.c_str());
}

pycpp::Object pycpp::GetAttributeString(PyObject* pObject, const char* attr_name)
{
    Object retVal = PyObject_GetAttrString(pObject, attr_name);
    if (!retVal)
        throw Error();
    return retVal;
}

pycpp::Object pycpp::GetAttributeString(const Object& pObject, const char* attr_name)
{
    return GetAttributeString(pObject.get(), attr_name);
}

pycpp::Object pycpp::GetAttributeString(PyObject* pObject, const std::string& attr_name)
{
    return GetAttributeString(pObject, attr_name.c_str());
}

pycpp::Object pycpp::GetAttributeString(const Object& pObject, const std::string& attr_name)
{
    return GetAttributeString(pObject.get(), attr_name.c_str());
}
