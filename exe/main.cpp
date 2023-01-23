#include "ustring.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    UString ustr;

    std::string cmd;
    while (true) {
        std::cin >> cmd;
        if (cmd == "pushc") {
            UString::uchar symb;
            std::cin >> symb;
            ustr.push_back(symb);
        } else if (cmd == "pushp") {
            unsigned int code = 0;
            std::cin >> code;
            ustr.push_back(code);
        } else if (cmd == "pushb") {
            int bytesNum = 0;
            std::cin >> bytesNum;
            if (bytesNum <= 0 || bytesNum > 4) {
                continue;
            }

            std::vector<char> bytes(bytesNum);
            for (int i = 0; i < bytesNum; ++i) {
                unsigned short byte = 0;
                std::cin >> byte;
                bytes[i] = byte;
            }
            bytes.push_back(0);

            ustr += bytes.data();
        } else if (cmd == "pop") {
            ustr.pop_back();
        } else if (cmd == "show") {
            std::cout << ustr << "\n";
        } else if (cmd == "exit") {
            break;
        }
    }

    return EXIT_SUCCESS;
}
