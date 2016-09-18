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

#ifndef ENGINEERING_UNITS_QUANTITY_HPP
#define ENGINEERING_UNITS_QUANTITY_HPP

#include <cmath>
#include <type_traits>
#include <utility>

#include <engineering_units/unit/traits.hpp>
#include <engineering_units/unit/mixed_unit.hpp>

#include <engineering_units/unit/conversion.hpp>
#include <engineering_units/unit/equality.hpp>
#include <engineering_units/unit/multiply.hpp>
#include <engineering_units/unit/pow.hpp>

#include <engineering_units/detail/doxygen.hpp>
#include <engineering_units/detail/fold_expressions.hpp>

namespace engunits
{

namespace detail
{

template<class ... Ts>
struct unit_type
{
    typedef mixed_unit<Ts...> type;
};

template<class T>
struct unit_type<T>
{
    typedef T type;
};

template<class ... Ts>
using unit_type_t = typename unit_type<Ts...>::type;

}

/**
 * @brief Tag a type @p T with a (list of) unit.
 * @tparam T Type to hold
 * @tparam Units List of units.
 * 
 * This is the main class used to attach dimensional information to a value.
 * Common mathematical operators are overloaded to implement the actual
 * dimensional analysis, and forward to the relative operator of @p T to 
 * compute the numerical value.
 * 
 * While @p T is usually a numerical type, @c quantity supports any kind of type,
 * so quantities of `vector`, `string`, `matrix`, etc. are allowed.
 * 
 * @note Quantities of compatible but different units 
 *          (meters and millimeters, kilos and tonnes, etc.) are not implicitly convertible.
 *       You have to use the explicit constructors of @c quantity, or @c quantity_cast
 * 
 * @note @c mixed_unit is not allowed into @p Units
 * 
 * Example:
 * @code{.cpp}
 *   quantity<double, si::meter> x = 3.0_m;
 *   quantity<double, si::meter> y = 3.0_m + x; // y == 6.0_m
 * 
 *   quantity<double, second> z = 51.2_s;
 *   
 *   // error: adding meters and seconds
 *   x + z;
 * 
 *   // ok:
 *   quantity<double, si::meter, second_<-1> > v = x / z;
 * 
 *   // bad: implicit conversion
 *   quantity<double, si::meter> q = 1.0_ft;
 * 
 *   // ok: explicit conversion
 *   quantity<double, si::meter> q (1.0_ft);
 * 
 * @endcode
 * 
 * @sa make_quantity
 * @sa quantity_cast
 */
template<class T, class ... Units>
class quantity
{
public:
    /**
     * @brief The underlying type of this quantity
     */
    typedef T value_type;
    
    /**
     * @brief The unit type of this quantity
     * @sa Unit
     */
    typedef ENGUNITS_UNSPECIFIED(detail::unit_type_t<Units...>) unit_type;

private:
    template<class U, class ... OtherUnits>
    static constexpr bool allow_implicit_constructor =
        std::is_convertible<U, T>::value &&
        unit_type() == detail::unit_type_t<OtherUnits...>();

    template<class U, class ... OtherUnits>
    static constexpr bool allow_explicit_constructor =
        !std::is_convertible<U, T>::value &&
        std::is_constructible<T, U>::value &&
        unit_type() == detail::unit_type_t<OtherUnits...>();

    template<class U, class ... OtherUnits>
    static constexpr bool allow_converting_constructor =
        !allow_explicit_constructor<U, OtherUnits...> &&
        std::is_constructible<T, U>::value &&
        is_convertible_v<detail::unit_type_t<OtherUnits...>, unit_type >;

public:
    static_assert( sizeof ...( Units ) > 0, "Empty quantity not allowed" );
    static_assert( detail::all_of( is_unit_v<Units> ... ),
                   "quantity must be made of units" );

    /**
     * @brief Default constructor
     */
    constexpr quantity(
        ENGUNITS_ENABLE_IF( std::is_default_constructible<T>::value )
            ) noexcept( std::is_nothrow_default_constructible<T>::value )
    {}
    
    /**
     * @brief Construct with a value
     * 
     * This is the constructor used to insert external data into a quantity.
     * 
     * @code{.cpp}
     *    quantity<double, si::meter> x (32.0); // 32 meters
     * @endcode
     */
    template<class U>
    explicit constexpr quantity( U && other,
        ENGUNITS_ENABLE_IF(( std::is_constructible<T, U&&>::value ))
            ) noexcept( std::is_nothrow_constructible<T, U&&>::value ) :
        value_( std::forward<U>(other) )
    {}

