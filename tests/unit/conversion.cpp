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

#include <engineering_units/unit/conversion.hpp>

#include <engineering_units/angle.hpp>
#include <engineering_units/time.hpp>
#include <engineering_units/si/length.hpp>
#include <engineering_units/si/mass.hpp>
#include <engineering_units/si/force.hpp>
#include <engineering_units/si/energy.hpp>

#include <engineering_units/imperial/length.hpp>
#include <engineering_units/imperial/mass.hpp>
#include <engineering_units/imperial/force.hpp>

namespace si = engunits::si;
namespace imperial = engunits::imperial;

using engunits::is_convertible;

using engunits::second_;
using engunits::second;

using engunits::minute_;
using engunits::minute;

using engunits::radian_;
using engunits::radian;

using engunits::degree_;
using engunits::degree;

void test_is_convertible()
{
    static_assert( 
        is_convertible( si::meter(), si::millimeter() ),
        "meter is convertible to millimeter" );
    
    static_assert( 
        is_convertible( si::meter_<-1>(), imperial::nautical_mile_<-1>() ),
        "meter^-1 is convertible to nm^-1" );
    
    static_assert( 
        !is_convertible( si::meter_<-1>(), si::millimeter() ),
        "meter^-1 is not convertible to mm" );
    
    static_assert( 
        !is_convertible( si::meter_<-1>(), imperial::nautical_mile() ),
        "meter^-1 is not convertible to nm" );
        
    static_assert( 
        is_convertible( si::meter() * si::kilogram() * second_<-2>(),
                        si::newton() ),
        "m kg / s^2 is convertible to Newton" );

    static_assert( 
        is_convertible( si::centimeter() * si::decagram() * minute_<-2>(),
                        si::newton() ),
        "cm dg / min^2 is convertible to Newton" );
    
    static_assert( 
        is_convertible( si::newton(),
                        si::centimeter() * si::decagram() * minute_<-2>() ),
        "Newton is convertible to cm dg / min^2 " );
    
    static_assert( 
        is_convertible( si::newton_<3,2>(),
                        si::centimeter_<3,2>() * si::decagram_<3,2>() * minute_<-3>() ),
        "Newton^(3/2) is convertible to cm^(3/2) dg^(3/2) / min^3 " );
    
    static_assert( 
        !is_convertible( si::joule(),
                        si::centimeter() * si::decagram() * minute_<-2>() ),
        "joule is not convertible to cm dg / min^2 " );
    
    static_assert( 
        !is_convertible( si::joule(),
                         si::kilometer() * si::centimeter() * si::decagram() * minute_<-2>() ),
        "joule is convertible to km cm dg / min^2 " );
    
    static_assert( 
        is_convertible( degree() * second() * si::meter() * si::kilogram(),
                        radian() * minute() * imperial::feet() * imperial::pound() ),
        "deg s m kg is convertible to rad min ft lb" );
    
    static_assert( 
        !is_convertible( degree() * second() * si::meter() * si::kilogram(),
                         radian_<-1>() * minute() * imperial::feet() * imperial::pound() ),
        "deg s m kg is NOT convertible to rad^-1 min ft lb" );

    static_assert( 
        is_convertible( imperial::pound_force() * imperial::feet(),
                        si::joule() ),
        "lbf ft is convertible to joule" );
    
    static_assert( 
        !is_convertible( imperial::pound_force(),
                         si::joule() ),
        "lbf is NOT convertible to joule" );
}

