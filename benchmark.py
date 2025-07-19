from random import choice
from itertools import product
from time import time
from solver import evaluate, solve


M = 20
N = 10
TESTS= 100

#======================RUN TESTS=========================
faster_count = 0
failure_count=0
sat_count=0
solved_count=0

variable_names = ["x" + str(i + 1) for i in range(N)]

for t in range(TESTS):
    print(f"========== TEST {t + 1} ==========")
    #=====GENERATE STATEMENT======
    _statement = []
    for _ in range(M):
        clause = []
        for _ in range(3):
            var = choice(variable_names)
            neg = choice(["", "-"])
            clause.append(neg + var)
        _statement.append(clause)
    _variables = {v: [0, 0, 0] for v in variable_names}
    #=====SOLVE======
    start = time()
    result = solve(_statement, _variables)
    solver_time = time() - start
    found = result is not False
    print("SOLVED" if found else "NOT SOLVED")
    print("SOLVER TIME: ", solver_time)
    #=====BRUTE FORCE======
    start = time()
    sat = False
    variable_list=[]
    for clause in _statement:
        for var in clause:
            if var[0]=="-": variable_list.append(var[1:])
            else: variable_list.append(var)
    for bits in product([0, 1], repeat=N):
        trial_vars = {v: [0, 0, bit] for v, bit in zip(set(variable_list), bits)}
        if evaluate(_statement, trial_vars):
            sat = True
            break
    force_time = time() - start
    print("SATISFIED" if sat else "UNSATISFIED")
    print("BRUTE FORCE TIME: ", force_time)
    #=====COMPARE======
    if (found and sat) and solver_time<force_time:
        faster_count += 1
        print("$ FASTER $")
    elif found == sat:
        print("SUCCESS")
    else:
        failure_count += 1
        print("! FAILED !")
    if sat:
        sat_count+=1
    if found:
        solved_count+=1

#======================RESULT=========================
print(f"{M} CLAUSES, {N} VARIABLES")
print("NO KNOWLEDGE SUCCESS RATE:", solved_count / TESTS)
print("ACTUAL SUCCESS RATE:", solved_count / sat_count)
