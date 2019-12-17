#include <iostream>

#include <pandora/incl.hpp>
#include <services/multiplication.hpp>

int main() {
    print();

    std::cerr << multiplication({10, 10}).x << '\n';
}