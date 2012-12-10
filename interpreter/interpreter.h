# include "../parser/parser.c"

Value* apply(Value* f, ConsCell* args, Environment* env);

Environment* combineEnvs(Environment first, Environment second);

ConsCell* evalEach(ConsCell* exprList, Environment* env);

Value* eval(Value* expr, Environment* env);

Environment* define(ConsCell* cell, Environment* env);
Environment* setBang(ConsCell* cell, Environment* env);

Value* let(ConsCell* cell, Environment* env);
Value* letStar(ConsCell* cell, Environment* env);
Value* letrec(ConsCell* cell, Environment* env);
Value* begin(ConsCell* cell, Environment* env);


Environment* createTopFrame();

void bind(char* symbol, Value* val, Environment* env);

Value* makePrimitiveValue(Value* (*primitiveValue)(ConsCell* actuals, Environment* env));

// input: symbol and an environment
// output: the symbol's bound value inside the environment env
Value* resolveVariable(char* symbol, Environment* env);


Value* makeClosure(ConsCell* cell, Environment* env);

Value* arbitraryFunction(ConsCell* args, Environment* env);

Value* cond(ConsCell* actuals, Environment* env);
Value* load(ConsCell* actuals, Environment* env);
Value* add(ConsCell* actuals, Environment* env);
Value* subtract(ConsCell* actuals, Environment* env);
Value* multiply(ConsCell* actuals, Environment* env);
Value* divide(ConsCell* actuals, Environment* env);
Value* equals(ConsCell* actuals, Environment* env);
Value* and(ConsCell* actuals, Environment* env);
Value* or(ConsCell* actuals, Environment* env);
Value* not(ConsCell* actuals, Environment* env);
Value* isNull(ConsCell* actuals, Environment* env);
Value* isList(ConsCell* actuals, Environment* env);
Value* lessThan(ConsCell* actuals, Environment* env);
Value* greaterThan(ConsCell* actuals, Environment* env);
Value* greaterThanEqual(ConsCell* actuals, Environment* env);
Value* lessThanEqual(ConsCell* actuals, Environment* env);
Value* display(ConsCell* actuals, Environment* env);
Value* car(ConsCell* actuals, Environment* env);
Value* cdr(ConsCell* actuals, Environment* env);
Value* cons(ConsCell* actuals, Environment* env);
Value* list(ConsCell* actuals, Environment* env);
Value* append(ConsCell* actuals, Environment* env);
Value* quote(ConsCell* actuals, Environment* env);





int main(int argc, char *argv[]);
