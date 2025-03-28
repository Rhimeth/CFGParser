#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graph_generator.h"
#include "visualizer.h"
#include "cfg_generation_action.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::generateCFG()
{
    if (m_sourceFiles.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select source files first.");
        return;
    }

    try {
        // Convert QStringList to std::vector<std::string>
        std::vector<std::string> sourceFilesPaths;
        for (const QString& file : m_sourceFiles) {
            sourceFilesPaths.push_back(file.toStdString());
        }

        // Create a compilation database
        std::string errorMessage;
        auto compilationDatabase = clang::tooling::FixedCompilationDatabase::loadFromDirectory(".", errorMessage);
        if (!compilationDatabase) {
            throw std::runtime_error("Failed to create compilation database: " + errorMessage);
        }

        // Vector to store generated CFGs
        std::vector<std::unique_ptr<GraphGenerator::CFGGraph>> cfgGraphs;

        // Create ClangTool
        clang::tooling::ClangTool tool(*compilationDatabase, sourceFilesPaths);

        // Create a custom factory for the frontend action
        CFGGenerationActionFactory actionFactory(cfgGraphs);

        // Run the tool
        int result = tool.run(&actionFactory);

        if (result != 0) {
            throw std::runtime_error("Failed to generate CFG");
        }

        if (cfgGraphs.empty()) {
            QMessageBox::warning(this, "No CFGs", "No valid functions found in the source files.");
            return;
        }

        // Generate DOT representation for the first graph (you might want to modify this)
        std::string dotGraph = Visualizer::generateDotRepresentation(cfgGraphs[0].get());

        // Render the graph
        renderDotGraph(QString::fromStdString(dotGraph));
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "CFG Generation Error", 
            QString("Failed to generate CFG: %1").arg(e.what()));
    }
}

void MainWindow::renderDotGraph(const QString& dotGraph)
{
    QProcess process;
    process.start("dot", QStringList() << "-Tplain");
    process.write(dotGraph.toUtf8());
    process.closeWriteChannel();
    process.waitForFinished();

    // Read the output
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    
    // Parse the plain format output
    parsePlainFormat(output);
}

void MainWindow::parsePlainFormat(const QString& plainOutput)
{
    // Clear previous graph
    ui->graph->clear();

    QStringList lines = plainOutput.split('\n', Qt::SkipEmptyParts);
    
    for (const QString& line : lines) {
        QStringList parts = line.split(' ');
        
        if (parts[0] == "node") {
            // node nodename x y width height label style shape color
            if (parts.size() >= 7) {
                QString nodeId = parts[1];
                double x = parts[2].toDouble();
                double y = parts[3].toDouble();
                QString label = parts[6];

                // Add node to graph view
                ui->graph->addNode(nodeId.toStdString(), label.toStdString());
            }
        }
        else if (parts[0] == "edge") {
            // edge nodename1 nodename2 
            if (parts.size() >= 3) {
                QString fromNode = parts[1];
                QString toNode = parts[2];
                // TODO: Implement edge drawing if needed
            }
        }
    }
}