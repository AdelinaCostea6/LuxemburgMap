#include "MapWidget.h"
#include <QPainter>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QSet>
#include <QMouseEvent>
#include <queue>
#include <QDir>

MapWidget::MapWidget(QWidget *parent) : QWidget(parent),isMapLoaded(false){
    readMap(scaledNodes, edges, costs);
}

void MapWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    if (scaledNodes.isEmpty() || edges.isEmpty()) {
        qDebug() << "Harta nu este incarcata sau nu exista date suficiente pentru desenare.";
        return;
    }

    painter.setPen(Qt::white);
    for (const auto &edge : edges) {
        if (edge.first >= 0 && edge.first < scaledNodes.size() &&
            edge.second >= 0 && edge.second < scaledNodes.size()) {
            painter.drawLine(scaledNodes[edge.first], scaledNodes[edge.second]);
        } else {
            qWarning() << "Muchie invalida:" << edge;
        }
    }

    painter.setBrush(Qt::yellow);
    for (const auto &node : scaledNodes) {
        painter.drawEllipse(node, 1.5, 1.5);
    }

    painter.setBrush(Qt::red);
    for (int nodeIndex : selectedNodes) {
        if (nodeIndex >= 0 && nodeIndex < scaledNodes.size()) {
            painter.drawEllipse(scaledNodes[nodeIndex], 3, 3);
        } else {
            qWarning() << "Nod selectat invalid:" << nodeIndex;
        }
    }

    if (!shortestPath.isEmpty()) {
        painter.setPen(QPen(Qt::blue, 2));
        for (int i = 0; i < shortestPath.size() - 1; ++i) {
            if (shortestPath[i] >= 0 && shortestPath[i] < scaledNodes.size() &&
                shortestPath[i + 1] >= 0 && shortestPath[i + 1] < scaledNodes.size()) {
                QPointF point1 = scaledNodes[shortestPath[i]];
                QPointF point2 = scaledNodes[shortestPath[i + 1]];
                painter.drawLine(point1, point2);
            } else {
                qWarning() << "Drum minim invalid intre nodurile:" << shortestPath[i] << "si" << shortestPath[i + 1];
            }
        }
    }
}

void MapWidget::readMap(QVector<QPointF> &scaledNodes, QVector<QPair<int, int>> &edges, QVector<double> &costs) {

    if (isMapLoaded) {
        qDebug() << "Fisierul XML a fost deja citit. Continuam celelalte operatiuni";
        return;
    }

    QFile file("Harta_Luxemburg.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Nu s-a putut deschide fisierul!");
        return;
    }
    \

    QXmlStreamReader xml(&file);
    double normalizationFactor = 10000.0;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == QStringLiteral("node")) {
                double latitude = xml.attributes().value("latitude").toDouble();
                double longitude = xml.attributes().value("longitude").toDouble();
                latitude /= normalizationFactor;
                longitude /= normalizationFactor;
                scaledNodes.append(QPointF(latitude, longitude));
            } else if (xml.name() == QStringLiteral("arc")) {
                int source = xml.attributes().value("from").toInt();
                int target = xml.attributes().value("to").toInt();
                double cost = xml.attributes().value("length").toDouble();
                edges.append(qMakePair(source, target));
                costs.append(cost);
            }
        }
    }


    double minLat = 1e9, minLon = 1e9, maxLat = -1e9, maxLon = -1e9;
    for (const auto &node : scaledNodes) {
        minLat = std::min(minLat, node.x());
        minLon = std::min(minLon, node.y());
        maxLat = std::max(maxLat, node.x());
        maxLon = std::max(maxLon, node.y());
    }

    int windowWidth = 1000;
    int windowHeight = 800;

    double scaleX = (windowWidth * 0.95) / (maxLon - minLon);
    double scaleY = (windowHeight * 0.95) / (maxLat - minLat);
    double scale = std::min(scaleX, scaleY);

    double mapCenterLat = (minLat + maxLat) / 2.0;
    double mapCenterLon = (minLon + maxLon) / 2.0;

    double offsetX = windowWidth / 2.0 - (mapCenterLon - minLon) * scale;
    double offsetY = windowHeight / 2.0 - (mapCenterLat - minLat) * scale;

    QVector<QPointF> tempScaledNodes;
    for (const auto &node : scaledNodes) {
        tempScaledNodes.append(QPointF(
            (node.y() - minLon) * scale + offsetX,
            (maxLat - node.x()) * scale + offsetY
            ));
    }
    scaledNodes = tempScaledNodes;


    isMapLoaded = true;
    file.close();

    adjacencyList = buildAdjacencyList(edges, costs, scaledNodes.size());

}



