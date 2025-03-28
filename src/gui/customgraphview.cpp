#include "customgraphview.h"
#include <QDebug>
#include <QJsonArray>
#include <QGraphicsTextItem>
#include <cmath>

CustomGraphView::CustomGraphView(QWidget *parent)
    : QGraphicsView(parent) {
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
}

QGraphicsTextItem* CustomGraphView::createNodeItem(const QString& label, bool isNewFile) {
    QGraphicsTextItem* nodeItem = m_scene->addText(label);
    
    if (isNewFile) {
        nodeItem->setDefaultTextColor(Qt::blue);
        QFont font = nodeItem->font();
        font.setBold(true);
        nodeItem->setFont(font);
    } else {
        nodeItem->setDefaultTextColor(Qt::black);
    }
    
    return nodeItem;
}

void CustomGraphView::addNode(const QString& nodeId, const QString& label, bool isNewFile) {
    qDebug() << "Adding node:" << nodeId << label;
    
    QGraphicsTextItem* nodeItem = createNodeItem(label, isNewFile);

    bool ok;
    int id = nodeId.toInt(&ok);
    if (ok) {
        int x = (id % 5) * 150;
        int y = (id / 5) * 100;
        nodeItem->setPos(x, y);
    }
}

void CustomGraphView::addNode(const std::string& id, const std::string& label) {
    addNode(QString::fromStdString(id), QString::fromStdString(label));
}

void CustomGraphView::addFunctionCallHierarchy(const QJsonObject& functionCalls) {
    qDebug() << "Adding function call hierarchy";
    
    m_scene->clear();
    
    int nodeCounter = 0;
    for (auto it = functionCalls.begin(); it != functionCalls.end(); ++it) {
        addNode(QString::number(nodeCounter), it.key());
        nodeCounter++;

        if (it.value().isArray()) {
            QJsonArray callees = it.value().toArray();
            for (const auto& calleeJson : callees) {
                addNode(QString::number(nodeCounter), calleeJson.toString());
                nodeCounter++;
            }
        }
    }

    m_scene->setSceneRect(m_scene->itemsBoundingRect());
}

void CustomGraphView::highlightFunction(const QString& searchText) {
    qDebug() << "Highlighting function:" << searchText;
    
    foreach (QGraphicsItem* item, m_scene->items()) {
        QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        if (textItem) {
            if (textItem->toPlainText().contains(searchText, Qt::CaseInsensitive)) {
                textItem->setDefaultTextColor(Qt::red);
                QFont font = textItem->font();
                font.setBold(true);
                textItem->setFont(font);
            }
        }
    }
}

void CustomGraphView::clear() {
    qDebug() << "Clearing graph";
    
    if (m_scene) {
        m_scene->clear();
    }
}

void CustomGraphView::addEdge(const QString& from, const QString& to) {
    qDebug() << "Adding edge:" << from << "to" << to;
}

void CustomGraphView::layoutNodes() {
    // Placeholder for more sophisticated node layout
}