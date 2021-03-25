#ifndef DYNARR_H
#define DYNARR_H

#include <stddef.h>
#include <stdlib.h>

/* typical usage:
 *
 * double *arr = da_init();
 * da_push(&arr, 1);
 * da_push(&arr, 2);
 * da_push(&arr, 3);
 * for (size_t i = 0; i < da_size(arr); ++i)
 *    printf("%ld ", arr[i]);
 * da_free(&arr);
 *
 */

#define da_push(ap, elem) \
  (da_grow((ap), 1) == 0 ? ((*(ap))[da_size(*(ap)) - 1] = (elem), 0) : -1)

#define da_resize(a,size)  da_resize_((a), (size), sizeof **(a))
#define da_reserve(a,size) da_reserve_((a), (size), sizeof **(a))
#define da_grow(a,diff)    da_grow_((a), (diff), sizeof *(a))

// todo:
//#define da_grow(a, cnt)   da_grow_((a), (cnt), sizeof *(a))
//#define da_create(TYPE,SIZE,VAL) da_create(&(type){VAL}
//#define da_pop(arrp)
//#define da_reserve()
//#define da_put()
//#define da_get()

#define DA_ALIGN _Alignof(max_align_t)

struct da_header_ {
	size_t size;
	size_t caps;
	// if da_header_ is max aligned then `data` is as well
	_Alignas(DA_ALIGN) char data[];
};

static inline struct da_header_ *da_header_(char* data) {
	void *ptr = data - sizeof(struct da_header_);
	struct da_header_ *hdr = ptr;
	return hdr;
}

static inline void *da_init(void) {
  static struct da_header_ dummy;
  return dummy.data;
}

static inline size_t da_size(void *data) {
	return da_header_(data)->size;
}

static inline size_t da_caps(void *data) {
	return da_header_(data)->caps;
}

static inline void da_free(void *pdata) {
	struct da_header_ *hdr = da_header_(*(void**)pdata);
	if (hdr->caps) free(hdr);
}

static inline struct da_header_ *da_realloc_(struct da_header_ *hdr, size_t caps, size_t elemsize) {
	size_t bytes = sizeof (struct da_header_) + caps * elemsize;
	// round allocations to 64 bytes
	bytes = (bytes + 63u) / 64u * 64u;
	hdr = (hdr->caps == 0) ? malloc(bytes) : realloc(hdr, bytes);
	if (!hdr) return NULL;
	hdr->caps = (bytes - sizeof *hdr) / elemsize;
	return hdr;
}

static inline int da_reserve_(void *pdata, size_t caps, size_t elemsize) {
	struct da_header_ *hdr = da_header_(*(void**)pdata);
	if (caps <= hdr->caps) return 0;
	size_t size = hdr->size;
	hdr = da_realloc_(hdr, caps, elemsize);
	if (!hdr) return -1;
	hdr->size = size;
	*(void**)pdata = hdr->data;
	return 0;
}

static inline int da_grow_(void *pdata, size_t diff, size_t elemsize) {
	struct da_header_ *hdr = da_header_(*(void**)pdata);
	size_t newsize = hdr->size + diff;
	if (newsize > hdr->caps) {
		size_t caps = (5 * newsize) / 4 + 1;
		hdr = da_realloc_(hdr, caps, elemsize);
		if (!hdr) return -1;
		*(void**)pdata = hdr->data;
	}
	hdr->size = newsize;
	return 0;
}

static inline int da_resize_(void *pdata, size_t size, size_t elemsize) {
	struct da_header_ *hdr = da_header_(*(void**)pdata);
	hdr = da_realloc_(hdr, size, elemsize);
	if (!hdr) return -1;
	hdr->size = size;
	*(void**)pdata = hdr->data;
	return 0;
}

#endif
