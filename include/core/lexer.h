#ifndef INCLUDEcorelexerlexer
#define INCLUDEcorelexerlexer

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  TOK_IDENT,
  TOK_NUM,
  TOK_CHAR,
  TOK_PUNCT,
  TOK_EOF,
  TOK_EOL,
  TOK_UNK
} TokenType;

typedef struct {
  TokenType type;
  char *value;
  size_t line, col;
} Token;

typedef struct {
  Token *tokens;
  size_t length;
  size_t idx;
} TokenStream;

TokenStream *tokenize(const char *code, bool verbose);

Token ts_seek(const TokenStream *stream, const int offset);
void ts_consume(TokenStream *stream, const TokenType type, const char *value);
Token ts_advance(TokenStream *stream);

const char *tokentype_str(TokenType type);
void free_tokens(TokenStream *stream);
void print_tokens(TokenStream *stream);

#endif // !INCLUDEcorelexerlexer
