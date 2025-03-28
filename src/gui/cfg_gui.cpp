#include "cfg_gui.h"
#include "cfg_analyzer.h"
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QFile>
#include <QPrinter>
#include <QPainter>
#include <QTextStream>
#include <QDateTime>
#include <QRandomGenerator>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <cmath>

namespace CFGAnalyzer {

CFGVisualizerWindow::CFGVisualizerWindow(QWidget *parent)
    : QMainWindow(parent), zoomFactor(1.0), currentLayoutAlgorithm(0) {
    setupUI();
    setWindowTitle("CFG Analyzer");
    resize(1200, 800);
    
    // Setup menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    QAction* openAction = fileMenu->addAction("&Open...");
    connect(openAction, &QAction::triggered, this, &CFGVisualizerWindow::browseFile);
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("&Exit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &CFGVisualizerWindow::showAbout);
    
    statusBar()->showMessage("Ready");
}

CFGVisualizerWindow::~CFGVisualizerWindow() {
    // Clean up resources if needed
}

void CFGVisualizerWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create tab widget
    tabWidget = new QTabWidget();
    
    // Input tab
    inputTab = new QWidget();
    QVBoxLayout* inputLayout = new QVBoxLayout(inputTab);
    
    QHBoxLayout* fileSelectLayout = new QHBoxLayout();
    QLabel* fileLabel = new QLabel("Source File:");
    filePathEdit = new QLineEdit();
    browseButton = new QPushButton("Browse...");
    fileSelectLayout->addWidget(fileLabel);
    fileSelectLayout->addWidget(filePathEdit);
    fileSelectLayout->addWidget(browseButton);
    
    analyzeButton = new QPushButton("Analyze");
    analyzeButton->setMinimumHeight(40);
    
    QLabel* outputLabel = new QLabel("Analysis Output:");
    outputConsole = new QTextEdit();
    outputConsole->setReadOnly(true);
    outputConsole->setFont(QFont("Monospace", 10));
    
    inputLayout->addLayout(fileSelectLayout);
    inputLayout->addWidget(analyzeButton);
    inputLayout->addWidget(outputLabel);
    inputLayout->addWidget(outputConsole);
    
    // Visualization tab
    visualizationTab = new QWidget();
    QVBoxLayout* visLayout = new QVBoxLayout(visualizationTab);
    
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    QLabel* layoutLabel = new QLabel("Layout:");
    layoutComboBox = new QComboBox();
    layoutComboBox->addItem("Force-Directed");
    layoutComboBox->addItem("Hierarchical");
    layoutComboBox->addItem("Circular");
    
    zoomInButton = new QPushButton("Zoom In");
    zoomOutButton = new QPushButton("Zoom Out");
    resetZoomButton = new QPushButton("Reset Zoom");
    exportButton = new QPushButton("Export");
    
    controlsLayout->addWidget(layoutLabel);
    controlsLayout->addWidget(layoutComboBox);
    controlsLayout->addStretch();
    controlsLayout->addWidget(zoomInButton);
    controlsLayout->addWidget(zoomOutButton);
    controlsLayout->addWidget(resetZoomButton);
    controlsLayout->addWidget(exportButton);
    
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    
    visLayout->addLayout(controlsLayout);
    visLayout->addWidget(view);
    
    // Add tabs to tab widget
    tabWidget->addTab(inputTab, "Input & Analysis");
    tabWidget->addTab(visualizationTab, "Visualization");
    
    mainLayout->addWidget(tabWidget);
    
    // Connect signals and slots
    connect(browseButton, &QPushButton::clicked, this, &CFGVisualizerWindow::browseFile);
    connect(analyzeButton, &QPushButton::clicked, this, &CFGVisualizerWindow::analyzeFile);
    connect(zoomInButton, &QPushButton::clicked, this, &CFGVisualizerWindow::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &CFGVisualizerWindow::zoomOut);
    connect(resetZoomButton, &QPushButton::clicked, this, &CFGVisualizerWindow::resetZoom);
    connect(exportButton, &QPushButton::clicked, this, &CFGVisualizerWindow::exportGraph);
    connect(layoutComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &CFGVisualizerWindow::switchLayoutAlgorithm);
}

void CFGVisualizerWindow::browseFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Source File", 
                                                  "", "C/C++ Files (*.c *.cpp *.h *.hpp);;All Files (*)");
    if (!filePath.isEmpty()) {
        filePathEdit->setText(filePath);
    }
}

