#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "regs.h"
#include "main.h"
#include "stringlib.h"

void executeDebugCommand(char *command, CPU *cpu);

void usage(char *name) {
    printf("Usage: %s <hexfile> [-d] [-q] [-s]\n", name);
    printf("Options:\n");
    printf("    -s   --step       Enable single-step debug mode\n");
    printf("    -e X --execute X  Run this debug command (\"X\") every instruction executed\n");
    printf("    -f <file>         Run the contents of <file> as if it was passed through -e\n");
}

char *parseArgs(int argc, char ** argv, CPU *cpu) {
    char* fname = NULL;
    if (argc < 2) {
        usage(argv[0]);
        return NULL;
    }
    
    int opt;
    opterr = 0;
    
    while ((opt = getopt(argc, argv, "se:f:")) != -1) {
        switch (opt) {
            case 's':
                cpu->step = true;
                cpu->debug = true;
                break;
            case 'e':
                if (cpu->eachTimeCommand != NULL) {
                    printf("Error: expected exactly one debug command or file");
                    free(cpu->eachTimeCommand);
                    return NULL;
                }
                cpu->eachTimeCommand = malloc(strlen(optarg));
                strcpy(cpu->eachTimeCommand, optarg);
                cpu->debug = true;
                break;
                
            case 'f': {
                cpu->debug = true;
                FILE* fp = fopen(optarg, "r");

                if (fp == NULL) {
                    printf("Error opening debug script file '%s'\n", optarg);
                    return NULL;
                }
                
                fseek(fp, 0, SEEK_END);
                size_t fsize = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                char *contents = malloc(fsize + 1);
                fread(contents, 1, fsize, fp);
                contents[fsize] = 0;
                fclose(fp);

                strrep(contents, '\n', ';');
                if (cpu->eachTimeCommand != NULL) {
                    printf("Error: expected exactly one debug command or file");
                    free(cpu->eachTimeCommand);
                    return NULL;
                }
                cpu->eachTimeCommand = contents;
                break;
            }
                
            case ':':
            case '?':
                printf("Error: unknown option '%c\n", (char)opt);
                usage(argv[0]);
                return NULL;
        }
    }
    
    if (optind < argc) {
        do {
            if (fname != NULL) {
                printf("Error: expected exactly one filename\n");
                usage(argv[0]);
                free(fname);
                return NULL;
            }
            fname = malloc(strlen(argv[optind]));
            strcpy(fname, argv[optind]);
        } while (++optind < argc);
    } else {
        printf("Error: expected file\n");
        usage(argv[0]);
        return NULL;
    }
    return fname;
}

int readFile(char* fname, CPU *cpu) {
    FILE* f = fopen(fname, "r");

    if (f == NULL) {
        printf("Error opening file '%s'\n", fname);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize > 65536) {
        printf("Memory file too large (%lu > 65536)\n", fsize);
        return 1;
    }
    fread(cpu->mem, 1, fsize, f);
    fclose(f);
    return 0;
}

void stepPrompt(CPU *cpu) {
    char *command = NULL;
    while (strcmp(command, "a") != 0 && strcmp(command, "advance") != 0) {
        printf(">> ");
        //size_t size;
        
        //std::getline(std::cin, command);
        executeDebugCommand(command, cpu);
    }
    if (command != NULL) free(command);
}

void executeDebugCommand(char *command, CPU *cpu) {
    char **lines = split(command, ';');
    for (uint32_t line = 0; line < arrlen(lines); line++) {
        char *text = lines[line];

        text = trim(text);
        if (strlen(text) == 0) continue;
        if (strcmp(text, "a") == 0 || strcmp(text, "advance") == 0 || strcmp(text, "break") == 0) continue;
        
        char **args = split(text, ' ');
        char *command = args[0];
        int argc = arrlen(args);
        if (strcmp(command, "regs") == 0) {
            printRegs(cpu);
        } else if (strcmp(command, "mem") == 0) {
            if (argc < 2) {
                printf("'mem' debug command takes two arguments: start address and end address (hex)\n");
            }
            uint32_t startAddr = 0;
            uint32_t endAddr = 0;
            //std::stringstream ss;
            //ss << std::hex << args[0];
            //ss >> startAddr;
            //
            //std::stringstream ss2;
            //ss2 << std::hex << args[1];
            //ss2 >> endAddr;

            for (uint32_t i = startAddr; i < endAddr; i += 8)  {
                for (uint32_t j = 0; j < 8 && i + j < endAddr; j++) {
                    printf("%#02X ", cpu->mem[i + j]);
                }
                printf("\n");
            }
        } else if (strcmp(command, "instr") == 0) {
            printf("Next instr: %#02X\n", cpu->mem[cpu->pc]);
        } else if (strcmp(command, "lastinstr") == 0) {
            if (cpu->pc > 0) {
                printf("Last instr: %#02X\n", cpu->lastInstr);
            } else {
                printf("PC is 0: no last instruction\n");
            }
        } else if (strcmp(command, "print") == 0) {
            for (int i = 0; i < argc; i++) {
                printf("%s", args[i]);
            }
            printf("\n");
        } else if (strcmp(command, "help") == 0) {
            if (argc == 0) {
                printf("Commands:\n");
                printf("regs:      Print registers\n");
                printf("mem:       Print a specified range of bytes from memory\n");
                printf("instr:     Print the next instruction to be executed\n");
                printf("lastinstr: Print the last instruction that was executed\n");
                printf("advance:   Advance the program counter and execute next instruction\n");
                printf("a:         Alias of advance\n");
                printf("print x:   Print a specified string or expression (see 'help expressions')\n");
            } else {
                if (strcmp(args[0], "expressions") == 0) {
                    printf("WIP\n");
                }
            }
        } else {
            printf("Unknown command '%s'\nTry 'help' for help.\n", command);
        }
        for (size_t i = 0; i < arrlen(args); i++) {
            free(args[i]);
        }
        free(args);
    }
    for (size_t i = 0; i < arrlen(lines); i++) {
        free(lines[i]);
    }
    free(lines);
}