def evaluate(statement, variables):
    for clause in statement:
        satisfied = False
        for var in clause:
            if var[0] == "-":
                satisfied |= not variables[var[1:]][2]
            else:
                satisfied |= variables[var][2]
        if not satisfied:
            return False
    return True

def solve(statement, variables):
    count = 0
    var_list = list(variables.keys())
    statement_copy=statement[:]
    for var in variables:
        pos_count = 0
        neg_count = 0
        for clause in statement_copy:
            if var in clause: variables[var][0]+=1
            if "-"+var in clause: variables[var][1]+=1
    while var_list and statement_copy:
        term=""
        var=""
        max_clause=0
        for v in var_list:
            if variables[v][0]>max_clause:
                term=v
                var=v
                max_clause=variables[v][0]
            if variables[v][1]>max_clause:
                term="-"+v
                var=v
                max_clause=variables[v][1]
        for clause in statement_copy:
            if clause[0]==clause[1] and clause[1]==clause[2]:
                term=clause[0]
                if term[0]=="-": var=term[1:]
                else: var=term
        if term=="": break
        if term[0]!="-": variables[var][2]=1
        else:variables[var][2]=0
        for clause in statement_copy[:]:
            if term in clause:
                for v in set(clause):
                    if v[0] == "-":
                        variables[v[1:]][1] -= 1
                    else:
                        variables[v][0] -= 1
                statement_copy.remove(clause)
        for clause in statement_copy[:]:
            good = False
            for v in clause:
                if var in var_list:
                    good = True
                    break
            if not good:
                for v in clause:
                    if v[0] == "-":
                        variables[v[1:]][1] -= 1
                    else:
                        variables[v][0] -= 1
                statement_copy.remove(clause)
        var_list.remove(var)
    if evaluate(statement, variables):
        return variables
    else: return False