    /**
     * @brief Copy construct from equivalent unit, and implicitly convertible @c value_type
     */
    template< class U, class ... OtherUnits >
    constexpr quantity(
        const quantity<U, OtherUnits ... > & other,
        ENGUNITS_ENABLE_IF( ( allow_implicit_constructor<const U &, OtherUnits ... > ) )
    ) noexcept( std::is_nothrow_constructible<T, U>::value ) :
        value_( other.value() )
    {}

    /**
     * @brief Copy construct from equivalent unit, and explicitly convertible @c value_type
     */
    template<class U, class ... OtherUnits >
    explicit constexpr quantity(
        const quantity<U, OtherUnits ... > & other,
        ENGUNITS_ENABLE_IF( ( allow_explicit_constructor<const U &, OtherUnits ... > ) )
    ) noexcept( std::is_nothrow_constructible<T, U>::value ) :
        value_( other.value() )
    {}

    /**
     * @brief Copy construct from a convertible unit and explicitly convertible @c value_type
     */
    template<class U, class ... OtherUnits >
    explicit constexpr quantity(
        const quantity<U, OtherUnits ... > & other,
        ENGUNITS_ENABLE_IF( ( allow_converting_constructor<const U &, OtherUnits ... > ) )
    ) noexcept( std::is_nothrow_constructible<T, U>::value ) :
        value_( other.value() * conversion_factor( other.unit(), unit() ) )
    {}
    
    /**
     * @brief Move construct from equivalent unit, and implicitly convertible @c value_type
     */
    template< class U, class ... OtherUnits >
    constexpr quantity(
        quantity<U, OtherUnits ... > && other,
        ENGUNITS_ENABLE_IF( ( allow_implicit_constructor<U&&, OtherUnits ... > ) )
    ) noexcept( std::is_nothrow_constructible<T, U>::value ) :
        value_( std::move(other.value()) )
    {}

    /**
     * @brief Move construct from equivalent unit, and explicitly convertible @c value_type
     */
    template<class U, class ... OtherUnits >
    explicit constexpr quantity(
        quantity<U, OtherUnits ... > && other,
        ENGUNITS_ENABLE_IF( ( allow_explicit_constructor<U&&, OtherUnits ... > ) )
    ) noexcept( std::is_nothrow_constructible<T, U>::value ) :
        value_( std::move(other.value()) )
    {}

    /**
     * @brief Move construct from a convertible unit and explicitly convertible @c value_type
     */
    template<class U, class ... OtherUnits >
    explicit constexpr quantity(
        quantity<U, OtherUnits ... > && other,
        ENGUNITS_ENABLE_IF( ( allow_converting_constructor<U&&, OtherUnits ... > ) )
    ) noexcept( std::is_nothrow_constructible<T, U>::value ) :
        value_( std::move(other.value()) * conversion_factor( other.unit(), unit() ) )
    {}
    
    /**
     * @brief Copy-assignment
     */
    constexpr quantity& operator=(const quantity & other)
        noexcept( std::is_nothrow_copy_assignable<T>::value )
    {
        value_ = other.value();
        return *this;
    }
    
    /**
     * @brief Move-assignment
     */
    constexpr quantity& operator=(quantity && other)
        noexcept( std::is_nothrow_move_assignable<T>::value )
    {
        value_ = std::move(other.value());
        return *this;
    }
    
    /**
     * @brief Return a const reference to the underlying value.
     */
    constexpr const T & value() const &
    {
        return value_;
    }
    
    /**
     * @brief Return a reference to the underlying value.
     */
    constexpr T & value() &
    {
        return value_;
    }
    
    /**
     * @brief Return a rvalue-reference to the underlying value.
     */
    constexpr T&& value() &&
    {
        return std::move(value_);
    }

    /**
     * @brief Obtain an unit object for this quantity's unit.
     */
    static constexpr auto unit()
    {
        return unit_type {};
    }
    
    explicit operator T() const &
    {
        static_assert( is_convertible_v<unit_type, dimensionless>,
                       "Use .value() to cast away the unit" );
        
        return value_ * conversion_factor( unit(), dimensionless() );
    }
    
