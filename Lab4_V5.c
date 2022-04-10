#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>

#define DefaultDir "/bin/"
#define _countof(array) (sizeof(array) / sizeof(array[0]))

#define RHFileMode S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define RHFileFlags O_CREAT | O_WRONLY
int RedirectHandler(const char *FileName, int Handle);
int GetCommands(struct dirent ***Commands); 
int GetNextCommand(char *argv[], int argc, int *FirsArg, int* CurGlobalArg, struct dirent **Commands, int AmOfCommands);
int ExecuteCommand(char **Directory, char *Arguments[]);
int WaitChild(pid_t ChildPid);

int main(int argc, char* argv[])
{
  argc = 3;
  argv[1] = "cat";
  argv[2] = "011.c";
  argv[3] = "ls";
  argv[4] = "-l";
  if (argc < 2){
    fprintf(stderr, "Incorrect parameters: ./{FileName} {Command}\n{FileName}: Name of exec. file\n{Command}: Command to exec.\n");
    return 1;
  }
  struct dirent **Commands;
  int AmOfCommands = GetCommands(&Commands);
  int CurGlobalArg = 1, FirstArg = 1;
  while (CurGlobalArg < argc){
    if (GetNextCommand(argv, argc, &FirstArg, &CurGlobalArg, Commands, AmOfCommands) != 0){
      fprintf(stderr, "Could not determine commnad\n");
      return 2;
    }
    char *Arguments[CurGlobalArg - FirstArg + 1];
    for (int i = 0; i < CurGlobalArg - FirstArg; i++){
      Arguments[i] = argv[i + FirstArg]; 
    }
    Arguments[CurGlobalArg - FirstArg] = NULL;
    char *Directory = calloc(_SC_TRACE_NAME_MAX, sizeof(char));
    if (Directory == NULL){
      perror("Could not allocate memory\n");
      return 3;
    }
    strcat(Directory, DefaultDir);
    strcat(Directory, Arguments[0]);
    ExecuteCommand(&Directory, Arguments);
  } 

  //free memory
  return 0;
}

//Free list:
//struct dirent **Commands
//char* Arguments


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

int GetCommands(struct dirent ***Commands)
{
  int iResult = scandir("/bin", Commands, ScanDirFilter, NULL);
  if (iResult == -1){
    perror("Could not get commands list");
  }
  return iResult;
}

int ScanDirFilter(const struct dirent *FilePtr)
{
  if (FilePtr->d_type == DT_REG){ // DT_REG == 8
    return 1;
  }
  return 0;
}

int GetNextCommand(char *argv[], int argc, int *FirstArg, int* CurGlobalArg, struct dirent **Commands, int AmOfCommands)
{
  int AmOfArg = 2, CurArg = 0;
  int iResult = 0;
  while (iResult == 0 && *CurGlobalArg < argc){
    for (int j = 0; j < AmOfCommands && iResult == 0; j++){
      if (strcmp(argv[*CurGlobalArg], Commands[j]->d_name) == 0){
        iResult = 1;
      }
    }
    if (iResult == 1){
      *FirstArg = *CurGlobalArg;
    }
    (*CurGlobalArg)++;
  }
  if (iResult == 0 && *CurGlobalArg == argc){
    fprintf(stderr, "Could not find command\n");
    return 1;  
  }

  iResult = 0;
  while (iResult == 0 && *CurGlobalArg < argc){
    for (int j = 0; j < AmOfCommands && iResult == 0; j++){
      if (strcmp(argv[*CurGlobalArg], Commands[j]->d_name) == 0){
        iResult = 1;
      }
    } 
    (*CurGlobalArg)++;
  }
  if (iResult == 1){
    (*CurGlobalArg)--;
  }
  return 0;
}

int ExecuteCommand(char **Directory, char *Arguments[])
{
  pid_t ChildPid = fork();
  switch (ChildPid){
    case -1:
			perror("Child 1 process could not be created\n");
      return 1;
      break;
    case 0:
      if (execvp(*Directory, Arguments) == -1){
        perror("Execute command failue");
        return 2;
      }
      break;
    default:
      if (WaitChild(ChildPid) == 1){
        return 1;
      }
      break;
  }
}

int WaitChild(pid_t ChildPid) 
{
	if (waitpid(ChildPid, NULL, 0) == -1)
		perror("Wait pid failue\n");
}