#include "respuesta_sii.h"
#include "ui_respuesta_sii.h"
#include "notas.h"
#include <QMessageBox>
#include "basedatos.h"
#include <QProgressDialog>

respuesta_sii::respuesta_sii(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::respuesta_sii)
{
    ui->setupUi(this);
    connect(ui->xml_pushButton,SIGNAL(clicked(bool)),SLOT(verxml()));
    connect(ui->copiar_pushButton,SIGNAL(clicked()),SLOT(copiar()));
    ui->tableWidget->setColumnWidth(5,200);
    recibidas=false;
    pruebas=false;
}

respuesta_sii::~respuesta_sii()
{
    delete ui;
}

void respuesta_sii::pasa_dom(QDomDocument qdom)
{

    QApplication::processEvents();

  doc=qdom;

  // pasamos info a tabla

  // QDomElement docElem = doc.documentElement();

  QDomNodeList lista = doc.elementsByTagName("siiR:RespuestaLinea");

  for (int veces=0; veces< lista.count(); veces++)
   {
     QApplication::processEvents();
     ui->tableWidget->insertRow(veces);
     QDomNode nodo_respuesta=lista.at(veces);
     QDomNode nodo_idfactura=nodo_respuesta.namedItem("siiR:IDFactura");
     if (!nodo_idfactura.isNull())
        {
         QDomNode nodo_idemisor=nodo_idfactura.namedItem("sii:IDEmisorFactura");
         if (!nodo_idemisor.isNull())
           {
            QDomNode nodo_nif=nodo_idemisor.namedItem("sii:NIF");
            if (!nodo_nif.isNull())
               {
                QDomElement e = nodo_nif.toElement();
                if (!e.isNull())
                  {
                    ui->tableWidget->setItem(veces,0,new QTableWidgetItem(e.text()));
                  }
               }
           }
         // num-serie factura emisor
         QDomNode nodo_numserie=nodo_idfactura.namedItem("sii:NumSerieFacturaEmisor");
         if (!nodo_numserie.isNull())
           {
             QDomElement e = nodo_numserie.toElement();
             if (!e.isNull())
               {
                 ui->tableWidget->setItem(veces,1,new QTableWidgetItem(e.text()));
               }
           }
         QDomNode nodo_fecha=nodo_idfactura.namedItem("sii:FechaExpedicionFacturaEmisor");
         if (!nodo_fecha.isNull())
           {
             QDomElement e = nodo_fecha.toElement();
             if (!e.isNull())
               {
                 ui->tableWidget->setItem(veces,2,new QTableWidgetItem(e.text()));
               }
           }
        }
     QDomNode nodo_estado=nodo_respuesta.namedItem("siiR:EstadoRegistro");
     QString estado;
     if (!nodo_estado.isNull())
       {
         QDomElement e = nodo_estado.toElement();
         if (!e.isNull())
           {
             estado=e.text();
             ui->tableWidget->setItem(veces,3,new QTableWidgetItem(e.text()));
           }
       }
     if (estado!="Correcto")
       {
          ui->tableWidget->item(veces,3)->setBackground(Qt::red);
          QDomNode nodo_codigo=nodo_respuesta.namedItem("siiR:CodigoErrorRegistro");
          if (!nodo_codigo.isNull())
            {
              QDomElement e = nodo_codigo.toElement();
              if (!e.isNull())
                {
                  ui->tableWidget->setItem(veces,4,new QTableWidgetItem(e.text()));
                }
            }
          QDomNode nodo_descrip=nodo_respuesta.namedItem("siiR:DescripcionErrorRegistro");
          if (!nodo_descrip.isNull())
            {
              QDomElement e = nodo_descrip.toElement();
              if (!e.isNull())
                {
                  ui->tableWidget->setItem(veces,5,new QTableWidgetItem(e.text()));
                }
            }
       }

     // actualizamos libros de IVA
     if (!pruebas)
      for (int veces=0; veces<ui->tableWidget->rowCount(); veces++)
        {
        // 0 emisor, 1 num/serie, 2 fecha, 3 resultado
        // void basedatos::actu_enviado_sii_fra(QString cod_factura, QDate fechaini, QDate fechafin)
        if (ui->tableWidget->item(veces,3)->text()!="Incorrecto") {
            QDate fecha_fra=QDate::fromString(ui->tableWidget->item(veces,2)->text(),"dd-MM-yyyy");
            basedatos::instancia()->actu_enviado_sii_fra(ui->tableWidget->item(veces,1)->text(),
                                                         fecha_fra, recibidas);
        }

       }

   }

  /*QDomNode n = doc.firstChildElement("siiR:RespuestaLinea");
  while(!n.isNull()) {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull()) {
          cout << qPrintable(e.tagName()) << endl; // the node really is an element.
      }
      n = n.nextSibling();
  }*/

}

void respuesta_sii::verxml()
{
    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->cambia_titulo(tr("RESPUESTA ENVÍO SII"));
    n->esconde_tex_imprimir();
    n->pasa_contenido(doc.toString());
    n->exec();
    delete(n);
}


int respuesta_sii::filas()
{
    return ui->tableWidget->rowCount();
}

QString respuesta_sii::contenido_celda(int fila, int columna)
{
    return ui->tableWidget->item(fila,columna)->text();
}


void respuesta_sii::copiar()
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
    QMessageBox::information( this, tr("RESPUESTA SII"),
                              tr("Se ha pasado el contenido al portapapeles") );
}

void respuesta_sii::pararecibidas() {
    recibidas=true;
}


void respuesta_sii::activa_pruebas() {
   pruebas=true;
}
