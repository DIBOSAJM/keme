#ifndef ETIQUETA_FICH_H
#define ETIQUETA_FICH_H


#include <QtGui>
#include <QLabel>

class CustomQLabel : public QLabel
    {
        Q_OBJECT
private:
    QString fichero;
    public:
        CustomQLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
        void dragEnterEvent(QDragEnterEvent *e);
        void dropEvent(QDropEvent *e);
        QString nombre_fich();
        void pasa_nombre_fich(QString qfichero);
        void borra_asignacion();
        void borra_imagen();
    signals:
        void filedroped();
    };



#endif // ETIQUETA_FICH_H
