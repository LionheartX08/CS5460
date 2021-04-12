#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  char* arg[0];
  if(argc != 3) {
      printf(1, "This program requires 2 arguments to execute. \n");
      exit();
  }  
  settickets(atoi(argv[1]));
  exec(argv[2], arg);
  printf(1, "exec failed\n");
  exit();
}
