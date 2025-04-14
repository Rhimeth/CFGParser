#include "mainwindow.h"
#include "cfg_analyzer.h"
#include "ui_mainwindow.h"
#include "visualizer.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebChannel>
#include <QPainter>
#include <QPrinter>
#include <QThreadPool>
#include <QPageLayout>
#include <QPageSize>
#include <QSvgGenerator>
#include <QBrush>
#include <QPen>
#include <QProcess>
#include <QTimer>
#include <QFuture>
#include <exception>
#include <QtConcurrent>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QRandomGenerator>
#include <QMutex>
#include <clang/Frontend/ASTUnit.h>
#include <cmath>
#include <QCheckBox>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QMenu>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_webChannel(new QWebChannel(this)),
    m_currentLayoutAlgorithm(Hierarchical),
    m_scene(nullptr),
    m_analysisThread(nullptr),
    m_graphView(nullptr),
    m_highlightNode(nullptr),
    m_highlightEdge(nullptr)
{
    ui->setupUi(this);

    ui->splitter_2->setSizes({400, 100});

    ui->webView->setMinimumSize(200, 200);
    ui->reportTextEdit->setMinimumSize(200, 100);
    
    m_webView = ui->webView;
    m_webView->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    m_webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    
    // Set up web channel
    m_webChannel->registerObject("bridge", this);
    m_webView->page()->setWebChannel(m_webChannel);
    
    // Hide the graphLabel as it might interfere
    ui->graphLabel->hide();
    
    m_currentGraph = nullptr;
    m_currentDotContent = QString();
    m_loadedFiles = QStringList();

    if (!verifyGraphvizInstallation()) {
        QMessageBox::warning(this, "Warning", 
            "Graph visualization features will be limited");
    }

    setupVisualizationComponents();
    
    // Set default theme
    m_currentTheme = {
        QColor("#ffffff"),  // nodeColor
        QColor("#000000"),  // edgeColor
        QColor("#000000"),  // textColor
        QColor("#f0f0f0")   // backgroundColor
    };

    // Connect signals
    setupConnections();
    
    // Load empty initial state
    if (m_webView) {
        loadEmptyVisualization();
    }
}

void MainWindow::setupVisualizationComponents() {

    if (!ui || !ui->splitter_2) {
        qCritical() << "UI not properly initialized";
        return;
    }

    if (!m_webView) {
        m_webView = new QWebEngineView(this);
        ui->splitter_2->insertWidget(0, m_webView);
    }

    if (!m_webChannel) {
        m_webChannel = new QWebChannel(this);
    }
    // Create web view with safety checks
    if (!m_webView) {
        m_webView = new QWebEngineView(this);
        // Check if ui and splitter_2 exist before accessing them
        if (ui && ui->splitter_2) {
            ui->splitter_2->insertWidget(0, m_webView);
            m_webView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            
            // Only proceed with web setup if m_webChannel exists
            if (m_webChannel) {
                m_webChannel->registerObject("bridge", this);
                m_webView->page()->setWebChannel(m_webChannel);
                
                QWebEngineSettings* settings = m_webView->settings();
                settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
                settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
            }
        } else {
            // Handle missing UI component
            qWarning() << "splitter_2 widget not found in UI";

            QVBoxLayout* mainLayout = new QVBoxLayout();
            mainLayout->addWidget(m_webView);
            QWidget* centralWidget = new QWidget(this);
            centralWidget->setLayout(mainLayout);
            setCentralWidget(centralWidget);
        }
    }
    
    // Initialize graph view with safety checks
    if (!m_graphView) {
        m_graphView = new CustomGraphView(this);
        
        QWidget* central = centralWidget();
        if (central) {
            if (!central->layout()) {
                central->setLayout(new QVBoxLayout());
            }
            central->layout()->addWidget(m_graphView);
        }
        
        // Create scene
        if (!m_scene) {
            m_scene = new QGraphicsScene(this);
            if (m_graphView) {
                m_graphView->setScene(m_scene);
            }
        }
    }
}

void MainWindow::setupConnections()
{
    // File operations
    connect(ui->browseButton, &QPushButton::clicked, 
            this, &MainWindow::on_browseButton_clicked);
    connect(ui->analyzeButton, &QPushButton::clicked,
            this, &MainWindow::on_analyzeButton_clicked);

    connect(ui->extractAstButton, &QPushButton::clicked, this, &MainWindow::on_extractAstButton_clicked);
    connect(ui->openFilesButton, &QPushButton::clicked, this, &MainWindow::on_openFilesButton_clicked);
    connect(ui->loadJsonButton, &QPushButton::clicked, this, &MainWindow::onLoadJsonClicked);
    connect(ui->mergeCfgsButton, &QPushButton::clicked, this, &MainWindow::onMergeCfgsClicked);
    
    // Visualization controls
    connect(ui->toggleFunctionGraph, &QPushButton::clicked, 
            this, &MainWindow::toggleVisualizationMode);
    connect(ui->searchButton, &QPushButton::clicked, 
            this, &MainWindow::highlightSearchResults);
    
    // Context menu
    m_webView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_webView, &QWebEngineView::customContextMenuRequested,
            this, &MainWindow::showVisualizationContextMenu);
}

void MainWindow::showVisualizationContextMenu(const QPoint& pos) {
    QMenu menu;
    menu.addAction("Export as PNG", this, [this]() {
        exportGraph("png");
    });
    menu.addAction("Export as SVG", this, [this]() {
        exportGraph("svg");
    });
    menu.addAction("Export as DOT", this, [this]() {
        exportGraph("dot");
    });
    menu.addSeparator();
    menu.addAction("Zoom In", this, &MainWindow::zoomIn);
    menu.addAction("Zoom Out", this, &MainWindow::zoomOut);
    menu.addAction("Reset View", this, &MainWindow::resetZoom);
    
    menu.exec(m_webView->mapToGlobal(pos));
}

void MainWindow::setupWebView()
{
    m_webChannel->registerObject("bridge", this);
    m_webView->page()->setWebChannel(m_webChannel);
    
    // Enable JavaScript
    m_webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
}

void MainWindow::loadEmptyVisualization() {
    if (m_webView && m_webView->isVisible()) {
        QString html = R"(
<!DOCTYPE html>
<html>
<head>
    <style>
        body { 
            background-color: #f0f0f0;
            color: #000000;
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        #placeholder {
            text-align: center;
            opacity: 0.5;
        }
    </style>
</head>
<body>
    <div id="placeholder">
        <h1>No CFG Loaded</h1>
        <p>Analyze a C++ file to visualize its control flow graph</p>
    </div>
</body>
</html>
        )";
        m_webView->setHtml(html);
    } else {
        ui->graphLabel->setText("No CFG Loaded\nAnalyze a C++ file to visualize its control flow graph");
        ui->graphLabel->setAlignment(Qt::AlignCenter);
        ui->graphLabel->show();
    }
}

void MainWindow::displayGraph(const QString& dotContent)
{
    if (!m_webView) {
        qCritical() << "Web view not initialized";
        return;
    }

    m_currentDotContent = dotContent;
    
    QString escapedDotContent = dotContent;
    escapedDotContent.replace("\\", "\\\\").replace("`", "\\`");
    
    QString html = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <title>CFG Visualization</title>
    <script src="qrc:/qtwebchannel/qwebchannel.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/viz.js/2.1.2/viz.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/viz.js/2.1.2/full.render.js"></script>
    <style>
        body { margin:0; padding:0; overflow:hidden; }
        #graph-container { 
            width:100%; 
            height:100%;
            background:#f8f8f8;
        }
        .node:hover { stroke-width:2px; }
        .edge:hover { stroke-width:3px; }
    </style>
