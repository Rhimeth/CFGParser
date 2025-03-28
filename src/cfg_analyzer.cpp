#include <iostream>
#include <fstream> 
#include "cfg_analyzer.h"
#include "parser.h"
#include "graph_generator.h"
#include "visualizer.h"
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Forward declarations
void processFile(const std::string& filePath, const std::string& outputDir);
void writeCfgToJsonFile(const clang::CFG* cfg, 
                        const std::string& outputFile,
                        const json& astJson, 
                        const json& functionCallJson);

namespace {
    static llvm::cl::OptionCategory CFGAnalyzerCategory("CFG Analyzer Options");
    
    static llvm::cl::opt<std::string> InputFile(
        "input",
        llvm::cl::desc("Specify input source file"),
        llvm::cl::value_desc("filename"),
        llvm::cl::cat(CFGAnalyzerCategory)
    );

    static llvm::cl::opt<std::string> CFGOutputDir(
        "cfg-output-dir",
        llvm::cl::desc("Output directory for CFG visualizations"),
        llvm::cl::value_desc("directory"),
        llvm::cl::cat(CFGAnalyzerCategory),
        llvm::cl::init("cfg_output")
    );

    static llvm::cl::opt<bool> VerboseOutput(
        "verbose",
        llvm::cl::desc("Enable verbose output"),
        llvm::cl::cat(CFGAnalyzerCategory)
    );
}

namespace CFGAnalyzer {

    bool CFGVisitor::VisitFunctionDecl(clang::FunctionDecl* FD) {
        if (FD && FD->hasBody()) {
            // Get qualified function name
            std::string FuncName = FD->getQualifiedNameAsString();
            CurrentFunction = FuncName;
            
            // Initialize entry in dependency map if it doesn't exist
            if (FunctionDependencies.find(FuncName) == FunctionDependencies.end()) {
                FunctionDependencies[FuncName] = std::set<std::string>();
            }
        }
        clang::SourceManager& SM = Context->getSourceManager();
        if (!SM.isInMainFile(FD->getLocation())) return true;
    
        // Skip function templates without instantiations
        if (FD->getTemplatedKind() == clang::FunctionDecl::TK_FunctionTemplate) {
            llvm::outs() << "Skipping function template definition: " 
                          << FD->getNameAsString() << "\n";
            return true;
        }
    
        llvm::outs() << "Analyzing function: " << FD->getNameAsString() << "\n";
    
        // Create individual file for this function's CFG
        std::string funcFilename = OutputDir + "/" + FD->getNameAsString() + "_cfg.dot";
    
        // Use this approach:
        auto clangGraph = GraphGenerator::generateCFG(FD);
        auto customGraph = GraphGenerator::generateCustomCFG(FD);
    
        if (!clangGraph) {
            llvm::errs() << "Failed to generate CFG for function: " 
                        << FD->getNameAsString() << "\n";
            return true; // or appropriate error handling
        }
    
        // Then use the right graph type with exportToDot:
        if (!Visualizer::exportToDot(customGraph.get(), funcFilename)) {
            llvm::errs() << "Failed to export CFG to DOT file: " << funcFilename << "\n";
        }
    
        // Now append to the combined file, but ensure it's properly formatted
        std::string combinedFilename = OutputDir + "/combined_cfg.dot";
        
        // Check if file exists to determine if we need to write header
        bool needsHeader = !llvm::sys::fs::exists(combinedFilename);
        
        std::ofstream outFile(combinedFilename, 
                             needsHeader ? std::ios::out : std::ios::app);
                             
        if (!outFile.is_open()) {
            llvm::errs() << "Failed to open combined output file: " << combinedFilename << "\n";
            return true;
        }
        
        // Write header if this is a new file
        if (needsHeader) {
            outFile << "digraph \"Combined CFGs\" {\n";
            outFile << "  node [shape=box];\n";
            outFile << "  compound=true;\n";
        }
        
        // Create a subgraph for this function
        outFile << "  subgraph \"cluster_" << FD->getNameAsString() << "\" {\n";
        outFile << "    label=\"Function: " << FD->getNameAsString() << "\";\n";
        
        // Here we would add all nodes and edges, but properly escaped
        // For simplicity, just reference the individual file
        outFile << "    // See individual file: " << FD->getNameAsString() << "_cfg.dot\n";
        outFile << "  }\n\n";
        
        // No need to close the main digraph - we'll keep appending
        
        outFile.close();
        
        return true;
    }

