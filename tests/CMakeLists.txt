
enable_testing()

## allunits
add_executable( allunits_test allunits.cpp )
target_link_libraries( allunits_test engineering_units )

add_test( NAME allunits_test COMMAND quantity_test )

## quantity
add_executable( quantity_test quantity.cpp )
target_link_libraries( quantity_test engineering_units )

add_test( NAME quantity_test COMMAND quantity_test )

### detail

## constexpr_pow
add_executable( constexpr_pow_test detail/constexpr_pow.cpp )
target_link_libraries( constexpr_pow_test engineering_units )

add_test( NAME constexpr_pow_test COMMAND constexpr_pow_test )

## contains
add_executable( contains_test detail/contains.cpp )
target_link_libraries( contains_test engineering_units )

add_test( NAME contains_test COMMAND contains_test )

### unit
## base_conversion
add_executable( base_conversion_test unit/base_conversion.cpp )
target_link_libraries( base_conversion_test engineering_units )

add_test( NAME base_conversion_test COMMAND base_conversion_test )

## conversion
add_executable( conversion_test unit/conversion.cpp )
target_link_libraries( conversion_test engineering_units )

add_test( NAME conversion_test COMMAND conversion_test )

## equality
add_executable( equality_test unit/equality.cpp )
target_link_libraries( equality_test engineering_units )

add_executable( equality_test_with_ns unit/equality.cpp )
target_link_libraries( equality_test_with_ns engineering_units )
target_compile_definitions( equality_test_with_ns PRIVATE "ENGUNITS_WITH_USING_NAMESPACE")

add_test( NAME equality_test         COMMAND equality_test )
add_test( NAME equality_test_with_ns COMMAND equality_test_with_ns )

## multiply
add_executable( multiply_test unit/multiply.cpp )
target_link_libraries( multiply_test engineering_units )

add_executable( multiply_test_with_ns unit/multiply.cpp )
target_link_libraries( multiply_test_with_ns engineering_units )
target_compile_definitions( multiply_test_with_ns PRIVATE "ENGUNITS_WITH_USING_NAMESPACE")

add_test( NAME multiply_test         COMMAND multiply_test )
add_test( NAME multiply_test_with_ns COMMAND multiply_test_with_ns )
