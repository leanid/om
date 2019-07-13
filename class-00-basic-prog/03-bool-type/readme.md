# prov to show sizeof struct for C++ and C compiler

    #ifdef __cplusplus
    #include <iostream>
    #else
    #include <stdio.h>
    #endif

    struct S
    {};

    int main()
    {
        #ifdef __cplusplus
            std::cout << "sizeof(S) == " << sizeof(S) << std::endl;
        #else
            printf("sizeof(S) == %ld", sizeof(struct S));
        #endif

        return 0;
    }
