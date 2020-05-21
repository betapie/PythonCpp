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

        PythonObject(nullptr_t) noexcept;

        PythonObject(PyObject* pObject) noexcept;

        PythonObject(const PythonObject& other);

        PythonObject& operator=(const PythonObject& other);

        PythonObject(PythonObject&& other) noexcept;

        PythonObject& operator=(PythonObject&& other) noexcept;

        PythonObject operator=(PyObject* pObject) noexcept;

        PythonObject& operator=(nullptr_t);

        virtual ~PythonObject();

        [[nodiscard]] PyObject* get() const noexcept;

        [[nodiscard]] PyObject* operator->() const noexcept;

        [[nodiscard]] operator bool() const noexcept;

        inline void Release();

        // This method will on success yield the same result als the buildin repr() function in Python would.
        // This can also fail. However, since this is needed to deduct PythonError::what() we will not throw here
        // In the worst case, the returned string is null
        [[nodiscard]] std::string StringRepr() noexcept;

        [[nodiscard]] static PythonObject BorrowedRef(PyObject* pPyObj);

    protected:
        PyObject* m_pObject = nullptr;
    };
}

#endif // PYTHON_OBJECT_H