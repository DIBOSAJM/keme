/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#include "conta347.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include <QRadioButton>
#include "datos_accesorios.h"
#include "consmayor.h"
#include "directorio.h"
#include "pidefecha.h"
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "externos.h"

conta347::conta347(QString qusuario) : QDialog() {
    ui.setupUi(this);


if (conigic()) setWindowTitle(tr("Relación de operaciones con terceras personas según libros IGIC"));


QString empresa, nif, nombre, apellidos;
basedatos::instancia()->empresa_nif_nombre_apellidos(&empresa, &nif, &nombre, &apellidos);

ui.ciflineEdit->setText(nif);
ui.nombrelineEdit->setText(empresa);
if (!apellidos.isEmpty()) {
    ui.nombrelineEdit->setText(apellidos + " " + nombre);
}

//ui.ciflineEdit->setText(basedatos::instancia()->cif());
//ui.nombrelineEdit->setText(nombreempresa());

 comadecimal=haycomadecimal(); decimales=haydecimales();

 usuario=qusuario;

 QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
 QStringList ej1;

 if ( query.isActive() ) {
         while ( query.next() )
               ej1 << query.value(0).toString();
         }
 ui.ejerciciocomboBox->addItems(ej1);

    QString ejercicio;
    ejercicio=ejercicio_igual_o_maxigual(QDate::currentDate());
    if (ejercicio.length()>0)
        {
         ui.inicialdateEdit->setDate(inicioejercicio(ejercicio));
         ui.finaldateEdit->setDate(finejercicio(ejercicio));
        }
        else
             {
              ui.inicialdateEdit->setDate(QDate::currentDate());
              ui.finaldateEdit->setDate(QDate::currentDate());
             }

  int item=0;
  for (int veces=0; veces<ui.ejerciciocomboBox->count(); veces++)
    {
     if (ui.ejerciciocomboBox->itemText(veces)==ejercicio) item=veces;
     break;
    }

  ui.ejerciciocomboBox->setCurrentIndex(item);

 QStringList columnas;
 columnas << tr("CUENTA") << tr("IMPORTE") << tr("1T") << tr("2T") << tr("3T") << tr("4T");
 columnas << tr("RAZÓN");
 columnas << tr("CIF/NIF") << tr("POBLACION") << tr("CP") << tr("PROVINCIA");
 columnas << tr("NACIÓN") << tr("TIPO");
 columnas << tr("EXTERNO");

 ui.latconta347->setHorizontalHeaderLabels(columnas);

  ui.latconta347->setColumnWidth(6,200);
  ui.latconta347->setColumnWidth(9,80);

  //ui.latconta347->hideColumn(11);

  // Fijamos fechas por defecto para inicial y final
  if (ejerciciodelafecha(QDate::currentDate()).length()>0)
     {
       QString ejercicio=ejerciciodelafecha(QDate::currentDate());
       ui.inicialdateEdit->setDate(inicioejercicio(ejercicio));
       ui.finaldateEdit->setDate(finejercicio(ejercicio));
     }
  // ui.latconta347->setEditTriggers ( QAbstractItemView::NoEditTriggers );

  connect(ui.cargapushButton,SIGNAL( clicked()),this,SLOT(cargadatos()));

  connect(ui.importelineEdit,SIGNAL(textChanged(QString)),this,SLOT(borratabla()));
  connect(ui.importelineEdit,SIGNAL(editingFinished()),this,SLOT(importefinedic()));

  connect(ui.inicialdateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(borratabla()));
  connect(ui.finaldateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(borratabla()));

  connect(ui.imprimepushButton,SIGNAL( clicked()),this,SLOT(imprime()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

  connect(ui.datospushButton,SIGNAL(clicked()),SLOT(datoscuenta()));
  connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borralinea()));

  connect(ui.ejerciciocomboBox, SIGNAL(currentIndexChanged(int)), SLOT(combocambiado()));

  connect(ui.mayorpushButton, SIGNAL(clicked()), SLOT(consultamayor()));

  connect(ui.gen347pushButton, SIGNAL(clicked()), SLOT(genera347()));

  connect(ui.copiarpushButton, SIGNAL(clicked()), SLOT(copiar()));

  connect(ui.comunicpushButton, SIGNAL(clicked()), SLOT(gen_cartas_347_latex()));

  connect(ui.csvpushButton,SIGNAL(clicked(bool)),SLOT(csv_import()));

#ifdef NOEDITTEX
 ui.latexpushButton->hide();
#endif

 if (basedatos::instancia()->hay_externos())
   ui.agrupcifcheckBox->hide();

}

void conta347::borratabla()
{
  ui.latconta347->setRowCount(0);
}


