/*
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef ENGINEERING_UNITS_UNIT_HELPER_MACROS_HPP
#define ENGINEERING_UNITS_UNIT_HELPER_MACROS_HPP

#include <ratio>

#include <engineering_units/quantity.hpp>

#include <engineering_units/unit/traits.hpp>

#include <engineering_units/detail/string_literal.hpp>

/** @file */

namespace engunits
{

#define ENGUNITS_DEFINE_UDL(name, symbol)                 \
    constexpr auto operator "" _##symbol(long double x)   \
    {                                                     \
        return quantity<long double, name>(x);            \
    }

/**
 * @def ENGUNITS_DEFINE_ROOT_UNIT(name, symbol, dimension)
 * @brief Define a new root unit.
 * @param name The name of the unit.
 * @param sym Symbol used for printing. 
 *          See @ref engunits::unit_traits::symbol "unit_traits::symbol".
 * @param dimension Name of the associated dimension.
 * 
 * A root unit is a base unit with unit exponent, which is also the
 * main unit of its own physical quantity, used internally for conversions.
 * 
 * This macro will generate:
 * 
 *   - An empty @p dimension type, used to tag the newly defined dimension.
 * 
 *   - A class template @c name_, with two integral template parameters 
 *     @c Num and @c Den. All the instances of this class model the @c BaseUnit 
 *     concept.
 *     @ref engunits::base_unit_tag "base_unit_tag"
 * 
 *   - An alias @p name for @p name_<1> 
 * 
 */
#define ENGUNITS_DEFINE_ROOT_UNIT(name, sym, dimension)             \
    struct dimension{};                                             \
    template<std::intmax_t Num, std::intmax_t Den = 1>              \
    struct name##_                                                  \
    {                                                               \
        typedef engunits::base_unit_tag unit_category;              \
        static constexpr auto symbol()                              \
        {                                                           \
            return engunits::detail::make_string_literal(#sym);     \
        }                                                           \
        typedef dimension dimension_tag;                            \
    };                                                              \
    using name = name##_<1>;                                        \



#define ENGUNITS_DEFINE_BASE_UNIT(name, sym, parent, conv_factor)    \
    template<std::intmax_t Num, std::intmax_t Den = 1>               \
    struct name##_                                                   \
    {                                                                \
        typedef engunits::base_unit_tag unit_category;               \
        static constexpr auto symbol()                               \
        {                                                            \
            return engunits::detail::make_string_literal(#sym);      \
        }                                                            \
        typedef parent parent_unit;                                  \
        typedef parent::dimension_tag dimension_tag;                 \
        static constexpr long double to_parent = conv_factor;        \
    };                                                               \
    using name = name##_<1>

#define ENGUNITS_DEFINE_DERIVED_UNIT(name, sym, ...)                \
    template<std::intmax_t Num, std::intmax_t Den = 1>              \
    struct name##_                                                  \
    {                                                               \
        typedef engunits::derived_unit_tag unit_category;           \
        static constexpr auto symbol()                              \
        {                                                           \
            return engunits::detail::make_string_literal(#sym);     \
        }                                                           \
        static constexpr auto flat()                                \
        {                                                           \
            using mixed = engunits::mixed_unit<__VA_ARGS__>;        \
            using mixed_p = engunits::unit_traits< mixed >::template\
                base_<Num, Den>;                                    \
            return engunits::unit_traits< mixed_p >::flat();        \
        }                                                           \
    };                                                              \
    using name = name##_<1> 

    
#define ENGUNITS_IMPORT_OPERATORS                       \
    using engunits::operator==;                         \
    using engunits::operator!=;                         \
    using engunits::operator*;                          \
    
}

#endif //ENGINEERING_UNITS_UNIT_HELPER_MACROS_HPP
