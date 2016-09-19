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

#ifndef ENGINEERING_UNITS_UNIT_PREDICATES_HPP
#define ENGINEERING_UNITS_UNIT_PREDICATES_HPP

#include <engineering_units/unit/traits.hpp>
#include <engineering_units/detail/doxygen.hpp>

namespace engunits
{
    
/**
 * @addtogroup metafunctions
 * @{
 */

/**
 * @brief Check if two units have the same base
 * 
 * Metafunction that returns true if @p Lhs and @p Rhs have the same base.
 * 
 * For instance meter and meter squared have the same base (meter), while
 * seconds and kilograms do not.
 * 
 * @note @p Lhs and @p Rhs must model the @ref Unit concept.
 */
template<class Lhs, class Rhs>
using is_same_base = 
    ENGUNITS_UNSPECIFIED(
        typename std::is_same<
            typename unit_traits<Lhs>::base,
            typename unit_traits<Rhs>::base
        >
    );

template<class Lhs, class Rhs>
constexpr bool is_same_base_v = is_same_base<Lhs, Rhs>::value;


#ifdef ENGUNITS_DOXYGEN

/**
 * @brief Check if @p T is a unit
 * @note This simply inspects @p T for a member typedef @c unit_tag
 */    
template<class T>
struct is_unit;

#else

template<class T, class = void>
struct is_unit : std::false_type {};

template<class T>
struct is_unit<T, detail::void_t< typename unit_traits<T>::unit_category > > :
    std::true_type
{};
    
template<class T>
constexpr bool is_unit_v = is_unit<T>::value;

#endif // ENGUNITS_DOXYGEN

}


#endif //ENGINEERING_UNITS_UNIT_PREDICATES_HPP