int MapWidget::findClosestNode(const QVector<QPointF> &scaledNodes, const QPointF &clickPosition) {
    int closestNode = -1;
    double minDistanceSquared = std::numeric_limits<double>::max();

    qDebug() << "Click position:" << clickPosition;

    for (int i = 0; i < scaledNodes.size(); ++i) {
        const QPointF &node = scaledNodes[i];

        double dx = node.x() - clickPosition.x();
        double dy = node.y() - clickPosition.y();
        double distanceSquared = dx * dx + dy * dy;


        if (distanceSquared < minDistanceSquared) {
            minDistanceSquared = distanceSquared;
            closestNode = i;
        }
    }

    if (closestNode == -1) {
        qDebug() << "Nu exista niciun nod aproape de locatia selectata.";
    } else {
        qDebug() << "Cel mai apropiat nod este:" << closestNode;

    }

    return closestNode;
}


void MapWidget::mousePressEvent(QMouseEvent *event) {
    QPointF clickPosition = event->pos();
    int closestNode = findClosestNode(scaledNodes, clickPosition);

    if (closestNode == -1) {
        qDebug() << "Nu există niciun nod aproape de locatia selectata.";
        return;
    }

    selectedNodes.append(closestNode);
    clickPoints.append(clickPosition);

    QFile NodesList("NodesList.txt");
    if (!NodesList.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Nu s-a putut deschide fisierul NodesList.txt pentru scriere!");
        return;
    }

    QTextStream nodeList(&NodesList);

    if (selectedNodes.size() == 2) {
        int source = selectedNodes[0];
        int target = selectedNodes[1];

        qDebug() << "Nod sursa:" << source << "Nod destinatie:" << target;
        QVector<QList<QPair<int, double>>> adjList = adjacencyList;
        shortestPath = dijkstraAdjec(adjList, source, target);

        if (!shortestPath.isEmpty()) {

            nodeList << "Drumul minim intre " << source << " si " << target << ":\n";
            for (int node : shortestPath) {
                nodeList << node << " ";
            }
            nodeList << "\n";
        } else {
            qDebug() << "Nu exista drum intre cele două noduri selectate.";
        }

        selectedNodes.clear();
        clickPoints.clear();
    }

    update();
}


QVector<QList<QPair<int, double>>> MapWidget::buildAdjacencyList(const QVector<QPair<int, int>> &edges, const QVector<double> &costs, int numNodes) {

    QVector<QList<QPair<int, double>>> adjacencyList(numNodes);


    for (int i = 0; i < edges.size(); ++i) {
        int u = edges[i].first;
        int v = edges[i].second;
        double cost = costs[i];


        adjacencyList[u].append(qMakePair(v, cost));
    }


    QFile outputFile("AdjacencyList.txt");
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Nu s-a putut deschide fisierul de iesire pentru lista de adiacenta!");
        return adjacencyList;
    }
    QTextStream out(&outputFile);

    for (int i = 0; i < adjacencyList.size(); ++i) {
        out << "Nodul " << i << ":\n";
        for (const auto &neighbor : adjacencyList[i]) {
            out << "  Vecin: " << neighbor.first << ", Cost: " << neighbor.second << "\n";
        }
    }

    outputFile.close();
    return adjacencyList;
}
QVector<int> MapWidget::dijkstraAdjec(const QVector<QList<QPair<int, double>>> &adjList, int source, int target) {
    int n = adjList.size();
    QVector<double> dist(n, std::numeric_limits<double>::max());
    QVector<int> parent(n, -1);
    dist[source] = 0;

    using Node = QPair<double, int>;
    std::priority_queue<Node, QVector<Node>, std::greater<Node>> priorityQueue;

    priorityQueue.push(qMakePair(0.0, source));

    while (!priorityQueue.empty()) {
        Node current = priorityQueue.top();
        priorityQueue.pop();
        int u = current.second;
        double currentDist = current.first;

        if (u == target) break;

        if (currentDist > dist[u]) continue;

        for (const auto &neighbor : adjList[u]) {
            int v = neighbor.first;
            double weight = neighbor.second;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                priorityQueue.push(qMakePair(dist[v], v));

            }
        }
    }


    if (dist[target] == std::numeric_limits<double>::max()) {
        return QVector<int>();
    }

    QVector<int> path;
    for (int v = target; v != -1; v = parent[v]) {
        path.prepend(v);
    }

    return path;
}
