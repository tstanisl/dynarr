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

#if 0
size_t da_size(void *data);
size_t da_caps(void *data);
//todo: rename to da_free
void da_destroy(void *pdata);

// private stuff, do not use directly
int da_resize_(void *pdata, size_t size, size_t elemsize);
#endif

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

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define DYNARR__ALIGN 64

#ifndef N_DEBUG
#  define DYNARR__MAGIC "DynArray"
#endif

struct da_header {
	size_t size;
	size_t caps;
#ifdef DYNARR__MAGIC
	char magic[8];
#endif
	_Alignas(DYNARR__ALIGN / 2) char data[];
};

static inline void *da_init(void) {
  static struct da_header dummy_header;
  return dummy_header.data;
}

//#define DBG printf
#define DBG(...)

_Static_assert(sizeof(struct da_header) <= DYNARR__ALIGN / 2, "Invalid size of DynArr header");

static inline struct da_header* da_header(char* data) {
	void *ptr = data - sizeof(struct da_header);
	//assert((uintptr_t)ptr % DYNARR__ALIGN == 0 && "bad alignment, not DynArr");
	struct da_header *hdr = ptr;
	DBG("header=%p\n", (void*)hdr);
#ifdef DYNARR__MAGIC
	assert(memcmp(hdr->magic, DYNARR__MAGIC, sizeof(hdr->magic)) == 0 && "invalid magic, not DynArr");
#endif
	assert(hdr->size <= hdr->caps);
	return hdr;
}

static inline size_t da_size(void *data) {
	return da_header(data)->size;
	return data ? da_header(data)->size : 0;
}

static inline size_t da_caps(void *data) {
	return da_header(data)->caps;
	return data ? da_header(data)->caps : 0;
}

static inline struct da_header *da_alloc_header(size_t size, size_t caps, size_t elemsize) {
	// fixme: realloc does not guarantee alignment
	size_t bytes = (sizeof (struct da_header) + caps * elemsize + DYNARR__ALIGN - 1) / DYNARR__ALIGN * DYNARR__ALIGN;
	struct da_header *hdr = aligned_alloc(DYNARR__ALIGN, bytes); 
	if (!hdr)
		return NULL;
#ifdef DYNARR__MAGIC
	memcpy(hdr->magic, DYNARR__MAGIC, sizeof hdr->magic);
#endif
	hdr->size = size;
	hdr->caps = (bytes - sizeof *hdr) / elemsize;
	return hdr;
}

static inline void da_destroy(void *pdata) {
	void **data = (void**)pdata;
	struct da_header *hdr = da_header(*data);
#ifdef DYNARR__MAGIC
	for (size_t i = 0; i < sizeof hdr->magic; ++i)
		((volatile char *)hdr->magic)[i] = 0;
#endif
	if (hdr->caps) free(hdr);
}

static inline int da_resize_(void *pdata, size_t size, size_t elemsize) {
	void **data = (void**)pdata;
	DBG("*pdata=%p size=%zi elemsize=%zi\n", *data, size, elemsize);

	struct da_header *hdr = da_header(*data);
	if (size <= hdr->caps) {
		hdr->size = size;
		return 0;
	}
	size_t caps = (9 * hdr->caps) / 8 + 1;
	if (hdr->caps == 0) hdr = NULL;
	if (caps < size) caps = size;
	//size_t bytes = (sizeof (struct da_header) + caps * elemsize + DYNARR__ALIGN - 1) / DYNARR__ALIGN * DYNARR__ALIGN;
	size_t bytes = sizeof (struct da_header) + caps * elemsize;
	struct da_header *newhdr = realloc(hdr, bytes);
	//struct da_header *newhdr = da_alloc_header(size, caps, elemsize);
	if (!newhdr)
		return -1;
	newhdr->size = size;
	newhdr->caps = caps;
	//newhdr->caps = (bytes - sizeof *hdr) / elemsize;
	//memcpy(newhdr->data, hdr->data, hdr->size * elemsize);
	//DBG("release %p\n", (void*)hdr);
	//free(hdr);
	*data = newhdr->data;
	return 0;
}
#endif
