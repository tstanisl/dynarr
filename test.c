#include "dynarr.h"
#include <stdio.h>

#define STR_(x) #x
#define STR(x) STR_(x)
#define WHERE __FILE__ ":" STR(__LINE__) ":"

#define TEST(cond) do { \
	if (!(cond)) { \
		fprintf(stderr, WHERE " condition '%s' failed\n", #cond); \
		abort(); \
	} \
} while (0)

#define PASS() fprintf(stderr, "%s: PASS\n", __func__)

void test_init() {
	double *d = DA_INIT;
	TEST(da_size(&d) == 0);
	TEST(da_caps(&d) == 0);

	double *d2 = da_init();
	TEST(da_size(&d2) == 0);
	TEST(da_caps(&d2) == 0);

	TEST(d == d2);

	PASS();
}

void test_free() {
	double *d = DA_INIT;
	da_push(&d, 42);
	da_free(&d);
	TEST(da_size(&d) == 0);
	TEST(da_caps(&d) == 0);

	da_push(&d, 42);
	TEST(da_size(&d) == 1);

	da_free(&d);
	TEST(da_size(&d) == 0);
	TEST(da_caps(&d) == 0);

	PASS();
}

void test_push() {
	double *d = DA_INIT;
	for (int i = 0; i < 1000; ++i) {
		TEST(da_size(&d) == i);
		da_push(&d, i);
		TEST(da_size(&d) == i + 1);
		for (int j = 0; j < i; ++j)
			TEST(d[j] == j);
	}
	da_free(&d);
	PASS();
}

void test_pop(void) {
	int *a = DA_INIT;
	da_push(&a, 0);
	da_push(&a, 1);
	da_push(&a, 2);

	TEST(da_size(&a) == 3);
	TEST(da_pop(&a) == 2);
	TEST(da_size(&a) == 2);
	TEST(da_pop(&a) == 1);
	TEST(da_size(&a) == 1);
	TEST(da_pop(&a) == 0);
	TEST(da_size(&a) == 0);

	da_free(&a);
	PASS();
}

void test_resize() {
	double *d = DA_INIT;
	da_resize(&d, 10);
	TEST(da_size(&d) == 10);
	TEST(da_caps(&d) >= 10);

	da_resize(&d, 20);
	TEST(da_size(&d) == 20);
	TEST(da_caps(&d) >= 20);

	size_t caps = da_caps(&d);
	da_resize(&d, 5);
	TEST(da_size(&d) == 5);
	TEST(da_caps(&d) >= 5);
	TEST(da_caps(&d) <= caps);

	da_free(&d);
	PASS();
}

void test_reserve() {
	double *d = DA_INIT;

	da_reserve(&d, 10);
	TEST(da_size(&d) == 0);
	TEST(da_caps(&d) >= 10);

	size_t caps = da_caps(&d);
	da_reserve(&d, 5);
	TEST(da_size(&d) == 0);
	TEST(da_caps(&d) >= 5);
	TEST(da_caps(&d) <= caps);

	da_push(&d, 1);
	da_reserve(&d, 20);
	TEST(da_size(&d) == 1);
	TEST(da_caps(&d) >= 20);

	da_free(&d);
	PASS();
}

int main() {
	test_init();
	test_push();
	test_pop();
	test_free();
	test_resize();
	test_reserve();
	return 0;
}

