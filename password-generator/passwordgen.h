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
        string generate(const passwordOptions& options);
        constexpr auto makePool(const passwordOptions& options);
};
