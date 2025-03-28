#ifndef CUSTOMGRAPHVIEW_H
#define CUSTOMGRAPHVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMap>
#include <QJsonObject>
#include <string>

class CustomGraphView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CustomGraphView(QWidget *parent = nullptr);

    void addNode(const std::string& id, const std::string& label);
    void addNode(const QString& id, const QString& label, bool isNewFile = false);
    void addEdge(const QString& from, const QString& to);
    void highlightFunction(const QString& functionName);
    void addFunctionCallHierarchy(const QJsonObject& functionCalls);
    void clear();

private:
    QGraphicsScene* m_scene;
    QGraphicsTextItem* createNodeItem(const QString& label, bool isNewFile = false);
    void layoutNodes();
};

#endif // CUSTOMGRAPHVIEW_H