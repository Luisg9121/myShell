#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void copy(char* srcFile, char* destFile)
{
	FILE *src;
	FILE * dest;
	src = fopen(srcFile, "r");
	dest = fopen(destFile, "w");

	if(src == NULL)
	{
		printf("%s does not exist\n", srcFile);
		exit(-1);
	}

	char tmp[10];
	size_t count;
	while(1)
	{
		count = fread(tmp, sizeof(char), 10, src);
		fwrite(tmp, sizeof(char), count, dest);
		if(feof(src))
			break;
	}

	fclose(src);
	fclose(dest);	
}

void copyDir(char* srcDir, char* destDir)
{
	DIR *src;
	DIR *dest;
	struct dirent *ep;
	struct stat st;
	char strSRC[500] = "";
	char strDEST[500] = "";
	printf("Source: %s\n", srcDir);
	src = opendir(srcDir);
	dest = opendir(destDir);
	if(dest == NULL)
	{
		mkdir(destDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		printf("Making Dir %s\n", destDir);
	}

	if(src == NULL)
	{
		printf("%s does not exist\n", srcDir);
		exit(-1);
	}
	
	while(ep = readdir(src))
	{
		
		printf("%s\n", ep->d_name);
		if((strcmp(ep->d_name, ".") != 0) && (strcmp(ep->d_name, "..") != 0))
		{
			stat(ep->d_name, &st);
			if(S_ISDIR(st.st_mode))
			{
				strcpy(strSRC, srcDir);
				strcat(strSRC, "/");
				strcat(strSRC, ep->d_name);
				strcpy(strDEST, destDir);
				strcat(strDEST, "/");
				strcat(strDEST, ep->d_name);
				copyDir(strSRC, strDEST);
				printf("Copying %s to %s", strSRC, strDEST);
			}
			else 
			{
				strcpy(strSRC, srcDir);
                strcat(strSRC, "/");
                strcat(strSRC, ep->d_name);
                strcpy(strDEST, destDir);
                strcat(strDEST, "/");
                strcat(strDEST, ep->d_name);
                copy(strSRC, strDEST);
                printf("Copying %s to %s", strSRC, strDEST);
			}
		}
	}

	closedir(src);
	closedir(dest);

}

int main(int argc, char *argv[])
{
	if(argv[1] == NULL)
	{
		printf("Error\n");
		printf("Usage: %s [source] [destination]\n", argv[0]);
		exit(-1);
	}	
	else if((argc < 3)||(argc >3))
	{
		printf("Error\n");
		printf("Usage: %s [source] [destination]\n", argv[0]);
		exit(-1);
	}
	
	else if(argc == 4)
	{
		int result = strncmp(argv[1], "-R", 2);
		if(result == 0)
		{
			copyDir(argv[2], argv[3]);
			exit(0);
		}
		else 
			printf("Error\n");
			printf("Usage: %s [source] [destination]\n", argv[0]);
			exit(-1);
	}
		
	else
	copy(argv[1], argv[2]);

	return 0;
}
