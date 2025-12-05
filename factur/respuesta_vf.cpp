#include "respuesta_vf.h"
#include "ui_respuesta_vf.h"
#include "notas.h"
#include <QMessageBox>
#include <QProgressDialog>

respuesta_vf::respuesta_vf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::respuesta_vf)
{
    ui->setupUi(this);
    connect(ui->xml_pushButton,SIGNAL(clicked(bool)),SLOT(verxml()));
    connect(ui->copiar_pushButton,SIGNAL(clicked()),SLOT(copiar()));
    ui->tableWidget->setColumnWidth(5,200);
}

respuesta_vf::~respuesta_vf()
{
    delete ui;
}

void respuesta_vf::pasa_dom(QDomDocument qdom)
{

    QApplication::processEvents();

  doc=qdom;

  // pasamos info a tabla

  // QDomElement docElem = doc.documentElement();

  QDomNodeList lista = doc.elementsByTagName("tikR:RespuestaLinea");

  for (int veces=0; veces< lista.count(); veces++)
   {
     QApplication::processEvents();
     ui->tableWidget->insertRow(veces);
     QDomNode nodo_respuesta=lista.at(veces);
     QDomNode nodo_idfactura=nodo_respuesta.namedItem("tikR:IDFactura");
     if (!nodo_idfactura.isNull())
        {         
         QDomNode nodo_idemisor=nodo_idfactura.namedItem("tik:IDEmisorFactura");
         QDomElement e = nodo_idemisor.toElement();
         if (!e.isNull())
         {
             ui->tableWidget->setItem(veces,0,new QTableWidgetItem(e.text()));
         }

         // num-serie factura emisor
         QDomNode nodo_numserie=nodo_idfactura.namedItem("tik:NumSerieFactura");
         if (!nodo_numserie.isNull())
           {
             QDomElement e = nodo_numserie.toElement();
             if (!e.isNull())
               {
                 ui->tableWidget->setItem(veces,1,new QTableWidgetItem(e.text()));
               }
           }
         QDomNode nodo_fecha=nodo_idfactura.namedItem("tik:FechaExpedicionFactura");
         if (!nodo_fecha.isNull())
           {
             QDomElement e = nodo_fecha.toElement();
             if (!e.isNull())
               {
                 ui->tableWidget->setItem(veces,2,new QTableWidgetItem(e.text()));
               }
           }
        }
     QDomNode nodo_estado_registro=nodo_respuesta.namedItem("tikR:EstadoRegistro");
     QDomElement e = nodo_estado_registro.toElement();
     QString estado;
     if (!e.isNull())
       {
         QDomElement e = nodo_estado_registro.toElement();
         if (!e.isNull())
           {
             estado=e.text();
             ui->tableWidget->setItem(veces,3,new QTableWidgetItem(e.text()));
           }
       }
     if (estado!="Correcto")
       {
          if (estado=="Incorrecto") ui->tableWidget->item(veces,3)->setBackground(Qt::red);
          QDomNode nodo_codigo_error=nodo_respuesta.namedItem("tikR:CodigoErrorRegistro");
          if (!nodo_codigo_error.isNull())
            {
              QDomElement e = nodo_codigo_error.toElement();
              if (!e.isNull())
                {
                  ui->tableWidget->setItem(veces,4,new QTableWidgetItem(e.text()));
                }
            }
          QDomNode nodo_descrip=nodo_respuesta.namedItem("tikR:DescripcionErrorRegistro");
          if (!nodo_descrip.isNull())
            {
              QDomElement e = nodo_descrip.toElement();
              if (!e.isNull())
                {
                  ui->tableWidget->setItem(veces,5,new QTableWidgetItem(e.text()));
                }
            }
     }

     if (estado=="Correcto") correcto=true;
     if (estado=="AceptadoConErrores") aceptadoConErrores=true;
   }

}

