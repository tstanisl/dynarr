#ifndef DYNARR_H
#define DYNARR_H

#include <stdlib.h>
/* typical usage:
 * double *arr = NULL;
 * darr_push(&arr, 1);
 * darr_push(&arr, 2);
 * darr_push(&arr, 3);
 * for (size_t i = 0; i < darr_size(arr); ++i)
 *    printf("%ld ", arr[i]);
 */

#define da_push(ap, elem)                           \
  (da_resize((ap), 1 + da_size(*(ap))) == 0 ?       \
   ((*(ap))[da_size(*(ap)) - 1] = (elem), 0) : -1)

// todo: add da_resize_safe(a,size,stmt) where stmt is excuted on failure
#define da_resize(a,size) da_resize_((a), (size), sizeof **(a))

size_t da_size(void *data);
size_t da_caps(void *data);
int da_resize_(void *pdata, size_t size, size_t elemsize);
void da_destroy(void *pdata);

// wip
//#define da_grow(a, cnt)   da_grow_((a), (cnt), sizeof *(a))
//void da_grow_(void **pdata, ptrdiff_t ecount, size_t esize);
//#define da_create(TYPE,SIZE,VAL) da_create(&(type){VAL}
//#define da_pop(arrp)
//#define da_capacity(arr)
//#define da_resize()
//#define da_reserve()
//#define darr_put()
//#define darr_get()

#endif
