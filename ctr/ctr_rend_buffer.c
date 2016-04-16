#include "ctr_rend.h"
#include <stdlib.h>
#ifdef _3DS
#include <3ds/allocator/linear.h>
#endif

#define CTR_REND_BUFFER_ALIGN_SIZE 16
#define CTR_REND_BUFFER_ALIGN (CTR_REND_BUFFER_ALIGN_SIZE-1)

#define CTR_REND_BUFFER_MAX (6 * 1024 * 1024)
static GLubyte *ctr_buffer = 0;
static int ctr_buffer_frame = 0;
static int ctr_buf_min;
static int ctr_buf_max;
static int ctr_buf_cnt;
static int ctr_buf_last_pos;
static int ctr_buf_last_cb;

void ctr_buf_cb(int cb) {
	ctr_buf_last_cb = cb;
	ctr_buf_last_pos = ctr_state.buffer_pos;
	if (cb < ctr_buf_min) {
		ctr_buf_min = cb;
	}
	if (cb > ctr_buf_max) {
		ctr_buf_max = cb;
	}
	ctr_buf_cnt++;
}

void ctr_buf_print(int cb) {
	printf("ctr_rend_buf: %d %d %d %d\n", cb, ctr_buf_min, ctr_buf_max, ctr_buf_cnt);
	printf("%d %d %d\n", cb, ctr_buf_last_cb, ctr_buf_last_pos, ctr_state.buffer_pos);
}

void ctr_rend_buffer_init() {
	ctr_state.buffer = ctr_buffer = linearMemAlign(CTR_REND_BUFFER_MAX, 128);
	if (ctr_state.buffer == 0) {
		printf("-------------\n\nctr_rend_buffer_copy_stride Out of Memory\n\n-------------\n");
		while (1);
	}
	ctr_state.buffer_len = CTR_REND_BUFFER_MAX;
	ctr_state.buffer_pos = 0;
	ctr_buffer_frame = 0;
}

void* ctr_rend_buffer_alloc(int len) {
	int cb = (len + CTR_REND_BUFFER_ALIGN) & (~CTR_REND_BUFFER_ALIGN);
	if (ctr_state.buffer_pos + cb > CTR_REND_BUFFER_MAX) {
		printf("-------------\n\nctr_rend_buffer_alloc Out of Memory\n\n-------------\n");
		ctr_buf_print(cb);
		do {
			svcSleepThread(20000);
		}
		while (1);
		return 0;
	}
	ctr_buf_cb(cb);
	GLubyte *addr = &ctr_state.buffer[ctr_state.buffer_pos];
	ctr_state.buffer_pos += cb;
	return addr;
}
void* ctr_rend_buffer_copy(void *p, int len) {
	int cb = (len + CTR_REND_BUFFER_ALIGN) & ( ~CTR_REND_BUFFER_ALIGN);
	if (ctr_state.buffer_pos + cb > CTR_REND_BUFFER_MAX) {
		printf("-------------\n\nctr_rend_buffer_copy Out of Memory\n\n-------------\n");
		ctr_buf_print(cb);
		do {
			svcSleepThread(20000);
		} while (1);
		return 0;
	}
	ctr_buf_cb(cb);
	GLubyte *addr = &ctr_state.buffer[ctr_state.buffer_pos];
	memcpy(addr, p, len);
	ctr_state.buffer_pos += cb;

	return addr;
}

void* ctr_rend_buffer_copy_stride(void *p, int count, int size, int stride) {
	int len = size * count;
	int cb = (len + CTR_REND_BUFFER_ALIGN) & (~CTR_REND_BUFFER_ALIGN);
	if (ctr_state.buffer_pos + cb > CTR_REND_BUFFER_MAX) {
		printf("-------------\n\nctr_rend_buffer_copy_stride Out of Memory\n\n-------------\n");
		ctr_buf_print(cb);
		do {
			svcSleepThread(20000);
		} while (1);
		return 0;
	}
	ctr_buf_cb(cb);
	GLubyte *addr = &ctr_state.buffer[ctr_state.buffer_pos];
	u8 *src = p;
	u8 *dst = addr;
	int i,j;
	//printf("copy: %08x %d %d %d\n", p, count, size, stride);
	for (i = 0; i < count; i++) {
		memcpy(dst, src, size);
		//for (j = 0; j < size; j++) {
		//	printf("%02x", src[j]);
		//}
		//printf("%s", (i&1) ? "\n" : " ");
		src += stride;
		dst += size;
	}
	//if(i&1) printf("\n");
	ctr_state.buffer_pos += cb;

	return addr;
}

void ctr_rend_buffer_reset() {
	//ctr_buf_print(0);
	//printf("ctr_state.buffer_pos: %8d\n", ctr_state.buffer_pos);
	ctr_state.buffer_pos = 0;
	ctr_buffer_frame = ctr_buffer_frame ? 0 : 1;
	ctr_state.buffer = ctr_buffer;// +ctr_buffer_frame * CTR_REND_BUFFER_MAX;
	ctr_state.dirty = 1;
	ctr_state.dirty_matrix = 1;
	ctr_buf_min = 999999999;
	ctr_buf_max = -1;
	ctr_buf_cnt = 0;
}