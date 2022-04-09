#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>

#define _countof(array) (sizeof(array) / sizeof(array[0]))

#define RHFileMode S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define RHFileFlags O_CREAT | O_WRONLY
int RedirectHandler(const char *FileName, int Handle);

//#define DT_REG 8
int GetCommands(struct dirent **Commands); 

int main(int argc, char* argv[])
{
  argc = 3;
  argv[1] = "ls";
  argv[2] = "-l";
  if (argc < 2){
    fprintf(stderr, "Incorrect paramtrs: ./{FileName} {Command}\n{FileName}: Name of exec. file\n{Command}: Command to exec.\n");
    return 1;
  }
  struct dirent *Commands;
  GetCommands(&Commands);
  char *arguments[] = {argv[1], argv[2], NULL};
  int iResult = execv( "/bin/ls",arguments); 
}


// Ad if O_APPEND | O_TRUNC
int RedirectHandler(const char *FileName, int Handle)
{
  int Fd = open(FileName, RHFileFlags, RHFileMode);
  if (Fd == -1){
    fprintf(stderr, "Could not open file \"%s\"\n", FileName);
    return 1;
  }
  if (dup2(Fd, Handle) < 0) {
        close(Fd);
        fprintf(stderr, "Could not override handle \"%d\"\n", Handle);
        return 2;
    }
  close(Fd);
  return 0;
}

int ScanDirFilter(const struct dirent *FilePtr);

int GetCommands(struct dirent **Commands)
{
  int iResult = scandir("/bin", &Commands, ScanDirFilter, NULL);
  if (iResult == -1){
    perror("Could not get commands list");
    return 1;
  }
  return 0;
}

int ScanDirFilter(const struct dirent *FilePtr)
{
  if (FilePtr->d_type == DT_REG){ // DT_REG == 8
    return 1;
  }
  return 0;
}