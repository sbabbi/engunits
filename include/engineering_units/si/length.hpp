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

#ifndef ENGINEERING_UNITS_SI_LENGTH_HPP
#define ENGINEERING_UNITS_SI_LENGTH_HPP

#include <engineering_units/unit/helper_macros.hpp>

namespace engunits
{
namespace si
{

ENGUNITS_DEFINE_ROOT_UNIT( meter, m, length );
ENGUNITS_DEFINE_BASE_UNIT( decimeter,  dm, meter, 0.1L );
ENGUNITS_DEFINE_BASE_UNIT( centimeter, cm, meter, 0.01L );
ENGUNITS_DEFINE_BASE_UNIT( millimeter, mm, meter, 0.001L );
ENGUNITS_DEFINE_BASE_UNIT( decameter, dam, meter, 10.0L );
ENGUNITS_DEFINE_BASE_UNIT( hectometer, hm, meter, 100.0L );
ENGUNITS_DEFINE_BASE_UNIT( kilometer,  km, meter, 1000.0L );
ENGUNITS_IMPORT_OPERATORS

}
}

#endif //ENGINEERING_UNITS_SI_LENGTH_HPP
