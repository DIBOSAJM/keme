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

#include "rellibiva.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include <QRadioButton>
#include "datos_accesorios.h"
#include <QProgressDialog>
#include <QMessageBox>
#include "externos.h"

rellibiva::rellibiva() : QDialog() {
    ui.setupUi(this);

if (conigic()) setWindowTitle(tr("Relación de operaciones con terceras personas según libros IGIC"));

 comadecimal=haycomadecimal(); decimales=haydecimales();

    QString ejercicio;
    ejercicio=ejerciciodelafecha(QDate::currentDate());
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


 QStringList columnas;
 columnas << tr("CUENTA") << tr("IMPORTE") << tr("1T") << tr("2T") << tr("3T") << tr("4T");
 columnas << tr("RAZÓN");
 columnas << tr("CIF/NIF") << tr("POBLACION") << tr("CP") << tr("PROVINCIA");
 columnas << tr("NACIÓN");
 columnas << tr("EXTERNO");

 ui.latrellibiva->setHorizontalHeaderLabels(columnas);

  ui.latrellibiva->setColumnWidth(6,200);
  ui.latrellibiva->setColumnWidth(9,80);
  // Fijamos fechas por defecto para inicial y final
  if (ejerciciodelafecha(QDate::currentDate()).length()>0)
     {
       QString ejercicio=ejerciciodelafecha(QDate::currentDate());
       ui.inicialdateEdit->setDate(inicioejercicio(ejercicio));
       ui.finaldateEdit->setDate(finejercicio(ejercicio));
     }
  // ui.latrellibiva->setEditTriggers ( QAbstractItemView::NoEditTriggers );

  connect(ui.cargapushButton,SIGNAL( clicked()),this,SLOT(cargadatos()));
  connect(ui.radioButton1,SIGNAL(toggled(bool)),this,SLOT(borratabla()));
  connect(ui.radioButton2,SIGNAL(toggled(bool)),this,SLOT(borratabla()));
  connect(ui.aibradioButton,SIGNAL(toggled(bool)),this,SLOT(borratabla()));
  connect(ui.eibradioButton,SIGNAL(toggled(bool)),this,SLOT(borratabla()));
  connect(ui.autofacturasnoueradioButton,SIGNAL(toggled(bool)),this,SLOT(borratabla()));
  connect(ui.autofacturasueradioButton,SIGNAL(toggled(bool)),this,SLOT(borratabla()));
  connect(ui.prestservUEradioButton,SIGNAL(toggled(bool)),this,SLOT(borratabla()));
  connect(ui.importelineEdit,SIGNAL(textChanged(QString)),this,SLOT(borratabla()));
  connect(ui.importelineEdit,SIGNAL(editingFinished()),this,SLOT(importefinedic()));

  connect(ui.inicialdateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(borratabla()));
  connect(ui.finaldateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(borratabla()));

  connect(ui.imprimepushButton,SIGNAL( clicked()),this,SLOT(imprime()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

  connect(ui.datospushButton,SIGNAL(clicked()),SLOT(datoscuenta()));
  connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borralinea()));

  connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borralinea()));
  connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));

#ifdef NOEDITTEX
 ui.latexpushButton->hide();
#endif

}

void rellibiva::borratabla()
{
  ui.latrellibiva->setRowCount(0);
}


