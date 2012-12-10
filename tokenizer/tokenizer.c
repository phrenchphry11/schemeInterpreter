# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include "tokenizer.h"


void assign(void* toAssign)
{
    pointerList[pointerListLength] = toAssign;
    pointerListLength++;
    if(pointerListLength >= maxPointerListLength)
    {
        printf("OUT OF MEMORY\n");
        exit(0);
    }
}

void freePointerList()
{
    int index;
    for(index = 0; index < pointerListLength; index++)
    {
        free((void*)(pointerList[index]));
    }
}

char determineType(char* token, int size)
{
  if(!strcmp(token, "true") || !strcmp(token, "false"))
  {
    return 'a';
  }

  int i;
  char state = 's';
  //-------------------------------------------------0-------------------------------0-------------------------------0--------------------------------
  char* automatons = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzdmkdddqfgdpzptdbbbbbbbbbbdzddddzddddddddddddddddddddddddddfzgddzddddddddddddddddddddddddddfzgdz";
  char* automatonz = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
  char* automatonk = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzazzzzzzzzzzzzzazzzzzzzzzzz";
  char* automatonm = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzmmemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmhmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmz";
  char* automatonh = "zzzzzzzzzmzzzzzzzzzzzzzzzzzzzzzzzzmzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzmzzzzzzzzzzzzzzzzzmzzzzzmzzzzzzzzzzz";
  char* automatonp = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzczbbbbbbbbbbzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
  char* automatonb = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzczbbbbbbbbbbzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
  char* automatonc = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzcccccccccczzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
  char* automatond = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzdzzdddzzzddzddddddddddddddzddddzddddddddddddddddddddddddddzzzddzddddddddddddddddddddddddddzzzdz";
  char* automatont = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzcccccccccczzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";

  //h = bkslash, p = sign, k = tempbool, m = tempstring, q = quote, t = dot, z = error state: a,e,f,g,q
  for (i = 0; i < size; i++)
  {
    char c = token[i];
    switch(state)
    {
      case 'z': case 'a': case 'e': case 'f': case 'g': case 'q':
        state = automatonz[(int)c]; //error state
        break;
      case 's':
        state = automatons[(int)c];
        break;
      case 'k':
        state = automatonk[(int)c];
        break;
      case 'm':
        state = automatonm[(int)c];
        break;
      case 'h':
        state = automatonh[(int)c];
        break;
      case 'p':
        state = automatonp[(int)c];
        break;
      case 'b':
        state = automatonb[(int)c];
        break;
      case 'c':
        state = automatonc[(int)c];
        break;
      case 'd':
        state = automatond[(int)c];
        break;
    }
  }
  return state;
}

// Takens in a token, then updates our linkedlist/pointer stuff accordingly
Value* addToken(char* current, int size, Value* head, Value** linkedListIterator) {
    char type = determineType(current, size);

    Value* tempValue = (Value*)malloc(sizeof(Value));
    assign(tempValue); //POINTER LIST
    ConsCell* tempCons = (ConsCell*)malloc(sizeof(ConsCell));
    assign(tempCons); //POINTER LIST
    Value* carValue = (Value*)malloc(sizeof(Value));
    assign(carValue); //POINTER LIST
    tempCons->car = carValue;

   switch (type)
    {
      case 'a':
        tempCons->car->uni.boolValue = (!strcmp(current, "#t") || !strcmp(current, "true")) ? 1 : 0;
        tempCons->car->type = 0;
        break;
      case 'b':
        tempCons->car->uni.intValue = atoi(current);
        tempCons->car->type = 1;
        break;
      case 'c':
        tempCons->car->uni.floatValue = atof(current);
        tempCons->car->type = 3;
        break;
      case 'd': case 'p':
        tempCons->car->uni.symbol = current;
        tempCons->car->type = 4;
        break;
      case 'e':
        tempCons->car->uni.stringValue = current;
        tempCons->car->type = 5;
        break;
      case 'f':
        tempCons->car->uni.open = *current;
        tempCons->car->type = 6;
        break;
      case 'g':
        tempCons->car->uni.close = *current;
        tempCons->car->type = 7;
        break;
      case 'q':
        tempCons->car->uni.quote = *current;
        tempCons->car->type = 2;
        break;
      case 'z': case 't': default:
        printf("SYNTAX ERROR\n");
        exit(0);
    }
    
    tempValue->type = 8;
    tempValue->uni.cons = tempCons;
     
    // If we're not at the first token, then set the cdr of the previous token to be this new value.
    if (*linkedListIterator != NULL) {
      (*linkedListIterator)->uni.cons->cdr = tempValue; 
    }

    // tempval is the token we're adding. if first node, set currentVal to be tempVal.
    *linkedListIterator = tempValue;

    // If we're at the first token, set the head.
    if (head == NULL) {
      head = tempValue;
    }

 // Then update the curValue (or, if we were at the first token, just set the curValue to the first token)
    return head;
}


