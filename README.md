# dynarr
A type-generic container similar to vector from C++ written in pure C.
It's header only library, just copy `dynarr.h` to your headers.

Typical usage:
```
double *arr = DA_INIT;
da_push(&arr, 1);
da_push(&arr, 2);
da_push(&arr, 3);
// da_push(&arr, "hello"); // error due to incompatible type
for (size_t i = 0; i < da_size(arr); ++i)
   printf("%ld ", arr[i]);
da_free(&arr);
```
