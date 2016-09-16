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

#ifndef ENGINEERING_UNITS_DETAIL_CONSTEXPR_POW_HPP
#define ENGINEERING_UNITS_DETAIL_CONSTEXPR_POW_HPP


namespace engunits
{

namespace detail
{


/**
 * @internal
 * @brief compute @p base raised to @p num
 */
constexpr long double constexpr_pow( long double base,
                                     std::intmax_t num )
{
    if ( num < 0 )
    {
        return constexpr_pow( 1.0L / base, -num );
    }

    long double y = 1.0L;

    for ( std::intmax_t i = 0; i < num; ++i )
    {
        y = y * base;
    }

    return y;
}

/**
 * @internal
 * @brief compute @p base raised to @p num / @p den
 * @note This is moderately sensitive to rounding error, for best results use
 *  co-prime @p num @p den.
 */
constexpr long double constexpr_pow( long double base,
                                     std::intmax_t num,
                                     std::intmax_t den )
{
    if ( den < 0 )
    {
        return constexpr_pow( 1.0L / base, num, -den );
    }

    long double y = constexpr_pow( base, num );

    if ( den == 1 )
    {
        return y;
    }

    // Solve x^den == y
    long double x = 1.0;

    for ( int i = 0; i < 1000; ++i )
    {
        const long double delta = ( y / constexpr_pow( x, den-1 ) - x ) / den;

        if ( x + delta == x )
            break;

        x += delta;
    }

    return x;
}

}
}

#endif //ENGINEERING_UNITS_DETAIL_CONSTEXPR_POW_HPP
