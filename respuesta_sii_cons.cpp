#include "respuesta_sii_cons.h"
#include "ui_respuesta_sii_cons.h"
#include "notas.h"
#include <QMessageBox>
#include "basedatos.h"
#include <QProgressDialog>
#include "funciones.h"

respuesta_sii_cons::respuesta_sii_cons(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::respuesta_sii_cons)
{
    ui->setupUi(this);
    connect(ui->xml_pushButton,SIGNAL(clicked(bool)),SLOT(verxml()));
    connect(ui->copiar_pushButton,SIGNAL(clicked()),SLOT(copiar()));
    ui->tableWidget->setColumnWidth(5,200);
    recibidas=false;
}

respuesta_sii_cons::~respuesta_sii_cons()
{
    delete ui;
}

void respuesta_sii_cons::pasa_dom(QDomDocument qdom)
{
  bool comadecimal=haycomadecimal();
  int decimales=haydecimales();
  QApplication::processEvents();

  doc=qdom;

  // pasamos info a tabla

  // QDomElement docElem = doc.documentElement();

  QDomNodeList lista;
  if (recibidas)
    lista = doc.elementsByTagName("siiLRRC:RegistroRespuestaConsultaLRFacturasRecibidas");
    else
      lista = doc.elementsByTagName("siiLRRC:RegistroRespuestaConsultaLRFacturasEmitidas");


  for (int veces=0; veces< lista.count(); veces++)
   {
     QApplication::processEvents();
     ui->tableWidget->insertRow(veces);
     QDomNode nodo_respuesta=lista.at(veces);
     QDomNode nodo_idfactura=nodo_respuesta.namedItem("siiLRRC:IDFactura"); //aquí nos quedamos
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
                 ui->tableWidget->setItem(veces,3,new QTableWidgetItem(e.text()));
               }
           }
        }
     QDomNode nodo_datos;
     if (recibidas)
       nodo_datos=nodo_respuesta.namedItem("siiLRRC:DatosFacturaRecibida");
      else
         nodo_datos=nodo_respuesta.namedItem("siiLRRC:DatosFacturaEmitida");
     if (!nodo_datos.isNull()) {
         QDomNode nodo_importe_total=nodo_datos.namedItem("siiLRRC:ImporteTotal");
         if (!nodo_importe_total.isNull()) {
            QDomElement e = nodo_importe_total.toElement();
            if (!e.isNull()) {
               ui->tableWidget->setItem(veces,4,new QTableWidgetItem(formateanumero(e.text().toDouble(),comadecimal,decimales)));
               ui->tableWidget->item(veces,4)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            }
         }
         // sigue descripción
         QDomNode nodo_descrip_oper=nodo_datos.namedItem("siiLRRC:DescripcionOperacion");
         if (!nodo_descrip_oper.isNull()) {
            QDomElement e = nodo_descrip_oper.toElement();
            if (!e.isNull())
               ui->tableWidget->setItem(veces,5,new QTableWidgetItem(e.text()));
         }
         QDomNode desglose_factura;
         if (recibidas)
            desglose_factura=nodo_datos.namedItem("siiLRRC:DesgloseFactura");
         else {
             QDomNode tipodesglose=nodo_datos.namedItem("siiLRRC:TipoDesglose");
             if (!tipodesglose.isNull()) desglose_factura=tipodesglose.namedItem("siiLRRC:DesgloseFactura");
         }
         if (!desglose_factura.isNull()) {
            QDomNode desglose_iva;
            if (recibidas)
               desglose_iva=desglose_factura.namedItem("sii:DesgloseIVA");
            else {
                QDomNode sujeta=desglose_factura.namedItem("sii:Sujeta");
                if (!sujeta.isNull()) {
                    QDomNode no_exenta=sujeta.namedItem("sii:NoExenta");
                    desglose_iva=no_exenta.namedItem("sii:DesgloseIVA");
                }
            }

            if (!desglose_iva.isNull()) {
               QDomNodeList lista_IVA=desglose_iva.childNodes(); // = doc.elementsByTagName("sii:DetalleIVA");
               int col_act=6;
               for (int el=0; el< lista_IVA.count(); el++) {
                   QString cad_base;
                   QString cad_cuota;
                   QString cad_tipo;
                   QDomNode nodo_detalle=lista_IVA.at(el);
                   QDomNode nodo_base_imponible=nodo_detalle.namedItem("sii:BaseImponible");
                   if (!nodo_base_imponible.isNull()) {
                       QDomElement e = nodo_base_imponible.toElement();
                       if (!e.isNull()) cad_base=e.text();
                   }
                   QDomNode nodo_cuota= recibidas ? nodo_detalle.namedItem("sii:CuotaSoportada") : nodo_detalle.namedItem("sii:CuotaRepercutida");
                   if (!nodo_cuota.isNull()) {
                       QDomElement e = nodo_cuota.toElement();
                       if (!e.isNull()) cad_cuota=e.text();
                   }
                   QDomNode nodo_tipo=nodo_detalle.namedItem("sii:TipoImpositivo");
                   if (!nodo_tipo.isNull()) {
                       QDomElement e = nodo_tipo.toElement();
                       if (!e.isNull()) cad_tipo=e.text();
                   }
                   if (cad_tipo.isEmpty()) {
                       ui->tableWidget->setItem(veces,15,new QTableWidgetItem(formateanumero(cad_base.toDouble(),comadecimal,decimales)));
                       ui->tableWidget->item(veces,15)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                   }
                   else {
                       ui->tableWidget->setItem(veces,col_act,new QTableWidgetItem(formateanumero(cad_base.toDouble(),comadecimal,decimales)));
                       ui->tableWidget->item(veces,col_act)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                       ui->tableWidget->setItem(veces,col_act+1,new QTableWidgetItem(formateanumero(cad_tipo.toDouble(),comadecimal,decimales)));
                       ui->tableWidget->item(veces,col_act+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                       ui->tableWidget->setItem(veces,col_act+2,new QTableWidgetItem(formateanumero(cad_cuota.toDouble(),comadecimal,decimales)));
                       ui->tableWidget->item(veces,col_act+2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                       col_act+=3;
                   }

               }

            }  // else desglose_iva null
         }
         QDomNode contraparte=nodo_datos.namedItem("siiLRRC:Contraparte");
         if (!contraparte.isNull()) {
            QDomNode nombre_razon=contraparte.namedItem("sii:NombreRazon");
            if (!nombre_razon.isNull()) {
               QDomElement e = nombre_razon.toElement();
               ui->tableWidget->setItem(veces,2,new QTableWidgetItem(e.text()));
            }
         }
     }

     QDomNode estado_factura=nodo_respuesta.namedItem("siiLRRC:EstadoFactura");
     if (!estado_factura.isNull()) {
        QDomNode estado_cuadre=estado_factura.namedItem("siiLRRC:EstadoCuadre");
        if (!estado_cuadre.isNull()) {
           QDomElement e = estado_cuadre.toElement();
           int cod=e.text().toInt();
           QString msj;
           switch (cod) {
           case 1:
               msj=tr("No contrastable");
               break;
           case 2:
               msj=tr("En proceso de contraste");
               break;
           case 3:
               msj=tr("No contrastada");
               break;
           case 4:
               msj=tr("Parcialmente contrastada");
               break;
           case 5:
               msj=tr("Contrastada");
               break;
           default:
               break;
           }
           ui->tableWidget->setItem(veces,16,new QTableWidgetItem(msj));
        }
     }
     QDomNode estado_registro=estado_factura.namedItem("siiLRRC:EstadoRegistro");
     if (!estado_registro.isNull()) {
        QDomElement e = estado_registro.toElement();
        ui->tableWidget->setItem(veces,17,new QTableWidgetItem(e.text()));
     }

  } // bucle for
  if (!recibidas) ui->tableWidget->hideColumn(15);
}

void respuesta_sii_cons::verxml()
{
    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->cambia_titulo(tr("RESPUESTA ENVÍO SII"));
    n->esconde_tex_imprimir();
    n->pasa_contenido(doc.toString());
    n->exec();
    delete(n);
}


int respuesta_sii_cons::filas()
{
    return ui->tableWidget->rowCount();
}

QString respuesta_sii_cons::contenido_celda(int fila, int columna)
{
    return ui->tableWidget->item(fila,columna)->text();
}


void respuesta_sii_cons::copiar()
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

void respuesta_sii_cons::pararecibidas() {
    recibidas=true;
}


