#include <stdio.h>
#include <sys/personality.h>

int main(void) {
  int previous = personality(0xffffffffUL);
  if (previous == -1 ||
      personality((unsigned long)previous | ADDR_NO_RANDOMIZE) == -1) {
    perror("native sanitizer container cannot adjust its process ASLR layout");
    return 1;
  }
  return 0;
}
