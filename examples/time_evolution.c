#undef SGA_INTERNAL // TODO just don't define it
#include "../StreamGraphAnalysis.h"
#include <stdlib.h>

int main() {
	// SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/S_external.txt");
	// SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	char* content	   = read_file("../data/S_external_multiple.txt");
	char* to_internal  = SGA_InternalFormat_from_External_str(content);
	SGA_StreamGraph sg = SGA_StreamGraph_from_string(to_internal);
	// SGA_init_events_table(&sg);

	SGA_Stream st = SGA_FullStreamGraph_from(&sg);

	TimesIterator all_key_moments = SGA_Stream_key_moments(&st);
	FOR_EACH_TIME(moment, all_key_moments) {
		printf("Induced subgraph at [%zu, %zu[:\n", moment.start, moment.end);
		NodesIterator nodes_present = SGA_Stream_nodes_present_at_t(&st, moment.start);
		printf("\tNodes: ");
		FOR_EACH_NODE(node_id, nodes_present) {
			printf("%zu, ", node_id);
		}
		printf("\n");
		LinksIterator links_present = SGA_Stream_links_present_at_t(&st, moment.start);
		printf("\tLinks: ");
		FOR_EACH_LINK(link_id, links_present) {
			Link link = SGA_Stream_link_by_id(&st, link_id);
			printf("(%zu - %zu), ", link.nodes[0], link.nodes[1]);
		}
		printf("\n");
	}

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}