</head>
<body>
    <div id="graph-container"></div>
    <script>
        new QWebChannel(qt.webChannelTransport, function(channel) {
            window.bridge = channel.objects.bridge;
        });

        const viz = new Viz();
        const dot = `%1`;
        
        viz.renderSVGElement(dot)
            .then(element => {
                element.style.width = '100%';
                element.style.height = '100%';
                
                element.addEventListener('click', (e) => {
                    const node = e.target.closest('[id^="node"]');
                    if (node && window.bridge) {
                        window.bridge.onNodeClicked(node.id.replace('node', ''));
                    }
                });
                
                element.addEventListener('mouseover', (e) => {
                    const edge = e.target.closest('[id^="edge"]');
                    if (edge && window.bridge) {
                        const [from, to] = edge.id.replace('edge', '').split('_');
                        window.bridge.onEdgeHovered(from, to);
                    }
                });
                
                document.getElementById('graph-container').appendChild(element);
            })
            .catch(error => {
                document.getElementById('graph-container').innerHTML = 
                    '<p style="color:red;padding:20px">Error rendering graph:<br>' + 
                    error + '</p>';
                console.error(error);
            });
    </script>
</body>
</html>
    )").arg(escapedDotContent);
    
    m_webView->setHtml(html);
}

QString MainWindow::generateInteractiveGraphHtml(const QString& dotContent)
{
    QString escapedDotContent = dotContent;
    escapedDotContent.replace("\\", "\\\\")
                   .replace("`", "\\`")
                   .replace("$", "\\$")
                   .replace("\"", "\\\"");

    QString html = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>CFG Visualization</title>
    <script src="qrc:/qtwebchannel/qwebchannel.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/viz.js/2.1.2/viz.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/viz.js/2.1.2/full.render.js"></script>
    <style>
        body { 
            margin: 0; 
            padding: 0; 
            overflow: hidden;
            background: #f8f8f8;
            font-family: Arial, sans-serif;
        }
        #graph-container {
            width: 100%;
            height: 100vh;
            position: relative;
        }
        #graph-svg {
            width: 100%;
            height: 100%;
        }
        .node {
            stroke-width: 1px;
            stroke: #333;
        }
        .node:hover { 
            stroke-width: 2px;
            stroke: #0066cc;
            cursor: pointer;
        }
        .edge {
            fill: none;
            stroke: #666;
            stroke-width: 1.5px;
        }
        .edge:hover {
            stroke-width: 3px;
            stroke: #cc0000;
        }
        .node-label {
            font-size: 12px;
            font-weight: bold;
            fill: #333;
            pointer-events: none;
        }
        .edge-label {
            font-size: 10px;
            fill: #666;
            pointer-events: none;
        }
        #loading-message {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            font-size: 16px;
            color: #666;
        }
        .error-message {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            color: #cc0000;
            padding: 20px;
            text-align: center;
            max-width: 80%;
            background: rgba(255,255,255,0.9);
            border-radius: 5px;
        }
        #tooltip {
            position: absolute;
            padding: 8px;
            background: rgba(0,0,0,0.8);
            color: white;
            border-radius: 4px;
            pointer-events: none;
            font-size: 12px;
            max-width: 300px;
            z-index: 100;
            display: none;
        }
    </style>
</head>
<body>
    <div id="graph-container">
        <div id="loading-message">Rendering control flow graph...</div>
        <div id="tooltip"></div>
    </div>

    <script>
        // Initialize Qt WebChannel
        var bridgeReady = false;
        new QWebChannel(qt.webChannelTransport, function(channel) {
            window.bridge = channel.objects.bridge;
            bridgeReady = true;
        });

        // DOM elements
        const container = document.getElementById('graph-container');
        const loadingMsg = document.getElementById('loading-message');
        const tooltip = document.getElementById('tooltip');

        // Track mouse position for tooltip
        let mouseX = 0, mouseY = 0;
        document.addEventListener('mousemove', (e) => {
            mouseX = e.clientX;
            mouseY = e.clientY;
        });

        // Enhanced label styling function
        function styleLabels(svgElement) {
            const nodes = svgElement.querySelectorAll('[id^="node"]');
            const edges = svgElement.querySelectorAll('[id^="edge"]');
            
            // Style node labels
            nodes.forEach(node => {
                const labels = node.querySelectorAll('text');
                labels.forEach(label => {
                    label.classList.add('node-label');
                    
                    // Add title for tooltip
                    const title = document.createElementNS('http://www.w3.org/2000/svg', 'title');
                    title.textContent = label.textContent;
                    node.appendChild(title);
                });
            });
            
            // Style edge labels
            edges.forEach(edge => {
                const labels = edge.querySelectorAll('text');
                labels.forEach(label => {
                    label.classList.add('edge-label');
                });
            });
        }

        // Tooltip functions
        function showTooltip(content) {
            tooltip.innerHTML = content;
            tooltip.style.display = 'block';
            tooltip.style.left = (mouseX + 15) + 'px';
            tooltip.style.top = (mouseY + 15) + 'px';
        }

        function hideTooltip() {
            tooltip.style.display = 'none';
        }

        // Event handlers
        function handleNodeClick(e) {
            const node = e.target.closest('[id^="node"]');
            if (node && bridgeReady) {
                const nodeId = node.id.replace('node', '');
                try {
                    window.bridge.onNodeClicked(nodeId);
                } catch (e) {
                    console.error('Node click handler error:', e);
                }
            }
        }

        function handleEdgeHover(e) {
            const edge = e.target.closest('[id^="edge"]');
            if (edge && bridgeReady) {
                const [from, to] = edge.id.replace('edge', '').split('_');
                try {
                    window.bridge.onEdgeHovered(from, to);
                } catch (e) {
                    console.error('Edge hover handler error:', e);
                }
            }
        }

        function handleMouseOver(e) {
            const element = e.target;
            const title = element.querySelector('title');
            if (title) {
                showTooltip(title.textContent);
            }
        }

        function handleMouseOut() {
            hideTooltip();
        }

        // Main rendering function
        function renderGraph() {
            try {
                const viz = new Viz();
                const dot = `%1`;
                
                viz.renderSVGElement(dot)
                    .then(svg => {
                        // Remove loading message
                        container.removeChild(loadingMsg);
                        
                        // Configure SVG element
                        svg.id = 'graph-svg';
                        svg.style.width = '100%';
                        svg.style.height = '100%';
                        
                        // Enhance labels and tooltips
                        styleLabels(svg);
                        
                        // Add interactivity
                        svg.addEventListener('click', handleNodeClick);
                        svg.addEventListener('mousemove', handleEdgeHover);
                        svg.addEventListener('mouseover', handleMouseOver);
                        svg.addEventListener('mouseout', handleMouseOut);
                        
                        container.appendChild(svg);
                    })
                    .catch(error => {
                        showError('Failed to render graph: ' + error);
                        console.error('Viz.js error:', error);
                    });
            } catch (e) {
                showError('Initialization error: ' + e);
                console.error('Initialization error:', e);
            }
        }

        function showError(message) {
            container.removeChild(loadingMsg);
            const errorDiv = document.createElement('div');
            errorDiv.className = 'error-message';
            errorDiv.innerHTML = message;
            container.appendChild(errorDiv);
        }

        // Start rendering when ready
        if (document.readyState === 'complete') {
            renderGraph();
        } else {
            window.addEventListener('load', renderGraph);
        }
    </script>
</body>
</html>
    )").arg(escapedDotContent);

    return html;
}

void MainWindow::onDisplayGraphClicked()
{
    if (!m_currentGraph) {
        QMessageBox::warning(this, "Warning", "No graph to display. Please analyze a file first.");
        return;
    }
    
    if (ui->webView->isVisible()) {
        visualizeCurrentGraph();
    } else if (m_graphView) {
        visualizeCFG(m_currentGraph);
    }
}

void MainWindow::exportGraph(const QString& defaultFormat) {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Graph",
        QDir::homePath(),
        "PNG Files (*.png);;SVG Files (*.svg);;PDF Files (*.pdf);;DOT Files (*.dot)"
    );

    if (fileName.isEmpty()) return;

    if (!m_currentGraph) {
        QMessageBox::warning(this, "Warning", "No graph to export");
        return;
    }

    try {
        std::string dotContent = Visualizer::generateDotRepresentation(m_currentGraph.get());
        
        if (fileName.endsWith(".dot")) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(dotContent.c_str());
            }
        } else {
            QString format;
            if (fileName.endsWith(".png")) format = "png";
            else if (fileName.endsWith(".svg")) format = "svg";
            else format = "pdf";
            
            QTemporaryFile tempFile;
            if (tempFile.open()) {
                tempFile.write(dotContent.c_str());
                tempFile.close();
                renderDotToImage(tempFile.fileName(), fileName, format);
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Export Error", QString("Failed to export: %1").arg(e.what()));
    }
}