void CFGVisualizerWindow::analyzeFile() {
    currentFile = filePathEdit->text().toStdString();
    if (currentFile.empty()) {
        QMessageBox::warning(this, "Error", "Please select a file to analyze.");
        return;
    }
    
    outputConsole->clear();
    outputConsole->append("Starting analysis of " + QString::fromStdString(currentFile) + "...");
    QApplication::processEvents();
    
    bool success = analyze(currentFile);
    
    if (success) {
        outputConsole->append("Analysis completed successfully.");
        tabWidget->setCurrentIndex(1); // Switch to visualization tab
        
        // In a real application, you would get the dependencies from your analyzer
        // This is a placeholder - replace with your actual data retrieval
        // functionDependencies = yourAnalyzer.getFunctionDependencies();
        
        // For demonstration, let's create some sample data
        std::unordered_map<std::string, std::set<std::string>> sampleDeps;
        sampleDeps["main"] = {"init", "processData", "cleanup"};
        sampleDeps["init"] = {"loadConfig", "setupEnvironment"};
        sampleDeps["processData"] = {"parseInput", "transform", "validate"};
        sampleDeps["validate"] = {"checkRange", "verifyFormat"};
        
        loadFunctionDependencies(sampleDeps);
    } else {
        outputConsole->append("Analysis failed. Check for errors.");
    }
}

void CFGVisualizerWindow::loadFunctionDependencies(
        const std::unordered_map<std::string, std::set<std::string>>& dependencies) {
    functionDependencies = dependencies;
    outputConsole->append("Loaded " + QString::number(dependencies.size()) + " function dependencies.");
    renderDependencyGraph();
}

void CFGVisualizerWindow::renderDependencyGraph() {
    clearGraph();
    
    // Choose layout algorithm based on current selection
    switch (currentLayoutAlgorithm) {
        case 0: applyForceDirectedLayout(); break;
        case 1: applyHierarchicalLayout(); break;
        case 2: applyCircularLayout(); break;
        default: applyForceDirectedLayout(); break;
    }
    
    // Adjust scene rect and center view
    scene->setSceneRect(scene->itemsBoundingRect().adjusted(-50, -50, 50, 50));
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    view->centerOn(0, 0);
}

void CFGVisualizerWindow::clearGraph() {
    scene->clear();
}

void CFGVisualizerWindow::applyForceDirectedLayout() {
    // Simple force-directed layout algorithm
    struct NodeInfo {
        QGraphicsEllipseItem* node;
        QGraphicsTextItem* label;
        qreal x, y;
        qreal dx, dy;
    };
    
    // Create nodes for all functions
    std::unordered_map<std::string, NodeInfo> nodes;
    
    // First pass: create all nodes at random positions
    for (const auto& [func, _] : functionDependencies) {
        // Generate random position
        qreal x = QRandomGenerator::global()->bounded(500) - 250;
        qreal y = QRandomGenerator::global()->bounded(500) - 250;
        
        // Create node
        auto* nodeItem = scene->addEllipse(-40, -20, 80, 40, 
                                         QPen(Qt::black), 
                                         QBrush(QColor(100, 149, 237)));
        nodeItem->setPos(x, y);
        nodeItem->setZValue(1);
        
        // Create label
        auto* textItem = scene->addText(QString::fromStdString(func));
        textItem->setPos(x - textItem->boundingRect().width()/2, y - 10);
        textItem->setZValue(2);
        
        // Store node info
        nodes[func] = {nodeItem, textItem, x, y, 0, 0};
    }
    
    // Create edges
    std::vector<QGraphicsLineItem*> edges;
    for (const auto& [caller, callees] : functionDependencies) {
        if (nodes.find(caller) != nodes.end()) {
            for (const auto& callee : callees) {
                if (nodes.find(callee) != nodes.end()) {
                    // Create a line from caller to callee
                    auto* line = scene->addLine(
                        nodes[caller].x, nodes[caller].y,
                        nodes[callee].x, nodes[callee].y,
                        QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
                    );
                    line->setZValue(0);
                    edges.push_back(line);
                }
            }
        }
    }
    
    // Simulate force-directed layout for a few iterations
    const int ITERATIONS = 50;
    const qreal REPULSION = 6000.0;
    const qreal ATTRACTION = 0.06;
    const qreal MAX_DISPLACEMENT = 30.0;
    
    for (int iter = 0; iter < ITERATIONS; iter++) {
        // Calculate repulsive forces between all nodes
        for (auto& [name1, node1] : nodes) {
            node1.dx = 0;
            node1.dy = 0;
            
            for (const auto& [name2, node2] : nodes) {
                if (name1 == name2) continue;
                
                qreal dx = node1.x - node2.x;
                qreal dy = node1.y - node2.y;
                qreal distance = std::max(1.0, std::sqrt(dx*dx + dy*dy));
                
                // Repulsive force
                node1.dx += (dx / distance) * (REPULSION / distance);
                node1.dy += (dy / distance) * (REPULSION / distance);
            }
        }
        
        // Calculate attractive forces along edges
        for (const auto& [caller, callees] : functionDependencies) {
            if (nodes.find(caller) == nodes.end()) continue;
            
            for (const auto& callee : callees) {
                if (nodes.find(callee) == nodes.end()) continue;
                
                qreal dx = nodes[caller].x - nodes[callee].x;
                qreal dy = nodes[caller].y - nodes[callee].y;
                qreal distance = std::sqrt(dx*dx + dy*dy);
                
                // Attractive force
                nodes[caller].dx -= dx * ATTRACTION;
                nodes[caller].dy -= dy * ATTRACTION;
                nodes[callee].dx += dx * ATTRACTION;
                nodes[callee].dy += dy * ATTRACTION;
            }
        }
        
        // Apply forces with limit
        for (auto& [name, node] : nodes) {
            qreal displacement = std::sqrt(node.dx*node.dx + node.dy*node.dy);
            if (displacement > 0) {
                qreal scale = std::min(MAX_DISPLACEMENT, displacement) / displacement;
                node.x += node.dx * scale;
                node.y += node.dy * scale;
            }
        }
    }
    
    // Update positions
    for (auto& [name, node] : nodes) {
        node.node->setPos(node.x, node.y);
        node.label->setPos(node.x - node.label->boundingRect().width()/2, 
                          node.y - node.label->boundingRect().height()/2);
    }
    
    // Update edges
    int edgeIndex = 0;
    for (const auto& [caller, callees] : functionDependencies) {
        if (nodes.find(caller) == nodes.end()) continue;
        
        for (const auto& callee : callees) {
            if (nodes.find(callee) == nodes.end() || edgeIndex >= edges.size()) continue;
            
            edges[edgeIndex]->setLine(
                nodes[caller].x, nodes[caller].y,
                nodes[callee].x, nodes[callee].y
            );
            edgeIndex++;
        }
    }
}

