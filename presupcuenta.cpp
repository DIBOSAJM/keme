/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#include "presupcuenta.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

presupcuenta::presupcuenta(QString qejercicio,QString qcuenta) : QDialog() {
    ui.setupUi(this);

    comadecimal=haycomadecimal();
    decimales=haydecimales();
    ejercicio=qejercicio;
    cuenta=qcuenta;
    QString cadena="Presupuesto cuenta:";
    cadena+=cuenta;
    cadena+=" - ";
    cadena+=ejercicio;
    ui.titulolabel->setText(cadena);

 connect(ui.periodoscheckBox,SIGNAL(stateChanged(int)), SLOT(checkperiodoscambiado()));
 connect(ui.tableWidget,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posicioncambiada ( int, int)));
 connect(ui.presupuestolineEdit,SIGNAL(textChanged (QString)),this,
           SLOT(presupcambiado()));
 connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(grabar()));
 connect(ui.imprimirpushButton,SIGNAL(clicked()),this,SLOT(imprime()));
 connect(ui.repartirpushButton,SIGNAL(clicked()),this,SLOT(repartir_periodos()));

 QStringList columnas;
 columnas << tr("Base") << tr("Presupuesto") << tr("Incremento");
 ui.tableWidget->setHorizontalHeaderLabels(columnas);

 QSqlQuery query = basedatos::instancia()->selectCodigoperiodoejercicioorderinicioasc(ejercicio);
   QStringList lfilas;
   int filas=0;
   if ( query.isActive() ) {
          while  ( query.next() ) 
                      {
                        filas++;
                        lfilas << query.value(0).toString();
                      }
          }
  ui.tableWidget->setRowCount(filas);
  ui.tableWidget->setVerticalHeaderLabels(lfilas);

  query = basedatos::instancia()->selectBasesPresupuestospresupuestoejerciciocuenta(ejercicio, cuenta);

  if ( (query.isActive()) && (query.next()) )
     {
       double sumabase=0;
       double sumapres=0;
       bool conperiodos=false;
       for (int veces=1;veces<16;veces++)
           {
             if (veces>ui.tableWidget->rowCount()) continue;
             // if (query.value(veces).toDouble()>0.001 || query.value(veces).toDouble()<-0.001)
             //    conperiodos=true;
             sumabase+=query.value(veces).toDouble();
             QLabel  *etiqueta = new QLabel();
             etiqueta->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
             etiqueta->setText(formateanumero(query.value(veces).toDouble(),comadecimal,decimales));
             ui.tableWidget->setCellWidget(veces-1,0,etiqueta);

           }

       for (int veces=17;veces<=31;veces++)
           {
             if (veces-17>ui.tableWidget->rowCount()-1) continue;
             if (query.value(veces).toDouble()>0.001 || query.value(veces).toDouble()<-0.001)
                 conperiodos=true;
             sumapres+=query.value(veces).toDouble();
             QLineEdit *ed = new QLineEdit();
             ed->setAlignment(Qt::AlignRight);

             connect(ed,SIGNAL( editingFinished ()),this,SLOT(celdafinedic()));

             ui.tableWidget->setCellWidget(veces-17,1,ed);

             if (query.value(veces).toDouble()>0.001 || query.value(veces).toDouble()<-0.001)
                ed->setText(formateanumero(query.value(veces).toDouble(),comadecimal,decimales));

             QLabel  *etiqueta = new QLabel();
             etiqueta->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
             etiqueta->setText("");
             ui.tableWidget->setCellWidget(veces-17,2,etiqueta);
             actualizaporc(veces-17);
           }
        if (!conperiodos)
          {
           ui.periodoscheckBox->setChecked(false);
           double base=query.value(0).toDouble();
           double pres=query.value(16).toDouble();
           ui.baselineEdit->setText(formateanumero(base,comadecimal,decimales));
           ui.presupuestolineEdit->setText(formateanumero(pres,comadecimal,decimales));
          }
        else
          {
           ui.periodoscheckBox->setChecked(true);
           ui.baselineEdit->setText(formateanumero(sumabase,comadecimal,decimales));
           ui.presupuestolineEdit->setText(formateanumero(sumapres,comadecimal,decimales));
          }
     }

}


void presupcuenta::checkperiodoscambiado()
{
 if (ui.periodoscheckBox->isChecked())
    {
     ui.tableWidget->setEnabled(true);
     ui.presupuestolineEdit->setReadOnly(true);
     ui.repartirpushButton->setEnabled(true);
    }
    else
         {
          ui.tableWidget->setEnabled(false);
          ui.presupuestolineEdit->setReadOnly(false);
          ui.repartirpushButton->setEnabled(false);
         }
}

void presupcuenta::celdafinedic()
{
 double sumapres=0;
 for (int veces=0;veces<ui.tableWidget->rowCount();veces++)
   {
     QLineEdit *linea=(QLineEdit*)ui.tableWidget->cellWidget(veces,1);
     if (linea->text().length()==0) { actualizaporc(veces); continue;}
     QString cadena=convapunto(linea->text());
     sumapres+=cadena.toDouble();
     linea->setText(formateanumero(cadena.toDouble(),
                  comadecimal,decimales));
     actualizaporc(veces);
   }
 ui.presupuestolineEdit->setText(formateanumero(sumapres,comadecimal,decimales));
}


void presupcuenta::posicioncambiada( int row, int col )
{
   if (col!=1) ui.tableWidget->setCurrentCell(row,1);
   // ui.variablestable->setFocus();
}

