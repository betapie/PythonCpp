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

        PythonTuple(const std::tuple<Ts...>& tuple)
        {
            m_pObject = detail::PyTupleUnpack(tuple, std::make_index_sequence<sizeof...(Ts)>());
            if (!m_pObject)
                throw PythonError();
        }

        PythonTuple(PyObject* pTupleObj)
            :PythonObject(pTupleObj)
        {
            if (PyTuple_Check(pTupleObj) == 0)
                throw PythonError("Pyobject not of Tuple type");
        }

        PythonTuple(const PythonTuple& other)
            :PythonObject(other)
        {}

        PythonTuple& operator=(const PythonTuple& other)
        {
            PythonObject::operator=(other);
            return *this;
        }

        PythonTuple(PythonTuple&& other) noexcept
            :PythonObject(std::move(other))
        {}

        PythonTuple& operator=(PythonTuple&& other) noexcept
        {
            PythonObject::operator=(std::move(other));
            return *this;
        }

        PythonTuple(const PythonObject& other)
            :PythonObject(other)
        {
            if (PyTuple_Check(m_pObject) == 0)
                throw PythonError("Pyobject not of Tuple type");
        }

        PythonTuple& operator=(const PythonObject& other)
        {
            PythonObject::operator=(other);
            if (PyTuple_Check(m_pObject) == 0)
                throw PythonError("Pyobject not of Tuple type");
        }

        // Note: No move construction/assignment from PythonObject, because due to the PyTuple_Check
        // they cannot be defined noexcept!

        [[nodiscard]] size_t size() const noexcept
        {
            return sizeof...(Ts);
        }

        template<size_t idx>
        typename std::tuple_element_t<idx, std::tuple<Ts...>> at() const
        {
            auto pItem = PyTuple_GetItem(m_pObject, idx);
            if (!pItem)
                throw PythonError();
            return python_cast<typename std::tuple_element_t<idx, std::tuple<Ts...>>>(pItem);
        }

    private:
        

    };

    template<typename... Ts>
    PythonTuple(const Ts&...)->PythonTuple<Ts...>;

    template<typename... Ts>
    PythonTuple(const std::tuple<Ts...>&)->PythonTuple<Ts...>;
}


#endif // !PYTHON_TUPLE_H

