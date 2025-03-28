#ifndef CFG_GUI_H
#define CFG_GUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QMessageBox>
#include <unordered_map>
#include <set>
#include <string>

namespace CFGAnalyzer {

class CFGVisualizerWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit CFGVisualizerWindow(QWidget *parent = nullptr);
    ~CFGVisualizerWindow();

    // Load function dependencies for visualization
    void loadFunctionDependencies(const std::unordered_map<std::string, std::set<std::string>>& dependencies);
    
private slots:
    void browseFile();
    void analyzeFile();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void exportGraph();
    void showAbout();
    void switchLayoutAlgorithm(int index);

private:
    void setupUI();
    void renderDependencyGraph();
    void clearGraph();
    
    // UI Elements
    QWidget* centralWidget;
    QTabWidget* tabWidget;
    
    // File input tab
    QWidget* inputTab;
    QLineEdit* filePathEdit;
    QPushButton* browseButton;
    QPushButton* analyzeButton;
    QTextEdit* outputConsole;
    
    // Visualization tab
    QWidget* visualizationTab;
    QGraphicsScene* scene;
    QGraphicsView* view;
    QComboBox* layoutComboBox;
    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;
    QPushButton* resetZoomButton;
    QPushButton* exportButton;
    
    // Data
    std::unordered_map<std::string, std::set<std::string>> functionDependencies;
    std::string currentFile;
    double zoomFactor;
    int currentLayoutAlgorithm;
    
    // Layout algorithms
    void applyForceDirectedLayout();
    void applyHierarchicalLayout();
    void applyCircularLayout();
};

} // namespace CFGAnalyzer

#endif // CFG_GUI_H