void MainWindow::displaySvgInWebView(const QString& svgPath) {
    QFile file(svgPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QString svgContent = file.readAll();
    file.close();
    
    // Create HTML wrapper
    QString html = QString(
        "<html><body style='margin:0;padding:0;'>"
        "<div style='width:100%%;height:100%%;overflow:auto;'>%1</div>"
        "</body></html>"
    ).arg(svgContent);
    
    if (!m_webView) {
        return;
    }
    
    m_webView->setHtml(html);
}

bool MainWindow::displayImage(const QString& imagePath) {
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) return false;

    if (m_graphView) {
        QGraphicsScene* scene = new QGraphicsScene(this);
        scene->addPixmap(pixmap);
        m_graphView->setScene(scene);
        m_graphView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    else if (m_scene) {
        m_scene->clear();
        m_scene->addPixmap(pixmap);
        if (m_graphView) {
            m_graphView->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        }
        return true;
    }
    return false;
}

bool MainWindow::renderAndDisplayDot(const QString& dotContent) {
    // Save DOT content
    QString dotPath = QDir::temp().filePath("live_cfg.dot");
    QFile dotFile(dotPath);
    if (!dotFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not write DOT to file:" << dotPath;
        return false;
    }
    QTextStream out(&dotFile);
    out << dotContent;
    dotFile.close();

    QString outputPath;
    if (m_webView && m_webView->isVisible()) {
        outputPath = dotPath + ".svg";
        if (!renderDotToImage(dotPath, outputPath, "svg")) return false;
        displaySvgInWebView(outputPath);
        return true;
    } else {
        outputPath = dotPath + ".png";
        if (!renderDotToImage(dotPath, outputPath, "png")) return false;
        return displayImage(outputPath);
    }
}

void MainWindow::safeInitialize() {
    if (!tryInitializeView(true)) {
        qWarning() << "Hardware acceleration failed, trying software fallback";
        
        if (!tryInitializeView(false)) {
            qCritical() << "All graphics initialization failed";
            startTextOnlyMode();
        }
    }
}

bool MainWindow::tryInitializeView(bool tryHardware) {
    // Cleanup any existing views
    if (m_graphView) {
        m_graphView->setScene(nullptr);
        delete m_graphView;
        m_graphView = nullptr;
    }
    if (m_scene) {
        delete m_scene;
        m_scene = nullptr;
    }

    try {
        // Create basic scene
        m_scene = new QGraphicsScene(this);
        m_scene->setBackgroundBrush(Qt::white);
        
        m_graphView = new CustomGraphView(centralWidget());
        
        if (tryHardware) {
            m_graphView->setViewport(new QOpenGLWidget());
        } else {
            QWidget* simpleViewport = new QWidget();
            simpleViewport->setAttribute(Qt::WA_OpaquePaintEvent);
            simpleViewport->setAttribute(Qt::WA_NoSystemBackground);
            m_graphView->setViewport(simpleViewport);
        }
        
        m_graphView->setScene(m_scene);
        
        // Add to layout
        if (!centralWidget()->layout()) {
            centralWidget()->setLayout(new QVBoxLayout());
        }
        centralWidget()->layout()->addWidget(m_graphView);
        
        return testRendering();
        
    } catch (...) {
        return false;
    }
}

bool MainWindow::verifyDotFile(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        qDebug() << "File does not exist:" << filePath;
        return false;
    }
    
    if (fileInfo.size() == 0) {
        qDebug() << "File is empty:" << filePath;
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    QString firstLine = in.readLine();
    file.close();

    if (!firstLine.contains("digraph") && !firstLine.contains("graph")) {
        qDebug() << "Not a valid DOT file:" << firstLine;
        return false;
    }

    return true;
}

bool MainWindow::verifyGraphvizInstallation() {
    QString dotPath = QStandardPaths::findExecutable("dot");
    if (dotPath.isEmpty()) {
        qWarning() << "Graphviz 'dot' executable not found";
        return false;
    }

    QProcess dotCheck;
    dotCheck.start(dotPath, {"-V"});
    if (!dotCheck.waitForFinished(1000) || dotCheck.exitCode() != 0) {
        qWarning() << "Graphviz check failed:" << dotCheck.errorString();
        return false;
    }

    qDebug() << "Graphviz found at:" << dotPath;
    return true;
}

bool MainWindow::testRendering() {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-variable"
    QGraphicsRectItem* testItem = m_scene->addRect(0, 0, 100, 100, 
        QPen(Qt::red), QBrush(Qt::blue));
    #pragma GCC diagnostic pop
    
    QImage testImg(100, 100, QImage::Format_ARGB32);
    QPainter painter(&testImg);
    m_scene->render(&painter);
    painter.end();
    
    // Verify some pixels changed
    return testImg.pixelColor(50, 50) != QColor(Qt::white);
}

void MainWindow::startTextOnlyMode() {
    qDebug() << "Starting in text-only mode";
    
    connect(this, &MainWindow::analysisComplete, this, 
        [this](const CFGAnalyzer::AnalysisResult& result) {
            ui->reportTextEdit->setPlainText(QString::fromStdString(result.dotOutput));
        });
}

void MainWindow::createNode() {
    if (!m_scene) return;
    
    QGraphicsEllipseItem* nodeItem = new QGraphicsEllipseItem(0, 0, 50, 50);
    nodeItem->setFlag(QGraphicsItem::ItemIsSelectable);
    nodeItem->setFlag(QGraphicsItem::ItemIsMovable);
    m_scene->addItem(nodeItem);
    
    // Center view on new item
    QTimer::singleShot(0, this, [this, nodeItem]() {
        if (m_graphView && nodeItem->scene()) {
            m_graphView->centerOn(nodeItem);
        }
    });
}

void MainWindow::createEdge() {
    Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
    
    if (!m_graphView || !m_graphView->scene()) {
        qWarning() << "Cannot create edge - graph view or scene not initialized";
        return;
    }

    QGraphicsLineItem* edgeItem = new QGraphicsLineItem();
    edgeItem->setData(MainWindow::EdgeItemType, 1);
    
    edgeItem->setPen(QPen(Qt::black, 2));
    edgeItem->setFlag(QGraphicsItem::ItemIsSelectable);
    edgeItem->setZValue(-1);

    try {
        m_graphView->scene()->addItem(edgeItem);
        qDebug() << "Edge created - scene items:" << m_graphView->scene()->items().size();
    } catch (const std::exception& e) {
        qCritical() << "Failed to add edge:" << e.what();
        delete edgeItem;
    }
}

void MainWindow::onAnalysisComplete(CFGAnalyzer::AnalysisResult result)
{
    Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
    
    if (result.success) {
        if (!result.dotOutput.empty()) {
            m_currentGraph = parseDotToCFG(QString::fromStdString(result.dotOutput));
            visualizeCFG(m_currentGraph);
        }
        
        ui->reportTextEdit->setPlainText(QString::fromStdString(result.report));
    } else {
        QMessageBox::warning(this, "Analysis Failed", 
                            QString::fromStdString(result.report));
    }
    
    setUiEnabled(true);
}

void MainWindow::connectNodesWithEdge(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to) {
    if (!from || !to || !m_scene) return;

    QPointF fromCenter = from->mapToScene(from->rect().center());
    QPointF toCenter = to->mapToScene(to->rect().center());
    
    QGraphicsLineItem* edge = new QGraphicsLineItem(QLineF(fromCenter, toCenter));
    edge->setData(EdgeItemType, 1);
    edge->setPen(QPen(Qt::black, 2));
    edge->setZValue(-1);
    
    m_scene->addItem(edge);
}

void MainWindow::addItemToScene(QGraphicsItem* item)
{
    if (!m_scene) {
        qWarning() << "No active scene - deleting item";
        delete item;
        return;
    }

    try {
        m_scene->addItem(item);
    } catch (...) {
        qCritical() << "Failed to add item to scene";
        delete item;
    }
}

void MainWindow::setupGraphView()
{
    qDebug() << "=== Starting graph view setup ===";
    
    if (m_scene) {
        m_scene->clear();
        delete m_scene;
    }
    if (m_graphView) {
        centralWidget()->layout()->removeWidget(m_graphView);
        delete m_graphView;
    }

    m_scene = new QGraphicsScene(this);
    QGraphicsRectItem* testItem = m_scene->addRect(0, 0, 100, 100, 
        QPen(Qt::red), QBrush(Qt::blue));
    testItem->setFlag(QGraphicsItem::ItemIsMovable);

    m_graphView = new CustomGraphView(centralWidget());
    m_graphView->setViewport(new QWidget());
    m_graphView->setScene(m_scene);
    m_graphView->setRenderHint(QPainter::Antialiasing, false);

    if (!centralWidget()->layout()) {
        centralWidget()->setLayout(new QVBoxLayout());
    }
    centralWidget()->layout()->addWidget(m_graphView);

    qDebug() << "=== Graph view test setup complete ===";
    qDebug() << "Test item at:" << testItem->scenePos();
    qDebug() << "Viewport type:" << m_graphView->viewport()->metaObject()->className();
}

void MainWindow::visualizeCFG(std::shared_ptr<GraphGenerator::CFGGraph> graph)
{
    if (!graph || !m_webView) {
        qWarning() << "Invalid graph or web view";
        return;
    }

    try {
        std::string dotContent = Visualizer::generateDotRepresentation(graph.get());
        m_currentGraph = graph;
        
        ui->webView->show();
        ui->graphLabel->hide();
        
        QString html = generateInteractiveGraphHtml(QString::fromStdString(dotContent));
        m_webView->setHtml(html);
        
        // Make sure the web view gets focus
        ui->splitter_2->setSizes({500, 100});
        
    } catch (const std::exception& e) {
        qCritical() << "Visualization error:" << e.what();
        QMessageBox::critical(this, "Error", 
            QString("Failed to visualize graph:\n%1").arg(e.what()));
    }
}

void MainWindow::onVisualizationError(const QString& error) {
    QMessageBox::warning(this, "Visualization Error", error);
    statusBar()->showMessage("Visualization failed", 3000);
}

void MainWindow::showEdgeContextMenu(const QPoint& pos) {
    QMenu menu;
    menu.addAction("Highlight Path", this, [this](){
        // Implementation for highlighting path
        statusBar()->showMessage("Path highlighting not implemented yet", 2000);
    });
    
    menu.exec(m_graphView->mapToGlobal(pos));
}

std::shared_ptr<GraphGenerator::CFGGraph> MainWindow::parseDotToCFG(const QString& dotContent)
{
    auto graph = std::make_shared<GraphGenerator::CFGGraph>();
        
    QRegularExpression nodeRegex(R"(^\s*(\d+)\s*\[([^\]]*)\]\s*;?\s*$)");
    QRegularExpression edgeRegex(R"(^\s*(\d+)\s*->\s*(\d+)\s*\[([^\]]*)\]\s*;?\s*$)");
    QRegularExpression labelRegex(R"~(label\s*=\\s*"([^"]*)")~");
    QRegularExpression colorRegex(R"~(color\s*=\s*"?(red|blue|green|black|white|gray)"?)~");
    QRegularExpression shapeRegex(R"~(shape\s*=\s*"?(box|ellipse|diamond|circle)"?)~");

    // Verify regex validity
    auto checkRegex = [](const QRegularExpression& re, const QString& name) {
        if (!re.isValid()) {
            qCritical() << "Invalid" << name << "regex:" << re.errorString() << "Pattern:" << re.pattern();
            return false;
        }
        return true;
    };

    if (!checkRegex(nodeRegex, "node") || 
        !checkRegex(edgeRegex, "edge") ||
        !checkRegex(labelRegex, "label") ||
        !checkRegex(colorRegex, "color") ||
        !checkRegex(shapeRegex, "shape")) {
        return graph;
    }

    QStringList lines = dotContent.split('\n', Qt::SkipEmptyParts);
    
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        
        if (trimmed.startsWith("//") || trimmed.startsWith("/*") || 
            trimmed.startsWith("digraph") || trimmed.startsWith("}") || 
            trimmed.isEmpty()) {
            continue;
        }
        
        // Parse node
        auto nodeMatch = nodeRegex.match(trimmed);
        if (nodeMatch.hasMatch()) {
            bool ok;
            int id = nodeMatch.captured(1).toInt(&ok);
            if (!ok) continue;
            
            graph->addNode(id);
            
            QString attributes = nodeMatch.captured(2);
            auto labelMatch = labelRegex.match(attributes);
            if (labelMatch.hasMatch()) {
                graph->addStatement(id, labelMatch.captured(1).toStdString());
            }
            
            auto colorMatch = colorRegex.match(attributes);
            if (colorMatch.hasMatch() && colorMatch.captured(1) == "red") {
                graph->markNodeAsThrowingException(id);
            }
            
            auto shapeMatch = shapeRegex.match(attributes);
            if (shapeMatch.hasMatch() && shapeMatch.captured(1) == "box") {
                graph->markNodeAsTryBlock(id);
            }
            continue;
        }
        
        // Parse edge
        auto edgeMatch = edgeRegex.match(trimmed);
        if (edgeMatch.hasMatch()) {
            bool ok1, ok2;
            int fromId = edgeMatch.captured(1).toInt(&ok1);
            int toId = edgeMatch.captured(2).toInt(&ok2);
            if (!ok1 || !ok2) continue;
            
            graph->addEdge(fromId, toId);
            
            QString attributes = edgeMatch.captured(3);
            auto colorMatch = colorRegex.match(attributes);
            if (colorMatch.hasMatch() && colorMatch.captured(1) == "red") {
                graph->addExceptionEdge(fromId, toId);
            }
        }
    }
    
    return graph;
}

void MainWindow::loadAndProcessJson(const QString& filePath) 
{
    if (!QFile::exists(filePath)) {
        qWarning() << "JSON file does not exist:" << filePath;
        QMessageBox::warning(this, "Error", "JSON file not found: " + filePath);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open JSON file:" << file.errorString();
        QMessageBox::warning(this, "Error", "Could not open JSON file: " + file.errorString());
        return;
    }

    // Read and parse JSON
    QJsonParseError parseError;
    QByteArray jsonData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error at offset" << parseError.offset << ":" << parseError.errorString();
        QMessageBox::warning(this, "JSON Error", 
                           QString("Parse error at position %1: %2")
                           .arg(parseError.offset)
                           .arg(parseError.errorString()));
        return;
    }

    if (doc.isNull()) {
        qWarning() << "Invalid JSON document";
        QMessageBox::warning(this, "Error", "Invalid JSON document");
        return;
    }

    try {
        QJsonObject jsonObj = doc.object();
        
        // Example processing - adapt to your needs
        if (jsonObj.contains("nodes") && jsonObj["nodes"].isArray()) {
            QJsonArray nodes = jsonObj["nodes"].toArray();
            for (const QJsonValue& node : nodes) {
                if (node.isObject()) {
                    QJsonObject nodeObj = node.toObject();
                    // Process each node
                }
            }
        }
        
        QMetaObject::invokeMethod(this, [this, jsonObj]() {
            m_graphView->parseJson(QJsonDocument(jsonObj).toJson());
            statusBar()->showMessage("JSON loaded successfully", 3000);
        });
        
    } catch (const std::exception& e) {
        qCritical() << "JSON processing error:" << e.what();
        QMessageBox::critical(this, "Processing Error", 
                            QString("Error processing JSON: %1").arg(e.what()));
    }
}

void MainWindow::initializeGraphviz()
{
    QString dotPath = QStandardPaths::findExecutable("dot");
    if (dotPath.isEmpty()) {
        qCritical() << "Graphviz 'dot' not found in PATH";
        QMessageBox::critical(this, "Error", 
                            "Graphviz 'dot' executable not found.\n"
                            "Please install Graphviz and ensure it's in your PATH.");
        startTextOnlyMode();
        return;
    }
    
    qDebug() << "Found Graphviz dot at:" << dotPath;
    setupGraphView();
}

void MainWindow::analyzeDotFile(const QString& filePath) {
    if (!verifyDotFile(filePath)) return;

    QString tempDir = QDir::tempPath();
    QString baseName = QFileInfo(filePath).completeBaseName();
    QString pngPath = tempDir + "/" + baseName + "_graph.png";
    QString svgPath = tempDir + "/" + baseName + "_graph.svg";

    // Try PNG first
    if (renderDotToImage(filePath, pngPath)) {
        displayImage(pngPath);
        return;
    }

    // Fallback to SVG
    if (renderDotToImage(filePath, svgPath)) {
        displaySvgInWebView(svgPath);
        return;
    }

    showRawDotContent(filePath);
}

bool MainWindow::renderDotToImage(const QString& dotPath, const QString& outputPath, const QString& format)
{
    // Validate inputs
    if (!QFile::exists(dotPath)) {  // Using parameter dotPath here
        qWarning() << "DOT file does not exist:" << dotPath;
        return false;
    }

    // Determine output format
    QString outputFormat = format.toLower();
    if (outputFormat.isEmpty()) {
        if (outputPath.endsWith(".png", Qt::CaseInsensitive)) outputFormat = "png";
        else if (outputPath.endsWith(".svg", Qt::CaseInsensitive)) outputFormat = "svg";
        else if (outputPath.endsWith(".pdf", Qt::CaseInsensitive)) outputFormat = "pdf";
        else {
            qWarning() << "Unsupported output format:" << outputPath;
            return false;
        }
    }

    QString program = "dot";
    QString dotExecutablePath = QStandardPaths::findExecutable(program);
    if (dotExecutablePath.isEmpty()) {
        qWarning() << "Graphviz not found in PATH";
        QMessageBox::warning(this, "Error", 
                           "Graphviz 'dot' tool not found.\n"
                           "Please install Graphviz (sudo apt install graphviz)");
        return false;
    }

    QStringList arguments;
    arguments << QString("-T%1").arg(outputFormat)
              << dotPath
              << "-o" << outputPath;

    QProcess dotProcess;
    dotProcess.start(dotExecutablePath, arguments);
    
    if (!dotProcess.waitForFinished(5000)) {
        qWarning() << "Graphviz error:" << dotProcess.errorString()
                  << "\nProcess output:" << dotProcess.readAllStandardError();
        return false;
    }
    
    if (dotProcess.exitCode() != 0) {
        qWarning() << "Graphviz failed with exit code:" << dotProcess.exitCode()
                  << "\nError output:" << dotProcess.readAllStandardError();
        return false;
    }

    return QFile::exists(outputPath);
}

void MainWindow::showRawDotContent(const QString& dotPath) {
    QFile file(dotPath);
    if (file.open(QIODevice::ReadOnly)) {
        ui->reportTextEdit->setPlainText(file.readAll());
        file.close();
    }
}

void MainWindow::visualizeCurrentGraph() {
    if (!m_currentGraph) return;
    
    std::string dot = Visualizer::generateDotRepresentation(m_currentGraph.get());
    
    // Load into web view
    QString html = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <title>CFG Visualization</title>
    <script src="qrc:/qtwebchannel/qwebchannel.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/viz.js/2.1.2/viz.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/viz.js/2.1.2/full.render.js"></script>
    <style>
        body { margin:0; background:#2D2D2D; }
        #graph-container { width:100%; height:100%; }
    </style>
</head>
<body>
    <div id="graph-container"></div>
    <script>
        new QWebChannel(qt.webChannelTransport, function(channel) {
            window.bridge = channel.objects.bridge;
        });

        const viz = new Viz();
        viz.renderSVGElement(`%1`)
            .then(element => {
                // Node click handling
                element.addEventListener('click', (e) => {
                    const node = e.target.closest('[id^="node"]');
                    if (node && window.bridge) {
                        window.bridge.onNodeClicked(node.id.replace('node', ''));
                    }
                });
                
                // Edge hover handling
                element.addEventListener('mousemove', (e) => {
                    const edge = e.target.closest('[id^="edge"]');
                    if (edge && window.bridge) {
                        const [from, to] = edge.id.replace('edge', '').split('_');
                        window.bridge.onEdgeHovered(parseInt(from), parseInt(to));
                    }
                });
                
                document.getElementById('graph-container').appendChild(element);
            });
    </script>
</body>
</html>
    )").arg(QString::fromStdString(dot));
    
    m_webView->setHtml(html);
}

void MainWindow::highlightNode(int nodeId, const QColor& color)
{
    if (!m_graphView || !m_graphView->scene()) return;
    
    // Reset previous highlighting
    resetHighlighting();
    
    foreach (QGraphicsItem* item, m_graphView->scene()->items()) {
        if (item->data(MainWindow::NodeItemType).toInt() == 1) {
            if (auto ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                if (item->data(MainWindow::NodeIdKey).toInt() == nodeId) {
                    QPen pen = ellipse->pen();
                    pen.setWidth(3);
                    pen.setColor(Qt::darkBlue);
                    ellipse->setPen(pen);
                    
                    QBrush brush = ellipse->brush();
                    brush.setColor(color);
                    ellipse->setBrush(brush);
                    
                    m_highlightNode = item;
                    m_graphView->centerOn(item);
                    break;
                }
            }
        }
    }
}

void MainWindow::highlightEdge(int fromId, int toId, const QColor& color)
{
    if (!m_graphView || !m_graphView->scene()) return;
    
    if (m_highlightEdge) {
        if (auto line = qgraphicsitem_cast<QGraphicsLineItem*>(m_highlightEdge)) {
            QPen pen = line->pen();
            pen.setWidth(1);
            pen.setColor(Qt::black);
            line->setPen(pen);
        }
        m_highlightEdge = nullptr;
    }
    
    foreach (QGraphicsItem* item, m_graphView->scene()->items()) {
        if (item->data(MainWindow::EdgeItemType).toInt() == 1) {
            if (auto line = qgraphicsitem_cast<QGraphicsLineItem*>(item)) {
                if (item->data(MainWindow::EdgeFromKey).toInt() == fromId &&
                    item->data(MainWindow::EdgeToKey).toInt() == toId) {
                    QPen pen = line->pen();
                    pen.setWidth(3);
                    pen.setColor(color);
                    line->setPen(pen);
                    
                    m_highlightEdge = item;
                    break;
                }
            }
        }
    }
}

void MainWindow::resetHighlighting()
{
    if (m_highlightNode) {
        if (auto ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(m_highlightNode)) {
            QPen pen = ellipse->pen();
            pen.setWidth(1);
            pen.setColor(Qt::black);
            ellipse->setPen(pen);
            ellipse->setBrush(QBrush(Qt::lightGray));
        }
        m_highlightNode = nullptr;
    }
    
    if (m_highlightEdge) {
        if (auto line = qgraphicsitem_cast<QGraphicsLineItem*>(m_highlightEdge)) {
            QPen pen = line->pen();
            pen.setWidth(1);
            pen.setColor(Qt::black);
            line->setPen(pen);
        }
        m_highlightEdge = nullptr;
    }
}

void MainWindow::onNodeClicked(const QString& nodeId)
{
    bool ok;
    int id = nodeId.toInt(&ok);
    
    if (ok) {
        ui->statusbar->showMessage(QString("Node %1 selected").arg(id), 3000);
        highlightInCodeEditor(id);
    } else {
        qWarning() << "Invalid node ID:" << nodeId;
    }
}

void MainWindow::onEdgeHovered(const QString& from, const QString& to)
{
    bool ok1, ok2;
    int fromId = from.toInt(&ok1);
    int toId = to.toInt(&ok2);
    
    if (ok1 && ok2) {
        ui->statusbar->showMessage(QString("Edge %1 → %2").arg(fromId).arg(toId), 2000);
    } else {
        ui->statusbar->showMessage(QString("Edge %1 → %2").arg(from).arg(to), 2000);
    }
}

void MainWindow::showNodeContextMenu(const QPoint& pos) {
    QMenu menu;
    menu.addAction("View Details", this, [this](){
        // Implement node detail view
    });
    menu.addAction("Highlight Path", this, [this](){
        // Implement path highlighting
    });
    menu.addSeparator();
    menu.addAction("Export as PNG", this, [this](){
        exportGraph("PNG");
    });
    
    menu.exec(ui->webView->mapToGlobal(pos));
}

QString MainWindow::generateExportHtml() const {
    return QString(R"(
<!DOCTYPE html>
<html>
<head>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/viz.js/2.1.2/viz.js"></script>
    <style>
        body { margin: 0; padding: 0; }
        svg { width: 100%; height: 100%; }
    </style>
</head>
<body>
    <script>
        const dot = `%1`;
        const svg = Viz(dot, { format: 'svg', engine: 'dot' });
        document.body.innerHTML = svg;
    </script>
</body>
</html>
    )").arg(m_currentDotContent);
}

void MainWindow::onParseButtonClicked()
{
    QString filePath = ui->filePathEdit->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file first");
        return;
    }

    setUiEnabled(false);
    ui->reportTextEdit->clear();
    statusBar()->showMessage("Parsing file...");

    QFuture<void> future = QtConcurrent::run([this, filePath]() {
        try {
            // Read file content
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                throw std::runtime_error("Could not open file: " + filePath.toStdString());
            }
            
            QString dotContent = file.readAll();
            file.close();
            
            // Parse DOT content
            auto graph = parseDotToCFG(dotContent);
            
            // Count nodes and edges
            int nodeCount = 0;
            int edgeCount = 0;
            for (const auto& [id, node] : graph->getNodes()) {
                nodeCount++;
                edgeCount += node.successors.size();
            }
            
            QString report = QString("Parsed CFG from DOT file\n\n")
                           + QString("File: %1\n").arg(filePath)
                           + QString("Nodes: %1\n").arg(nodeCount)
                           + QString("Edges: %1\n").arg(edgeCount);
            
            QMetaObject::invokeMethod(this, [this, report, graph]() mutable {
                ui->reportTextEdit->setPlainText(report);
                visualizeCFG(graph); // Pass the shared_ptr directly
                setUiEnabled(true);
                statusBar()->showMessage("Parsing completed", 3000);
            });
            
        } catch (const std::exception& e) {
            QMetaObject::invokeMethod(this, [this, e]() {
                QMessageBox::critical(this, "Error", QString("Parsing failed: %1").arg(e.what()));
                setUiEnabled(true);
                statusBar()->showMessage("Parsing failed", 3000);
            });
        }
    });
}

