#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/stringlib.h"

size_t cstrlen(const char *n, char d) {
    size_t idx = 0;
    while (n[idx++] != d && n[idx] != 0);
    return idx;
}

size_t arrlen(char **arr) {
    size_t idx = 0;
    while (arr[idx++] != 0);
    return idx - 1;
}

char **split(const char *str, char delim) {
    size_t slen = strlen(str);
    size_t delimCount = 0;
    for (size_t i = 0; i < slen; i++) {
        if (str[i] == delim) delimCount++;
    }
    if (delimCount == 0) {
        char **out = malloc(sizeof(char*) * 2);
        out[0] = malloc(slen + 1);
        strcpy(out[0], str);
        out[0][slen] = 0;
        out[1] = 0;
        return out;
    }
    char **out = malloc(sizeof(char*) * (delimCount + 2));
    size_t idx = 0;
    size_t idx2 = 0;
    out[0] = malloc(cstrlen(str, ';') + 1);
    for (size_t i = 0; i < slen; i++) {
        if (str[i] == delim) {
            out[idx][idx2] = 0;
            out[++idx] = malloc(cstrlen(str + i + 1, ';') + 1);
            idx2 = 0;
        } else {
            out[idx][idx2++] = str[i];
        }
    }
    out[delimCount + 1] = 0;
    return out;
}

void strrep(char *str, char src, char dest) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == src) str[i] = dest;
    }
}

char *trim(char *s) { return s; }

/*
std::vector<std::string> split(const std::string& s, char delimiter) {
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   
   while (std::getline(tokenStream, token, delimiter)) {
      tokens.push_back(token);
   }
   return tokens;
}

std::string trim(const std::string s) {
    std::string x = s;
    for(int i=0; i<(int)s.length(); i++) {
        const int l = (int)x.length()-1;
        if(x[l]==' '||x[l]=='\t'||x[l]=='\n'||x[l]=='\v'||x[l]=='\f'||x[l]=='\r'||x[l]=='\0') x.erase(l, 1);
        if(x[0]==' '||x[0]=='\t'||x[0]=='\n'||x[0]=='\v'||x[0]=='\f'||x[0]=='\r'||x[0]=='\0') x.erase(0, 1);
    }
    return x;
}//246580*/