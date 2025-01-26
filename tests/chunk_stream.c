/**
 * @file tests/chunk_stream.c
 * @brief Tests regarding ChunkStream
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stdint.h>

bool test_chunk_stream_nodes_and_links_set() {
	bool result		  = true;
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(2);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 2);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	SGA_Stream st	      = SGA_ChunkStream_with(&sg, &nodes, &links, SGA_Interval_from(0, 100));
	StreamFunctions funcs = ChunkStream_stream_functions;

	SGA_NodesIterator nodes_iter  = funcs.nodes_set(st.stream_data);
	SGA_NodeIdArrayList nodes_vec = SGA_collect_node_ids(nodes_iter);

	SGA_LinksIterator links_iter  = funcs.links_set(st.stream_data);
	SGA_LinkIdArrayList links_vec = SGA_collect_link_ids(links_iter);

	result &= EXPECT(SGA_NodeIdArrayList_len(&nodes_vec) == 2) && EXPECT(SGA_LinkIdArrayList_len(&links_vec) == 1);
	result &= EXPECT(nodes_vec.array[0] == 0) && EXPECT(nodes_vec.array[1] == 2);
	result &=
	    EXPECT(EXPECT(sg.links.links[links_vec.array[0]].nodes[0] == 0) && EXPECT(sg.links.links[links_vec.array[0]].nodes[1] == 2)) ||
	    (EXPECT(sg.links.links[links_vec.array[0]].nodes[0] == 2) && EXPECT(sg.links.links[links_vec.array[0]].nodes[1] == 0));

	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);

	SGA_NodeIdArrayList_destroy(nodes_vec);
	SGA_LinkIdArrayList_destroy(links_vec);
	return result;
}

bool test_neighbours_of_node_chunk_stream() {
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(2);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 1);
	SGA_NodeIdArrayList_push(&nodes, 3);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	// SGA_Stream st		 = CS_with(&sg, &nodes, &links, 0, 100);
	SGA_Stream st		     = SGA_ChunkStream_with(&sg, &nodes, &links, SGA_Interval_from(0, 100));
	StreamFunctions funcs	     = ChunkStream_stream_functions;
	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		SGA_LinksIterator neighbours_iter = funcs.neighbours_of_node(st.stream_data, node_id);
		printf("NODE %zu\n", node_id);
		SGA_FOR_EACH_LINK(link_id, neighbours_iter) {
			printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
		}
	}

	// CS_destroy(st);
	// StreamGraph_destroy(sg);
	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);
	return true;
}

bool test_times_node_present_chunk_stream() {
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(3);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 1);
	SGA_NodeIdArrayList_push(&nodes, 3);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	// SGA_Stream st		 = CS_with(&sg, &nodes, &links, 20, 80);
	SGA_Stream st		     = SGA_ChunkStream_with(&sg, &nodes, &links, SGA_Interval_from(20, 80));
	StreamFunctions funcs	     = ChunkStream_stream_functions;
	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		printf("NODE %zu : ", node_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	// CS_destroy(st);
	// StreamGraph_destroy(sg);
	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);
	return true;
}

bool test_times_node_present_chunk_stream_2() {
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(3);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 1);
	SGA_NodeIdArrayList_push(&nodes, 3);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	// SGA_Stream st		 = CS_with(&sg, &nodes, &links, 40, 45);
	SGA_Stream st		     = SGA_ChunkStream_with(&sg, &nodes, &links, SGA_Interval_from(40, 45));
	StreamFunctions funcs	     = ChunkStream_stream_functions;
	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		printf("NODE %zu : ", node_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	// CS_destroy(st);
	// StreamGraph_destroy(sg);
	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);
	return true;
}

bool test_link_presence_chunk_stream() {
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(3);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 1);
	SGA_NodeIdArrayList_push(&nodes, 3);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	// SGA_Stream st		 = CS_with(&sg, &nodes, &links, 20, 80);
	SGA_Stream st		     = SGA_ChunkStream_with(&sg, &nodes, &links, SGA_Interval_from(20, 80));
	StreamFunctions funcs	     = ChunkStream_stream_functions;
	SGA_LinksIterator links_iter = funcs.links_set(st.stream_data);
	SGA_FOR_EACH_LINK(link_id, links_iter) {
		SGA_TimesIterator times_iter = funcs.times_link_present(st.stream_data, link_id);
		printf("LINK %zu : ", link_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	// CS_destroy(st);
	// StreamGraph_destroy(sg);
	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);
	return true;
}

bool test_nodes_and_links_present_at_t_chunk_stream() {
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(3);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 1);
	SGA_NodeIdArrayList_push(&nodes, 3);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	// SGA_Stream st		 = CS_with(&sg, &nodes, &links, 20, 80);
	SGA_Stream st		     = SGA_ChunkStream_with(&sg, &nodes, &links, SGA_Interval_from(20, 80));
	StreamFunctions funcs	     = ChunkStream_stream_functions;
	SGA_NodesIterator nodes_iter = funcs.nodes_present_at_t(st.stream_data, 40);
	printf("STUFF PRESENT AT 40\n");
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		printf("NODE %zu : ", node_id);
		SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	SGA_LinksIterator links_iter = funcs.links_present_at_t(st.stream_data, 40);
	SGA_FOR_EACH_LINK(link_id, links_iter) {
		printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
	}

	// CS_destroy(st);
	// events_destroy(&sg);
	// StreamGraph_destroy(sg);
	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);
	return true;
}

bool test_key_moments() {
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(3);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 1);
	SGA_NodeIdArrayList_push(&nodes, 3);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	// SGA_Stream st		 = CS_with(&sg, &nodes, &links, 20, 80);
	SGA_Stream st			      = SGA_ChunkStream_with(&sg, &nodes, &links, SGA_Interval_from(20, 80));
	StreamFunctions funcs		      = ChunkStream_stream_functions;
	SGA_TimesIterator key_moments	      = funcs.key_moments(st.stream_data);
	SGA_IntervalArrayList key_moments_vec = SGA_collect_times(key_moments);
	for (size_t i = 0; i < key_moments_vec.length; i++) {
		SGA_Interval interval = key_moments_vec.array[i];
		printf("KEY MOMENT : [%lu, %lu[\n", interval.start, interval.end);
	}
	SGA_IntervalArrayList_destroy(key_moments_vec);

	// printf("KEY MOMENTS\n");
	// SGA_FOR_EACH_TIME(interval, key_moments) {
	// 	printf("KEY MOMENT : [%lu, %lu]\n", interval.start, interval.end);
	// }

	// CS_destroy(st);
	// events_destroy(&sg);
	// StreamGraph_destroy(sg);
	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);
	return true;
}

int main() {
	Test* tests[] = {
	    TEST(test_chunk_stream_nodes_and_links_set),
	    TEST(test_neighbours_of_node_chunk_stream),
	    TEST(test_times_node_present_chunk_stream),
	    TEST(test_times_node_present_chunk_stream_2),
	    TEST(test_link_presence_chunk_stream),
	    TEST(test_nodes_and_links_present_at_t_chunk_stream),
	    TEST(test_key_moments),
	    NULL,
	};
	return test("Chunk Stream", tests);
}