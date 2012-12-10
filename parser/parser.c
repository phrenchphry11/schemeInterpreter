# include "../tokenizer/tokenizer.c"

void push(ConsCell** stack, Value* element)
{
  if (*stack == NULL) 
  {
    ConsCell* newStack = (ConsCell*)malloc(sizeof(ConsCell));
    assign(newStack); //POINTER LIST
    newStack->car = element;
    newStack->cdr = NULL;
    *stack = newStack;
  }
  else
  {
    ConsCell* newHead = (ConsCell*)malloc(sizeof(ConsCell));
    assign(newHead); //POINTER LIST
    newHead->car = element;
    
    Value* tmpValue = (Value*)malloc(sizeof(Value));
    assign(tmpValue); //POINTER LIST
    tmpValue->type = 8;
    tmpValue->uni.cons = *stack;
    
    newHead->cdr = tmpValue;
    *stack = newHead;
  }
}

Value* pop(ConsCell** stack)
{
  Value* element = (*stack)->car;
  if ((*stack)->cdr != NULL)
  {
    // If we're not popping the last element (list is of size > 1), then reset the stack to be the rest.
    *stack = (*stack)->cdr->uni.cons;
  }
  else
  {
    *stack = NULL;
  }
  return element;
}

void freeParseTree(Value* parseTree) 
{
  Value* current = parseTree;
  if (current == NULL) 
  {
    return;
  }
  if(current->type == 8)
  {
    freeParseTree(current->uni.cons->car);
    freeParseTree(current->uni.cons->cdr);
  }
  free(current);
}


ConsCell* parse(ConsCell* tokens, int* depth, ConsCell* parseTree)
{
  ConsCell* current = tokens;
  while(current != NULL && current->car != NULL)
  {

    Value* element = current->car;

    if(element->type == 6) //open
    {
      // push ( onto stack
      push(&parseTree, element);
      //return parse(current->cdr->uni.cons, depth+1, *stackPointer);
      (*depth)++;

    }
    else if(element->type == 7) //close
    {
      (*depth)--;
      if(*depth < 0)
      {
        printf("SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS\n");
        exit(0);
      }

      ConsCell* listElement = (ConsCell*)malloc(sizeof(ConsCell));
      assign(listElement); //POINTER LIST
      push(&listElement, element); // Push the ')' onto the listElement
      while(1)
      {
        Value* popElement = pop(&parseTree);
     

        if(popElement->type == 6)
        {
          push(&listElement, popElement);
          break;
        }
        else
        {
          push(&listElement, popElement);
        }

      }
        //push listElement onto stack
        Value* listElementValue = (Value*)malloc(sizeof(Value));
        assign(listElementValue); //POINTER LIST
        listElementValue->type = 8;
        listElementValue->uni.cons = listElement;
        push(&parseTree, listElementValue);
    }
    else
    {
      push(&parseTree, element);
    }

    if(current->cdr != NULL)
    {
      current = current->cdr->uni.cons;
    }
    else
    {
      break;
    }
  }
  return parseTree;
}

void printParseTree(ConsCell* parseTree)
{
  while(parseTree != NULL && parseTree->car != NULL)
  {
    Value* currentValue = pop(&parseTree);

    switch(currentValue->type) 
    {
      case 0:
        printf("%s ", currentValue->uni.boolValue == 1 ? "#t" : "#f");
        //free(currentValue);
        break;
      case 1:
        printf("%d ",currentValue->uni.intValue);
        //free(currentValue);
        break;
      case 2:
        printf("%c ",currentValue->uni.quote);
        //free(currentValue);
        break;
      case 3:
        printf("%f ",currentValue->uni.floatValue);
        //free(currentValue);
        break;
      case 4:
        printf("%s ",currentValue->uni.symbol);
        //free(currentValue);
        break;
      case 5:
        printf("%s ",currentValue->uni.stringValue);
        //free(currentValue);
        break;
      case 6:
        printf("%c",currentValue->uni.open);
        //free(currentValue);
         break;
      case 7:
        printf("%c ",currentValue->uni.close);
        //free(currentValue);
        
        pop(&parseTree);
        printParseTree(parseTree);
        return;
      case 8:
        //printf("(");
        //printf("printing case 8");
        
        printParseTree(currentValue->uni.cons);
        //freeParseTree(currentValue);
        //printf(")");
        break;
      case 11:
        printf("Symbol: %s ", currentValue->uni.binding->symbol);
        printf("Bound type: %d ", currentValue->uni.binding->val->type);
      default:
        //printf("SYNTAX ERROR\n");
        //printf("weird default");
        break;
    }
  }
}
// void printParseTree(ConsCell* parseTree)
// {
//   ConsCell* current = parseTree;
//   while(current != NULL && current->car != NULL)
//   {
//     Value* currentValue = current->car;

//     switch(currentValue->type) 
//     {
//       case 0:
//         printf("%s ", currentValue->uni.boolValue == 1 ? "#t" : "#f");
//         break;
//       case 1:
//         printf("%d ",currentValue->uni.intValue);
//         break;
//       case 2:
//         printf("%c ",currentValue->uni.quote);
//         break;
//       case 3:
//         printf("%f ",currentValue->uni.floatValue);
//         break;
//       case 4:
//         printf("%s ",currentValue->uni.symbol);
//         break;
//       case 5:
//         printf("%s ",currentValue->uni.stringValue);
//         break;
//       case 6:
//         printf("%c",currentValue->uni.open);
//          break;
//       case 7:
//         printf("%c ",currentValue->uni.close);
//         if(current->cdr != NULL)
//         {
//           current = current->cdr->uni.cons;
//           printParseTree(current);
//         }
//         return;
//       case 8:
//         //printf("(");
//         //printf("printing case 8");
//         printParseTree(currentValue->uni.cons);
//         //printf(")");
//         break;
//       default:
//         //printf("SYNTAX ERROR\n");
//         //printf("weird default");
//         break;
//     }
//     if(current->cdr != NULL)
//     {
//      // printf("BEFOREcurrent type: %d\n",currentValue->type);
//       current = current->cdr->uni.cons;
//     }
//     else
//     {
//       break;
//     }
//   }
// }


// int main(int argc, char *argv[]) {
//   ConsCell *tokens = NULL;
//   ConsCell *parseTree = NULL;
//   int depth = 0;

//   char *line = (char *) malloc(256 * sizeof(char));

//   while (fgets(line, 256, stdin)) {
//     Value* tokensValue = tokenize(line);
//     if (tokensValue == NULL)
//     {
//       continue;
//     }
//     tokens = tokensValue->uni.cons;

//     if (!tokens) 
//     {
//       printf("syntax error\n");
//       return 0; //SYNTAX_ERROR_UNTOKENIZABLE;
//     }

//     parseTree = parse(tokens, &depth, parseTree);
//     if (depth < 0) 
//     {
//       printf("syntax error\n");
//       return 0;
//     } 
//     else if (depth == 0) 
//     {
//       /* No unclosed parens; we have a full parse tree! */
//       ConsCell* parseTreeTwo = NULL;
//       while (parseTree != NULL && parseTree->car != NULL)
//       {
//         Value* temp = NULL;
//          temp = pop(&parseTree);
//          push(&parseTreeTwo,temp);
//       }
//       ConsCell parseTreeCopy = *parseTreeTwo;
//       printParseTree(&parseTreeCopy);

//       //freeParseTree(tokensValue);
//       //freeParseTree(parseTree);
//       //freeParseTree(&parseTreeCopy);
//       free(parseTree);
//       free(tokens);
//       parseTree = NULL;
//     }
//   }
//   free(line);
//   return 0;
// }
