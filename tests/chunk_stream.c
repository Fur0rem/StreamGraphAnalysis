#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stdint.h>

bool test_chunk_stream_nodes_and_links_set() {
	bool result	      = true;
	StreamGraph sg	      = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(2);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 2);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st	      = CS_with(&sg, &nodes, &links, 0, 100);
	StreamFunctions funcs = ChunkStream_stream_functions;

	NodesIterator nodes_iter  = funcs.nodes_set(st.stream_data);
	NodeIdArrayList nodes_vec = SGA_collect_node_ids(nodes_iter);

	LinksIterator links_iter  = funcs.links_set(st.stream_data);
	LinkIdArrayList links_vec = SGA_collect_link_ids(links_iter);

	result &= EXPECT(NodeIdArrayList_len(&nodes_vec) == 2) && EXPECT(LinkIdArrayList_len(&links_vec) == 1);
	result &= EXPECT(nodes_vec.array[0] == 0) && EXPECT(nodes_vec.array[1] == 2);
	result &=
	    EXPECT(EXPECT(sg.links.links[links_vec.array[0]].nodes[0] == 0) && EXPECT(sg.links.links[links_vec.array[0]].nodes[1] == 2)) ||
	    (EXPECT(sg.links.links[links_vec.array[0]].nodes[0] == 2) && EXPECT(sg.links.links[links_vec.array[0]].nodes[1] == 0));

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdArrayList_destroy(nodes);
	LinkIdArrayList_destroy(links);

	NodeIdArrayList_destroy(nodes_vec);
	LinkIdArrayList_destroy(links_vec);
	return result;
}

bool test_neighbours_of_node_chunk_stream() {
	StreamGraph sg	      = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(2);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st		 = CS_with(&sg, &nodes, &links, 0, 100);
	StreamFunctions funcs	 = ChunkStream_stream_functions;
	NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	FOR_EACH_NODE(node_id, nodes_iter) {
		LinksIterator neighbours_iter = funcs.neighbours_of_node(st.stream_data, node_id);
		printf("NODE %zu\n", node_id);
		FOR_EACH_LINK(link_id, neighbours_iter) {
			printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
		}
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdArrayList_destroy(nodes);
	LinkIdArrayList_destroy(links);
	return true;
}

bool test_times_node_present_chunk_stream() {
	StreamGraph sg	      = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(3);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st		 = CS_with(&sg, &nodes, &links, 20, 80);
	StreamFunctions funcs	 = ChunkStream_stream_functions;
	NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	FOR_EACH_NODE(node_id, nodes_iter) {
		TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		printf("NODE %zu : ", node_id);
		FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdArrayList_destroy(nodes);
	LinkIdArrayList_destroy(links);
	return true;
}

bool test_times_node_present_chunk_stream_2() {
	StreamGraph sg	      = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(3);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st		 = CS_with(&sg, &nodes, &links, 40, 45);
	StreamFunctions funcs	 = ChunkStream_stream_functions;
	NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	FOR_EACH_NODE(node_id, nodes_iter) {
		TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		printf("NODE %zu : ", node_id);
		FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdArrayList_destroy(nodes);
	LinkIdArrayList_destroy(links);
	return true;
}

bool test_link_presence_chunk_stream() {
	StreamGraph sg	      = StreamGraph_from_file("data/S.txt");
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(3);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st		 = CS_with(&sg, &nodes, &links, 20, 80);
	StreamFunctions funcs	 = ChunkStream_stream_functions;
	LinksIterator links_iter = funcs.links_set(st.stream_data);
	FOR_EACH_LINK(link_id, links_iter) {
		TimesIterator times_iter = funcs.times_link_present(st.stream_data, link_id);
		printf("LINK %zu : ", link_id);
		FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdArrayList_destroy(nodes);
	LinkIdArrayList_destroy(links);
	return true;
}

bool test_nodes_and_links_present_at_t_chunk_stream() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	NodeIdArrayList nodes = NodeIdArrayList_with_capacity(3);
	NodeIdArrayList_push(&nodes, 0);
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);

	LinkIdArrayList links = LinkIdArrayList_with_capacity(4);
	LinkIdArrayList_push(&links, 0);
	LinkIdArrayList_push(&links, 1);
	LinkIdArrayList_push(&links, 2);
	LinkIdArrayList_push(&links, 3);

	Stream st		 = CS_with(&sg, &nodes, &links, 20, 80);
	StreamFunctions funcs	 = ChunkStream_stream_functions;
	NodesIterator nodes_iter = funcs.nodes_present_at_t(st.stream_data, 40);
	printf("STUFF PRESENT AT 40\n");
	FOR_EACH_NODE(node_id, nodes_iter) {
		printf("NODE %zu : ", node_id);
		TimesIterator times_iter = funcs.times_node_present(st.stream_data, node_id);
		FOR_EACH_TIME(interval, times_iter) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	LinksIterator links_iter = funcs.links_present_at_t(st.stream_data, 40);
	FOR_EACH_LINK(link_id, links_iter) {
		printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
	}

	CS_destroy(st);
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	NodeIdArrayList_destroy(nodes);
	LinkIdArrayList_destroy(links);
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
	    NULL,
	};
	return test("Chunk Stream", tests);
}