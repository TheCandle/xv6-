#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;

  myproc()->tracemask = n;
  return 0;                     
}

uint64
sys_sysinfo(void)
{
  // uint64 fmem,nproc;
  // struct proc *p = myproc();
  // uint64 ans1 = freebytes();
  // uint64 ans2 = getproc();
  // if(argaddr(0, &fmem) < 0 || argaddr(1, &nproc) < 0)                             //fmem得到用户传来的结构体地址
  //   return -1;
  // if(copyout(p->pagetable,fmem , (char*)&ans1, sizeof(ans1)) < 0 ||
  //    copyout(p->pagetable,nproc, (char *)&ans2, sizeof(ans2)) < 0){
  //   return -1;
  // }                                                                                       //写入结构体
  
  // return 0;    

  uint64 addr;
  if(argaddr(0, &addr) < 0)
    return -1;
  struct proc *p = myproc();
  struct sysinfo info;
  info.freemem = freebytes();
  info.nproc = getproc();    
  if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
      return -1;
  return 0;                 
}
