#include <string>
#include <iostream>


typedef struct {
    size_t       len;
    const char  *content;
} raw_string;

int main () {
    raw_string str;
    str.content = "wo cao nima";
    str.len = 2;

    std::cout <<  << "\n";
}
