#ifndef DYNARR_H
#define DYNARR_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
 *   PRIVATE STUFF
 */

struct da_header_ {
	size_t size;
	size_t caps;
	// if da_header_ is max aligned then `data` is as well
	_Alignas(_Alignof(max_align_t)) char data[];
};

static inline struct da_header_ *da_header_(char* data) {
	void *ptr = data - sizeof(struct da_header_);
	struct da_header_ *hdr = ptr;
	return hdr;
}

static struct da_header_ da_dummy_;

static inline struct da_header_ *da_realloc_(struct da_header_ *hdr, size_t caps, size_t elemsize) {
	if (caps == 0) {
		// silence the compiler about freeing `da_dummy_`
		if (hdr->caps && hdr != &da_dummy_) {
		  free(hdr);
		}
		return &da_dummy_;
	}
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

static inline int da_grow_(void *pdata, ptrdiff_t diff, size_t elemsize) {
	struct da_header_ *hdr = da_header_(*(void**)pdata);
	size_t newsize = hdr->size + diff;
	if (newsize > hdr->caps) {
		size_t caps = (5 * newsize + 3) / 4;
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

static inline int da_copy_(void *psrc, void *pdst, size_t elemsize) {
	struct da_header_ *shdr = da_header_(*(void**)psrc);
	struct da_header_ *dhdr = da_header_(*(void**)pdst);
	dhdr = da_realloc_(dhdr, shdr->size, elemsize);
	if (!dhdr) return -1;
	dhdr->size = shdr->size;
	memcpy(dhdr->data, shdr->data, shdr->size * elemsize);
	*(void**)pdst = dhdr->data;
	return 0;
}

/*
 *   PUBLIC API
 */

/* typical usage:
 *
 * double *arr = DA_INIT;
 * da_push(&arr, 1);
 * da_push(&arr, 2);
 * da_push(&arr, 3);
 * for (size_t i = 0; i < da_size(arr); ++i)
 *    printf("%ld ", arr[i]);
 * da_free(&arr);
 *
 */

#define DA_INIT ((void*)da_dummy_.data)

static inline void *da_init(void) {
  return DA_INIT;
}

#define da_resize(a,size)  da_resize_((a), (size), sizeof **(a))
#define da_reserve(a,size) da_reserve_((a), (size), sizeof **(a))
#define da_grow(a,diff)    da_grow_((a), (diff), sizeof *(a))

static inline size_t da_size(void *pdata) {
	return da_header_(*(void**)pdata)->size;
}

static inline size_t da_caps(void *pdata) {
	return da_header_(*(void**)pdata)->caps;
}

static inline void da_free(void *pdata) {
	struct da_header_ *hdr = da_header_(*(void**)pdata);
	if (hdr->caps) free(hdr);
	*(void**)pdata = DA_INIT;
}

#define da_push(ap, elem) \
  (da_grow((ap), 1) == 0 ? ((*(ap))[da_size(ap) - 1] = (elem), 0) : -1)

#define da_pop(ap) \
  (da_grow((ap), -1), (*(ap))[da_size(ap)])

#define da_copy(src, dst) \
  ((void)sizeof(*(src) == *(dst)), da_copy_(src, dst, sizeof **(src)))

// todo:
//#define da_create(TYPE,SIZE,VAL) da_create(&(type){VAL}
//#define da_put()
//#define da_get()

#endif
