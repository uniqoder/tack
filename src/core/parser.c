#include "../../include/core/parser.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Utility //
bool isEqual(const char *lhs, const char *rhs) { return strcmp(lhs, rhs) == 0; }

char *getArgNum(TokenStream *stream) {
  Token t = ts_seek(stream, 0);

  if (t.type == TOK_NUM) {
    ts_advance(stream);
    return strdup(t.value);
  }

  if (t.type == TOK_CHAR) {
    ts_advance(stream);
    char n_str[64];
    sprintf(n_str, "%d", t.value[0]);
    return strdup(n_str);
  }

  ts_consume(stream, TOK_EOL, "\\n");
  return strdup("0");
}

char *getArgLabel(TokenStream *stream) {
  Token t = ts_seek(stream, 0);
  if (t.type != TOK_IDENT) {
    printf("Parsing error [%zu:%zu]: invalid argument as label name: '%s'\n",
           t.line, t.col, t.value);
    exit(EXIT_FAILURE);
  }

  ts_advance(stream);
  return strdup(t.value);
}

// Helpers for Setup //
OPStream *stream_setup(size_t cap) {
  OPStream *stream = (OPStream *)malloc(sizeof(OPStream));
  if (!stream) {
    printf("Parsing error []: couldn't allocate memory for OP Stream.\n");
    exit(EXIT_FAILURE);
  }

  stream->tokens = (OPToken *)malloc(sizeof(OPToken) * cap);
  if (!stream->tokens) {
    printf("Parsing error []: couldn't allocate memory for OP Stream.\n");
    exit(EXIT_FAILURE);
  }

  stream->length = 0;
  return stream;
}

LabelList *labellist_setup(size_t cap) {
  LabelList *labels = (LabelList *)malloc(sizeof(LabelList));
  if (!labels) {
    printf("Parsing error []: couldn't allocate memory for Label List.\n");
    exit(EXIT_FAILURE);
  }

  labels->length = 0;
  labels->names = (char **)malloc(cap);
  labels->idxs = (size_t *)malloc(sizeof(size_t) * cap);
  if (!labels->names || !labels->idxs) {
    printf("Parsing error []: couldn't allocate memory for Label List.\n");
    exit(EXIT_FAILURE);
  }

  return labels;
}

// List Resize Handlers //
void stream_resize(OPStream *stream, size_t *cap) {
  *cap *= 2;
  stream->tokens = (OPToken *)realloc(stream->tokens, sizeof(OPToken) * *cap);
  if (!stream->tokens) {
    printf("Parsing error []: couldn't reallocate memory for OP Stream.\n");
    exit(EXIT_FAILURE);
  }
}

void label_resize(LabelList *labels, size_t *cap) {
  *cap *= 2;
  labels->names = (char **)realloc(labels->names, *cap);
  labels->idxs = (size_t *)realloc(labels->names, sizeof(size_t) * *cap);
  if (!labels->names || !labels->idxs) {
    printf("Parsing error []: couldn't reallocate memory for Label List.\n");
    exit(EXIT_FAILURE);
  }
}