void respuesta_vf::pasa_dom_consulta_facts(QDomDocument qdom)
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(14);
    QStringList cabeceras;
    cabeceras << tr("Emisor") << tr("NumSerie") << tr("Expedición");
    cabeceras << tr("TipoFactura") << tr("Descripción") << tr("Nombre/Razón");
    cabeceras << tr("NIF") << tr("CuotaTotal") << tr("ImporteTotal");
    cabeceras << tr("Huella") << tr("TimeStamp") << tr("Estado");
    cabeceras << tr("Cod.Error") << tr("Error");
    ui->tableWidget->setHorizontalHeaderLabels(cabeceras);

    QApplication::processEvents();

    doc=qdom;

    // pasamos info a tabla

    // QDomElement docElem = doc.documentElement();

    QDomNodeList lista = doc.elementsByTagName("tikLRRC:RegistroRespuestaConsultaFactuSistemaFacturacion");

    for (int veces=0; veces< lista.count(); veces++)
    {
        QApplication::processEvents();
        ui->tableWidget->insertRow(veces);

        QDomNode nodo_respuesta=lista.at(veces);
        QDomNode nodo_idfactura=nodo_respuesta.namedItem("tikLRRC:IDFactura");
        if (!nodo_idfactura.isNull())
        {
            QDomNode nodo_idemisor=nodo_idfactura.namedItem("tik:IDEmisorFactura");
            QDomElement e = nodo_idemisor.toElement();
            if (!e.isNull())
            {
                ui->tableWidget->setItem(veces,0,new QTableWidgetItem(e.text()));
            }

            // num-serie factura emisor
            QDomNode nodo_numserie=nodo_idfactura.namedItem("tik:NumSerieFactura");
            if (!nodo_numserie.isNull())
            {
                QDomElement e = nodo_numserie.toElement();
                if (!e.isNull())
                {
                    ui->tableWidget->setItem(veces,1,new QTableWidgetItem(e.text()));
                }
            }
            QDomNode nodo_fecha=nodo_idfactura.namedItem("tik:FechaExpedicionFactura");
            if (!nodo_fecha.isNull())
            {
                QDomElement e = nodo_fecha.toElement();
                if (!e.isNull())
                {
                    ui->tableWidget->setItem(veces,2,new QTableWidgetItem(e.text()));
                }
            }
        }

        QDomNode DatosRegistroFacturacion=nodo_respuesta.namedItem("tikLRRC:DatosRegistroFacturacion");
        if (!DatosRegistroFacturacion.isNull()) {
            QDomNode TipoFactura=DatosRegistroFacturacion.namedItem("tikLRRC:TipoFactura");
            QDomElement e = TipoFactura.toElement();
            if (!e.isNull())
                ui->tableWidget->setItem(veces,3,new QTableWidgetItem(e.text()));

            QDomNode DescripcionOperacion=DatosRegistroFacturacion.namedItem("tikLRRC:DescripcionOperacion");
            e = DescripcionOperacion.toElement();
            if (!e.isNull())
                ui->tableWidget->setItem(veces,4,new QTableWidgetItem(e.text()));

            QDomNode Destinatarios=DatosRegistroFacturacion.namedItem("tikLRRC:Destinatarios");
            if (!Destinatarios.isNull()) {
                QDomNode IDDestinatario=Destinatarios.namedItem("tikLRRC:IDDestinatario");
                if (!IDDestinatario.isNull()) {
                    QDomNode NombreRazon=IDDestinatario.namedItem("tik:NombreRazon");
                    QDomElement e = NombreRazon.toElement();
                    if (!e.isNull())
                        ui->tableWidget->setItem(veces,5,new QTableWidgetItem(e.text()));
                    QDomNode NIF=IDDestinatario.namedItem("tik:NIF");
                    e=NIF.toElement();
                    if (!e.isNull())
                        ui->tableWidget->setItem(veces,6,new QTableWidgetItem(e.text()));
                }
            }

            QDomNode CuotaTotal=DatosRegistroFacturacion.namedItem("tikLRRC:CuotaTotal");
            if (!CuotaTotal.isNull()) {
                QDomElement e = CuotaTotal.toElement();
                if (!e.isNull()) {
                    QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui->tableWidget->setItem(veces,7,newItem);
                }
            }

            QDomNode ImporteTotal=DatosRegistroFacturacion.namedItem("tikLRRC:ImporteTotal");
            if (!ImporteTotal.isNull()) {
                QDomElement e=ImporteTotal.toElement();
                if (!e.isNull()) {
                    QTableWidgetItem *newItem = new QTableWidgetItem(e.text());
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui->tableWidget->setItem(veces,8,newItem);
                }
            }

            QDomNode Huella=DatosRegistroFacturacion.namedItem("tikLRRC:Huella");
            if (!Huella.isNull()) {
                QDomElement e=Huella.toElement();
                if (!e.isNull())
                    ui->tableWidget->setItem(veces,9,new QTableWidgetItem(e.text()));
            }
        }

        QDomNode Estado_registro=nodo_respuesta.namedItem("tikLRRC:EstadoRegistro");
        if (!Estado_registro.isNull()) {
            QDomNode Timestamp=Estado_registro.namedItem("tikLRRC:TimestampUltimaModificacion");
            if (!Timestamp.isNull()) {
                QDomElement e=Timestamp.toElement();
                if (!e.isNull())
                    ui->tableWidget->setItem(veces,10,new QTableWidgetItem(e.text()));
            }

            QDomNode EstadoRegistro=Estado_registro.namedItem("tikLRRC:EstadoRegistro");
            if (!EstadoRegistro.isNull()) {
                QDomElement e=EstadoRegistro.toElement();
                if (!e.isNull())
                    ui->tableWidget->setItem(veces,11,new QTableWidgetItem(e.text()));
            }

            QDomNode CodigoErrorRegistro=Estado_registro.namedItem("tikLRRC:CodigoErrorRegistro");
            if (!CodigoErrorRegistro.isNull()) {
                QDomElement e=CodigoErrorRegistro.toElement();
                if (!e.isNull()) {
                    ui->tableWidget->setItem(veces,12,new QTableWidgetItem(e.text()));
                    ui->tableWidget->item(veces,12)->setBackground(Qt::yellow);
                }
            }

            QDomNode DescripcionErrorRegistro=Estado_registro.namedItem("tikLRRC:DescripcionErrorRegistro");
            if (!DescripcionErrorRegistro.isNull()) {
                QDomElement e=DescripcionErrorRegistro.toElement();
                if (!e.isNull()) {
                    ui->tableWidget->setItem(veces,13,new QTableWidgetItem(e.text()));
                    ui->tableWidget->item(veces,13)->setBackground(Qt::yellow);
                }
            }
        }

    }
}


void respuesta_vf::verxml()
{
    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->cambia_titulo(tr("RESPUESTA ENVÍO VERI*FACTU"));
    n->esconde_tex_imprimir();
    n->pasa_contenido(doc.toString());
    n->exec();
    delete(n);
}


int respuesta_vf::filas()
{
    return ui->tableWidget->rowCount();
}


QString respuesta_vf::contenido_celda(int fila, int columna)
{
    return ui->tableWidget->item(fila,columna)->text();
}

bool respuesta_vf::es_correcto()
{
    return correcto;
}

bool respuesta_vf::es_aceptado_con_errores()
{
    return aceptadoConErrores;
}


void respuesta_vf::copiar()
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

