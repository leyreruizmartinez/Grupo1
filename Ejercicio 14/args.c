#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    printf("Number of arguments: %d\n", argc - 1);
    
    for (int i = 1; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
    
    if (argc > 1 && strcmp(argv[1], "test") == 0) {
        printf("The first argument is 'test'.\n");
    } else {
        printf("The first argument is not 'test'.\n");
    }
    
    return 0;
}