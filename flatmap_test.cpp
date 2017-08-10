#include <stdio.h>

#define UB_FLATMAP_IMPLEMENTATION
#define UB_FLATMAP_CXX_OVERLOADING
#include "ub_flatmap.h"

int main() {
	ub_flatmap kv;
	ub_flatmap_init<int, int>(&kv);

	ub_flatmap_insert(&kv, 1, 1);

	if (int* gotten_val = (int*)ub_flatmap_get(&kv, 1)) {
		printf("got a value: %d\n", *gotten_val);
	} else {
		printf("got a null pointer\n");
	}

	return 0;
}