Value* tokenize(char* exp) {
  Value* head = (Value*)malloc(sizeof(Value));
  assign(head); //POINTER LIST
	head = NULL; //This points to the first thing in the LinkedList so we can return it at the end

  
  Value** linkedListIterator = (Value**)malloc(sizeof(Value));
  assign(linkedListIterator); //POINTER LIST
  *linkedListIterator = NULL;

  int beginTokenIndex = 0;

  int i;
  int strFlag = 0, bsFlag = 0;
  for(i = 0; i < 256; i++) {

    if(strFlag)
    {
      if(bsFlag)
      {
        i++;
        bsFlag = 0;
      }
      if(exp[i] == '"')
      {
        // Build and add the previous token TODO: check if token is empty
        int sizeOfNewToken = i - beginTokenIndex + 1;
        char* newToken = (char*)malloc(sizeof(char) * (sizeOfNewToken));
        assign(newToken); //POINTER LIST
        strncpy(newToken, exp + beginTokenIndex, sizeOfNewToken); // build token by getting substring of exp from beginTokenIndex to i
        head = addToken(newToken, sizeOfNewToken, head, linkedListIterator);
        beginTokenIndex = i + 1;
        strFlag = 0;
      }
      else if(exp[i] == '\\')
      {
        bsFlag = 1;
      }
    }
    else
    {

        if(exp[i] == '"')
        {
          strFlag = 1;
          continue;
        }
        // SPACE
        if (exp[i] == ' ')
        {
          int endToken = i;
          while (exp[i + 1] == ' ')
          {
            i++;
          }
          if (endToken != beginTokenIndex) {
            // Build and add the previous token TODO: check if token is empty
            int sizeOfNewToken = endToken - beginTokenIndex;
            char* newToken = (char*)malloc(sizeof(char) * (sizeOfNewToken));
            assign(newToken); //POINTER LIST
            strncpy(newToken, exp + beginTokenIndex, sizeOfNewToken); // build token by getting substring of exp from beginTokenIndex to i
            
            head = addToken(newToken, sizeOfNewToken, head, linkedListIterator);
          }

          beginTokenIndex = i + 1; //'i' will be the last space in this series of spaces, so set beginToke to the next char.

          continue;
        }

        // COMMENT
        if (exp[i] == ';' || exp[i] == '\n' || exp[i] == '\t')
        {
          int endToken = i;
          if (endToken != beginTokenIndex) {
            // Build and add the previous token TODO: check if token is empty
            int sizeOfNewToken = endToken - beginTokenIndex;
            char* newToken = (char*)malloc(sizeof(char) * (sizeOfNewToken));
            assign(newToken); //POINTER LIST
            strncpy(newToken, exp + beginTokenIndex, sizeOfNewToken); // build token by getting substring of exp from beginTokenIndex to i
            
            head = addToken(newToken, sizeOfNewToken, head, linkedListIterator);

          }

          break;
        }

        // PAREN
        if (exp[i] == '(' || exp[i] == ')' || exp[i] == ']' || exp[i] == '[' || exp[i] == '}' || exp[i] == '{' || exp[i] == '\'') {

          if (i != beginTokenIndex) {
            // Build and add the previous token TODO: check if token is empty
            int sizeOfNewToken = i - beginTokenIndex;
            char* newToken = (char*)malloc(sizeof(char) * (sizeOfNewToken));
            assign(newToken); //POINTER LIST
            strncpy(newToken, exp + beginTokenIndex, sizeOfNewToken); // build token by getting substring of exp from beginTokenIndex to i
            
            head = addToken(newToken, sizeOfNewToken, head, linkedListIterator);

          }

          // Add paren as token
          int sizeOfNewToken = 1;
          char* newToken = (char*)malloc(sizeof(char) * (sizeOfNewToken));
          assign(newToken); //POINTER LIST
          strncpy(newToken, exp + i, sizeOfNewToken); // build token by getting substring of exp from beginTokenIndex to i

          head = addToken(newToken, 1, head, linkedListIterator); // NOTE: exp + i might pass the entire rest of the string. which is probs bad.
          beginTokenIndex = i + 1;

        }

      }
    }
  //free(linkedListIterator);

  return head;	
}

