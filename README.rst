===================================================================================
StreamGraphAnalysis : A C library for processing and analysing undirected unweighted stream graphs
===================================================================================

Purpose of the library
----------------------
The library is designed to process and analyse continuous undirected unweighted stream graphs for the team Complex Networks at the LIP6.
It it based on a paper by Matthieu Latapy, Tiphaine Viard and Clémence Magnien, which is available at https://arxiv.org/abs/1710.04073.
It is optimised for reading and analysing huge stream graphs, with a balance between memory usage and speed.

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
- If a node is specified in the stream graph, it must be present at least once in the lifespan of the stream graph, same for links
- If a link is present at time t, the nodes u and v must be present at time t

Data structure
--------------
The data structure used was designed to allow fast reading by node, by link, or by time.

Project guidelines and architecture
--------------------
- Guidelines :
All functions, structures and types are prefixed with "SGA" to avoid conflicts with other libraries, since C does not have namespaces, so you can use other graph libraries in the same project.

- Architecture :

The src/ directory contains the entire source code of the library.
The source code is divided into 3 parts :
  - stream_graph, which contains the base data structure and functions to read a stream graph from a file
  - metrics, which contains the functions to compute the metrics
  - utils, which contains utility functions used by the other parts, such as generic dynamic arrays and hash tables

The tests/ directory contains the tests for the library, which are written using a home-made very basic testing framework.
One test exists per source file, and each test must have the same name as the source file it tests.
You can run them using the run_tests.sh script in the main directory.

Supported metrics (In order of first mention in the paper)
-----------------------------------------------------------

- Coverage : Section 3
- Contribution of a node : Section 4
- Number of nodes : Section 4
- Contribution of a link : Section 4
- Number of links : Section 4
- Node duration : Section 4
- Link duration : Section 4
- Uniformity : Section 4
- Compactness : Section 4
- Density : Section 5
- Degree of a node : Section 8
- Average node degree : Section 8
