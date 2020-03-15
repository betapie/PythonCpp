#pragma once

#ifndef PYTHON_TYPE_TRAITS_H
#define PYTHON_TYPE_TRAITS_H

#include <type_traits>
#include "PythonObject.h"
//#include "PythonList.h"

namespace pycpp
{
    template<typename T>
    struct isPythonBaseType : std::false_type
    {};

    template<>
    struct isPythonBaseType<bool> : std::true_type
    {};

    template<>
    struct isPythonBaseType<long> : std::true_type
    {};

    template<>
    struct isPythonBaseType<unsigned long> : std::true_type
    {};

    template<>
    struct isPythonBaseType<long long> : std::true_type
    {};

    template<>
    struct isPythonBaseType<unsigned long long> : std::true_type
    {};

    template<>
    struct isPythonBaseType<double> : std::true_type
    {};

    // using generic PythonObjects should also be allowed but can be dangerous
    template<>
    struct isPythonBaseType<PythonObject> : std::true_type
    {};

    template<typename T>
    class PythonList;

    template<typename U>
    struct isPythonBaseType<PythonList<U>> : std::conditional_t<isPythonBaseType<U>::value, std::true_type, std::false_type>
    {};

    template<typename T>
    constexpr auto isPythonBaseType_v = isPythonBaseType<T>::value;

    template<typename T> // base template, this will not do anything except warning about wrong types
    [[nodiscard]] PythonObject ToPythonObject(const T& val)
    {
        static_assert(isPythonBaseType_v<T>, "ToPythonObject: Type not supported");
        throw PythonError("How did we even get here?");
    }

    // note: no implementations for PyLong_FromDouble... and other implicit conversions. If you want your PythonObject
    // to be of a certain type, be explicit about the type you pass in

    template<> // refactor when wrappers around PyLong_From<type> have been implemented
    [[nodiscard]] PythonObject ToPythonObject(const long& val)
    {
        PythonObject pObject = PyLong_FromLong(val);
        if (!pObject)
            throw PythonError(); // Maybe create badCastError or similar
        return pObject;
    }

    template<>
    [[nodiscard]] PythonObject ToPythonObject(const unsigned long& val)
    {
        PythonObject pObject = PyLong_FromUnsignedLong(val);
        if (!pObject)
            throw PythonError();
        return pObject;
    }

    template<>
    [[nodiscard]] PythonObject ToPythonObject(const long long& val)
    {
        PythonObject pObject = PyLong_FromLongLong(val);
        if (!pObject)
            throw PythonError();
        return pObject;
    }

    template<>
    [[nodiscard]] PythonObject ToPythonObject(const unsigned long long& val)
    {
        PythonObject pObject = PyLong_FromUnsignedLongLong(val);
        if (!pObject)
            throw PythonError();
        return pObject;
    }

    template<>
    [[nodiscard]] PythonObject ToPythonObject(const bool& val)
    {
        PythonObject pObject = PyBool_FromLong(val ? static_cast<long>(1) : static_cast<long>(0));
        if (!pObject)
            throw PythonError();
        return pObject;
    }

    template<>
    [[nodiscard]] PythonObject ToPythonObject(const double& val)
    {
        PythonObject pObject = PyFloat_FromDouble(val);
        if (!pObject)
            throw PythonError();
        return pObject;
    }

    // python_cast converts the contents of a PythonObject to your desired type, if possible.
    // for example a conversion to long will succeed, if the Python object pointed to is of type int or has
    // __int__() implemented (can be converted to int). This cast will also check for possible overflow
    // note: Python C API allows for conversion from int to double etc. These will not be supported.
    // Please cast them accordingly and if you need conversions cast them manually
    template<typename T> // base template, this will not do anything except warning about wrong types
    [[nodiscard]] T python_cast(const PythonObject& pyObj)
    {
        static_assert(isPythonBaseType_v<T>, "python_cast: Type not supported");
        return T{};
    }

    template<>
    [[nodiscard]] bool python_cast<bool>(const PythonObject& pyObj)
    {
        const auto check = PyObject_IsTrue(pyObj.get());
        if (check == -1)
            throw PythonError();
        if (check == 1)
            return true;
        return false;
    }

    template<>
    [[nodiscard]] long python_cast<long>(const PythonObject& pyObj)
    {
        const auto ret = PyLong_AsLong(pyObj.get());
        if (PyErr_Occurred())
            throw PythonError();

        return ret;
    }

    template<>
    [[nodiscard]] unsigned long python_cast<unsigned long>(const PythonObject& pyObj)
    {
        const auto ret = PyLong_AsUnsignedLong(pyObj.get());
        if (PyErr_Occurred())
            throw PythonError();

        return ret;
    }

    template<>
    [[nodiscard]] long long python_cast<long long>(const PythonObject& pyObj)
    {
        const auto ret = PyLong_AsLongLong(pyObj.get());
        if (PyErr_Occurred())
            throw PythonError();

        return ret;
    }

    template<>
    [[nodiscard]] unsigned long long python_cast<unsigned long long>(const PythonObject& pyObj)
    {
        const auto ret = PyLong_AsUnsignedLongLong(pyObj.get());
        if (PyErr_Occurred())
            throw PythonError();

        return ret;
    }

    template<>
    [[nodiscard]] double python_cast<double>(const PythonObject& pyObj)
    {
        const auto ret = PyFloat_AsDouble(pyObj.get());
        if (PyErr_Occurred())
            throw PythonError();

        return ret;
    }


    // TODO: implement unchecked_python_cast and forced_python_cast
}

#endif // PYTHON_TYPE_TRAITS_H