#include <fstream>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <limits>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "regs.h"
#include "main.h"
#include "stringlib.h"

void executeDebugCommand(std::string arg);

void usage(std::string name) {
    std::cout << "Usage: " << name << " <hexfile> [-d] [-q] [-s]\n";
    std::cout << "Options:\n";
    std::cout << "    -s   --step       Enable single-step debug mode\n";
    std::cout << "    -e X --execute X  Run this debug command (\"X\") every instruction executed\n";
    std::cout << "    -f <file>         Run the contents of <file> as if it was passed through -e\n";
}

std::string parseArgs(int argc, char ** argv) {
    std::string fname = "";
    if (argc < 2) {
        usage(std::string(argv[0]));
        return "";
    }
    
    int opt;
    opterr = 0;
    
    while ((opt = getopt(argc, argv, "se:f:")) != -1) {
        switch (opt) {
            case 's':
                step = true;
                debug = true;
                break;
            case 'e':
                eachTimeCommand = std::string(optarg);
                debug = true;
                std::cout << eachTimeCommand << "\n";
                break;
                
            case 'f': {
                debug = true;
                std::string dfname = std::string(optarg);
                std::ifstream inFile;
                inFile.open(dfname, std::ios::in);
    
                if (!inFile.is_open()) {
                    std::cout << "Error opening debug script file '" << dfname << "'\n";
                    return "";
                }
                
                for (std::string line; getline(inFile, line);) {
                    eachTimeCommand += line + ";";
                }

                break;
            }
                
            case ':':
            case '?':
                std::cerr << "Error: unknown option '" << (char)opt << "'\n";
                usage(std::string(argv[0]));
                return "";
        }
    }
    
    if (optind < argc) {
        do {
            if (fname != "") {
                std::cerr << "Error: expected exactly one filename\n";
                usage(std::string(argv[0]));
                return "";
            }
            fname = std::string(argv[optind]);
        } while (++optind < argc);
    } else {
        std::cerr << "Error: expected file\n";
        usage(std::string(argv[0]));
        return "";
    }
    return fname;
}

int readFile(std::string fname) {
    std::ifstream inFile;
    std::ifstream::pos_type size;
    inFile.open(fname, std::ios::in | std::ios::binary | std::ios::ate);
    
    if (!inFile.is_open()) {
        std::cerr << "Error opening file '"<< fname << "'\n";
        return 1;
    }
    
    size = inFile.tellg();

    if (size > 65536) {
        std::cerr << "Memory file too large (>65536)\n";
        return 1;
    }
        
    inFile.seekg(0, std::ios::beg);
    inFile.read((char*)mem, size);
    
    inFile.close();
    
    return 0;
}

void stepPrompt() {
    std::string command;
    while (command != "a" && command != "advance") {
        std::cout << ">> ";
        std::getline(std::cin, command);
        executeDebugCommand(command);
    }
}

std::string hex(uint32_t num) {
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << num;
    return ss.str();
}

void executeDebugCommand(std::string total) {
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
                std::cerr << "'mem' debug command takes two arguments: start address and end address (hex)\n";
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
                    std::cout << hex(mem[i + j]) << " ";
                }
                std::cout << "\n";
            }
        } else if (command == "instr") {
            std::cout << "Next instr: 0x" << std::hex << (int)mem[pc] << "\n";
        } else if (command == "lastinstr") {
            if (pc > 0) {
                std::cout << "Last instr: 0x" << std::hex << (int)lastInstr << "\n";
            } else {
                std::cout << "PC is 0: no last instruction\n";
            }
        } else if (command == "print") {
            for (int i = 0; i < argc; i++) {
                std::cout << args[i];
            }
            std::cout << "\n";
        } else if (command == "help") {
            if (argc == 0) {
                std::cout << "Commands:\n";
                std::cout << "regs:      Print registers\n";
                std::cout << "mem:       Print a specified range of bytes from memory\n";
                std::cout << "instr:     Print the next instruction to be executed\n";
                std::cout << "lastinstr: Print the last instruction that was executed\n";
                std::cout << "advance:   Advance the program counter and execute next instruction\n";
                std::cout << "a:         Alias of advance\n";
                std::cout << "print x:   Print a specified string or expression (see 'help expressions')\n";
            } else {
                if (args[0] == "expressions") {
                    std::cout << "WIP\n";
                }
            }
        } else {
           std:: cout << "Unknown command '" << command << "'\nTry 'help' for help.\n";
        }
    }
}