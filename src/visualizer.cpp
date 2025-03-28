#include "visualizer.h"
#include <fstream>
#include <sstream>

namespace Visualizer {

std::string generateDotRepresentation(const GraphGenerator::CFGGraph* graph) {
    std::stringstream dot;
    dot << "digraph CFG {\n";
    dot << "  node [shape=box];\n";
    
    // Add nodes
    for (const auto& [id, node] : graph->getNodes()) {
        dot << "  " << id << " [label=\"" << graph->getNodeLabel(id) << "\"";
        
        // Special styling for try blocks
        if (graph->isNodeTryBlock(id)) {
            dot << ", style=filled, fillcolor=lightblue";
        }
        
        // Special styling for throwing blocks
        if (graph->isNodeThrowingException(id)) {
            dot << ", style=filled, fillcolor=lightcoral";
        }
        
        dot << "];\n";
    }
    
    // Add edges
    for (const auto& [id, node] : graph->getNodes()) {
        for (int succ : node.successors) {
            dot << "  " << id << " -> " << succ;
            
            // Special styling for exception edges
            if (graph->isExceptionEdge(id, succ)) {
                dot << " [color=red, style=dashed, label=\"exception\"]";
            }
            
            dot << ";\n";
        }
    }
    
    dot << "}\n";
    return dot.str();
}

bool exportToDot(const GraphGenerator::CFGGraph* graph, const std::string& filename) {
    if (!graph)
        return false;
        
    std::ofstream outFile(filename);
    if (!outFile)
        return false;
        
    outFile << generateDotRepresentation(graph);
    return true;
}

}