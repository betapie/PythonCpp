#include "Object.h"
#include "Error.h"

pycpp::Object::Object(std::nullptr_t) noexcept
{}

pycpp::Object::Object(PyObject * pObject) noexcept
    : m_pObject(pObject)
{}

pycpp::Object::Object(const Object & other)
{
    m_pObject = other.m_pObject;
    Py_INCREF(m_pObject);
}

pycpp::Object& pycpp::Object::operator=(const Object& other)
{
    if (this == &other)
        return *this;

    Release();
    m_pObject = other.m_pObject;
    Py_INCREF(m_pObject);

    return *this;
}

pycpp::Object::Object(Object&& other) noexcept
{
    m_pObject = other.m_pObject;
    other.m_pObject = nullptr;
}

pycpp::Object& pycpp::Object::operator=(Object&& other) noexcept
{
    if (this == &other)
        return *this;

    Release();
    m_pObject = other.m_pObject;
    other.m_pObject = nullptr;
    return *this;
}

pycpp::Object pycpp::Object::operator=(PyObject* pObject) noexcept
{
    Release();
    m_pObject = pObject;
    return *this;
}

pycpp::Object& pycpp::Object::operator=(std::nullptr_t)
{
    Release();
    return *this;
}

pycpp::Object::~Object()
{
    Release();
}

PyObject* pycpp::Object::get() const noexcept
{
    return m_pObject;
}

PyObject* pycpp::Object::operator->() const noexcept
{
    return m_pObject;
}

pycpp::Object::operator bool() const noexcept
{
    return m_pObject != nullptr;
}

void pycpp::Object::Release()
{
    if (m_pObject)
    {
        Py_DECREF(m_pObject);
        m_pObject = nullptr;
    }
}

// This method will on success yield the same result als the buildin repr() function in Python would.
// This can also fail. However, since this is needed to deduct Error::what() we will not throw here
// In the worst case, the returned string is null

std::string pycpp::Object::StringRepr() noexcept
{
    if (!m_pObject)
        return std::string();
    Object pyStringRepr = PyObject_Repr(m_pObject);
    if (!pyStringRepr)
        return std::string();
    Object pyStr = PyUnicode_AsEncodedString(pyStringRepr.get(), "utf-8", "~E~");
    if (!pyStr)
        return std::string();
    return std::string(PyBytes_AsString(pyStr.get()));
}

bool pycpp::Object::HasAttribute(const char* attribute) noexcept
{
    return PyObject_HasAttrString(m_pObject, attribute) == 1;
}

bool pycpp::Object::HasAttribute(const std::string& str) noexcept
{
    return PyObject_HasAttrString(m_pObject, str.c_str()) == 1;
}

pycpp::Object pycpp::Object::GetAttribute(const char* attribute)
{
    auto pRes = PyObject_GetAttrString(m_pObject, attribute);
    if (!pRes)
        throw(Error());
    return Object(pRes);
}

pycpp::Object pycpp::Object::GetAttribute(const std::string& str)
{
    return GetAttribute(str.c_str());
}

pycpp::Object pycpp::Object::BorrowedRef(PyObject* pPyObj)
{
    if (pPyObj)
        Py_INCREF(pPyObj);
    return Object(pPyObj);
}
