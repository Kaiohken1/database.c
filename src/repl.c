#include "repl.h"
#include "database.h"
#include "storage.h"


typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_DELETE } StatementType;

typedef struct {
  StatementType type;
  char name[255];
  char selector[50];
  char columns[255];
  char columnName[255];
} Statement;


typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void print_prompt() { printf("db > "); }




void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}


void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void closeBtree(BTree *tr) {
    // printTree(tr);
    freeBTree(tr);
}


MetaCommandResult do_meta_command(InputBuffer* input_buffer, BTree **tr) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    close_input_buffer(input_buffer);
    closeBtree(*tr);
    exit(EXIT_SUCCESS);
  } else if (strcmp(input_buffer->buffer, ".save") == 0) {
    saveBTree(*tr);
    return META_COMMAND_SUCCESS;
  } else if (strcmp(input_buffer->buffer, ".load") == 0) {
    freeBTree(*tr); 
    *tr = loadBTree();
    printTree(*tr, TRUE);
    return META_COMMAND_SUCCESS;
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}


PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {

  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    sscanf(input_buffer->buffer, "insert %s", statement->name);
    return PREPARE_SUCCESS;
  }
  if (strncmp(input_buffer->buffer, "select", 6) == 0) {
    statement->type = STATEMENT_SELECT;
    if (strncmp(input_buffer->buffer, "select *", 8) == 0) {
      sscanf(input_buffer->buffer, "select %s", statement->selector);
      statement->columns[0] = '\0';
    } else {
      sscanf(input_buffer->buffer, "select %[^w] where %[^=] = %[^\n]", statement->columns, statement->columnName, statement->selector);
    }
    return PREPARE_SUCCESS;
  }
  if (strncmp(input_buffer->buffer, "delete", 6) == 0) {
    statement->type = STATEMENT_DELETE;
    sscanf(input_buffer->buffer, "delete where %[^=] = %[^\n]", statement->columnName, statement->selector);
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement, BTree *tr) {
  switch (statement->type) {
    case (STATEMENT_INSERT):
      insertData(statement->name, tr);
      break;
    case (STATEMENT_SELECT):
      if (strncmp(statement->selector, "*", 1) == 0) {
        initSelectAll(tr->root);
      } else {
        initSelect(tr->root, statement->selector, statement->columns);
      }
      break;
    case (STATEMENT_DELETE):
      initDelete(tr, statement->selector);
      break;
  }
}


void repl(void){
  InputBuffer* input_buffer = new_input_buffer();
  BTree *tr =  createTree(NULL);

  while (true) {
    print_prompt();
    read_input(input_buffer);
    if (input_buffer->buffer[0] == '.') {
      switch (do_meta_command(input_buffer, &tr)) {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'\n", input_buffer->buffer);
          continue;
      }
    }
    Statement statement;
    switch (prepare_statement(input_buffer, &statement)) {
      case (PREPARE_SUCCESS):
        printf("recognized statement\n");
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n",
               input_buffer->buffer);
        continue;
    }
     execute_statement(&statement, tr);
     printf("Executed.\n");
  }
}
