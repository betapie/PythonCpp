#pragma once
#ifndef PYTHON_TUPLE_H
#define PYTHON_TUPLE_H

#include <type_traits>
#include <tuple>
#include "PythonTypeTraits.h"
#include "PythonObject.h"
#include "PythonError.h"


namespace pycpp
{
    namespace detail
    {
        template<typename tuple_t, size_t... idx>
        constexpr auto PyTupleUnpack(tuple_t tuple, std::index_sequence<idx...>)
        {
            return PyTuple_Pack(std::tuple_size<tuple_t>::value, ToPythonObject(std::get<idx>(tuple)).get()...);
        }
    }

    template<typename... Ts>
    class PythonTuple : public PythonObject
    {
        static_assert(std::conjunction_v<isPythonBaseType<Ts>...>
            , "PythonTuple<Ts...>: Not all types of Ts... are valid PythonBaseType");

    public:
        PythonTuple(const Ts&... vals)
        {
            m_pObject = PyTuple_Pack(sizeof...(vals), ToPythonObject(vals).get()...);
            if (!m_pObject)
                throw PythonError();
        }

        //template<typename... Ts>
        PythonTuple(const std::tuple<Ts...>& tuple)
        {
            m_pObject = detail::PyTupleUnpack(tuple, std::make_index_sequence<sizeof...(Ts)>());
            if (!m_pObject)
                throw PythonError();
        }

    private:
        

    };

    template<typename... Ts>
    PythonTuple(const Ts&...)->PythonTuple<Ts...>;

    template<typename... Ts>
    PythonTuple(const std::tuple<Ts...>&)->PythonTuple<Ts...>;
}


#endif // !PYTHON_TUPLE_H

