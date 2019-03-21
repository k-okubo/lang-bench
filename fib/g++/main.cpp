#include <iostream>

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
	std::cin >> n;
	std::cout << fib(n) << std::endl;
	return 0;
}
