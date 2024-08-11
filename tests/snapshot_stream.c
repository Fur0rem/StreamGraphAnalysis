#include "../src/stream/snapshot_stream.h"
#include "../src/interval.h"
#include "../src/metrics.h"
#include "../src/stream.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/units.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_split() {
	StreamGraph S_L = StreamGraph_from_external("data/S_concat_L.txt");
	Stream split_S = SnapshotStream_from(&S_L, Interval_from(0, 100));
	Stream split_L = SnapshotStream_from(&S_L, Interval_from(100, 200));
	StreamGraph S = StreamGraph_from_external("data/S_external.txt");
	StreamGraph L = StreamGraph_from_external("data/L_10.txt");
	Stream full_S = FullStreamGraph_from(&S);
	Stream full_L = FullStreamGraph_from(&L);

	bool S_equals_split_S = EXPECT(Stream_equals(&split_S, &full_S));
	bool L_equals_split_L = EXPECT(Stream_equals(&split_L, &full_L));

	String strS = Stream_to_string(&split_S);
	String strL = Stream_to_string(&split_L);
	printf("Split S : %s\n", strS.data);
	printf("Split L : %s\n", strL.data);
	String_destroy(strS);
	String_destroy(strL);

	return S_equals_split_S && L_equals_split_L;
}

int main() {
	Test* tests[] = {
		&(Test){.fn = test_split, .name = "test_split"},
		NULL,
	};

	return test("SnapshotStream", tests);
}