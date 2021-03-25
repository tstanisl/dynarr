#include <dynarr.h>
#include <stdio.h>

void foo(float **a) {
	da_push(a, 0);
	da_push(a, 1);
	size_t s; 
	while ((s = da_size(*a)) < 120) {
		int elem = (*a)[s - 1] + (*a)[s - 2];
		printf("%d size=%zi caps=%zi\n", elem, s, da_caps(*a));
		da_push(a, elem);
	}
	puts("done");
}

int main() {
	float *arr = NULL;
	foo(&arr);
	da_push(&arr, 1);
	da_push(&arr, 2);
	da_push(&arr, 3);
	for (size_t i = 0; i < da_size(arr); ++i)
		printf("%f\n", arr[i]);

#if 0
	float **arr2d = NULL;
	da_push(&arr2d, arr);
	da_push(&arr2d, arr);
#endif

#if 0
	for (size_t i = 0; i < da_size(arr2d); ++i)
	for (size_t j = 0; j < da_size(arr2d[i]); ++j)
		printf("%f\n", arr2d[i][j]);
	da_destroy(&arr2d);
#endif
	da_destroy(&arr);
	//float a[4], *ap = a;
	//da_push(&ap, 4);
	return 0;
}
