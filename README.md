# StreamGraphAnalysis : A C library for processing and analysing undirected unweighted stream graphs

## Current state of the Project

Quite usable, but not fully fleshed out yet.

If you do end up using it, please let me know, I would be happy to hear your feedback and suggestions. Same for bug reports, issues, or any other kind of feedback.
I'd be happy if you linked to the project in your paper or project if you use it.

Also if you can send me more data to test or benchmark the library with, that would be great, so I can have a better idea of how it performs on real data and optimise it better.

## Before you start

The library is based on a paper by Matthieu Latapy, Tiphaine Viard and Clémence Magnien, which is available at <https://arxiv.org/abs/1710.04073>.

It is designed to process and analyse continuous undirected unweighted stream graphs for the team Complex Networks at the LIP6.
It is made to be efficient in terms of memory usage and speed, and to support the computation of metrics different transformations of the stream graph without copying much data.

It is highly recommended to read the paper before using the library, or at least have a good enough understanding of what a stream graph is and what the metrics can represent and tell you.

## Regarding the paper, definitions and namings

I tried to follow the paper namings as closely as possible, but I had to make some decisions for the implementation, so here are the differences :

- The paper defines the stream graph as a tuple of 4 sets (W : Temporal nodes, V : distinct nodes, E : links, T : time), but in the implementation, I only use 3 sets (W, E, T), and renamed to their more explicit names (Nodes, Links, Times).
- For their cardinals, which are often used in the paper, I renamed them to |W| : temporal_cardinal_of_node_set, |V| : distinct_cardinal_of_node_set, |E| : temporal_cardinal_of_link_set, |T| : duration (for the stream, not for a node or a link).
- For the rest, most of the same names are used, and I put the corresponding notation in the documentation of the functions.
- The documentation is split by sections of the paper, in the same order as the first mention of the metrics in the paper, to make it easier to find the corresponding function for a metric.
- I define a key instant as a time where a node or a link appears or disappears, i.e. a time where the stream graph changes.
- I define an event as a change in the stream graph, i.e. appearance or disappearance of a node or a link. Though I don't think that you will ever need to work with events directly.

## Basic information about the implementation

The library is written in C, and is designed to be very generic, to be able to support different types of stream graphs, and different types of metrics.
Anything related to the library (data types, functions, ect...) contain the prefix SGA_ to avoid conflicts with other libraries.
There are 2 main data structures in the library :

- StreamGraph, which is the main data structure, and contains the entire stream graph.
- Stream, which is a wrapper around the StreamGraph, and allows for different transformations of the stream graph. For example, only considering a subset of the stream graph, or considering it as a link stream.

You need to first load the StreamGraph, and then wrap it in a Stream to compute metrics on it.
The stream contains a reference to the StreamGraph, so be careful to not delete the StreamGraph before the Stream.

Stream's are optimised to be very lightweight, to not copy much data, and will just transform the data on the fly when it is queried.
For example, a LinkStream will only have a reference to the StreamGraph, and return the full lifespan of the stream graph when asked for the lifespan of node.
All the metrics are made to work with streams, but some types of streams can optimise the computation of some metrics, for example, a LinkStream can optimise the computation of the coverage, which is always 1.

The different streams are :

- FullStreamGraph : The entire stream graph, no modifications
- LinkStream : All nodes are present at all times, and the links are the only thing that changes
- TimeFrameStream : Study the stream graph only on a subset of the interval it is defined on, doesn't change nodes or links present
- ChunkStream and ChunkStreamSmall : Study the stream graph only on a subset of the interval, subset of the nodes, and subset of the links. Note that deleting a node will cause the deletion of all its links. ChunkStream is optimised for when you have a lot of nodes and links, and ChunkStreamSmall is optimised for more sparse chunks with a small portion of the nodes and links.

I plan to add DeltaStreamGraph, to support delta-analysis of the stream graph, and maybe a few others.

The library was designed to have efficient access on:

