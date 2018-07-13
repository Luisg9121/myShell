#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>

int isdirectory (char *filename)
{
  int errno;
  struct  stat mystat;

  errno = stat(filename, &mystat);
  if (-1 == errno) {
    if (ENOENT == errno) {
      /* does not exist */
      return 0;
    }
    else {
      perror("stat");
      exit(1);
    }
  }
  else { // no error
    if (S_ISDIR(mystat.st_mode)) {
      /* it's a dir */
      return 1;
    }
    else {
      /* exists but is not dir */
      //printf("ls: %s: No such file or directory", filename );
      return 0;
    }
  }
}

void printDeets (struct stat *buf, char *name)
{
    unsigned char type;
    if (S_ISDIR(buf->st_mode)) type = 'd';
    else if (S_ISREG(buf->st_mode)) type = '.';
    else if (S_ISLNK(buf->st_mode)) type = 'l';
    else type = '?';

    char rights[10];
    rights[0] = (buf->st_mode & S_IRUSR) ? 'r' : '-';
    rights[1] = (buf->st_mode & S_IWUSR) ? 'w' : '-';
    rights[2] = (buf->st_mode & S_IXUSR) ? 'x' : '-';
    rights[3] = (buf->st_mode & S_IRGRP) ? 'r' : '-';
    rights[4] = (buf->st_mode & S_IWGRP) ? 'w' : '-';
    rights[5] = (buf->st_mode & S_IXGRP) ? 'x' : '-';
    rights[6] = (buf->st_mode & S_IROTH) ? 'r' : '-';
    rights[7] = (buf->st_mode & S_IWOTH) ? 'w' : '-';
    rights[8] = (buf->st_mode & S_IXOTH) ? 'x' : '-';
    rights[9] = '\0';

    unsigned int link = buf->st_nlink;

    char username[50];
    struct passwd *user = getpwuid(buf->st_uid);
    strncpy(username, user->pw_name, 49);
	
    char groupname[50];
    struct group *grp = getgrgid(buf->st_gid);
    strncpy(groupname, grp->gr_name, 49);
	
    unsigned int size = buf->st_size;

    char time[50];
    strncpy(time, ctime(&buf->st_atime), 50);
    time[strlen(time)-1] = 0;
    
    printf("%c%s %u %s %s %u %s %s\n", type, rights, link, username, groupname, size, time, name);
}

int main(int argc, char **argv)
{

  char *dirname = ".";
  DIR *mydir;

  int c, lflag = 0; 
  while ((c = getopt(argc, argv, "l")) != -1) {
    switch (c) {
      case 'l':
        lflag = 1;
	break;
      case '?':
	fprintf(stderr, "incorrect usage"); 
        exit(-1);
    }
  }
	
  if (lflag && argc > 2) {	
    dirname = argv[2];
    struct stat buf;
    if (stat(dirname, &buf) < 0) 
      perror("myls cannot open file");
    printDeets(&buf,dirname);
  }
  else if (lflag) { 
    struct dirent *bufs[50];
    mydir = opendir(".");
    if(mydir == NULL) {
      perror(".");
    }
    int count = 0;
    while((bufs[count] = readdir(mydir))) {
      char *temp = bufs[count]->d_name;
      if(!strcmp(temp, ".") || !strcmp(temp, "..") || temp[0] == '.')
        continue;
        count++;
      }	
      int j, i, x, y;
      char temp1[50], temp2[50];
      struct dirent *temp;
      for (i = 0; i < count; i++) {
        for (j = 0; j < count - 1; j++) {	    
	  for(x = 0; x < strlen(bufs[j]->d_name); x++)
	    temp1[x] = tolower(bufs[j]->d_name[x]);
	  for(y = 0; y < strlen(bufs[j+1]->d_name); y++)
	    temp2[y] = tolower(bufs[j+1]->d_name[y]);
	  if (strcmp(temp1, temp2) > 0) {
	    temp = bufs[j];
	    bufs[j] = bufs[j+1];
	    bufs[j+1] = temp;
          }
        }
      }
      	
      for(i = 0; i < count; i++) {
        dirname = bufs[i]->d_name;
	struct stat buf;
	if (stat(dirname, &buf) < 0)
	  perror("myls cannot open file");
        printDeets(&buf,dirname);
      }
			
  }
  else {
    if(argc > 1)
      dirname = argv[1];

    if(!isdirectory(dirname)) 
      printf("%s\n", dirname);

    else {
      int i, j;
      struct dirent **list;
      i = scandir(dirname, &list, NULL, alphasort);
      for (j = 0; j < i; j++) {
        if (strcmp(list[j]->d_name, ".") != 0 && strcmp(list[j]->d_name, "..") != 0)
          printf("%s\n", list[j]->d_name);
      }
    }
  }

  return 0;
}
