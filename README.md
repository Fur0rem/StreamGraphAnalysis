===================================================================================
StreamGraphAnalysis : A C library for processing and analysing undirected unweighted stream graphs
===================================================================================

Current state of the Project
----------------------------
This is only a prototype, so the library is not usable at all for now. I'm still experimenting with how I approach the problem for now.

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

Project guidelines and organisation
--------------------
- Guidelines :
None for now, I'm focusing on having something that works first, rather than something that is perfect but with no results.
Things I'm considering for guidelines are namespacing to work with other libraries.
Maybe I could make it standardised like ISO C, or C99, but I'm doing weird macro magic for the genericity specified above, so I'm not sure how to do that.

- Organisation :

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
- Uniformity of a pair of nodes : Section 4
- Density : Section 5
- Density of link : Section 5
- Density of node : Section 5
- Density of time : Section 5
- Degree of a node : Section 8
- Average node degree : Section 8
- Clustering coefficient of a node : Section 9
- Node clustering coefficient : Section 9
- Robustness by length : Outside of the paper

(These were implemented in previous commits but have not been ported for generic streams yet)
- Compactness : Section 4
