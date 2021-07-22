#!/bin/bash

cd /Users/Rachel/Desktop/Documents/School/team21-cp-spa-20s1/Team21/Code21/cmake-build-debug/src/autotester

# Simple Affects, source and query focused
./autotester ../../../tests/Iteration_3/sources/valid/1.simple_affects_source.txt ../../../tests/Iteration_3/queries/valid/1.valid_simple_affects_queries.txt ../../../tests/Iteration_3/out/valid/1.out_valid_simple_affects.xml
./autotester ../../../tests/Iteration_3/sources/valid/1.simple_affects_source.txt ../../../tests/Iteration_3/queries/invalid/1.invalid_simple_affects_queries.txt ../../../tests/Iteration_3/out/invalid/1.out_invalid_simple_affects.xml

# Simple Affects*, source and query focused
./autotester ../../../tests/Iteration_3/sources/valid/2.simple_affectsStar_source.txt ../../../tests/Iteration_3/queries/valid/2.valid_simple_affectsStar_queries.txt ../../../tests/Iteration_3/out/valid/2.out_valid_simple_affectsStar.xml
./autotester ../../../tests/Iteration_3/sources/valid/2.simple_affectsStar_source.txt ../../../tests/Iteration_3/queries/invalid/2.invalid_simple_affectsStar_queries.txt ../../../tests/Iteration_3/out/invalid/2.out_invalid_simple_affectsStar.xml

# Affects, more complex source design
./autotester ../../../tests/Iteration_3/sources/valid/3.complex_affects_source.txt ../../../tests/Iteration_3/queries/valid/3a.complex_affects_queries.txt ../../../tests/Iteration_3/out/valid/3a.out_valid_complex_affects.xml
./autotester ../../../tests/Iteration_3/sources/valid/3.complex_affects_source.txt ../../../tests/Iteration_3/queries/invalid/3a.invalid_complex_affects_queries.txt ../../../tests/Iteration_3/out/invalid/3a.out_invalid_complex_affects.xml
./autotester ../../../tests/Iteration_3/sources/valid/3.complex_affects_source.txt ../../../tests/Iteration_3/queries/valid/3b.complex_affects_star_queries.txt ../../../tests/Iteration_3/out/valid/3b.out_valid_complex_affects_star.xml
./autotester ../../../tests/Iteration_3/sources/valid/3.complex_affects_source.txt ../../../tests/Iteration_3/queries/invalid/3b.invalid_complex_affects_star_queries.txt ../../../tests/Iteration_3/out/invalid/3b.out_invalid_complex_affects_star.xml

# Multiclauses
./autotester ../../../tests/Iteration_2/sources/valid/7.multiclauses_source1.txt ../../../tests/Iteration_3/queries/valid/4.multiclauses_queries1.txt ../../../tests/Iteration_3/out/valid/4.out_multiclauses1.xml
./autotester ../../../tests/Iteration_3/sources/valid/5.multiclauses_source3.txt ../../../tests/Iteration_3/queries/valid/5.valid_multiclause_c1_queries.txt ../../../tests/Iteration_3/out/valid/5.out_valid_multiclause_c1.xml
./autotester ../../../tests/Iteration_3/sources/valid/6.multiclauses_source2.txt ../../../tests/Iteration_3/queries/valid/6.valid_multiclause_c2_queries.txt ../../../tests/Iteration_3/out/valid/6.out_valid_multiclause_c2.xml
./autotester ../../../tests/Iteration_3/sources/valid/5.multiclauses_source3.txt ../../../tests/Iteration_3/queries/valid/7.valid_multiclause_c3_queries.txt ../../../tests/Iteration_3/out/valid/7.out_valid_multiclause_c3.xml
./autotester ../../../tests/Iteration_3/sources/valid/5.multiclauses_source3.txt ../../../tests/Iteration_3/queries/valid/8.valid_multiclause_c4_queries.txt ../../../tests/Iteration_3/out/valid/8.out_valid_multiclause_c4.xml

# Invalid multiclauses queries
./autotester ../../../tests/Iteration_3/sources/valid/5.multiclauses_source3.txt ../../../tests/Iteration_3/queries/invalid/4.invalid_multiclause_queries.txt ../../../tests/Iteration_3/out/invalid/4.out_invalid_multiclause.xml

# Stress test
./autotester ../../../tests/Iteration_3/sources/valid/9.multiclauses_stress_test_source.txt ../../../tests/Iteration_3/queries/valid/9.valid_multiclauses_stress_test_queries.txt ../../../tests/Iteration_3/out/valid/9.out_multiclause_stress_test.xml
./autotester ../../../tests/Iteration_3/sources/valid/10.nested_stress_test_source.txt ../../../tests/Iteration_3/queries/valid/10.nested_stress_test_queries.txt ../../../tests/Iteration_3/out/valid/10.out_nested_stress_test.xml