void CFGVisualizerWindow::applyHierarchicalLayout() {
    // Simple hierarchical layout (top to bottom)
    std::unordered_map<std::string, int> depths;
    std::unordered_map<std::string, QGraphicsEllipseItem*> nodeItems;
    std::unordered_map<std::string, QGraphicsTextItem*> textItems;
    
    // Calculate depths (longest path from root)
    std::function<int(const std::string&, std::set<std::string>&)> calculateDepth;
    calculateDepth = [&](const std::string& func, std::set<std::string>& visited) -> int {
        if (visited.find(func) != visited.end()) return 0; // Prevent cycles
        visited.insert(func);
        
        int maxChildDepth = 0;
        if (functionDependencies.find(func) != functionDependencies.end()) {
            for (const auto& callee : functionDependencies.at(func)) {
                maxChildDepth = std::max(maxChildDepth, calculateDepth(callee, visited));
            }
        }
        
        depths[func] = maxChildDepth + 1;
        return depths[func];
    };
    
    // Find root nodes (not called by anyone)
    std::set<std::string> allFunctions;
    std::set<std::string> calledFunctions;
    
    for (const auto& [caller, callees] : functionDependencies) {
        allFunctions.insert(caller);
        for (const auto& callee : callees) {
            allFunctions.insert(callee);
            calledFunctions.insert(callee);
        }
    }
    
    std::vector<std::string> roots;
    for (const auto& func : allFunctions) {
        if (calledFunctions.find(func) == calledFunctions.end()) {
            roots.push_back(func);
        }
    }
    
    // If no roots found, use all functions as potential roots
    if (roots.empty()) {
        for (const auto& [func, _] : functionDependencies) {
            roots.push_back(func);
        }
    }
    
    // Calculate depths starting from roots
    for (const auto& root : roots) {
        std::set<std::string> visited;
        calculateDepth(root, visited);
    }
    
    // Count functions at each depth
    std::map<int, int> depthCounts;
    for (const auto& [func, depth] : depths) {
        depthCounts[depth]++;
    }
    
    // Position nodes by depth
    const int LEVEL_HEIGHT = 100;
    const int NODE_WIDTH = 120;
    
    for (const auto& [func, depth] : depths) {
        int count = depthCounts[depth];
        int position = 0;
        
        // Find position of this function among functions at the same depth
        int i = 0;
        for (const auto& [otherFunc, otherDepth] : depths) {
            if (otherDepth == depth) {
                if (otherFunc == func) break;
                i++;
            }
        }
        position = i;
        
        // Calculate x position to center nodes at each level
        qreal x = (position - (count - 1) / 2.0) * NODE_WIDTH;
        qreal y = depth * LEVEL_HEIGHT;
        
        // Create node
        auto* nodeItem = scene->addEllipse(-40, -20, 80, 40, 
                                         QPen(Qt::black), 
                                         QBrush(QColor(100, 149, 237)));
        nodeItem->setPos(x, y);
        
        // Create label
        auto* textItem = scene->addText(QString::fromStdString(func));
        textItem->setPos(x - textItem->boundingRect().width()/2, y - 10);
        
        nodeItems[func] = nodeItem;
        textItems[func] = textItem;
    }
    
    // Create edges
    for (const auto& [caller, callees] : functionDependencies) {
        if (nodeItems.find(caller) == nodeItems.end()) continue;
        
        for (const auto& callee : callees) {
            if (nodeItems.find(callee) == nodeItems.end()) continue;
            
            QPointF callerPos = nodeItems[caller]->pos();
            QPointF calleePos = nodeItems[callee]->pos();
            
            // Create a line from caller to callee
            scene->addLine(
                callerPos.x(), callerPos.y() + 20, // Bottom of caller
                calleePos.x(), calleePos.y() - 20, // Top of callee
                QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
            );
        }
    }
}