void MainWindow::onParsingFinished(bool success)
{
    if (success) {
        qDebug() << "Parsing completed successfully";
    } else {
        qDebug() << "Parsing failed";
    }
}

void MainWindow::applyGraphTheme() {
    QColor normalNodeColor = Qt::white;
    QColor tryBlockColor = QColor(173, 216, 230);
    QColor throwBlockColor = QColor(240, 128, 128);
    QColor normalEdgeColor = Qt::black;

    const int TryBlockKey = QGraphicsItem::UserType + 3;
    const int ThrowingExceptionKey = QGraphicsItem::UserType + 4;

    if (m_graphView) {
        m_graphView->setThemeColors(normalNodeColor, normalEdgeColor, Qt::black);
        
        m_currentTheme.nodeColor = normalNodeColor;
        m_currentTheme.edgeColor = normalEdgeColor;
        
        if (m_graphView->scene()) {
            foreach (QGraphicsItem* item, m_graphView->scene()->items()) {
                if (item && item->data(NodeItemType).toInt() == 1) {
                    bool isTryBlock = item->data(TryBlockKey).toBool();
                    bool isThrowBlock = item->data(ThrowingExceptionKey).toBool();
                    
                    if (auto ellipse = dynamic_cast<QGraphicsEllipseItem*>(item)) {
                        if (isTryBlock)
                            ellipse->setBrush(tryBlockColor);
                        else if (isThrowBlock)
                            ellipse->setBrush(throwBlockColor);
                        else
                            ellipse->setBrush(normalNodeColor);
                    }
                }
            }
        }
    }
}

