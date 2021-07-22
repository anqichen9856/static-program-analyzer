#!/bin/bash

cd /Users/Rachel/Desktop/Documents/School/team21-cp-spa-20s1/Team21/Code21/cmake-build-debug/src/autotester
./autotester ../../../tests/Iteration_1/sources/1.no_clause_source.txt ../../../tests/Iteration_1/queries/1.no_clause_queries.txt ../../../tests/Iteration_1/out/1.out_no_clause.xml
./autotester ../../../tests/Iteration_1/sources/2.Follows_Parent_source.txt ../../../tests/Iteration_1/queries/2.Follows_Parent_queries.txt ../../../tests/Iteration_1/out/2.out_Follows_Parent.xml
./autotester ../../../tests/Iteration_1/sources/3.pattern_Modifies_Uses_source.txt ../../../tests/Iteration_1/queries/3.pattern_Modifies_Uses_queries.txt ../../../tests/Iteration_1/out/3.out_pattern_ModifiesS_UsesS.xml
./autotester ../../../tests/Iteration_1/sources/4.pattern_suchthat_clauses_source.txt ../../../tests/Iteration_1/queries/4.pattern_suchthat_clauses_queries.txt ../../../tests/Iteration_1/out/4.out_pattern_suchthat_clauses.xml
./autotester ../../../tests/Iteration_1/sources/5.complex_condexpr_nested_source.txt ../../../tests/Iteration_1/queries/5.complex_condexpr_nested_queries.txt ../../../tests/Iteration_1/out/5.out_complex_condexpr_nested.xml
./autotester ../../../tests/Iteration_1/sources/6.uncommon_invalid_source.txt ../../../tests/Iteration_1/queries/6.uncommon_invalid_queries.txt ../../../tests/Iteration_1/out/6.out_uncommon_invalid.xml
./autotester ../../../tests/Iteration_1/sources/7.bonus_3_features_source.txt ../../../tests/Iteration_1/queries/7.bonus_3_features_queries.txt ../../../tests/Iteration_1/out/7.out_bonus_3_features.xml
./autotester ../../../tests/Iteration_1/sources/8.invalid_test_source.txt ../../../tests/Iteration_1/queries/7.bonus_3_features_queries.txt ../../../tests/Iteration_1/out/8.out_no_out_file.xml