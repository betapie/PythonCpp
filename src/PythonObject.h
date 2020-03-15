#pragma once
#ifndef PYTHON_OBJECT_H
#define PYTHON_OBJECT_H

/*
    The PythonObject class resembles the basic wrapper for owning a PyObject*. Any returned
    PyObject* that has ownership (participates in the ref count) for the pointed to object
    can be wrapped in a PythonObject which will take care of the cleanup.
*/


#include "Python.h"
#include <string>

namespace pycpp
{
    class PythonObject
    {
    public:
        PythonObject() noexcept = default;

        PythonObject(nullptr_t) noexcept
        {}

        PythonObject(PyObject* pObject) noexcept
            : m_pObject(pObject)
        {}

        PythonObject(const PythonObject& other)
        {
            m_pObject = other.m_pObject;
            Py_INCREF(m_pObject);
        }

        PythonObject& operator=(const PythonObject& other)
        {
            if (this == &other)
                return *this;

            Release();
            m_pObject = other.m_pObject;
            Py_INCREF(m_pObject);

            return *this;
        }

        PythonObject(PythonObject&& other) noexcept
        {
            m_pObject = other.m_pObject;
            other.m_pObject = nullptr;
        }

        PythonObject& operator=(PythonObject&& other) noexcept
        {
            if (this == &other)
                return *this;
            
            Release();
            m_pObject = other.m_pObject;
            other.m_pObject = nullptr;
            return *this;
        }

        PythonObject operator=(PyObject* pObject) noexcept
        {
            m_pObject = pObject;
        }

        PythonObject& operator=(nullptr_t)
        {
            Release();
        }

        virtual ~PythonObject()
        {
            Release();
        }

        [[nodiscard]] PyObject* get() const noexcept
        {
            return m_pObject;
        }

        [[nodiscard]] PyObject* operator->() const
        {
            return m_pObject;
        }

        [[nodiscard]] operator bool() const noexcept
        {
            return m_pObject != nullptr;
        }

        inline void Release()
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
        [[nodiscard]] std::string StringRepr() noexcept
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

    protected:
        PyObject* m_pObject = nullptr;
    };

    /*template<typename return_type, typename... Args>
    class PythonFunction
    {
    public:
        return_type operator()(Args&&...)
        {
            
            return return_type();
        }
    private:

    };*/
}

#endif // PYTHON_OBJECT_H