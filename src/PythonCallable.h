#pragma once
#include "PythonUtilities.h"

namespace pycpp
{
    namespace detail
    {
        template<typename T>
        struct ArgRepr
        {
            constexpr static auto value = 'O';
        };

        // TODO checkout type for bool

        template<>
        struct ArgRepr<int>
        {
            constexpr static auto value = 'i';
        };

        template<>
        struct ArgRepr<long>
        {
            constexpr static auto value = 'l';
        };

        template<>
        struct ArgRepr<float>
        {
            constexpr static auto value = 'f';
        };

        template<>
        struct ArgRepr<double>
        {
            constexpr static auto value = 'd';
        };

        template<>
        struct ArgRepr<PythonObject>
        {
            constexpr static auto value = 'O';
        };

        template<>
        struct ArgRepr<PyObject*>
        {
            constexpr static auto value = 'O';
        };

        template<typename... Args>
        struct ArgFormatString
        {
            constexpr static char value[] = { '(', ArgRepr<Args>::value ..., ')' };
        };

        template<typename T, bool isPyObj = false>
        struct BuildTypeBase
        {
            using type = const T&;
        };

        template<typename T>
        struct BuildTypeBase<T, true>
        {
            using type = PyObject*;
        };

        template<typename T>
        struct BuildType : BuildTypeBase<T, std::is_base_of_v<PythonObject, T>>
        {};

        template<typename T, std::enable_if_t<!std::is_base_of_v<PythonObject, T>, int> = 0>
        typename BuildType<T>::type PrepareForBuild(const T& value)
        {
            return value;
        }

        template<typename T, std::enable_if_t<std::is_base_of_v<PythonObject, T>, int> = 0>
        typename BuildType<T>::type PrepareForBuild(const T& pyObj)
        {
            return pyObj.get();
        }

        template<typename... Args>
        PythonObject BuildArgList(const Args&... args)
        {
            return BuildValue(ArgFormatString<Args...>::value, PrepareForBuild(args)...);
        }
    }

    /* PythonCallable represents a highlevel abstraction to any PythonObject which is callable
    *  meaning that callable(object) in Python evaluates to True. A PythonCallable can not be
    *  constructed from a PythonObject or PyObject* which does not satisfy that requirement.
    *  A PythonError will be thrown in that instance.
    */

    class PythonCallable : public PythonObject
    {
    public:
        PythonCallable() noexcept = default;

        // Take ownership of an existing PyObject which points to a callable Python Object
        // Will throw PythonError if object pointed to by PyObject* is not callable
        PythonCallable(PyObject* pCallableObject);

        PythonCallable(const PythonCallable& other);

        PythonCallable& operator=(const PythonCallable& other);

        PythonCallable(PythonCallable&& other) noexcept;

        PythonCallable& operator=(PythonCallable&& other) noexcept;

        PythonCallable(const PythonObject& other);

        PythonCallable& operator=(const PythonObject& other);

        // Note: No move construction/assignment from PythonObject, because due to the PyCallable_Check
        // they cannot be defined noexcept!

        // Args can be of type PythonObject or of any type which is convertible to PythonObject like long
        template<typename... Args>
        PythonObject Invoke(const Args&... args) const
        {
            return CallObject(m_pObject, detail::BuildArgList(args...));
        }

        template<typename... Args>
        PythonObject operator()(const Args&... args) const
        {
            return Invoke(args...);
        }
    private:

    };

    // Shortcut function for calling a function or method which is owned by owningObject with args functionArgs
    template<typename... Args>
    PythonObject CallFunction(const PythonObject& owningObject, const char* functionName, const Args&... functionArgs)
    {
        const PythonCallable methodObj = GetAttributeString(owningObject, functionName);

        return methodObj(functionArgs...);
    }

    template<typename... Args>
    PythonObject CallFunction(const PythonObject& owningObject, const std::string& functionName, const Args&... functionArgs)
    {
        return CallFunction(owningObject, functionName.c_str(), functionArgs...);
    }
}