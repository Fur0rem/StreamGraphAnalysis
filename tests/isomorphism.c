/**
 * @file tests/isomorphism.c
 * @brief Tests regarding isomorphism
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"

/**
 * @brief Test if two identical streams are isomorphic.
 */
bool test_equals() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_FullStreamGraph_destroy(stream2);

	return result;
}

/**
 * @brief Test if an extracted chunk is isomorphic to the original stream.
 */
bool test_isomorphism_chunk() {
	SGA_StreamGraph sg	    = SGA_StreamGraph_from_file("data/tests/kcores_fused_with_L.sga");
	SGA_StreamGraph kcores_only = SGA_StreamGraph_from_file("data/tests/kcores.sga");

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

	SGA_Interval timeframe = SGA_Interval_from(0, 10);
	// SGA_Stream st	  = CS_with(&sg, &nodes, &links, timeframe.start, timeframe.end);
	SGA_Stream st = SGA_ChunkStream_with(&sg, &nodes, &links, timeframe);
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

/**
 * @brief Test if two different streams are not isomorphic.
 */
bool test_not_isomorphic() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/S_concat_L.sga");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_FullStreamGraph_destroy(stream2);

	return result;
}

/**
 * @brief Test if two streams are not isomorphic when one is a time-stretched version of the other.
 */
bool test_not_isomorphic_stretched() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/L.sga");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/L_stretched_by_10.sga");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

/**
 * @brief Test if two identical streams are still isomorphic when one has a time offset.
 */
bool test_isomorphism_offset() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/L.sga");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/L_offset.sga");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

/**
 * @brief Test if two identical streams are still isomorphic when the node IDs are scrambled.
 */
bool test_isomorphic_ids_scrambled() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/L.sga");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/L_scrambled.sga");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

/**
 * @brief Test if two identical streams are not isomorphic when one has different node times.
 */
bool test_isomorphism_different_node_times() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/S_one_different_appearance.sga");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_FullStreamGraph_destroy(stream2);

	return result;
}

/**
 * @brief Test if two streams are not isomorphic when one has a non-constant offset.
 */
bool test_isomorphism_not_constant_offset() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/L.sga");
	SGA_Stream stream1  = SGA_LinkStream_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/L_offset_but_one.sga");
	SGA_Stream stream2  = SGA_LinkStream_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	SGA_StreamGraph_destroy(sg1);
	SGA_StreamGraph_destroy(sg2);
	SGA_LinkStream_destroy(stream1);
	SGA_LinkStream_destroy(stream2);

	return result;
}

/**
 * @brief Test if two identical streams are not isomorphic when they have different lifespans, but the same events.
 */
bool test_different_lifespans() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/S_different_lifespan.sga");
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