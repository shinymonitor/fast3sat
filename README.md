## Fast3SAT
A fast, purely deterministic solver for a subset of 3SAT problems. 

This algorithm uses frequency-based greedy variable assignment to find satisfying solutions in polynomial time. 
While it cannot solve all 3SAT problems (NP-complete), it performs very well on under-constrained or imbalanced instances.
It can solve most problems in milliseconds with a high success rate.

It can be used to attempt solving 3SAT problems before passing them to more complex algorithms like DPLL or WalkSAT if needed.
