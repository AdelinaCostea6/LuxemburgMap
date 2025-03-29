#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QVector>
#include <QList>
#include <QPair>
#include <QPointF>

class MapWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<QPointF> scaledNodes;
    QVector<QPair<int, int>> edges;
    QVector<double> costs;
    QVector<QList<QPair<int, double>>> adjacencyList;
    QVector<int> selectedNodes;
    QVector<QPointF> clickPoints;
    QVector<int> shortestPath;
    bool isMapLoaded = false;
    void readMap(QVector<QPointF> &nodes, QVector<QPair<int, int>> &edges, QVector<double> &costs);
    QVector<QList<QPair<int, double>>> buildAdjacencyList(const QVector<QPair<int, int>> &edges, const QVector<double> &costs, int numNodes);
    QVector<int> dijkstraAdjec(const QVector<QList<QPair<int, double>>> &adjList, int source, int target);
    int findClosestNode(const QVector<QPointF> &scaledNodes, const QPointF &clickPosition);
};

#endif // MAPWIDGET_H


