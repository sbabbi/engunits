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

#ifndef ENGINEERING_UNITS_UNIT_EQUALITY_HPP
#define ENGINEERING_UNITS_UNIT_EQUALITY_HPP

#include <engineering_units/unit/traits.hpp>
#include <engineering_units/unit/mixed_unit.hpp>
#include <engineering_units/unit/multiply.hpp>
#include <engineering_units/unit/pow.hpp>

#include <engineering_units/detail/doxygen.hpp>


namespace engunits
{
    
namespace detail
{

template<class Lhs, class Rhs>
constexpr bool flat_equal( const Lhs &, 
                           const Rhs &)
{
    static_assert( is_unit_v<Lhs> && is_unit_v<Rhs>, "");
    
    return is_same_base_v<Lhs, Rhs> &&
           std::ratio_equal< 
                typename unit_traits<Lhs>::exponent,
                typename unit_traits<Rhs>::exponent
            >::value;
}

template<class ... Lhs>
constexpr bool flat_equal( const mixed_unit<Lhs...> &, 
                           const dimensionless & )
{
    return false;
}

template<class Lhs>
constexpr bool flat_equal( const Lhs &, 
                           const dimensionless &)
{
    return false;
}

constexpr bool flat_equal( const dimensionless &, 
                           const dimensionless &)
{
    return true;
}

template<class ... Lhs, class Rhs>
constexpr bool flat_equal( const mixed_unit<Lhs...> & lhs, 
                           const Rhs & rhs )
{
    return flat_equal( lhs * inverse(rhs), dimensionless() );
}

template<class ... Lhs, class ... Rhs>
constexpr bool flat_equal( const mixed_unit<Lhs...> & lhs, 
                           const mixed_unit<Rhs...> & rhs )
{
    return flat_equal( lhs * inverse(rhs), dimensionless() );
}

}

/**
 * @addtogroup operators
 * @{
 */

/**
 * @brief Compare two @c mixed_unit for equality
 */
template<class ... Lhs, class ... Rhs>
constexpr bool operator==( const mixed_unit<Lhs...> & lhs, 
                           const mixed_unit<Rhs...> & rhs)
{
    return detail::flat_equal(lhs.flat(), rhs.flat());
}

template<class ... Lhs, class ... Rhs>
constexpr bool operator!=( const mixed_unit<Lhs...> & lhs, 
                           const mixed_unit<Rhs...> & rhs )
{
    return !(lhs == rhs);
}

/**
 * @brief Compare a @c mixed_unit with a @ref Unit
 * 
 * This function participates in the overload resolution only if
 * @p Rhs models @ref Unit.
 */
template<class ... Lhs, class Rhs>
constexpr ENGUNITS_ENABLE_IF_T( 
    is_unit_v<Rhs>, 
    bool
    ) operator==( const mixed_unit<Lhs...> & lhs, 
                  const Rhs & )
{
    return detail::flat_equal(lhs.flat(), 
                              unit_traits<Rhs>::flat() );
}

/**
 * @brief Compare a @c mixed_unit with a @ref Unit
 * 
 * This function participates in the overload resolution only if
 * @p Rhs models @ref Unit.
 */
template<class ... Lhs, class Rhs>
constexpr ENGUNITS_ENABLE_IF_T( 
    is_unit_v<Rhs>, 
    bool
    ) operator!=( const mixed_unit<Lhs...> & lhs, 
                  const Rhs & rhs )
{
    return !(lhs == rhs);
}

/**
 * @brief Compare a @ref Unit with a @c mixed_unit
 * 
 * This function participates in the overload resolution only if
 * @p Lhs models @ref Unit.
 */
template<class Lhs, class ... Rhs>
constexpr ENGUNITS_ENABLE_IF_T( 
    is_unit_v<Lhs>, 
    bool
    ) operator==( const Lhs &,
                  const mixed_unit<Rhs...> & rhs )
{
    return detail::flat_equal( rhs.flat(),
                               unit_traits<Lhs>::flat() );
}

/**
 * @brief Compare a @ref Unit with a @c mixed_unit
 * 
 * This function participates in the overload resolution only if
 * @p Lhs models @ref Unit.
 */
template<class Lhs, class ... Rhs>
constexpr ENGUNITS_ENABLE_IF_T( 
    is_unit_v<Lhs>, 
    bool
    ) operator!=( const Lhs & lhs,
                  const mixed_unit<Rhs...> & rhs )
{
    return !(lhs == rhs);
}

template<class ... Lhs>
constexpr bool operator==( const mixed_unit<Lhs...> & lhs, 
                           const dimensionless & rhs )
{
    return detail::flat_equal( lhs.flat(), rhs );
}

template<class ... Lhs>
constexpr bool operator!=( const mixed_unit<Lhs...> & lhs, 
                           const dimensionless & rhs )
{
    return !(lhs == rhs);
}

template<class ... Rhs>
constexpr bool operator==( const dimensionless & lhs,
                           const mixed_unit<Rhs...> & rhs)
{
    return detail::flat_equal( rhs.flat(), lhs );
}

template<class ... Rhs>
constexpr bool operator!=( const dimensionless & lhs,
                           const mixed_unit<Rhs...> & rhs )
{
    return !(lhs == rhs);
}

/**
 * @brief Compare two non @c mixed_unit for equality
 */
template<class Lhs,
         class Rhs>
constexpr ENGUNITS_ENABLE_IF_T(
    (engunits::is_unit_v<Lhs> && 
     engunits::is_unit_v<Rhs>),
    bool) operator==( const Lhs &, const Rhs &)
{
    return detail::flat_equal(
        unit_traits<Lhs>::flat(),
        unit_traits<Rhs>::flat() );
}

template<class Lhs,
         class Rhs>
constexpr ENGUNITS_ENABLE_IF_T(
    engunits::is_unit_v<Lhs> && 
    engunits::is_unit_v<Rhs>,
    bool) operator!=( const Lhs & lhs, const Rhs & rhs)
{
    return !(lhs == rhs);
}

/** @} */

}

#endif //ENGINEERING_UNITS_UNIT_EQUALITY_HPP
