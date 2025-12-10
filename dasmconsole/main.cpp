#include <iostream>
#include <string>

int main() {
    // Ensure the process has its own console
    // (If started with CREATE_NEW_CONSOLE, this is automatic)
    std::string line;
    while (std::getline(std::cin, line)) {
        std::cout << line << std::endl;
    }
    return 0;
}