void presupcuenta::actualizaporc(int fila)
{
  QLabel  *etiqueta1 = (QLabel*)ui.tableWidget->cellWidget(fila,0);
  QLineEdit *linea=(QLineEdit*)ui.tableWidget->cellWidget(fila,1);
  QLabel  *etiqueta2 = (QLabel*)ui.tableWidget->cellWidget(fila,2);

  double base=etiqueta1->text().toDouble();
  double pres=linea->text().toDouble();
  if (base>0.001 || base < -0.001)
     {
      etiqueta2->setText(formateanumero((pres-base)/base*100,comadecimal,decimales));
     }
}

void presupcuenta::presupcambiado()
{
 double base=convapunto(ui.baselineEdit->text()).toDouble();
 double presupuesto=convapunto(ui.presupuestolineEdit->text()).toDouble();
 if (base>0.001 || base<-0.001)
    ui.incrementolineEdit->setText(formateanumero((presupuesto-base)/base*100,comadecimal,decimales));
  else ui.incrementolineEdit->setText("");
}

void presupcuenta::repartir_periodos()
{

 // reparte el presupuesto por periodos en partes iguales
 // CutAndRoundNumberToNDecimals (double dValue, int nDec)
    double reparto=CutAndRoundNumberToNDecimals (
            ui.presupuestolineEdit->text().toDouble()/ui.tableWidget->rowCount(),2);
    QString cadreparto=formateanumero(reparto,comadecimal,decimales);
    for (int veces=0;veces<ui.tableWidget->rowCount();veces++)
    {

        QLineEdit *linea=(QLineEdit*)ui.tableWidget->cellWidget(veces,1);
        linea->disconnect(SIGNAL(editingFinished()));
        linea->setText(cadreparto);
        connect(linea,SIGNAL( editingFinished ()),this,SLOT(celdafinedic()));

    }

    double sumapres=0;
    for (int veces=0;veces<ui.tableWidget->rowCount();veces++)
      {
        QLineEdit *linea=(QLineEdit*)ui.tableWidget->cellWidget(veces,1);
        if (linea->text().length()==0) { actualizaporc(veces); continue;}
        QString cadena=convapunto(linea->text());
        sumapres+=cadena.toDouble();
        actualizaporc(veces);
      }
    ui.presupuestolineEdit->setText(formateanumero(sumapres,comadecimal,decimales));

    // celdafinedic();

}


void presupcuenta::grabar()
{

    if (ui.periodoscheckBox->isChecked())
    {
        QStringList lista;
        for (int veces=0;veces<ui.tableWidget->rowCount();veces++)
        {
            QLineEdit *linea=(QLineEdit*)ui.tableWidget->cellWidget(veces,1);
            lista << linea->text();
        }
        basedatos::instancia()->updatePresupuestopresupuestosejerciciocuenta( ui.presupuestolineEdit->text() , lista, ejercicio, cuenta );

    }
    else
    {
        QStringList lista;
        for (int i=1; i<16; i++) { lista << "0"; }

        basedatos::instancia()->updatePresupuestopresupuestosejerciciocuenta( ui.presupuestolineEdit->text() , lista, ejercicio, cuenta );
    }
    accept();
}


void presupcuenta::generalatex()
{

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("def_presupuesto_cta.tex");
   QFile fichero(adapta(qfichero));
    if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    QString mensaje=tr("PRESUPUESTO EJERCICIO: ");
    mensaje+=ejercicio;
    mensaje+=" - CUENTA: ";
    mensaje+=cuenta;
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << mensaje << "}}";
    stream << "\\end{center}\n";
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << descripcioncuenta(cuenta) << "}}";
    stream << "\\end{center}\n";
// ***************************************************************************************** 
    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|l|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";
    QString cadena;
    stream << "\\multicolumn{4}{|c|} {\\textbf{";
    cadena=tr("DEFINICIÓN PRESUPUESTO");
    // --------------------------------------------------------------------------------------
    stream << cadena;
    stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // --------------------------------------------------------------------------------------
    stream << "{\\small{" << tr("PERIODO") << "}} & ";
    stream << "{\\small{" << tr("BASE") << "}} & ";
    stream << "{\\small{" << tr("PRESUPUESTO") << "}} & ";
    stream << "{\\small{" << tr("INCREMENTO") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\small{" << tr("PERIODO") << "}} & ";
    stream << "{\\small{" << tr("BASE") << "}} & ";
    stream << "{\\small{" << tr("PRESUPUESTO") << "}} & ";
    stream << "{\\small{" << tr("INCREMENTO") << "}}";

    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    while (fila<ui.tableWidget->rowCount())
          {
             QLabel  *etiqueta1 = (QLabel*)ui.tableWidget->cellWidget(fila,0);
             QLineEdit *linea=(QLineEdit*)ui.tableWidget->cellWidget(fila,1);
             QLabel  *etiqueta2 = (QLabel*)ui.tableWidget->cellWidget(fila,2);
             stream << "{\\small "; 
             stream << ui.tableWidget->verticalHeaderItem(fila)->text();
             stream << "} & {\\small ";
             stream << etiqueta1->text();
             stream << "} & {\\small ";
             stream << linea->text();
             stream << "} & {\\small ";
             stream << etiqueta2->text();
             stream << "} \\\\ \n  " << "\\hline\n";
             fila++;
          }

    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}

void presupcuenta::imprime()
{

   generalatex();
   int valor=imprimelatex2(tr("def_presupuesto_cta"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir presupuesto"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir presupuesto"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir presupuesto"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());
}
