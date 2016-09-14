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

#ifndef ENGINEERING_UNITS_DETAIL_STRING_LITERAL_HPP
#define ENGINEERING_UNITS_DETAIL_STRING_LITERAL_HPP

#include <iosfwd>
#include <string>
#include <type_traits>

namespace engunits
{

namespace detail
{

template<std::size_t N>
struct string_literal
{
    string_literal(const char (&x)[N+1]) : v_{}
    {
        for ( int i = 0; i < N; ++i )
            v_[i] = x[i];
        v_[N] = 0;
    }
    
    constexpr const char * c_str() const noexcept { return v_; }
    
    constexpr char operator[](std::size_t i) const
    {
        return v_[i];
    }
    
    template<std::size_t U>
    string_literal<N+U> operator+(const string_literal<U> & other ) const noexcept
    {
        char buff[U+N+1];
        for ( int i = 0; i < N; ++i )
            buff[i] = v_[i];
        
        for ( int i = 0; i < U; ++i )
            buff[i + N] = other[i];
        
        buff[U+N] = 0;
        
        return string_literal<N+U>(buff);
    }

    operator std::string() const
    {
        return std::string(v_);
    }
    
private:
    char v_[N+1];
};

template<std::size_t N>
std::ostream& operator<<(std::ostream & os, string_literal<N> const & s)
{
    return os << s.c_str();
}

template<std::size_t N>
constexpr string_literal<N-1> make_string_literal(const char (&x)[N])
{
    return string_literal<N-1>(x);
}

}
}

#endif //ENGINEERING_UNITS_DETAIL_STRING_LITERAL_HPP
