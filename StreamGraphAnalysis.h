/**
 * @file StreamGraphAnalysis.h
 * @brief The main header file of the library.
 */

/**
 * @defgroup EXTERNAL_API External API
 * @defgroup INTERNAL_API Internal API
 */

/**
 * @mainpage StreamGraphAnalysis
 *
 * @section intro_sec Introduction
 *
 * Welcome to the documentation of the project! I highly advise you to read the README.md file before this one for context on the library.
 *
 * @section external_api_section What you can use
 *
 * Here is everything that you can do using this library!
 * @ref EXTERNAL_API "External API".
 *
 * @section internal_api_section Internal API
 *
 * This section is dedicated to the documentation of the inner workings of the library, you can check it to understand how it works behind
 the scenes but you are not meant to directly interact with these.
 * @ref INTERNAL_API "Internal API".
 */

#include "src/analysis/cliques.h"
#include "src/analysis/isomorphism.h"
#include "src/analysis/kcores.h"
#include "src/analysis/line_stream.h"
#include "src/analysis/metrics.h"
#include "src/analysis/walks.h"
#include "src/iterators.h"
#include "src/stream.h"
#include "src/stream_data_access/induced_graph.h"
#include "src/stream_data_access/key_instants.h"
#include "src/stream_data_access/link_access.h"
#include "src/stream_data_access/node_access.h"
#include "src/stream_functions.h"
#include "src/streams.h"
#include "src/utils.h"