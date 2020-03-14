#pragma once
#include "PythonTypeTraits.h"
#include "PythonObject.h"
#include <typeinfo>

namespace pycpp
{
    /*
        PythonList will can be created of a single type. If you want to use multiple types,
        use the generic PythonObject as template parameter
    */
    template<typename T>
    class PythonList : public PythonObject
    {
        static_assert(isPythonBaseType_v<T>, "PythonList T not valid PythonBaseType");
    public:
        // Default constructor will create a new PythonList with size 0
        // analog to myList = [] or myList = List() in Python
        PythonList()
        {
            m_pObject = PyList_New(0);
            if (!m_pObject)
                throw PythonError();
        }

        // Create a Python List of size sizeof(Args...) with the contents of args...
        template<typename... Args> // TODO enable_if Args... are convertible to T
        PythonList(Args... args)
        {

        }

        // Take ownership of an existing PyObject which points to a Python List or subtype of List
        // Will throw PythonError if object pointed to by PyObject* is not of List type
        PythonList(PyObject* pListObj)
            :PythonObject(pListObj)
        {
            if (PyList_Check(pListObj) == 0)
                throw PythonError("PyObject not of List type"); // TODO more info
        }

        // Copy / Move Construct and Assign

        [[nodiscard]] size_t size() noexcept
        {
            return PyList_Size(m_pObject); // can this fail in any way?
        }

        // How to implement operator[]? What shall it return?
        // Probably create a proxy PythonRef class that will allow
        // reading and writing to the value of the object

        void append(const T& val)
        {
            // convert val to PyObject of type T

            if (PyList_Append(m_pObject, nullptr) == -1)
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
            // convert val to PyObject

            if (PyList_Insert(m_pObject, index, nullptr) == -1)
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
            // TODO
        }

        void reverse()
        {
            // TODO
        }

    private:
    };
}