void test_base_conversion()
{
    using engunits::conversion_factor;

    static_assert( conversion_factor(si::meter(), si::meter() ) == 1.0L,
                   "meter to meter = 1.0 " );
    
    static_assert( conversion_factor(imperial::feet(), si::meter() ) == 0.3048L,
                   "feet to meter = 0.3048L " );
    
    static_assert( conversion_factor(si::meter(), imperial::feet() ) == 
                   (1.0L / 0.3048L),
                   "meter to feet = 1.0 / 3048L " );
    
    static_assert( conversion_factor(si::meter_<-1>(), imperial::feet_<-1>() ) == 
                   1.0L / (1.0L / 0.3048L),
                   "meter^-1 to feet^-1 = 0.3048L" );

    
    static_assert( conversion_factor(imperial::inch(), imperial::feet() ) == 
                    (1.0L / 12.0L),
                   "inch to feet = 1.0 / 12.0L" );
    
    static_assert( conversion_factor(imperial::feet(), imperial::inch() ) == (12.0L),
                   "feet to inch = 12.0L" );
    
    static_assert( conversion_factor(imperial::inch(), si::meter() ) == 
                   0.3048L * (1.0L / 12.0L),
                   "inch to meter = (inch to feet) * (feet to meter) = "
                   "1.0 / 12.0L * 0.3048L" );
    
    static_assert( 
        double( conversion_factor(imperial::inch(), si::millimeter() ) ) == 
        double( (1.0L / 12.0L) * (0.3048L) * 1000.0L) ,
        "inch to millimeter = (inch to feet) * (feet to meter) * (meter to millimeter) = "
        "(1.0 / 12.0L) * 0.3048L * 1000" );
    
    static_assert( 
        double( conversion_factor(si::millimeter(), imperial::inch() ) ) == 
        double( 0.001L * (1.0L / 0.3048L) * 12.0L),
        "millimeter to inch = "
        "(millimeter to meter) * (meter to feet) * (feet to inch) = "
        "0.001L * (1.0L / 0.3048L) * 12.0L" );

    static_assert( conversion_factor(si::meter_<2>(), si::meter_<2>() ) == 1.0L,
                   "meter^2 to meter^2 = 1.0 " );
    
    static_assert( conversion_factor(imperial::feet_<2>(), si::meter_<2>() ) == 
                   0.3048L * 0.3048L,
                   "feet to meter = 0.3048L * 0.3048L " );
    
    static_assert( conversion_factor(si::meter_<3>(), imperial::feet_<3>() ) == 
                   (1.0L / 0.3048L) * (1.0L / 0.3048L) * (1.0L / 0.3048L),
                   "meter to feet = (1.0 / 3048L) ^3  " );
    
    static_assert( conversion_factor(imperial::inch_<-1>(), imperial::feet_<-1>() ) == 
                   1.0L / (1.0L / 12.0L),
                   "inch^-1 to feet^-1 = 12.0L" );
    
    static_assert( 
        double( conversion_factor(imperial::feet_<1,2>(), imperial::inch_<1,2>() ) *
                conversion_factor(imperial::feet_<1,2>(), imperial::inch_<1,2>() ) ) == 
        double( 12.0L ),
        "(feet^(1/2) to inch^(1/2) )^2 = 12.0L" );
    
    static_assert( conversion_factor(imperial::inch_<2>(), si::meter_<2>() ) == 
                   ( 0.3048L * (1.0L / 12.0L) * 0.3048L * (1.0L / 12.0L) ),
                   "inch^2 to meter^2 = ( (inch to feet) * (feet to meter) )^2 ="
                   "1.0 / 12.0L * 0.3048L" );
    
    static_assert( 
        double( conversion_factor(imperial::inch_<2>(), si::millimeter_<2>() ) ) == 
        double( ( (1.0L / 12.0L) * (0.3048L) * 1000.0L) * 
                ( (1.0L / 12.0L) * (0.3048L) * 1000.0L) ),
        "inch^2 to millimeter^2 = "
        "((inch to feet) * (feet to meter) * (meter to millimeter))^2 = "
        "( (1.0 / 12.0L) * 0.3048L * 1000 )^2" );
    
    static_assert( 
        double( conversion_factor(si::millimeter_<3>(), imperial::inch_<3>() ) ) == 
        double( (0.001L * (1.0L / 0.3048L) * 12.0L) *
                (0.001L * (1.0L / 0.3048L) * 12.0L) *
                (0.001L * (1.0L / 0.3048L) * 12.0L) ),
        "millimeter^3 to inch^3 = "
        "( (millimeter to meter) * (meter to feet) * (feet to inch) )^3 = "
        "( 0.001L * (1.0L / 0.3048L) * 12.0L)^3" );
    
}

void test_derived_conversion()
{
    using engunits::conversion_factor;

    static_assert( 
        conversion_factor( si::meter() * si::kilogram() * second_<-2>(),
                           si::newton() ) == 1.0L,
        "m kg / s^2 to Newton" );
    
    static_assert( 
        double(conversion_factor( 
            si::centimeter() * si::decagram() * minute_<-2>(),
            si::newton() )
        ) == 
        double( (0.01L) * (0.01L) * (1.0L / (60.0L * 60.0L) ) ),
        "cm dg / min^2 to Newton" );
    
    static_assert( 
        double(conversion_factor( 
            si::newton(),
            si::centimeter() * si::decagram() * minute_<-2>() ) 
        ) ==
        double( (1.0L/0.01L) * (1.0L/0.01L) * (60.0L * 60.0L) ),
        "Newton to cm dg / min^2 " );
    
    static_assert( 
        double(conversion_factor( 
            si::newton_<3,2>(),
            si::centimeter_<3,2>() * si::decagram_<3,2>() * minute_<-3>() )
        ) ==
        double(216000000000.0L),
        "Newton^(3/2) to cm^(3/2) dg^(3/2) / min^3 " );
    
    static_assert(
        double(conversion_factor( degree() * second(), radian() * minute() ) ) ==
        double(0.0002908882086657216),
        "degree second to rad minute");

    static_assert( 
        double(conversion_factor( 
            degree() * second() * si::meter() * si::kilogram(),
            radian() * minute() * imperial::feet() * imperial::pound() )
        ) ==
        double( (0.017453292519943295088125L) *
                (1.0L / 60.0L) *
                (1.0L / 0.3048L) *
                (1.0L / 0.45359237L) 
        ),
        "deg s m kg to rad min ft lb" );
    
    static_assert( 
        double( conversion_factor( imperial::pound_force() * imperial::feet(),
                                   si::joule() ) ) ==
        double( 32.174049L * 0.45359237L * 0.3048L * 0.3048L ), 
        "lbf ft to joule" );
}

int main()
{
    test_is_convertible();
    test_base_conversion();
    test_derived_conversion();
}
