#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define M 42
#define N 10
#define TESTS 10000

typedef struct {bool solved, answer; size_t pos_count, neg_count;} Variable;
typedef struct {Variable variables[N];} VariableList;
typedef struct {bool negation; size_t variable_number;} Term;
typedef struct {bool solved; Term terms[3];} Clause;
typedef struct {Clause clauses[M];} Statement;

static inline bool fast3sat_solver(Statement* statement, VariableList* variable_list){
    for (size_t i=0; i<N; ++i) {
        for (size_t j=0; j<M; ++j){
            for (uint8_t k=0; k<3; ++k) if ((statement->clauses[j].terms[k].variable_number==i) && !(statement->clauses[j].terms[k].negation)) {++variable_list->variables[i].pos_count; break;}
            for (uint8_t k=0; k<3; ++k) if ((statement->clauses[j].terms[k].variable_number==i) && (statement->clauses[j].terms[k].negation)) {++variable_list->variables[i].neg_count; break;}
        }
    }
    size_t solved_variables=0;
    size_t solved_clauses=0;
    while (solved_clauses!=M && solved_variables!=N){
        Term term={0};
        size_t max_count=0;
        for (size_t i=0; i<N; ++i) {
            if (variable_list->variables[i].solved) continue;
            if (variable_list->variables[i].pos_count>max_count){
                term.negation=false;
                term.variable_number=i;
                max_count=variable_list->variables[i].pos_count;
            }
            if (variable_list->variables[i].neg_count>max_count){
                term.negation=true;
                term.variable_number=i;
                max_count=variable_list->variables[i].neg_count;
            }
        }
        for (size_t i=0; i<M; ++i){
            if (!statement->clauses[i].solved && statement->clauses[i].terms[0].variable_number==statement->clauses[i].terms[1].variable_number && statement->clauses[i].terms[0].negation==statement->clauses[i].terms[1].negation && statement->clauses[i].terms[1].variable_number==statement->clauses[i].terms[2].variable_number && statement->clauses[i].terms[1].negation==statement->clauses[i].terms[2].negation){
                term.negation=statement->clauses[i].terms[0].negation;
                term.variable_number=statement->clauses[i].terms[0].variable_number;
                max_count=(variable_list->variables[statement->clauses[i].terms[0].variable_number].pos_count>variable_list->variables[statement->clauses[i].terms[0].variable_number].neg_count?variable_list->variables[statement->clauses[i].terms[0].variable_number].pos_count:variable_list->variables[statement->clauses[i].terms[0].variable_number].neg_count);
            }
        }
        if (max_count==0) break;
        if (term.negation) variable_list->variables[term.variable_number].answer=false;
        else variable_list->variables[term.variable_number].answer=true;
        variable_list->variables[term.variable_number].solved=true;
        for (size_t i=0; i<M; ++i){
            if (statement->clauses[i].solved) continue;
            if ((statement->clauses[i].terms[0].variable_number==term.variable_number && statement->clauses[i].terms[0].negation==term.negation) || (statement->clauses[i].terms[1].variable_number==term.variable_number && statement->clauses[i].terms[1].negation==term.negation) || (statement->clauses[i].terms[2].variable_number==term.variable_number && statement->clauses[i].terms[2].negation==term.negation)){
                if (!statement->clauses[i].terms[0].negation) --variable_list->variables[statement->clauses[i].terms[0].variable_number].pos_count;
                else --variable_list->variables[statement->clauses[i].terms[0].variable_number].neg_count;
                if (statement->clauses[i].terms[1].variable_number!=statement->clauses[i].terms[0].variable_number || statement->clauses[i].terms[1].negation!=statement->clauses[i].terms[0].negation){
                    if (!statement->clauses[i].terms[1].negation) --variable_list->variables[statement->clauses[i].terms[1].variable_number].pos_count;
                    else --variable_list->variables[statement->clauses[i].terms[1].variable_number].neg_count;
                }
                if ((statement->clauses[i].terms[2].variable_number!=statement->clauses[i].terms[0].variable_number || statement->clauses[i].terms[2].negation!=statement->clauses[i].terms[0].negation) && (statement->clauses[i].terms[2].variable_number!=statement->clauses[i].terms[1].variable_number || statement->clauses[i].terms[2].negation!=statement->clauses[i].terms[1].negation)){
                    if (!statement->clauses[i].terms[2].negation) --variable_list->variables[statement->clauses[i].terms[2].variable_number].pos_count;
                    else --variable_list->variables[statement->clauses[i].terms[2].variable_number].neg_count;
                }
                ++solved_clauses;
                statement->clauses[i].solved=true;
            }
        }
        ++solved_variables;
    }
    return evaluate(statement, variable_list);
}

static inline bool evaluate(Statement* statement, VariableList* variable_list){
    for (size_t i=0; i<M; ++i){
        bool satisfied=false;
        for (uint8_t j=0; j<3; ++j){
            if (statement->clauses[i].terms[j].negation) satisfied|=!(variable_list->variables[statement->clauses[i].terms[j].variable_number].answer);
            else satisfied|=variable_list->variables[statement->clauses[i].terms[j].variable_number].answer;
        }
        if (!satisfied) return false;
    }
    return true;
}

bool brute_force(Statement* statement, VariableList* variable_list) {
    for (size_t assignment = 0; assignment < (1U << N); ++assignment) {
        for (size_t i = 0; i < N; ++i) variable_list->variables[i].answer = (assignment >> i) & 1;
        if (evaluate(statement, variable_list)) return true;
    }
    return false;
}

void generate_random_formula(Statement* statement) {
    for (size_t i = 0; i < M; ++i) {
        for (uint8_t j = 0; j < 3; ++j) {
            statement->clauses[i].terms[j].negation = ((float)rand()/RAND_MAX) > 0.5f;
            statement->clauses[i].terms[j].variable_number = (rand() % N);
        }
    }
}

int main() {
    srand(time(0));
    size_t brute_force_count = 0;
    size_t solver_count = 0;
    for (size_t n=0; n<TESTS; ++n){
        Statement statement = {0};
        generate_random_formula(&statement);
        VariableList variable_list1 = {0};
        VariableList variable_list2 = {0};
        bool brute_force_result = brute_force(&statement, &variable_list1);
        if (brute_force_result) ++brute_force_count;
        bool solver_result = fast3sat_solver(&statement, &variable_list2);
        if (solver_result) ++solver_count;
    }
    printf("Results for %d random 3-SAT instances:\n", TESTS);
    printf("Brute Force: %zu/%d solved (%.1f%%)\n", brute_force_count, TESTS, 100.0 * brute_force_count / TESTS);
    printf("FAST3SAT: %zu/%d solved (%.1f%%)\n", solver_count, TESTS, 100.0 * solver_count / TESTS);
    printf("\nPerformance Analysis:\n");
    printf("FAST3SAT vs Truth:  %.1f%% efficiency\n",brute_force_count > 0 ? 100.0 * solver_count / brute_force_count : 0);
    return 0;
}