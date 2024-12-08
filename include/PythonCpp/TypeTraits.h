#pragma once

#ifndef PYCPP_TYPE_TRAITS_H
#define PYCPP_TYPE_TRAITS_H

#include <PythonCpp/Object.h>
#include <PythonCpp/Error.h>
#include <type_traits>
#include <complex>

namespace pycpp
{
    template<typename T>
    struct isPythonBaseType : std::false_type
    {};

#ifndef Py_LIMITED_API
    template<>
    struct isPythonBaseType<int> : std::true_type
    {};
#endif //Py_LIMITED_API

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

    // C API only allows initialization from and conversion to double
    // so that is the only type that is supported
    template<>
    struct isPythonBaseType<std::complex<double>> : std::true_type
    {};

    template<>
    struct isPythonBaseType<const char*> : std::true_type
    {};

    template<>
    struct isPythonBaseType<std::string> : std::true_type
    {};

    // using generic Objects should also be allowed but can be dangerous
    template<>
    struct isPythonBaseType<Object> : std::true_type
    {};

    template<typename T>
    class List;

    template<typename U>
    struct isPythonBaseType<List<U>> : std::conditional_t<isPythonBaseType<U>::value, std::true_type, std::false_type>
    {};

    template<typename... Ts>
    class Tuple;

    template<typename... Ts>
    struct isPythonBaseType<Tuple<Ts...>> : std::conjunction<isPythonBaseType<Ts>...>
    {};

    template<typename T>
    constexpr auto isPythonBaseType_v = isPythonBaseType<T>::value;

    // base template, this will not do anything except warning about wrong types
    template<typename T, std::enable_if_t<!std::is_base_of_v<Object, T>, int> = 0>
    [[nodiscard]] Object ToObject(const T&)
    {
        static_assert(isPythonBaseType_v<T>, "ToObject: Type not supported");
        throw Error("How did we even get here?");
    }

    // note: no implementations for PyLong_FromDouble... and other implicit conversions. If you want your Object
    // to be of a certain type, be explicit about the type you pass in

#ifndef Py_LIMITED_API
    template<>
    [[nodiscard]] Object ToObject(const int& val)
    {
        Object pObject = PyLong_FromLong(val);
        if (!pObject)
            throw Error();
        return pObject;
    }
#endif //Py_LIMITED_API

    template<>
    [[nodiscard]] Object ToObject(const long& val)
    {
        Object pObject = PyLong_FromLong(val);
        if (!pObject)
            throw Error(); // Maybe create badCastError or similar
        return pObject;
    }

    template<>
    [[nodiscard]] Object ToObject(const unsigned long& val)
    {
        Object pObject = PyLong_FromUnsignedLong(val);
        if (!pObject)
            throw Error();
        return pObject;
    }

    template<>
    [[nodiscard]] Object ToObject(const long long& val)
    {
        Object pObject = PyLong_FromLongLong(val);
        if (!pObject)
            throw Error();
        return pObject;
    }

    template<>
    [[nodiscard]] Object ToObject(const unsigned long long& val)
    {
        Object pObject = PyLong_FromUnsignedLongLong(val);
        if (!pObject)
            throw Error();
        return pObject;
    }

    template<>
    [[nodiscard]] Object ToObject(const bool& val)
    {
        Object pObject = PyBool_FromLong(val ? static_cast<long>(1) : static_cast<long>(0));
        if (!pObject)
            throw Error();
        return pObject;
    }

    template<>
    [[nodiscard]] Object ToObject(const double& val)
    {
        Object pObject = PyFloat_FromDouble(val);
        if (!pObject)
            throw Error();
        return pObject;
    }

    template<>
    [[nodiscard]] Object ToObject(const std::complex<double>& val)
    {
        Object pObject = PyComplex_FromDoubles(val.real(), val.imag());
        if (!pObject)
            throw Error();
        return pObject;
    }

    [[nodiscard]] Object ToObject(const char* str)
    {
        Object pObject = PyUnicode_FromString(str);
        if (!pObject)
            throw Error();
        return pObject;
    }

    template<>
    [[nodiscard]] Object ToObject(const std::string& str)
    {
        Object pObject = PyUnicode_FromString(str.c_str());
        if (!pObject)
            throw Error();
        return pObject;
    }

    template<typename T, std::enable_if_t<std::is_base_of_v<Object, T>, int> = 0>
    [[nodiscard]] constexpr T ToObject(const T& val)
    {
        return T(val);
    }

    // python_cast converts the contents of a Object to your desired type, if possible.
    // for example a conversion to long will succeed, if the Python object pointed to is of type int or has
    // __int__() implemented (can be converted to int). This cast will also check for possible overflow
    // note: Python C API allows for conversion from int to double etc. These will not be supported.
    // Please cast them accordingly and if you need conversions cast them manually

    template<typename T, std::enable_if_t<!std::is_base_of_v<Object, T>, int> = 0> // base template, this will not do anything except warning about wrong types
    [[nodiscard]] T python_cast(const Object&)
    {
        static_assert(isPythonBaseType_v<T>, "python_cast: Type not supported");
        return T{};
    }

    template<typename T, std::enable_if_t<std::is_base_of_v<Object, T>, int> = 0>
    [[nodiscard]] T python_cast(const Object& pyObj)
    {
        return T{ pyObj };
    }

