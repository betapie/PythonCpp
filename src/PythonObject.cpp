#include "PythonObject.h"
#include "PythonError.h"

pycpp::PythonObject::PythonObject(nullptr_t) noexcept
{}

pycpp::PythonObject::PythonObject(PyObject * pObject) noexcept
    : m_pObject(pObject)
{}

pycpp::PythonObject::PythonObject(const PythonObject & other)
{
    m_pObject = other.m_pObject;
    Py_INCREF(m_pObject);
}

pycpp::PythonObject& pycpp::PythonObject::operator=(const PythonObject& other)
{
    if (this == &other)
        return *this;

    Release();
    m_pObject = other.m_pObject;
    Py_INCREF(m_pObject);

    return *this;
}

pycpp::PythonObject::PythonObject(PythonObject&& other) noexcept
{
    m_pObject = other.m_pObject;
    other.m_pObject = nullptr;
}

pycpp::PythonObject& pycpp::PythonObject::operator=(PythonObject&& other) noexcept
{
    if (this == &other)
        return *this;

    Release();
    m_pObject = other.m_pObject;
    other.m_pObject = nullptr;
    return *this;
}

pycpp::PythonObject pycpp::PythonObject::operator=(PyObject* pObject) noexcept
{
    Release();
    m_pObject = pObject;
    return *this;
}

pycpp::PythonObject& pycpp::PythonObject::operator=(nullptr_t)
{
    Release();
    return *this;
}

pycpp::PythonObject::~PythonObject()
{
    Release();
}

PyObject* pycpp::PythonObject::get() const noexcept
{
    return m_pObject;
}

PyObject* pycpp::PythonObject::operator->() const noexcept
{
    return m_pObject;
}

pycpp::PythonObject::operator bool() const noexcept
{
    return m_pObject != nullptr;
}

void pycpp::PythonObject::Release()
{
    if (m_pObject)
    {
        Py_DECREF(m_pObject);
        m_pObject = nullptr;
    }
}

// This method will on success yield the same result als the buildin repr() function in Python would.
// This can also fail. However, since this is needed to deduct PythonError::what() we will not throw here
// In the worst case, the returned string is null

std::string pycpp::PythonObject::StringRepr() noexcept
{
    if (!m_pObject)
        return std::string();
    PythonObject pyStringRepr = PyObject_Repr(m_pObject);
    if (!pyStringRepr)
        return std::string();
    PythonObject pyStr = PyUnicode_AsEncodedString(pyStringRepr.get(), "utf-8", "~E~");
    if (!pyStr)
        return std::string();
    return std::string(PyBytes_AsString(pyStr.get()));
}

bool pycpp::PythonObject::HasAttribute(const char* attribute) noexcept
{
    return PyObject_HasAttrString(m_pObject, attribute) == 1;
}

bool pycpp::PythonObject::HasAttribute(const std::string& str) noexcept
{
    return PyObject_HasAttrString(m_pObject, str.c_str()) == 1;
}

pycpp::PythonObject pycpp::PythonObject::GetAttribute(const char* attribute)
{
    auto pRes = PyObject_GetAttrString(m_pObject, attribute);
    if (!pRes)
        throw(PythonError());
    return PythonObject(pRes);
}

pycpp::PythonObject pycpp::PythonObject::GetAttribute(const std::string& str)
{
    return GetAttribute(str.c_str());
}

pycpp::PythonObject pycpp::PythonObject::BorrowedRef(PyObject* pPyObj)
{
    if (pPyObj)
        Py_INCREF(pPyObj);
    return PythonObject(pPyObj);
}
