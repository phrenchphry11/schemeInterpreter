
typedef struct __Closure
{
    struct __Environment* env;
    struct __ConsCell* params; //list of parameters.
    struct __Value* (*functionPtr)(struct __ConsCell*, struct __Environment*);
    struct __ConsCell* toExecute;
} Closure;

typedef struct __Environment //list of frames
{
    struct __ConsCell* head;
} Environment;

typedef struct __Frame //list of bindings no more than 1 binding per variable per frame
{
  struct __ConsCell* head;
} Frame;

typedef struct __ConsCell {
  struct __Value* car;
  struct __Value* cdr;
} ConsCell;

typedef struct __Binding
{
    char* symbol; //name
    struct __Value* val; //value
} Binding;

typedef struct __Value {
  char type; /* Make sure type includes a consType or some such */
  union {         // TYPE
    int boolValue; // 0
    int intValue; // 1
    char quote; // 2
    float floatValue; // 3
    char* symbol; // 4
    char* stringValue; // 5
    char open; // 6
    char close; // 7
    struct __ConsCell *cons; // 8
    struct __Value* (*primitiveValue)(struct __ConsCell*, struct __Environment*); //9
    struct __Closure *closureValue; //10
    struct __Binding* binding; //11
    struct __Frame* frame; //12
  } uni;
} Value;

static void** pointerList;
static int pointerListLength;
static int maxPointerListLength = 250000;

void assign(void* toAssign);
void freePointerList();



char determineType(char* token, int size);


// Takens in a token, then updates our linkedlist/pointer stuff accordingly
Value* addToken(char* current, int size, Value* head, Value** linkedListIterator);

Value* tokenize(char* exp);

void printTokens(Value* tokens);

void freeTokens(Value* tokens);

void printValue(Value* val);
