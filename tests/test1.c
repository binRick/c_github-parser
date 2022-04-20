/******************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/******************************************************************/
#include "../deps/commander/commander.c"
#include "../deps/list/list.c"
#include "../deps/list/list_iterator.c"
/******************************************************************/

struct args_t args_t;
struct args_t {
  char *username;
  bool verbose;
}              *args;
struct command *cmd;


static void debug_args(){
  printf("Github Username: %s\n", args->username);
  printf("Verbose Mode: %s\n", args->verbose ? "Enabled" : "Disabled");

  if (cmd->argc != NULL) {
    printf("additional args:\n");
    for (int i = 0; i < cmd->argc; ++i) {
      printf("  - '%s'\n", cmd->argv[i]);
    }
  }
}


static void set_username(command_t *self) {
  args->username = strdup(self->arg);
}


static void verbose(command_t *self) {
  args->verbose = true;
}


void setup_args(const int argc, char **argv){
  args = malloc(sizeof(args_t));
  cmd  = malloc(sizeof(command_t));
  command_init(cmd, argv[0], "0.0.1");
  command_option(cmd, "-v", "--verbose", "Enable Verbose Mode", verbose);
  command_option(cmd, "-u", "--username <username>", "Github Username", set_username);
  command_parse(cmd, argc, argv);
}


int main(const int argc, char **argv) {
  setup_args(argc, argv);
  debug_args();
}
