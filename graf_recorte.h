#ifndef GRAF_RECORTE_H
#define GRAF_RECORTE_H

#include <QDialog>
#include <QGraphicsPixmapItem>

namespace Ui {
class graf_recorte;
}

class graf_recorte : public QDialog
{
    Q_OBJECT

public:
    explicit graf_recorte(QString cadfich, qreal w=150, qreal h=100, QWidget *parent = 0);
    ~graf_recorte();
    QPixmap recorte();

private:
    Ui::graf_recorte *ui;
    QGraphicsScene* scene;
    QGraphicsRectItem *rectangle;
    QGraphicsPixmapItem *itempixmap ;
    QPixmap *pixmap;
    qreal ancho_rec,alto_rec;

private slots:
    void scala_recuadro();
};

#endif // GRAF_RECORTE_H
