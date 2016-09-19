# Introduction

`Engunits` is a C++14 library that provides compile-time dimensional analysis. 
It aims to help developers in the physics and engineering field, allowing to catch bugs much earlier in the developement process, while mantaining zero runtime overhead.

It works by allowing the user to attach a unit to an integral type (such as double), and then wrapping all the operators and common mathematical functions in order to check that the units used are compatible, emitting a compiler error through `static_assert` if they are not.

Additionally, the library support explicit unit conversions, in order to automatically convert units to and from the S.I. system, without hard-coded constants hanging around in the code.

This library a compiler compliant with the C++14 standard. It has been tested with gcc 6.2.1 and clang 3.8.1.

### Design criteria

* Readable error messages
        - No generic template for units. Every unit has its own class.
        - Detect quantity errors through `static_assert`, no SFINAE.
        - Derived units are never expanded.
* Unit conversions must be explicit.
        - Either via `quantity` explicit constructor, or via `quantity_cast`.
        - Any operation that has a run-time overhead must be explicitly requested.
* No concept of "system of measurement".
        - Allow mixing between different systems: `feet * meters` is perfectly fine.
        - Multiplication and division never perform any conversion.
        - If the result of mult/div is dimensionless, it is always returned as the underlying type.

### Basic usage

The library is entirely header-only, no compilation is required; to pull the library in your code, you can start by #including `<engineering_units/si.hpp>`. This file provides definition for all the units in the international system.

```cpp
    #include <engineering_units/si.hpp>
    
    namespace si = engunits::si; // Alias the SI namespace, for convenience
    using namespace si::literals; // Pulls the user-defined literal in the current scope.
    
    auto two_meters = 1.0_m + 1.0_m; //ok
```

Should you make a mistake, and add units that are not compatible:

```cpp
    auto wrong = 1.0_m + 1.0_J; // bad: adding meters and joule.
```

You will get a relatively short compiler error message:

     In file included from ./include/engineering_units/si.hpp:30:0,
                 from test.cpp:1:
    ./include/engineering_units/quantity.hpp: In instantiation of ‘constexpr auto engunits::operator+ ... `
    test.cpp:8:18:   required from here
    ./include/engineering_units/quantity.hpp:502:5: error: static assertion failed: operator+ with different units

that points to the exact line in your code where the error happened.

---

Quantities are stored in the `quantity` class template, so the previous example can also be written as:

```cpp
    quantity<double, si::meter> two_meters = 1.0_m + 1.0_m;
```

What if we need meters squared? We can simply write:

```cpp
    quantity<double, si::meter_<2> > area = 1.0_m  * 2.0_m;
```

Notice the underscore at the end of `si::meter_`? It is needed to distinguish between the class template `si::meter_<Exp>` and `si::meter`, which is simply an alias for `si::meter_<1>`.

---

Not all the units have an associated string literal, the general way of generating a quantity from a integral type
is to multiply the type itself by the unit:

```cpp
     quantity<double, si::meter_<2> > area = 3.0 * si::meter_<2>(); 
```

Rational exponents are also supported, so you can write:

```cpp
    auto steel_fracture_toughness = 50.0_MPa * si::meter_<1,2>(); // Megapascal * sqrt(m)
```

What about derived units? Actually `quantity` accepts a list of units after the underlying type, so you can have:

```cpp
    using second_ = engunits::second_;
    quantity<double, si::meter, second_<-1> > velocity = delta_x / 0.1_s;
```

### Conversions

Beside dimensional analysis, this library also provides units conversion. By design all the unit conversion must be explicit,
in order to signal that this is actually a non-zero-overhead operation (we have to mulitply by a conversion factor).

```cpp
    quantity<double, si::millimeter> length( 4.0_m ); // ok, convert from meter to millimeter.
    quantity<double, si::millimeter> x = 4.0_m; //bad, no implicit conversion allowed!
```

Explicit conversion can also be obtained with `quantity_cast`:

```cpp    
    quantity<double, si::millimeter> x = quantity_cast<si::millimeter>(4.0_m); //ok, convert to millimeter
```

Ratios of convertible units are not automatically expanded:

```cpp     
     // decltype(x) is quantity<double, si::meter, si::millimeter_<-1> > !
     auto x = 1.0_m / (1.0 * si::millimeter());
     
     // Can use quantity_cast without template arguments to get a real number.
     double y = quantity_cast(x); // Multiply by conversion factor.
```

When computing the ratio between the same unit, the underlying type is automatically unwrapped:

```cpp
     quantity<double, si::meter> x = 3.0_m;
     auto y = x / x; // decltype(y) is double
```

Conversion factors for derived units are automatically computed from the conversion factor of the base units,
which allows for arbitrary complex conversions.

```cpp
     // Convert feet to meters, seconds to minutes, pounds^-1 to kilogram^-1.
     // The resulting conversion_factor is a compile-time constant, this line only performs one multiplication!
     quantity<double, meter, minute, kilogram_<-1> > x ( 3.0_ft * 12.0_s / 14.0_lb ); 
```

### Math functions

Most of the functions from `<cmath>` are overloaded in this library to provide transparent usage. The definition is inside the `engunits` namespace, but you can rely on argument-dependent-lookup to pick the right function.

```cpp
    auto length = sqrt(4.0 * si::meter_<2>() ); // length is meter.
    auto length2 = cbrt(4.0 * si::meter_<3>() ); // length is meter.
    auto hyp = hypoth( 3.0_m, 4.0_m ); // hyp = 5.0_m;
    auto hyp2 = hypoth( 3.0_m, 4.0 * si::millimeter() ); // error, need an explicit conversion
```

Trigoniometric functions are also provided:

```cpp 
    double x = tan( 45.0_deg ); // x == 1
    double y = sin( 3.141592_rad ); // y == 0
```

### Define custom units

Should you need an unit that is not provided directly by this library, some helper macros are provided:

```cpp
    #include <engunits/unit/helper_macros.hpp>
    
    // Define a root unit, with a tag to keep track of the physical quantity it refers to.
    // meter, second, kilogram, etc. are all root units.
    ENGUNITS_DEFINE_ROOT_UNIT(candela, cd, luminous_intensity);
    
    // Define a unit that is simply a multiple of another one
    ENGUNITS_DEFINE_BASE_UNIT(millicandela, mcd, 0.001L ); // 1 mcd = 0.001 cd
    
    // Attach a name to a derived unit.
    ENGUNITS_DEFINE_DERIVED_UNIT(newton, N, meter, kilogram, second_<-2> ); // newton = meter * kilogram / second^2
    
    // Import the unit operators (comparison, multiplication) in the current scope
    // This consist of some `using` declaration, and it is needed if you are defining the new unit in your own namespace.
    ENGUNITS_IMPORT_OPERATORS
```
