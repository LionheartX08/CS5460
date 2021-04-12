#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

void printp(struct pstat p);

int
main(int argc, char *argv[])
{
  struct pstat p;
  int success = getpinfo(&p);
  if (success >= 0) {
    //printf(1, "PID TICKETS TICKS\n");
    if (argv[1] && !strcmp("-r", argv[1])) {      
        while(1) {
          getpinfo(&p);
          printp(p);
          sleep(100);
        } 
    }    
    printp(p);
    exit();
  }
  else {
    printf(1, "getpinfo syscall failed\n");
    exit();
  }  
}

void
printp(struct pstat p) 
{
  printf(1, "PID TICKETS TICKS\n");
  int i;
  for (i = 0; i < NPROC; i++) {
    if(p.inuse[i] == 1) {
      printf(1, "%d, %d, %d\n", p.pid[i], p.tickets[i], p.ticks[i]);  
    }
  }
}
