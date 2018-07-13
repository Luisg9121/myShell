#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(){
  char pwd[2048];

   if(getcwd(pwd, sizeof(pwd)) == NULL){
      perror("getcwd() error\n");
      return -1;
      }
   else
      printf("%s\n", pwd);
   return 0;
}
