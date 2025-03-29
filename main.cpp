
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QDebug>
#include "mapwidget.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MapWidget widget;
    widget.resize(1000, 800);
    widget.show();

    return app.exec();
}