void conta347::cargadatos()
{
   borratabla();

   QProgressDialog progreso(tr("Cargando información ... ")
                            , 0, 0, 0);
   progreso.setWindowModality(Qt::WindowModal);
   progreso.setMinimumDuration ( 0 );
   progreso.show();
   progreso.raise();
   progreso.update();
   QApplication::processEvents();

   QString cadimporte=ui.importelineEdit->text();
   if (!cadnumvalido(convapunto(cadimporte)) || cadimporte.length()==0) cadimporte="0";
   cadimporte=convapunto(cadimporte);

   // QDate inicial, QDate final,
   // bool aib, bool eib, bool soportado, bool repercutido,
   // bool autofacturaue, bool autofacturanoue,
   // bool prestserviciosue

   if (ui.agrupcifcheckBox->isChecked())
      {
       carga_agrup_nif();
       return;
      }

    QSqlQuery consulta = basedatos::instancia()->datos347(
            ui.inicialdateEdit->date() , ui.finaldateEdit->date(), ui.cuentaslineEdit->text());

    int fila=0;
   while (consulta.next())
      {
         if (fabs(consulta.value(1).toDouble())-0.0001>=cadimporte.toDouble())
            {
              QString externo=consulta.value(3).toString().trimmed();

              QSqlQuery consulta2;
              if (externo.isEmpty())
                consulta2 = basedatos::instancia()->select7Datossubcuentacuenta( consulta.value(0).toString() );
               else
                  consulta2=basedatos::instancia()->select7datos_externo(externo);
              bool hayconsultadatos=false;
              if ( (consulta2.isActive()) && (consulta2.next()) ) hayconsultadatos=true;

              if (ui.nacionales_checkBox->isChecked())
                  if (hayconsultadatos && consulta2.value(6).toString().toUpper()!="ES") continue;

              ui.latconta347->insertRow(fila);
              // cuenta
              ui.latconta347->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
              // EXTERNO
              ui.latconta347->setItem(fila,13,new QTableWidgetItem(consulta.value(3).toString()));
              // volumen operación
              QString cadvolumen=formateanumero(consulta.value(1).toDouble(),comadecimal,decimales);
              if (consulta.value(4).toBool()) // isp_op_interiores
                  cadvolumen=formateanumero(consulta.value(5).toDouble(),comadecimal,decimales); // solo base
              QTableWidgetItem *newItem1 = new QTableWidgetItem(cadvolumen);
              newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.latconta347->setItem(fila,1,newItem1);

              QString operacion;
              operacion = consulta.value(2).toBool() ? "A" : "B";
              QTableWidgetItem *newItem111 = new QTableWidgetItem(operacion);
              newItem111->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
              ui.latconta347->setItem(fila,12,newItem111);


                  // averiguamos para la cuenta la info trimestral
                  int anyo=ui.inicialdateEdit->date().year();
                  QString cuenta=consulta.value(0).toString();
                  bool de_soportado=consulta.value(2).toBool();
                  bool de_isp_op_interiores=consulta.value(4).toBool();
                  QDate inicial, final;
                  inicial.setDate(anyo,1,1);
                  final.setDate(anyo,3,31);
                  double t1=basedatos::instancia()->vol_op_347_cuenta (cuenta,
                      inicial , final, externo, de_soportado, de_isp_op_interiores );
                  QTableWidgetItem *newItem1T = new QTableWidgetItem(
                                       formateanumero(t1,comadecimal,decimales));
                  newItem1T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,2,newItem1T);

                  inicial.setDate(anyo,4,1);
                  final.setDate(anyo,6,30);
                  double t2=basedatos::instancia()->vol_op_347_cuenta (cuenta,
                      inicial , final, externo, de_soportado, de_isp_op_interiores );
                  QTableWidgetItem *newItem2T = new QTableWidgetItem(
                                       formateanumero(t2,comadecimal,decimales));
                  newItem2T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,3,newItem2T);

                  inicial.setDate(anyo,7,1);
                  final.setDate(anyo,9,30);
                  double t3=basedatos::instancia()->vol_op_347_cuenta (cuenta,
                      inicial , final, externo, de_soportado, de_isp_op_interiores );
                  QTableWidgetItem *newItem3T = new QTableWidgetItem(
                                       formateanumero(t3,comadecimal,decimales));
                  newItem3T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,4,newItem3T);

                  inicial.setDate(anyo,10,1);
                  final.setDate(anyo,12,31);
                  double t4=basedatos::instancia()->vol_op_347_cuenta (cuenta,
                      inicial , final, externo, de_soportado, de_isp_op_interiores );
                  QTableWidgetItem *newItem4T = new QTableWidgetItem(
                                       formateanumero(t4,comadecimal,decimales));
                  newItem4T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,5,newItem4T);

              // Datos accesorios

              if ( hayconsultadatos )
              {
                QString identif=consulta2.value(0).toString();
                if (!externo.isEmpty())
                   if (!consulta2.value(8).toString().isEmpty() &&
                        !consulta2.value(9).toString().isEmpty())
                    identif= consulta2.value(8).toString()+ " " + consulta2.value(9).toString();

                ui.latconta347->setItem(fila,6,new QTableWidgetItem(identif));
                ui.latconta347->setItem(fila,7,new QTableWidgetItem(consulta2.value(1).toString()));
                ui.latconta347->setItem(fila,8,new QTableWidgetItem(consulta2.value(2).toString()));
                ui.latconta347->setItem(fila,9,new QTableWidgetItem(consulta2.value(3).toString()));
                ui.latconta347->setItem(fila,10,new QTableWidgetItem(consulta2.value(4).toString()));
                ui.latconta347->setItem(fila,11,new QTableWidgetItem(consulta2.value(6).toString()));
              }
               else
                    {
                      ui.latconta347->setItem(fila,6,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,7,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,8,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,9,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,10,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,11,new QTableWidgetItem(""));
                    }
              fila++;
              QApplication::processEvents();
            }
      }
   // progreso.reset();
}