    bool CFGVisitor::VisitCallExpr(clang::CallExpr* CE) {
        if (!CurrentFunction.empty() && CE) {
            // Try to get the called function
            if (auto *CalledFunc = CE->getDirectCallee()) {
                std::string CalledFuncName = CalledFunc->getQualifiedNameAsString();
                
                // Add dependency: CurrentFunction calls CalledFuncName
                FunctionDependencies[CurrentFunction].insert(CalledFuncName);
            }
        }
        return true;
    }
    
    void CFGVisitor::PrintFunctionDependencies() const {
        std::cout << "Function Dependencies:\n";
        std::cout << "---------------------\n";
        
        for (const auto& [caller, callees] : FunctionDependencies) {
            std::cout << caller << " calls:\n";
            for (const auto& callee : callees) {
                std::cout << "  - " << callee << "\n";
            }
            std::cout << "\n";
        }
    }
    
    std::unordered_map<std::string, std::set<std::string>> CFGVisitor::GetFunctionDependencies() const {
        return FunctionDependencies;
    }
    
    // Add method to finalize the combined file
    void CFGVisitor::FinalizeCombinedFile() {
        std::string combinedFilename = OutputDir + "/combined_cfg.dot";
        if (!llvm::sys::fs::exists(combinedFilename)) {
            return;  // Nothing to finalize
        }
        
        // Open in append mode to add closing brace
        std::ofstream outFile(combinedFilename, std::ios::app);
        if (!outFile.is_open()) {
            llvm::errs() << "Failed to finalize combined output file: " << combinedFilename << "\n";
            return;
        }
        
        outFile << "}\n";  // Close the main digraph
        outFile.close();
    }
}

void CFGAnalyzer::CFGConsumer::HandleTranslationUnit(clang::ASTContext& Context) {
    Visitor->TraverseDecl(Context.getTranslationUnitDecl());
    Visitor->PrintFunctionDependencies();
    Visitor->FinalizeCombinedFile();  // Call to finalize the combined dot file
}

void processFile(const std::string& filePath, const std::string& outputDir) {
    // Parse the file and generate AST + CFG
    std::cout << "DEBUG: Processing file: " << filePath << std::endl;

    clang::ASTContext* astContext = Parser::parseFile(filePath);
    if (!astContext) {
        std::cerr << "DEBUG: Failed to parse " << filePath << std::endl;
        return;
    }

    std::cout << "DEBUG: Successfully obtained AST Context" << std::endl;
    std::cout << "DEBUG: Beginning to search for function declarations..." << std::endl;

    bool foundAnyFunctions = false;
    int totalDecls = 0;
    int functionDecls = 0;
    int functionsWithBodies = 0;

    std::unique_ptr<GraphGenerator::CFGGraph> lastCfgGraph = nullptr;
    
    // Iterate through all function declarations in the AST
    for (auto decl : astContext->getTranslationUnitDecl()->decls()) {
        totalDecls++;
        
        if (auto functionDecl = llvm::dyn_cast<clang::FunctionDecl>(decl)) {
            functionDecls++;
            std::cout << "DEBUG: Found function declaration: " << functionDecl->getNameAsString() << std::endl;
            
            if (functionDecl->hasBody()) {
                functionsWithBodies++;
                foundAnyFunctions = true;
                std::cout << "DEBUG: Function " << functionDecl->getNameAsString() << " has a body" << std::endl;
                
                auto cfgGraph = GraphGenerator::generateCFG(functionDecl);
                if (cfgGraph) {
                    lastCfgGraph.reset(cfgGraph.release());
                    std::cout << "DEBUG: Successfully generated CFG for " << functionDecl->getNameAsString() << std::endl;
                } else {
                    std::cout << "DEBUG: Failed to generate CFG for function " << functionDecl->getNameAsString() << std::endl;
                }
            } else {
                std::cout << "DEBUG: Function " << functionDecl->getNameAsString() << " has NO body" << std::endl;
            }
        } else if (auto methodDecl = llvm::dyn_cast<clang::CXXMethodDecl>(decl)) {
            // Handle class methods separately
            functionDecls++;
            std::cout << "DEBUG: Found method declaration: " << methodDecl->getNameAsString() 
                      << " in class " << methodDecl->getParent()->getNameAsString() << std::endl;
            
            if (methodDecl->hasBody()) {
                functionsWithBodies++;
                foundAnyFunctions = true;
                std::cout << "DEBUG: Method " << methodDecl->getNameAsString() << " has a body" << std::endl;
                
                auto cfgGraph = GraphGenerator::generateCFG(methodDecl);
                if (cfgGraph) {
                    lastCfgGraph.reset(cfgGraph.release());
                    std::cout << "DEBUG: Successfully generated CFG for " << methodDecl->getNameAsString() << std::endl;
                } else {
                    std::cout << "DEBUG: Failed to generate CFG for method " << methodDecl->getNameAsString() << std::endl;
                }
            } else {
                std::cout << "DEBUG: Method " << methodDecl->getNameAsString() << " has NO body" << std::endl;
            }
        }
    }

    std::cout << "DEBUG: AST analysis summary:" << std::endl;
    std::cout << "  Total declarations: " << totalDecls << std::endl;
    std::cout << "  Function/method declarations: " << functionDecls << std::endl;
    std::cout << "  Functions/methods with bodies: " << functionsWithBodies << std::endl;

    // Check if we found any functions
    if (!foundAnyFunctions) {
        std::cerr << "Error: No functions with bodies found in " << filePath << std::endl;
        return;
    }

    std::cout << "DEBUG: Functions with bodies were successfully found" << std::endl;

    // Extract function dependencies
    json functionCallJson;
    //FunctionCallAnalyzer functionAnalyzer;
    //functionAnalyzer.analyzeFunctionCalls(*astContext);
    //functionCallJson = functionAnalyzer.getResults();

    // Save merged JSON output - using the last CFG for now
    std::string outputFile = outputDir + "/cfg_" + llvm::sys::path::filename(filePath).str() + ".json";
    json astJson;  // Define an empty JSON object
    
}

