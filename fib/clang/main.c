#include <stdio.h>

int fib(int n) {
	if (n <= 2) {
		return 1;
	}
	else {
		return fib(n - 1) + fib(n - 2);
	}
}

int main(int argc, char **argv) {
	int n;
	if (scanf("%d", &n) != 1) {
		return 1;
	}

	printf("%d\n", fib(n));
	return 0;
}
