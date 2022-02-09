#include "PythonCpp.h"
#include <gtest/gtest.h>

#define PYTHON_BASE_TYPE_LIST int,                  \
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
    void PythonBaseTypeTest()
    {
        EXPECT_TRUE(pycpp::isPythonBaseType_v<T>);
        if constexpr (sizeof...(Rest) > 0)
            PythonBaseTypeTest<Rest...>();
    }
}

TEST(PythonTypeTraitsTests, BaseTypeTests)
{
    detail::PythonBaseTypeTest<PYTHON_BASE_TYPE_LIST>();
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
    detail::PythonListTypeTest<PYTHON_BASE_TYPE_LIST>();
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
    constexpr auto testFullList = pycpp::isPythonBaseType_v<pycpp::PythonTuple<PYTHON_BASE_TYPE_LIST>>;
    EXPECT_TRUE(testFullList);
    detail::PythonTuplePairwiseTypeTest<PYTHON_BASE_TYPE_LIST>();
}