void MainWindow::setupGraphLayout() {
    if (!m_graphView) return;

    switch (m_currentLayoutAlgorithm) {
        case Hierarchical: 
            m_graphView->applyHierarchicalLayout(); 
            break;
        case ForceDirected: 
            m_graphView->applyForceDirectedLayout(); 
            break;
        case Circular: 
            m_graphView->applyCircularLayout(); 
            break;
    }
}

void MainWindow::applyGraphLayout() {
    if (!m_graphView) return;

    switch (m_currentLayoutAlgorithm) {
        case Hierarchical: 
            m_graphView->applyHierarchicalLayout(); 
            break;
        case ForceDirected: 
            m_graphView->applyForceDirectedLayout(); 
            break;
        case Circular: 
            m_graphView->applyCircularLayout(); 
            break;
    }
    
    if (m_graphView->scene()) {
        m_graphView->fitInView(m_graphView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::highlightFunction(const QString& functionName) {
    if (!m_graphView) return;
    
    foreach (QGraphicsItem* item, m_graphView->scene()->items()) {
        if (item->data(MainWindow::NodeItemType).toInt() == 1) {
            bool highlight = false;
            foreach (QGraphicsItem* child, item->childItems()) {
                if (auto text = dynamic_cast<QGraphicsTextItem*>(child)) {
                    if (text->toPlainText().contains(functionName, Qt::CaseInsensitive)) {
                        highlight = true;
                        break;
                    }
                }
            }
            
            if (auto ellipse = dynamic_cast<QGraphicsEllipseItem*>(item)) {
                QBrush brush = ellipse->brush();
                brush.setColor(highlight ? Qt::yellow : m_currentTheme.nodeColor);
                ellipse->setBrush(brush);
            }
        }
    }
}

void MainWindow::zoomIn() {
    m_graphView->scale(1.2, 1.2);
}

void MainWindow::zoomOut() {
    m_graphView->scale(1/1.2, 1/1.2);
}

void MainWindow::resetZoom() {
    m_graphView->resetTransform();
    m_graphView->fitInView(m_graphView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

    void MainWindow::on_browseButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Source File");
    if (!filePath.isEmpty()) {
        ui->filePathEdit->setText(filePath);
    }
}

void MainWindow::on_analyzeButton_clicked()
{
    QString filePath = ui->filePathEdit->text().trimmed();
    
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file first");
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    try {
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isReadable()) {
            throw std::runtime_error("Cannot read the selected file");
        }

        QStringList validExtensions = {".cpp", ".cxx", ".cc", ".h", ".hpp"};
        bool validExtension = std::any_of(validExtensions.begin(), validExtensions.end(),
            [&filePath](const QString& ext) {
                return filePath.endsWith(ext, Qt::CaseInsensitive);
            });
        
        if (!validExtension) {
            throw std::runtime_error(
                "Invalid file type. Please select a C++ source file");
        }

        // Clear previous results
        ui->reportTextEdit->clear();
        loadEmptyVisualization();

        statusBar()->showMessage("Analyzing file...");

        CFGAnalyzer::CFGAnalyzer analyzer;
        auto result = analyzer.analyzeFile(filePath);
        
        if (!result.success) {
            throw std::runtime_error(result.report);
        }

        m_currentGraph = parseDotToCFG(QString::fromStdString(result.dotOutput));
        displayGraph(QString::fromStdString(result.dotOutput));
        ui->reportTextEdit->setPlainText(QString::fromStdString(result.report));
        statusBar()->showMessage("Analysis completed", 3000);

    } catch (const std::exception& e) {
        QString errorMsg = QString("Analysis failed:\n%1\n"
                                 "Please verify:\n"
                                 "1. File contains valid C++ code\n"
                                 "2. Graphviz is installed").arg(e.what());
        QMessageBox::critical(this, "Error", errorMsg);
        statusBar()->showMessage("Analysis failed", 3000);
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_exportButton_clicked()
{
    if (!verifyGraphvizInstallation()) {
        QMessageBox::warning(this, "Error", "Graphviz 'dot' tool not found");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, "Export Graph", QDir::homePath(),  // Better default path
        "PNG (*.png);;SVG (*.svg);;PDF (*.pdf);;DOT (*.dot)"
    );

    if (fileName.isEmpty()) return;

    if (!m_currentGraph) {
        QMessageBox::warning(this, "Error", "No graph to export");
        return;
    }

    try {
        std::string dotStr = Visualizer::generateDotRepresentation(m_currentGraph.get());
        QTemporaryFile tempFile;
        if (!tempFile.open()) {
            throw std::runtime_error("Could not create temporary file");
        }
        tempFile.write(QString::fromStdString(dotStr).toUtf8());
        tempFile.close();

        if (fileName.endsWith(".dot")) {
            if (!QFile::copy(tempFile.fileName(), fileName)) {
                throw std::runtime_error("Could not copy DOT file");
            }
        } else {
            QString format = fileName.endsWith(".png") ? "png" :
                           fileName.endsWith(".svg") ? "svg" : "pdf";
            
            if (!renderDotToImage(tempFile.fileName(), fileName, format)) {
                throw std::runtime_error("Failed to generate image");
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Export Error", 
                            QString("Failed to export: %1").arg(e.what()));
    }
}

void MainWindow::handleAnalysisResult(const CFGAnalyzer::AnalysisResult& result) {
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(this, "handleAnalysisResult", 
                                 Qt::QueuedConnection,
                                 Q_ARG(CFGAnalyzer::AnalysisResult, result));
        return;
    }

    if (!result.success) {
        ui->reportTextEdit->setPlainText(QString::fromStdString(result.report));
        QMessageBox::critical(this, "Analysis Error", 
                            QString::fromStdString(result.report));
        return;
    }

    if (!result.dotOutput.empty()) {
        try {
            auto graph = parseDotToCFG(QString::fromStdString(result.dotOutput));
            m_currentGraph = graph;
            visualizeCFG(graph);
        } catch (...) {
            qWarning() << "Failed to visualize CFG";
        }
    }

    if (!result.jsonOutput.empty()) {
        m_graphView->parseJson(QString::fromStdString(result.jsonOutput).toUtf8());
    }

    statusBar()->showMessage("Analysis completed", 3000);
}

void MainWindow::on_extractAstButton_clicked() {
    QString filePath = ui->filePathEdit->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file first");
        return;
    }

    setUiEnabled(false);
    ui->reportTextEdit->clear();
    statusBar()->showMessage("Extracting AST...");

    QtConcurrent::run([this, filePath]() {
        try {
            // Create analyzer instance with fully qualified name
            CFGAnalyzer::CFGAnalyzer analyzer;
            // Pass QString directly without conversion
            auto result = analyzer.analyzeFile(filePath);
            
            // Update UI in main thread
            QMetaObject::invokeMethod(this, [this, result]() {
                handleAnalysisResult(result);
                setUiEnabled(true);
            });
        } catch (const std::exception& e) {
            QMetaObject::invokeMethod(this, [this, e]() {
                ui->reportTextEdit->setPlainText(QString("Error: %1").arg(e.what()));
                setUiEnabled(true);
                statusBar()->showMessage("Extraction failed", 3000);
            });
        }
    });
}

void MainWindow::displayFunctionInfo(const QString& input) {

    if (!m_currentGraph) {
        ui->reportTextEdit->append("No CFG loaded");
        return;
    }

    bool found = false;
    const auto& nodes = m_currentGraph->getNodes();
    
    for (const auto& [id, node] : nodes) {
        if (QString::fromStdString(node.functionName).contains(input, Qt::CaseInsensitive)) {
            found = true;
            
            // Display basic function info
            ui->reportTextEdit->append(QString("Function: %1").arg(QString::fromStdString(node.functionName)));
            ui->reportTextEdit->append(QString("Node ID: %1").arg(id));
            ui->reportTextEdit->append(QString("Label: %1").arg(QString::fromStdString(node.label)));
            
            // Display statements if available
            if (!node.statements.empty()) {
                ui->reportTextEdit->append("\nStatements:");
                for (const auto& stmt : node.statements) {
                    ui->reportTextEdit->append(QString::fromStdString(stmt));
                }
            }
            
            // Display successors
            if (!node.successors.empty()) {
                ui->reportTextEdit->append("\nConnects to:");
                for (int successor : node.successors) {
                    QString edgeType = m_currentGraph->isExceptionEdge(id, successor) 
                        ? " (exception edge)" 
                        : "";
                    ui->reportTextEdit->append(QString("  -> Node %1%2")
                        .arg(successor)
                        .arg(edgeType));
                }
            }
            
            ui->reportTextEdit->append("------------------");
        }
    }

    if (!found) {
        ui->reportTextEdit->append(QString("Function '%1' not found in CFG").arg(input));
    }
}

void MainWindow::on_fileList_itemClicked(QListWidgetItem *item)
{
    if (item) {
        ui->filePathEdit->setText(item->text());
        on_analyzeButton_clicked();
    }
}

void MainWindow::on_searchButton_clicked()
{
    QString searchText = ui->search->text().trimmed();
    if (!searchText.isEmpty()) {
        // First try to highlight existing nodes
        m_graphView->highlightFunction(searchText);
        
        // Then try to visualize the function if not found
        if (!m_graphView->hasHighlightedItems()) {
            visualizeFunction(searchText);
        }
    }
}

void MainWindow::on_toggleFunctionGraph_clicked()
{
    if (!m_graphView) {
        qWarning() << "Graph view not initialized";
        return;
    }

    static bool showFullGraph = true;
    
    try {
        m_graphView->toggleGraphDisplay(!showFullGraph);
        showFullGraph = !showFullGraph;
        
        ui->toggleFunctionGraph->setText(showFullGraph ? "Show Simplified" : "Show Full Graph");
        
        QTimer::singleShot(100, this, [this]() {
            if (m_graphView && m_graphView->scene()) {
                m_graphView->fitInView(m_graphView->scene()->itemsBoundingRect(), 
                                     Qt::KeepAspectRatio);
            }
        });
    } catch (const std::exception& e) {
        qCritical() << "Failed to toggle graph view:" << e.what();
        QMessageBox::critical(this, "Error", 
                            QString("Failed to toggle view: %1").arg(e.what()));
    }
}

void MainWindow::onLoadJsonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open CFG JSON", 
                                                  "", "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray jsonData = file.readAll();
            file.close();
            
            if (!m_loadedFiles.contains(fileName)) {
                m_loadedFiles.append(fileName);
                ui->fileList->addItem(fileName);
            }
            
            m_graphView->parseJson(jsonData);
            m_graphView->fitInView(m_graphView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
        } else {
            QMessageBox::warning(this, "Error", "Could not open file: " + fileName);
        }
    }
}

void MainWindow::onMergeCfgsClicked() {
    if (m_loadedFiles.size() < 2) {
        QMessageBox::warning(this, "Merge Error", "Need at least 2 CFGs to merge");
        return;
    }
    
    QJsonObject mergedGraph;
    QJsonArray nodes;
    QJsonArray edges;
    
    foreach (const QString &filePath, m_loadedFiles) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();
            
            if (!doc.isNull()) {
                QJsonObject obj = doc.object();
                nodes.append(obj["nodes"].toArray());
                edges.append(obj["edges"].toArray());
            }
        }
    }
    
    mergedGraph["nodes"] = nodes;
    mergedGraph["edges"] = edges;
    
    // Display merged graph
    m_graphView->parseJson(QJsonDocument(mergedGraph).toJson());
    m_graphView->fitInView(m_graphView->scene()->itemsBoundingRect(), 
                         Qt::KeepAspectRatio);
}

