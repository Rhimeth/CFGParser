#include "graph_generator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <clang/AST/Stmt.h>
#include <nlohmann/json.hpp>
#include <clang/AST/ASTContext.h>

using json = nlohmann::json;

namespace GraphGenerator {

std::string getStmtString(const clang::Stmt* S) {
    if (!S) return "NULL";
    
    std::string stmtStr;
    llvm::raw_string_ostream stream(stmtStr);
    S->printPretty(stream, nullptr, clang::PrintingPolicy(clang::LangOptions()));
    stream.flush();
    return stmtStr;
}

std::unique_ptr<CFGGraph> generateCustomCFG(const clang::FunctionDecl* FD) {
    if (!FD || !FD->hasBody()) {
        return nullptr;
    }

    // Build Clang's standard CFG
    std::unique_ptr<clang::CFG> cfg = clang::CFG::buildCFG(
        FD, 
        FD->getBody(), 
        &FD->getASTContext(), 
        clang::CFG::BuildOptions()
    );

    if (!cfg) {
        return nullptr;
    }

    // Create our custom CFG graph
    auto customGraph = std::make_unique<CFGGraph>();

    // Iterate through CFG blocks
    for (const auto& block : *cfg) {
        int blockID = block->getBlockID();
        
        // Create a node for this block
        customGraph->addNode(blockID);

        // Extract and add statements from the block
        for (const auto& element : *block) {
            if (element.getKind() == clang::CFGElement::Statement) {
                const clang::Stmt* stmt = element.getAs<clang::CFGStmt>()->getStmt();
                
                // Convert statement to string using the helper function
                std::string stmtStr;
                llvm::raw_string_ostream stream(stmtStr);
                stmt->printPretty(stream, nullptr, 
                    clang::PrintingPolicy(FD->getASTContext().getLangOpts()));
                stream.flush();
                
                customGraph->addStatement(blockID, stmtStr);
            }
        }

        // Add successors
        for (const auto& successor : block->succs()) {
            if (successor) {
                customGraph->addEdge(blockID, successor->getBlockID());
            }
        }

        // Check for try blocks
        for (const auto& element : *block) {
            if (element.getKind() == clang::CFGElement::Statement) {
                const clang::Stmt* stmt = element.getAs<clang::CFGStmt>()->getStmt();
                if (llvm::isa<clang::CXXTryStmt>(stmt)) {
                    customGraph->markNodeAsTryBlock(blockID);
                    break;
                }
            }
        }

        // Check if block might throw an exception 
        for (const auto& element : *block) {
            if (element.getKind() == clang::CFGElement::Statement) {
                const clang::Stmt* stmt = element.getAs<clang::CFGStmt>()->getStmt();
                if (llvm::isa<clang::CXXThrowExpr>(stmt)) {
                    customGraph->markNodeAsThrowingException(blockID);
                    break;
                }
            }
        }
    }

    return customGraph;
}

std::unique_ptr<CFGGraph> generateCFG(const clang::FunctionDecl* FD) {
    if (!FD || !FD->hasBody()) {
        return nullptr;
    }
    
    //If template function that hasn't been instantiated
    if (FD->getTemplatedKind() == clang::FunctionDecl::TK_FunctionTemplate) {
        llvm::errs() << "Skipping uninstantiated template function: " 
                   << FD->getNameAsString() << "\n";
        return nullptr;
    }
    
    // For template specializations, make sure we have the right function body
    if (FD->isTemplateInstantiation()) {
        // Get the pattern from which this was instantiated
        const clang::FunctionDecl* Pattern = FD->getTemplateInstantiationPattern();
        if (Pattern && Pattern->hasBody()) {
            // We'll use the pattern's body but with the instantiated function's context
            FD = Pattern;
        }
    }
    
    // Create our custom graph
    auto graph = std::make_unique<CFGGraph>();
    
    // Build CFG from Clang
    std::unique_ptr<clang::CFG> cfg = clang::CFG::buildCFG(
        FD, FD->getBody(), &FD->getASTContext(), clang::CFG::BuildOptions());
    
    if (!cfg) {
        llvm::errs() << "Failed to build CFG for function: " 
                   << FD->getNameAsString() << "\n";
        return nullptr;
    }
    
    // Map block statements to their containing CFG blocks for later lookup
    std::map<const clang::Stmt*, clang::CFGBlock*> stmtToBlock;
    for (auto it = cfg->begin(); it != cfg->end(); ++it) {
        clang::CFGBlock* block = *it;
        for (const auto& element : *block) {
            if (element.getKind() == clang::CFGElement::Statement) {
                const clang::Stmt* stmt = element.castAs<clang::CFGStmt>().getStmt();
                stmtToBlock[stmt] = block;
            }
        }
    }
    
    // Map Clang CFG to our graph representation
    for (auto it = cfg->begin(); it != cfg->end(); ++it) {
        clang::CFGBlock* block = *it;
        int blockID = block->getBlockID();
        
        // Create node
        graph->addNode(blockID);
        
        // Add statements and check for special statement types
        for (const auto& element : *block) {
            if (element.getKind() == clang::CFGElement::Statement) {
                const clang::Stmt* stmt = element.castAs<clang::CFGStmt>().getStmt();
                std::string stmtStr;
                llvm::raw_string_ostream stream(stmtStr);
                stmt->printPretty(stream, nullptr, clang::PrintingPolicy(FD->getASTContext().getLangOpts()));
                graph->addStatement(blockID, stream.str());
                
                // Check for try statements
                if (const auto* tryStmt = llvm::dyn_cast<clang::CXXTryStmt>(stmt)) {
                    graph->markNodeAsTryBlock(blockID);
                    
                    // Find blocks for catch handlers
                    for (unsigned i = 0; i < tryStmt->getNumHandlers(); ++i) {
                        const clang::CXXCatchStmt* catchStmt = tryStmt->getHandler(i);
                        auto catchIt = stmtToBlock.find(catchStmt);
                        if (catchIt != stmtToBlock.end()) {
                            graph->addExceptionEdge(blockID, catchIt->second->getBlockID());
                        }
                    }
                }
                
                // Check for throw expressions
                if (llvm::isa<clang::CXXThrowExpr>(stmt)) {
                    graph->markNodeAsThrowingException(blockID);
                }
            }
        }
        
        // Add normal successors
        for (auto succ = block->succ_begin(); succ != block->succ_end(); ++succ) {
            if (*succ)
                graph->addEdge(blockID, (*succ)->getBlockID());
        }
        
        // Add exception successors
        if (graph->isNodeTryBlock(blockID) || graph->isNodeThrowingException(blockID)) {
            for (auto succ = block->succ_begin(); succ != block->succ_end(); ++succ) {
                if (*succ) {
                    bool mightBeExceptionHandler = false;
                    
                    //Check if the successor has statements that look like catch blocks
                    for (const auto& element : **succ) {
                        if (element.getKind() == clang::CFGElement::Statement) {
                            const clang::Stmt* stmt = element.castAs<clang::CFGStmt>().getStmt();
                            if (llvm::isa<clang::CXXCatchStmt>(stmt)) {
                                mightBeExceptionHandler = true;
                                break;
                            }
                        }
                    }
                    
                    if (mightBeExceptionHandler) {
                        graph->addExceptionEdge(blockID, (*succ)->getBlockID());
                    }
                }
            }
        }
    }
    
    return graph;
}

}