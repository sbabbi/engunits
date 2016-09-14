
enable_testing()

add_executable( contains_test detail/contains.cpp )
target_link_libraries( contains_test engineering_units )

add_test( NAME contains COMMAND contains_test )

add_executable( multiply_test unit/multiply.cpp )
target_link_libraries( multiply_test engineering_units )


add_executable( multiply_test_with_ns unit/multiply.cpp )
target_link_libraries( multiply_test_with_ns engineering_units )
target_compile_definitions( multiply_test_with_ns PRIVATE "ENGUNITS_WITH_USING_NAMESPACE")

add_test( NAME multiply_test         COMMAND multiply_test )
add_test( NAME multiply_test_with_ns COMMAND multiply_test_with_ns )



add_executable( mixed_unit_test unit/mixed_unit.cpp )
target_link_libraries( mixed_unit_test engineering_units )

add_test( NAME mixed_unit COMMAND mixed_unit_test )

