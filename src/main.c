#include "../include/core/lexer.h"
#include "../include/core/parser.h"
#include "../include/core/vm.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bool warn;
  bool debug;
  bool verbose;
  char *filename;
} CliOptions;

// Helpers
void print_usage(char *exec_name) {
  printf("Usage: %s [options] <src file>\n", exec_name);
}

void print_help(char *exec_name) {
  printf("Usage: %s [options] <src file>\n", exec_name);
  printf("Options:\n");
  printf("\t-w  :  Enables warnings (not implemented).\n");
  printf("\t-d  :  Allows you to step through your program to debug it (not "
         "implemented).\n");
  printf("\t-v  :  Echoes everything its doing.\n");
  printf("\t-h  :  Shows this message.\n");
}

CliOptions parse_args(int argc, char *argv[]) {
  if (argc == 1) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  CliOptions opts;
  memset(&opts, 0, sizeof(opts));

  for (int i = 1; i < argc; i++) {
    char *opt = argv[i];
    if (opt[0] == '-') {
      for (int j = 1; opt[j]; j++) {
        switch (opt[j]) {
        case 'w':
          opts.warn = true;
          break;
        case 'd':
          opts.debug = true;
          break;
        case 'v':
          opts.verbose = true;
          break;
        case 'h': {
          print_help(argv[0]);
          exit(EXIT_SUCCESS);
        }
        default: {
          printf("CLI error: use if invalid option: -%c.\nType `%s -h` for "
                 "help.\n",
                 opt[j], argv[0]);
          exit(EXIT_FAILURE);
        }
        }
      }
    } else if (!opts.filename)
      opts.filename = argv[i];
    else {
      printf("CLI error: invalid argument: %s. Only provide a single file.\n",
             argv[i]);
      exit(EXIT_FAILURE);
    }
  }

  if (!opts.filename) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  return opts;
}

int main(int argc, char *argv[]) {
  CliOptions options = parse_args(argc, argv);

  // Read the src file
  if (options.verbose) {
    printf("[?] Options passed: %s%s%s\n", (options.warn ? "warn " : ""),
           (options.debug ? "debug " : ""),
           (options.verbose ? "verbose " : ""));
    printf("[?] Opening input file: %s\n", options.filename);
  }

  FILE *fp = fopen(options.filename, "r");
  fseek(fp, 0, SEEK_END);
  const size_t length = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (options.verbose)
    printf("[?] File length detected: %zu\n", length);

  char *src_buff = (char *)malloc(length + 1);
  fread(src_buff, 1, length, fp);
  src_buff[length] = '\0';
  fclose(fp);

  if (options.verbose) {
    printf("[?] File read and closed\n[?] Tokenizing...\n");
    printf("\n===== Src Code =====\n");
    printf("%s\n", src_buff);
  }

  TokenStream *tok_stream = tokenize(src_buff, options.verbose);
  free(src_buff);

  if (options.verbose) {
    printf("\n[?] Tokenization done, %zu tokens generated\n",
           tok_stream->length);
    printf("\n===== Tokens =====\n");
    print_tokens(tok_stream);
    printf("[?] Parsing tokens...\n");
  }

  LabelList *labels = NULL;
  OPStream *bytecode = parse_tokens(tok_stream, &labels);

  free_tokens(tok_stream);

  if (options.verbose) {
    printf("\n[?] Parsing done, %zu tokens generated\n", bytecode->length);
    printf("\n===== Labels =====\n");
    print_labels(labels);

    printf("\n===== Byte Code =====\n");
    print_opstream(bytecode);

    printf("[?] Executing bytecode...\n");
    printf("\n===== Output =====\n");
  }

  if (options.debug)
    debug_bytecode(bytecode, labels);
  else
    execute_bytecode(bytecode, labels);

  free_opstream(bytecode);
  free_labels(labels);
  return 0;
}