void MainWindow::setGraphTheme(int theme)
{
    if (!m_graphView) return;

    struct Theme {
        QColor background;
        QColor node;
        QColor edge;
        QColor text;
    };

    const QVector<Theme> themes = {
        {Qt::white, QColor(240, 240, 240), Qt::black, Qt::black}, // Light
        {QColor(53, 53, 53), QColor(80, 80, 80), Qt::white, Qt::white}, // Dark
        {QColor(240, 248, 255), QColor(173, 216, 230), QColor(0, 0, 139), Qt::black} // Blue
    };

    if (theme < 0 || theme >= themes.size()) return;
    
    m_graphView->setBackgroundBrush(themes[theme].background);
    m_graphView->setThemeColors(themes[theme].node, themes[theme].edge, themes[theme].text);
}

void MainWindow::toggleNodeLabels(bool visible) {
    if (!m_graphView || !m_graphView->scene()) return;
    
    foreach (QGraphicsItem* item, m_graphView->scene()->items()) {
        if (item->data(MainWindow::NodeItemType).toInt() == 1) {
            foreach (QGraphicsItem* child, item->childItems()) {
                if (dynamic_cast<QGraphicsTextItem*>(child)) {
                    child->setVisible(visible);
                }
            }
        }
    }
}

void MainWindow::toggleEdgeLabels(bool visible) {
    if (!m_graphView || !m_graphView->scene()) return;
    
    foreach (QGraphicsItem* item, m_graphView->scene()->items()) {
        if (item->data(MainWindow::EdgeItemType).toInt() == 1) {
            foreach (QGraphicsItem* child, item->childItems()) {
                if (dynamic_cast<QGraphicsTextItem*>(child)) {
                    child->setVisible(visible);
                }
            }
        }
    }
}

