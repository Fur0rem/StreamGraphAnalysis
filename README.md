===================================================================================
StreamGraphAnalysis : A C library for processing and analysing undirected unweighted stream graphs
===================================================================================

Current state of the Project
----------------------------
Quite usable, but not fully fleshed out yet.

How to use the library
----------------------

Clone the repository, and then run make libSGA to compile the library.
You will have a libSGA.a file in the bin/ directory, which you can link to your project to use the library.
There is a header StreamGraphAnalysis.h in the main directory that you can include in your project to use the library.
You can find examples of how to use the library in the examples/ directory, how to format your files in the data/ directory and the documentation of the library in the doc/ directory.

Data format
-----------

You have two ways to format your data, either in the external format or in the internal format.
The external format is easier to write, but the internal format is more efficient to read.
So it's recommended to write your data in the external format, and then convert it to the internal format using the InternalFormat_from_External_str function, and save that for future use.

External format :
```
SGA External Format <version>

[General]
Lifespan=(<start>, <end>)
Scale=<scale>

[Events]
<Timestamp> <+ or -> <N (for node) or L (for link)> (<NodeId> for a node or <Node1> <Node2> for a link)
...

[EndOfStream]
Any text before this line will be ignored, and can be used for comments or metadata
```

All timestamps must be integers, which is why the scale is used to bring metrics back to the original scale.
For example, if you have a stream with nodes present at 0.5, 1.5, ect...
You can multiply all your timestamps by 2, and set the scale to 2, to have all your timestamps as integers, but have the metrics be correct according to the original scale.

Purpose of the library
----------------------
The library is designed to process and analyse continuous undirected unweighted stream graphs for the team Complex Networks at the LIP6.
It it based on a paper by Matthieu Latapy, Tiphaine Viard and Clémence Magnien, which is available at https://arxiv.org/abs/1710.04073.
It is optimised for reading and analysing huge stream graphs, with a balance between memory usage and speed.
The library also supports the computation of metrics on subgraphs of the stream graph without copying much data, but at some cost I will explain in the code architecture section.

Code architecture
-----------------
- The library was made for only reading stream graphs, and therefore the data structure doesn't allow for modifications, but at the benefit of enabling a lot of optimisations, and concurrent access to the stream graph.
- The data structure was made to have 3 very efficient primitives : Access by node, access by link, and access by time. All of these have a complexity of O(1) (The complexity of the access by time is O(log(255)), but it is then equivalent by complexity classes to O(1)). However it does involve a bit of information duplication, which is why the library is optimised for reading only.
- There is also a constraint of being able to compute metrics on subgraphs of the stream graph without copying much data. This is done through having an underlying stream graph, lazy iterators to access the data that the subgraph can alter to its needs, caching already-computed base elements, and double-layered dynamic dispatch to compute the base elements needed for each type of subgraph based on its iterators, with different types of subgraphs being able to hijack the computation of other functions, for example a LinkStream can hijack the computation of the coverage which is always 1.

What it can do
--------------
- Read a stream graph from a file
- Compute different metrics on the stream graph, the list of which is available below
- Support concurrent access to the stream graph, since it is read-only

What it cannot do
-----------------
- Modify the stream graph : the library was made to analyse huge stream graphs, so the data structured is optimised for reading only.

Specifications of the stream graph data
---------------------------------------
- The stream graph is continuous, undirected and unweighted
- The stream graph cannot have instantaneous links, i.e. a link must last at least one time unit
- The stream graph uses closed on the left intervals, i.e. a link (u, v) is active at time t if and only if t is in [t_start, t_end[
- If a link is present at time t, the nodes u and v must be present at time t

Stream wrappers
---------------

The library supports analysing substreams of the stream graph, which are called (for now) stream wrappers.
You need to wrap your stream graph in one of these to compute metrics on it.

The stream wrappers are :
- FullStreamGraph : The entire stream graph, no modifications
- LinkStream : All nodes are present at all times, and the links are the only thing that changes
- SnapshotStream : Study the stream graph only on a subset of the interval it is defined on, doesn't change nodes or links present
- ChunkStream and ChunkStreamSmall : Study the stream graph only on a subset of the interval, subset of the nodes, and subset of the links. Note that deleting a node will cause the deletion of all its links. ChunkStream is optimised for when you have a lot of nodes and links, and ChunkStreamSmall is optimised for more sparse chunks with a small portion of the nodes and links.

I plan to add DeltaStreamGraph, to support delta-analysis of the stream graph, and maybe a few others.


Project guidelines and organisation
--------------------
- Guidelines :
None for now, I'm focusing on having something that works first, rather than something that is perfect but with no results.
Things I'm considering for guidelines are namespacing to work with other libraries.
Maybe I could make it standardised like ISO C, or C99, but I'm doing weird macro magic for the genericity specified above, so I'm not sure how to do that.

- Organisation :

The src/ directory contains the entire source code of the library.
The tests/ directory contains the tests for the library, which are written using a home-made very basic testing framework.
One test exists per source file, and each test must have the same name as the source file it tests.
You can run them using the run_tests.sh script in the main directory.

Supported metrics and analysis (In order of first mention in the paper)
-----------------------------------------------------------

- Coverage : Section 3
- Contribution of a node : Section 4
- Number of nodes : Section 4
- Contribution of a link : Section 4
- Number of links : Section 4
- Node duration : Section 4
- Link duration : Section 4
- Uniformity : Section 4
- Uniformity of a pair of nodes : Section 4
- Density : Section 5
- Density of link : Section 5
- Density of node : Section 5
- Density of time : Section 5
- Maximal cliques enumeration : Section 7
- Degree of a node : Section 8
- Average node degree : Section 8
- Clustering coefficient of a node : Section 9
- Node clustering coefficient : Section 9
- K-cores : Section 13
- Paths and distances : Section 14
- Robustness by length : Outside of the paper

(These were implemented in previous commits but have not been ported for generic streams yet)
- Compactness : Section 4
