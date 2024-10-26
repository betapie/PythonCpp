#pragma once
#ifndef PYCPP_LIST_H
#define PYCPP_LIST_H

#include "TypeTraits.h"
#include "Object.h"
#include "Error.h"
#include <initializer_list>
#include <vector>
#include <algorithm>

namespace pycpp
{
    /*
        List can be created of a single type. If you want to use multiple types,
        use the generic Object as template parameter.
    */
    template<typename T>
    class List : public Object
    {
        static_assert(isPythonBaseType_v<T>, "List<T>: T is not a valid PythonBaseType");
    private:
        // Helper proxy to return some kind of reference type from subscription operator
        class Reference
        {
        public:
            explicit Reference(List& pyList, size_t idx) noexcept
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
                    throw Error();
                return python_cast<T>(pItem);
            }

            Reference& operator=(const T& val)
            {
                auto pyObj = ToObject(val);
                Py_INCREF(pyObj.get()); // PyList_SetItem steals a reference
                if (PyList_SetItem(m_list.get(), m_idx, pyObj.get()) == -1)
                    throw Error();
                return *this;
            }

        private:
            List& m_list;
            size_t m_idx;
        };

    public:
        // Default constructor will create a new List with size 0
        // analog to myList = [] or myList = List() in Python
        List()
        {
            m_pObject = PyList_New(0);
            if (!m_pObject)
                throw Error();
        }

        List(const std::initializer_list<T>& iList)
        {
            m_pObject = PyList_New(iList.size());
            if (!m_pObject)
                throw Error();
            size_t idx = 0;
            for (const auto& elem : iList)
            {
                auto newElem = ToObject(elem);
                Py_INCREF(newElem.get()); // PyList_SetItem steals a ref..
                if (PyList_SetItem(m_pObject, idx, newElem.get()) == -1)
                    throw Error();
                ++idx;
            }
        }

        template<typename Container, typename val_t = typename Container::value_type, std::enable_if_t<isPythonBaseType_v<val_t>, int> = 0>
        List(const Container& container)
        {
            m_pObject = PyList_New(container.size());
            if (!m_pObject)
                throw Error();
            auto idx = 0;
            std::for_each(container.begin(), container.end(), [&](const auto& elem)
                {
                    auto newElem = ToObject(elem);
                    Py_INCREF(newElem.get()); // PyList_SetItem steals a ref..
                    if (PyList_SetItem(m_pObject, idx, newElem.get()) == -1)
                        throw Error();
                    ++idx;
                });
        }

        // Take ownership of an existing PyObject which points to a Python List or subtype of List
        // Will throw Error if object pointed to by PyObject* is not of List type
        List(PyObject* pListObj)
            :Object(pListObj)
        {
            if (PyList_Check(pListObj) == 0)
                throw Error("PyObject not of List type"); // TODO more info
        }

        List(const List& other)
            :Object(other)
        {}

        List& operator=(const List& other)
        {
            Object::operator=(other);
            return *this;
        }

        List(List&& other) noexcept
            :Object(std::move(other))
        {}

        List& operator=(List&& other) noexcept
        {
            Object::operator=(std::move(other));
            return *this;
        }

        List(const Object& other)
            :Object(other)
        {
            if (PyList_Check(m_pObject) == 0)
                throw Error("PyObject not of List type"); // TODO more info
        }

        List& operator=(const Object& other)
        {
            if (PyList_Check(other.get()) == 0)
                throw Error("PyObject not of List type"); // TODO more info
            Object::operator=(other);
            return *this;
        }

        // Note: No move construction/assignment from Object, because due to the PyList_Check
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
                throw Error();
            return python_cast<T>(pItem);
        }

        void append(const T& val)
        {
            const auto py_val = ToObject(val);

            if (PyList_Append(m_pObject, py_val.get()) == -1)
                throw Error();
        }

        void append(const Object& pyObj)
        {
            if (PyList_Append(m_pObject, pyObj.get()) == -1)
                throw Error();
        }

        void append(PyObject* pPyObj)
        {
            if (PyList_Append(m_pObject, pPyObj) == -1)
                throw Error();
        }

        void insert(size_t index, const T& val)
        {
            const auto py_val = ToObject(val);

            if (PyList_Insert(m_pObject, index, py_val.get()) == -1)
                throw Error();
        }

        void insert(size_t index, const Object& pyObj)
        {
            if (PyList_Insert(m_pObject, index, pyObj.get()) == -1)
                throw Error();
        }

        void insert(size_t index, PyObject* pPyObj)
        {
            if (PyList_Insert(m_pObject, index, pPyObj) == -1)
                throw Error();
        }

        List slice(size_t lowIdx, size_t highIdx) const
        {
            return List(PyList_GetSlice(m_pObject, lowIdx, highIdx));
        }

        void sort()
        {
            if (PyList_Sort(m_pObject) == -1)
                throw Error();
        }

        void reverse()
        {
            if (PyList_Reverse(m_pObject) == -1)
                throw Error();
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
                    throw Error();
                ret.push_back(python_cast<T>(pItem));
            }
            return ret;
        }

    private:
    };

    template<typename Container>
    List(const Container& container)->List<typename Container::value_type>;

    template<typename T>
    List(std::initializer_list<T>)->List<T>;
}

#endif // !PYTHON_LIST_H
