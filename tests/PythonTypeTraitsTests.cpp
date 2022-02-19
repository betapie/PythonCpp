#include "PythonCpp.h"
#include <gtest/gtest.h>

// Types that we expect to succeed
#define PYTHON_BASE_TYPES int,                \
                          bool,               \
                          long,               \
                          unsigned long,      \
                          long long,          \
                          unsigned long long, \
                          double,             \
                          std::complex<double>

#define PYTHON_STRING_TYPES const char *, \
                            std::string

#define PYTHON_ALL_TYPES PYTHON_BASE_TYPES,   \
                         PYTHON_STRING_TYPES, \
                         pycpp::PythonObject

// Types that we expect to fail
#define NONE_PYTHON_BASE_TYPES short, \
                               char,  \
                               float, \
                               void

namespace detail
{
    template <typename T, typename... Rest>
    void PythonBaseTypeTest()
    {
        EXPECT_TRUE(pycpp::isPythonBaseType_v<T>);
        if constexpr (sizeof...(Rest) > 0)
            PythonBaseTypeTest<Rest...>();
    }
}

TEST(PythonTypeTraitsTests, BaseTypeTests)
{
    detail::PythonBaseTypeTest<PYTHON_ALL_TYPES>();
}

namespace detail
{
    template <typename T, typename... Rest>
    void PythonListTypeTest()
    {
        EXPECT_TRUE(pycpp::isPythonBaseType_v<pycpp::PythonList<T>>);
        if constexpr (sizeof...(Rest) > 0)
            PythonListTypeTest<Rest...>();
    }
}

TEST(PythonTypeTraitsTests, ListTypeTests)
{
    detail::PythonListTypeTest<PYTHON_ALL_TYPES>();
}

namespace detail
{
    template <typename T, typename U, typename... Rest>
    void PythonTuplePairwiseTypeTest()
    {
        // adjacent pair wise test, should be enough...
        constexpr auto test = pycpp::isPythonBaseType_v<pycpp::PythonTuple<T, U>>;
        EXPECT_TRUE(test);
        if constexpr (sizeof...(Rest) > 0)
            PythonTuplePairwiseTypeTest<U, Rest...>();
    }
}

TEST(PythonTypeTraitsTests, TupleTypeTests)
{
    constexpr auto testFullList = pycpp::isPythonBaseType_v<pycpp::PythonTuple<PYTHON_ALL_TYPES>>;
    EXPECT_TRUE(testFullList);
    detail::PythonTuplePairwiseTypeTest<PYTHON_ALL_TYPES>();
}

namespace detail
{
    template <typename T, typename... Rest>
    void PythonNoneBaseTypeTest()
    {
        EXPECT_FALSE(pycpp::isPythonBaseType_v<T>);
        if constexpr (sizeof...(Rest) > 0)
            PythonNoneBaseTypeTest<Rest...>();
    }
}

TEST(PythonTypeTraitsTests, NoneBaseTypeTests)
{
    detail::PythonNoneBaseTypeTest<NONE_PYTHON_BASE_TYPES>();
}

namespace detail
{
    template <typename T>
    void BasicConversionTestImpl()
    {
        auto value = T{};
        auto pyObj = pycpp::ToPythonObject(value);
        auto convValue = pycpp::python_cast<T>(pyObj);
        EXPECT_EQ(value, convValue);
    }

    template <>
    void BasicConversionTestImpl<const char *>()
    {
        auto *str = "";
        auto pyObj = pycpp::ToPythonObject(str);
        auto convStr = pycpp::python_cast<const char *>(pyObj);
        EXPECT_EQ(strcmp(str, convStr), 0);
    }

    template <>
    void BasicConversionTestImpl<std::string>()
    {
        std::string str = "";
        auto pyObj = pycpp::ToPythonObject(str);
        auto convStr = pycpp::python_cast<std::string>(pyObj);
        EXPECT_EQ(str, convStr);
    }

    template <typename T, typename... Rest>
    void BasicConversionTest()
    {
        BasicConversionTestImpl<T>();
        if constexpr (sizeof...(Rest) > 0)
            BasicConversionTest<Rest...>();
    }
}

TEST(PythonTypeTraitsTests, BasicConversionTests)
{
    auto handle = pycpp::PythonInterpreter::Handle();
    detail::BasicConversionTest<PYTHON_BASE_TYPES>();
}

TEST(PythonTypeTraitsTests, BasicStringConversionTests)
{
    auto handle = pycpp::PythonInterpreter::Handle();
    detail::BasicConversionTest<PYTHON_STRING_TYPES>();
}