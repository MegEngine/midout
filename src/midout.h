#pragma once

#if defined(MIDOUT_GENERATED) || defined(MIDOUT_PROFILING)

#include <type_traits>
#include <typeinfo>

namespace midout {
    //! used as extra params to MIDOUT_BEGIN to wrap int value as type
    template<int val>
    using iv = std::integral_constant<int, val>;

//! cast compile-time constant value to int
#define midout_iv(_val) ::midout::iv<static_cast<int>(_val)>

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
 * Variadic args (must be classes) can be passed to disambiguate different
 * regions in the same tag
 *
 * The code block must be enclosed by a pair of braces and ended by MIDOUT_END
 */
#define MIDOUT_BEGIN(tag, ...)  \
    do { \
        ::midout::Region<::midout::tags::tag, ## __VA_ARGS__>::on_used(); \
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
        if (::midout::Region<::midout::tags::tag, ## __VA_ARGS__>::enable)

#define MIDOUT_END() \
        else { \
            __builtin_trap(); \
        } \
    } while(0)

#endif  // MIDOUT_GENERATED

// vim: syntax=cpp.doxygen foldmethod=marker foldmarker=f{{{,f}}}
