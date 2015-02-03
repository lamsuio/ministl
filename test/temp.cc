#include <cstdio>

template<class T>
void foo(T params);

const char hello[]= "Hello world!";

int main(int argc, char const** argv)
{
    foo(hello);
    return 0;
}

template<class T>
void foo(T params) {
    printf("%s\n", params);
}
