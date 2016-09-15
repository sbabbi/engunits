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

#include <engineering_units/unit/base_conversion.hpp>
#include <engineering_units/unit/conversion.hpp>
#include <engineering_units/si/length.hpp>
#include <engineering_units/si/time.hpp>
#include <engineering_units/si/mass.hpp>
#include <engineering_units/si/force.hpp>
#include <engineering_units/imperial/length.hpp>

template<class From, class To, class = void>
struct check_base_unit_conversion : std::false_type {};

template<class From, class To>
struct check_base_unit_conversion<
    From,
    To,
    decltype( engunits::detail::base_unit_conversion(From{}, To{}), void() )> : 
std::true_type {};

void check_custom_conversion()
{
    namespace si = engunits::si;
    using engunits::detail::custom_conversion_exists_v;
    
    static_assert( custom_conversion_exists_v< si::millimeter, si::meter >,
                   " custom_conversion meter, millimeter " );
}

void check_basic_conversion()
{
    namespace si = engunits::si;
    namespace imperial = engunits::imperial;

    using engunits::detail::base_unit_conversion;
    using engunits::detail::find_common_ancestor_t;
    
    static_assert( base_unit_conversion( si::meter(), si::meter() ) == 1.0L,
                   " 1 meter == 1 meter " );
    
    static_assert( base_unit_conversion( si::meter(), si::millimeter() ) == (1.0L / 0.001L),
                   " 1 meter == 1000 millimeters" );
    
    static_assert( base_unit_conversion( si::millimeter(), si::meter() ) == 0.001L,
                   " 1 millimeter == 0.001 meter" );
    
    static_assert( check_base_unit_conversion<si::millimeter, si::second>::value == false,
                   "millimeter can not be converted to second" );
    
    static_assert( check_base_unit_conversion<imperial::inch, imperial::feet>::value == true,
                   "inch can be converted to feet" );
    
    static_assert( std::is_same<
                    find_common_ancestor_t< imperial::inch, si::meter >,
                    si::meter >::value, "ancestor(inch, meter) == meter" );
    
//     static_assert( base_unit_conversion(imperial::inch(), si::meter()) ==
//                         (1.0L / 12.0L) * 0.3048L,
//                    "inch to meter" );

}

void check_derived_conversion()
{
    namespace si = engunits::si;
    namespace imperial = engunits::imperial;
    using engunits::mixed_unit;
    
    auto x1 = mixed_unit<si::meter, si::second_<-1> >();
    auto x2 = mixed_unit<imperial::feet, si::second_<-1> >();
    
    static_assert( 
        double( engunits::conversion_factor(x1, x2) ) ==
        double( 1.0L / 0.3048L ),
        " m s^1 to ft s^-1" );
}

int main()
{
    check_custom_conversion();
    check_basic_conversion();
    check_derived_conversion();
}
