#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>
#include <QListWidgetItem>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebChannel>
#include <QMenu>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "cfg_analyzer.h"
#include "customgraphview.h"
#include "graph_generator.h"
#include "parser.h"
#include "ui_mainwindow.h"
#include "ast_extractor.h"

namespace Ui {
class MainWindow;
}

struct VisualizationTheme {
    QColor nodeColor;
    QColor edgeColor;
    QColor textColor;
    QColor backgroundColor;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static const int NodeItemType = QGraphicsItem::UserType + 1;
    static const int EdgeItemType = QGraphicsItem::UserType + 2;
    static const int NodeIdKey = QGraphicsItem::UserType + 3;
    static const int EdgeFromKey = QGraphicsItem::UserType + 4;
    static const int EdgeToKey = QGraphicsItem::UserType + 5;

    static const int TryBlockKey = QGraphicsItem::UserType + 3;
    static const int ThrowingExceptionKey = QGraphicsItem::UserType + 4;

    static bool verifyGraphvizInstallation();

    enum LayoutAlgorithm {
        Hierarchical,
        ForceDirected,
        Circular
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void handleAnalysisResult(const CFGAnalyzer::AnalysisResult& result);
    void setupConnections();
    void onAnalysisComplete(CFGAnalyzer::AnalysisResult result);
    void loadAndProcessJson(const QString& filePath);
    void initializeGraphviz();
    void safeInitialize();
    void startTextOnlyMode();
    bool tryInitializeView(bool tryHardware);
    bool testRendering();
    void visualizeCFG(std::shared_ptr<GraphGenerator::CFGGraph> graph);


public slots:
    void handleVisualizationResult(std::shared_ptr<GraphGenerator::CFGGraph> graph);
    void handleVisualizationError(const QString& error);

    void onNodeClicked(const QString& nodeId);
    void onEdgeHovered(const QString& from, const QString& to);
    void onVisualizationError(const QString& error);
    void showVisualizationContextMenu(const QPoint& pos);
    void showEdgeContextMenu(const QPoint& pos);

signals:
    void analysisComplete(const CFGAnalyzer::AnalysisResult& result);

private slots:
    void onDisplayGraphClicked();
    void on_exportButton_clicked();
    void on_browseButton_clicked();
    void on_analyzeButton_clicked();
    void on_openFilesButton_clicked();
    void on_searchButton_clicked();
    void on_toggleFunctionGraph_clicked();
    void on_fileList_itemClicked(QListWidgetItem *item);
    void displayFunctionInfo(const QString& functionName);
    void onParseButtonClicked();
    void onParsingFinished(bool success);
    void onLoadJsonClicked();
    void onMergeCfgsClicked();
    void on_extractAstButton_clicked();
    void exportGraph(const QString& defaultFormat = "png");
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void setGraphTheme(int theme);
    void toggleNodeLabels(bool visible);
    void toggleEdgeLabels(bool visible);
    void connectNodesWithEdge(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to);
    void dumpSceneInfo();
    void verifyScene();
    void addItemToScene(QGraphicsItem* item);
    void switchLayoutAlgorithm(int index);
    void onErrorOccurred(const QString& message);
    void showNodeContextMenu(const QPoint& pos);
    void loadEmptyVisualization();
    void visualizeCurrentGraph();
    void connectSignals();
    void highlightNode(int nodeId, const QColor& color);
    void highlightInCodeEditor(int nodeId);
    void toggleVisualizationMode();
    void highlightSearchResults();
    bool verifyDotFile(const QString& filePath);
    void analyzeDotFile(const QString& filePath);

private:
    Ui::MainWindow *ui;
    QWebEngineView* m_webView;
    QWebChannel* m_webChannel;
    LayoutAlgorithm m_currentLayoutAlgorithm;
    QGraphicsScene* m_scene;
    QThread* m_analysisThread;
    CustomGraphView* m_graphView;
    QStringList m_loadedFiles;
    QSet<QString> m_functionNames;
    Parser m_parser;
    ASTExtractor m_astExtractor;
    VisualizationTheme m_currentTheme;
    QString m_currentDotContent;
    QGraphicsItem* m_highlightNode;
    QGraphicsItem* m_highlightEdge;
    std::shared_ptr<GraphGenerator::CFGGraph> m_currentGraph;

    void createNode();
    void createEdge();
    void setupGraphView();
    void setupWebView();
    void displaySvgInWebView(const QString& dotFilePath);
    void displayGraph(const QString& dotContent);

    QString generateInteractiveGraphHtml(const QString& dotContent);
    void highlightEdge(int fromId, int toId, const QColor& color);
    void resetHighlighting();

    // Existing private functions
    bool renderAndDisplayDot(const QString& dotContent);
    bool renderDotToImage(const QString& dotPath, const QString& outputPath, const QString& format = "");    bool displayImage(const QString& imagePath);
    bool displaySvg(const QString& svgPath);
    QString generateExportHtml() const;
    
    // Add these new declarations
    void showRawDotContent(const QString& dotPath);
    void applyGraphTheme();
    void setupGraphLayout();
    void applyGraphLayout();
    void highlightFunction(const QString& functionName);
    void visualizeFunction(const QString& functionName);
    std::shared_ptr<GraphGenerator::CFGGraph> generateFunctionCFG(const QString& filePath, const QString& functionName);
    void setUiEnabled(bool enabled);
    std::shared_ptr<GraphGenerator::CFGGraph> parseDotToCFG(const QString& dotContent);
    bool displayPngGraph(const QString& pngPath);
    bool displaySvgGraph(const QString& svgPath);
    void setupVisualizationComponents();
};

#endif // MAINWINDOW_H