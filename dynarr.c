#include "dynarr.h"

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

//#define DBG printf
#define DBG

_Static_assert(sizeof(struct da_header) <= DYNARR__ALIGN / 2, "Invalid size of DynArr header");

static struct da_header* da_header(char* data) {
	void *ptr = data - sizeof(struct da_header);
	assert((uintptr_t)ptr % DYNARR__ALIGN == 0 && "bad alignment, not DynArr");
	struct da_header *hdr = ptr;
	DBG("header=%p\n", (void*)hdr);
#ifdef DYNARR__MAGIC
	assert(memcmp(hdr->magic, DYNARR__MAGIC, sizeof(hdr->magic)) == 0 && "invalid magic, not DynArr");
#endif
	assert(hdr->size <= hdr->caps);
	return hdr;
}

size_t da_size(void *data) {
	return data ? da_header(data)->size : 0;
}

size_t da_caps(void *data) {
	return data ? da_header(data)->caps : 0;
}

static struct da_header *da_alloc_header(size_t size, size_t caps, size_t elemsize) {
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

void da_destroy(void *pdata) {
	void **data = (void**)pdata;
	if (!*data)
		return;
	struct da_header *hdr = da_header(*data);
#ifdef DYNARR__MAGIC
	for (size_t i = 0; i < sizeof hdr->magic; ++i)
		((volatile char *)hdr->magic)[i] = 0;
#endif
	free(hdr);
}

int da_resize_(void *pdata, size_t size, size_t elemsize) {
	void **data = (void**)pdata;
	DBG("*pdata=%p size=%zi elemsize=%zi\n", *data, size, elemsize);
	if (!*data) {
		struct da_header *hdr = da_alloc_header(size, size, elemsize);
		if (!hdr)
			return -1;
		*data = hdr->data;
		return 0;
	}
	struct da_header *hdr = da_header(*data);
	if (size <= hdr->caps) {
		hdr->size = size;
		return 0;
	}
	size_t caps = (3 * hdr->caps) / 2 + 1;
	if (caps < size) caps = size;
	struct da_header *newhdr = da_alloc_header(size, caps, elemsize);
	if (!newhdr)
		return -1;
	memcpy(newhdr->data, hdr->data, hdr->size * elemsize);
	DBG("release %p\n", (void*)hdr);
	free(hdr);
	*data = newhdr->data;
	return 0;
}
