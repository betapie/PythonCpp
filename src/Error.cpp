#include "Error.h"

pycpp::Error::Error()
    : std::runtime_error(RetrievePyErrorString())
{}

pycpp::Error::Error(const std::string & errMsg)
    : std::runtime_error(errMsg)
{}

pycpp::Error::Error(const char* errMsg)
    : std::runtime_error(errMsg)
{}

std::string pycpp::Error::RetrievePyErrorString()
{
    PyObject* pyExcType;
    PyObject* pyExcValue;
    PyObject* pyExcTraceback;

    PyErr_Fetch(&pyExcType, &pyExcValue, &pyExcTraceback);
    PyErr_NormalizeException(&pyExcType, &pyExcValue, &pyExcTraceback);

    Object pyExcTypeObj(pyExcType);
    Object pyExcValueObj(pyExcValue);
    Object pyExcTracebackObj(pyExcTraceback);

    const auto typeString = pyExcTypeObj.StringRepr();
    const auto valueString = pyExcValueObj.StringRepr();
    const auto traceString = pyExcTracebackObj.StringRepr();

    return valueString + traceString;
}
