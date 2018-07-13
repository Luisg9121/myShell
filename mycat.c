#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
   char *cmd = "cat", 
        *file;
   struct stat buff;
   int fopen;

   //check command './mycat <file' command cause argc == 1
   if(argc == 1){
      file = argv[0];
      fstat(0, &buff);  
      if(S_ISDIR(buff.st_mode) || (fopen = open(file, O_RDONLY) == -1)){
         perror("Error: input is directory or file does not exist.\nUsage: ./mycat <file1> (optional)<file2>");
         return -1;
         }
      execvp(cmd, argv);
      }
   else{//handle the rest of the commands
      int counter = 0;
      while(counter < argc){  
           file = argv[counter];
           stat(file, &buff);
           if(S_ISDIR(buff.st_mode) || (fopen = open(file, O_RDONLY) == -1)){
              perror("Error: input is directory or file does not exist.\nUsage: ./mycat <file1> (optional)<file2>");
              return -1;
              } 
           counter++;
           }
        execvp(cmd, argv);
       }
   return 0;
}
