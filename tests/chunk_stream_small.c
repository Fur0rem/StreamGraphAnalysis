#include "../StreamGraphAnalysis.h"
#include "test.h"

bool test_chunk_stream_small_nodes_set() {
	StreamGraph sg		  = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(2);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 2);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st				 = ChunkStreamSmall_from(&sg, nodes, links, Interval_from(0, 100));
	StreamFunctions funcs	 = ChunkStreamSmall_stream_functions;
	NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	FOR_EACH_NODE(node_id, nodes_iter) {
		printf("NODE %zu\n", node_id);
	}

	LinksIterator links_iter = funcs.links_set(st.stream_data);
	FOR_EACH_LINK(link_id, links_iter) {
		printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
	}

	ChunkStreamSmall_destroy(st);
	StreamGraph_destroy(sg);
	return true;
}

bool test_chunk_stream_small_neighbours_of_node() {
	StreamGraph sg		  = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(2);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st			  = ChunkStreamSmall_from(&sg, nodes, links, Interval_from(0, 100));
	StreamFunctions funcs = ChunkStreamSmall_stream_functions;

	NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	FOR_EACH_NODE(node_id, nodes_iter) {
		LinksIterator neighbours_iter = funcs.neighbours_of_node(st.stream_data, node_id);
		printf("NODE %zu\n", node_id);
		FOR_EACH_LINK(link_id, neighbours_iter) {
			printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
		}
	}

	ChunkStreamSmall_destroy(st);
	StreamGraph_destroy(sg);
	return true;
}

// TODO: why is this here ???
bool test_chunk_stream_small_times_node_present() {
	StreamGraph sg		  = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(3);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st			  = ChunkStreamSmall_from(&sg, nodes, links, Interval_from(20, 80));
	StreamFunctions funcs = ChunkStreamSmall_stream_functions;

	NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	FOR_EACH_NODE(node_id, nodes_iter) {
		TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		printf("NODE %zu : ", node_id);
		FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	LinksIterator links_iter = funcs.links_set(st.stream_data);
	FOR_EACH_LINK(link_id, links_iter) {
		TimesIterator times_iter = funcs.times_link_present(st.stream_data, link_id);
		printf("LINK %zu : ", link_id);
		FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	ChunkStreamSmall_destroy(st);
	StreamGraph_destroy(sg);
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