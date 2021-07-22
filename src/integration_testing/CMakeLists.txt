file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend_pkb/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/pkb_pql/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/pkb_pql/evaluator_pattern_clauses/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/pkb_pql/evaluator_suchthat_clauses/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/pkb_pql/evaluator_with_clauses/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/pkb_pql/utils/*.cpp"  )
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(integration_testing ${srcs})

target_link_libraries(integration_testing spa)