int isImproperList(ConsCell* actuals)
{
    Value* list = actuals->car;
    
    if(list->type != 8)
    {
        return 1;
    }
    ConsCell* current = list->uni.cons;
    while(current != NULL && current->cdr != NULL)
    {
        Value* cdrValue = current->cdr;
        if(cdrValue->type != 8)
        {
            return 1;
        }
        current = cdrValue->uni.cons;
    } 
  return 0;
}

void printTokens(Value* tokens) {
  Value* current = tokens;
  if(current == NULL)
  {
    return;
  }

  while(current != NULL)
  {
    int currentType = current->uni.cons->car->type;
    switch(currentType) 
    {
      case 0:
        printf("%s:boolean\n", current->uni.cons->car->uni.boolValue == 1 ? "#t" : "#f");
        break;
      case 1:
        printf("%d:int\n", current->uni.cons->car->uni.intValue);
        break;
      case 2:
        printf("':quote\n", (char)current->uni.cons->car->uni.quote);
        break;
      case 3:
        printf("%f:float\n", current->uni.cons->car->uni.floatValue);
        break;
      case 4:
        printf("%s:symbol\n", current->uni.cons->car->uni.symbol);
        break;
      case 5:
        printf("%s:string\n", current->uni.cons->car->uni.stringValue);
        break;
      case 6:
        printf("%c:open\n", (char)current->uni.cons->car->uni.open);
        break;
      case 7:
        printf("%c:close\n", (char)current->uni.cons->car->uni.close);
        break;
      default:
        printf("SYNTAX ERROR\n");
    }
      current = current->uni.cons->cdr;
  }
}


void printValue(Value* val)
{
  if(val == NULL)
  {
    return;
  }
  int currentType = val->type;
  ConsCell* current = NULL;
    switch(currentType) 
    {
      case 0:
        printf("%s", val->uni.boolValue == 1 ? "#t" : "#f");
        break;
      case 1:
        printf("%d", val->uni.intValue);
        break;
      case 2:
        printf("'", (char)val->uni.quote);
        break;
      case 3:
        printf("%f", val->uni.floatValue);
        break;
      case 4:
        printf("%s", val->uni.symbol);
        break;
      case 5:
        printf("%s", val->uni.stringValue);
        break;
      case 6:
        printf("%c", (char)val->uni.open);
        break;
      case 7:
        printf("%c", (char)val->uni.close);
        break;
      case 8:
            // ADDED THIS TO FIX list
          // OLD VER:
          /*
          current = val->uni.cons;
          while(current != NULL && current->car != NULL)
          {
            printValue(current->car);
            printf(" ");
            current = current->cdr->uni.cons;
          }
          */
        current = val->uni.cons;
        while(current != NULL && current->car != NULL)
        {
          printValue(current->car);
          printf(" ");
          if(current->cdr != NULL && current->cdr->type != 7)
          {
            if(current->cdr->type != 8 && isImproperList(current))
            {
              // Improper list!
              printf(". ");
              printValue(current->cdr);
              printf(" )");
              break;
            }            
            current = current->cdr->uni.cons;
          }
          else
          {
            printf(")");
            break;
          }
        }
        break;
      case 10:
        printf("#<closure>");
        break;
      //default:
        //printf("Value not of primitive type\n");
    }
}


void freeTokens(Value* tokens) {
  Value* current = tokens;
  if (current == NULL) 
  {
    return;
  }
  if(current->type == 8)
  {
    freeTokens(current->uni.cons->car);
    freeTokens(current->uni.cons->cdr);
  }
  free(current);
}


// int main(int argc, char *argv[]) {
//   // SomeKindOfList *tokens;
//   Value* tokens;
//   char *expression = malloc(256 * sizeof(char));
//   while (fgets(expression, 256, stdin)) {
//     tokens = tokenize(expression);
//     printTokens(tokens);
//     freeTokens(tokens);
//   }
//   free(expression);
//   /* also free anything you need to free in the list of tokens */
// }