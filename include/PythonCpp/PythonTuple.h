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
    template<typename... Ts>
    class PythonTuple;

    namespace detail
    {
        template<typename tuple_t, size_t... idx>
        constexpr auto PyTupleUnpack(tuple_t tuple, std::index_sequence<idx...>)
        {
            return PyTuple_Pack(std::tuple_size<tuple_t>::value, ToPythonObject(std::get<idx>(tuple)).get()...);
        }

        // Helper for deducing slice types:
        template<size_t... idx>
        struct Seq
        {
            using type = Seq;
        };

        template<typename seq1, typename seq2>
        struct ConcatSeqs;

        template<size_t... idx1, size_t... idx2>
        struct ConcatSeqs<Seq<idx1...>, Seq<idx2...>>
        {
            using type = Seq<idx1..., idx2...>;
        };

        template<size_t low, size_t len>
        struct MakeSliceImpl;

        template<size_t low, size_t len>
        struct MakeSliceHelper
        {
            using type = typename MakeSliceImpl<low, len>::type;
        };

        template<size_t low, size_t len>
        struct MakeSliceImpl
        {
            using type = typename ConcatSeqs<
                typename MakeSliceHelper<low, len / 2>::type,
                typename MakeSliceHelper<low + len / 2, len - len / 2>::type>::type;
        };

        template<size_t low>
        struct MakeSliceImpl<low, 0>
        {
            using type = Seq<>;
        };

        template<size_t low>
        struct MakeSliceImpl<low, 1>
        {
            using type = Seq<low>;
        };

        template<size_t low, size_t high>
        struct MakeSlice
        {
            using type = typename MakeSliceHelper<low, high - low>::type;
        };

        template<typename tuple_t, typename seq>
        struct TupleSliceImpl
        {};

        template<typename tuple_t, size_t... idx>
        struct TupleSliceImpl<tuple_t, Seq<idx...>>
        {
            using type = typename PythonTuple<std::tuple_element_t<idx, tuple_t>...>;
        };

        template<typename tuple_t, size_t low, size_t high>
        struct TupleSlice
        {
            using type = typename TupleSliceImpl<tuple_t, typename MakeSlice<low, high>::type>::type;
        };
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
            if (PyTuple_Size(pTupleObj) != sizeof...(Ts))
                throw PythonError("Size mismatch in PythonTuple");
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

        [[nodiscard]] constexpr size_t size() const noexcept
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

        template<size_t lowIdx, size_t highIdx>
        typename detail::TupleSlice<std::tuple<Ts...>, lowIdx, highIdx>::type slice()
        {
            auto pSlice = PyTuple_GetSlice(m_pObject, lowIdx, highIdx);
            if (!pSlice)
                throw PythonError();
            return detail::TupleSlice<std::tuple<Ts...>, lowIdx, highIdx>::type(pSlice);
        }

        std::tuple<Ts...> ToStdTuple()
        {
            return _ToStdTupleImpl(std::make_index_sequence<sizeof...(Ts)>());
        }

    private:
        template<size_t... idx>
        std::tuple<Ts...> _ToStdTupleImpl(std::index_sequence<idx...>)
        {
            return std::make_tuple(at<idx>()...);
        }
    };

    template<typename... Ts>
    PythonTuple(const Ts&...)->PythonTuple<Ts...>;

    template<typename... Ts>
    PythonTuple(const std::tuple<Ts...>&)->PythonTuple<Ts...>;
}


#endif // !PYTHON_TUPLE_H

