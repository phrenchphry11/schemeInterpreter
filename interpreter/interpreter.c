# include "interpreter.h"

Value* apply(Value* f, ConsCell* args, Environment* env) 
{
    if (f->type == 9)
    {
        return (*(f->uni.primitiveValue))(args, env);
    }

    else if (f->type == 10) 
    {
        Environment *env = f->uni.closureValue->env;
        ConsCell* formals = f->uni.closureValue->params->cdr->uni.cons;

        if (getLen(formals) != getLen(args))
        {
            printf("SYNTAX ERROR: PARAMS ARE NOT CORRECT.\n");
            exit(0);
        }

        // Make the new frame
        Frame* paramFrame = (Frame*)malloc(sizeof(Frame));
        assign(paramFrame); //POINTER LIST
        ConsCell* paramFrameHead = (ConsCell*)malloc(sizeof(ConsCell));
        assign(paramFrameHead); //POINTER LIST
        paramFrame->head  = paramFrameHead;

        // Make the new Value containing the new frame
        Value* frameVal = (Value*)malloc(sizeof(Value));
        assign(frameVal); //POINTER LIST
        frameVal->type = 12;
        frameVal->uni.frame = paramFrame;

        // Make the new Cons containing the new val+frame
        ConsCell* frameCons = (ConsCell*)malloc(sizeof(ConsCell));
        assign(frameCons); //POINTER LIST
        frameCons->car = frameVal;

        Value* frameConsCdrVal = (Value*)malloc(sizeof(Value));
        assign(frameConsCdrVal); //POINTER LIST
        frameConsCdrVal->type = 8;
        frameConsCdrVal->uni.cons = env->head;
        frameCons->cdr = frameConsCdrVal;

        Environment* paramEnv = (Environment*)malloc(sizeof(Environment));
        assign(paramEnv); //POINTER LIST
        paramEnv->head = frameCons;

        /* bind formals to actuals here */
        while(args != NULL && formals != NULL && args->car != NULL && formals->car != NULL && args->car->type != 7 && formals->car->type != 7)
        {
            if(formals->car->type != 4)
            {
                printf("ERROR: parameters must be symbols: %d\n", formals->car->type);
                exit(0);
            }
            bind(formals->car->uni.symbol, args->car, paramEnv);

            //now move to the next parameter
            if(args->cdr != NULL && formals->cdr != NULL)
            {
                args = args->cdr->uni.cons;
                formals = formals->cdr->uni.cons;
                if(args->car == NULL || formals->car == NULL)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        return (*(f->uni.closureValue->functionPtr))(f->uni.closureValue->toExecute, paramEnv);
    }
    else
    {
        printf("ERROR: %d\n", f->type);
        exit(0);
    }
}

int getLen(ConsCell* cell)
{
    int len = 0;

    while (cell != NULL && cell->car != NULL && cell->car->type != 7)
    {
        ++len;
        cell = cell->cdr->uni.cons;
    }

    return len;
}

ConsCell* evalEach(ConsCell* exprList, Environment* env)
{
    Value* expr = NULL;
    Value* result = NULL;
    ConsCell* evaluatedExprs = (ConsCell*)malloc(sizeof(ConsCell));
    assign(evaluatedExprs); //POINTER LIST
    ConsCell* currentExpr = exprList;
    ConsCell* currentRes = evaluatedExprs;

    while(currentExpr != NULL && currentExpr->car != NULL && currentExpr->car->type != 7)
    {
        expr = currentExpr->car;
        result = eval(expr, env);

        currentRes->car = result;
        ConsCell* next = (ConsCell*)malloc(sizeof(ConsCell));
        assign(next); //POINTER LIST
        currentRes->cdr = (Value*)malloc(sizeof(Value));
        assign(currentRes->cdr); //POINTER LIST
        currentRes->cdr->type = 8;
        currentRes->cdr->uni.cons = next;
        currentRes = next;
        if(currentExpr->cdr != NULL)
        {
            currentExpr = currentExpr->cdr->uni.cons;
        }
        else
        {
            break;
        }
    }

    return evaluatedExprs;
}

Value* eval(Value* expr, Environment* env)
{
    ConsCell* cell; 
    Closure* closure;
    Value* result;
    switch (expr->type)
    {
        case 0: // Bool
            return expr;
        case 1: // Int
            return expr;
        case 3: // Float
            return expr;
        case 4: // Symbol
            result = resolveVariable(expr->uni.symbol, env);
            if(result == NULL)
            {
                printf("SYNTAX ERROR: UNABLE TO RESOLVE VARIABLE: %s\n", expr->uni.symbol);
                exit(0);
            }
            return result;
        case 5: // String
            return expr;
        case 6: return expr;
        case 7: return expr;
        case 8: // Cons Cell

            cell = expr->uni.cons; 
            Value* first = cell->cdr->uni.cons->car; // Skip over the "("
            Value* result;

            //special forms
            if(first->type == 4 && strcmp(first->uni.symbol, "if") == 0)
            {
                //implement if bindings
                Value* second = cell->cdr->uni.cons->cdr->uni.cons->car;
                //if nothing after "if" then we have syntax error
                if(second->type == 7){
                    printf("Bad conditional syntax\n");
                    return NULL;
                }

                Value* third = cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car;
                //if nothing after "if" then we have syntax error
                if(third->type == 7)
                {
                    printf("Bad conditional syntax\n");
                    return NULL;
                }

                Value* cond = eval(second, env);

                // if test is true, return first, else return second
                if(cond->type != 0 || cond->uni.boolValue != 0)
                {
                    return eval(third, env);
                }
                else 
                {  
                    Value* fourth = cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car;
                    if(fourth->type == 7)
                    {
                        return NULL;
                    }
                    return eval(fourth, env);
                }
            }
            else if(first->type == 4 && strcmp(first->uni.symbol, "quote") == 0)
            {
                ConsCell* second = cell->cdr->uni.cons->cdr->uni.cons;
                 //if nothing after "quote" then we have syntax error
                if(second->car->type == 7)
                {
                    printf("Bad quote syntax\n");
                    return NULL;
                }
                return quote(second, env);
            }

            else if(first->type == 4 && strcmp(first->uni.symbol, "let") == 0)
            {
                return let(cell, env);
            }
            else if(first->type == 4 && strcmp(first->uni.symbol, "let*") == 0)
            {
                return letStar(cell, env);
            }
            else if(first->type == 4 && strcmp(first->uni.symbol, "letrec") == 0)
            {
                return letrec(cell, env);
            }
            else if(first->type == 4 && strcmp(first->uni.symbol, "begin") == 0)
            {
                return begin(cell, env);
            }

            else if(first->type == 4 && strcmp(first->uni.symbol, "define") == 0)
            {
                env = define(cell, env);
                return NULL;
            }
            else if(first->type == 4 && strcmp(first->uni.symbol, "set!") == 0)
            {
                env = setBang(cell, env);
                return NULL;
            }

            else if(first->type == 4 && strcmp(first->uni.symbol, "lambda") == 0)
            {   
                return makeClosure(cell, env);
            }
            else if(first->type == 4 && strcmp(first->uni.symbol, "cond") == 0)
            {   
                return cond(cell, env);
            }
            else
            {
                Value* f = eval(first, env);

                if(f->type == 7)
                {
                    return expr;
                }
                ConsCell* args = evalEach(cell->cdr->uni.cons->cdr->uni.cons, env);
                return apply(f, args, env);
            }
        default:
            printf("TYPE ERROR IN EVAL: %d\n", expr->type);
            exit(0);
    }
}

Value* cond(ConsCell* cell, Environment* env)
{
    if (cell->cdr->uni.cons->cdr->uni.cons->car->type != 8)
    {
        printf("SYNTAX ERROR: THIS IS NOT HOW CONDITIONALS WORK\n");
        exit(0);
    }

    ConsCell* actuals = cell->cdr->uni.cons->cdr->uni.cons;
    int count = 0;
    int numActuals = getLen(actuals);

    while(actuals != NULL && actuals->cdr != NULL && actuals->cdr->type == 8)
    {
        ++count;
        Value* firstStmt = actuals->car;
        Value* toEvaluateIfTrue = eval(firstStmt->uni.cons->cdr->uni.cons->cdr->uni.cons->car,env);
        if(firstStmt->uni.cons->cdr->uni.cons->car->type == 4 && (strcmp(firstStmt->uni.cons->cdr->uni.cons->car->uni.symbol, "else") == 0))
        {
            if (count != numActuals)
            {
                printf("Syntax error: else must appear at the end of a conditional block.\n");
                exit(0);
            }
            return toEvaluateIfTrue;
        }
        //evaluate the condition
        Value* boolVal = eval(firstStmt->uni.cons->cdr->uni.cons->car,env);
        if((boolVal->type == 0 && boolVal->uni.boolValue == 1) || (boolVal->type != 0))
        {
            return toEvaluateIfTrue;
        }
        actuals = actuals->cdr->uni.cons;
        if (actuals == NULL) break;
    }
    return NULL;
}

Environment* define(ConsCell* cell, Environment* env)
{
    if(cell->cdr->uni.cons->cdr->uni.cons->car->type != 4)
    {
        printf("SYNTAX ERROR\n"); // Next thing is not a symbol
        exit(0);
    }
    if(cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car->type == 7)
    {
        printf("SYNTAX ERROR\n"); //No assignment
        exit(0);
    }
    if(cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car->type != 7)
    {
        printf("SYNTAX ERROR\n"); //Extra arguments
        exit(0);
    }

    char* symbol = cell->cdr->uni.cons->cdr->uni.cons->car->uni.symbol;
    Value* assignment = eval(cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car, env);

    bind(symbol, assignment, env);
    return env;
}

Environment* setBang(ConsCell* cell, Environment* env)
{
    if(cell->cdr->uni.cons->cdr->uni.cons->car->type != 4)
    {
        printf("SYNTAX ERROR\n"); // Next thing is not a symbol
        exit(0);
    }
    if(cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car->type == 7)
    {
        printf("SYNTAX ERROR\n"); //No assignment
        exit(0);
    }
    if(cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car->type != 7)
    {
        printf("SYNTAX ERROR\n"); //Extra arguments
        exit(0);
    }

    char* symbol = cell->cdr->uni.cons->cdr->uni.cons->car->uni.symbol;
    Value* val = eval(cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons->car, env);
    if(resolveVariable(symbol, env) == NULL)
    {
        printf("ERROR: Variable %s NOT DEFINED\n", symbol);
        exit(0);
    }


    ConsCell* currentFrame = env->head;

    //now cycle through topFrame, and modify binding if you find it
    while(currentFrame != NULL && currentFrame->car != NULL)
    {
        Frame* frame = currentFrame->car->uni.frame;
        ConsCell* currentBinding = frame->head;
        while(currentBinding != NULL && currentBinding->car != NULL)
        {
            Binding* tmpBind = currentBinding->car->uni.binding;

            if(strcmp(tmpBind->symbol, symbol) == 0) //symbol in env
            {
                tmpBind->val = val;
                return env;
            }
            else if(currentBinding->cdr == NULL)
            {
                break;
            }
            currentBinding = currentBinding->cdr->uni.cons;
        }
        currentFrame = currentFrame->cdr->uni.cons;
    }
    printf("ERROR: Variable %s IS NOT DEFINED\n", symbol);
    exit(0);
}

Value* let(ConsCell* cell, Environment* env)
{

    // Check syntax
    Value* letBindings = cell->cdr->uni.cons->cdr->uni.cons->car;
    if(letBindings->type != 8)
    {
        printf("Bad let syntax\n");
        exit(0);
    }

    // Make the new frame
    Frame* letFrame = (Frame*)malloc(sizeof(Frame));
    assign(letFrame); //POINTER LIST
    ConsCell* letFrameHead = (ConsCell*)malloc(sizeof(ConsCell));
    assign(letFrameHead); //POINTER LIST
    letFrame->head  = letFrameHead;

    // Make the new Value containing the new frame
    Value* frameVal = (Value*)malloc(sizeof(Value));
    assign(frameVal); //POINTER LIST
    frameVal->type = 12;
    frameVal->uni.frame = letFrame;

    // Make the new Cons containing the new val+frame
    ConsCell* frameCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(frameCons); //POINTER LIST
    frameCons->car = frameVal;

    Value* frameConsCdrVal = (Value*)malloc(sizeof(Value));
    assign(frameConsCdrVal); //POINTER LIST
    frameConsCdrVal->type = 8;
    frameConsCdrVal->uni.cons = env->head;
    frameCons->cdr = frameConsCdrVal;

    Environment* letEnv = (Environment*)malloc(sizeof(Environment));
    assign(letEnv); //POINTER LIST
    letEnv->head = frameCons;

    // ITERATOR (starts as first binding)
    ConsCell* curBind = letBindings->uni.cons;

    // Iterate through the let bindings and add Binding objects to bindingList
    while(curBind != NULL && curBind->car != NULL)
    {
        Value* subConsVal = curBind->cdr;

        if(subConsVal->uni.cons->car->type == 7)
        {
            break;
        }

        if(subConsVal->uni.cons->car->type != 8)
        {
            printf("Bad let syntax\n");
            exit(0);
        }

        // Get the symbol and value to bind together
        Value* innerOpenParen = subConsVal->uni.cons->car->uni.cons->car;
        Value* innerSymbol = subConsVal->uni.cons->car->uni.cons->cdr->uni.cons->car;
        Value* unevalInnerVal = subConsVal->uni.cons->car->uni.cons->cdr->uni.cons->cdr->uni.cons->car;
        Value* innerVal = eval(unevalInnerVal, env);

       if(innerSymbol->type != 4)
       {
            printf("SYNTAX ERROR IN LET\n");
            exit(0);
       }

        bind(innerSymbol->uni.symbol, innerVal, letEnv);

        curBind = curBind->cdr->uni.cons;
    }
    
    ConsCell* executablesList = cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons;
    Value* result = NULL;
    result = eval(executablesList->car, letEnv);
    while(executablesList != NULL && executablesList->car != NULL)
    {
        if(executablesList->car->type == 7)
        {
            break;
        }
         result = eval(executablesList->car, letEnv);
         executablesList = executablesList->cdr->uni.cons;
    }
    free(letFrame);
    free(letFrameHead);
    free(frameVal);
    free(frameCons);
    return result;
}

Value* letStar(ConsCell* cell, Environment* env)
{

    // Check syntax
    Value* letBindings = cell->cdr->uni.cons->cdr->uni.cons->car;
    if(letBindings->type != 8)
    {
        printf("Bad let syntax\n");
        exit(0);
    }

    // Make the new frame
    Frame* letFrame = (Frame*)malloc(sizeof(Frame));
    assign(letFrame); //POINTER LIST
    ConsCell* letFrameHead = (ConsCell*)malloc(sizeof(ConsCell));
    assign(letFrameHead); //POINTER LIST
    letFrame->head  = letFrameHead;

    // Make the new Value containing the new frame
    Value* frameVal = (Value*)malloc(sizeof(Value));
    assign(frameVal); //POINTER LIST
    frameVal->type = 12;
    frameVal->uni.frame = letFrame;

    // Make the new Cons containing the new val+frame
    ConsCell* frameCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(frameCons); //POINTER LIST
    frameCons->car = frameVal;

    Value* frameConsCdrVal = (Value*)malloc(sizeof(Value));
    assign(frameConsCdrVal); //POINTER LIST
    frameConsCdrVal->type = 8;
    frameConsCdrVal->uni.cons = env->head;
    frameCons->cdr = frameConsCdrVal;

    Environment* letEnv = (Environment*)malloc(sizeof(Environment));
    assign(letEnv); //POINTER LIST
    letEnv->head = frameCons;

    // ITERATOR (starts as first binding)
    ConsCell* curBind = letBindings->uni.cons;

    // Iterate through the let bindings and add Binding objects to bindingList
    while(curBind != NULL && curBind->car != NULL)
    {
        Value* subConsVal = curBind->cdr;

        if(subConsVal->uni.cons->car->type == 7)
        {
            break;
        }

        if(subConsVal->uni.cons->car->type != 8)
        {
            printf("Bad let syntax\n");
            exit(0);
        }

        // Get the symbol and value to bind together
        Value* innerOpenParen = subConsVal->uni.cons->car->uni.cons->car;
        Value* innerSymbol = subConsVal->uni.cons->car->uni.cons->cdr->uni.cons->car;
        Value* unevalInnerVal = subConsVal->uni.cons->car->uni.cons->cdr->uni.cons->cdr->uni.cons->car;
        Value* innerVal = eval(unevalInnerVal, letEnv);

       if(innerSymbol->type != 4)
       {
            printf("SYNTAX ERROR IN LET\n");
            exit(0);
       }

        bind(innerSymbol->uni.symbol, innerVal, letEnv);

        curBind = curBind->cdr->uni.cons;
    }
    
    ConsCell* executablesList = cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons;
    Value* result = NULL;
    result = eval(executablesList->car, letEnv);
    while(executablesList != NULL && executablesList->car != NULL)
    {
        if(executablesList->car->type == 7)
        {
            break;
        }
         result = eval(executablesList->car, letEnv);
         executablesList = executablesList->cdr->uni.cons;
    }
    free(letFrame);
    free(letFrameHead);
    free(frameVal);
    free(frameCons);
    return result;
}

Value* letrec(ConsCell* cell, Environment* env)
{

    // Check syntax
    Value* letBindings = cell->cdr->uni.cons->cdr->uni.cons->car;
    if(letBindings->type != 8)
    {
        printf("Bad let syntax\n");
        exit(0);
    }

    // Make the new frame
    Frame* letFrame = (Frame*)malloc(sizeof(Frame));
    assign(letFrame); //POINTER LIST
    ConsCell* letFrameHead = (ConsCell*)malloc(sizeof(ConsCell));
    assign(letFrameHead); //POINTER LIST
    letFrame->head  = letFrameHead;

    // Make the new Value containing the new frame
    Value* frameVal = (Value*)malloc(sizeof(Value));
    assign(frameVal); //POINTER LIST
    frameVal->type = 12;
    frameVal->uni.frame = letFrame;

    // Make the new Cons containing the new val+frame
    ConsCell* frameCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(frameCons); //POINTER LIST
    frameCons->car = frameVal;

    Value* frameConsCdrVal = (Value*)malloc(sizeof(Value));
    assign(frameConsCdrVal); //POINTER LIST
    frameConsCdrVal->type = 8;
    frameConsCdrVal->uni.cons = env->head;
    frameCons->cdr = frameConsCdrVal;

    Environment* letEnv = (Environment*)malloc(sizeof(Environment));
    assign(letEnv); //POINTER LIST
    letEnv->head = frameCons;

    // ITERATOR (starts as first binding)
    ConsCell* curBind = letBindings->uni.cons;

    // Iterate through the let bindings and add Binding objects to bindingList
    while(curBind != NULL && curBind->car != NULL)
    {
        Value* subConsVal = curBind->cdr;

        if(subConsVal->uni.cons->car->type == 7)
        {
            break;
        }

        if(subConsVal->uni.cons->car->type != 8)
        {
            printf("Bad let syntax\n");
            exit(0);
        }

        // Get the symbol and value to bind together
        Value* innerOpenParen = subConsVal->uni.cons->car->uni.cons->car;
        Value* innerSymbol = subConsVal->uni.cons->car->uni.cons->cdr->uni.cons->car;

       if(innerSymbol->type != 4)
       {
            // We found a close paren instead of a symbol? Should be syntax error?
            printf("SYNTAX ERROR IN LET\n");
            exit(0);
       }

        bind(innerSymbol->uni.symbol, NULL, letEnv);

        curBind = curBind->cdr->uni.cons;
    }

    // ITERATOR (starts as first binding)
    curBind = letBindings->uni.cons;

    // Iterate through the let bindings and add Binding objects to bindingList
    while(curBind != NULL && curBind->car != NULL)
    {
        Value* subConsVal = curBind->cdr;

        if(subConsVal->uni.cons->car->type == 7)
        {
            break;
        }

        if(subConsVal->uni.cons->car->type != 8)
        {
            printf("Bad let syntax\n");
            exit(0);
        }

        // Get the symbol and value to bind together
        Value* innerOpenParen = subConsVal->uni.cons->car->uni.cons->car;
        Value* innerSymbol = subConsVal->uni.cons->car->uni.cons->cdr->uni.cons->car;
        Value* unevalInnerVal = subConsVal->uni.cons->car->uni.cons->cdr->uni.cons->cdr->uni.cons->car;
        Value* innerVal = eval(unevalInnerVal, letEnv);

       if(innerSymbol->type != 4)
       {
            printf("SYNTAX ERROR IN LET\n");
            exit(0);
       }

        bind(innerSymbol->uni.symbol, innerVal, letEnv);

        curBind = curBind->cdr->uni.cons;
    }
    
    ConsCell* executablesList = cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons;
    Value* result = NULL;

    result = eval(executablesList->car, letEnv);
    while(executablesList != NULL && executablesList->car != NULL)
    {
        if(executablesList->car->type == 7)
        {
            break;
        }
         result = eval(executablesList->car, letEnv);
         executablesList = executablesList->cdr->uni.cons;
     }
    free(letFrame);
    free(letFrameHead);
    free(frameVal);
    free(frameCons);
    return result;
}

Value* begin(ConsCell* cell, Environment* env)
{
    
    ConsCell* executablesList = cell->cdr->uni.cons->cdr->uni.cons;
    Value* result = NULL;
    result = eval(executablesList->car, env);
    while(executablesList != NULL && executablesList->car != NULL)
    {
        if(executablesList->car->type == 7)
        {
            break;
        }
         result = eval(executablesList->car, env);
         executablesList = executablesList->cdr->uni.cons;
    }
    return result;
}

Environment* createTopFrame()
{
    Environment* env = (Environment*)malloc(sizeof(Environment));
    assign(env); //POINTER LIST
    Frame* topFrame = (Frame*)malloc(sizeof(Frame));
    assign(topFrame); //POINTER LIST

    Value* topFrameValue = (Value*)malloc(sizeof(Value));
    assign(topFrameValue); //POINTER LIST
    topFrameValue->type = 12;
    topFrameValue->uni.frame = topFrame;

    ConsCell* topFrameCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(topFrameCons); //POINTER LIST
    topFrameCons->car = topFrameValue;
    topFrameCons->cdr = NULL;
    env->head = topFrameCons;

    bind("+", makePrimitiveValue(add), env);
    bind("-", makePrimitiveValue(subtract), env);
    bind("=", makePrimitiveValue(equals), env);
    bind("and", makePrimitiveValue(and), env);
    bind("or", makePrimitiveValue(or), env);
    bind("not", makePrimitiveValue(not), env);
    bind("*", makePrimitiveValue(multiply), env);
    bind("/", makePrimitiveValue(divide), env);
    bind("<", makePrimitiveValue(lessThan), env);
    bind(">", makePrimitiveValue(greaterThan), env);
    bind(">=", makePrimitiveValue(greaterThanEqual), env);
    bind("<=", makePrimitiveValue(lessThanEqual), env);
    bind("car", makePrimitiveValue(car), env);
    bind("cdr", makePrimitiveValue(cdr), env);
    bind("cons", makePrimitiveValue(cons), env);
    bind("list", makePrimitiveValue(list), env);
    bind("append", makePrimitiveValue(append), env);
    bind("null?", makePrimitiveValue(isNull), env);
    bind("list?", makePrimitiveValue(isList), env);
    bind("load", makePrimitiveValue(load), env);
    bind("display", makePrimitiveValue(display), env);
    /* ... */
    return env;
}

void bind(char* symbol, Value* val, Environment* env)
{
    Frame* topFrame = env->head->car->uni.frame;

    //now cycle through topFrame, and modify binding if you find it
    ConsCell* currentBinding = topFrame->head;
    while(currentBinding != NULL && currentBinding->car != NULL)
    {
        Binding* tmpBind = currentBinding->car->uni.binding;

        if(strcmp(tmpBind->symbol, symbol) == 0) //symbol in env
        {
            tmpBind->val = val;
            return;
        }
        else if(currentBinding->cdr == NULL)
        {
            break;
        }
        currentBinding = currentBinding->cdr->uni.cons;
    }

    //there was no binding on the top frame, so push one onto the frame
    Binding* binding = (Binding*)malloc(sizeof(Binding));
    assign(binding); //POINTER LIST
    binding->symbol = symbol;
    binding->val = val;
    Value* tmpValue = (Value*)malloc(sizeof(Value));
    assign(tmpValue); //POINTER LIST
    tmpValue->type = 11;
    tmpValue->uni.binding = binding;

    ConsCell* newHead = topFrame->head;
    push(&(newHead), tmpValue);
    topFrame->head = newHead;
}

Value* makePrimitiveValue(Value* (*primitiveValue)(ConsCell* actuals, Environment* env))
{
    Value *ret = (Value*)malloc(sizeof(Value));
    assign(ret); //POINTER LIST
    ret->type = 9;
    ret->uni.primitiveValue = primitiveValue;
    return ret;
}

// input: symbol and an environment
// output: the symbol's bound value inside the environment env
Value* resolveVariable(char* symbol, Environment* env)
{
    ConsCell* currentFrame = env->head;
    while(currentFrame != NULL && currentFrame->car != NULL)
    {

        ConsCell* head = currentFrame->car->uni.frame->head;
        while(head != NULL && head->car != NULL)
        {
            Binding* tmpBind = head->car->uni.binding;

            if(strcmp(tmpBind->symbol, symbol) == 0) //symbol in env
            {
                return tmpBind->val;
            }
            else if(head->cdr == NULL)
            {
                break;
            }
            head = head->cdr->uni.cons;
        }

        if(currentFrame->cdr == NULL)
        {
            return NULL;
        }
        currentFrame = currentFrame->cdr->uni.cons;
    }
    return NULL;
}

Value* makeClosure(ConsCell* cell, Environment* env)
{
    Closure* closure = (Closure*)malloc(sizeof(Closure));
    assign(closure); //POINTER LIST

    //create top frame for parameters of function
    // Make the new frame
    Frame* lambdaFrame = (Frame*)malloc(sizeof(Frame));
    assign(lambdaFrame); //POINTER LIST
    ConsCell* lambdaFrameHead = (ConsCell*)malloc(sizeof(ConsCell));
    assign(lambdaFrameHead); //POINTER LIST
    lambdaFrame->head  = lambdaFrameHead;

    // Make the new Value containing the new frame
    Value* frameVal = (Value*)malloc(sizeof(Value));
    assign(frameVal); //POINTER LIST
    frameVal->type = 12;
    frameVal->uni.frame = lambdaFrame;

    // Make the new Cons containing the new val+frame
    ConsCell* frameCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(frameCons); //POINTER LIST
    frameCons->car = frameVal;

    Value* frameConsCdrVal = (Value*)malloc(sizeof(Value));
    assign(frameConsCdrVal); //POINTER LIST
    frameConsCdrVal->type = 8;
    frameConsCdrVal->uni.cons = env->head;
    frameCons->cdr = frameConsCdrVal;

    Environment* lambdaEnv = (Environment*)malloc(sizeof(Environment));
    assign(lambdaEnv); //POINTER LIST
    lambdaEnv->head = frameCons;

    closure->env = lambdaEnv;

    if(cell->cdr->uni.cons->cdr == NULL || cell->cdr->uni.cons->cdr->uni.cons->car == NULL || cell->cdr->uni.cons->cdr->uni.cons->car->type != 8)
    {
        printf("SYNTAX ERROR\n");
        exit(0);
    }
    closure->params = cell->cdr->uni.cons->cdr->uni.cons->car->uni.cons;
    closure->functionPtr = arbitraryFunction;
    if(cell->cdr->uni.cons->cdr->uni.cons->cdr == NULL)
    {
        printf("SYNTAX ERROR\n");
        exit(0);
    }
    closure->toExecute = cell->cdr->uni.cons->cdr->uni.cons->cdr->uni.cons;

    Value* tmpValue = (Value*)malloc(sizeof(Value));
    assign(tmpValue); //POINTER LIST
    tmpValue->type = 10;
    tmpValue->uni.closureValue = closure;

    return tmpValue;
}

Value* arbitraryFunction(ConsCell* args, Environment* env)
{
    Value* result = NULL;
    while(args != NULL && args->car != NULL && args->car->type != 7)
    {
        result = eval(args->car, env);
        args = args->cdr->uni.cons;
    }
    return result;
}

Value* add(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 1;
    result->uni.intValue = 0;

    while(actuals != NULL && actuals->car != NULL)
    {
        Value* val = eval(actuals->car,env);
        if(val->type == 1 && result->type == 1)
        {
            result->uni.intValue += val->uni.intValue;
        }
        else if(val->type == 3 || (val->type == 1 && result->type == 3))
        {   
            if (result->type == 1) 
            {
                float tempVal = (float)result->uni.intValue;
                result->type = 3;
                result->uni.floatValue = tempVal;
            } 
            if(val->type == 1)
            {
                result->uni.floatValue += (float)val->uni.intValue;
            }
            else
            {
                result->uni.floatValue += val->uni.floatValue;
            }
        }
        else
        {
            printf("ERROR: non-number arguments to +\n");
            exit(0);
        }

        actuals = actuals->cdr->uni.cons;
    }
    return result;
}

Value* subtract(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 1;
    result->uni.intValue = 0;
    int positive = 1;

    while(actuals != NULL && actuals->car != NULL)
    {
        Value* val = eval(actuals->car,env);
        if(val->type == 1 && result->type == 1)
        {
            result->uni.intValue = positive ? result->uni.intValue + val->uni.intValue : result->uni.intValue - val->uni.intValue;
        }
        else if(val->type == 3 || (val->type == 1 && result->type == 3))
        {
            if (result->type == 1) 
            {
                float tempVal = (float)result->uni.intValue;
                result->type = 3;
                result->uni.floatValue = tempVal;
            } 
            if(val->type == 1)
            {
                result->uni.floatValue = positive ? result->uni.floatValue + (float)(val->uni.intValue) : result->uni.floatValue - (float)(val->uni.intValue);

            }
            else
            {
                result->uni.floatValue = positive ? result->uni.floatValue + val->uni.floatValue : result->uni.floatValue - val->uni.intValue;
            }
        }
        else
        {
            printf("ERROR:\n");
        }

        actuals = actuals->cdr->uni.cons;
        positive = 0;
    }
    return result;
}

Value* multiply(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 1;
    result->uni.intValue = 1;

    while(actuals != NULL && actuals->car != NULL)
    {
        Value* val = eval(actuals->car,env);
        if(val->type == 1 && result->type == 1)
        {
            result->uni.intValue *= val->uni.intValue;
        }
        else if(val->type == 3 || (val->type == 1 && result->type == 3))
        {
            if (result->type == 1) 
            {
                float tempVal = (float)result->uni.intValue;
                result->type = 3;
                result->uni.floatValue = tempVal;
            } 
            if(val->type == 1)
            {
                result->uni.floatValue *= (float)val->uni.intValue;
            }
            else
            {
                result->uni.floatValue *= val->uni.floatValue;
            }
        }
        else
        {
            printf("ERROR:\n");
        }

        actuals = actuals->cdr->uni.cons;
    }
    return result;
}

Value* divide(ConsCell* actuals, Environment* env)
{
    // Sets result to the first value in the operation
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result = eval(actuals->car,env);

    // Steps actuals to be the second value in the operation
    actuals = actuals->cdr->uni.cons;

    while(actuals != NULL && actuals->car != NULL)
    {
        Value* val = eval(actuals->car,env);
        if(val->type == 1 && result->type == 1)
        {
            result->uni.intValue /= val->uni.intValue;
        }
        else if(val->type == 3 || (val->type == 1 && result->type == 3))
        {
            if (result->type == 1) 
            {
                float tempVal = (float)result->uni.intValue;
                result->type = 3;
                result->uni.floatValue = tempVal;
            } 
            if(val->type == 1)
            {
                result->uni.floatValue /= (float)val->uni.intValue;
            }
            else
            {
                result->uni.floatValue /= val->uni.floatValue;
            }
        }
        else
        {
            printf("ERROR:\n");
        }

        actuals = actuals->cdr->uni.cons;
    }
    return result;
}

Value* equals(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 0;

    float compareToThis;
    float toCompare;
    Value* val1;
    Value* val2;
    while(actuals != NULL && actuals->car != NULL && actuals->cdr->uni.cons->car != NULL)
    {
        val1 = eval(actuals->car, env);
        val2 = eval(actuals->cdr->uni.cons->car, env);
        if(val1->type != 1 && val1->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }
        if(val2->type != 1 && val2->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }

        if(val1->type == 1)
        {
            compareToThis = val1->uni.intValue;
        }
        else if(val1->type == 3)
        {
            compareToThis = val1->uni.floatValue;
        }

        if(val2->type == 1)
        {            
            toCompare = val2->uni.intValue;
        }
        else if(val2->type == 3)
        {
            toCompare = val2->uni.floatValue;

        }

        result->uni.boolValue = toCompare == compareToThis ? 1 : 0;
        if (result->uni.boolValue == 0)
        {
            break;
        }
        actuals = actuals->cdr->uni.cons;

    }
    return result;
}

Value* and(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 1;

    float compareToThis;
    float toCompare;
    Value* val1;
    while(actuals != NULL && actuals->car != NULL)
    {
        val1 = eval(actuals->car, env);
        if(val1->type != 0)
        {
            printf("Syntax error: unable to compare non bool types\n");
            exit(0);
        }

        if (val1->uni.boolValue == 0)
        {
            result->uni.boolValue = 0;
            break;
        }
        actuals = actuals->cdr->uni.cons;

    }
    return result;
}

Value* or(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 0;

    float compareToThis;
    float toCompare;
    Value* val1;
    while(actuals != NULL && actuals->car != NULL)
    {
        val1 = eval(actuals->car, env);
        if(val1->type != 0)
        {
            printf("Syntax error: unable to compare non bool types\n");
            exit(0);
        }

        if (val1->uni.boolValue == 1)
        {
            result->uni.boolValue = 1;
            break;
        }
        actuals = actuals->cdr->uni.cons;

    }
    return result;
}

Value* not(ConsCell* actuals, Environment* env)
{
    Value* val1 = eval(actuals->car, env);
    if(val1->type != 0)
    {
        printf("Syntax error: unable to compare non bool types\n");
        exit(0);
    }
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = val1->uni.boolValue ? 0 : 1;
    return result;
}

Value* lessThan(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 0;

    float compareToThis;
    float toCompare;
    Value* val1;
    Value* val2;
    while(actuals != NULL && actuals->car != NULL && actuals->cdr->uni.cons->car != NULL)
    {
        val1 = eval(actuals->car, env);
        val2 = eval(actuals->cdr->uni.cons->car, env);
        if(val1->type != 1 && val1->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }
        if(val2->type != 1 && val2->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }

        if(val1->type == 1)
        {
            compareToThis = val1->uni.intValue;
        }
        else if(val1->type == 3)
        {
            compareToThis = val1->uni.floatValue;
        }

        if(val2->type == 1)
        {            
            toCompare = val2->uni.intValue;
        }
        else if(val2->type == 3)
        {
            toCompare = val2->uni.floatValue;

        }

        result->uni.boolValue = toCompare > compareToThis ? 1 : 0;
        if (result->uni.boolValue == 0)
        {
            break;
        }
        actuals = actuals->cdr->uni.cons;

    }
    return result;
}

Value* lessThanEqual(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 0;

    float compareToThis;
    float toCompare;
    Value* val1;
    Value* val2;
    while(actuals != NULL && actuals->car != NULL && actuals->cdr->uni.cons->car != NULL)
    {
        val1 = eval(actuals->car, env);
        val2 = eval(actuals->cdr->uni.cons->car, env);
        if(val1->type != 1 && val1->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }
        if(val2->type != 1 && val2->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }

        if(val1->type == 1)
        {
            compareToThis = val1->uni.intValue;
        }
        else if(val1->type == 3)
        {
            compareToThis = val1->uni.floatValue;
        }

        if(val2->type == 1)
        {            
            toCompare = val2->uni.intValue;
        }
        else if(val2->type == 3)
        {
            toCompare = val2->uni.floatValue;

        }

        result->uni.boolValue = toCompare >= compareToThis ? 1 : 0;
        if (result->uni.boolValue == 0)
        {
            break;
        }
        actuals = actuals->cdr->uni.cons;

    }
    return result;
}

Value* greaterThan(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 0;

    float compareToThis;
    float toCompare;
    Value* val1;
    Value* val2;
    while(actuals != NULL && actuals->car != NULL && actuals->cdr->uni.cons->car != NULL)
    {
        val1 = eval(actuals->car, env);
        val2 = eval(actuals->cdr->uni.cons->car, env);
        if(val1->type != 1 && val1->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }
        if(val2->type != 1 && val2->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }

        if(val1->type == 1)
        {
            compareToThis = val1->uni.intValue;
        }
        else if(val1->type == 3)
        {
            compareToThis = val1->uni.floatValue;
        }

        if(val2->type == 1)
        {            
            toCompare = val2->uni.intValue;
        }
        else if(val2->type == 3)
        {
            toCompare = val2->uni.floatValue;

        }

        result->uni.boolValue = toCompare < compareToThis ? 1 : 0;
        if (result->uni.boolValue == 0)
        {
            break;
        }
        actuals = actuals->cdr->uni.cons;

    }
    return result;
}

Value* greaterThanEqual(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 0;

    float compareToThis;
    float toCompare;
    Value* val1;
    Value* val2;
    while(actuals != NULL && actuals->car != NULL && actuals->cdr->uni.cons->car != NULL)
    {
        val1 = eval(actuals->car, env);
        val2 = eval(actuals->cdr->uni.cons->car, env);
        if(val1->type != 1 && val1->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }
        if(val2->type != 1 && val2->type != 3)
        {
            printf("Syntax error: unable to compare non numerical types\n");
            exit(0);
        }

        if(val1->type == 1)
        {
            compareToThis = val1->uni.intValue;
        }
        else if(val1->type == 3)
        {
            compareToThis = val1->uni.floatValue;
        }

        if(val2->type == 1)
        {            
            toCompare = val2->uni.intValue;
        }
        else if(val2->type == 3)
        {
            toCompare = val2->uni.floatValue;

        }

        result->uni.boolValue = toCompare <= compareToThis ? 1 : 0;
        if (result->uni.boolValue == 0)
        {
            break;
        }
        actuals = actuals->cdr->uni.cons;

    }
    return result;
}

Value* isNull(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    Value* paramVal = actuals->car;

    if(paramVal->type != 8)
    {
        result->uni.boolValue = 0;
        return result;
    }
    ConsCell* param = paramVal->uni.cons;
    if(param->car == NULL || param->car->type != 6 )
    {
        result->uni.boolValue = 0;
        return result;
    }
    else if(param->cdr == NULL)
    {
        result->uni.boolValue = 1;
        return result;
    }
    else if (param->cdr->uni.cons->car == NULL || param->cdr->uni.cons->car->type != 7)
    {
        result->uni.boolValue = 0;
    }
    result->uni.boolValue = 0; //else
    return result;
}

Value* isList(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result); //POINTER LIST
    result->type = 0;
    result->uni.boolValue = 0;

    Value* list = actuals->car;
    if(actuals->cdr != NULL && actuals->cdr->type == 8 && actuals->cdr->uni.cons != NULL && actuals->cdr->uni.cons->car != NULL && actuals->cdr->uni.cons->car->type != 7)
    {
        printf("ERROR: list? MUST TAKE ONLY ONE ARGUMENT\n");
        exit(0);
    }
    
    if(list->type != 8)
    {
        return result;
    }
    result->uni.boolValue = 1;
    ConsCell* current = list->uni.cons;
    while(current != NULL && current->cdr != NULL)
    {
        Value* cdrValue = current->cdr;
        if(cdrValue->type != 8)
        {
            result->uni.boolValue = 0;
            return result;
        }
        current = cdrValue->uni.cons;
    }
    return result;
}

Value* car(ConsCell* actuals, Environment* env)
{
    if (getLen(actuals) > 1)
    {
        printf("Syntax error: can only have one argument in car.\n");
        exit(0);
    }
    Value* list = actuals->car;
    if(list->type != 8 || list->uni.cons->cdr == NULL)
    {
        printf("SYNTAX ERROR: function car takes a pair.\n");
        exit(0);
    }
    return list->uni.cons->cdr->uni.cons->car;
}

Value* cdr(ConsCell* actuals, Environment* env)
{
    if (getLen(actuals) > 1)
    {
        printf("Syntax error: can only have one argument in cdr.\n");
        exit(0);
    }
    Value* list = actuals->car;
    if(list->type != 8)
    {
        printf("SYNTAX ERROR: function cdr takes a pair.\n");
        exit(0);
    }
    if(list->uni.cons->cdr == NULL) //Empty list
    {
        printf("ERROR: cdr does not act on the empty list\n");
        exit(0);
    }

    Value* toReturn = list->uni.cons->cdr->uni.cons->cdr;

    if(toReturn != NULL)
    {
        if(toReturn->type != 8)
        {
            return toReturn;
        }
    }
    else 
    { 
        Value* toReturn = (Value*)malloc(sizeof(Value));
        assign(toReturn);
        toReturn->type = 8;

        ConsCell* consCar = (ConsCell*)malloc(sizeof(ConsCell));
        assign(consCar);
        toReturn->uni.cons = consCar;
        consCar->cdr = NULL;

        Value* openParen = (Value*)malloc(sizeof(Value));
        assign(openParen);
        openParen->type = 6;
        openParen->uni.open = '(';
        consCar->car = openParen;

        return toReturn;
    }
    Value* openParenthesis = (Value*)malloc(sizeof(Value));
    assign(openParenthesis); //POINTER LIST
    openParenthesis->type = 6;
    openParenthesis->uni.open = '(';
    ConsCell* tmpCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(tmpCons); //POINTER LIST
    tmpCons->car = openParenthesis;
    tmpCons->cdr = toReturn;
    Value* head = (Value*)malloc(sizeof(Value));
    assign(head); //POINTER LIST
    head->type = 8;
    head->uni.cons = tmpCons;
    return head;
}


Value* cons(ConsCell* actuals, Environment* env)
{
    if (getLen(actuals) != 2)
    {
        printf("Syntax Error: must have two arguments in cons.\n");
        exit(0);
    }

    ConsCell* newCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(newCons);

    Value* first = actuals->car;

    Value* second = actuals->cdr->uni.cons->car;
    if (second->type == 8)
    {
        second = actuals->cdr->uni.cons->car->uni.cons->cdr;
    }

    Value* openParenCdr = (Value*)malloc(sizeof(Value));
    assign(openParenCdr);
    openParenCdr->type = 8;
    openParenCdr->uni.cons = newCons;

    newCons->car = first;
    newCons->cdr = second;

    Value* head = (Value*)malloc(sizeof(Value));
    assign(head);
    head->type = 8;

    ConsCell* openParen = (ConsCell*)malloc(sizeof(ConsCell));
    assign(openParen);

    Value* openParenVal = (Value*)malloc(sizeof(Value));
    assign(openParenVal);
    openParenVal->type = 6;
    openParenVal->uni.open = '(';

    openParen->car = openParenVal;
    openParen->cdr = openParenCdr;

    head->uni.cons = openParen;

    return head;
}


Value* list(ConsCell* actuals, Environment* env)
{
    Value* listHeadValue = (Value*)malloc(sizeof(Value));
    assign(listHeadValue); //POINTER LIST
    listHeadValue->type = 8;
    ConsCell* head = (ConsCell*)malloc(sizeof(ConsCell));
    assign(head); //POINTER LIST
    listHeadValue->uni.cons = head;
    Value* openParenthesis = (Value*)malloc(sizeof(Value));
    assign(openParenthesis); //POINTER LIST
    openParenthesis->type = 6;
    openParenthesis->uni.open = '(';
    head->car = openParenthesis;

    ConsCell* actualsCurrent = actuals;
    while(actualsCurrent != NULL && actualsCurrent->car != NULL && actualsCurrent->car->type != 7)
    {
        Value* nextValue = (Value*)malloc(sizeof(Value));
        assign(nextValue); //POINTER LIST
        nextValue->type = 8;
        ConsCell* nextCons = (ConsCell*)malloc(sizeof(ConsCell));
        assign(nextCons); //POINTER LIST
        nextCons->car = actualsCurrent->car;
        if(actuals == NULL)
        {
            Value* lastValue = (Value*)malloc(sizeof(Value));
            assign(lastValue); //POINTER LIST
            head->cdr = lastValue;
        }
        else
        {
            nextValue->uni.cons = nextCons;
            head->cdr = nextValue;
            head = head->cdr->uni.cons;
        }
        actualsCurrent = actualsCurrent->cdr->uni.cons;
    }
    head->cdr = NULL;
    return listHeadValue;
}

Value* append(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result);
    Value* current = actuals->car;
    if (current->type != 8)
    {
        printf("Contract Violation: Cannot append to non-list.\n");
        exit(0);
    }
    result = current;

    ConsCell* actualsCurrent = actuals->cdr->uni.cons;
    while(actualsCurrent != NULL && actualsCurrent->car != NULL)
    {
        while(current->uni.cons != NULL && current->uni.cons->car != NULL)
        {

          if(current->uni.cons->cdr != NULL && current->uni.cons->cdr->uni.cons != NULL)
          {
  
            if(current->uni.cons->cdr->type != 8 && isImproperList(current->uni.cons))
            {
                // Improper list!
                printf("ERROR: LIST EXPECTED IN APPEND\n");
                exit(0);
            }
            current = current->uni.cons->cdr;
          }
          else
          {
            break;
          }
        }

        Value* next = actualsCurrent->car;
        if (next->type == 8 && next->uni.cons->car->type == 6)
        {
            next = next->uni.cons->cdr;
        }
        current->uni.cons->cdr = next;
        current = current->uni.cons->cdr;
        actualsCurrent = actualsCurrent->cdr->uni.cons;
    }

    return result;
}

