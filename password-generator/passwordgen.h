#pragma once
#include <string>

using namespace std;

struct passwordOptions {
    int length = 8;
    bool useUppercase = true;
    bool useLowercase = true;
    bool useDigits = true;
    bool useSymbols = true;
};

class passwordGenerator {
    public:
        string generate();
        constexpr auto makePool(const passwordOptions& options);
};
