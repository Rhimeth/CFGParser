#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include "graph_generator.h"

namespace Visualizer {
    // Function to generate DOT representation of the graph
    std::string generateDotRepresentation(const GraphGenerator::CFGGraph* graph);

    // Function to export graph to a DOT file
    bool exportToDot(const GraphGenerator::CFGGraph* graph, const std::string& filename);
}

#endif // VISUALIZER_H