Value* quote(ConsCell* actuals, Environment* env)
{
    Value* result = (Value*)malloc(sizeof(Value));
    assign(result);
    int numArgs = getLen(actuals);
    ConsCell* currentCons = actuals;
    Value currentVal = *(currentCons->car);
    Value* current = &currentVal;
    if (numArgs != 1)
    {
        printf("Quote: wrong number of actuals.\n");
        exit(0);
    }
    if (current->type != 8)
    {
        return current;
    }
    result = current;
    return list(result->uni.cons->cdr->uni.cons, env);
    // actuals = result->uni.cons;
    // // CODE FROM LIST:

    // Value* listHeadValue = (Value*)malloc(sizeof(Value));
    // assign(listHeadValue); //POINTER LIST
    // listHeadValue->type = 8;
    // ConsCell* head = (ConsCell*)malloc(sizeof(ConsCell));
    // assign(head); //POINTER LIST
    // listHeadValue->uni.cons = head;
    // Value* openParenthesis = (Value*)malloc(sizeof(Value));
    // assign(openParenthesis); //POINTER LIST
    // openParenthesis->type = 6;
    // openParenthesis->uni.open = '(';
    // head->car = openParenthesis;

    // ConsCell* actualsCurrent = actuals;
    // while(actualsCurrent != NULL && actualsCurrent->car != NULL)
    // {
    //     Value* nextValue = (Value*)malloc(sizeof(Value));
    //     assign(nextValue); //POINTER LIST
    //     nextValue->type = 8;
    //     ConsCell* nextCons = (ConsCell*)malloc(sizeof(ConsCell));
    //     assign(nextCons); //POINTER LIST
    //     nextCons->car = actualsCurrent->car;
    //     if(actuals == NULL)
    //     {
    //         Value* lastValue = (Value*)malloc(sizeof(Value));
    //         assign(lastValue); //POINTER LIST
    //         head->cdr = lastValue;
    //     }
    //     else
    //     {
    //         nextValue->uni.cons = nextCons;
    //         head->cdr = nextValue;
    //         head = head->cdr->uni.cons;
    //     }
    //     actualsCurrent = actualsCurrent->cdr->uni.cons;
    // }
    // head->cdr = NULL;
    // return listHeadValue;



    // //strip close parenthesis
    // if(result->type != 8)
    // {
    //     return result;
    // }
    // currentCons = result->uni.cons;
    // if(currentCons->cdr == NULL)
    // {
    //     return result;
    // }

    // ConsCell* nextCons = currentCons->cdr->uni.cons;

    // while(nextCons != NULL && nextCons->car != NULL)
    // {
    //     if(nextCons->car->type == 7)
    //     {
    //         currentCons->cdr = NULL;
    //         return result;
    //     }
    //     currentCons = nextCons;
    //     nextCons = currentCons->cdr->uni.cons;
    // }
    // return result;
}


