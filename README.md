<div align="center">
    <img src="assets/LOGO.png", width="200"/>
    <h1>FAST3SAT</h1>
</div>

A fast, incomplete solver for a subset of 3SAT problems.

The algorithm combines frequency-based greedy variable assignment with random restarts, making it most closely related to [GSAT](https://cdn.aaai.org/AAAI/1992/AAAI92-068.pdf) (Selman et al., 1992). The differences from GSAT are:

- Greedy construction instead of hill-climbing on a complete assignment: variables are assigned one at a time based on frequency counts rather than flipping variables in a complete assignment.
- Random restarts over top-K: each retry independently picks from the highest-scoring K unsolved variables at random, breaking the deterministic dead-end of pure greedy without the overhead of GSAT's full flip evaluation.

Like GSAT, this is an incomplete heuristic. It cannot prove unsatisfiability and may fail on satisfiable instances.

## Benchmark Results
| M/N Ratio | Success Rate (Benchmarked at) | Use Case                   |
|-----------|-------------------------------|----------------------------|
| 1.0       | ~97% (M=80, N=80, T=10, K=3)  | Under-constrained          |
| 2.0       | ~90% (M=80, N=40, T=10, K=3)  | Typical instances          |
| 4.2       | ~60% (M=84, N=20, T=10, K=3)  | Phase transition (hardest) |
| 10.0+     | Variable                      | Over-constrained           |

Benchmarks use planted-solution instances. This gives a accuracy measurement against known-SAT ground truth without requiring brute force.

## Performance
- Time Complexity: O(M*N) per pass, O(retries\*M\*N)
- Memory: O(M + N) space complexity
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
- Incomplete: cannot solve all satisfiable instances and cannot prove unsatisfiability
- No backtracking: may commit to an assignment that makes later clauses unsatisfiable
- No clause learning: does not derive new constraints from conflicts (unlike CDCL solvers)
