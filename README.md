# FAST3SAT
A fast, purely deterministic solver for a subset of 3SAT problems. 
This algorithm uses frequency-based greedy variable assignment to find satisfying solutions in polynomial time. 

## Benchmark Results
| M/N Ratio | Success Rate | Use Case                   |
|-----------|--------------|----------------------------|
| 1.0       | ~99%         | Under-constrained          |
| 2.0       | ~83%         | Typical instances          |
| 4.2       | ~40%-50%     | Phase transition (hardest) |
| 10.0+     | Variable     | Over-constrained           |

## Performance
- Time Complexity: Polynomial time O(MN) vs exponential for complete solvers
- Success Rate: 40-99% depending on problem structure
- Speed: Solves most instances in microseconds
- Memory: O(M+N) space complexity
- Best Performance: Under-constrained instances (M/N < 3)

# Use Case
- Under-constrained problems (M/N < 3)
- Quick first-pass attempts
- Real-time applications requiring fast responses
- Preprocessing step before expensive solvers

# Not good for
- Problems requiring completeness guarantees
- Adversarially constructed instances
- Phase transition region problems (unless speed > accuracy)

# Limitations
- Incomplete: Cannot solve all satisfiable instances
- No backtracking: May get stuck in local optima
- No clause learning: Doesn't learn from conflicts