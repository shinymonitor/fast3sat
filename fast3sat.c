#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
//======================CONFIG========================
#define M 84
#define N 20
#define TESTS 1000

#define MAX_RETRIES 10   // number of solver retries with randomized selection
#define TOP_K 3          // pick randomly among top-K candidates each retry
//=======================TYPES=========================
typedef struct {bool solved, answer; size_t pos_count, neg_count;} Variable;
typedef struct {Variable* variables;} VariableList;
typedef struct {bool negation; size_t variable_number;} Term;
typedef struct {bool solved; Term terms[3];} Clause;
typedef struct {Clause* clauses;} Statement;
//======================SOLVER========================
static inline bool evaluate(Statement* statement, VariableList* variable_list, size_t m){
    for (size_t i=0; i<m; ++i){
        bool satisfied=false;
        for (uint8_t j=0; j<3; ++j){
            if (statement->clauses[i].terms[j].negation) satisfied|=!(variable_list->variables[statement->clauses[i].terms[j].variable_number].answer);
            else satisfied|=variable_list->variables[statement->clauses[i].terms[j].variable_number].answer;
        }
        if (!satisfied) return false;
    }
    return true;
}
static inline bool fast3sat_pass(Statement* statement, VariableList* variable_list, size_t m, size_t n){
    for (size_t i=0; i<n; ++i) variable_list->variables[i]=(Variable){0};
    for (size_t i=0; i<m; ++i) statement->clauses[i].solved=false;
    for (size_t i=0; i<n; ++i) {
        for (size_t j=0; j<m; ++j){
            for (uint8_t k=0; k<3; ++k) if ((statement->clauses[j].terms[k].variable_number==i) && !(statement->clauses[j].terms[k].negation)) {++variable_list->variables[i].pos_count; break;}
            for (uint8_t k=0; k<3; ++k) if ((statement->clauses[j].terms[k].variable_number==i) && (statement->clauses[j].terms[k].negation)) {++variable_list->variables[i].neg_count; break;}
        }
    }
    size_t solved_clauses=0;
    size_t pc, nc;
    while (solved_clauses!=m){
        Term term={0};
        size_t candidate_variables[TOP_K]={0};
        size_t weakest_candidate_index=0;
        size_t weakest_candidate_count=0;
        size_t candidate_variables_size = 0;
        for (size_t i = 0; i < n; ++i) {
            if (variable_list->variables[i].solved) continue;
            pc = variable_list->variables[i].pos_count; nc = variable_list->variables[i].neg_count;
            if (candidate_variables_size < TOP_K) {
                candidate_variables[candidate_variables_size++] = i;
                weakest_candidate_count = SIZE_MAX;
                for (size_t j = 0; j < candidate_variables_size; ++j) {
                    pc = variable_list->variables[candidate_variables[j]].pos_count; nc = variable_list->variables[candidate_variables[j]].neg_count;
                    if ((pc>nc?pc:nc) < weakest_candidate_count) {weakest_candidate_count = pc>nc?pc:nc; weakest_candidate_index=j;}
                }
            } 
            else if ((pc>nc?pc:nc) > weakest_candidate_count) {
                candidate_variables[weakest_candidate_index] = i;
                weakest_candidate_count = SIZE_MAX;
                for (size_t j = 0; j < candidate_variables_size; ++j) {
                    pc = variable_list->variables[candidate_variables[j]].pos_count; nc = variable_list->variables[candidate_variables[j]].neg_count;
                    if ((pc>nc?pc:nc) < weakest_candidate_count) {weakest_candidate_count = pc>nc?pc:nc; weakest_candidate_index = j;}
                }
            }
        }
        if (candidate_variables_size == 0) break;
        size_t chosen = rand() % candidate_variables_size;
        term.variable_number = candidate_variables[chosen];
        pc = variable_list->variables[term.variable_number].pos_count; nc = variable_list->variables[term.variable_number].neg_count;
        term.negation = nc>pc;
        if ((pc>nc?pc:nc)==0) break;
        variable_list->variables[term.variable_number].answer=!term.negation;
        variable_list->variables[term.variable_number].solved=true;
        for (size_t i=0; i<m; ++i){
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
    }
    return evaluate(statement, variable_list, m);
}
static inline bool fast3sat_solver(Statement* statement, VariableList* variable_list, size_t m, size_t n){
    for (int r=0; r<MAX_RETRIES; ++r) if (fast3sat_pass(statement, variable_list, m, n)) return true;
    return false;
}
//======================HELPERS========================
static bool* planted;
static inline void generate_planted_formula(Statement* statement, size_t m, size_t n) {
    for (size_t i=0; i<n; ++i) planted[i]=rand()&1;
    for (size_t i=0; i<m; ++i){
        while (true) {
            bool sat=false;
            for (uint8_t j=0; j<3; ++j){
                statement->clauses[i].terms[j].negation=rand()&1;
                statement->clauses[i].terms[j].variable_number=rand()%n;
            }
            for (uint8_t j=0; j<3; ++j){
                size_t v=statement->clauses[i].terms[j].variable_number;
                bool neg=statement->clauses[i].terms[j].negation;
                if (planted[v]^neg){sat=true; break;}
            }
            if (sat) break;
        }
    }
}
static inline void generate_random_formula(Statement* statement, size_t m, size_t n) {
    for (size_t i = 0; i < m; ++i) {
        for (uint8_t j = 0; j < 3; ++j) {
            statement->clauses[i].terms[j].negation=rand()&1;
            statement->clauses[i].terms[j].variable_number=rand()%n;
        }
    }
}
//======================TESTS========================
int main() {
    clock_t c1, c2;
    size_t planted_count=0, random_count=0;
    float planted_time=0, random_time=0;
    Statement statement = {0};
    VariableList variable_list = {0};
    srand(time(0));
    statement.clauses=(Clause*)malloc(sizeof(Clause)*M);
    variable_list.variables=(Variable*)malloc(sizeof(Variable)*N);
    planted=(bool*)malloc(sizeof(bool)*N);
    for (size_t t=0; t<TESTS; ++t){
        //======================PROGRESS PRINT========================
        printf("\x1b[2J\x1b[H");
        printf("[");
        for (size_t j=0; j<((t+1)*100/TESTS); ++j) printf("*"); 
        for (size_t j=0; j<(100-((t+1)*100/TESTS)); ++j) printf(" ");
        printf("] %zu%%", ((t+1)*100/TESTS));
        printf(" (TEST #%zu/%d)\n", t+1, TESTS);
        //============================================================
        generate_planted_formula(&statement, M, N);
        c1=clock();
        if (fast3sat_solver(&statement, &variable_list, M, N)) ++planted_count;
        c2=clock();
        planted_time+=(float)(c2-c1)/CLOCKS_PER_SEC;
        generate_random_formula(&statement, M, N);
        c1=clock();
        if (fast3sat_solver(&statement, &variable_list, M, N)) ++random_count;
        c2=clock();
        random_time+=(float)(c2-c1)/CLOCKS_PER_SEC;
    }
    for (int i=0;i<60;++i) {printf("=");} printf("\n");
    printf("Benchmark  M=%-4d N=%-4d M/N=%.2f  RETRIES=%d TOP_K=%d\n", M, N, (float)M/N, MAX_RETRIES, TOP_K);
    for (int i=0;i<60;++i) {printf("=");} printf("\n");
    printf("Planted:  %zu/%d (%.1f%%)  avg %.6f secs\n", planted_count, TESTS, 100.0*planted_count/TESTS, planted_time/TESTS);
    printf("Random:   %zu/%d (%.1f%%)  avg %.6f secs\n", random_count,  TESTS, 100.0*random_count /TESTS, random_time /TESTS);
    printf("\nPlanted = known-SAT ground truth (use for accuracy benchmarks).\n");
    printf("Random  = mix of SAT/UNSAT (at M/N=4.2 approx 50%% are truly SAT).\n");
    for (int i=0;i<60;++i) {printf("=");} printf("\n");
    free(statement.clauses); free(variable_list.variables); free(planted);
    return 0;

}
