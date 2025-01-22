#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#define DeclareBinaryHeap(type)                                                                                                            \
	typedef struct {                                                                                                                   \
		type##ArrayList heap;                                                                                                      \
	} type##BinaryHeap;                                                                                                                \
                                                                                                                                           \
	type##BinaryHeap type##BinaryHeap_with_capacity(size_t capacity);                                                                  \
	void type##BinaryHeap_destroy(type##BinaryHeap self);                                                                              \
	type type##BinaryHeap_peak(type##BinaryHeap* self);                                                                                \
	type type##BinaryHeap_pop(type##BinaryHeap* self);                                                                                 \
	void type##BinaryHeap_insert(type##BinaryHeap* self, type value);                                                                  \
	bool type##BinaryHeap_is_empty(type##BinaryHeap* self);

// TODO: add is_superior and is_inferior functions for comparison because it's easier to understand and read
#define DefineBinaryHeap(type)                                                                                                             \
	type##BinaryHeap type##BinaryHeap_with_capacity(size_t capacity) {                                                                 \
		return (type##BinaryHeap){                                                                                                 \
		    .heap = type##ArrayList_with_capacity(capacity),                                                                       \
		};                                                                                                                         \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##BinaryHeap_destroy(type##BinaryHeap self) {                                                                             \
		type##ArrayList_destroy(self.heap);                                                                                        \
	}                                                                                                                                  \
                                                                                                                                           \
	size_t type##BinaryHeap_idx_left_child(type##BinaryHeap* self, size_t idx) {                                                       \
		ASSERT(idx < self->heap.length);                                                                                           \
		return 2 * idx + 1;                                                                                                        \
	}                                                                                                                                  \
                                                                                                                                           \
	size_t type##BinaryHeap_idx_right_child(type##BinaryHeap* self, size_t idx) {                                                      \
		ASSERT(idx < self->heap.length);                                                                                           \
		return 2 * idx + 2;                                                                                                        \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##BinaryHeap_heapify(type##BinaryHeap* self, size_t idx) {                                                                \
		size_t left_child_idx  = type##BinaryHeap_idx_left_child(self, idx);                                                       \
		size_t right_child_idx = type##BinaryHeap_idx_right_child(self, idx);                                                      \
		size_t smallest_idx    = idx;                                                                                              \
		if (left_child_idx < self->heap.length &&                                                                                  \
		    type##_compare(&self->heap.array[left_child_idx], &self->heap.array[smallest_idx]) < 0) {                              \
			smallest_idx = left_child_idx;                                                                                     \
		}                                                                                                                          \
		if (right_child_idx < self->heap.length &&                                                                                 \
		    type##_compare(&self->heap.array[right_child_idx], &self->heap.array[smallest_idx]) < 0) {                             \
			smallest_idx = right_child_idx;                                                                                    \
		}                                                                                                                          \
		if (smallest_idx != idx) {                                                                                                 \
			type##ArrayList_swap(&self->heap, idx, smallest_idx);                                                              \
			type##BinaryHeap_heapify(self, smallest_idx);                                                                      \
		}                                                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	type type##BinaryHeap_peak(type##BinaryHeap* self) {                                                                               \
		ASSERT(self->heap.length > 0);                                                                                             \
		return self->heap.array[0];                                                                                                \
	}                                                                                                                                  \
                                                                                                                                           \
	type type##BinaryHeap_pop(type##BinaryHeap* self) {                                                                                \
		ASSERT(self->heap.length > 0);                                                                                             \
		if (self->heap.length == 1) {                                                                                              \
			return type##ArrayList_pop_first(&self->heap);                                                                     \
		}                                                                                                                          \
		else {                                                                                                                     \
			type result = type##ArrayList_pop_nth_swap(&self->heap, 0);                                                        \
			type##BinaryHeap_heapify(self, 0);                                                                                 \
			return result;                                                                                                     \
		}                                                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##BinaryHeap_insert(type##BinaryHeap* self, type value) {                                                                 \
		type##ArrayList_push(&self->heap, value);                                                                                  \
		size_t idx = self->heap.length - 1;                                                                                        \
		while (idx > 0 && type##_compare(&self->heap.array[idx], &self->heap.array[(idx - 1) / 2]) < 0) {                          \
			type##ArrayList_swap(&self->heap, idx, (idx - 1) / 2);                                                             \
			idx = (idx - 1) / 2;                                                                                               \
		}                                                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	bool type##BinaryHeap_is_empty(type##BinaryHeap* self) {                                                                           \
		return self->heap.length == 0;                                                                                             \
	}

#endif