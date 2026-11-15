#include "passwordgen.h"
#include <iostream>
#include <string>
#include <random>
#include <stdexcept>

using namespace std;

string passwordGenerator::generate(const passwordOptions& options) {
    string pool = makePool(options);

    // error handling pag walang pool
    if (pool.empty() || options.length <= 0) {
        return "";
    }
    
    // generate password
    string password = "";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, pool.length() - 1);

    for (int i = 0; i < options.length; ++i) {
        password += pool[dist(gen)];
    }

    return password;
}

// build the character pool so generate() has data on what to generate
string auto makePool(const passwordOptions& options) {
    string pool = "";
    if (options.useUppercase) for (char c = 'A'; c <= 'Z'; ++c) pool += c;
    if (options.useLowercase) for (char c = 'a'; c <= 'z'; ++c) pool += c;
    if (options.useDigits)    for (char c = '0'; c <= '9'; ++c) pool += c;
    // did the symbols loop like this because not all symbols are usable in passwords
    if (options.useSymbols) {
        for (char c : "!#$%&()*+,-./:;=?@[]^_{|}~") {
            pool += c;
        }
    }
    return pool;
}


