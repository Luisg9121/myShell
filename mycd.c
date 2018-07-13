#include <stdio.h>
#include <unistd.h>

#define name 100

int main(int argc, char *argv[])
{
    char buf[name];
    char *path = argv[1];
	//checks path
    if (chdir(path) == -1) {  
        fprintf(stderr, "Error:\nUsage: ./mycd [Directory] or [.] or [..] \n");
        return 1;
    }
	//prints new directory
    getcwd(buf, name);
    printf("Current Directory: %s\n", buf); 
    chdir(argv[1]);
    return 0;
}

