/**
 * \file src/midout.h
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2020 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#define _MIDOUT_NR_PARAMS_GET(_1, _2, _3, _4, _5, _6, _7, _8, _n, ...) _n
//! get number macro params
#define MIDOUT_NR_PARAMS(...) \
    _MIDOUT_NR_PARAMS_GET(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
#define _MIDOUT_MAP1(f, _1) f(_1)
#define _MIDOUT_MAP2(f, _1, _2) f(_1), f(_2)
#define _MIDOUT_MAP3(f, _1, _2, _3) f(_1), f(_2), f(_3)
#define _MIDOUT_MAP4(f, _1, _2, _3, _4) f(_1), f(_2), f(_3), f(_4)
#define _MIDOUT_MAP5(f, _1, _2, _3, _4, _5) \
    f(_1), f(_2), f(_3), f(_4), f(_5)
#define _MIDOUT_MAP6(f, _1, _2, _3, _4, _5, _6) \
    f(_1), f(_2), f(_3), f(_4), f(_5), f(_6)
#define _MIDOUT_MAP7(f, _1, _2, _3, _4, _5, _6, _7) \
    f(_1), f(_2), f(_3), f(_4), f(_5), f(_6), f(_7)
#define _MIDOUT_MAP8(f, _1, _2, _3, _4, _5, _6, _7, _8) \
    f(_1), f(_2), f(_3), f(_4), f(_5), f(_6), f(_7), f(_8)
#define _MIDOUT_MAP_CALL1(f, n, ...) _MIDOUT_MAP##n(f, __VA_ARGS__)
#define _MIDOUT_MAP_CALL(f, n, ...) _MIDOUT_MAP_CALL1(f, n, __VA_ARGS__)
//! apply macro function \p f to given list
#define MIDOUT_MAP(f, ...) \
    _MIDOUT_MAP_CALL(f, MIDOUT_NR_PARAMS(__VA_ARGS__), __VA_ARGS__)

#if defined(MIDOUT_GENERATED) || defined(MIDOUT_PROFILING)

#include <type_traits>
#include <typeinfo>

namespace midout {
    namespace details {
        //! used as extra params to MIDOUT_BEGIN to wrap int value as type
        template<int val>
        using iv = std::integral_constant<int, val>;

        template<class T>
        struct type_wrapper {
            using type = T;
        };

        //! convert type and non-type template params to type
        template<class T> type_wrapper<T> as_type() { return {}; }
        template<int I> type_wrapper<iv<I>> as_type() { return {}; }
    }

//! cast compile-time constant value to type
#define midout_iv(_val) ::midout::details::iv<static_cast<int>(_val)>

#define _midout_astype(x) \
    typename decltype(::midout::details::as_type<x>())::type
#define _midout_tparams(...) MIDOUT_MAP(_midout_astype, __VA_ARGS__)
}

/*!
 * \brief declare a tag that can be used in MIDOUT_BEGIN
 *
 * This macro must be called from the global scope
 *
 * \param tag tag name, which must be a valid C++ identifier
 */
#define MIDOUT_DECL(tag) \
namespace midout { \
namespace tags { \
    class tag; \
} \
static_assert(std::is_same<::midout::tags::tag, tags::tag>::value, \
        "MIDOUT_DECL must be called from global scope"); \
}

#endif  // MIDOUT_GENERATED || MIDOUT_PROFILING


#ifndef MIDOUT_GENERATED

#ifdef MIDOUT_PROFILING

namespace midout {
    void on_region_used(const std::type_info &type);

    template<class Tag, typename ...Args>
    struct Region {
        static void on_used() {
            on_region_used(typeid(Region));
        }
    };

}

/*!
 * \brief mark a code block to be removed if not executed during profiling
 * \param tag tag name that has been declared via MIDOUT_DECL
 *
 * Variadic args (each can either be an int or a class) can be passed to
 * disambiguate different regions in the same tag
 *
 * The code block must be enclosed by a pair of braces and ended by MIDOUT_END
 */
#define MIDOUT_BEGIN(tag, ...)  \
    do { \
        ::midout::Region< \
            _midout_tparams(::midout::tags::tag, ## __VA_ARGS__)>::on_used(); \
        if (1)

#define MIDOUT_END() \
    } while(0)

#else   // MIDOUT_PROFILING

#define MIDOUT_DECL(tag)
#define MIDOUT_BEGIN(tag, ...) do
#define MIDOUT_END() while(0)

#endif  // MIDOUT_PROFILING

#else   // MIDOUT_GENERATED

namespace midout {
    template<class Tag, typename ...Args>
    struct Region {
        static constexpr bool enable = false;
    };

}

MIDOUT_GENERATED

#define MIDOUT_BEGIN(tag, ...)  \
    do { \
        if (::midout::Region< \
                _midout_tparams(::midout::tags::tag, ## __VA_ARGS__)>::enable)

#define MIDOUT_END() \
        else { \
            __builtin_trap(); \
        } \
    } while(0)

#endif  // MIDOUT_GENERATED

// vim: syntax=cpp.doxygen foldmethod=marker foldmarker=f{{{,f}}}
