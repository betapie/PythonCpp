#include "PythonError.h"

pycpp::PythonError::PythonError()
    : std::runtime_error(RetrievePyErrorString())
{}

pycpp::PythonError::PythonError(const std::string & errMsg)
    : std::runtime_error(errMsg)
{}

pycpp::PythonError::PythonError(const char* errMsg)
    : std::runtime_error(errMsg)
{}

std::string pycpp::PythonError::RetrievePyErrorString()
{
    PyObject* pyExcType;
    PyObject* pyExcValue;
    PyObject* pyExcTraceback;

    PyErr_Fetch(&pyExcType, &pyExcValue, &pyExcTraceback);
    PyErr_NormalizeException(&pyExcType, &pyExcValue, &pyExcTraceback);

    PythonObject pyExcTypeObj(pyExcType);
    PythonObject pyExcValueObj(pyExcValue);
    PythonObject pyExcTracebackObj(pyExcTraceback);

    const auto typeString = pyExcTypeObj.StringRepr();
    const auto valueString = pyExcValueObj.StringRepr();
    const auto traceString = pyExcTracebackObj.StringRepr();

    return valueString + traceString;
}
