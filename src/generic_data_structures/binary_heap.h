#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#define DeclareBinaryHeap(type)                                                                              \
	typedef struct {                                                                                         \
		type##Vector array;                                                                                  \
	} type##BinaryHeap;                                                                                      \
                                                                                                             \
	type##BinaryHeap type##BinaryHeap_with_capacity(size_t capacity);                                        \
	void type##BinaryHeap_destroy(type##BinaryHeap self);                                                    \
	type type##BinaryHeap_peak(type##BinaryHeap* self);                                                      \
	type type##BinaryHeap_pop(type##BinaryHeap* self);                                                       \
	void type##BinaryHeap_insert(type##BinaryHeap* self, type value);                                        \
	bool type##BinaryHeap_is_empty(type##BinaryHeap* self);

// TODO: add is_superior and is_inferior functions for comparison because it's easier to understand and read
#define DefineBinaryHeap(type)                                                                               \
	type##BinaryHeap type##BinaryHeap_with_capacity(size_t capacity) {                                       \
		return (type##BinaryHeap){                                                                           \
			.array = type##Vector_with_capacity(capacity),                                                   \
		};                                                                                                   \
	}                                                                                                        \
                                                                                                             \
	void type##BinaryHeap_destroy(type##BinaryHeap self) {                                                   \
		type##Vector_destroy(self.array);                                                                    \
	}                                                                                                        \
                                                                                                             \
	size_t type##BinaryHeap_idx_left_child(type##BinaryHeap* self, size_t idx) {                             \
		ASSERT(idx < self->array.size);                                                                      \
		return 2 * idx + 1;                                                                                  \
	}                                                                                                        \
                                                                                                             \
	size_t type##BinaryHeap_idx_right_child(type##BinaryHeap* self, size_t idx) {                            \
		ASSERT(idx < self->array.size);                                                                      \
		return 2 * idx + 2;                                                                                  \
	}                                                                                                        \
                                                                                                             \
	void type##BinaryHeap_heapify(type##BinaryHeap* self, size_t idx) {                                      \
		size_t left_child_idx  = type##BinaryHeap_idx_left_child(self, idx);                                 \
		size_t right_child_idx = type##BinaryHeap_idx_right_child(self, idx);                                \
		size_t smallest_idx	   = idx;                                                                        \
		if (left_child_idx < self->array.size &&                                                             \
			type##_compare(&self->array.array[left_child_idx], &self->array.array[smallest_idx]) < 0) {      \
			smallest_idx = left_child_idx;                                                                   \
		}                                                                                                    \
		if (right_child_idx < self->array.size &&                                                            \
			type##_compare(&self->array.array[right_child_idx], &self->array.array[smallest_idx]) < 0) {     \
			smallest_idx = right_child_idx;                                                                  \
		}                                                                                                    \
		if (smallest_idx != idx) {                                                                           \
			type##Vector_swap(&self->array, idx, smallest_idx);                                              \
			type##BinaryHeap_heapify(self, smallest_idx);                                                    \
		}                                                                                                    \
	}                                                                                                        \
                                                                                                             \
	type type##BinaryHeap_peak(type##BinaryHeap* self) {                                                     \
		ASSERT(self->array.size > 0);                                                                        \
		return self->array.array[0];                                                                         \
	}                                                                                                        \
                                                                                                             \
	type type##BinaryHeap_pop(type##BinaryHeap* self) {                                                      \
		ASSERT(self->array.size > 0);                                                                        \
		if (self->array.size == 1) {                                                                         \
			return type##Vector_pop_first(&self->array);                                                     \
		}                                                                                                    \
		else {                                                                                               \
			type result = type##Vector_pop_nth_swap(&self->array, 0);                                        \
			type##BinaryHeap_heapify(self, 0);                                                               \
			return result;                                                                                   \
		}                                                                                                    \
	}                                                                                                        \
                                                                                                             \
	void type##BinaryHeap_insert(type##BinaryHeap* self, type value) {                                       \
		type##Vector_push(&self->array, value);                                                              \
		size_t idx = self->array.size - 1;                                                                   \
		while (idx > 0 && type##_compare(&self->array.array[idx], &self->array.array[(idx - 1) / 2]) < 0) {  \
			type##Vector_swap(&self->array, idx, (idx - 1) / 2);                                             \
			idx = (idx - 1) / 2;                                                                             \
		}                                                                                                    \
	}                                                                                                        \
                                                                                                             \
	bool type##BinaryHeap_is_empty(type##BinaryHeap* self) {                                                 \
		return self->array.size == 0;                                                                        \
	}

#endif