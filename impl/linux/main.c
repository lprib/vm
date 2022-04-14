#include "li_Interpret.h"
#include "vm_ProcessOpcode.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>

const char * cmd_version = "VM linux interpreter 1.0";
const char * cmd_address = "<jackpribis@gmail.com>";
static char cmd_doc[] = "Interpret VM bytecode";
static char cmd_args_doc[] = "[FILENAME]";

typedef struct
{
    char * input_file;
} arguments_t;

static error_t parse_opt(int key, char * arg, struct argp_state * state)
{
    arguments_t * arguments = state->input;
    switch (key)
    {
    case 't':
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num > 0)
        {
            // Only support one arg
            fprintf(stderr, "Only one input file supported.\n\n");
            argp_usage(state);
        }
        arguments->input_file = arg;
        break;
    case ARGP_KEY_END:
        if (state->arg_num > 1)
        {
            fprintf(stderr, "[FILENAME] required\n\n");
            argp_usage(state);
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

// Allow missing fields for argp
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

static struct argp_option cmd_options[] = {
    {"test", 't', "TEST_INPUT", 0, "A test cmdline flag"}};

static struct argp cmd_argp = {cmd_options, parse_opt, cmd_args_doc, cmd_doc};

#pragma GCC diagnostic pop

int main(int argc, char ** argv)
{
    arguments_t args;
    args.input_file = NULL;
    argp_parse(&cmd_argp, argc, argv, 0, 0, &args);
    li_InitInterpreter(100, 100);
    li_loadProgramResult_t res = li_LoadProgram(args.input_file);
    switch (res)
    {
    case LOAD_FAILURE_COULD_NOT_OPEN_FILE:
        fprintf(stderr, "Could not open input file\n");
        exit(1);
        break;
    case LOAD_FAILURE_NOT_ENOUGH_MEM:
        fprintf(stderr, "Not enough allocated to load program\n");
        exit(1);
        break;
    case LOAD_SUCCESS:
        break;
    }

    vm_programTickResult_t run_result = li_RunProgram();
    switch (run_result)
    {
    case VM_PROCESS_CONTINUE:
        break;
    case VM_PROCESS_PROGRAM_HALT:
        printf("Finished successfully.\n");
        break;
    case VM_PROCESS_ERROR_UNDEF_IO_FN:
        printf("Undefined IO function call\n");
        break;
    case VM_PROCESS_ERROR_INVALID_OPCODE:
        printf("Invalid opcode\n");
    }
    li_DestroyInterpreter();
}