void conta347::carga_agrup_nif()
{
    QString cadimporte=ui.importelineEdit->text();
    if (!cadnumvalido(convapunto(cadimporte)) || cadimporte.length()==0) cadimporte="0";
    cadimporte=convapunto(cadimporte);

    QSqlQuery consulta = basedatos::instancia()->datos347_agrup_nif(
            ui.inicialdateEdit->date() , ui.finaldateEdit->date(), ui.cuentaslineEdit->text());

    int fila=0;
   while (consulta.next())
      {
         if (consulta.value(1).toDouble()-0.0001>=cadimporte.toDouble())
            {
              ui.latconta347->insertRow(fila);
              // cuenta
              ui.latconta347->setItem(fila,0,new QTableWidgetItem(""));
              // nif
              ui.latconta347->setItem(fila,7,new QTableWidgetItem(consulta.value(0).toString()));
              // volumen operación
              QTableWidgetItem *newItem1 = new QTableWidgetItem(
                                   formateanumero(consulta.value(1).toDouble(),comadecimal,decimales));
              newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.latconta347->setItem(fila,1,newItem1);

              QString operacion;
              operacion = consulta.value(2).toBool() ? "A" : "B";
              QTableWidgetItem *newItem111 = new QTableWidgetItem(operacion);
              newItem111->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
              ui.latconta347->setItem(fila,12,newItem111);


                  // averiguamos para la cuenta la info trimestral
                  int anyo=ui.inicialdateEdit->date().year();
                  QString nif=ui.latconta347->item(fila,7)->text();
                  QDate inicial, final;
                  inicial.setDate(anyo,1,1);
                  final.setDate(anyo,3,31);
                  double t1=basedatos::instancia()->vol_op_347_nif (nif,
                      inicial , final );
                  QTableWidgetItem *newItem1T = new QTableWidgetItem(
                                       formateanumero(t1,comadecimal,decimales));
                  newItem1T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,2,newItem1T);

                  inicial.setDate(anyo,4,1);
                  final.setDate(anyo,6,30);
                  double t2=basedatos::instancia()->vol_op_347_nif (nif,
                      inicial , final );
                  QTableWidgetItem *newItem2T = new QTableWidgetItem(
                                       formateanumero(t2,comadecimal,decimales));
                  newItem2T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,3,newItem2T);

                  inicial.setDate(anyo,7,1);
                  final.setDate(anyo,9,30);
                  double t3=basedatos::instancia()->vol_op_347_nif (nif,
                      inicial , final );
                  QTableWidgetItem *newItem3T = new QTableWidgetItem(
                                       formateanumero(t3,comadecimal,decimales));
                  newItem3T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,4,newItem3T);

                  inicial.setDate(anyo,10,1);
                  final.setDate(anyo,12,31);
                  double t4=basedatos::instancia()->vol_op_347_nif (nif,
                      inicial , final );
                  QTableWidgetItem *newItem4T = new QTableWidgetItem(
                                       formateanumero(t4,comadecimal,decimales));
                  newItem4T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latconta347->setItem(fila,5,newItem4T);
              // Datos accesorios
              QString cuenta=basedatos::instancia()->selectCuentaCifDatossubcuenta (nif);
              QSqlQuery consulta2 = basedatos::instancia()->select7Datossubcuentacuenta( cuenta );
              if ( (consulta2.isActive()) && (consulta2.next()) )
              {
                ui.latconta347->setItem(fila,6,new QTableWidgetItem(consulta2.value(0).toString()));
                ui.latconta347->setItem(fila,7,new QTableWidgetItem(consulta2.value(1).toString()));
                ui.latconta347->setItem(fila,8,new QTableWidgetItem(consulta2.value(2).toString()));
                ui.latconta347->setItem(fila,9,new QTableWidgetItem(consulta2.value(3).toString()));
                ui.latconta347->setItem(fila,10,new QTableWidgetItem(consulta2.value(4).toString()));
                ui.latconta347->setItem(fila,11,new QTableWidgetItem(consulta2.value(6).toString()));
              }
               else
                    {
                      ui.latconta347->setItem(fila,6,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,7,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,8,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,9,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,10,new QTableWidgetItem(""));
                      ui.latconta347->setItem(fila,11,new QTableWidgetItem(""));
                    }
              fila++;
              QApplication::processEvents();

            }
      }

}

void conta347::importefinedic()
{
  ui.importelineEdit->setText(formateanumero(convapunto(ui.importelineEdit->text()).toDouble(),
                                             comadecimal,decimales));
  borratabla();
}

