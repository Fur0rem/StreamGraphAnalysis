#ifndef STREAM_H
#define STREAM_H

typedef struct {
	enum {
		FULL_STREAM_GRAPH,
		LINK_STREAM,
	} type;
	void* stream;
} Stream;

#endif // STREAM_H