void CFGVisualizerWindow::applyCircularLayout() {
    // Simple circular layout
    std::unordered_map<std::string, QGraphicsEllipseItem*> nodeItems;
    std::unordered_map<std::string, QGraphicsTextItem*> textItems;
    
    // Count total functions
    std::set<std::string> allFunctions;
    for (const auto& [caller, callees] : functionDependencies) {
        allFunctions.insert(caller);
        for (const auto& callee : callees) {
            allFunctions.insert(callee);
        }
    }
    
    int count = allFunctions.size();
    if (count == 0) return;
    
    const qreal RADIUS = 200;
    const qreal CENTER_X = 0;
    const qreal CENTER_Y = 0;
    
    // Position nodes in a circle
    int i = 0;
    for (const auto& func : allFunctions) {
        qreal angle = 2.0 * M_PI * i / count;
        qreal x = CENTER_X + RADIUS * std::cos(angle);
        qreal y = CENTER_Y + RADIUS * std::sin(angle);
        
        // Create node
        auto* nodeItem = scene->addEllipse(-40, -20, 80, 40, 
                                         QPen(Qt::black), 
                                         QBrush(QColor(100, 149, 237)));
        nodeItem->setPos(x, y);
        
        // Create label
        auto* textItem = scene->addText(QString::fromStdString(func));
        textItem->setPos(x - textItem->boundingRect().width()/2, y - 10);
        
        nodeItems[func] = nodeItem;
        textItems[func] = textItem;
        
        i++;
    }
    
    // Create edges
    for (const auto& [caller, callees] : functionDependencies) {
        if (nodeItems.find(caller) == nodeItems.end()) continue;
        
        for (const auto& callee : callees) {
            if (nodeItems.find(callee) == nodeItems.end()) continue;
            
            QPointF callerPos = nodeItems[caller]->pos();
            QPointF calleePos = nodeItems[callee]->pos();
            
            // Create a curved line from caller to callee
            QPointF controlPoint = QPointF(
                (callerPos.x() + calleePos.x()) / 2 + (calleePos.y() - callerPos.y()) / 4,
                (callerPos.y() + calleePos.y()) / 2 - (calleePos.x() - callerPos.x()) / 4
            );
            
            QPainterPath path;
            path.moveTo(callerPos);
            path.quadTo(controlPoint, calleePos);
            
            scene->addPath(path, QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }
    }
}

void CFGVisualizerWindow::zoomIn() {
    zoomFactor *= 1.2;
    view->scale(1.2, 1.2);
}

void CFGVisualizerWindow::zoomOut() {
    zoomFactor /= 1.2;
    view->scale(1/1.2, 1/1.2);
}

void CFGVisualizerWindow::resetZoom() {
    view->resetTransform();
    zoomFactor = 1.0;
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void CFGVisualizerWindow::exportGraph() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Graph", 
                                                  "", "PNG Images (*.png);;PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;
    
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::white);
    
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter);
    
    if (fileName.endsWith(".png", Qt::CaseInsensitive)) {
        if (image.save(fileName)) {
            statusBar()->showMessage("Graph exported to " + fileName, 3000);
        } else {
            QMessageBox::warning(this, "Export Failed", "Failed to export graph to " + fileName);
        }
    } else if (fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        
        QPainter pdfPainter(&printer);
        pdfPainter.setRenderHint(QPainter::Antialiasing);
        scene->render(&pdfPainter);
        
        statusBar()->showMessage("Graph exported to " + fileName, 3000);
    }
}

void CFGVisualizerWindow::switchLayoutAlgorithm(int index) {
    currentLayoutAlgorithm = index;
    renderDependencyGraph();
}

void CFGVisualizerWindow::showAbout() {
    QMessageBox::about(this, "About CFG Analyzer",
                     "CFG Analyzer - Control Flow Graph Analysis Tool\n\n"
                     "Version 1.0\n\n"
                     "A tool for analyzing and visualizing control flow graphs\n"
                     "and function dependencies in C/C++ code.");
}

}