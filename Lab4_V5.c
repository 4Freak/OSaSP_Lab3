#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/wait.h>

int main(int argc, char* argv[])
{
  if (argc < 2){
    fprintf(stderr, "Incorrect paramtrs: ./{FileName} {Command}\n{FileName}: Name of exec. file\n{Command}: Command to exec.\n");
  }
  int  result = execl("./01 ", "1", NULL);
  perror("Error: ");
  printf("%d\n", result);
}