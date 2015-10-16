#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"


void
testwaitstat(void)
{
  int i = 0;
  int *turnaround;
  int *running;
  i = waitstat(turnaround, running);
  printf(1,"%d\n", i);
  exit();
}
int
main(int argc, char *argv[])
{
  //procdump();
  testwaitstat();
  exit();
  return 0;
}
