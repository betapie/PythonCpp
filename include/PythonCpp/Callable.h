#pragma once
#include "Utilities.h"

namespace pycpp
{
    template<typename... Args>
    Object BuildValue(const char* format, Args... args)
    {
        Object ret = Py_BuildValue(format, args...);
        if (!ret)
            throw Error();
        return ret;
    }

    PYCPP_API Object CallObject(PyObject* pCallableObject, PyObject* pArglist);
    PYCPP_API Object CallObject(const Object& callableObject, PyObject* pArglist);
    PYCPP_API Object CallObject(PyObject* pCallableObject, const Object& arglist);
    PYCPP_API Object CallObject(const Object& callableObject, const Object& arglist);

    namespace detail
    {
        template<typename T, typename U = void>
        struct ArgRepr
        {
            
        };

        // TODO special case for bool

        template<typename CharT>
        struct ArgRepr<std::basic_string<CharT>>
        {
            constexpr static auto value = ArgRepr<const CharT*>::value;
        };

        template<>
        struct ArgRepr<const char*>
        {
            constexpr static auto value = 's';
        };

        template<>
        struct ArgRepr<const wchar_t*>
        {
            constexpr static auto value = 'u';
        };

        template<>
        struct ArgRepr<int>
        {
            constexpr static auto value = 'i';
        };

        template<>
        struct ArgRepr<char>
        {
            constexpr static auto value = 'b';
        };

        template<>
        struct ArgRepr<short>
        {
            constexpr static auto value = 'h';
        };

        template<>
        struct ArgRepr<long>
        {
            constexpr static auto value = 'l';
        };

        template<>
        struct ArgRepr<unsigned int>
        {
            constexpr static auto value = 'I';
        };

        template<>
        struct ArgRepr<unsigned char>
        {
            constexpr static auto value = 'B';
        };

        template<>
        struct ArgRepr<unsigned short>
        {
            constexpr static auto value = 'H';
        };

        template<>
        struct ArgRepr<unsigned long>
        {
            constexpr static auto value = 'k';
        };

        template<>
        struct ArgRepr<long long>
        {
            constexpr static auto value = 'L';
        };

        template<>
        struct ArgRepr<unsigned long long>
        {
            constexpr static auto value = 'K';
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
        struct ArgRepr<Object>
        {
            constexpr static auto value = 'O';
        };

        template<typename T>
        struct ArgRepr<T, typename std::enable_if_t<std::is_base_of_v<Object, T>>>
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
            constexpr static char value[] = { '(', ArgRepr<Args>::value ..., ')', '\0' };
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
        struct BuildType : BuildTypeBase<T, std::is_base_of_v<Object, T>>
        {};

        template<typename CharT>
        struct BuildType<std::basic_string<CharT>>
        {
            using type = const CharT*;
        };

        template<typename T, std::enable_if_t<!std::is_base_of_v<Object, T>, int> = 0>
        typename BuildType<T>::type PrepareForBuild(const T& value)
        {
            return value;
        }

        template<typename T, std::enable_if_t<std::is_base_of_v<Object, T>, int> = 0>
        typename BuildType<T>::type PrepareForBuild(const T& pyObj)
        {
            return pyObj.get();
        }

        template<typename CharT>
        typename BuildType<std::basic_string<CharT>>::type PrepareForBuild(const std::basic_string<CharT>& str)
        {
            return str.c_str();
        }
        

        template<typename... Args>
        Object BuildArgList(const Args&... args)
        {
            return BuildValue(ArgFormatString<Args...>::value, PrepareForBuild(args)...);
        }
    }

    /* Callable represents a highlevel abstraction to any Object which is callable
    *  meaning that callable(object) in Python evaluates to True. A Callable can not be
    *  constructed from a Object or PyObject* which does not satisfy that requirement.
    *  A Error will be thrown in that instance.
    */

    class PYCPP_API Callable : public Object
    {
    public:
        Callable() noexcept = default;

        // Take ownership of an existing PyObject which points to a callable Python Object
        // Will throw Error if object pointed to by PyObject* is not callable
        Callable(PyObject* pCallableObject);

        Callable(const Callable& other);

        Callable& operator=(const Callable& other);

        Callable(Callable&& other) noexcept;

        Callable& operator=(Callable&& other) noexcept;

        Callable(const Object& other);

        Callable& operator=(const Object& other);

        // Note: No move construction/assignment from Object, because due to the PyCallable_Check
        // they cannot be defined noexcept!

        // Args can be of type Object or of any type which is convertible to Object
        template<typename... Args>
        Object Invoke(const Args&... args) const
        {
            return CallObject(m_pObject, detail::BuildArgList(args...));
        }

        template<typename... Args>
        Object operator()(const Args&... args) const
        {
            return Invoke(args...);
        }
    private:

    };

    // Shortcut function for calling a function or method which is owned by owningObject with args functionArgs
    template<typename... Args>
    Object CallFunction(const Object& owningObject, const char* functionName, const Args&... functionArgs)
    {
        const Callable methodObj = GetAttributeString(owningObject, functionName);

        return methodObj(functionArgs...);
    }

    template<typename... Args>
    Object CallFunction(const Object& owningObject, const std::string& functionName, const Args&... functionArgs)
    {
        return CallFunction(owningObject, functionName.c_str(), functionArgs...);
    }
}
