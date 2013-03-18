#ifndef _STRING_HELPER_H
#define _STRING_HELPER_H
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

namespace Rabbit{
    template<typename T> 
    void tokenize(const char* source,  vector<T>& tokens)
    {
        istringstream sin(source);
        T temp;
        while(sin>>temp)
            tokens.push_back(temp);        
    }
}
#endif
