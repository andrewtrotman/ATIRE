#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    printf("%s\n", argv[0]);
    if (argc > 1) 
        printf("%s\n%s\n%d\n", argv[1], argv[argc - 1], argc - 1);    
    
    return EXIT_SUCCESS;
}
