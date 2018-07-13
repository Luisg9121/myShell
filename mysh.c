#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

void replace_str(char *res, char *str, char *orig, char *rep);
int parse(char *input, char **args, char *delim);
void docmd(char **cmds, int cmdc);
void execute(char **args, int argc);
void pipecmd(char **cmds, int pos);


#define INPUT_BUFSIZ 3000 //max num of characters of input, e.g: "ls -l" is 5.
#define NUM_CMD 200 // max num of commands piped, e.g: "ls -l | head -10" is 2.
#define CMD_NARGS 30 // max num of arguments for each command, e.g: "ls -l" is 2.
#define CMD_CHAR 1024 //max num of characters for each command.




int main(void){
	char buf[INPUT_BUFSIZ];
	char str[INPUT_BUFSIZ];
	char *cmds[NUM_CMD];
	int cmdc;
	int pid, status;
	int background;

	while(1){
	background= 0;
	char *currentDir; //get current directory
	char *cwd = getcwd(NULL, PATH_MAX);
	char *dirToken = strtok(cwd, "/");
	while(dirToken = strtok(NULL, "/")) {
		currentDir = dirToken;
	}
	printf("[mysh: ~%s]$ ", currentDir);

	if(fgets(buf, sizeof(buf), stdin)== NULL){
		if(ferror(stdin)){
			printf("ERROR: ");
			perror("standard input");
			printf("\nERROR: cannot reading stdin\n");
		}
		break;
	}

	if(strcmp(buf, "exit\n")== 0) exit(0);
	if(strcmp(buf, "\n")== 0) continue;	

	replace_str(str, buf, ">", " > ");
	strcpy(buf, str);
	replace_str(str, buf, "<", " < ");
	cmdc= parse(str, cmds, "|");//split user's input by "|" and return the number of cmds.

	char *pch;
	char *fn_cmd= cmds[cmdc-1];
	/*if((pch=strchr(fn_cmd, '&'))!= NULL){
		background= 1;
		int pos= pch-fn_cmd;
		fn_cmd[pos]= '\0';
	}
      
	fflush(stdout);
	*/


	switch((pid=fork())){
	case -1:
		printf("ERROR: ");
		perror("fork");
		break;
	case 0:
		docmd(cmds, cmdc);
		break;
	default:
		if(background== 0){
			pid= wait(&status);
		}
	
	}



	}
	
	return 0;
}


void replace_str(char *res, char *str, char *orig, char *rep)
{
	char *p;
	if(p = strstr(str, orig)) { // Is 'orig' even in 'str'?
		strncpy(res, str, p-str); // Copy characters from 'str' start to 'orig' st$
		res[p-str] = '\0';
		sprintf(res +(p-str), "%s%s", rep, p+strlen(orig));
	}
	else{ 
		strcpy(res, str);
        }
}



int parse(char *input, char **args, char *delim){
	args[0]= strtok(input, delim);
	int i= 0;
	while(args[i]!= NULL){
		i++;
		args[i]= strtok(NULL, delim);
	}
	
	return i;	
}


void docmd(char **cmds, int cmdc){
	if(cmdc== 1){
		char *args[CMD_NARGS];
		int argc= parse(cmds[0], args, " \t\n");//split each cmd by " \t\n", return num  args.
		execute(args, argc);
		exit(126);
		
	}
	else if(cmdc> 1){
		pipecmd(cmds, cmdc-1);
	}
}


void execute(char **args, int argc){
	if(argc> 2){
	        int LR_pos= -1; // "<" position (left redirect)
        	int RR_pos= -1; // ">" position (right redirect)
		int i;
		for(i=1;i<argc-1;i++){
		if(strcmp(args[i], "<")== 0){
			LR_pos= i;
			continue;
		}
		if(strcmp(args[i], ">")== 0){
			RR_pos= i;
			continue;
		}		
		}
		
		if(LR_pos!= -1 && RR_pos== -1){
			int new= open(args[LR_pos+1], O_RDONLY);
			int bak= dup(0);
		        dup2(new, 0);
                        close(new);
                        args[LR_pos]= NULL;
                        execvp(args[0], args);
                        dup2(bak, 0);
                        close(bak);
                   
		}
		else if(LR_pos== -1 && RR_pos!=-1){
			int bak,new;
                        new= open(args[RR_pos+1], O_WRONLY | O_CREAT, S_IRWXU);
                        bak= dup(1);
                        dup2(new,1);
                        close(new);
                        args[RR_pos]= NULL;
                        execvp(args[0], args);
                        dup2(bak,1);
                        close(bak);
                  
		}
		else if(LR_pos!= -1 && RR_pos!= -1){
			int newi= open(args[LR_pos+1], O_RDONLY);
       	                int baki= dup(0);
        		dup2(newi, 0);
        		close(newi);
        		int newo= open(args[RR_pos+1], O_WRONLY | O_CREAT, S_IRWXU);
        		int bako= dup(1);
        		dup2(newo, 1);
        		close(newo);
        		args[LR_pos]= NULL;
        		execvp(args[0], args);
        		dup2(baki, 0);
        		dup2(bako, 1);
        		close(baki);
        		close(bako);

		}
		else{
			execvp(args[0], args);
		}
	}
	else{
        	execvp(args[0], args);
	}
	printf("ERROR: ");
	perror(args[0]);
	printf("ERROR: exec failed.\n");
}


void pipecmd(char **cmds, int pos){
	int pipefd[2];
	if(pipe(pipefd)){
		printf("ERROR: ");
		perror("pipe");
		exit(127);
	}
	if(pos== 1){
		char *res1[CMD_NARGS];
		char *res2[CMD_NARGS];
		int argc1= parse(cmds[0], res1, " \t\n");
		int argc2= parse(cmds[1], res2, " \t\n");
		switch(fork()){
		case -1:
			printf("ERROR: ");
			perror("fork");
			exit(127);
		case 0:
			close(pipefd[0]);
			dup2(pipefd[1],1);
			close(pipefd[1]);
			execute(res1, argc1);
			exit(126);
		default:
			close(pipefd[1]);
			dup2(pipefd[0], 0);
			close(pipefd[0]);
			execute(res2, argc2);
			exit(125);
		}
	}
	if(pos> 1){
		char *res[CMD_NARGS];
		int argc= parse(cmds[pos], res, " \t\n");
		switch(fork()){
		case -1:
			printf("ERROR: ");
			perror("fork");
			exit(127);
		case 0:
			close(pipefd[0]);
			dup2(pipefd[1],1);
			close(pipefd[1]);
			pipecmd(cmds, pos-1);
			exit(126);
		default:
			close(pipefd[1]);
			dup2(pipefd[0],0);
			close(pipefd[0]);
			execute(res, argc);
			exit(125);
		}
	}
}