std::unique_ptr<clang::ASTConsumer> CFGAnalyzer::CFGAction::CreateASTConsumer(
    clang::CompilerInstance& CI, llvm::StringRef File) {
    return std::make_unique<CFGConsumer>(&CI.getASTContext(), OutputDir);  // Pass OutputDir
}

class CFGAnalysisConsumer : public clang::ASTConsumer {
public:
    explicit CFGAnalysisConsumer(clang::ASTContext* Context, const std::string& OutputDir) 
        : Context(Context), OutputDir(OutputDir) {
        if (!Context) {
            llvm::errs() << "Error: Null ASTContext passed to CFGAnalysisConsumer\n";
        }
    }

    void HandleTranslationUnit(clang::ASTContext& Context) override {
        // Create a visitor and traverse the entire translation unit
        CFGVisitor visitor(Context, OutputDir);
        visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    clang::ASTContext* Context;
    std::string OutputDir;

    // Inner visitor class to handle AST traversal
    class CFGVisitor : public clang::RecursiveASTVisitor<CFGVisitor> {
    public:
        CFGVisitor(clang::ASTContext& Context, const std::string& OutputDir) 
            : Context(Context), OutputDir(OutputDir) {}

        bool VisitFunctionDecl(clang::FunctionDecl* FD) {
            if (!FD || !FD->hasBody()) return true;

            // Detailed logging
            llvm::outs() << "Analyzing function: " << FD->getNameAsString() << "\n";

            try {
                // Attempt CFG generation with error handling
                auto clangGraph = GraphGenerator::generateCFG(FD);
                if (!clangGraph) {
                    llvm::errs() << "Failed to generate standard CFG for function: " 
                                 << FD->getNameAsString() << "\n";
                    
                    // Additional diagnostic information
                    if (FD->isTemplateInstantiation()) {
                        llvm::errs() << "Function is a template instantiation\n";
                    }
                    if (FD->getTemplatedKind() == clang::FunctionDecl::TK_FunctionTemplate) {
                        llvm::errs() << "Function is a function template\n";
                    }
                }

                auto customGraph = GraphGenerator::generateCustomCFG(FD);
                if (!customGraph) {
                    llvm::errs() << "Failed to generate custom CFG for function: " 
                                 << FD->getNameAsString() << "\n";
                }
            } catch (const std::exception& e) {
                llvm::errs() << "Exception during CFG generation for " 
                             << FD->getNameAsString() << ": " << e.what() << "\n";
            }

            return true;
        }

    private:
        clang::ASTContext& Context;
        std::string OutputDir;
    };
};

class CFGAnalysisFrontendAction : public clang::ASTFrontendAction {
public:
    CFGAnalysisFrontendAction(const std::string& OutputDir) 
        : OutputDir(OutputDir) {}

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance& CI, llvm::StringRef File) override {
        // Ensure we always return a valid ASTConsumer
        return std::make_unique<CFGAnalysisConsumer>(&CI.getASTContext(), OutputDir);
    }

private:
    std::string OutputDir;
};

