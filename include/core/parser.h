#ifndef INCLUDEcoreparserparser
#define INCLUDEcoreparserparser

#include "lexer.h"
#include <stddef.h>

typedef enum {
  OP_STACK,
  OP_ARITHMATIC,
  OP_RELATIONAL,
  OP_IO,
  OP_CONTROL
} CodeType;

typedef enum {
  ST_PUSH,
  ST_POP,
  ST_DUP,
  ST_OVER,
  ST_SWAP,
  ART_ADD,
  ART_SUB,
  ART_MUL,
  ART_DIV,
  ART_MOD,
  REL_EQ,
  REL_NE,
  REL_GT,
  REL_LT,
  REL_GE,
  REL_LE,
  IO_PRINTN,
  IO_PRINTC,
  IO_READN,
  IO_READC,
  CTRL_JMP,
  CTRL_JMPZ,
  CTRL_EXIT
} OPCode;

typedef struct {
  CodeType code;
  OPCode op;
  char *arg;
} OPToken;

typedef struct {
  OPToken *tokens;
  size_t length;
} OPStream;

typedef struct {
  char **names;
  size_t *idxs; // Stores indeces to tokens in OP_Stream
  size_t length;
} LabelList;

OPStream *parse_tokens(TokenStream *tok_stream, LabelList **labels);
size_t *get_label(const LabelList *labels, const char *name);

void print_opstream(const OPStream *stream);
void print_labels(const LabelList *labels);

void free_opstream(OPStream *stream);
void free_labels(LabelList *labels);

#endif // INCLUDEcoreparserparser
