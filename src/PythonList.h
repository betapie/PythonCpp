#pragma once
#ifndef PYTHON_LIST_H
#define PYTHON_LIST_H

#include "PythonTypeTraits.h"
#include "PythonObject.h"
#include "PythonError.h"
#include <initializer_list>
#include <vector>
#include <algorithm>

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
    private:
        // Helper proxy to return some kind of reference type from subscription operator
        class Reference
        {
        public:
            explicit Reference(PythonList& pyList, size_t idx) noexcept
                : m_list(pyList), m_idx(idx)
            {}

            Reference(const Reference& other) = delete;
            Reference& operator=(const Reference& other) = delete;

            Reference(Reference&& other) noexcept
                : m_list(other.m_list), m_idx(other.m_idx)
            {}

            Reference& operator=(Reference&& other) noexcept
            {
                m_list = other.m_list;
                m_idx = other.m_idx;
                return *this;
            }

            operator T()
            {
                auto pItem = PyList_GetItem(m_list.get(), m_idx);
                if (!pItem)
                    throw PythonError();
                return python_cast<T>(pItem);
            }

            Reference& operator=(const T& val)
            {
                auto pyObj = ToPythonObject(val);
                Py_INCREF(pyObj.get()); // PyList_SetItem steals a reference
                if (PyList_SetItem(m_list.get(), m_idx, pyObj.get()) == -1)
                    throw PythonError();
                return *this;
            }

        private:
            PythonList& m_list;
            size_t m_idx;
        };

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

        template<typename Container, typename val_t = typename Container::value_type, std::enable_if_t<isPythonBaseType_v<val_t>, int> = 0>
        PythonList(const Container& container)
        {
            m_pObject = PyList_New(container.size());
            if (!m_pObject)
                throw PythonError();
            auto idx = 0;
            std::for_each(container.begin(), container.end(), [&](const auto& elem)
                {
                    auto newElem = ToPythonObject(elem);
                    Py_INCREF(newElem.get()); // PyList_SetItem steals a ref..
                    if (PyList_SetItem(m_pObject, idx, newElem.get()) == -1)
                        throw PythonError();
                    ++idx;
                });
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

        Reference operator[](size_t idx)
        {
            return Reference(*this, idx);
        }

        T operator[](size_t idx) const
        {
            auto pItem = PyList_GetItem(m_pObject, idx);
            if (!pItem)
                throw PythonError();
            return python_cast<T>(pItem);
        }

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

        // I did not seem to find any way to access the raw data of the PyList object
        std::vector<T> ToVector()
        {
            std::vector<T> ret;
            ret.reserve((size()));
            for (size_t idx = 0; idx < size(); ++idx)
            {
                auto pItem = PyList_GetItem(m_pObject, idx);
                if (!pItem)
                    throw PythonError();
                ret.push_back(python_cast<T>(pItem));
            }
            return ret;
        }

    private:
    };
}

#endif // !PYTHON_LIST_H