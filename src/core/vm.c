#include "../../include/core/vm.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Globals
size_t SP;
size_t PC;

// Debug: To print the stack
void print_stack(int32_t stack[]) {
  printf("Stack[%zu]: {", SP);
  for (size_t i = 0; i < SP; i++) {
    printf("%d", stack[i]);
    if (i + 1 < SP)
      printf(", ");
  }
  printf("}\n");
}

int execute_stack_op(int32_t stack[], const OPToken token) {
  switch (token.op) {
  case ST_PUSH: {
    stack[SP++] = atoi(token.arg);
    break;
  }
  case ST_POP: {
    if (SP == 0) {
      printf("Runtime error: can't pop an empty stack.");
      return 1;
    }
    SP--;
    break;
  }
  case ST_DUP: {
    if (SP == 0) {
      printf("Runtime error: can't duplicate, stack is empty.");
      return 1;
    }
    stack[SP] = stack[SP - 1];
    SP++;
    break;
  }
  case ST_OVER: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `over`.");
      return 1;
    }
    stack[SP] = stack[SP - 2];
    SP++;
    break;
  }
  case ST_SWAP: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `swap`.");
      return 1;
    }
    stack[SP - 1] = stack[SP - 2];
    break;
  }
  default:
    break;
  }
  return 0;
}

int execute_arithmatic_op(int32_t stack[], OPToken token) {
  switch (token.op) {
  case ART_ADD: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `add`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] + stack[SP - 1];
    SP--;
    break;
  }
  case ART_SUB: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `sub`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] - stack[SP - 1];
    SP--;
    break;
  }
  case ART_MUL: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `mul`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] * stack[SP - 1];
    SP--;
    break;
  }
  case ART_DIV: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `div`.");
      return 1;
    }
    int32_t op2 = stack[--SP];
    int32_t op1 = stack[--SP];
    if (op2 == 0) {
      printf("Runtime error: division by zero.");
      return 1;
    }
    stack[SP++] = op1 / op2;
    break;
  }
  case ART_MOD: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `mod`.");
      return 1;
    }
    int32_t op2 = stack[--SP];
    int32_t op1 = stack[--SP];
    if (op2 == 0) {
      printf("Runtime error: division by zero.");
      return 1;
    }
    stack[SP++] = op1 % op2;
    break;
  }
  default:
    break;
  }
  return 0;
}

int execute_relational_op(int32_t stack[], OPToken token) {
  switch (token.op) {
  case REL_EQ: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `eq`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] == stack[SP - 1];
    SP--;
    break;
  }
  case REL_NE: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `ne`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] != stack[SP - 1];
    SP--;
    break;
  }
  case REL_GT: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `gt`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] > stack[SP - 1];
    SP--;
    break;
  }
  case REL_LT: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `lt`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] < stack[SP - 1];
    SP--;
    break;
  }
  case REL_GE: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `ge`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] >= stack[SP - 1];
    SP--;
    break;
  }
  case REL_LE: {
    if (SP < 2) {
      printf(
          "Runtime error: stack must contain atleast two elements for `le`.");
      return 1;
    }
    stack[SP - 2] = stack[SP - 2] <= stack[SP - 1];
    SP--;
    break;
  }
  default:
    break;
  }
  return 0;
}

int execute_io_op(int32_t stack[], OPToken token) {
  switch (token.op) {
  case IO_PRINTN: {
    if (SP == 0) {
      printf("Runtime error: stack must contain atleast one element for "
             "`printn`.");
      return 1;
    }
    printf("%d", stack[--SP]);
    break;
  }
  case IO_PRINTC: {
    if (SP == 0) {
      printf("Runtime error: stack must contain atleast one element for "
             "`printc`.");
      return 1;
    }
    printf("%c", (char)stack[--SP]);
    break;
  }
  case IO_READN: {
    scanf("%d", &stack[SP++]);
    break;
  }
  case IO_READC: {
    char ch;
    scanf("%c", &ch);
    stack[SP++] = (int32_t)ch;
    break;
  }
  default:
    break;
  }
  return 0;
}

// Main executer //
int execute_instruction(int32_t stack[], const OPToken inst,
                        const LabelList *labels) {
  switch (inst.code) {
  case OP_STACK:
    return execute_stack_op(stack, inst);
  case OP_ARITHMATIC:
    return execute_arithmatic_op(stack, inst);
  case OP_RELATIONAL:
    return execute_relational_op(stack, inst);
  case OP_IO:
    return execute_io_op(stack, inst);
  case OP_CONTROL: {
    if (inst.op == CTRL_JMP) {
      size_t *idx = get_label(labels, inst.arg);
      if (!idx) {
        printf("Runtime error: cannot jump to an undefined label: %s.",
               inst.arg);
        return 1; // error code
      }
      PC = *idx;
      return 2; // jump code
    } else if (inst.op == CTRL_JMPZ) {
      size_t *idx = get_label(labels, inst.arg);
      if (!idx) {
        printf("Runtime error: cannot jump to an undefined label: %s.",
               inst.arg);
        return 1;
      }
      if (SP == 0) {
        printf("Runtime error: stack must contain an element as argument for "
               "`jmpz`.");
        return 1;
      }
      if (stack[SP - 1] == 0) {
        SP--;
        PC = *idx;
        return 2; // jump code
      }
    } else if (inst.op == CTRL_EXIT)
      return -1; // exit code
  }
  }
  return 0;
}

