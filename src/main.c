#include <stdio.h>
#include <stdlib.h>

int main(void)
{
#if defined(DEBUG)
    puts("debug mode enabled");
#endif
    printf("%s\n", "Hello !");
    return EXIT_SUCCESS;
}