void MainWindow::switchLayoutAlgorithm(int index)
{
    if (!m_graphView) return;

    switch(index) {
    case 0: m_graphView->applyHierarchicalLayout(); break;
    case 1: m_graphView->applyForceDirectedLayout(); break;
    case 2: m_graphView->applyCircularLayout(); break;
    default: break;
    }
    
    m_graphView->fitInView(m_graphView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::visualizeFunction(const QString& functionName) 
{
    QString filePath = ui->filePathEdit->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file first");
        return;
    }

    setUiEnabled(false);
    statusBar()->showMessage("Generating CFG for function...");

    QtConcurrent::run([this, filePath, functionName]() {
        try {
            auto cfgGraph = generateFunctionCFG(filePath, functionName);
            QMetaObject::invokeMethod(this, [this, cfgGraph]() {
                handleVisualizationResult(cfgGraph);
            });
        } catch (const std::exception& e) {
            QMetaObject::invokeMethod(this, [this, e]() {
                handleVisualizationError(QString::fromStdString(e.what()));
            });
        }
    });
}

std::shared_ptr<GraphGenerator::CFGGraph> MainWindow::generateFunctionCFG(
    const QString& filePath, const QString& functionName)
{
    try {
        CFGAnalyzer::CFGAnalyzer analyzer;
        auto result = analyzer.analyzeFile(filePath);
        
        if (!result.success) {
            QString detailedError = QString("Failed to analyze file %1:\n%2")
                                  .arg(filePath)
                                  .arg(QString::fromStdString(result.report));
            throw std::runtime_error(detailedError.toStdString());
        }
        
        auto cfgGraph = std::make_shared<GraphGenerator::CFGGraph>();
        
        if (!result.dotOutput.empty()) {
            cfgGraph = parseDotToCFG(QString::fromStdString(result.dotOutput));
            
            if (!functionName.isEmpty()) {
                auto filteredGraph = std::make_shared<GraphGenerator::CFGGraph>();
                const auto& nodes = cfgGraph->getNodes();
                for (const auto& [id, node] : nodes) {
                    if (QString::fromStdString(node.functionName)
                            .compare(functionName, Qt::CaseInsensitive) == 0) {
                        filteredGraph->addNode(id);
                        for (int successor : node.successors) {
                            filteredGraph->addEdge(id, successor);
                        }
                    }
                }
                cfgGraph = filteredGraph;
            }
        }
        
        return cfgGraph;
    }
    catch (const std::exception& e) {
        qCritical() << "Error generating function CFG:" << e.what();
        throw;
    }
}

void MainWindow::connectSignals() {
    connect(ui->analyzeButton, &QPushButton::clicked, this, [this](){
        QString filePath = ui->filePathEdit->text();
        if (!filePath.isEmpty()) {
            std::vector<std::string> sourceFiles = { filePath.toStdString() };
            auto graph = GraphGenerator::generateCFG(sourceFiles);
            m_currentGraph = std::shared_ptr<GraphGenerator::CFGGraph>(graph.release());
            visualizeCurrentGraph();
        }
    });
    
    connect(ui->toggleFunctionGraph, &QPushButton::clicked, this, &MainWindow::toggleVisualizationMode);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::highlightSearchResults);
    
    m_webView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_webView, &QWebEngineView::customContextMenuRequested,
            this, &MainWindow::showNodeContextMenu);
}

