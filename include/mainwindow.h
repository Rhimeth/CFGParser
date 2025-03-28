#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customgraphview.h"
#include <QMainWindow>
#include <QStringList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void generateCFG();
    void renderDotGraph(const QString& dotGraph);
    void parsePlainFormat(const QString& plainOutput);

private:
    Ui::MainWindow *ui;
    QStringList m_sourceFiles;
};

#endif // MAINWINDOW_H