Value* display(ConsCell* actuals, Environment* env)
{
    if (getLen(actuals) > 1)
    {
        printf("Syntax error: can only have one argument in display.\n");
        exit(0);
    }
    Value* paramVal = actuals->car;

    if(paramVal->type == 5)
    {
        char* string = paramVal->uni.stringValue;

        int index = 1;
        while(string[index] != '"')
        {
            char c = string[index];
            if(c == '\\')
            {
                index++;
                c = string[index];
                if(c == '"' || c == '\\')
                {
                    printf("%c", string[index]);
                }
                else if(c == 'n')
                {
                    printf("\n");
                }
                else if(c == 't')
                {
                    printf("\t");
                }
                else
                {
                    printf("SYNTAX ERROR: IMPROPER BACKSLASH\n");
                    exit(0);
                }
            }
            else
            {
                printf("%c", c);
            }
            index++;
        }
    }
    else
    {
        printValue(paramVal);
    }
    printf("\n");
    return NULL;
}

Value* load(ConsCell* actuals, Environment* env)
{
    if(actuals->car->type != 5)
    {
        printf("LOAD ERROR\n");
        exit(0);
    }
    char* filename = actuals->car->uni.stringValue;
    filename[strlen(filename)-1] = '\0';
    filename++;

    ConsCell *tokens = NULL;
    ConsCell *parseTree = NULL;
    int depth = 0;
    char *line = (char *) malloc(256 * sizeof(char));
    assign(line); //POINTER LIST
    FILE *fp; 
    fp=fopen(filename, "r");
    if(fp == NULL)
    {
        printf("ERROR READING FILE\n");
        exit(0);
    }

    while (fgets(line, 256, fp) != NULL)
    {
        Value* tokensValue = tokenize(line);
        if (tokensValue == NULL)
        {
            continue;
        }
        tokens = tokensValue->uni.cons;
        if (!tokens) 
        {
            printf("SYNTAX ERROR: UNTOKENIZABLE\n");
            return 0;
        }

        parseTree = parse(tokens, &depth, parseTree);
        if (depth < 0) 
        {
            printf("Syntax Error in File\n");
            return 0;
        } 
        else if (depth == 0) 
        {
            printValue(eval(parseTree->car, env));
            printf("\n");
        }
    }
    if(depth != 0)
    {
        printf("Syntax Error in File\n");
        exit(0);
    }
    return NULL;
}


