===================================================================================
stream_graph_analysis : A C library for processing and analysing undirected unweighted stream graphs
===================================================================================

Purpose of the library
----------------------
The library is designed to process and analyse undirected unweighted stream graphs.
It is optimised for reading and analysing huge stream graphs, with a balance between memory usage and speed.

What it can do
--------------
- Read a stream graph from a file

What it cannot do
-----------------
- Process directed and weighted stream graphs : the library was made for the team Complex Networks at the LIP6, which is mainly interested in undirected unweighted stream graphs.
- Modify the stream graph : the library was made to analyse huge stream graphs, so the data structured is optimised for reading and analysing, not for modifying.