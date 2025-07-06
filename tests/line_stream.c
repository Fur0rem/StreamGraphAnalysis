/**
 * @file tests/line_stream.c
 * @brief Tests regarding line streams
 */

#include "../StreamGraphAnalysis.h"
#include "test.h"

bool test_line_stream() {
	SGA_StreamGraph _original = SGA_StreamGraph_from_file("data/tests/line_stream/original.sga");
	SGA_Stream original	  = SGA_LinkStream_from(&_original);

	SGA_StreamGraph _line_stream = SGA_line_stream_of(&original);
	SGA_Stream line_stream	     = SGA_FullStreamGraph_from(&_line_stream);

	SGA_StreamGraph _expected = SGA_StreamGraph_from_file("data/tests/line_stream/expected_line_stream.sga");
	SGA_Stream expected	  = SGA_FullStreamGraph_from(&_expected);

	bool result = EXPECT(SGA_Stream_are_isomorphic(&line_stream, &expected));

	// Clean up
	SGA_Stream_destroy(original);
	SGA_Stream_destroy(line_stream);
	SGA_Stream_destroy(expected);
	SGA_StreamGraph_destroy(_original);
	SGA_StreamGraph_destroy(_line_stream);
	SGA_StreamGraph_destroy(_expected);

	return result;
}

int main() {
	Test* tests[] = {
	    TEST(test_line_stream),
	    NULL,
	};

	return test("Line stream", tests);
}