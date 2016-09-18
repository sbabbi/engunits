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

#include <cassert>
#include <iostream>
#include <limits>

#include <engineering_units/quantity.hpp>
#include <engineering_units/io.hpp>

#include <engineering_units/angle.hpp>
#include <engineering_units/time.hpp>
#include <engineering_units/si/length.hpp>
#include <engineering_units/si/mass.hpp>
#include <engineering_units/si/force.hpp>
#include <engineering_units/si/energy.hpp>
#include <engineering_units/si/power.hpp>

#include <engineering_units/imperial/length.hpp>
#include <engineering_units/imperial/mass.hpp>
#include <engineering_units/imperial/force.hpp>
#include <engineering_units/imperial/velocity.hpp>

namespace si = engunits::si;
namespace imperial = engunits::imperial;
using namespace si::literals;
using namespace imperial::literals;
using namespace engunits::literals;
using engunits::quantity;
using engunits::quantity_cast;

using engunits::second;
using engunits::second_;

void test_addition()
{
    static_assert( 1.0_m + 1.0_m == 2.0_m, " 1m + 1m = 2m " );
    static_assert( 1.0_m - 1.0_m == 0.0_m, " 1m - 1m = 0m " );
    
    static_assert( 1.0_N + 1.0_m * 1.0_kg / (1.0_s * 1.0_s) == 2.0_N, 
                   " 1N + 1m * 1kg / (1s * 1s) = 2N " );
    
    static_assert( 3.0*si::meter() + si::meter() * 5.0 ==
                   quantity<double, si::meter>(8.0),
                   "3m + 5m == 8");

    // These are expected to fail to compile
    // TODO setup a test suite to check for compilation error.
    // 1.0_m + 0.0_mm;
    // 1.0_m + 1.0_s;
    
}

void test_mult()
{
    static_assert( 2.0_m * 2.0_m == 4.0 * si::meter_<2>(), " 2m * 2m = 4m^2 " );
    
    static_assert( 4.0_m * 3.0_N / 2.0_s == 6.0_W, " 4m * 3N / 2s  = 6 W " );
    
    static_assert( double(quantity_cast(2.0_N / 4.0_lbf)) == 
                   double(0.5L * (1.0L/32.174049L) * (1.0L/0.45359237L) * (1.0L / 0.3048L)),
                    " 2N / 4 lbf" );
    
    static_assert( 3.0_m / 1.0_m == 3.0L,
                   "3m/m = 3");
}

void test_div()
{
    static_assert( 4.0_m / 2.0_m == 2.0, " 4m / 2m = 2" );
    static_assert( (4.0_m * si::meter()) / 2.0_m == 2.0_m, " 4m^2 / 2m = 2m" );
    static_assert( (4.0_m * imperial::foot()) / 2.0_ft == 2.0_m, " 4m ft / 2ft = 2m" );
    
    static_assert( 1.0 / 1.0_m == 1.0 * si::meter_<-1>(), "1 / 1m = 1m^-1");
    static_assert( 16.0_m / 2.0 == 8.0_m, "16m / 2 = 8m");
    
}

void test_functions()
{
    using engunits::pow;
    
    assert( 2.0_m == 2.0_m );
    assert( 2.0_m != 4.0_m );
    assert( 2.0_m < 3.0_m );
    assert( 2.0_m <= 2.0_m );
    assert( 2.0_m < 3.0_m );
    assert( 4.0_m > 1.0_m );
    assert( 6.0_m >= 4.0_m );
    assert( 6.0_m >= 6.0_m );

    assert( abs(-3.0_m) == 3.0_m );
    assert( fabs(-5.0_s) == 5.0_s );
    assert( fma(4.0_m, 2.0_s, 8.0 * si::meter() * second() ) == 
            16.0 * si::meter() * second() );
    
    assert( fmax(12.0_m, 
                 ( (0.0 * si::meter() * si::meter()) / (0.0_m) ) ) ==
            12.0_m );
    
    assert( fmin(3.0_J, 5.0_J) == 3.0_J );
    assert( fdim(3.0_W, 5.0_W) == 0.0_W );
    assert( pow<2>(3.0_m) == 9.0 * si::meter() * si::meter() );
    
    assert( sqrt(16.0_m) == (4.0 * si::meter_<1,2>()) );
    assert( cbrt(8.0 * second_<-3>() ) == (2.0L * second_<-1>()) );
    
    assert( fabs( hypot(3.0_m, 4.0_m) - 5.0_m ) < 1e-15_m );

    using std::fabs;
    // no hypot(a,b,c) pre C++17
    //assert( fabs( hypot(3.0_m, 4.0_m, 0.0_m) - 5.0_m ) < 1e-15_m );
    
    assert( fabs(sin(90.0_deg) - 1.0) < 1e-10 );
    assert( fabs(cos(90.0_deg)) < 1e-10 );
    assert( fabs(tan(45.0_deg) - 1.0 ) < 1e-10 );
}

int main() 
{
    test_addition();
    test_mult();
    test_div();
    test_functions();
}
