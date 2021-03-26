# dynarr
A type-generic container similar to vector from C++ written in pure C.
It's header only library, just copy `dynarr.h` to your headers.
Benchmark shows that it's slighly faster that `std::vector`.

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

# API

````
DA_INIT
```

a literal of `void*` type used to initialize all `DynArr` objects.

```
da_init();
```

A helper that returns `DA_INIT` value.

```
da_size(&da);
```
Returns a number of elements in `DynArr` container.

```
da_caps(&da);
```
Returns a capacity of the container. Adding new elements will not cause reallocation
until `da_caps()` objects are inserted.

```
da_resize(&da, newsize);
```

A macro, resizes the `DynArr` to desired size. Content of objects at `da[oldsize ... newsize-1]` is undefined.
Return 0 on success, -1 on failure.

```
da_reserve(&da, caps);
```
A macro, reserves place for `caps` object but leaves size unchanged.
Return 0 on success, -1 on failure.

```
da_grow(&da, diff);
```
Change size by `diff`, can be negative to make `DynArr` samller.
value of `-diff` must be lesser or equal to the current size.
Return 0 on success, -1 on failure.
Negative grows are guaranteed to succeed.

```
da_push(&da, value);
```
Adds `value` at the end of container.
Return 0 on success, -1 on failure.

```
da_pop(&da);
```
Returns the last value and shrinks the container.

