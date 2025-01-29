#undef SGA_INTERNAL // TODO just don't define it
#include "../StreamGraphAnalysis.h"
#include <stdlib.h>

int main() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/S_external_multiple.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	SGA_TimesIterator all_key_moments = SGA_Stream_key_moments(&st);
	SGA_FOR_EACH_TIME(moment, all_key_moments) {
		printf("Induced subgraph at [%zu, %zu[:\n", moment.start, moment.end);
		SGA_NodesIterator nodes_present = SGA_Stream_nodes_present_at_t(&st, moment.start);
		printf("\tNodes: ");
		SGA_FOR_EACH_NODE(node_id, nodes_present) {
			printf("%zu, ", node_id);
		}
		printf("\n");
		SGA_LinksIterator links_present = SGA_Stream_links_present_at_t(&st, moment.start);
		printf("\tLinks: ");
		SGA_FOR_EACH_LINK(link_id, links_present) {
			SGA_Link link = SGA_Stream_link_by_id(&st, link_id);
			printf("(%zu - %zu), ", link.nodes[0], link.nodes[1]);
		}
		printf("\n");
	}

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}