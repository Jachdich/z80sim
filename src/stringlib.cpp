#include <stdio.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <limits>
#include <vector>
#include <algorithm>

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
}//246580