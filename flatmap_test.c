#include <stdio.h>

#define UB_FLATMAP_IMPLEMENTATION
#include "ub_flatmap.h"

int int_eq(void const* lhs, void const* rhs) {
	return *(int*)lhs == *(int*)rhs;
}

int main() {
	ub_flatmap kv;
	int key;
	int val;
	int* gotten_val;

	ub_flatmap_init(&kv, int_eq, sizeof(int), sizeof(int));

	key = 1;
	val = 1;
	ub_flatmap_insert(&kv, &key, &val, NULL);

	key = 0;
	if ((gotten_val = ub_flatmap_get(&kv, &key))) {
		printf("got a value: %d\n", *gotten_val);
	} else {
		printf("got a null pointer\n");
	}

	return 0;
}
