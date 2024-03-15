#include "respuesta_vnifv2.h"
#include "ui_respuesta_vnifv2.h"
#include "notas.h"
#include <QMessageBox>

respuesta_vnifv2::respuesta_vnifv2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::respuesta_vnifv2)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(1,300);
}

respuesta_vnifv2::~respuesta_vnifv2()
{
    delete ui;
}

void respuesta_vnifv2::pasa_dom(QDomDocument qdom) {
    QApplication::processEvents();

  doc=qdom;

  // pasamos info a tabla

  // QDomElement docElem = doc.documentElement();

  QDomNodeList lista = doc.elementsByTagName("VNifV2Sal:Contribuyente");

  for (int veces=0; veces< lista.count(); veces++)
   {
     QApplication::processEvents();
     ui->tableWidget->insertRow(veces);
     QDomNode nodo_respuesta=lista.at(veces);
     QDomNode nodo_nif=nodo_respuesta.namedItem("VNifV2Sal:Nif");
     if (!nodo_nif.isNull())
        {
          QDomElement e = nodo_nif.toElement();
          if (!e.isNull())
             {
                    ui->tableWidget->setItem(veces,0,new QTableWidgetItem(e.text()));
             }
        }
      QDomNode nodo_nombre=nodo_respuesta.namedItem("VNifV2Sal:Nombre");
      if (!nodo_nombre.isNull())
           {
             QDomElement e = nodo_nombre.toElement();
             if (!e.isNull())
               {
                 ui->tableWidget->setItem(veces,1,new QTableWidgetItem(e.text()));
               }
           }
      QDomNode nodo_resultado=nodo_respuesta.namedItem("VNifV2Sal:Resultado");
      if (!nodo_resultado.isNull())
           {
             QDomElement e = nodo_resultado.toElement();
             if (!e.isNull())
               {
                 ui->tableWidget->setItem(veces,2,new QTableWidgetItem(e.text()));
                 if (e.text()!="IDENTIFICADO")
                   ui->tableWidget->item(veces,2)->setBackground(Qt::red);
               }
           }
   }

}

void respuesta_vnifv2::on_xml_pushButton_clicked()
{
    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->cambia_titulo(tr("RESPUESTA ENVÍO SII"));
    n->esconde_tex_imprimir();
    n->pasa_contenido(doc.toString());
    n->exec();
    delete(n);
}

void respuesta_vnifv2::on_copiar_pushButton_clicked()
{
    QClipboard *cb = QApplication::clipboard();
    QString global;
    int numcolumnas=ui->tableWidget->columnCount();
    for (int veces=0; veces<numcolumnas; veces++) {
        global+=ui->tableWidget->horizontalHeaderItem(veces)->text();
        global+= veces<numcolumnas-1 ? "\t" : "\n";
    }

    int numfilas=ui->tableWidget->rowCount();
    for (int fila=0; fila<numfilas; fila++) {
        for (int columna=0; columna<numcolumnas; columna++) {
            global+=ui->tableWidget->item(fila,columna)->text();
            global+= columna<numcolumnas-1 ? "\t" : "\n";
        }
    }

    cb->setText(global);
    QMessageBox::information( this, tr("RESPUESTA VERIFICACIÓN"),
                              tr("Se ha pasado el contenido al portapapeles") );
}