- Access a node: which is in O(1)
- Access a link: which is in O(1)
- Access a time: which is in O(1) (actually in O(log(255)), but it's a constant time)

More details about the methods and the data structures are available in the documentation.

## How to use the library

Clone the repository, and then run make libSGA to compile the library.
You will have a libSGA.a file in the bin/ directory, which you can link to your project to use the library.
There is a header StreamGraphAnalysis.h in the main directory that you can include in your project to use the library.

You can find examples of how to use the library in the examples/ directory.
You can find the documentation in the docs/ directory, which is generated using Doxygen, which you can generate using the make docs -B command.
You can find utility programs in the utilities/ directory, which you can compile using the make utilities -B command.

## Data format

You have two ways to format your data, either in the external format or in the internal format.
The external format is easier to write for you, but the internal format is more efficient to read for the program.
So it's recommended to write your data in the external format, and then convert it to the internal format using the  function, and save that for future use.
You can use the utilities/bin/external_to_internal program to convert your data from the external format to the internal format.

External format :

```txt
SGA External Format <version>

Any text between the version and the [General] section will be ignored, and can be used for comments or metadata

[General]
Lifespan=(<start>, <end>)
TimeScale=<scale>

[Events]
<Timestamp> <+ (for appearance) or - (for disappearance)> <N (for node) or L (for link)> (<NodeId> for a node or <Node1> <Node2> for a link)
...

[EndOfStream]

Any text before this line will be ignored, and can be used for comments or metadata
```

All timestamps must be integers, which is why the scale is used to bring metrics back to the original scale.
For example, if you have a stream with nodes present at 0.5, 1.5, ect...
You can multiply all your timestamps by 2, and set the scale to 2, to have all your timestamps as integers, but have the metrics be correct according to the original scale.

There are some example data files in the data/examples/ directory, which contains examples of the external format taken from the paper.

You can also find a more detailed documentation of each format in the documentation for SGA_StreamGraph_from_internal_format_v_1_0_0 and SGA_StreamGraph_from_external_format_v_1_0_0.

## What it can do

- Read a stream graph from a file
- Compute different metrics on the stream graph, the list of which is available below
- Support concurrent access to the stream graph, since it is read-only

## What it cannot do

- Modify the stream graph : the library was made to analyse huge stream graphs, so the data structured is optimised for reading only.
- Support directed or weighted stream graphs : the library is made to support undirected unweighted stream graphs only, as defined in the paper. (Maybe in the future, but not for now)

## Specifications of the stream graph data

- The stream graph is continuous, undirected and unweighted
- The stream graph cannot have instantaneous links, i.e. a link must last at least one time unit
- The stream graph uses closed on the left intervals, i.e. a link (u, v) is active at time t if and only if t is in [t_start, t_end[
- If a link is present at time t, the nodes u and v must be present at time t

## Project guidelines and organisation

- Guidelines :
None for now, I'm focusing on having something that works first, rather than something that is perfect but with no results.
Maybe I could make it standardised like ISO C, or C99, but I'm doing weird macro magic for the genericity specified above, so I'm not sure how to do that.

- Organisation :

The `src` directory contains the entire source code of the library.

The `src/stream_graph` directory contains the source code of the StreamGraph data structure.

The `src/streams` directory contains the source code of the different streams.

The `src/analysis` directory contains implementations of the different ways to analyse the stream graph which are not metrics. For example, maximal cliques, walks, k-cores, etc...

The `tests` directory contains the tests for the library, which are written using a home-made very basic testing framework.
You can run them using the run_tests.sh script in the main directory.

The `benchmarks` directory contains the benchmarks for the library.
You can run them using the run_benchmarks.sh script in the main directory.

The `examples` directory contains examples of how to use the library.

The `data` directory contains data files, test data, example data, and benchmark data.

The `docs` directory contains the documentation of the library, which is generated using Doxygen.

The `utilities` directory contains utility programs, which are compiled using the

```bash
make utilities -B
```

command.

## Supported metrics and analysis

- |W|, |V|, |E|, |T| : Section 3
- Coverage : Section 3
- Contribution of a node : Section 4
- Number of nodes : Section 4
- Contribution of a link : Section 4
- Number of links : Section 4
- Node duration : Section 4
- Link duration : Section 4
- Uniformity : Section 4
- Uniformity of a pair of nodes : Section 4
- Compactness : Section 4
- Density : Section 5
- Density of link : Section 5
- Density of node : Section 5
- Density of time : Section 5
- Maximal cliques enumeration : Section 7
- Degree of a node : Section 8
- Average node degree : Section 8
- Clustering coefficient of a node : Section 9
- Node clustering coefficient : Section 9
- Line streams: Section 12
- K-cores : Section 13
- Walks and distances : Section 14 (The paper mentionned paths but for now I only implemented walks)
- Delta-analysis : Section 19
- Robustness by length : Outside of the paper
- Isomorphism : Outside of the paper
