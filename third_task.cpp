#include <iostream>
#include <sys/mman.h>
#include <cstring>

using namespace std;
/*
int gcd(int a) {
    const int value = 10;
    int b = value;
    while (b) {
        a %= b;
        int x = a;
        a = b;
        b = x;
    }
    return a;
}*/


unsigned char gcd_code[] = {0x89, 0xf8, 0xb9,
                            0x0a, 0x00, 0x00, 0x00, // value
                            0xeb, 0x09, 0x0f, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x89, 0xd1, 0x99, 0xf7,
                            0xf9, 0x89, 0xc8, 0x85, 0xd2, 0x75, 0xf5, 0xf3, 0xc3}; // gcd with -O3

const int gcd_len = sizeof(gcd_code);
typedef int(*func)(int);
void *gcd_ptr;

void change_value(unsigned int x) {
    if (mprotect(gcd_ptr, gcd_len, PROT_READ | PROT_WRITE) == -1) {
        cerr << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 4; i++) {
        gcd_code[3 + i] = static_cast<unsigned char>(x % 256);
        x /= 256;
    }
    memcpy(static_cast<unsigned char*>(gcd_ptr) + 3, gcd_code + 3, 4);
}

void invoke (int x) {
    if (mprotect(gcd_ptr, gcd_len, PROT_READ | PROT_EXEC) == -1) {
        cerr  << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    cout << func(gcd_ptr)(x) << endl;
}

void help() {
    cout << "Function find gcd {const value} and <input>" << endl;
    cout << "By default {const value} = 10" << endl;
    cout << R"(To change {const value} enter "change {value}")" << endl;
    cout << R"(To invoke function enter "invoke {value}")" << endl;
    cout << R"(To exit enter "exit" or ":q")" << endl;
}


int main() {
    gcd_ptr = mmap(nullptr, gcd_len, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (gcd_ptr == MAP_FAILED) {
        cerr << strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    memcpy(gcd_ptr, gcd_code, gcd_len);
    help();

    string command;
    while (cin >> command) {
        if (command == "change") {
            unsigned int value;
            cin >> value;
            change_value(value);
        } else if (command == "invoke"){
            int value;
            cin >> value;
            invoke(value);
        } else if (command == ":q" || command == "exit") {
            break;
        } else {
            cout << "print <change value> or <invoke value> or <:q>" << endl;
        }
    }
    return 0;
}
