#pragma once
#ifndef PYCPP_OBJECT_H
#define PYCPP_OBJECT_H

/*
    The Object class resembles the basic wrapper for owning a PyObject*. Any returned
    PyObject* that has ownership (participates in the ref count) for the pointed to object
    can be wrapped in a Object which will take care of the cleanup.
*/


#include "Python.h"
#include <PythonCpp/Defines.h>
#include <cstddef>
#include <string>

namespace pycpp
{
    class PYCPP_API Object
    {
    public:
        Object() noexcept = default;

        Object(std::nullptr_t) noexcept;

        Object(PyObject* pObject) noexcept;

        Object(const Object& other);

        Object& operator=(const Object& other);

        Object(Object&& other) noexcept;

        Object& operator=(Object&& other) noexcept;

        Object operator=(PyObject* pObject) noexcept;

        Object& operator=(std::nullptr_t);

        virtual ~Object();

        [[nodiscard]] PyObject* get() const noexcept;

        [[nodiscard]] PyObject* operator->() const noexcept;

        [[nodiscard]] operator bool() const noexcept;

        inline void Release();

        // This method will on success yield the same result als the buildin repr() function in Python would.
        // This can also fail. However, since this is needed to deduct Error::what() we will not throw here
        // In the worst case, the returned string is null
        [[nodiscard]] std::string StringRepr() noexcept;

        [[nodiscard]] bool HasAttribute(const char* attribute) noexcept;
        [[nodiscard]] bool HasAttribute(const std::string& str) noexcept;

        Object GetAttribute(const char* attribute);
        Object GetAttribute(const std::string& str);

        [[nodiscard]] static Object BorrowedRef(PyObject* pPyObj);

    protected:
        PyObject* m_pObject = nullptr;
    };
}

#endif // PYTHON_OBJECT_H
