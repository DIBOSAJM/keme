#include "graf_recorte.h"
#include "ui_graf_recorte.h"
#include <QGraphicsPixmapItem>

graf_recorte::graf_recorte(QString cadfich, qreal w, qreal h, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::graf_recorte)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    //scene->setSceneRect(-300, -300, 600, 600);
    pixmap=new QPixmap(cadfich);
    if (pixmap->height()>400) *pixmap=pixmap->scaledToHeight(400);
    //*pixmap=pixmap->scaledToHeight(400);
    //*pixmap=pixmap->scaledToHeight(400);
    itempixmap = new QGraphicsPixmapItem(*pixmap);

    scene->addItem(itempixmap);
    //itempixmap->setPos(0,0);
    //itempixmap->moveBy(50,50);
    QBrush CBrush(QColor(255,0,255,64));
    QPen outlinePen(Qt::black);
    ancho_rec=w; alto_rec=h;
    rectangle = scene->addRect(0, 0, w, h, outlinePen, CBrush);
    rectangle->moveBy(100,100);
    /*QRectF rect=rectangle->rect();
    rectangle->setPos(rect.topLeft());
    rect.moveTo(0.0,0.0);
    rectangle->setRect(rect);*/


    rectangle->setFlag(QGraphicsItem::ItemIsMovable);
    rectangle->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    //rectangle->
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
    /*
    ui->graphicsView->setBackgroundBrush(QPixmap("/home/pepe/Documentos/recorte/recorte/foto.jpg"));
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);*/

    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),SLOT(scala_recuadro()));

}

graf_recorte::~graf_recorte()
{
    delete ui;
}


void graf_recorte::scala_recuadro()
{
 qreal factor=qreal(ui->horizontalSlider->value())/50;
 //QPointF punto = rectangle->scenePos();
 rectangle->setRect(0,0,ancho_rec*factor,alto_rec*factor);
 //rectangle->moveBy(punto.rx(),punto.ry());

}

QPixmap graf_recorte::recorte()
{
   QPointF punto = rectangle->scenePos();

   return pixmap->copy(punto.rx(),punto.ry(),rectangle->rect().width(),rectangle->rect().height());
}