void MainWindow::toggleVisualizationMode() {
    static bool showFullGraph = true;
    if (m_graphView) {
        m_graphView->setVisible(showFullGraph);
    }
    if (m_webView) {
        m_webView->setVisible(!showFullGraph);
    }
    showFullGraph = !showFullGraph;
}

void MainWindow::highlightSearchResults() {
    QString searchText = ui->search->text().trimmed();
    if (!searchText.isEmpty()) {
        highlightFunction(searchText);
    }
}

void MainWindow::highlightInCodeEditor(int nodeId) {

    qDebug() << "Highlighting node" << nodeId << "in code editor";
}

void MainWindow::handleVisualizationResult(std::shared_ptr<GraphGenerator::CFGGraph> graph)
{
    if (graph) {
        m_currentGraph = graph;
        visualizeCFG(graph);
    }
    setUiEnabled(true);
    statusBar()->showMessage("Visualization complete", 3000);
}

void MainWindow::handleVisualizationError(const QString& error)
{
    QMessageBox::warning(this, "Visualization Error", error);
    setUiEnabled(true);
    statusBar()->showMessage("Visualization failed", 3000);
}

void MainWindow::onErrorOccurred(const QString& message) {
    ui->reportTextEdit->setPlainText("Error: " + message);
    setUiEnabled(true);
    QMessageBox::critical(this, "Analysis Error", message);
}

void MainWindow::on_openFilesButton_clicked()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Select Source Files");
    if (!filePaths.isEmpty()) {
        ui->fileList->clear();
        for (const QString &path : filePaths) {
            ui->fileList->addItem(path);
        }
    }
}

void MainWindow::setUiEnabled(bool enabled)
{
    QList<QWidget*> widgets = {
        ui->browseButton, ui->analyzeButton, ui->openFilesButton,
        ui->searchButton, ui->toggleFunctionGraph, ui->fileList,
        ui->loadJsonButton, ui->mergeCfgsButton
    };
    
    foreach (QWidget* widget, widgets) {
        widget->setEnabled(enabled);
    }
    
    if (enabled) {
        statusBar()->showMessage("Ready");
    } else {
        statusBar()->showMessage("Processing...");
    }
}

void MainWindow::dumpSceneInfo() {
    if (!m_scene) {
        qDebug() << "Scene: nullptr";
        return;
    }
    
    qDebug() << "=== Scene Info ===";
    qDebug() << "Items count:" << m_scene->items().size();
    qDebug() << "Scene rect:" << m_scene->sceneRect();
    
    if (m_graphView) {
        qDebug() << "View transform:" << m_graphView->transform();
        qDebug() << "View visible items:" << m_graphView->items().size();
    }
}

void MainWindow::verifyScene()
{
    if (!m_scene || !m_graphView) {
        qCritical() << "Invalid scene or view!";
        return;
    }

    if (m_graphView->scene() != m_scene) {
        qCritical() << "Scene/view mismatch!";
        m_graphView->setScene(m_scene);
    }
}

MainWindow::~MainWindow()
{
    if (m_analysisThread && m_analysisThread->isRunning()) {
        m_analysisThread->quit();
        m_analysisThread->wait();
    }

    if (m_scene) {
        m_scene->clear();
        delete m_scene;
        m_scene = nullptr;
    }

    if (m_graphView) {
        if (centralWidget() && centralWidget()->layout()) {
            centralWidget()->layout()->removeWidget(m_graphView);
        }
        delete m_graphView;
        m_graphView = nullptr;
    }
    
    delete ui;
}