void loadLibrary(char* filename, Environment* env)
{



    ConsCell *tokens = NULL;
    ConsCell *parseTree = NULL;
    int depth = 0;
    char *line = (char *) malloc(256 * sizeof(char));
    assign(line); //POINTER LIST
    FILE *fp; 
    fp=fopen(filename, "r");
    if(fp == NULL)
    {
        printf("ERROR READING FILE\n");
        exit(0);
    }

    while (fgets(line, 256, fp) != NULL)
    {
        Value* tokensValue = tokenize(line);
        if (tokensValue == NULL)
        {
            continue;
        }
        tokens = tokensValue->uni.cons;
        if (!tokens) 
        {
            printf("SYNTAX ERROR: UNTOKENIZABLE\n");
            return;
        }

        parseTree = parse(tokens, &depth, parseTree);
        if (depth < 0) 
        {
            printf("Syntax Error in File\n");
            return;
        } 
        else if (depth == 0) 
        {
            printValue(eval(parseTree->car, env));
            printf("\n");
        }
    }
    if(depth != 0)
    {
        printf("Syntax Error in File\n");
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    pointerList = (void**)malloc(maxPointerListLength * sizeof(void*));
    pointerListLength = 0;

    ConsCell *tokens = NULL;
    ConsCell *parseTree = NULL;
    int depth = 0;
    char *line = (char *) malloc(256 * sizeof(char));
    assign(line);
    Environment* env = createTopFrame();
    
    int i;
    for(i = 1;i < argc;i++)
    {
        printf("argv[i]:%s\n",argv[i]);
        loadLibrary(argv[i], env);
    }

    printf("> ");
    while (fgets(line, 256, stdin))
    {
        if(strcmp(line,"What is the meaning of life?\n") == 0)
        {
            printf("42\n");
            printf("> ");
            continue;
        }
        if(strcmp(line,"hello!\n") == 0 || strcmp(line,"Hello!\n") == 0 || strcmp(line,"Szia!\n") == 0)
        {
            printf("Szia! Hogy Vagy?\n");
            printf("> ");
            continue;
        }
        if(strcmp(line,"What is the best CS class?\n") == 0)
        {
            printf("Programming Languages.\n");
            printf("> ");
            continue;
        }
        if(strcmp(line,":quit\n") == 0)
        {
            freePointerList();
            free(pointerList);
            printf("Goodbye.\n");
            exit(0);
        }
        if(strcmp(line,":q\n") == 0)
        {
            freePointerList();
            free(pointerList);
            printf("Goodbye.\n");
            exit(0);
        }
        if(strcmp(line,"segfault\n") == 0 || strcmp(line,"seg fault\n") == 0 || strcmp(line,"Segmentation Fault.\n") == 0)
        {
            freePointerList();
            free(pointerList);
            printf("Bus Error.\n");
            exit(1);
        }
        Value* tokensValue = tokenize(line);
        if (tokensValue == NULL)
        {
            continue;
        }
        tokens = tokensValue->uni.cons;

        if (!tokens) 
        {
            printf("SYNTAX ERROR: UNTOKENIZABLE\n");
            return 0;
        }

        parseTree = parse(tokens, &depth, parseTree);  
        if (depth < 0) 
        {
            printf("Syntax Error in File\n");
            return 0;
        } 
        else if (depth == 0) 
        {
            Value* toPrint = eval(parseTree->car,env);
            
            printValue(toPrint);
            if(toPrint != NULL)
            {
                printf("\n");
            }

        }
        if (depth > 0)
        {
            printf("... ");
        }
        else
            printf("> ");
    }
    if(depth != 0)
    {
        printf("Syntax Error in File\n");
    }
    printf("LENGTH: %d\n", pointerListLength);
    freePointerList();
    free(pointerList);
}
