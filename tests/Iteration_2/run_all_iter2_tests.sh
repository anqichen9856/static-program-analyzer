#!/bin/bash

cd /Users/Rachel/Desktop/Documents/School/team21-cp-spa-20s1/Team21/Code21/cmake-build-debug/src/autotester

# simple Select, no clause focused
./autotester ../../../tests/Iteration_2/sources/valid/1.Select_source.txt ../../../tests/Iteration_2/queries/valid/1.valid_Select_queries.txt ../../../tests/Iteration_2/out/valid/1.out_valid_Select_clause.xml
./autotester ../../../tests/Iteration_2/sources/valid/1.Select_source.txt ../../../tests/Iteration_2/queries/invalid/1.invalid_Select_queries.txt ../../../tests/Iteration_2/out/invalid/1.out_invalid_Select_clause.xml

# simple With clause focused
./autotester ../../../tests/Iteration_2/sources/valid/2.With_clause_source.txt ../../../tests/Iteration_2/queries/valid/2.valid_With_clause_queries.txt ../../../tests/Iteration_2/out/valid/2.out_valid_With_clause.xml
./autotester ../../../tests/Iteration_2/sources/valid/2.With_clause_source.txt ../../../tests/Iteration_2/queries/invalid/2.invalid_With_clause_queries.txt ../../../tests/Iteration_2/out/invalid/2.out_invalid_With_clause.xml

# simple Calls, Calls* focused
./autotester ../../../tests/Iteration_2/sources/valid/3.simple_Calls_source.txt ../../../tests/Iteration_2/queries/valid/3.simple_Calls_queries.txt ../../../tests/Iteration_2/out/valid/3.out_simple_Calls.xml

./autotester ../../../tests/Iteration_2/sources/valid/4.Calls_Next_ModifiesP_UsesP_source.txt ../../../tests/Iteration_2/queries/valid/4a.valid_Calls_queries.txt ../../../tests/Iteration_2/out/valid/4a.out_valid_Calls.xml
./autotester ../../../tests/Iteration_2/sources/valid/4.Calls_Next_ModifiesP_UsesP_source.txt ../../../tests/Iteration_2/queries/invalid/4a.invalid_Calls_queries.txt ../../../tests/Iteration_2/out/invalid/4a.out_invalid_Calls.xml

# simple Next, Next* focused
./autotester ../../../tests/Iteration_2/sources/valid/4.Calls_Next_ModifiesP_UsesP_source.txt ../../../tests/Iteration_2/queries/valid/4b.valid_Next_queries.txt ../../../tests/Iteration_2/out/valid/4b.out_valid_Next.xml
./autotester ../../../tests/Iteration_2/sources/valid/4.Calls_Next_ModifiesP_UsesP_source.txt ../../../tests/Iteration_2/queries/invalid/4b.invalid_Next_queries.txt ../../../tests/Iteration_2/out/invalid/4b.out_invalid_Next.xml

# ModifiesP, UsesP, correct the mistakes
./autotester ../../../tests/Iteration_2/sources/valid/4.Calls_Next_ModifiesP_UsesP_source.txt ../../../tests/Iteration_2/queries/valid/4c.valid_ModifiesP_UsesP_queries.txt ../../../tests/Iteration_2/out/valid/4c.out_valid_ModifiesP_UsesP.xml
./autotester ../../../tests/Iteration_2/sources/valid/4.Calls_Next_ModifiesP_UsesP_source.txt ../../../tests/Iteration_2/queries/invalid/4c.invalid_ModifiesP_UsesP_queries.txt ../../../tests/Iteration_2/out/invalid/4c.out_invalid_ModifiesP_UsesP.xml

# simple pattern assign focused
./autotester ../../../tests/Iteration_2/sources/valid/5.pattern_assign_source.txt ../../../tests/Iteration_2/queries/valid/5.valid_pattern_assign_queries.txt ../../../tests/Iteration_2/out/valid/5.out_valid_pattern_assign.xml
./autotester ../../../tests/Iteration_2/sources/valid/5.pattern_assign_source.txt ../../../tests/Iteration_2/queries/invalid/5.invalid_pattern_assign_queries.txt ../../../tests/Iteration_2/out/invalid/5.out_invalid_pattern_assign.xml

# pattern while, ifs and assign
./autotester ../../../tests/Iteration_2/sources/valid/6.pattern_full_spec_source.txt ../../../tests/Iteration_2/queries/valid/6.valid_pattern_full_spec_queries.txt ../../../tests/Iteration_2/out/valid/6.out_valid_pattern_full_spec.xml
./autotester ../../../tests/Iteration_2/sources/valid/6.pattern_full_spec_source.txt ../../../tests/Iteration_2/queries/invalid/6.invalid_pattern_full_spec_queries.txt ../../../tests/Iteration_2/out/invalid/6.out_invalid_pattern_full_spec.xml

# multi-clauses
./autotester ../../../tests/Iteration_2/sources/valid/7.multiclauses_source1.txt ../../../tests/Iteration_2/queries/valid/7.valid_multiclauses_queries1.txt ../../../tests/Iteration_2/out/valid/7.out_multiclauses1.xml

# Invalid source, should exit the program
./autotester ../../../tests/Iteration_2/sources/invalid/invalid_source_call_missing_procedure.txt ../../../tests/Iteration_2/queries/valid/7.multiclauses_queries1.txt ../../../tests/Iteration_2/out/invalid/out_invalid_source.xml
./autotester ../../../tests/Iteration_2/sources/invalid/invalid_source_cyclic_links.txt ../../../tests/Iteration_2/queries/valid/7.multiclauses_queries1.txt ../../../tests/Iteration_2/out/invalid/out_invalid_source.xml