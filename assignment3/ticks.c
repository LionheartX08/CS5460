#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i = uptime();
  int waittime = atoi(argv[1]);
  int ticks;
  while((uptime()-i <= waittime)) {
    //do nothing
  }
  ticks = getticks();
  printf(1, "%d\n", ticks);
  exit();
}
