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

#ifndef ENGINEERING_UNITS_UNIT_SIMPLIFY_HPP
#define ENGINEERING_UNITS_UNIT_SIMPLIFY_HPP

#include <engineering_units/unit/traits.hpp>
#include <engineering_units/unit/mixed_unit.hpp>
#include <engineering_units/unit/base_conversion.hpp>

#include <engineering_units/detail/merge.hpp>


namespace engunits
{

namespace detail
{

template<class T, class U>
struct can_simplify : 
    std::is_same<
        typename T::dimension_tag,
        typename U::dimension_tag
    > {};

template<class T, class U>
constexpr bool can_simplify_v = can_simplify<T, U>::value;

/**
 * @internal
 * @brief Hold a conversion factor and a unit
 * @note This is useful to hold temporary conversion results.
 */
template<class ... Ts>
class conversion_factor_with_unit
{
public:
    
    constexpr conversion_factor_with_unit(long double c = 1.0L) : 
        cf_(c)
    {}
    
    constexpr long double factor() const { return cf_; }

private:
    long double cf_;
};

struct simplify_strategy
{
    template<class Lhs, class Rhs>
    static constexpr auto get_conversion_factor( Lhs const &, Rhs const & rhs )
    {
        // Get rhs exponent
        using rhs_exponent = typename unit_traits<Rhs>::exponent;
        
        using new_unit = typename unit_traits<Lhs>::template 
            base_< rhs_exponent::num, rhs_exponent::den >;
            
        return convert_base_unit( rhs, new_unit{} );
    }

    template<class ... Ts, class ... Us>
    static constexpr auto join( conversion_factor_with_unit<Ts...> const & t,
                                conversion_factor_with_unit<Us...> const & u)
    {
        return conversion_factor_with_unit<Ts..., Us...>(t.factor() * u.factor());
    }
    
    template<class Lhs, class ... Us>
    static constexpr auto join( Lhs const &,
                                conversion_factor_with_unit<Us...> const & u)
    {
        static_assert( is_unit_v<Lhs>, "" );
        return conversion_factor_with_unit<Lhs, Us...>(u.factor());
    }
    
    template<class ... Us, class Rhs>
    static constexpr auto join( conversion_factor_with_unit<Us...> const & u,
                                Rhs const & )
    {
        static_assert( is_unit_v<Rhs>, "" );
        return conversion_factor_with_unit<Us..., Rhs>(u.factor());
    }
    
    template<class Lhs, class Rhs, class ResultExponent>
    static constexpr auto join_base( Lhs const & lhs, Rhs const & rhs, ResultExponent )
    {
        // Result is base(lhs) ^ newexponent
        using result = typename unit_traits<Lhs>::template 
            base_< ResultExponent::num, ResultExponent::den >;

        return conversion_factor_with_unit< result > (
            get_conversion_factor(lhs, rhs) );
    }
    
    template<class Lhs, class Rhs>
    static constexpr auto join_base( Lhs const & lhs, Rhs const & rhs, std::ratio<0> )
    {
        return conversion_factor_with_unit< > ( get_conversion_factor(lhs, rhs) );
    }
    
    template<class Lhs, class Rhs>
    static constexpr auto join( Lhs const & lhs, Rhs const & rhs, std::true_type )
    {
        using lhs_exponent = typename unit_traits<Lhs>::exponent;
        using rhs_exponent = typename unit_traits<Rhs>::exponent;
        
        return join_base(lhs, rhs, std::ratio_add<lhs_exponent, rhs_exponent>{});
    }
    
    template<class Lhs, class Rhs>
    static constexpr auto join( Lhs const &, Rhs const &, std::false_type )
    {
        return conversion_factor_with_unit<Lhs, Rhs>();
    }
    
    template<class Lhs, class Rhs>
    static constexpr auto join( Lhs const & lhs,
                                Rhs const & rhs )
    {
        static_assert( is_unit_v<Lhs>, "" );
        static_assert( is_unit_v<Rhs>, "" );
        return join(lhs, rhs, can_simplify<Lhs, Rhs>{} );
    }
        
    template<class Head, class ... Tail>
    static constexpr auto front( conversion_factor_with_unit<Head, Tail ... > const & )
    {
        return Head{};
    }

    template<class Head, class Tail>
    static constexpr auto back( conversion_factor_with_unit<Head, Tail > const & )
    {
        return Tail{};
    }

    template<class Head, class ... Tail>
    static constexpr auto back( conversion_factor_with_unit<Head, Tail ... > const & )
    {
        return back( conversion_factor_with_unit<Tail ... >{} );
    }

    template<class Head, class ... Tail>
    static constexpr auto pop_front( conversion_factor_with_unit<Head, Tail ... > const & u )
    {
        return conversion_factor_with_unit<Tail...>(u.factor());
    }

    template<class Head, class ... Tail>
    static constexpr auto pop_back( conversion_factor_with_unit<Head, Tail ... > const & u )
    {
        return join( Head{}, pop_back( conversion_factor_with_unit<Tail...>(u.factor()) ) );
    }

    template<class Head, class Tail >
    static constexpr auto pop_back( conversion_factor_with_unit<Head, Tail> const & u)
    {
        return conversion_factor_with_unit<Head>(u.factor());
    }
};

constexpr merge_t< conversion_factor_with_unit,
                   simplify_strategy,
                   can_simplify > simplifies{};

}
}


#endif //ENGINEERING_UNITS_UNIT_CONVERSION_HPP
