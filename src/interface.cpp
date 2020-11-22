#include <stdio.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <limits>
#include <vector>
#include <algorithm>
#include "regs.h"
#include "main.h"
#include "stringlib.h"

using namespace std;

void executeDebugCommand(std::string arg);

void usage(char * name) {
    printf("Usage: %s <hexfile> [-d] [-q] [-s]\n", name);
    printf("Options:\n");
    printf("    -s   --step       Enable single-step debug mode\n");
    printf("    -e X --execute X  Run this debug command (\"X\") every instruction executed\n");
    printf("    -f <file>         Run the contents of <file> as if it was passed through -e\n");
}

char * parseArgs(int argc, char ** argv) {
    char * fname = NULL;
    if (argc < 2) {
        usage(argv[0]);
        return NULL;
    }
    
    int opt;
    opterr = 0;
    
    while ( (opt = getopt(argc, argv, "se:f:")) != -1) {
        switch (opt) {
            case 's':
                step = true;
                debug = true;
                break;
            case 'e':
                eachTimeCommand = std::string(optarg);
                debug = true;
                cout << eachTimeCommand << "\n";
                break;
                
            case 'f': {
                debug = true;
                char * dfname = optarg;
                ifstream inFile;
                inFile.open(dfname, ios::in);
    
                if (!inFile.is_open()) {
                    printf("Error opening debug script file '%s'\n", dfname);
                    return NULL;
                }
                
                for (std::string line; getline(inFile, line);) {
                    eachTimeCommand += line + ";";
                }

                break;
            }
                
            case ':':
            case '?':
                cerr << "Error: unknown option '" << char(opt) << "'\n";
                usage(argv[0]);
                return NULL;
        }
    }
    
    if (optind < argc) {
        do {
            if (fname != 0) {
                cerr << "Error: expected exactly one filename\n";
                usage(argv[0]);
                return NULL;
            }
            fname = argv[optind];
        } while (++optind < argc);
    } else {
        printf("Error: expected file\n");
        usage(argv[0]);
        return NULL;
    }
    return fname;
}

int readFile(char * fname) {
    ifstream inFile;
    ifstream::pos_type size;
    inFile.open(fname, ios::in | ios::binary | ios::ate);
    
    if (!inFile.is_open()) {
        printf("Error opening file '%s'\n", fname);
        return 1;
    }
    
    size = inFile.tellg();

    if (size > 65536) {
        printf("Memory file too large (>65536)\n");
        return 1;
    }
    char * buffer = new char[size];
    
    inFile.seekg(0, ios::beg);
    inFile.read(buffer, size);
    
    inFile.close();
    
    for (int i = 0; i < size; i++) {
        mem[i] = buffer[i];
    }
    return 0;
}

void stepPrompt() {
    std::string command;
    while (command != "a" && command != "advance") {
        printf(">> ");
        std::getline(std::cin, command);
        executeDebugCommand(command);
    }
}

void executeDebugCommand(string total) {
    std::vector<std::string> lines = split(total, ';');
    for (unsigned int line = 0; line < lines.size(); line++) {
        std::string text = lines.at(line);

        text = trim(text);
        if (text.size() == 0) continue;
        if (text == "a" || text == "advance" || text == "break") continue;
        
        std::vector<std::string> args = split(text, ' ');
        std::string command = args.at(0);
        args.erase(args.begin());
        int argc = args.size();
        if (command == "regs") {
            print_regs();
        } else if (command == "mem") {
            if (argc < 2) {
                cerr << "'mem' debug command takes two arguments: start address and end address (hex)\n";
            }
            unsigned int startAddr = 0;
            unsigned int endAddr = 0;
            std::stringstream ss;
            ss << std::hex << args[0];
            ss >> startAddr;
            
            std::stringstream ss2;
            ss2 << std::hex << args[1];
            ss2 >> endAddr;

            for (unsigned int i = startAddr; i < endAddr; i += 8)  {
                for (int j = 0; j < 8 && i + j < endAddr; j++) {
                    printf("0x%02x ", mem[i + j]);
                }
                printf("\n");
            }
        } else if (command == "instr") {
            printf("Next instr: 0x%02x\n", mem[pc]);
        } else if (command == "lastinstr") {
            if (pc > 0) {
                printf("Last instr: 0x%02x\n", lastInstr);
            } else {
                printf("PC is 0: no last instruction\n");
            }
        } else if (command == "print") {
            for (int i = 0; i < argc; i++) {
                cout << args[i];
            }
            cout << endl;
        } else if (command == "help") {
            if (argc == 0) {
                printf("Commands:\n");
                printf("regs:      Print registers\n");
                printf("mem:       Print a specified range of bytes from memory\n");
                printf("instr:     Print the next instruction to be executed\n");
                printf("lastinstr: Print the last instruction that was executed\n");
                printf("advance:   Advance the program counter and execute next instruction\n");
                cout << "a:         Alias of advance\n";
                cout << "print x:   Print a specified string or expression (see 'help expressions')\n";
            } else {
                if (args[0] == "expressions") {
                    cout << "WIP\n";
                }
            }
        } else {
            cout << "Unknown command '" << command << "'\nTry 'help' for help.\n";
        }
    }
}