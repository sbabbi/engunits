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

#include <iostream>

#include <engineering_units/time.hpp>
#include <engineering_units/si/length.hpp>
#include <engineering_units/si/mass.hpp>
#include <engineering_units/si/force.hpp>

#include <engineering_units/unit/traits.hpp>

template<class U>
constexpr auto get_symbol(const U &)
{
    using engunits::unit_traits;
    return unit_traits<U>::symbol();
}

void test_symbol()
{
    namespace si = engunits::si;

    static_assert( get_symbol( si::meter() ) == "m", "");
    static_assert( get_symbol( si::meter_<2>() ) == "m^2", "");
    static_assert( get_symbol( si::meter_<-3>() ) == "m^-3", "");
    static_assert( get_symbol( si::meter_<2,3>() ) == "m^(2/3)", "");
    static_assert( get_symbol( si::meter_<-2,3>() ) == "m^(-2/3)", "");
}

int main()
{
    test_symbol();
}
