/**
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

#ifndef ENGINEERING_UNITS_UNIT_MIXED_UNIT_HPP
#define ENGINEERING_UNITS_UNIT_MIXED_UNIT_HPP

#include <type_traits>

#include <engineering_units/unit/traits.hpp>

#include <engineering_units/detail/fold_expressions.hpp>

namespace engunits
{

template<class ... Ts>
struct mixed_unit;

namespace detail
{

template<class T> 
struct is_mixed_unit : std::false_type {};

template<class ... Ts> 
struct is_mixed_unit< mixed_unit<Ts ... > > : std::true_type {};

template<class T>
constexpr bool is_mixed_unit_v = is_mixed_unit<T>::value;

}
    
/**
 * @brief A list of base units.
 * 
 * No duplicates are allowed, i.e. "m^2, m" is ill-formed.
 */
template<class ... Ts>
struct mixed_unit
{
    static_assert( detail::all_of( is_unit_v<Ts> ... ),
                   "All elements of mixed_unit must be units" );
    
    static_assert( sizeof ... (Ts) != 0, "empty mixed_unit are not allowed");
    static_assert( sizeof ... (Ts) != 1, 
                   "mixed_unit with a single element are not allowed" );
    
    static_assert( detail::none_of( detail::is_mixed_unit_v<Ts> ... ),
                   "mixed_unit of mixed_unit are not allowed" );
    
    //TODO: static_assert Ts bases are all different

    static constexpr auto symbol()
    {
        return concatenate(' ', Ts::symbol() ... );
    }
    
    static constexpr auto flat()
    {
        return detail::multiply( unit_traits<Ts>::flat() ... );
    }
};

// Specialization for base units and derived units.
template< class ... Ts >
struct unit_traits< mixed_unit<Ts ... > >
{
    typedef mixed_unit<Ts ... > unit;
    typedef derived_unit_tag unit_category;
    
    typedef unit base;

    // TODO: implement base_<Num, Den>

    typedef std::ratio<1> exponent;
    
    // Convert derived units into mixed units.
    static constexpr auto flat() 
    {
        return unit::flat();
    }
};

}

#endif //ENGINEERING_UNITS_UNIT_MIXED_UNIT_HPP