    explicit operator T() &&
    {
        static_assert( is_convertible_v<unit_type, dimensionless>,
                       "Use .value() to cast away the unit" );
        
        return std::move(value_) * conversion_factor( unit(), dimensionless() );
    }

private:
    T value_;
};

/**
 * @brief Helper function to construct a quantity.
 * @relates quantity
 * 
 * This can be used to construct a quantity inline, example:
 * 
 * @code{.cpp}
 *   auto x = make_quantity( 3.0, si::meter(), si::second_<-1>()); // 3 mps
 * @endcode
 * 
 * It is usually more expressive to use the multiplication operator defined for units,
 * like:
 * 
 * @code{.cpp}
 * 
 *   auto x = 3.0 * si::meter() * si::second_<-1>(); // 3mps
 * 
 * @endcode
 * 
 */
template<class T, class U>
constexpr auto make_quantity( T && t, U const & )
{
    return quantity< std::remove_reference_t<T>, U >( 
        std::forward<T>(t)
    );
}

template<class T, class ... Us>
constexpr auto make_quantity( T && t, mixed_unit<Us...> const & )
{
    return quantity< std::remove_reference_t<T>, Us ... >( 
        std::forward<T>(t)
    );
}

template<class T>
constexpr auto make_quantity( T && t, dimensionless const & )
{
    return std::forward<T>(t);
}

/**
 * @brief Explicit cast between convertible quantities.
 * @relates quantity
 * 
 * This function can be used to perform a unit conversion.
 
 * @code{.cpp}
 *    auto y = 3.0_m;
 *    quantity< double, imperial::feet > x;
 * 
 *    x = quantity_cast< si::meter >(x); // multiplies the payload by the conversion factor
 *    
 *    // also possible:
 *    x = quantity<double, si::meter>(x);
 *
 *    // for multiple units:
 *    auto z = quantity_cast<double, si::meter, si::seconds_<-1> >(v);
 *
 *
 * @endcode
 * 
 * The list of destination units can also be empty. This is allowed
 * if the argument is dimensionless, but has a unit payload. Example:
 *
 * @code{.cpp}
 *     double x = 3.0_m / 1.0_ft; // Bad, implicit unit conversion are never allowed!
 *
 *     double x = quantity_cast(3.0_m / 1.0_ft); // ok, multiplies the payload by the conversion factor.
 * @endcode
 * 
 */
template<class ... To, class T, class ... Ts>
constexpr auto quantity_cast( const quantity<T, Ts ... > & u )
{
    auto unit = detail::multiply( dimensionless(), To() ... );
    return make_quantity<T>( u.value() * conversion_factor( u.unit(), unit ),
                             unit );
}

namespace detail
{

template<class Lhs,
         class Rhs,
         class ... RhsUnits>
constexpr auto dispatch_mult( const Lhs & lhs,
                              const quantity<Rhs, RhsUnits ... > & rhs,
                              std::true_type /*is_unit<Lhs>*/ )
{
    return make_quantity(rhs.value(), lhs * rhs.unit() );
}

template<class Lhs,
         class Rhs,
         class ... RhsUnits>
constexpr auto dispatch_mult( const Lhs & lhs,
                              const quantity<Rhs, RhsUnits ... > & rhs,
                              std::false_type /*is_unit<Lhs>*/ )
{
    return make_quantity( lhs * rhs.value(), rhs.unit() );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs>
constexpr auto dispatch_mult( const quantity<Lhs, LhsUnits ... > & lhs,
                              const Rhs & rhs,
                              std::true_type /*is_unit<Rhs>*/ )
{
    return make_quantity(lhs.value(), lhs.unit() * rhs );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs>
constexpr auto dispatch_mult( const quantity<Lhs, LhsUnits ... > & lhs,
                              const Rhs & rhs,
                              std::false_type /*is_unit<Rhs>*/ )
{
    return make_quantity(lhs.value() * rhs, lhs.unit() );
}

template<class Lhs,
         class Rhs,
         class ... RhsUnits>
constexpr auto dispatch_div( const Lhs & lhs,
                             const quantity<Rhs, RhsUnits ... > & rhs,
                             std::true_type /*is_unit<Lhs>*/ )
{
    return make_quantity( Rhs(1.0) / rhs.value(), lhs * inverse(rhs.unit()) );
}

template<class Lhs,
         class Rhs,
         class ... RhsUnits>
constexpr auto dispatch_div( const Lhs & lhs,
                             const quantity<Rhs, RhsUnits ... > & rhs,
                             std::false_type /*is_unit<Lhs>*/ )
{
    return make_quantity( lhs / rhs.value(), inverse(rhs.unit()) );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs>
constexpr auto dispatch_div( const quantity<Lhs, LhsUnits ... > & lhs,
                             const Rhs & rhs,
                             std::true_type /*is_unit<Rhs>*/ )
{
    return make_quantity(lhs.value(), lhs.unit() * inverse(rhs) );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs>
constexpr auto dispatch_div( const quantity<Lhs, LhsUnits ... > & lhs,
                             const Rhs & rhs,
                             std::false_type /*is_unit<Rhs>*/ )
{
    return make_quantity(lhs.value() / rhs, lhs.unit() );
}
}

/**
 * @addtogroup operators
 * @{
 * 
 */
template<class Rhs,
         class ... RhsUnits>
constexpr auto operator+( const quantity<Rhs, RhsUnits ... > & rhs )
{
    return make_quantity( +rhs.value(), rhs.unit() );
}

template<class Rhs,
         class ... RhsUnits>
constexpr auto operator-( const quantity<Rhs, RhsUnits ... > & rhs )
{
    return make_quantity( -rhs.value(), rhs.unit() );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator+( const quantity<Lhs, LhsUnits ... > & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator+ with different units" );
    
    return make_quantity( lhs.value() + rhs.value(), lhs.unit() );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator-( const quantity<Lhs, LhsUnits ... > & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator- with different units" );
    
    return make_quantity( lhs.value() - rhs.value(), lhs.unit() );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator*( const quantity<Lhs, LhsUnits ... > & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{    
    return make_quantity( lhs.value() * rhs.value(), lhs.unit() * rhs.unit() );
}

template<class Lhs,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator*( const Lhs & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{    
    return detail::dispatch_mult( lhs, rhs, is_unit<Lhs>{} );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs>
constexpr auto operator*( const quantity<Lhs, LhsUnits ... > & lhs,
                          const Rhs & rhs )
{    
    return detail::dispatch_mult( lhs, rhs, is_unit<Rhs>{} );
}

template<class Lhs,
         class Rhs>
constexpr ENGUNITS_ENABLE_IF_T(
    is_unit_v<Lhs> && !is_unit_v<Rhs>,
    quantity<Rhs, Lhs>) operator*( const Lhs &,
                                   const Rhs & rhs )
{    
    return quantity<Rhs, Lhs>(rhs);
}

template<class Lhs,
         class Rhs>
constexpr ENGUNITS_ENABLE_IF_T(
    !is_unit_v<Lhs> && is_unit_v<Rhs>,
    quantity<Lhs, Rhs>) operator*( const Lhs & lhs,
                                   const Rhs & )
{    
    return quantity<Lhs, Rhs>(lhs);
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator/( const quantity<Lhs, LhsUnits ... > & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{    
    return make_quantity( lhs.value() / rhs.value(), 
                          lhs.unit() * inverse(rhs.unit()) );
}

template<class Lhs,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator/( const Lhs & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{    
    return detail::dispatch_div( lhs, rhs, is_unit<Lhs>{} );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs>
constexpr auto operator/( const quantity<Lhs, LhsUnits ... > & lhs,
                          const Rhs & rhs )
{    
    return detail::dispatch_div( lhs, rhs, is_unit<Rhs>{} );
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator==( const quantity<Lhs, LhsUnits ... > & lhs,
                           const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator== with different units" );
    
    return lhs.value() == rhs.value();
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator!=( const quantity<Lhs, LhsUnits ... > & lhs,
                           const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator!= with different units" );
    
    return lhs.value() != rhs.value();
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator<( const quantity<Lhs, LhsUnits ... > & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator< with different units" );
    
    return lhs.value() < rhs.value();
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator<=( const quantity<Lhs, LhsUnits ... > & lhs,
                           const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator<= with different units" );
    
    return lhs.value() <= rhs.value();
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator>( const quantity<Lhs, LhsUnits ... > & lhs,
                          const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator> with different units" );
    
    return lhs.value() > rhs.value();
}

template<class Lhs,
         class ... LhsUnits,
         class Rhs,
         class ... RhsUnits>
constexpr auto operator>=( const quantity<Lhs, LhsUnits ... > & lhs,
                           const quantity<Rhs, RhsUnits ... > & rhs )
{
    static_assert( quantity<Lhs, LhsUnits ... >::unit() ==
                   quantity<Rhs, RhsUnits ... >::unit(), 
                   "operator>= with different units" );
    
    return lhs.value() >= rhs.value();
}

template<class Rhs,
         class ... RhsUnits>
constexpr auto abs( const quantity<Rhs, RhsUnits ... > & rhs )
{
    using std::abs;
    return make_quantity( abs(rhs.value()), rhs.unit() );
}

template<class Rhs,
         class ... RhsUnits>
constexpr auto fabs( const quantity<Rhs, RhsUnits ... > & rhs )
{
    using std::fabs;
    return make_quantity( fabs(rhs.value()), rhs.unit() );
}

template<class T,
         class ... TsUnits,
         class U,
         class ... UsUnits,
         class V,
         class ... VsUnits>
constexpr auto fma( const quantity<T, TsUnits ... > & x,
                    const quantity<U, UsUnits ... > & y,
                    const quantity<V, VsUnits ... > & z )
{
    static_assert( quantity<T, TsUnits ... >::unit() *
                   quantity<U, UsUnits ... >::unit() ==
                   quantity<V, VsUnits ... >::unit(),
                   "fma with incompatible units" );
    
    using std::fma;
    return make_quantity( fma(x.value(), y.value(), z.value()),
                          z.unit() );
}

template<class T,
         class ... TsUnits,
         class U,
         class ... UsUnits>
constexpr auto fmax( const quantity<T, TsUnits ... > & x,
                     const quantity<U, UsUnits ... > & y )
{
    static_assert( quantity<T, TsUnits ... >::unit() ==
                   quantity<U, UsUnits ... >::unit(),
                   "fmax with different units" );
        
    using std::fmax;
    return make_quantity( fmax(x.value(), y.value()), x.unit() );
}

template<class T,
         class ... TsUnits,
         class U,
         class ... UsUnits>
constexpr auto fmin( const quantity<T, TsUnits ... > & x,
                     const quantity<U, UsUnits ... > & y )
{
    static_assert( quantity<T, TsUnits ... >::unit() ==
                   quantity<U, UsUnits ... >::unit(),
                   "fmin with different units" );
    
    using std::fmin;
    return make_quantity( fmin(x.value(), y.value()), x.unit() );
}

template<class T,
         class ... TsUnits,
         class U,
         class ... UsUnits>
constexpr auto fdim( const quantity<T, TsUnits ... > & x,
                     const quantity<U, UsUnits ... > & y )
{
    static_assert( quantity<T, TsUnits ... >::unit() ==
                   quantity<U, UsUnits ... >::unit(),
                   "fdim with different units" );
    
    using std::fdim;
    return make_quantity( fdim(x.value(), y.value()), x.unit() );
}

template<std::intmax_t Exp,
         class Rhs,
         class ... RhsUnits>
constexpr auto pow( const quantity<Rhs, RhsUnits ... > & rhs )
{
    using std::pow;
    return make_quantity( pow(rhs.value(), Exp),
                          engunits::pow(rhs.unit(), std::ratio<Exp>()) );
}

template<class Rhs,
         class ... RhsUnits>
constexpr auto sqrt( const quantity<Rhs, RhsUnits ... > & rhs )
{
    using std::sqrt;
    return make_quantity( sqrt(rhs.value()), 
                          pow(rhs.unit(), std::ratio<1,2>()) );
}

template<class Rhs,
         class ... RhsUnits>
constexpr auto cbrt( const quantity<Rhs, RhsUnits ... > & rhs )
{
    using std::cbrt;
    return make_quantity( cbrt(rhs.value()),
                          pow(rhs.unit(), std::ratio<1,3>()) );
}

template<class T,
         class ... TsUnits,
         class U,
         class ... UsUnits>
constexpr auto hypot( const quantity<T, TsUnits ... > & x,
                      const quantity<U, UsUnits ... > & y )
{
    static_assert( quantity<T, TsUnits ... >::unit() ==
                   quantity<U, UsUnits ... >::unit(),
                   "hypot with different units" );
    
    using std::hypot;
    return make_quantity( hypot(x.value(), y.value() ), x.unit() );
}

template<class T,
         class ... TsUnits,
         class U,
         class ... UsUnits,
         class V,
         class ... VsUnits>
constexpr auto hypot( const quantity<T, TsUnits ... > & x,
                      const quantity<U, UsUnits ... > & y,
                      const quantity<V, VsUnits ... > & z )
{
    static_assert( quantity<T, TsUnits ... >::unit() ==
                   quantity<U, UsUnits ... >::unit() &&
                   quantity<T, TsUnits ... >::unit() ==
                   quantity<V, VsUnits ... >::unit(),
                   "hypot with different units" );
    
    using std::hypot;
    return make_quantity( hypot(x.value(), y.value(), z.value()),
                          x.unit() );
}

/** @} */

}

#endif //ENGINEERING_UNITS_QUANTITY_HPP