void conta347::gen_cartas_347_latex()
{
    pidefecha *p = new pidefecha();
    p->pasafecha(QDate::currentDate());
    p->esconde_concepto_documento();
    p->asignaetiqueta(tr("Fecha de la comunicación"));
    p->exec();
    QDate fecha=p->fecha();
    delete(p);
    bool desglose347=true;
    QString cabecera347, cuerpo347, cuerpo2_347, pie347, saludo347, firma347;
    QSqlQuery q=basedatos::instancia()->datos_carta347();
    // cabecera347, cuerpo347, desglose347, cuerpo2_347, pie347, saludo347, firma347
    if (q.isActive())
        if (q.next())
          {
            cabecera347=q.value(0).toString();
            cuerpo347=q.value(1).toString();
            desglose347=q.value(2).toBool();
            cuerpo2_347=q.value(3).toString();
            pie347=q.value(4).toString();
            saludo347=q.value(5).toString();
            firma347=q.value(6).toString();
          }

    QString qfichero=dirtrabajo();
    qfichero.append(QDir::separator());
    qfichero=qfichero+tr("comunicaciones_347.tex");
    QFile fichero(adapta(qfichero));
     if (! fichero.open( QIODevice::WriteOnly ) ) return;

     QTextStream stream( &fichero );
     stream.setEncoding(QStringConverter::Utf8);

     if (es_os_x()) stream << soporte_xelatex();
     
     stream << "\\documentclass{letter}\n";
     
     if (es_os_x()) stream << config_font_xelatex();
        else
           {         
            stream << "\\usepackage{ucs}\n";
            stream << "\\usepackage[utf8x]{inputenc}\n";
           }
     stream << "\\usepackage[spanish]{babel}\n\n";

     stream << "\\addtolength{\\voffset}{-0.8in}\n";
     stream << "\\addtolength{\\hoffset}{-0.3in}\n";
     stream << "\\addtolength{\\textheight}{2cm}\n";
     stream << "\\date{";
     stream << fecha.toString("dd") + " de " + fecha.toString("MMMM") + " de " + fecha.toString("yyyy");
     stream << "}\n";
     stream << "\\signature{";
     stream << filtracad(firma347);
     stream << "}\n";

     /*
     stream << "\\address{\\textbf{";
     stream << filtracad(nombreempresa());
     stream << "} \\\\ ";
     stream << filtracad(basedatos::instancia()->domicilio());
     stream << " \\\\ ";
     stream << filtracad(basedatos::instancia()->cpostal());
     stream << " ";
     stream << filtracad(basedatos::instancia()->ciudad());
     stream << " \\\\ ";
     stream << filtracad(basedatos::instancia()->provincia());
     stream << "}\n\n";
     */

     stream << "\\begin{document}\n\n";

     QProgressDialog progreso(tr("Procesando información ... ")
                              , 0, 0, 0);
     // progreso.setWindowModality(Qt::WindowModal);
     progreso.setMinimumDuration ( 0 );
     progreso.setMaximum(ui.latconta347->rowCount());
     QApplication::processEvents();

     int fila=0;

     while (fila<ui.latconta347->rowCount())
           {
            progreso.setValue(fila);
            QApplication::processEvents();
            // -------------------------------------------------------------
            QString razon=((!ui.latconta347->item(fila,6)->text().isEmpty()) ? filtracad(ui.latconta347->item(fila,6)->text()) : "-");
            stream << (razon.length()<30 ? "\\address{\\large " : "\\address{ ");
            QString razon1=razon;
            QString razon2;
            if (razon.length()>27)
              {
                int pos=27;
                while (razon.at(pos)!=' ' && pos>0) pos--;
                if (pos>0)
                   {
                    razon1=razon.left(pos+1);
                    razon2=razon.mid(pos);
                   }
              }

            stream << razon1;
            if (!razon2.isEmpty())
              {
                stream << " \\\\ ";
                stream << razon2;
              }
            // stream << "";
            QString domicilio;
            QSqlQuery q = basedatos::instancia()->select7Datossubcuentacuenta( ui.latconta347->item(fila,0)->text() );
            if (q.isActive())
                if (q.next()) domicilio=q.value(7).toString();
            if (!domicilio.isEmpty()) stream << " \\\\ ";
            //
            QString linea1=domicilio;
            QString linea2;
            if (domicilio.length()>27)
              {
                int pos=27;
                while (domicilio.at(pos)!=' ' && pos>0) pos--;
                if (pos>0)
                   {
                    linea1=domicilio.left(pos+1);
                    linea2=domicilio.mid(pos);
                   }
              }
            stream << filtracad(linea1);
            if (!linea2.isEmpty())
              {
                stream << " \\\\ ";
                stream << linea2;
              }
            //

            if (!ui.latconta347->item(fila, 9)->text().isEmpty()) stream << " \\\\ ";
            stream << filtracad(ui.latconta347->item(fila, 9)->text()) << " ";
            stream << filtracad(ui.latconta347->item(fila,8)->text()) << " - ";
            stream << filtracad(ui.latconta347->item(fila,10)->text());
            // stream << "}"; // línea añadida
            /*
            stream << filtracad(nombreempresa());
            stream << "} \\\\ ";
            stream << filtracad(basedatos::instancia()->domicilio());
            stream << " \\\\ ";
            stream << filtracad(basedatos::instancia()->cpostal());
            stream << " ";
            stream << filtracad(basedatos::instancia()->ciudad());
            stream << " \\\\ ";
            stream << filtracad(basedatos::instancia()->provincia());
            */
            stream << " \\\\ \\\\ \\\\";
            stream << "}\n\n";
            // --------------------------------------------------------------
            // \begin{letter}{Director \\ Corporation \\ 123 Pleasant Lane \\ City, State 12345} % Name/title of the addressee
            stream << "\\begin{letter}{";

            stream << filtracad(nombreempresa());
            stream << " \\\\ ";
            // stream << filtracad(basedatos::instancia()->domicilio());
            // stream << " \\\\ ";
            stream << filtracad(basedatos::instancia()->cpostal());
            stream << " ";
            stream << filtracad(basedatos::instancia()->ciudad());
            stream << " \\\\ ";
            stream << filtracad(basedatos::instancia()->provincia());
            /*
            stream << ((!ui.latconta347->item(fila,6)->text().isEmpty()) ? filtracad(ui.latconta347->item(fila,6)->text()) : "-");
            QString domicilio;
            QSqlQuery q = basedatos::instancia()->select7Datossubcuentacuenta( ui.latconta347->item(fila,0)->text() );
            if (q.isActive())
                if (q.next()) domicilio=q.value(7).toString();
            if (!domicilio.isEmpty()) stream << " \\\\ ";
            stream << filtracad(domicilio);
            if (!ui.latconta347->item(fila, 9)->text().isEmpty()) stream << " \\\\ ";
            stream << filtracad(ui.latconta347->item(fila, 9)->text()) << " ";
            stream << filtracad(ui.latconta347->item(fila,8)->text());
            // stream << filtracad(ui.latconta347->item(fila,10)->text());
            */

            stream << "}\n\n";

            stream << "\\opening{\\textbf{";
            stream << filtracad(cabecera347);
            stream << "}}\n\n";
            QString cifra=formateanumerosep(convapunto(ui.latconta347->item(fila,1)->text()).toDouble(),comadecimal,decimales);
            QString cuerpo347ed=cuerpo347;
            stream << cuerpo347ed.replace("{cifra}", cifra).replace("{nif}",ui.latconta347->item(fila,7)->text()) << "\n\n";

            if (desglose347)
               {
                 stream << "\\begin{center}\n";
                 stream << "\\begin{tabular}{|l|r|}\n";
                 stream << "\\hline\n";
                 stream << "{\\scriptsize ";
                 stream << tr("Trimestre 1") << "} & {\\scriptsize " << ui.latconta347->item(fila,2)->text() << "}\\\\\n";
                 stream << "\\hline\n";
                 stream << "{\\scriptsize ";
                 stream << tr("Trimestre 2") << "} & {\\scriptsize " << ui.latconta347->item(fila,3)->text() << "}\\\\\n";
                 stream << "\\hline\n";
                 stream << "{\\scriptsize ";
                 stream << tr("Trimestre 3") << "} & {\\scriptsize " << ui.latconta347->item(fila,4)->text() << "}\\\\\n";
                 stream << "\\hline\n";
                 stream << "{\\scriptsize ";
                 stream << tr("Trimestre 4") << "} & {\\scriptsize " << ui.latconta347->item(fila,5)->text() << "}\\\\\n";
                 stream << "\\hline\n";
                 stream << "\\end{tabular}\n";
                 stream << "\\end{center}\n";
               }
            QString cuerpo2_347ed=cuerpo2_347;
            stream << cuerpo2_347ed.replace("{cifra}",cifra).replace("{nif}", ui.latconta347->item(fila,7)->text()) << "\n\n";
            stream << pie347 << "\n\n";
            stream << "\\vspace{2\\parskip}\n";
            stream << "\\closing{" << saludo347 << "}\n";
            stream << "\\vspace{2\\parskip}\n";
            stream << "\\end{letter}\n\n\n";
            fila++;
           }
    stream << "\\end{document}\n";
    fichero.close();

    int valor=imprimelatex2(tr("comunicaciones_347"));
    if (valor==1)
        QMessageBox::warning( this, tr("Imprimir comunicación de operaciones"),tr("PROBLEMAS al llamar a Latex"));
    if (valor==2)
        QMessageBox::warning( this, tr("Imprimir comunicación de operaciones"),
                                 tr("PROBLEMAS al llamar a 'dvips'"));
    if (valor==3)
        QMessageBox::warning( this, tr("Imprimir comunicación de operaciones"),
                              tr("PROBLEMAS al llamar a ")+programa_imprimir());

}

