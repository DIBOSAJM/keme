#include "etiqueta_fich.h"

CustomQLabel::CustomQLabel(QWidget *parent, Qt::WindowFlags f) {
    Q_UNUSED(*parent);
    Q_UNUSED(f);
    setAcceptDrops(true);
    setAutoFillBackground(true);
    QPalette pal=palette();
    pal.setColor(QPalette::Window,QColor(Qt::lightGray));
    setPalette(pal);
}

void CustomQLabel::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void CustomQLabel::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        // qDebug() << "Dropped file:" << fileName;
        //setText(fileName);
        setMaximumHeight(height());
        clear();        
        //setScaledContents(true);
        QPixmap elpixmap;
        if (fileName.endsWith(".pdf"))
          elpixmap=QPixmap(":/iconos/graf/pdf-icon.png");
         else
            elpixmap=QPixmap(":/iconos/graf/Files-New-File-icon.png");
        elpixmap=elpixmap.scaledToHeight(height());
        setPixmap(elpixmap);
        fichero=fileName;
        emit filedroped();
    }
}


QString CustomQLabel::nombre_fich() {
    return fichero;
}

void CustomQLabel::pasa_nombre_fich(QString qfichero) {
    fichero=qfichero;
    setText(fichero);
}

void CustomQLabel::borra_asignacion() {
    fichero.clear();
    setText(tr("Soltar archivo aquí"));
}

void CustomQLabel::borra_imagen() {
    QPixmap foto;
    setPixmap(foto);
    fichero.clear();
    setText(tr("Soltar archivo aquí"));
}
