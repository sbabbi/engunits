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

#ifndef ENGINEERING_UNITS_DETAIL_MERGE_HPP
#define ENGINEERING_UNITS_DETAIL_MERGE_HPP

#include <type_traits>

namespace engunits
{

namespace detail
{

/**
 * @internal
 * @brief Merges two sequences, according to a set of rules.
 *
 * @tparam Sequence The template that represents the sequence.
 * @tparam Traits A type that shall define static @c join, @c front, @c pop_front,
 *  @c back, @c pop_back functions.
 * @tparam Predicate A metafunction predicate @c pred\<a,b\>, where @c a and @c b are
 *  elements held in the sequence, used to detect when the elements can be merged.
 *
 * @note Base elements are merged with @c Traits::join.
 */
template<template<class ... > class Sequence,
         class Traits,
         template<class, class> class Predicate >
class merge_t
{
    // Predicate helper
    template<class T, class U>
    constexpr auto pred( const T &, const U & ) const
    {
        return typename Predicate<T,U>::type {};
    }

    template<class Lhs, class ... Ts>
    constexpr auto merge( const Lhs & lhs,
                          const Sequence<Ts...> & rhs,
                          std::true_type ) const
    {
        return Traits::join( Traits::pop_back( rhs ),
                             Traits::join( lhs, Traits::back( rhs ) ) );
    }

    template<class Lhs, class ... Ts>
    constexpr auto merge( const Lhs & lhs,
                          const Sequence<Ts...> & rhs,
                          std::false_type ) const
    {
        return Traits::join(
                   merge( lhs, Traits::pop_back( rhs ) ),
                   Traits::back( rhs ) );
    }

    template<class ... Ts, class Rhs>
    constexpr auto merge( const Sequence<Ts... > & lhs,
                          const Rhs & rhs,
                          std::true_type ) const
    {
        return Traits::join(
                   Traits::join( Traits::front( lhs ), rhs ),
                   Traits::pop_front( lhs ) );
    }

    template<class ... Ts, class Rhs>
    constexpr auto merge( const Sequence<Ts... > & lhs,
                          const Rhs & rhs,
                          std::false_type ) const
    {
        return Traits::join(
                   Traits::front( lhs ),
                   merge( Traits::pop_front( lhs ), rhs ) );
    }

    template<class Lhs, class ... Ts>
    constexpr auto merge( const Lhs & lhs,
                          const Sequence<Ts...> & rhs ) const
    {
        return merge( lhs, rhs, pred( lhs, Traits::back( rhs ) ) );
    }

    template<class ... Ts, class Rhs>
    constexpr auto merge( const Sequence<Ts...> & lhs,
                          const Rhs & rhs ) const
    {
        return merge( lhs, rhs, pred( Traits::front( lhs ), rhs ) );
    }

    template<class Lhs, class Rhs>
    constexpr auto merge( const Lhs & lhs,
                          const Rhs & rhs ) const
    {
        return Traits::join( lhs, rhs );
    }

    template<class ... Lhs, class ... Rhs>
    constexpr auto merge( const Sequence<Lhs...> & lhs,
                          const Sequence<Rhs...> & rhs ) const
    {
        return merge( merge( lhs, Traits::front( rhs ) ),
                      Traits::pop_front( rhs ) );
    }

    template<class ... Lhs, class ... None>
    constexpr auto merge( const Sequence<Lhs...> & lhs,
                          const Sequence<None...> & rhs,
                          std::enable_if_t<
                            sizeof ...( None ) == 0,
                            int
                          > = 0 ) const

    {
        return Traits::join( lhs, rhs );
    }

    template<class ... None, class ... Rhs>
    constexpr auto merge( const Sequence<None...> & lhs,
                          const Sequence<Rhs...> & rhs,
                          std::enable_if_t<
                            sizeof ...( None ) == 0,
                            int
                          > = 0
                        ) const

    {
        return Traits::join( lhs, rhs );
    }

    template<class ... None>
    constexpr auto merge( const Sequence<None ...> & lhs,
                          const Sequence<None ...> & rhs,
                          std::enable_if_t<
                            sizeof ...( None ) == 0,
                            int
                          > = 0 ) const

    {
        return Traits::join( lhs, rhs );
    }

public:
    template<class Lhs, class Rhs>
    constexpr auto operator()( const Lhs & lhs,
                               const Rhs & rhs ) const
    {
        return merge( lhs, rhs );
    }
};

}
}

#endif //ENGINEERING_UNITS_DETAIL_MERGE_HPP