void rellibiva::cargadatos()
{
   borratabla();
   bool trimestral=true;
   if (ui.inicialdateEdit->date().year()!= ui.finaldateEdit->date().year())
     {
      trimestral=false;
      ui.latrellibiva->hideColumn(2);
      ui.latrellibiva->hideColumn(3);
      ui.latrellibiva->hideColumn(4);
      ui.latrellibiva->hideColumn(5);
     }
     else
         {
          ui.latrellibiva->showColumn(2);
          ui.latrellibiva->showColumn(3);
          ui.latrellibiva->showColumn(4);
          ui.latrellibiva->showColumn(5);
         }

   if (!ui.radioButton1->isChecked() &&
       !ui.radioButton2->isChecked() &&
       !ui.eibradioButton->isChecked() &&
       !ui.aibradioButton->isChecked() &&
       !ui.autofacturasueradioButton->isChecked() &&
       !ui.autofacturasnoueradioButton->isChecked() &&
       !ui.prestservUEradioButton->isChecked() &&
       !ui.isp_interiores_radioButton->isChecked()
       ) return;

   QProgressDialog progreso(tr("Cargando información ... ")
                            , 0, 0, 0);
   progreso.setWindowModality(Qt::WindowModal);
   progreso.setMinimumDuration ( 0 );
   progreso.show();
   progreso.update();
   QApplication::processEvents();

   QString cadimporte=ui.importelineEdit->text();
   if (!cadnumvalido(convapunto(cadimporte)) || cadimporte.length()==0) cadimporte="0";
   cadimporte=convapunto(cadimporte);

    QSqlQuery consulta = basedatos::instancia()->selectCuenta_frabase_ivafechasgroupordercuenta_fra(
            ui.inicialdateEdit->date() , ui.finaldateEdit->date() , ui.aibradioButton->isChecked() ,
            ui.eibradioButton->isChecked() , ui.radioButton1->isChecked() , ui.radioButton2->isChecked(),
            ui.autofacturasueradioButton->isChecked(), ui.autofacturasnoueradioButton->isChecked(),
            ui.prestservUEradioButton->isChecked(), ui.isp_interiores_radioButton->isChecked());

    int fila=0;
   while (consulta.next())
      {
         if (consulta.value(1).toDouble()-0.0001>=cadimporte.toDouble())
            {
              ui.latrellibiva->insertRow(fila);
              // cuenta
              ui.latrellibiva->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
              // volumen operación
              QTableWidgetItem *newItem1 = new QTableWidgetItem(
                                   formateanumero(consulta.value(1).toDouble(),comadecimal,decimales));
              newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.latrellibiva->setItem(fila,1,newItem1);

              QString externo=consulta.value(2).toString();

              if (trimestral)
                 {
                  // averiguamos para la cuenta la info trimestral
                  int anyo=ui.inicialdateEdit->date().year();
                  QString cuenta=consulta.value(0).toString();
                  QDate inicial, final;
                  inicial.setDate(anyo,1,1);
                  final.setDate(anyo,3,31);
                  double t1=basedatos::instancia()->vol_op_iva_cuenta (cuenta,
                      inicial , final , ui.aibradioButton->isChecked() ,
                      ui.eibradioButton->isChecked() , ui.radioButton1->isChecked() , ui.radioButton2->isChecked(),
                      ui.autofacturasueradioButton->isChecked(), ui.autofacturasnoueradioButton->isChecked(),
                      ui.prestservUEradioButton->isChecked(), ui.isp_interiores_radioButton->isChecked(),externo);
                  QTableWidgetItem *newItem1T = new QTableWidgetItem(
                                       formateanumero(t1,comadecimal,decimales));
                  newItem1T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latrellibiva->setItem(fila,2,newItem1T);

                  inicial.setDate(anyo,4,1);
                  final.setDate(anyo,6,30);
                  double t2=basedatos::instancia()->vol_op_iva_cuenta (cuenta,
                      inicial , final , ui.aibradioButton->isChecked() ,
                      ui.eibradioButton->isChecked() , ui.radioButton1->isChecked() , ui.radioButton2->isChecked(),
                      ui.autofacturasueradioButton->isChecked(), ui.autofacturasnoueradioButton->isChecked(),
                      ui.prestservUEradioButton->isChecked(),ui.isp_interiores_radioButton->isChecked(),externo);
                  QTableWidgetItem *newItem2T = new QTableWidgetItem(
                                       formateanumero(t2,comadecimal,decimales));
                  newItem2T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latrellibiva->setItem(fila,3,newItem2T);

                  inicial.setDate(anyo,7,1);
                  final.setDate(anyo,9,30);
                  double t3=basedatos::instancia()->vol_op_iva_cuenta (cuenta,
                      inicial , final , ui.aibradioButton->isChecked() ,
                      ui.eibradioButton->isChecked() , ui.radioButton1->isChecked() , ui.radioButton2->isChecked(),
                      ui.autofacturasueradioButton->isChecked(), ui.autofacturasnoueradioButton->isChecked(),
                      ui.prestservUEradioButton->isChecked(),ui.isp_interiores_radioButton->isChecked(),externo);
                  QTableWidgetItem *newItem3T = new QTableWidgetItem(
                                       formateanumero(t3,comadecimal,decimales));
                  newItem3T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latrellibiva->setItem(fila,4,newItem3T);

                  inicial.setDate(anyo,10,1);
                  final.setDate(anyo,12,31);
                  double t4=basedatos::instancia()->vol_op_iva_cuenta (cuenta,
                      inicial , final , ui.aibradioButton->isChecked() ,
                      ui.eibradioButton->isChecked() , ui.radioButton1->isChecked() , ui.radioButton2->isChecked(),
                      ui.autofacturasueradioButton->isChecked(), ui.autofacturasnoueradioButton->isChecked(),
                      ui.prestservUEradioButton->isChecked(),ui.isp_interiores_radioButton->isChecked(),externo);
                  QTableWidgetItem *newItem4T = new QTableWidgetItem(
                                       formateanumero(t4,comadecimal,decimales));
                  newItem4T->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                  ui.latrellibiva->setItem(fila,5,newItem4T);

                  ui.latrellibiva->setItem(fila,12,new QTableWidgetItem(externo));
                 }

              QSqlQuery consulta2;
              if (externo.isEmpty())
                consulta2 = basedatos::instancia()->select7Datossubcuentacuenta( consulta.value(0).toString() );
               else
                  consulta2 = basedatos::instancia()->select7datos_externo(externo);

              if ( (consulta2.isActive()) && (consulta2.next()) )
                {
                 ui.latrellibiva->setItem(fila,6,new QTableWidgetItem(consulta2.value(0).toString()));
                 if (ui.aibradioButton->isChecked() || ui.eibradioButton->isChecked())
                    ui.latrellibiva->setItem(fila,7,new QTableWidgetItem(consulta2.value(1).toString()));
                  else
                     ui.latrellibiva->setItem(fila,7,new QTableWidgetItem(consulta2.value(1).toString()));
                 ui.latrellibiva->setItem(fila,8,new QTableWidgetItem(consulta2.value(2).toString()));
                 ui.latrellibiva->setItem(fila,9,new QTableWidgetItem(consulta2.value(3).toString()));
                 ui.latrellibiva->setItem(fila,10,new QTableWidgetItem(consulta2.value(4).toString()));
                 ui.latrellibiva->setItem(fila,11,new QTableWidgetItem(consulta2.value(6).toString()));
                }
               else
                    {
                      ui.latrellibiva->setItem(fila,6,new QTableWidgetItem(""));
                      ui.latrellibiva->setItem(fila,7,new QTableWidgetItem(""));
                      ui.latrellibiva->setItem(fila,8,new QTableWidgetItem(""));
                      ui.latrellibiva->setItem(fila,9,new QTableWidgetItem(""));
                      ui.latrellibiva->setItem(fila,10,new QTableWidgetItem(""));
                      ui.latrellibiva->setItem(fila,11,new QTableWidgetItem(""));
                    }
              fila++;
              QApplication::processEvents();
            }
      }
   // progreso.reset();
}



