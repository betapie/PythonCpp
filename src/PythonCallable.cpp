#include "PythonCallable.h"

pycpp::PythonObject pycpp::CallObject(PyObject* pCallableObject, PyObject* pArglist)
{
    PythonObject retVal = PyEval_CallObject(pCallableObject, pArglist);
    if (!retVal)
        throw PythonError();
    return retVal;
}

pycpp::PythonObject pycpp::CallObject(const PythonObject& callableObject, PyObject* pArglist)
{
    return CallObject(callableObject.get(), pArglist);
}

pycpp::PythonObject pycpp::CallObject(PyObject* pCallableObject, const PythonObject& arglist)
{
    return CallObject(pCallableObject, arglist.get());
}

pycpp::PythonObject pycpp::CallObject(const PythonObject& callableObject, const PythonObject& arglist)
{
    return CallObject(callableObject.get(), arglist.get());
}

pycpp::PythonCallable::PythonCallable(PyObject* pCallableObject)
    :PythonObject(pCallableObject)
{
    if (PyCallable_Check(m_pObject) != 1)
        throw(PythonError("PyObject not callable"));
}

pycpp::PythonCallable::PythonCallable(const PythonCallable& other)
    :PythonObject(other)
{}

pycpp::PythonCallable& pycpp::PythonCallable::operator=(const PythonCallable & other)
{
    PythonObject::operator=(other);
    return *this;
}

pycpp::PythonCallable::PythonCallable(PythonCallable&& other) noexcept
    :PythonObject(std::move(other))
{}

pycpp::PythonCallable& pycpp::PythonCallable::operator=(PythonCallable && other) noexcept
{
    PythonObject::operator=(std::move(other));
    return *this;
}

pycpp::PythonCallable::PythonCallable(const PythonObject& other)
    :PythonObject(other)
{
    if (PyCallable_Check(m_pObject) == 0)
        throw PythonError("PyObject not Callable"); // TODO more info
}

pycpp::PythonCallable& pycpp::PythonCallable::operator=(const PythonObject& other)
{
    if (PyCallable_Check(other.get()) == 0)
        throw PythonError("PyObject not Callable"); // TODO more info
    PythonObject::operator=(other);
    return *this;
}
