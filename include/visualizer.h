#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include "graph_generator.h"

namespace Visualizer {
<<<<<<< HEAD

std::string generateDotRepresentation(
    const GraphGenerator::CFGGraph* graph,
    bool showLineNumbers = true,
    bool simplifyGraph = false,
    const std::vector<int>& highlightPaths = {}
);

bool exportToDot(
    const GraphGenerator::CFGGraph* graph,
    const std::string& filename,
    bool showLineNumbers = true,
    bool simplifyGraph = false,
    const std::vector<int>& highlightPaths = {}
);

enum class ExportFormat {
    DOT,
    PNG,
    SVG,
    PDF
};

bool exportGraph(
    const GraphGenerator::CFGGraph* graph,
    const std::string& filename,
    ExportFormat format = ExportFormat::DOT,
    bool showLineNumbers = true,
    bool simplifyGraph = false,
    const std::vector<int>& highlightPaths = {}
);

} // namespace Visualizer
=======
    // Function to generate DOT representation of the graph
    std::string generateDotRepresentation(const GraphGenerator::CFGGraph* graph);

    // Function to export graph to a DOT file
    bool exportToDot(const GraphGenerator::CFGGraph* graph, const std::string& filename);
}
>>>>>>> e4e3a1ee3e7575d1f091a453a24f18f29459330b

#endif // VISUALIZER_H