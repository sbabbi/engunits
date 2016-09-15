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

#ifndef ENGINEERING_UNITS_UNIT_MULTIPLY_HPP
#define ENGINEERING_UNITS_UNIT_MULTIPLY_HPP

#include <engineering_units/unit/mixed_unit.hpp>
#include <engineering_units/unit/traits.hpp>

#include <engineering_units/detail/merge.hpp>

namespace engunits
{

namespace detail
{
    
struct multiply_strategy
{

    template<class ... Ts, class ... Us >
    static constexpr auto join( mixed_unit<Ts ... > const &,
                                mixed_unit<Us ... > const & )
    {
        return mixed_unit<Ts ..., Us ... >{};
    }

    template<class T, class ... Ts >
    static constexpr auto join( mixed_unit<Ts ... > const &, const T & )
    {
        return mixed_unit<Ts ..., T>{};
    }

    template<class T, class ... Ts >
    static constexpr auto join( const T &, mixed_unit<Ts ... > const & )
    {
        return mixed_unit<T, Ts ... >{};
    }

    template<class ... Ts >
    static constexpr auto join( mixed_unit<Ts ... > const &, 
                                const dimensionless & )
    {
        return mixed_unit<Ts ...>{};
    }

    template<class ... Ts >
    static constexpr auto join( const dimensionless &, 
                                mixed_unit<Ts ... > const & )
    {
        return mixed_unit<Ts ... >{};
    }

    static constexpr auto join( const dimensionless &, 
                                const dimensionless & )
    {
        return dimensionless{};
    }

    template<class T>
    static constexpr auto join( const T & t, 
                                const dimensionless &, 
                                std::enable_if_t<
                                    is_unit_v<T>,
                                    int
                                > = {} )
    {
        return t;
    }

    template<class U>
    static constexpr auto join( const dimensionless &,
                                const U & u, 
                                std::enable_if_t<
                                    is_unit_v<U>,
                                    int
                                > = {} )
    {
        return u;
    }
    
    template<class Base, class Exponent>
    static constexpr auto raise( const Base &, Exponent )
    {
        return typename unit_traits<Base>::template 
            base_< Exponent::num, Exponent::den > {};
    }
    
    template<class Base>
    static constexpr auto raise( const Base &, std::ratio<0> )
    {
        return dimensionless{};
    }
    
    template<class T, class U>
    static constexpr auto join_unit( const T &, const U &, std::true_type )
    {
        using base = typename unit_traits<T>::base;

        using exponent = std::ratio_add<
            typename unit_traits<T>::exponent,
            typename unit_traits<U>::exponent >;

        return raise( base{}, exponent{} );
    }
    
    template<class T, class U>
    static constexpr auto join_unit( const T &, const U &, std::false_type )
    {
        return mixed_unit<T, U>{};
    }

    template<class T, class U>
    static constexpr auto join( const T & t,
                                const U & u, 
                                std::enable_if_t<
                                    is_unit_v<T> && 
                                    is_unit_v<U>,
                                    int
                                > = {} )
    {
        return join_unit(t, u, is_same_base<T,U>{} );
    }

    template<class Head, class ... Tail>
    static constexpr auto front( mixed_unit<Head, Tail ... > const & )
    {
        return Head{};
    }

    template<class Head, class Tail>
    static constexpr auto back( mixed_unit<Head, Tail > const & )
    {
        return Tail{};
    }

    template<class Head, class ... Tail>
    static constexpr auto back( mixed_unit<Head, Tail ... > const & )
    {
        return back( mixed_unit<Tail ... >{} );
    }

    template<class Head, class ... Tail>
    static constexpr auto pop_front( mixed_unit<Head, Tail ... > const & )
    {
        return mixed_unit<Tail...>{};
    }

    template<class Head, class Tail >
    static constexpr auto pop_front( mixed_unit<Head, Tail> const & )
    {
        return Tail{};
    }

    template<class Head, class ... Tail>
    static constexpr auto pop_back( mixed_unit<Head, Tail ... > const & )
    {
        return join( Head{}, pop_back( mixed_unit<Tail...>{} ) );
    }

    template<class Head, class Tail >
    static constexpr auto pop_back( mixed_unit<Head, Tail> const & )
    {
        return Head{};
    }
};

template<class U, class T>
struct can_multiply : is_same_base<U, T> {};

template<class U>
struct can_multiply<U, dimensionless> : std::true_type {};

template<class U>
struct can_multiply<dimensionless, U> : std::true_type {};

template<>
struct can_multiply<dimensionless, dimensionless> : std::true_type {};

constexpr merge_t< mixed_unit,
                   multiply_strategy,
                   can_multiply > multiplies{};

}

template<class Lhs, class Rhs>
constexpr auto operator* (const Lhs & lhs, const Rhs & rhs) ->

    std::enable_if_t<
        is_unit_v<Lhs> && 
        is_unit_v<Rhs>,
        decltype( detail::multiplies(lhs, rhs) )
    >
{
    return detail::multiplies(lhs, rhs);
}

}

#endif //ENGINEERING_UNITS_UNIT_TRAITS_HPP
