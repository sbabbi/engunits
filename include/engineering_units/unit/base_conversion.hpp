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

#ifndef ENGINEERING_UNITS_UNIT_BASE_CONVERSION_HPP
#define ENGINEERING_UNITS_UNIT_BASE_CONVERSION_HPP

#include <type_traits>

#include <engineering_units/unit/traits.hpp>
#include <engineering_units/detail/pow_ratio.hpp>

namespace engunits
{
    
template<class From, class To>
constexpr long double custom_conversion(From const &, To const &) = delete;

namespace detail
{

template<class U>
using is_base_unit = 
    std::is_same<
        typename unit_traits<U>::unit_category,
        base_unit_tag
    >;

template<class U>
constexpr bool is_base_unit_v = is_base_unit<U>::value;

    
/**
 * @internal
 * @brief Detect if a custom conversion between @p From and @p To exists.
 * 
 * Returns true if @c custom_conversion(@p From, @p To) is well formed.
 */
template<class From, class To, class = void>
struct custom_conversion_exists : std::false_type {};

template<class From, class To>
struct custom_conversion_exists<
    From,
    To,
    decltype( custom_conversion(
        std::declval<From const &>(),
        std::declval<To const &>()
    ), void() ) > : std::true_type {};

template<class From, class To>
constexpr bool custom_conversion_exists_v = custom_conversion_exists<From, To>::value;

namespace conversion_tag
{

/**
 * @internal
 * @brief Conversion only available through parent
 */
struct parent {};

/**
 * @internal
 * @brief Conversion available directly through opposite direction
 */
struct opposite : parent {};

/**
 * @internal
 * @brief Direct conversion available, via @c custom_conversion
 */
struct direct : opposite {};

}

template<class U, class = void>
struct find_root
{
    typedef U type;
};

template<class U>
struct find_root<U, decltype( std::declval<typename U::root_unit>(), void() )>
{
    typedef typename find_root<typename U::root_unit>::type type;
};

template<class From,
         class To,
         class FromRoot = typename find_root<From>::type,
         class ToRoot = typename find_root<To>::type>
struct find_common_ancestor
{};

template<class From,
         class To,
         class Root>
struct find_common_ancestor<
    From,
    To,
    Root,
    Root>
{
    typedef Root type;
};

template<class From, class To>
using find_common_ancestor_t = typename find_common_ancestor<From, To>::type;

template<class From, class To>
constexpr long double do_conversion( const From & from,
                                     const To & to,
                                     conversion_tag::parent,
                                     decltype( 
                                        find_common_ancestor_t<From, To>{},
                                        int()
                                     ) = 0 ) 
{
    using parent = find_common_ancestor_t<From, To>;
    
    // Go from "From" to "parent", then from "parent" to "To"
    return custom_conversion(from, parent{}) * custom_conversion(parent{}, to);
}

template<class From, class To>
constexpr long double do_conversion( const From & from,
                                     const To & to,
                                     conversion_tag::opposite,
                                     std::enable_if_t<
                                        custom_conversion_exists_v<To, From>,
                                        int
                                     > = 0) 
{    
    // Go from "From" to "parent", then from "parent" to "To"
    return 1.0L / custom_conversion(to, from);
}

template<class From, class To>
constexpr long double do_conversion( const From & from,
                                     const To & to,
                                     conversion_tag::direct,
                                     std::enable_if_t<
                                        custom_conversion_exists_v<From, To>,
                                        int
                                     > = 0 ) 
{    
    // Go from "From" to "parent", then from "parent" to "To"
    return custom_conversion(from, to);
}

/**
 * @internal 
 * @brief Conversion between the same unit is always one
 */
template<class U>
constexpr long double base_unit_conversion( const U &, const U & )
{
    static_assert( is_base_unit_v<U>, "" );
    return 1.0;
}

/**
 * @internal 
 * @brief Conversion between two base unit
 */
template<class From,
         class To,
         class FromBase = typename unit_traits<From>::base,
         class ToBase = typename unit_traits<To>::base,
         class FromExponent = typename unit_traits<From>::exponent,
         class ToExponent = typename unit_traits<To>::exponent >
constexpr auto base_unit_conversion( const From &, 
                                     const To &,
                                     std::enable_if_t<
                                        std::ratio_equal<FromExponent, ToExponent>::value,
                                        int
                                     > = 0 ) ->
    decltype( do_conversion( FromBase{}, ToBase{}, conversion_tag::direct{} ) )
{
    static_assert( is_base_unit_v<From>, "" );
    static_assert( is_base_unit_v<To>, "" );

    return pow_ratio( do_conversion( FromBase{}, ToBase{}, conversion_tag::direct{} ),
                      FromExponent::num,
                      FromExponent::den );
}

template<class From, class To, class = void>
struct base_unit_conversion_exists : std::false_type {};

template<class From, class To>
struct base_unit_conversion_exists<
    From,
    To,
    decltype( base_unit_conversion(From{}, To{}), void() ) > : std::true_type {};
    
template<class From, class To>
constexpr bool base_unit_conversion_exists_v =
    base_unit_conversion_exists<From, To>::value;

}
}


#endif //ENGINEERING_UNITS_UNIT_CONVERSION_HPP
