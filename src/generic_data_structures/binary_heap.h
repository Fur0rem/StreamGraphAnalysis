#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#define DeclareBinaryHeap(type)                                                                                        \
	typedef struct {                                                                                                   \
		type##Vector array;                                                                                            \
	} type##BinaryHeap;                                                                                                \
                                                                                                                       \
	type##BinaryHeap type##BinaryHeap_with_capacity(size_t capacity);                                                  \
	void type##BinaryHeap_destroy(type##BinaryHeap heap);                                                              \
	type type##BinaryHeap_peak(type##BinaryHeap* heap);                                                                \
	type type##BinaryHeap_pop(type##BinaryHeap* heap);                                                                 \
	void type##BinaryHeap_insert(type##BinaryHeap* heap, type value);                                                  \
	bool type##BinaryHeap_is_empty(type##BinaryHeap* heap);

// TODO: add is_superior and is_inferior functions for comparison because it's easier to understand and read
#define DefineBinaryHeap(type)                                                                                         \
	type##BinaryHeap type##BinaryHeap_with_capacity(size_t capacity) {                                                 \
		return (type##BinaryHeap){                                                                                     \
			.array = type##Vector_with_capacity(capacity),                                                             \
		};                                                                                                             \
	}                                                                                                                  \
                                                                                                                       \
	void type##BinaryHeap_destroy(type##BinaryHeap heap) {                                                             \
		type##Vector_destroy(heap.array);                                                                              \
	}                                                                                                                  \
                                                                                                                       \
	size_t type##BinaryHeap_idx_left_child(type##BinaryHeap* heap, size_t idx) {                                       \
		ASSERT(idx < heap->array.size);                                                                                \
		return 2 * idx + 1;                                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	size_t type##BinaryHeap_idx_right_child(type##BinaryHeap* heap, size_t idx) {                                      \
		ASSERT(idx < heap->array.size);                                                                                \
		return 2 * idx + 2;                                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	void type##BinaryHeap_heapify(type##BinaryHeap* heap, size_t idx) {                                                \
		size_t left_child_idx  = type##BinaryHeap_idx_left_child(heap, idx);                                           \
		size_t right_child_idx = type##BinaryHeap_idx_right_child(heap, idx);                                          \
		size_t smallest_idx	   = idx;                                                                                  \
		if (left_child_idx < heap->array.size &&                                                                       \
			type##_compare(&heap->array.array[left_child_idx], &heap->array.array[smallest_idx]) < 0) {                \
			smallest_idx = left_child_idx;                                                                             \
		}                                                                                                              \
		if (right_child_idx < heap->array.size &&                                                                      \
			type##_compare(&heap->array.array[right_child_idx], &heap->array.array[smallest_idx]) < 0) {               \
			smallest_idx = right_child_idx;                                                                            \
		}                                                                                                              \
		if (smallest_idx != idx) {                                                                                     \
			type##Vector_swap(&heap->array, idx, smallest_idx);                                                        \
			type##BinaryHeap_heapify(heap, smallest_idx);                                                              \
		}                                                                                                              \
	}                                                                                                                  \
                                                                                                                       \
	type type##BinaryHeap_peak(type##BinaryHeap* heap) {                                                               \
		ASSERT(heap->array.size > 0);                                                                                  \
		return heap->array.array[0];                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	type type##BinaryHeap_pop(type##BinaryHeap* heap) {                                                                \
		ASSERT(heap->array.size > 0);                                                                                  \
		if (heap->array.size == 1) {                                                                                   \
			return type##Vector_pop_first(&heap->array);                                                               \
		}                                                                                                              \
		else {                                                                                                         \
			type result = type##Vector_pop_nth_swap(&heap->array, 0);                                                  \
			type##BinaryHeap_heapify(heap, 0);                                                                         \
			return result;                                                                                             \
		}                                                                                                              \
	}                                                                                                                  \
                                                                                                                       \
	void type##BinaryHeap_insert(type##BinaryHeap* heap, type value) {                                                 \
		type##Vector_push(&heap->array, value);                                                                        \
		size_t idx = heap->array.size - 1;                                                                             \
		while (idx > 0 && type##_compare(&heap->array.array[idx], &heap->array.array[(idx - 1) / 2]) < 0) {            \
			type##Vector_swap(&heap->array, idx, (idx - 1) / 2);                                                       \
			idx = (idx - 1) / 2;                                                                                       \
		}                                                                                                              \
	}                                                                                                                  \
                                                                                                                       \
	bool type##BinaryHeap_is_empty(type##BinaryHeap* heap) {                                                           \
		return heap->array.size == 0;                                                                                  \
	}

#endif