// Main Parser //
OPStream *parse_tokens(TokenStream *tok_stream, LabelList **labels) {
  size_t token_cap = 10, label_cap = 10;
  OPStream *stream = stream_setup(token_cap);
  *labels = labellist_setup(label_cap);

  Token tok;
  while (true) {
    tok = ts_seek(tok_stream, 0);
    if (tok.type == TOK_EOF)
      break;

    // Dynamic lists resize
    if (stream->length >= token_cap)
      stream_resize(stream, &token_cap);
    if ((*labels)->length >= label_cap)
      label_resize(*labels, &label_cap);

    if (tok.type == TOK_EOL) {
      ts_consume(tok_stream, TOK_EOL, "\\n");
      continue;
    }

    // Main handler
    if (tok.type == TOK_IDENT) {
      char *id = tok.value;
      tok = ts_advance(tok_stream);

      // Handle labels
      if (isEqual(tok.value, ":")) {
        ts_consume(tok_stream, TOK_PUNCT, ":");
        (*labels)->names[(*labels)->length] = strdup(id);
        (*labels)->idxs[(*labels)->length++] = stream->length;
      }

      // Handle stack operations
      else if (isEqual(id, "push"))
        stream->tokens[stream->length++] =
            (OPToken){OP_STACK, ST_PUSH, getArgNum(tok_stream)};
      else if (isEqual(id, "pop"))
        stream->tokens[stream->length++] = (OPToken){OP_STACK, ST_POP, NULL};
      else if (isEqual(id, "dup"))
        stream->tokens[stream->length++] = (OPToken){OP_STACK, ST_DUP, NULL};
      else if (isEqual(id, "over"))
        stream->tokens[stream->length++] = (OPToken){OP_STACK, ST_OVER, NULL};
      else if (isEqual(id, "swap"))
        stream->tokens[stream->length++] = (OPToken){OP_STACK, ST_SWAP, NULL};

      // Handle arithmatic operations
      else if (isEqual(id, "add"))
        stream->tokens[stream->length++] =
            (OPToken){OP_ARITHMATIC, ART_ADD, NULL};
      else if (isEqual(id, "sub"))
        stream->tokens[stream->length++] =
            (OPToken){OP_ARITHMATIC, ART_SUB, NULL};
      else if (isEqual(id, "mul"))
        stream->tokens[stream->length++] =
            (OPToken){OP_ARITHMATIC, ART_MUL, NULL};
      else if (isEqual(id, "div"))
        stream->tokens[stream->length++] =
            (OPToken){OP_ARITHMATIC, ART_DIV, NULL};
      else if (isEqual(id, "mod"))
        stream->tokens[stream->length++] =
            (OPToken){OP_ARITHMATIC, ART_MOD, NULL};

      // Handle relational operations
      else if (isEqual(id, "eq"))
        stream->tokens[stream->length++] =
            (OPToken){OP_RELATIONAL, REL_EQ, NULL};
      else if (isEqual(id, "ne"))
        stream->tokens[stream->length++] =
            (OPToken){OP_RELATIONAL, REL_NE, NULL};
      else if (isEqual(id, "lt"))
        stream->tokens[stream->length++] =
            (OPToken){OP_RELATIONAL, REL_LT, NULL};
      else if (isEqual(id, "gt"))
        stream->tokens[stream->length++] =
            (OPToken){OP_RELATIONAL, REL_GT, NULL};
      else if (isEqual(id, "le"))
        stream->tokens[stream->length++] =
            (OPToken){OP_RELATIONAL, REL_LE, NULL};
      else if (isEqual(id, "ge"))
        stream->tokens[stream->length++] =
            (OPToken){OP_RELATIONAL, REL_GE, NULL};

      // Handle I/O operations
      else if (isEqual(id, "printn"))
        stream->tokens[stream->length++] = (OPToken){OP_IO, IO_PRINTN, NULL};
      else if (isEqual(id, "printc"))
        stream->tokens[stream->length++] = (OPToken){OP_IO, IO_PRINTC, NULL};
      else if (isEqual(id, "readn"))
        stream->tokens[stream->length++] = (OPToken){OP_IO, IO_READN, NULL};
      else if (isEqual(id, "readc"))
        stream->tokens[stream->length++] = (OPToken){OP_IO, IO_READC, NULL};

      // Handle control operations
      else if (isEqual(id, "jmp"))
        stream->tokens[stream->length++] =
            (OPToken){OP_CONTROL, CTRL_JMP, getArgLabel(tok_stream)};
      else if (isEqual(id, "jmpz"))
        stream->tokens[stream->length++] =
            (OPToken){OP_CONTROL, CTRL_JMPZ, getArgLabel(tok_stream)};
      else if (isEqual(id, "exit"))
        stream->tokens[stream->length++] =
            (OPToken){OP_CONTROL, CTRL_EXIT, NULL};

      // Handle unknown identifiers
      else {
        Token t = ts_seek(tok_stream, -1);
        printf("Parsing error [%zu:%zu]: encountered unexpected identifier: "
               "\"%s\", was this a label? Did you forget the ':'?\n",
               t.line, t.col, id);
        exit(EXIT_FAILURE);
      }
    } else {
      printf("Parsing error [%zu:%zu]: line must start with an identifier, not "
             "a(n) %s token.\n",
             tok.line, tok.col, tokentype_str(tok.type));
      exit(EXIT_FAILURE);
    }
  }

  return stream;
}

// Label Locater //
size_t *get_label(const LabelList *labels, const char *name) {
  for (size_t i = 0; i < labels->length; i++) {
    if (isEqual(labels->names[i], name))
      return &labels->idxs[i];
  }
  return NULL;
}

// Debugging Helpers
void print_opstream(const OPStream *stream) {
  if (!stream)
    return;

  for (size_t i = 0; i < stream->length; i++) {
    printf("%zu. %2d \"%s\"\n", i + 1, stream->tokens[i].op,
           stream->tokens[i].arg);
  }
}

void print_labels(const LabelList *labels) {
  if (!labels)
    return;

  printf("Labels: [");
  for (size_t i = 0; i < labels->length; i++) {
    printf("%s = %zu", labels->names[i], labels->idxs[i]);
    if (i + 1 < labels->length)
      printf(", ");
  }
  printf("]\n");
}

// Deallocators //
void free_opstream(OPStream *stream) {
  if (!stream)
    return;

  for (size_t i = 0; i < stream->length; i++) {
    if (stream->tokens[i].arg)
      free(stream->tokens[i].arg);
  }

  if (stream->tokens)
    free(stream->tokens);
  free(stream);
}

void free_labels(LabelList *labels) {
  if (!labels)
    return;
  for (size_t i = 0; i < labels->length; i++) {
    if (labels->names[i])
      free(labels->names[i]);
  }

  if (labels->names)
    free(labels->names);
  if (labels->idxs)
    free(labels->idxs);
  if (labels)
    free(labels);
}
