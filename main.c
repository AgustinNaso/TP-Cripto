#include <stdio.h>
#include <string.h>

int extract() {
    //do something
    return 0;
}

int embed() {
    //do something
    return 0;
}

int main(int argc, char* argv[]){
    if(argc != 2)
        printf("Wrong parameters!\n");
        return 1;
    if(strcmp(argv[1], "-embed") == 0)
       embed();
    if(strcmp(argv[1], "-extract") == 0)
       extract();
    return 0;
}