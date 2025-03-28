#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

class CFGVisualizer : public QMainWindow {
    Q_OBJECT

public:
    CFGVisualizer(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
    }

private:
    QListWidget *fileList;
    QGraphicsView *graphView;
    QGraphicsScene *graphScene;
    QLabel *statusLabel;

    void setupUI() {
        setWindowTitle("CFG Visualizer");
        resize(1200, 800);

        // Main layout
        QWidget *centralWidget = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

        // Left panel for file list
        QVBoxLayout *leftPanel = new QVBoxLayout();
        fileList = new QListWidget();
        QPushButton *loadButton = new QPushButton("Load JSON");
        QPushButton *mergeButton = new QPushButton("Merge CFGs");
        
        leftPanel->addWidget(new QLabel("Loaded CFG Files:"));
        leftPanel->addWidget(fileList);
        leftPanel->addWidget(loadButton);
        leftPanel->addWidget(mergeButton);

        // Graph view
        graphView = new QGraphicsView();
        graphScene = new QGraphicsScene(this);
        graphView->setScene(graphScene);
        graphView->setRenderHint(QPainter::Antialiasing);

        // Status bar
        statusLabel = new QLabel("Ready");

        // Add layouts
        mainLayout->addLayout(leftPanel, 1);
        mainLayout->addWidget(graphView, 3);

        setCentralWidget(centralWidget);

        // Connect signals
        connect(loadButton, &QPushButton::clicked, this, &CFGVisualizer::loadCFGFile);
        connect(mergeButton, &QPushButton::clicked, this, &CFGVisualizer::mergeCFGs);
        connect(fileList, &QListWidget::itemSelectionChanged, this, &CFGVisualizer::visualizeCFG);
    }

    void loadCFGFile() {
        QString filename = QFileDialog::getOpenFileName(this, 
            "Load CFG JSON", "", "JSON Files (*.json)");
        
        if (filename.isEmpty()) return;

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error", "Could not open file");
            return;
        }

        QByteArray fileContent = file.readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent);
        
        if (jsonDoc.isNull()) {
            QMessageBox::warning(this, "Error", "Invalid JSON file");
            return;
        }

        fileList->addItem(QFileInfo(filename).fileName());
        fileList->item(fileList->count() - 1)->setData(
            Qt::UserRole, QVariant::fromValue(jsonDoc)
        );

        statusLabel->setText(QString("Loaded: %1").arg(filename));
    }

    void visualizeCFG() {
        // Clear previous visualization
        graphScene->clear();

        // Get selected JSON
        QListWidgetItem *selectedItem = fileList->currentItem();
        if (!selectedItem) return;

        QJsonDocument jsonDoc = selectedItem->data(Qt::UserRole).value<QJsonDocument>();
        QJsonObject jsonObj = jsonDoc.object();
        QJsonArray cfgArray = jsonObj["cfg"].toArray();

        // Basic graph layout parameters
        const int NODE_WIDTH = 150;
        const int NODE_HEIGHT = 100;
        const int VERTICAL_SPACING = 150;
        const int HORIZONTAL_SPACING = 200;

        // First pass: create nodes
        QMap<int, QGraphicsRectItem*> nodeMap;
        for (const QJsonValue &nodeVal : cfgArray) {
            QJsonObject nodeObj = nodeVal.toObject();
            int nodeId = nodeObj["id"].toInt();
            QString label = nodeObj["label"].toString();

            QGraphicsRectItem *nodeRect = graphScene->addRect(
                0, 0, NODE_WIDTH, NODE_HEIGHT, 
                QPen(Qt::black), 
                QBrush(nodeObj["is_throwing"].toBool() ? Qt::red : 
                       nodeObj["is_try_block"].toBool() ? Qt::blue : Qt::lightGray)
            );

            // Add text to node
            QGraphicsTextItem *textItem = graphScene->addText(label);
            textItem->setParentItem(nodeRect);
            textItem->setPos(10, 10);

            // Position nodes in a grid-like layout
            int row = nodeId / 5;
            int col = nodeId % 5;
            nodeRect->setPos(col * HORIZONTAL_SPACING, row * VERTICAL_SPACING);

            nodeMap[nodeId] = nodeRect;
        }

        // Second pass: create edges
        for (const QJsonValue &nodeVal : cfgArray) {
            QJsonObject nodeObj = nodeVal.toObject();
            int nodeId = nodeObj["id"].toInt();
            QJsonArray successors = nodeObj["successors"].toArray();

            QGraphicsRectItem *sourceNode = nodeMap[nodeId];
            for (const QJsonValue &succVal : successors) {
                int succId = succVal.toInt();
                QGraphicsRectItem *destNode = nodeMap[succId];

                // Draw line between nodes
                graphScene->addLine(
                    sourceNode->pos().x() + NODE_WIDTH/2, 
                    sourceNode->pos().y() + NODE_HEIGHT,
                    destNode->pos().x() + NODE_WIDTH/2, 
                    destNode->pos().y()
                );
            }
        }

        // Fit the view to scene
        graphView->fitInView(graphScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }

    void mergeCFGs() {
        QStringList filenames = QFileDialog::getOpenFileNames(this, 
            "Select CFG JSON files to merge", "", "JSON Files (*.json)");
        
        if (filenames.isEmpty()) return;

        // Here you would call your C++ mergeCFGs function
        // For now, we'll just simulate merging by combining file contents
        QJsonArray mergedCfg;
        QJsonArray mergedAst;

        for (const QString &filename : filenames) {
            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

            QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
            QJsonObject jsonObj = jsonDoc.object();

            // Merge CFG and AST arrays
            mergedCfg.append(jsonObj["cfg"].toArray());
            mergedAst.append(jsonObj["ast"].toArray());
        }

        // Create merged JSON
        QJsonObject mergedJson;
        mergedJson["cfg"] = mergedCfg;
        mergedJson["ast"] = mergedAst;

        // Save merged file
        QString outputFile = QFileDialog::getSaveFileName(this, 
            "Save Merged CFG", "", "JSON Files (*.json)");
        
        if (!outputFile.isEmpty()) {
            QFile saveFile(outputFile);
            if (saveFile.open(QIODevice::WriteOnly)) {
                saveFile.write(QJsonDocument(mergedJson).toJson());
                QMessageBox::information(this, "Success", "CFGs merged successfully!");
            }
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CFGVisualizer window;
    window.show();
    return app.exec();
}

#include "main.moc"  // For Qt's meta-object compiler