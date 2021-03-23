#include <dynarr.h>
#include <stdio.h>

void foo(float **a) {
	da_push(a, 1);
	da_push(a, 1);
	da_push(a, 1);
	da_push(a, 1);
}

int main() {
	float *arr = NULL;
	foo(&arr);
	da_push(&arr, 1);
	da_push(&arr, 2);
	da_push(&arr, 3);
	for (size_t i = 0; i < da_size(arr); ++i)
		printf("%f\n", arr[i]);
	da_destroy(&arr);

	//float a[4], *ap = a;
	//da_push(&ap, 4);
	return 0;
}