void rellibiva::importefinedic()
{
  ui.importelineEdit->setText(formateanumero(ui.importelineEdit->text().toDouble(),comadecimal,decimales));
  borratabla();
}


void rellibiva::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("operaciones_iva.tex");
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
    stream << filtracad(mensaje) << "}}";
    stream << "\\end{center}";

    stream << "\n";
    mensaje=tr("A PARTIR DE IMPORTE: ");
    mensaje+=ui.importelineEdit->text();
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << filtracad(mensaje) << "}}";
    stream << "\\end{center}";
    
    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|r|r|r|r|r|p{3cm}|c|p{3cm}|c|p{3cm}|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{11}{|c|} {\\textbf{";
   QString cadena=tr("VOLUMEN DE OPERACIONES ");
   if (ui.radioButton1->isChecked()) cadena+=tr("POR ADQUISICIONES DE BIENES Y SERVICIOS ");
   if (ui.radioButton2->isChecked()) cadena+=tr("POR VENTAS DE BIENES Y SERVICIOS");
   if (ui.aibradioButton->isChecked()) cadena+=tr("POR ADQUISICIONES INTRACOMUNITARIAS");
   if (ui.eibradioButton->isChecked()) cadena+=tr("POR ENTREGAS INTRACOMUNITARIAS");
   if (ui.autofacturasueradioButton->isChecked()) cadena+=tr("POR AUTOFACTURAS CON PAÍSES UE");
   if (ui.autofacturasnoueradioButton->isChecked()) cadena+=tr("POR AUTOFACTURAS CON PAÍSES FUERA DE LA UE");
   if (ui.prestservUEradioButton->isChecked()) cadena+=tr("POR PRESTACIÓN DE SERVICIOS CON LA UE");
   // --------------------------------------------------------------------------------------
   stream << filtracad(cadena);
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
    stream << "{\\tiny{" << tr("PROVINCIA") << "}}";
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
    stream << "{\\tiny{" << tr("Provincia") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    double suma=0;

    while (fila<ui.latrellibiva->rowCount())
          {
             stream << "{\\tiny " << ui.latrellibiva->item(fila,0)->text() << "} & {\\tiny "; 
             stream << formateanumerosep(convapunto(ui.latrellibiva->item(fila,1)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             if (ui.latrellibiva->item(fila,2)!=NULL)
             {
               stream << formateanumerosep(convapunto(ui.latrellibiva->item(fila,2)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
               stream << formateanumerosep(convapunto(ui.latrellibiva->item(fila,3)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
               stream << formateanumerosep(convapunto(ui.latrellibiva->item(fila,4)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
               stream << formateanumerosep(convapunto(ui.latrellibiva->item(fila,5)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             }
              else
                {
                 stream << " - - " << "} & {\\tiny ";
                 stream << " - - " << "} & {\\tiny ";
                 stream << " - - " << "} & {\\tiny ";
                 stream << " - - " << "} & {\\tiny ";
                }

             stream << filtracad(ui.latrellibiva->item(fila,6)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latrellibiva->item(fila,7)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latrellibiva->item(fila,8)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latrellibiva->item(fila,9)->text()) << "} & {\\tiny ";
             stream << filtracad(ui.latrellibiva->item(fila,10)->text());
             if (ui.latrellibiva->item(fila,11)->text().length()>0) stream << "-" <<
                  filtracad(ui.latrellibiva->item(fila,11)->text());
             stream << "} \\\\ \n  " << "\\hline\n";
             suma+=convapunto(ui.latrellibiva->item(fila,1)->text()).toDouble();
             fila++;
          }

    stream << "\\multicolumn{11}{|c|} {\\textbf{";
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


void rellibiva::imprime()
{
    generalatex();

   int valor=imprimelatex2(tr("operaciones_iva"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void rellibiva::latex()
{
    generalatex();
    int valor=editalatex("operaciones_iva");
    if (valor==1)
        QMessageBox::warning( this, tr("Relación de operaciones"),
                              tr("PROBLEMAS al llamar al editor Latex"));

}

void rellibiva::datoscuenta()
{
    if (ui.latrellibiva->rowCount()==0) return;
    if (ui.latrellibiva->currentRow()<0 ||
        ui.latrellibiva->currentRow()>ui.latrellibiva->rowCount()) return;
    QString cuenta=ui.latrellibiva->item(ui.latrellibiva->currentRow(),0)->text();
    QString externo=ui.latrellibiva->item(ui.latrellibiva->currentRow(),12)->text();
    if (!externo.isEmpty())
      {
        externos *e = new externos();
        e->pasa_codigo(externo);
        e->exec();
        delete(e);
        return;
      }
    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    datos_accesorios *d = new datos_accesorios();
    d->cargacodigo( cuenta );
    d->exec();
    delete(d);
}


void rellibiva::borralinea( void )
{
 int fila=ui.latrellibiva->currentRow();
 ui.latrellibiva->removeRow(fila);
}


void rellibiva::copiar()
{
    QString cadena;
    cadena=filtracad(nombreempresa());
    cadena+= "\n";

    cadena+=tr("FECHA INICIAL:")+"\t";
    cadena+=ui.inicialdateEdit->date().toString("dd-MM-yyyy");
    cadena+="\t";
    cadena+=tr("FECHA FINAL:");
    cadena+="\t";
    cadena+=ui.finaldateEdit->date().toString("dd-MM-yyyy");
    cadena+= "\n";
    cadena+=tr("A PARTIR DE IMPORTE: ");
    cadena+="\t"+ui.importelineEdit->text();
    cadena+= "\n";

   cadena+=tr("VOLUMEN DE OPERACIONES ");
   if (ui.radioButton1->isChecked()) cadena+=tr("POR ADQUISICIONES DE BIENES Y SERVICIOS ");
   if (ui.radioButton2->isChecked()) cadena+=tr("POR VENTAS DE BIENES Y SERVICIOS");
   if (ui.aibradioButton->isChecked()) cadena+=tr("POR ADQUISICIONES INTRACOMUNITARIAS");
   if (ui.eibradioButton->isChecked()) cadena+=tr("POR ENTREGAS INTRACOMUNITARIAS");
   if (ui.autofacturasueradioButton->isChecked()) cadena+=tr("POR AUTOFACTURAS CON PAÍSES UE");
   if (ui.autofacturasnoueradioButton->isChecked()) cadena+=tr("POR AUTOFACTURAS CON PAÍSES FUERA DE LA UE");
   if (ui.prestservUEradioButton->isChecked()) cadena+=tr("POR PRESTACIÓN DE SERVICIOS CON LA UE");
   // --------------------------------------------------------------------------------------
   cadena+= "\n";
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
    cadena+="\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    double suma=0;

    while (fila<ui.latrellibiva->rowCount())
          {
             cadena+=ui.latrellibiva->item(fila,0)->text() ;
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latrellibiva->item(fila,1)->text()).toDouble(),
                                      comadecimal,decimales) ;
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latrellibiva->item(fila,2)->text()).toDouble(),
                                      comadecimal,decimales) ;
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latrellibiva->item(fila,3)->text()).toDouble(),
                                      comadecimal,decimales) ;
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latrellibiva->item(fila,4)->text()).toDouble(),
                                      comadecimal,decimales) ;
             cadena+="\t";
             cadena+=formateanumerosep(convapunto(ui.latrellibiva->item(fila,5)->text()).toDouble(),
                                      comadecimal,decimales) ;
             cadena+="\t";
             cadena+=filtracad(ui.latrellibiva->item(fila,6)->text()) ;
             cadena+="\t";
             cadena+=filtracad(ui.latrellibiva->item(fila,7)->text()) ;
             cadena+="\t";
             cadena+=filtracad(ui.latrellibiva->item(fila,8)->text()) ;
             cadena+="\t";
             cadena+=filtracad(ui.latrellibiva->item(fila,9)->text()) ;
             cadena+="\t";
             cadena+=filtracad(ui.latrellibiva->item(fila,10)->text());
             if (ui.latrellibiva->item(fila,11)->text().length()>0) cadena+= "-" +
                                   filtracad(ui.latrellibiva->item(fila,11)->text());
             cadena+="\n";
             suma+=convapunto(ui.latrellibiva->item(fila,1)->text()).toDouble();
             fila++;
          }
   // --------------------------------------------------------------------------------------
    cadena+="\n";
    cadena+= tr("SUMA OPERACIONES:")+"\t";
    cadena+=formateanumerosep(suma,comadecimal,decimales);
    cadena+="\n";

    QClipboard *cb = QApplication::clipboard();
    cb->setText(cadena);
    QMessageBox::information( this, tr("Consulta operaciones"),
                              tr("Se ha pasado el contenido al portapapeles") );

}
