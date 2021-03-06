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

#ifndef ENGINEERING_UNITS_UNIT_CONCEPTS_HPP
#define ENGINEERING_UNITS_UNIT_CONCEPTS_HPP

#include <type_traits>
#include <engineering_units/unit/traits.hpp>
#include <engineering_units/detail/doxygen.hpp>

/**
 * @defgroup Concepts Concepts
 * @{
 */

namespace engunits
{

/**
 * @defgroup Unit Unit
 * @{
 *
 * @par Description
 *   
 *  A @c Unit type carries two pieces of information, the base and the exponent.
 *  -   The @c base is a tag used to distinguish between different units 
 *      (meter, joule, seconds, etc.). The @c base itself is a @c Unit, but with
 *      @c exponent equal to one. From that follows that any @c Unit with unit
 *      exponent is the base of itself.
 *
 *  -   @c exponent is always a specialization of @c std::ratio.
 *
 *  Accessing a @c Unit always happens through the @c unit_traits class. Users
 *  can specialize @c unit_traits to support any type that satisfies the 
 *  @c DefaultConstructible, @c CopyConstructible and @c Destructible concepts.
 * 
 *  This library defines equality, inequality and multiplication operator
 *  for all the units.
 * 
 * @par Requires
 *  - @c DefaultConstructible
 * 
 *  - @c CopyConstructible
 * 
 *  - @c Destructible
 * 
 *  - `unit_traits::base` 
 *          
 *     Models @ref Unit
 * 
 *  - `unit_traits::exponent -> std::ratio<Num, Den> `
 *
 *  - `unit_traits::base_<Num, Den>`
 * 
 *     Obtains a new unit with same base and different exponent
 * 
 *  - `unit_traits::symbol().c_str() -> const char * `
 * 
 *     Descriptive symbol
 * 
 *  - `unit_traits::flat()`
 * 
 *     Models @ref Unit
 * 
 * @sa is_unit
 * @sa unit_traits
 * 
 * @}
 */


/**
 * @defgroup BaseUnit BaseUnit
 * @{
 * 
 * @par Description
 * 
 *   The definition of @c BaseUnit in this library is a bit different from the
 *   usual one used in physics.
 *   We consider a base unit any unit whose *physical quantity* is not a derived one.
 *   For instance meters, millimeters and feet are all @c BaseUnit,
 *   while newtons, joule etc are not base units because \<force\> and 
 *   \<energy\> are derived physical quantities.
 *  
 *   Custom base units can be defined with the @ref ENGUNITS_DEFINE_BASE_UNIT macro.
 *      
 *   Custom physical quantities can be defined with the @ref ENGUNITS_DEFINE_ROOT_UNIT,
 *   which also defines a 'root' base unit for the newly created physical quantity.
 * 
 * @todo Document the conversion process, what 'root' is, and how @c parent is used.
 * 
 * @par Requires
 * 
 *   - @ref Unit
 * 
 *   - `unit_traits::unit_category == base_unit_tag`
 * 
 *   - `unit_traits::flat() == U()`
 * 
 * @sa Unit
 * @sa is_unit 
 * @sa base_unit_tag
 * 
 * @}
 */

/**
 * @defgroup DerivedUnit DerivedUnit
 * @{
 * 
 * @par Description
 * 
 *   Derived units are containers of @ref BaseUnit, that also models the @ref Unit
 *   concept.
 * 
 *   Units like \<newton\> or \<joule\> are derived units, because their physical
 *   quantities are derived.
 * 
 *   The list of @c BaseUnit that forms a derived unit can be accessed with 
 *   `unit_traits::flat` (see below).
 * 
 *   The class template @c mixed_unit is the generic model for *unnamed* @c DerivedUnits.
 * 
 * @par Requires
 * 
 *   - @ref Unit
 * 
 *   - `unit_traits::unit_category == derived_unit_tag`
 *      
 *   - `unit_traits::flat == mixed_unit<BaseUnits ...>`
 *          
 *      Notice that the @c mixed_unit returned by @c flat shall only contain
 *      @ref BaseUnit "BaseUnits".
 * 
 * @sa Unit
 * @sa BaseUnit
 * @sa is_unit
 * @sa derived_unit_tag
 * @sa mixed_unit
 * 
 * @}
 */

/** @} */
}


#endif //ENGINEERING_UNITS_UNIT_TRAITS_HPP