bool CFGAnalyzer::analyze(const std::string& filename) {
    if (!llvm::sys::fs::exists(filename)) {
        llvm::errs() << "Error: File does not exist: " << filename << "\n";
        return false;
    }

    // Create output directory
    std::error_code EC = llvm::sys::fs::create_directories(CFGOutputDir.getValue());
    if (!EC) {
        // If EC is empty (no error), directory creation succeeded
        if (VerboseOutput) {
            llvm::outs() << "Created output directory: " << CFGOutputDir.getValue() << "\n";
        }
    } else if (EC == std::errc::file_exists) {
        // Directory already exists, which is fine
        if (VerboseOutput) {
            llvm::outs() << "Using existing output directory: " << CFGOutputDir.getValue() << "\n";
        }
    } else {
        // Some other error occurred
        llvm::errs() << "Error creating output directory: " << EC.message() << "\n";
        return false;
    }

    // Setup compilation arguments
    std::vector<std::string> CommandLine = {
        "-std=c++17",
        "-I.",
        "-I/usr/include",
        "-I/usr/local/include"
    };

    // Create compilation database
    auto Compilations = std::make_unique<clang::tooling::FixedCompilationDatabase>(
        ".", CommandLine);
    if (!Compilations) {
        llvm::errs() << "Failed to create compilation database.\n";
        return false;
    }

    // Create and run ClangTool
    std::vector<std::string> Sources{filename};
    clang::tooling::ClangTool Tool(*Compilations, Sources);

    // Create a custom frontend action factory
    class CFGActionFactory : public clang::tooling::FrontendActionFactory {
    public:
        explicit CFGActionFactory(const std::string& outputDir) 
            : OutputDir(outputDir) {}

        std::unique_ptr<clang::FrontendAction> create() override {
            return std::make_unique<CFGAction>(OutputDir);
        }

    private:
        std::string OutputDir;
    };

    // Create action factory with the output directory
    CFGActionFactory ActionFactory(CFGOutputDir.getValue());

    if (VerboseOutput) {
        llvm::outs() << "Starting analysis of " << filename << "\n";
    }

    // Run the tool using the action factory
    int Result = Tool.run(&ActionFactory);
    
    if (Result != 0) {
        llvm::errs() << "Analysis failed with code: " << Result << "\n";
        return false;
    }

    return true;
}

CFGAnalyzer::CFGVisitor::CFGVisitor(clang::ASTContext* Ctx, const std::string& outputDir) 
    : Context(Ctx), OutputDir(outputDir) {
}

CFGAnalyzer::CFGConsumer::CFGConsumer(clang::ASTContext* Context, const std::string& outputDir) {
    Visitor = std::make_unique<CFGVisitor>(Context, outputDir);  // Pass outputDir
}

CFGAnalyzer::CFGAction::CFGAction(const std::string& outputDir) 
    : OutputDir(outputDir) {
}

int main(int argc, const char** argv) {
    llvm::cl::opt<std::string> outputDir("output-dir", llvm::cl::desc("Output directory"), 
                                        llvm::cl::value_desc("directory"), 
                                        llvm::cl::init("output"));
    llvm::cl::list<std::string> inputFiles(llvm::cl::Positional, 
                                         llvm::cl::desc("<input source files>"), 
                                         llvm::cl::OneOrMore);

    llvm::cl::HideUnrelatedOptions(CFGAnalyzerCategory);
    llvm::cl::ParseCommandLineOptions(argc, argv, "CFG Analyzer Tool\n");

    // Create output directory
    std::error_code EC = llvm::sys::fs::create_directories(outputDir.getValue());
    if (EC && EC != std::errc::file_exists) {
        llvm::errs() << "Error creating output directory: " << EC.message() << "\n";
        return 1;
    }

    // Create compilation database
    std::vector<std::string> CommandLine = {
        "-std=c++17",
        "-I.",
        "-I/usr/include",
        "-I/usr/local/include"
    };
    auto Compilations = std::make_unique<clang::tooling::FixedCompilationDatabase>(
        ".", CommandLine);

    // Collect source files
    std::vector<std::string> Sources;
    if (!InputFile.empty()) {
        Sources.push_back(InputFile);
    }
    Sources.insert(Sources.end(), inputFiles.begin(), inputFiles.end());

    // Create ClangTool
    clang::tooling::ClangTool Tool(*Compilations, Sources);

    // Create action factory
    class ActionFactory : public clang::tooling::FrontendActionFactory {
    public:
        explicit ActionFactory(const std::string& outputDir) : OutputDir(outputDir) {}

        std::unique_ptr<clang::FrontendAction> create() override {
            return std::make_unique<CFGAnalyzer::CFGAction>(OutputDir);
        }

    private:
        std::string OutputDir;
    };

    // Run the tool
    ActionFactory factory(outputDir);
    int Result = Tool.run(&factory);
    
    if (Result != 0) {
        llvm::errs() << "Analysis failed with code: " << Result << "\n";
        return 1;
    }

    return 0;
}