void conta347::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("operaciones_347.tex");
   QFile fichero(adapta(qfichero));
    if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{landscape}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    stream << "\n";
    QString mensaje=tr("FECHA INICIAL: ");
    mensaje+=ui.inicialdateEdit->date().toString("dd.MM.yyyy");
    mensaje+=tr("  -  FECHA FINAL: ");
    mensaje+=ui.finaldateEdit->date().toString("dd.MM.yyyy");
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << mensaje << "}}";
    stream << "\\end{center}";

    stream << "\n";
    mensaje=tr("A PARTIR DE IMPORTE: ");
    mensaje+=ui.importelineEdit->text();
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << mensaje << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|r|r|r|r|r|p{3cm}|c|p{3cm}|c|p{3cm}|c|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{12}{|c|} {\\textbf{";
   QString cadena=tr("VOLUMEN DE OPERACIONES PARA 347");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // -----------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("CUENTA") << "}} & ";
    stream << "{\\tiny{" << tr("IMPORTE") << "}} & ";
    stream << "{\\tiny{" << tr("T1") << "}} & ";
    stream << "{\\tiny{" << tr("T2") << "}} & ";
    stream << "{\\tiny{" << tr("T3") << "}} & ";
    stream << "{\\tiny{" << tr("T4") << "}} & ";
    stream << "{\\tiny{" << tr("RAZÓN") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("POBLACIÓN") << "}} & ";
    stream << "{\\tiny{" << tr("CP") << "}} & ";
    stream << "{\\tiny{" << tr("PROVINCIA") << "}} & ";
    stream << "{\\tiny{" << tr("TIPO") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // -------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Importe") << "}} & ";
    stream << "{\\tiny{" << tr("T1") << "}} & ";
    stream << "{\\tiny{" << tr("T2") << "}} & ";
    stream << "{\\tiny{" << tr("T3") << "}} & ";
    stream << "{\\tiny{" << tr("T4") << "}} & ";
    stream << "{\\tiny{" << tr("Razón") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("Población") << "}} & ";
    stream << "{\\tiny{" << tr("CP") << "}} & ";
    stream << "{\\tiny{" << tr("Provincia") << "}} & ";
    stream << "{\\tiny{" << tr("Tipo") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    double suma=0;

    while (fila<ui.latconta347->rowCount())
          {
             stream << "{\\tiny " << ui.latconta347->item(fila,0)->text() << "} & {\\tiny ";
             stream << formateanumerosep(convapunto(ui.latconta347->item(fila,1)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             stream << formateanumerosep(convapunto(ui.latconta347->item(fila,2)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             stream << formateanumerosep(convapunto(ui.latconta347->item(fila,3)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             stream << formateanumerosep(convapunto(ui.latconta347->item(fila,4)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             stream << formateanumerosep(convapunto(ui.latconta347->item(fila,5)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             stream << filtracad(ui.latconta347->item(fila,6)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latconta347->item(fila,7)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latconta347->item(fila,8)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latconta347->item(fila,9)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latconta347->item(fila,10)->text());
             if (ui.latconta347->item(fila,11)->text().length()>0) stream << "-" <<
                  filtracad(ui.latconta347->item(fila,11)->text());
             stream << "} & {\\tiny ";
             stream << filtracad(ui.latconta347->item(fila,12)->text());
             stream << "} \\\\ \n  " << "\\hline\n";
             suma+=convapunto(ui.latconta347->item(fila,1)->text()).toDouble();
             fila++;
          }

    stream << "\\multicolumn{12}{|c|} {\\textbf{";
   // --------------------------------------------------------------------------------------
    stream << tr("SUMA OPERACIONES:  ");
    stream << formateanumerosep(suma,comadecimal,decimales) << "}}";
    stream <<  " \\\\ \n";
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "\\end{landscape}\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}


void conta347::imprime()
{
    generalatex();

   int valor=imprimelatex2(tr("operaciones_347"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void conta347::latex()
{
    generalatex();
    int valor=editalatex("operaciones_347");
    if (valor==1)
        QMessageBox::warning( this, tr("Relación de operaciones"),
                              tr("PROBLEMAS al llamar al editor Latex"));

}

void conta347::datoscuenta()
{
    if (ui.latconta347->rowCount()==0) return;
    if (ui.latconta347->currentRow()<0 ||
        ui.latconta347->currentRow()>ui.latconta347->rowCount()) return;
    QString cuenta=ui.latconta347->item(ui.latconta347->currentRow(),0)->text();
    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    QString externo=ui.latconta347->item(ui.latconta347->currentRow(),13)->text();
    if (externo.isEmpty())
       {
        datos_accesorios *d = new datos_accesorios();
        d->cargacodigo( cuenta );
        d->exec();
        delete(d);
       }
      else
          {
            externos *e = new externos();
            e->pasa_codigo(externo);
            e->exec();
            delete(e);
            return;
          }
}


void conta347::borralinea( void )
{
 int fila=ui.latconta347->currentRow();
 ui.latconta347->removeRow(fila);
}

void conta347::combocambiado()
{
  QString ejercicio=ui.ejerciciocomboBox->currentText();
  ui.inicialdateEdit->setDate(inicioejercicio(ejercicio));
  ui.finaldateEdit->setDate(finejercicio(ejercicio));
}


void conta347::consultamayor()
{
    if (ui.latconta347->currentRow()<0 ||
        ui.latconta347->currentRow()>ui.latconta347->rowCount()) return;
    if (ui.latconta347->rowCount()<=0) return;
    QString cuenta=ui.latconta347->item(ui.latconta347->currentRow(),0)->text();
    QString externo=ui.latconta347->item(ui.latconta347->currentRow(),13)->text();
    if (cuenta.length()>0)
    {
        QDate fecha;
        consmayor *elmayor=new consmayor(comadecimal,decimales,usuario);
        fecha=ui.inicialdateEdit->date();
        elmayor->pasadatos(cuenta,fecha,externo);
        elmayor->exec();
        delete(elmayor);
    }

}


void conta347::genera347()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajobd());
  dir->activa_pide_archivo(tr("modelo.347"));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      QString nombre=dir->respuesta_arch();
      if (nombre.length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           if (nombre.right(4)!=".347") nombre=nombre+".347";
           // QMessageBox::information( this, tr("selección"),
           //                         nombre );
           QString rutadir=dir->directorio_actual();
           if (!rutadir.endsWith(QDir::separator())) rutadir.append(QDir::separator());
           nombre=rutadir+nombre;
           // QMessageBox::information( this, tr("selección"),
           //                          nombre );
           // if (!genfich347(nombre))
           //   QMessageBox::information( this, tr("Fichero 347"),tr("NO se ha generado correctamente el fichero 347"));
           if (nombre.right(4)!=".347") nombre=nombre+".347";
           if (!genfich347(nombre))
              QMessageBox::information( this, tr("Fichero 347"),tr("NO se ha generado correctamente el fichero 347"));
          }
    }
   delete(dir);
#else


  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::AnyFile);
  dialogofich.setOption(QFileDialog::DontConfirmOverwrite,true);
    dialogofich.setAcceptMode (QFileDialog::AcceptSave );
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  QString filtrotexto=tr("Archivos 347 (*.347)");
  filtros << filtrotexto;
  filtros << tr("Archivos 347 (*.347)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO 347"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      // QMessageBox::information( this, tr("EXPORTAR PLAN"),dialogofich.selectedNameFilter () );
      if (fileNames.at(0).length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           QString nombre=fileNames.at(0);
           if (nombre.right(4)!=".347") nombre=nombre+".347";
           if (!genfich347(nombre))
              QMessageBox::information( this, tr("Fichero 347"),tr("NO se ha generado correctamente el fichero 347"));
          }
       }
#endif

}



bool conta347::genfich347(QString nombre)
{
 QFile plan( adapta(nombre)  );
 if (plan.exists() && QMessageBox::question(this,
       tr("¿ Sobreescribir ?"),
       tr("'%1' ya existe."
      "¿ Desea sobreescribirlo ?")
       .arg( nombre ),
       tr("&Sí"), tr("&No"),
       QString(), 0, 1 ) )
           return false;

   QFile doc( adapta(nombre)  );

   if ( !doc.open( QIODevice::WriteOnly ) )
      {
       QMessageBox::warning( this, tr("Fichero 347"),tr("Error: Imposible grabar fichero"));
       return false;
      }
   QTextStream docstream( &doc );
   docstream.setEncoding(QStringConverter::Latin1);
   //docstream.setAutoDetectUnicode(false);
   QString contenido="1347";

   QString cadanyo;
   cadanyo.setNum(ui.finaldateEdit->date().year());
   contenido+=cadanyo.trimmed();

   QString nif=completacadnum(ui.ciflineEdit->text(),9);
   contenido+=nif;

   QString nombred=completacadcad(adaptacad_aeat(ui.nombrelineEdit->text()),40);
   contenido+=nombred;

   contenido+="T"; // Presentación telemática

   QString tfno=completacadnum(adaptacad_aeat(ui.telefonolineEdit->text()),9);
   contenido+=tfno;

   QString nombrerel=completacadcad(adaptacad_aeat(ui.nombrerellineEdit->text()),40);
   contenido+=nombrerel;

   contenido+="3470000000000";

   if (ui.complgroupBox->isChecked())
      {
        if (ui.complementariaradioButton->isChecked())
           contenido+="C ";
         else
             contenido+=" S";
        contenido+=completacadnum(ui.anteriorlineEdit->text(),13);
      }
      else
          {
           contenido+="  ";
           contenido+="0000000000000";
          }

  // Número total de personas y entidades
  QString numregistros;
  numregistros.setNum(ui.latconta347->rowCount());
  numregistros=completacadnum(numregistros.trimmed(),9);
  contenido+=numregistros;

  // importe anual de las operaciones
  // hay que sumar la columna 1

  double total_operaciones=0;
  for (int veces=0; veces<ui.latconta347->rowCount(); veces++)
     {
      total_operaciones+=convapunto(ui.latconta347->item(veces,1)->text()).toDouble();
     }
  QString cad_total_op;
  cad_total_op.setNum(total_operaciones,'f',2);
  cad_total_op=cad_total_op.trimmed();

  if (total_operaciones<0)
     contenido+="N";
    else
        contenido+=" ";

  QString cad_tot_entera=cad_total_op.section(".",0,0).trimmed().remove('-');
  QString cad_tot_decimal=cad_total_op.section(".",1,1).trimmed();
  cad_tot_entera=completacadnum(cad_tot_entera,13);
  contenido+=cad_tot_entera;
  cad_tot_decimal=completacadnum(cad_tot_decimal,2);
  contenido+=cad_tot_decimal;

  // Número total de inmuebles: 9 posiciones
  contenido+="000000000";

  // IMPORTE TOTAL DE LAS OPERACIONES DE ARRENDAMIENTO DE LOCALES DE NEGOCIO.
  // Campo numérico de 15 posiciones.
  contenido+=" "; // para el signo
  contenido+="000000000000000";

  // 205 blancos
  QString str; str.fill(' ', 205);
  contenido+=str;

  // NIF DEL REPRESENTANTE LEGAL
  str.clear(); str.fill(' ',9);
  contenido+=str;

  // 88 BLANCOS
  str.clear(); str.fill(' ',88);
  contenido+=str;

  // Blancos para el sello electrónico: 13 posiciones
  str.clear(); str.fill(' ',13);
  contenido+=str;
  contenido+="\r\n";
  // -----------------------------------
  // FIN REGISTRO TIPO I
  // -----------------------------------

  // REGISTRO DE DETALLE
  // TIPO DE REGISTRO 2

  for (int veces=0; veces<ui.latconta347->rowCount(); veces++)
      {
       contenido+="2347";

       cadanyo.setNum(ui.finaldateEdit->date().year());
       contenido+=cadanyo.trimmed();

       nif=completacadnum(ui.ciflineEdit->text(),9);
       contenido+=nif;

       // nif del declarado
       nif=completacadnum(ui.latconta347->item(veces,7)->text().trimmed(),9);
       contenido+=nif;

       // nif del representate legal (lo suponemos vacío)
       // sólo sirve para s.p. menores de 14 años

       str.clear();
       str.fill(' ',9);
       contenido+=str;

       QString denominacion;
       denominacion=ui.latconta347->item(veces,6)->text();
       denominacion.replace(',',' ');
       denominacion.replace("  "," ");
       contenido+=completacadcad(adaptacad_aeat(denominacion),40);

       // TIPO DE HOJA.
       // Constante 'D'.

       contenido+="D";

       // CÓDIGO PROVINCIA
       contenido+=completacadnum(ui.latconta347->item(veces,9)->text().left(2),2);

       // CÓDIGO PAÍS
       // insertamos blancos
       if (ui.latconta347->item(veces,9)->text().left(2)!="99")
         contenido+="  ";
        else contenido+=ui.latconta347->item(veces,11)->text().left(2);

       // BLANCO
       contenido+=" ";

       // Clave operación
       contenido+=ui.latconta347->item(veces,12)->text().left(1);

       // Importe anual de las operaciones
       QString cad_vol_op=convapunto(ui.latconta347->item(veces,1)->text().trimmed());
       double importe=cad_vol_op.toDouble();
       if (importe<0)
          contenido+="N";
         else
             contenido+=" ";

       QString cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
       QString cad_decimal=cad_vol_op.section(".",1,1).trimmed();
       cad_entero=completacadnum(cad_entero,13);
       contenido+=cad_entero;
       cad_decimal=completacadnum(cad_decimal,2);
       contenido+=cad_decimal;

       // OPERACIÓN SEGURO
       contenido+=" ";

      // ARRENDAMIENTO LOCAL NEGOCIO
       contenido+=" ";

      // IMPORTE PERCIBIDO EN METÁLICO. 15 posiciones
       str.clear();
       str.fill('0',15);
       contenido+=str;

       // IMPORTE ANUAL PERCIBIDO POR TRANSMISIONES DE INMUEBLES SUJETAS A IVA.
       // 16 posiciones
       str.clear();
       str.fill('0',15);
       contenido+=" ";
       contenido+=str;

       // Ejercicio cobro en metálico
       contenido+="0000";

       // IMPORTE DE LAS OPERACIONES PRIMER TRIMESTRE.
       // SIGNO + Número 15 posiciones
       cad_vol_op=convapunto(ui.latconta347->item(veces,2)->text().trimmed());
       importe=cad_vol_op.toDouble();
       if (importe<0)
            contenido+="N";
          else
            contenido+=" ";

       cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
       cad_decimal=cad_vol_op.section(".",1,1).trimmed();
       cad_entero=completacadnum(cad_entero,13);
       contenido+=cad_entero;
       cad_decimal=completacadnum(cad_decimal,2);
       contenido+=cad_decimal;

       // IMPORTE PERCIBIDO POR TRANSMISIONES DE INMUEBLES SUJETAS A IVA PRIMER TRIMESTRE.
       // SIGNO + 15 posiciones
       str.clear();
       str.fill('0',15);
       contenido+=" ";
       contenido+=str;

       // IMPORTE DE LAS OPERACIONES SEGUNDO TRIMESTRE.
       cad_vol_op=convapunto(ui.latconta347->item(veces,3)->text().trimmed());
       importe=cad_vol_op.toDouble();
       if (importe<0)
            contenido+="N";
          else
            contenido+=" ";

       cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
       cad_decimal=cad_vol_op.section(".",1,1).trimmed();
       cad_entero=completacadnum(cad_entero,13);
       contenido+=cad_entero;
       cad_decimal=completacadnum(cad_decimal,2);
       contenido+=cad_decimal;

       // IMPORTE PERCIBIDO POR TRANSMISIONES DE INMUEBLES SUJETAS A IVA SEGUNDO TRIMESTRE.
       // SIGNO + 15 posiciones
       str.clear();
       str.fill('0',15);
       contenido+=" ";
       contenido+=str;

       // IMPORTE DE LAS OPERACIONES TERCER TRIMESTRE
       cad_vol_op=convapunto(ui.latconta347->item(veces,4)->text().trimmed());
       importe=cad_vol_op.toDouble();
       if (importe<0)
            contenido+="N";
          else
            contenido+=" ";

       cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
       cad_decimal=cad_vol_op.section(".",1,1).trimmed();
       cad_entero=completacadnum(cad_entero,13);
       contenido+=cad_entero;
       cad_decimal=completacadnum(cad_decimal,2);
       contenido+=cad_decimal;

       // IMPORTE PERCIBIDO POR TRANSMISIONES DE INMUEBLES SUJETAS A IVA TERCER TRIMESTRE.
       // SIGNO + 15 posiciones
       str.clear();
       str.fill('0',15);
       contenido+=" ";
       contenido+=str;

       // IMPORTE DE LAS OPERACIONES CUARTO TRIMESTRE
       cad_vol_op=convapunto(ui.latconta347->item(veces,5)->text().trimmed());
       importe=cad_vol_op.toDouble();
       if (importe<0)
            contenido+="N";
          else
            contenido+=" ";

       cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
       cad_decimal=cad_vol_op.section(".",1,1).trimmed();
       cad_entero=completacadnum(cad_entero,13);
       contenido+=cad_entero;
       cad_decimal=completacadnum(cad_decimal,2);
       contenido+=cad_decimal;

       // IMPORTE PERCIBIDO POR TRANSMISIONES DE INMUEBLES SUJETAS A IVA CUARTO TRIMESTRE.
       // SIGNO + 15 posiciones
       str.clear();
       str.fill('0',15);
       contenido+=" ";
       contenido+=str;

       // NIF OPERADOR COMUNITARIO
       str.clear();
       str.fill(' ',17);
       contenido+=str;

       // OPERACIONES CRITERIO DE CAJA
       contenido+=" ";

       // OPERACIÓN CON INVERSIÓN SUJETO PASIVO
       contenido+=" ";

       // OPERACIÓN con bienes vinculados
       contenido+=" ";

       // importe anual operaciones devengadas criterio caja
       contenido+=" "; //signo
       str.clear();
       str.fill('0',15);
       contenido+=str;

       // BLANCOS 201
       str.clear();
       str.fill(' ',201);
       //contenido+=" ";
       contenido+=str;
       if (veces<ui.latconta347->rowCount()-1) contenido+="\r\n";
  // ----------------------------------------------------------------------------------------
   }

  //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

  //QTextCodec *codec = QTextCodec::codecForName("ISO 8859-1");
  //QByteArray paso=contenido.toLatin1();
  //paso.replace(45,209);
  docstream << contenido;
  doc.close();

  QMessageBox::information( this, tr("Fichero 347"),tr("347 generado en archivo: ")+nombre);

  return true;
}




void conta347::copiar()
{
   QString cadena;
   cadena+=nombreempresa();
   cadena+="\n\n";

   cadena+=tr("FECHA INICIAL:\t");
   cadena+=ui.inicialdateEdit->date().toString("dd.MM.yyyy");
   cadena+=tr("\tFECHA FINAL:\t");
   cadena+=ui.finaldateEdit->date().toString("dd.MM.yyyy");
   cadena+="\n";

   cadena+=tr("A PARTIR DE IMPORTE:\t");
   cadena+=ui.importelineEdit->text();
   cadena+="\n\n";

   cadena+=tr("VOLUMEN DE OPERACIONES PARA MODELO 347");
   // --------------------------------------------------------------------------------------
   cadena+="\n";
    // -----------------------------------------------------------------------------------------
   cadena+= tr("CUENTA");
   cadena+="\t";
   cadena+= tr("IMPORTE");
   cadena+="\t";
   cadena+= tr("T1");
   cadena+="\t";
   cadena+= tr("T2");
   cadena+="\t";
   cadena+= tr("T3");
   cadena+="\t";
   cadena+= tr("T4");
   cadena+="\t";
   cadena+= tr("RAZÓN");
   cadena+="\t";
   cadena+= tr("CIF/NIF");
   cadena+="\t";
   cadena+= tr("POBLACIÓN");
   cadena+="\t";
   cadena+= tr("CP");
   cadena+="\t";
   cadena+= tr("PROVINCIA");
   cadena+="\t";
   cadena+= tr("TIPO");
   cadena+="\n";
    // -------------------------------------------------------------------------------------------
    int fila=0;
    double suma=0;

    while (fila<ui.latconta347->rowCount())
          {
             cadena+=ui.latconta347->item(fila,0)->text();
             cadena+="\t";
             cadena+=formateanumero(convapunto(ui.latconta347->item(fila,1)->text()).toDouble(),
                                      comadecimal,decimales);
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latconta347->item(fila,2)->text()).toDouble(),
                                      comadecimal,decimales);
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latconta347->item(fila,3)->text()).toDouble(),
                                      comadecimal,decimales);
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latconta347->item(fila,4)->text()).toDouble(),
                                      comadecimal,decimales);
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latconta347->item(fila,5)->text()).toDouble(),
                                      comadecimal,decimales);
             cadena+="\t";
             cadena+=ui.latconta347->item(fila,6)->text();
             cadena+="\t";
             cadena+=ui.latconta347->item(fila,7)->text();
             cadena+="\t";
             cadena+=ui.latconta347->item(fila,8)->text();
             cadena+="\t";
             cadena+=ui.latconta347->item(fila,9)->text();
             cadena+="\t";
             cadena+=ui.latconta347->item(fila,10)->text();
             //if (ui.latconta347->item(fila,11)->text().length()>0) cadena+= "-" +
             //     ui.latconta347->item(fila,11)->text();
             cadena+="\t";
             cadena+=ui.latconta347->item(fila,12)->text();
             cadena+="\n";
             suma+=convapunto(ui.latconta347->item(fila,1)->text()).toDouble();
             fila++;
          }

   // --------------------------------------------------------------------------------------
    cadena+= "\n\t\t" + tr("SUMA OPERACIONES:  ")+"\t";
    cadena+=formateanumerosep(suma,comadecimal,decimales) + "\n";
    QClipboard *cb = QApplication::clipboard();
    cb->setText(cadena);
    QMessageBox::information( this, tr("Modelo 347"),
                              tr("Se ha pasado el contenido al portapapeles") );

}

void conta347::csv_import()
{
    QFileDialog dialogofich(this);
    dialogofich.setFileMode(QFileDialog::ExistingFile);
    dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
    dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
    dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
    dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
    dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

    dialogofich.setNameFilter("*.csv");
    dialogofich.setDirectory(dirtrabajo());
    dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR REGISTROS 347"));
    QStringList fileNames;
    if (dialogofich.exec())
       {
        fileNames = dialogofich.selectedFiles();
        if (fileNames.at(0).length()>0)
            {
             // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
             QString cadfich=fileNames.at(0);
             if (importar_csv(cadfich))
                 {
                  QMessageBox::information( this, tr("IMPORTACIÓN"),
                               tr("El archivo seleccionado se ha importado correctamente"));
                 }
            }
       }

}

bool conta347::importar_csv(QString qfichero)
{
    QFile fichero(qfichero);
    if (!fichero.exists()) return false;
    if ( fichero.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea;
          QProgressDialog progreso(tr("Cargando información ... ")
                                   , 0, 0, 0);
          // progreso.setWindowModality(Qt::WindowModal);
          progreso.setMinimumDuration ( 0 );
          progreso.show();
          progreso.update();
          QApplication::processEvents();
          int fila=ui.latconta347->rowCount();
          while ( !stream.atEnd() )
            {
              QApplication::processEvents();
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              ui.latconta347->insertRow(fila);
              for (int veces=0;veces<12;veces++)
               {
                  QString sec=linea.section('\t',veces,veces);
                  if (veces>0 && veces<6)
                      sec=formateanumero(convapunto(sec).toDouble(),comadecimal,decimales);
                  if (veces==11) veces++;
                  QTableWidgetItem *newItem = new QTableWidgetItem(sec);
                  if (veces>0 && veces<6) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);

                  ui.latconta347->setItem(fila,veces,newItem);
               }
              fila++;
            }
          fichero.close();
        } else return false;

    return true;

}
