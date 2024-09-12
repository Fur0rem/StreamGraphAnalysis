#include "../StreamGraphAnalysis.h"
#include "test.h"

bool test_equals() {
	StreamGraph sg1 = StreamGraph_from_file("data/S.txt");
	Stream stream1	= FullStreamGraph_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_file("data/S.txt");
	Stream stream2	= FullStreamGraph_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(stream1);
	FullStreamGraph_destroy(stream2);

	return result;
}

bool test_isomorphism_chunk() {
	StreamGraph sg			= StreamGraph_from_external("data/kcores_with_L.txt");
	StreamGraph kcores_only = StreamGraph_from_external("data/kcores_test.txt");

	LinkIdVector links = LinkIdVector_new();
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		LinkIdVector_push(&links, i);
		printf("Link %zu (%zu, %zu)\n", i, sg.links.links[i].nodes[0], sg.links.links[i].nodes[1]);
	}

	NodeIdVector nodes = NodeIdVector_new();
	NodeIdVector_push(&nodes, 0);
	NodeIdVector_push(&nodes, 2);
	NodeIdVector_push(&nodes, 4);
	NodeIdVector_push(&nodes, 6);

	Interval snapshot = Interval_from(0, 10);
	Stream st		  = CS_with(&sg, &nodes, &links, snapshot.start, snapshot.end);
	Stream og		  = LinkStream_from(&kcores_only);

	bool result = EXPECT(are_isomorphic(&st, &og));

	StreamGraph_destroy(sg);
	StreamGraph_destroy(kcores_only);
	LinkStream_destroy(og);
	CS_destroy(st);

	return result;

	// return false;
}

bool test_not_isomorphic() {
	StreamGraph sg1 = StreamGraph_from_file("data/S.txt");
	Stream stream1	= FullStreamGraph_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/S_concat_L.txt");
	Stream stream2	= FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(stream1);
	FullStreamGraph_destroy(stream2);

	return result;
}

bool test_not_isomorphic_stretched() {
	// FIXME: this crashes with LinkStreams
	StreamGraph sg1 = StreamGraph_from_external("data/L.txt");
	Stream stream1	= LinkStream_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/L_10.txt");
	Stream stream2	= LinkStream_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	LinkStream_destroy(stream1);
	LinkStream_destroy(stream2);

	return result;
}

bool test_isomorphism_offset() {
	StreamGraph sg1 = StreamGraph_from_external("data/L.txt");
	Stream stream1	= LinkStream_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/L_offset.txt");
	Stream stream2	= LinkStream_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	LinkStream_destroy(stream1);
	LinkStream_destroy(stream2);

	return result;
}

bool test_isomorphic_ids_scrambled() {
	StreamGraph sg1 = StreamGraph_from_external("data/L.txt");
	Stream stream1	= LinkStream_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/L_scrambled.txt");
	Stream stream2	= LinkStream_from(&sg2);

	bool result = EXPECT(are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	LinkStream_destroy(stream1);
	LinkStream_destroy(stream2);

	return result;
}

bool test_isomorphism_different_node_times() {
	StreamGraph sg1 = StreamGraph_from_external("data/S_external.txt");
	Stream stream1	= FullStreamGraph_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/S_external_different_node_time.txt");
	Stream stream2	= FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(stream1);
	FullStreamGraph_destroy(stream2);

	return result;
}

bool test_isomorphism_not_constant_offset() {
	StreamGraph sg1 = StreamGraph_from_external("data/L.txt");
	Stream stream1	= LinkStream_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/L_offset_but_one.txt");
	Stream stream2	= LinkStream_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	LinkStream_destroy(stream1);
	LinkStream_destroy(stream2);

	return result;
}

bool test_different_lifespans() {
	StreamGraph sg1 = StreamGraph_from_external("data/S_external.txt");
	Stream stream1	= FullStreamGraph_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/S_external_different_lifespan.txt");
	Stream stream2	= FullStreamGraph_from(&sg2);

	bool result = EXPECT(!are_isomorphic(&stream1, &stream2));

	StreamGraph_destroy(sg1);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(stream1);
	FullStreamGraph_destroy(stream2);

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