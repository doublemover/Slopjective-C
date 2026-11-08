#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
__attribute__((noinline)) int profile_branch(int n) {
  if(n==0)return 10;
  if(n==1)return 11;
  if(n==2)return 12;
  if(n==3)return 13;
  return 14;
}
int main(int argc,char **argv) {
  if(argc>1){profile_branch(3);return 0;}
  for(int i=0;i<4;++i) {
    pid_t child=fork();
    if(child<0)return 1;
    if(child==0) {
      if(i==3){execl(argv[0],argv[0],"exec",(char *)0);return 2;}
      profile_branch(i);exit(0);
    }
    int status=0;
    if(waitpid(child,&status,0)!=child || !WIFEXITED(status) || WEXITSTATUS(status))return 3;
  }
  profile_branch(4);return 0;
}
