#include "../../include/core/lexer.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// Helper: creates a token
Token new_token(const TokenType type, const char *value, const size_t line,
                const size_t col) {
  Token tok;
  tok.type = type;
  if (value) {
    tok.value = (char *)malloc(strlen(value) + 1);
    strcpy(tok.value, value);
  } else
    tok.value = NULL;
  tok.line = line;
  tok.col = col;

  return tok;
}

TokenStream *tokenize(const char *code, bool verbose) {
  size_t stream_cap = 10;
  TokenStream *stream = (TokenStream *)malloc(sizeof(TokenStream));
  if (!stream) {
    printf("Lexing error: Couldn't allocate memory for TokenStream\n");
    exit(EXIT_FAILURE);
  }

  stream->tokens = (Token *)malloc(
      sizeof(Token) * (stream_cap + 1)); // The +1 will ensure space for EOF
  stream->length = stream->idx = 0;

  if (!stream->tokens) {
    printf("Lexing error: Couldn't allocate memory for TokenStream\n");
    exit(EXIT_FAILURE);
  }

  if (verbose) {
    printf(
        "Stream: Created. Stream Token Array: Initialized with size = %ld.\n",
        sizeof(Token) * (stream_cap + 1));
  }

  size_t line = 1, col = 1;
  size_t i = 0;

  while (code[i] != '\0') {
    // Handle array resize
    if (stream->length >= stream_cap) {
      stream_cap *= 2;

      if (verbose) {
        printf("Stream Token Array is full. Resizing from %zu to %zu.\n",
               sizeof(Token) * (stream_cap / 2 + 1),
               sizeof(Token) * (stream_cap + 1));
      }

      stream->tokens =
          (Token *)realloc(stream->tokens, sizeof(Token) * (stream_cap + 1));
      if (!stream->tokens) {
        printf("Lexing error: Couldn't reallocate memory for Tokenstream->\n");
        exit(EXIT_FAILURE);
      }
    }

    // Handle whitespace
    if (code[i] == '\n') {
      if (verbose)
        printf("End of Line token added.\n");

      stream->tokens[stream->length++] = new_token(TOK_EOL, "\\n", line, col);
      line++;
      i++;
      col = 1;
    } else if (isspace((unsigned char)code[i])) {
      i++;
      col++;
    }

    // Handle comments
    else if (code[i] == '#') {
      line++;
      col = 1;
      while (code[i] && code[i] != '\n')
        i++;
      i++; // Consume newline
    }

    // Handle identifiers
    else if (isalpha((unsigned char)code[i]) || code[i] == '_') {
      size_t start = i++;
      while (isalnum((unsigned char)code[i]) || code[i] == '_')
        i++;

      char *value = (char *)malloc(i - start + 1); // +1 is for '\0'
      memcpy(value, code + start, i - start);
      value[i - start] = '\0';

      stream->tokens[stream->length++] = new_token(TOK_IDENT, value, line, col);
      if (verbose)
        printf("Identifier token added: '%s'.\n", value);

      free(value);
      col += i - start;
    }

    // Handle numbers
    else if (isdigit((unsigned char)code[i])) {
      size_t start = i++;
      while (isdigit((unsigned char)code[i]))
        i++;
      char *value = (char *)malloc(i - start + 1);
      memcpy(value, code + start, i - start);
      value[i - start] = '\0';

      stream->tokens[stream->length++] = new_token(TOK_NUM, value, line, col);
      if (verbose)
        printf("Number token added: '%s'.\n", value);

      free(value);
      col += i - start;
    }

    // Handle character constants
    else if (code[i] == '\'') {
      col++;
      size_t start = ++i;
      while (code[i] && code[i] != '\'' && code[i] != '\n')
        i++;
      char *value = (char *)malloc(i - start + 1);
      memcpy(value, code + start, i - start);
      value[i - start] = '\0';

      stream->tokens[stream->length++] = new_token(TOK_CHAR, value, line, col);
      if (verbose)
        printf("Character token added: '%s'.\n", value);

      free(value);
      col += i - start;

      if (code[i] != '\'') {
        printf("Lexing Error [%zu:%zu]: Opened quote wasn't closed.\n", line,
               col);
        exit(EXIT_FAILURE);
      }
      i++; // Skip ending quote
    }

    // Handle punctuation
    else if (ispunct((unsigned char)code[i])) {
      char value[] = {code[i], '\0'};
      stream->tokens[stream->length++] =
          new_token(TOK_PUNCT, value, line, col++);

      if (verbose)
        printf("Punctuation token added: '%s'.\n", value);

      i++;
    }

    // Handle unknown crap
    else {
      char value[] = {code[i], '\0'};
      stream->tokens[stream->length++] = new_token(TOK_UNK, value, line, col++);
      if (verbose)
        printf("Unknown token added: '%s'.\n", value);

      i++;
    }
  }

  // Append EOF token at the end
  stream->tokens[stream->length++] = new_token(TOK_EOF, NULL, line, col);
  if (verbose)
    printf("EOF token added.\n");

  return stream;
}

// Helper: returns a string corresponding to the token type
const char *tokentype_str(TokenType type) {
  switch (type) {
  case TOK_PUNCT:
    return "punctuation";
  case TOK_IDENT:
    return "identifier";
  case TOK_NUM:
    return "number";
  case TOK_EOL:
    return "end-of-line";
  case TOK_EOF:
    return "end-of-file";
  case TOK_UNK:
    return "unknown";
  default:
    return "undefined-token";
  }
}

// Stream methods //
Token ts_seek(const TokenStream *stream, const int offset) {
  if (!stream)
    return (Token){TOK_EOF, NULL, 0, 0};

  // Calculate idx while handling overflow
  ssize_t idx = (ssize_t)stream->idx + offset;
  if (idx >= 0 && idx < (ssize_t)stream->length)
    return stream->tokens[idx];
  return stream->tokens[stream->length - 1];
}

Token ts_advance(TokenStream *stream) {
  if (!stream)
    return (Token){TOK_EOF, NULL, 0, 0};

  if (stream->idx < stream->length - 1)
    stream->idx++;
  return ts_seek(stream, 0);
}

void ts_consume(TokenStream *stream, const TokenType type, const char *value) {
  if (!stream)
    return;

  Token tok = ts_seek(stream, 0);
  if (tok.type != type || strcmp(tok.value, value) != 0) {
    printf("Parsing error [%zu:%zu]: expected token: (%s) \"%s\", got (%s) "
           "\"%s\".\n",
           tok.line, tok.col, tokentype_str(type), value,
           tokentype_str(tok.type), tok.value);
    exit(EXIT_FAILURE);
  }

  ts_advance(stream);
}

void free_tokens(TokenStream *stream) {
  if (!stream)
    return;

  for (size_t i = 0; i < stream->length; i++)
    if (stream->tokens[i].value)
      free(stream->tokens[i].value);
  free(stream->tokens);
  free(stream);
}

void print_tokens(TokenStream *stream) {
  if (!stream)
    return;

  size_t org_idx = stream->idx;
  Token t = ts_seek(stream, 0);

  size_t srno = 1;
  while (t.type != TOK_EOF) {
    printf("%zu. (%s) \"%s\"[%zu:%zu]\n", srno++, tokentype_str(t.type),
           t.value, t.line, t.col);
    t = ts_advance(stream);
  }

  stream->idx = org_idx;
}
