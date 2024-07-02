#include "../src/metrics.h"
#include "../src/stream.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream/link_stream.h"
#include "benchmark.h"

Stream stream;

DONT_OPTIMISE void number_of_links() {
	Stream_number_of_links(&stream);
	reset_cache(&stream);
}

int main() {

	// read the file benchmarks/data/LS.txt with seek
	size_t file_size = 0;
	FILE* file = fopen("benchmarks/data/LS_90.ls", "r");
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = (char*)malloc(file_size + 1);
	fread(buffer, 1, file_size, file);
	fclose(file);
	buffer[file_size] = '\0';
	// write it to a file

	char* internal_format = InternalFormat_from_External_str(buffer);
	FILE* file2 = fopen("benchmarks/data/LS_90_internal.txt", "w");
	fwrite(internal_format, 1, strlen(internal_format), file2);
	fclose(file2);
	free(internal_format);

	StreamGraph sg = StreamGraph_from_external("benchmarks/data/LS.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links small", 10000);
	StreamGraph_destroy(sg);
	sg = StreamGraph_from_external("benchmarks/data/LS_90.ls");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links big", 10000);

	return 0;
}