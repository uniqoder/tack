#ifndef INCLUDEcorevmvm
#define INCLUDEcorevmvm

#include "parser.h"

#define MAX_STACK_DEPTH 256

int execute_bytecode(const OPStream *bytecode, const LabelList *labels);
int debug_bytecode(const OPStream *bytecode, const LabelList *labels);

#endif // INCLUDEcorevmvm
