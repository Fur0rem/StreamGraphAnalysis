/**
 * @file tests/isomorphism.c
 * @brief Tests regarding isomorphism
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"

bool test_equals() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_FullStreamGraph_destroy(stream2);

	return result;
}

bool test_isomorphism_chunk() {
	SGA_StreamGraph sg	    = SGA_StreamGraph_from_external("data/kcores_with_L.txt");
	SGA_StreamGraph kcores_only = SGA_StreamGraph_from_external("data/kcores_test.txt");

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_new();
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		SGA_LinkIdArrayList_push(&links, i);
		printf("Link %zu (%zu, %zu)\n", i, sg.links.links[i].nodes[0], sg.links.links[i].nodes[1]);
	}

	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_new();
	SGA_NodeIdArrayList_push(&nodes, 0);
	SGA_NodeIdArrayList_push(&nodes, 2);
	SGA_NodeIdArrayList_push(&nodes, 4);
	SGA_NodeIdArrayList_push(&nodes, 6);

	SGA_Interval snapshot = SGA_Interval_from(0, 10);
	// SGA_Stream st	  = CS_with(&sg, &nodes, &links, snapshot.start, snapshot.end);
	SGA_Stream st = SGA_ChunkStream_with(&sg, &nodes, &links, snapshot);
	// SGA_Stream og	  = SGA_LinkStream_from(&kcores_only);
	SGA_Stream og = SGA_LinkStream_from(&kcores_only);

	bool result = EXPECT(are_isomorphic(&st, &og));

	SGA_StreamGraph_destroy(sg);
	SGA_StreamGraph_destroy(kcores_only);
	SGA_LinkStream_destroy(og);
	SGA_ChunkStream_destroy(st);
	SGA_LinkIdArrayList_destroy(links);
	SGA_NodeIdArrayList_destroy(nodes);

	return result;

	// return false;
}

bool test_not_isomorphic() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/S_concat_L.txt");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_FullStreamGraph_destroy(stream2);

	return result;
}

bool test_not_isomorphic_stretched() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_external("data/L.txt");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/L_10.txt");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

bool test_isomorphism_offset() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_external("data/L.txt");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/L_offset.txt");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

bool test_isomorphic_ids_scrambled() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_external("data/L.txt");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/L_scrambled.txt");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

bool test_isomorphism_different_node_times() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/S_external_different_node_time.txt");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_FullStreamGraph_destroy(stream2);

	return result;
}

bool test_isomorphism_not_constant_offset() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_external("data/L.txt");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/L_offset_but_one.txt");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

bool test_different_lifespans() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/S_external_different_lifespan.txt");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_FullStreamGraph_destroy(stream2);

	return result;
}

int main() {
	Test* tests[] = {
	    TEST(test_equals),
	    TEST(test_isomorphism_chunk),
	    TEST(test_not_isomorphic),
	    TEST(test_not_isomorphic_stretched),
	    TEST(test_isomorphism_offset),
	    TEST(test_isomorphic_ids_scrambled),
	    TEST(test_isomorphism_different_node_times),
	    TEST(test_isomorphism_not_constant_offset),
	    TEST(test_different_lifespans),
	    NULL,
	};

	return test("Isomorphism", tests);
}