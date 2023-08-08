#include <stdio.h>

int main() {
    int num = 10;

    if (num > 5) {
        printf("Number is greater than 5\n");
	return 1;
    } else {
        printf("Number is less than or equal to 5\n");
    }

    if (num == 10) {
        printf("Number is equal to 10\n");
    }

    return 0;
}
