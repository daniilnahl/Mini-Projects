#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>

#define MY_SYSCALL_NUM 470U

int main(void) {
	long res = syscall(MY_SYSCALL_NUM);
	if (res == -1) perror("syscall failed\n");
	else printf("syscall success\n");
	return 0;
}
