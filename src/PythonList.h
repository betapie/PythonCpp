#pragma once
#ifndef PYTHON_LIST_H
#define PYTHON_LIST_H

#include "PythonTypeTraits.h"
#include "PythonObject.h"
#include "PythonError.h"
#include <initializer_list>

// WARNING: THIS IS NOWHERE NEAR FEATURE COMPLETE!

namespace pycpp
{
    /*
        PythonList will can be created of a single type. If you want to use multiple types,
        use the generic PythonObject as template parameter. For the time being, do not do that
        as it's not safe
    */
    template<typename T>
    class PythonList : public PythonObject
    {
        static_assert(isPythonBaseType_v<T>, "PythonList<T>: T is not a valid PythonBaseType");
    public:
        // Default constructor will create a new PythonList with size 0
        // analog to myList = [] or myList = List() in Python
        PythonList()
        {
            m_pObject = PyList_New(0);
            if (!m_pObject)
                throw PythonError();
        }

        // Variadic Constructor disabled atm. Use the initializer_list overload

        //// Create a Python List of size sizeof(Args...) with the contents of args...
        //template<typename... Args> // TODO enable_if Args... are convertible to T
        //PythonList(Args... args)
        //{
        //    m_pObject = PyList_New(sizeof(Args...));
        //    // Initialize the values
        //}

        PythonList(const std::initializer_list<T>& iList)
        {
            m_pObject = PyList_New(iList.size());
            if (!m_pObject)
                throw PythonError();
            size_t idx = 0;
            for (const auto& elem : iList)
            {
                auto newElem = ToPythonObject(elem);
                Py_INCREF(newElem.get()); // PyList_SetItem steals a ref..
                if (PyList_SetItem(m_pObject, idx, newElem.get()) == -1)
                    throw PythonError();
                ++idx;
            }
        }

        // Take ownership of an existing PyObject which points to a Python List or subtype of List
        // Will throw PythonError if object pointed to by PyObject* is not of List type
        PythonList(PyObject* pListObj)
            :PythonObject(pListObj)
        {
            if (PyList_Check(pListObj) == 0)
                throw PythonError("PyObject not of List type"); // TODO more info
        }

        PythonList(const PythonList& other)
            :PythonObject(other)
        {}

        PythonList& operator=(const PythonList& other)
        {
            PythonObject::operator=(other);
            return *this;
        }

        PythonList(PythonList&& other) noexcept
            :PythonObject(std::move(other))
        {}

        PythonList& operator=(PythonList&& other) noexcept
        {
            PythonObject::operator=(std::move(other));
            return *this;
        }

        PythonList(const PythonObject& other)
            :PythonObject(other)
        {
            if (PyList_Check(m_pObject) == 0)
                throw PythonError("PyObject not of List type"); // TODO more info
        }

        PythonList& operator=(const PythonObject& other)
        {
            if (PyList_Check(other.m_pObject) == 0)
                throw PythonError("PyObject not of List type"); // TODO more info
            PythonObject::operator=(other);
            return *this;
        }

        // Note: No move construction/assignment from PythonObject, because due to the PyList_Check
        // they cannot be defined noexcept!

        [[nodiscard]] size_t size() const noexcept
        {
            return PyList_Size(m_pObject); // can this fail in any way?
        }

        // How to implement operator[]? What shall it return?
        // Probably create a proxy PythonRef class that will allow
        // reading and writing to the value of the object

        void append(const T& val)
        {
            const auto py_val = ToPythonObject(val);

            if (PyList_Append(m_pObject, py_val.get()) == -1)
                throw PythonError();
        }

        void append(const PythonObject& pyObj)
        {
            if (PyList_Append(m_pObject, pyObj.get()) == -1)
                throw PythonError();
        }

        void append(PyObject* pPyObj)
        {
            if (PyList_Append(m_pObject, pPyObj) == -1)
                throw PythonError();
        }

        void insert(size_t index, const T& val)
        {
            const auto py_val = ToPythonObject(val);

            if (PyList_Insert(m_pObject, index, py_val.get()) == -1)
                throw PythonError();
        }

        void insert(size_t index, const PythonObject& pyObj)
        {
            if (PyList_Insert(m_pObject, index, pyObj.get()) == -1)
                throw PythonError();
        }

        void insert(size_t index, PyObject* pPyObj)
        {
            if (PyList_Insert(m_pObject, index, pPyObj) == -1)
                throw PythonError();
        }

        void sort()
        {
            if (PyList_Sort(m_pObject) == -1)
                throw PythonError();
        }

        void reverse()
        {
            if (PyList_Reverse(m_pObject) == -1)
                throw PythonError();
        }

    private:
    };
}

#endif // !PYTHON_LIST_H