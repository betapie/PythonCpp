#include <PythonCpp/Callable.h>

pycpp::Object pycpp::CallObject(PyObject* pCallableObject, PyObject* pArglist)
{
    Object retVal = PyEval_CallObject(pCallableObject, pArglist);
    if (!retVal)
        throw Error();
    return retVal;
}

pycpp::Object pycpp::CallObject(const Object& callableObject, PyObject* pArglist)
{
    return CallObject(callableObject.get(), pArglist);
}

pycpp::Object pycpp::CallObject(PyObject* pCallableObject, const Object& arglist)
{
    return CallObject(pCallableObject, arglist.get());
}

pycpp::Object pycpp::CallObject(const Object& callableObject, const Object& arglist)
{
    return CallObject(callableObject.get(), arglist.get());
}

pycpp::Callable::Callable(PyObject* pCallableObject)
    :Object(pCallableObject)
{
    if (PyCallable_Check(m_pObject) != 1)
        throw(Error("PyObject not callable"));
}

pycpp::Callable::Callable(const Callable& other)
    :Object(other)
{}

pycpp::Callable& pycpp::Callable::operator=(const Callable & other)
{
    Object::operator=(other);
    return *this;
}

pycpp::Callable::Callable(Callable&& other) noexcept
    :Object(std::move(other))
{}

pycpp::Callable& pycpp::Callable::operator=(Callable && other) noexcept
{
    Object::operator=(std::move(other));
    return *this;
}

pycpp::Callable::Callable(const Object& other)
    :Object(other)
{
    if (PyCallable_Check(m_pObject) == 0)
        throw Error("PyObject not Callable"); // TODO more info
}

pycpp::Callable& pycpp::Callable::operator=(const Object& other)
{
    if (PyCallable_Check(other.get()) == 0)
        throw Error("PyObject not Callable"); // TODO more info
    Object::operator=(other);
    return *this;
}