    template<>
    [[nodiscard]] bool python_cast<bool>(const Object& pyObj)
    {
        const auto check = PyObject_IsTrue(pyObj.get());
        if (check == -1)
            throw Error();
        if (check == 1)
            return true;
        return false;
    }

#ifndef Py_LIMITED_API
    template<>
    [[nodiscard]] int python_cast<int>(const Object& pyObj)
    {
        const auto ret = _PyLong_AsInt(pyObj.get());
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }
#endif //Py_LIMITED_API

    template<>
    [[nodiscard]] long python_cast<long>(const Object& pyObj)
    {
        const auto ret = PyLong_AsLong(pyObj.get());
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] unsigned long python_cast<unsigned long>(const Object& pyObj)
    {
        const auto ret = PyLong_AsUnsignedLong(pyObj.get());
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] long long python_cast<long long>(const Object& pyObj)
    {
        const auto ret = PyLong_AsLongLong(pyObj.get());
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] unsigned long long python_cast<unsigned long long>(const Object& pyObj)
    {
        const auto ret = PyLong_AsUnsignedLongLong(pyObj.get());
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] double python_cast<double>(const Object& pyObj)
    {
        const auto ret = PyFloat_AsDouble(pyObj.get());
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] std::complex<double> python_cast<std::complex<double>>(const Object& pyObj)
    {
        const auto real = PyComplex_RealAsDouble(pyObj.get());
        if (PyErr_Occurred())
            throw Error();
        const auto imag = PyComplex_ImagAsDouble(pyObj.get());
        if (PyErr_Occurred())
            throw Error();

        return { real, imag };
    }

    template<>
    [[nodiscard]] const char* python_cast<const char*>(const Object& pyObj)
    {
        auto pData = PyUnicode_AsUTF8(pyObj.get());
        if (!pData)
            throw Error();
        return pData;
    }

    template<>
    [[nodiscard]] std::string python_cast<std::string>(const Object& pyObj)
    {
        auto pData = PyUnicode_AsUTF8(pyObj.get());
        if (!pData)
            throw Error();
        return std::string(pData);
    }

    // base template, this will not do anything except warning about wrong types
    template<typename T, std::enable_if_t<!std::is_base_of_v<Object, T>, int> = 0>
    [[nodiscard]] T python_cast(PyObject*)
    {
        static_assert(isPythonBaseType_v<T>, "python_cast: Type not supported");
        return T{};
    }

    template<typename T, std::enable_if_t<std::is_base_of_v<Object, T>, int> = 0>
    [[nodiscard]] T python_cast(PyObject* pPyObj)
    {
        Py_INCREF(pPyObj);
        return T(pPyObj);
    }

    template<>
    [[nodiscard]] bool python_cast<bool>(PyObject* pPyObj)
    {
        const auto check = PyObject_IsTrue(pPyObj);
        if (check == -1)
            throw Error();
        if (check == 1)
            return true;
        return false;
    }

#ifndef Py_LIMITED_API
    template<>
    [[nodiscard]] int python_cast<int>(PyObject* pPyObj)
    {
        const auto ret = _PyLong_AsInt(pPyObj);
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }
#endif //Py_LIMITED_API

    template<>
    [[nodiscard]] long python_cast<long>(PyObject* pPyObj)
    {
        const auto ret = PyLong_AsLong(pPyObj);
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] unsigned long python_cast<unsigned long>(PyObject* pPyObj)
    {
        const auto ret = PyLong_AsUnsignedLong(pPyObj);
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] long long python_cast<long long>(PyObject* pPyObj)
    {
        const auto ret = PyLong_AsLongLong(pPyObj);
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] unsigned long long python_cast<unsigned long long>(PyObject* pPyObj)
    {
        const auto ret = PyLong_AsUnsignedLongLong(pPyObj);
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] double python_cast<double>(PyObject* pPyObj)
    {
        const auto ret = PyFloat_AsDouble(pPyObj);
        if (PyErr_Occurred())
            throw Error();

        return ret;
    }

    template<>
    [[nodiscard]] std::complex<double> python_cast<std::complex<double>>(PyObject* pPyObj)
    {
        const auto real = PyComplex_RealAsDouble(pPyObj);
        if (!real)
            throw Error();
        const auto imag = PyComplex_ImagAsDouble(pPyObj);
        if (!imag)
            throw Error();

        return { real, imag };
    }

    template<>
    [[nodiscard]] const char* python_cast<const char*>(PyObject* pPyObj)
    {
        const auto pData = PyUnicode_AsUTF8(pPyObj);
        if (!pData)
            throw Error();
        return pData;
    }

    template<>
    [[nodiscard]] std::string python_cast<std::string>(PyObject* pPyObj)
    {
        auto pData = PyUnicode_AsUTF8(pPyObj);
        if (!pData)
            throw Error();
        return std::string(pData);
    }

   /* template<typename T, std::enable_if_t<isPythonBaseType_v<T>, int> = 0>
    [[nodiscard]] List<T> python_cast<List<T>>(PyObject* pPyObj)
    {
        return List<T>(pPyObj);
    }*/

    // TODO: implement unchecked_python_cast and forced_python_cast
}

#endif // PYTHON_TYPE_TRAITS_H
