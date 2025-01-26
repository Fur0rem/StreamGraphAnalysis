/**
 * @file tests/chunk_stream_small.c
 * @brief Tests regarding ChunkStreamSmall
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"

bool test_chunk_stream_small_nodes_set() {
	SGA_StreamGraph sg	  = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_with_capacity(2);
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 2);

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_with_capacity(4);
	SGA_LinkIdArrayList_push(&links, 0);
	SGA_LinkIdArrayList_push(&links, 1);
	SGA_LinkIdArrayList_push(&links, 2);
	SGA_LinkIdArrayList_push(&links, 3);

	SGA_Stream st		     = SGA_ChunkStreamSmall_from(&sg, nodes, links, SGA_Interval_from(0, 100));
	StreamFunctions funcs	     = ChunkStreamSmall_stream_functions;
	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		printf("NODE %zu\n", node_id);
	}

	SGA_LinksIterator links_iter = funcs.links_set(st.stream_data);
	SGA_FOR_EACH_LINK(link_id, links_iter) {
		printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
	}

	SGA_ChunkStreamSmall_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_chunk_stream_small_neighbours_of_node() {
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

	SGA_Stream st	      = SGA_ChunkStreamSmall_from(&sg, nodes, links, SGA_Interval_from(0, 100));
	StreamFunctions funcs = ChunkStreamSmall_stream_functions;

	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		SGA_LinksIterator neighbours_iter = funcs.neighbours_of_node(st.stream_data, node_id);
		printf("NODE %zu\n", node_id);
		SGA_FOR_EACH_LINK(link_id, neighbours_iter) {
			printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
		}
	}

	SGA_ChunkStreamSmall_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_chunk_stream_small_times_node_present() {
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

	SGA_Stream st	      = SGA_ChunkStreamSmall_from(&sg, nodes, links, SGA_Interval_from(20, 80));
	StreamFunctions funcs = ChunkStreamSmall_stream_functions;

	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		printf("NODE %zu : ", node_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	SGA_LinksIterator links_iter = funcs.links_set(st.stream_data);
	SGA_FOR_EACH_LINK(link_id, links_iter) {
		SGA_TimesIterator times_iter = funcs.times_link_present(st.stream_data, link_id);
		printf("LINK %zu : ", link_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	SGA_ChunkStreamSmall_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return true;
}

int main() {
	Test* tests[] = {
	    TEST(test_chunk_stream_small_nodes_set),
	    TEST(test_chunk_stream_small_neighbours_of_node),
	    TEST(test_chunk_stream_small_times_node_present),
	    NULL,
	};

	return test("Chunk Stream Small", tests);
}