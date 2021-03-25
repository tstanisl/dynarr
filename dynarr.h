#ifndef DYNARR_H
#define DYNARR_H

#include <assert.h>
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

#define da_push(ap, elem)                           \
  (da_resize((ap), 1 + da_size(*(ap))) == 0 ?       \
   ((*(ap))[da_size(*(ap)) - 1] = (elem), 0) : -1)

#define da_resize(a,size) da_resize_((a), (size), sizeof **(a))

// todo:
//#define da_grow(a, cnt)   da_grow_((a), (cnt), sizeof *(a))
//#define da_create(TYPE,SIZE,VAL) da_create(&(type){VAL}
//#define da_pop(arrp)
//#define da_reserve()
//#define da_put()
//#define da_get()

#define DA_ALIGN _Alignof(max_align_t)

struct da_header {
	size_t size;
	size_t caps;
	// if da_header is max aligned then `data` is as well
	_Alignas(DA_ALIGN) char data[];
};

static inline struct da_header* da_header(char* data) {
	void *ptr = data - sizeof(struct da_header);
	struct da_header *hdr = ptr;
	assert(hdr->size <= hdr->caps);
	return hdr;
}

static inline void *da_init(void) {
  static struct da_header dummy;
  return dummy.data;
}

static inline size_t da_size(void *data) {
	return da_header(data)->size;
}

static inline size_t da_caps(void *data) {
	return da_header(data)->caps;
}

static inline void da_destroy(void *pdata) {
	struct da_header *hdr = da_header(*(void**)pdata);
	if (hdr->caps) free(hdr);
}

static inline int da_resize_(void *pdata, size_t size, size_t elemsize) {
	struct da_header *hdr = da_header(*(void**)pdata);
	if (size <= hdr->caps) {
		hdr->size = size;
		return 0;
	}
	size_t caps = (9 * hdr->caps) / 8 + 1;
	if (hdr->caps == 0) hdr = NULL;
	if (caps < size) caps = size;
	size_t bytes = sizeof (struct da_header) + caps * elemsize;
	struct da_header *newhdr = realloc(hdr, bytes);
	if (!newhdr)
		return -1;
	newhdr->size = size;
	newhdr->caps = caps;
	*(void**)pdata = newhdr->data;
	return 0;
}
#endif
