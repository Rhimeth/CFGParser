#include "graph_generator.h"
#include <fstream>
#include <sstream>

namespace GraphGenerator {

// Implementation of CFGGraph methods defined in graph_generator.h

void CFGGraph::addStatement(int nodeID, const std::string& stmt) {
    addStatementToNode(nodeID, stmt);
}

void CFGGraph::addExceptionEdge(int sourceID, int targetID) {
    exceptionEdges.insert({sourceID, targetID});
}

bool CFGGraph::isExceptionEdge(int sourceID, int targetID) const {
    return exceptionEdges.count({sourceID, targetID}) > 0;
}

void CFGGraph::markNodeAsTryBlock(int nodeID) {
    tryBlocks.insert(nodeID);
}

void CFGGraph::markNodeAsThrowingException(int nodeID) {
    throwingBlocks.insert(nodeID);
}

bool CFGGraph::isNodeTryBlock(int nodeID) const {
    return tryBlocks.count(nodeID) > 0;
}

bool CFGGraph::isNodeThrowingException(int nodeID) const {
    return throwingBlocks.count(nodeID) > 0;
}

std::string CFGGraph::getNodeLabel(int nodeID) const {
    auto it = nodes.find(nodeID);
    if (it != nodes.end()) {
        return it->second.label.empty() ? 
            "Block " + std::to_string(nodeID) : 
            it->second.label;
    }
    return "Unknown Block";
}

void CFGGraph::writeToDotFile(const std::string& filename) const {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        throw std::runtime_error("Could not open dot file for writing");
    }

    dotFile << "digraph CFG {\n";
    
    // Write nodes
    for (const auto& [nodeID, node] : nodes) {
        dotFile << "    " << nodeID << " [label=\"" 
                << getNodeLabel(nodeID) << "\"];\n";
    }

    // Write edges
    for (const auto& [nodeID, node] : nodes) {
        for (int successorID : node.successors) {
            dotFile << "    " << nodeID << " -> " << successorID 
                    << (isExceptionEdge(nodeID, successorID) ? " [color=red]" : "") 
                    << ";\n";
        }
    }

    dotFile << "}\n";
    dotFile.close();
}

void CFGGraph::writeToJsonFile(const std::string& filename, 
                                const json& astJson, 
                                const json& functionCallJson) {
    json graphJson;
    
    // Add nodes
    for (const auto& [nodeID, node] : nodes) {
        graphJson["nodes"][std::to_string(nodeID)] = {
            {"label", getNodeLabel(nodeID)},
            {"statements", node.statements},
            {"isTryBlock", isNodeTryBlock(nodeID)},
            {"isThrowingException", isNodeThrowingException(nodeID)}
        };
    }

    // Add edges
    for (const auto& [nodeID, node] : nodes) {
        for (int successorID : node.successors) {
            graphJson["edges"].push_back({
                {"source", nodeID},
                {"target", successorID},
                {"isExceptionEdge", isExceptionEdge(nodeID, successorID)}
            });
        }
    }

    // Combine with additional JSON data
    graphJson["ast"] = astJson;
    graphJson["functionCalls"] = functionCallJson;

    std::ofstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        throw std::runtime_error("Could not open JSON file for writing");
    }

    jsonFile << graphJson.dump(4);
    jsonFile.close();
}

} // namespace GraphGenerator