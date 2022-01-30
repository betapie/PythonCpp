#include "PythonCpp.h"
#include <gtest/gtest.h>

#define PYTHON_BASE_TYPE_LIST \
    int,                      \
        bool,                 \
        long,                 \
        unsigned long,        \
        long long,            \
        unsigned long long,   \
        double,               \
        std::complex<double>, \
        const char *,         \
        std::string,          \
        pycpp::PythonObject

namespace detail
{
    template <typename T, typename... Rest>
    void PythonBaseTypeTestImpl()
    {
        EXPECT_TRUE(pycpp::isPythonBaseType_v<T>);
        if constexpr (sizeof...(Rest) > 0)
            PythonBaseTypeTestImpl<Rest...>();
    }

    template <typename... Ts>
    void PythonBaseTypeTest()
    {
        PythonBaseTypeTestImpl<Ts...>();
    }
}

TEST(PythonTypeTraitsTests, BaseTypeTests)
{
    detail::PythonBaseTypeTest<PYTHON_BASE_TYPE_LIST>();
}

namespace detail
{
    template <typename T, typename... Rest>
    void PythonListTypeTestImpl()
    {
        EXPECT_TRUE(pycpp::isPythonBaseType_v<pycpp::PythonList<T>>);
        if constexpr (sizeof...(Rest) > 0)
            PythonListTypeTestImpl<Rest...>();
    }

    template <typename... Ts>
    void PythonListTypeTest()
    {
        PythonListTypeTestImpl<Ts...>();
    }
}

TEST(PythonTypeTraitsTests, ListTypeTests)
{
    detail::PythonListTypeTest<PYTHON_BASE_TYPE_LIST>();
}