int execute_bytecode(const OPStream *bytecode, const LabelList *labels) {
  int32_t stack[MAX_STACK_DEPTH];
  PC = 0;
  SP = 0;

  while (PC < bytecode->length) {
    int ret = execute_instruction(stack, bytecode->tokens[PC], labels);
    if (ret == -1)
      break; // exit success
    else if (ret == 1)
      return 1; // error
    else if (ret == 2)
      continue; // jump exec
    PC++;
  }

  return 0;
}

// Helper: checkss stringg  equalityty
int equals(const char *lhs, const char *rhs) { return strcmp(lhs, rhs) == 0; }

void print_breakpoints(const size_t *bps, size_t count,
                       const LabelList *labels) {
  printf("Breakpoints[%zu]: {", count);
  for (size_t i = 0; i < labels->length; i++) {
    for (size_t j = 0; j < count; j++) {
      if (labels->idxs[i] == bps[j]) {
        printf("%s = %zu", labels->names[i], bps[j]);
        if (j + 1 < count)
          printf(", ");
        break;
      }
    }
  }
  printf("}\n");
}

int8_t contains_bp(size_t breakpoints[], size_t count, size_t bp) {
  for (size_t i = 0; i < count; i++)
    if (breakpoints[i] == bp)
      return 1;
  return 0;
}

int debug_bytecode(const OPStream *bytecode, const LabelList *labels) {
  size_t *breakpoints = (size_t *)malloc(sizeof(size_t) * labels->length);
  size_t bp_count = 0;
  int32_t stack[MAX_STACK_DEPTH];
  PC = 0;
  SP = 0;

  int ret = 99;
  char line[256];
  char cmd[64];
  char arg[64];
  while (1) {
    printf("$ ");
    fgets(line, 255, stdin);
    line[strlen(line) - 1] = '\0'; // Get rid of the newline
    arg[0] = '\0';
    sscanf(line, "%s %s", cmd, arg);

    if (equals(cmd, "run") || equals(cmd, "r")) {
      PC = 0;
      while (PC < bytecode->length && !contains_bp(breakpoints, bp_count, PC)) {
        ret = execute_instruction(stack, bytecode->tokens[PC++], labels);
        if (ret == -1) {
          ret = 0; // exit success
          break;
        } else if (ret == 2) {
          continue; // dont want to update PC
        } else if (ret == 1)
          break;
        ret = 99;
      }
      if (PC < bytecode->length) {
        printf("[?] Execution paused at breakpoint. PC = %zu\n", PC);
      }
    } else if (equals(cmd, "quit") || equals(cmd, "q")) {
      printf("\n[Quitting...]\n");
      break;
    } else if (equals(cmd, "stack") || equals(cmd, "stk")) {
      print_stack(stack);
    } else if (equals(cmd, "labels") || equals(cmd, "lab")) {
      print_labels(labels);
    } else if (equals(cmd, "pc")) {
      OPToken t = bytecode->tokens[PC];
      printf("PC: %zu (%d %s)\n", PC, t.op, t.arg);
    } else if (equals(cmd, "breakpoints") || equals(cmd, "bps")) {
      print_breakpoints(breakpoints, bp_count, labels);
    } else if (equals(cmd, "breakpoint") || equals(cmd, "bp")) {
      if (arg[0] == '\0') {
        printf("Debug: please provide a label tp set as a breakpoint (run `lb` "
               "to get all labels)\n");
      } else if (bp_count < labels->length) {
        size_t *lb_pos = get_label(labels, arg);
        if (!lb_pos) {
          printf("Debug: label (%s) does not exist in your program (run `lb` "
                 "to list all labels)\n",
                 arg);
        } else {
          printf("[?] Label `%s` added, pos = %zu\n", arg, *lb_pos);
          breakpoints[bp_count++] = *lb_pos;
        }
      }
    } else {
      printf("Invalid command: '%s'\n", cmd);
    }

    if (ret == 0) {
      printf("\n[Program exited without errors]\n");
      ret = 99;
    } else if (ret == 1) {
      printf("\n[Program exited with error]\n");
      ret = 99;
    }
  }

  free(breakpoints);
  return 0;
}
