/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#include "consret.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "privilegios.h"
#include "funciones.h"
#include "editarasiento.h"
#include "retencion.h"
#include"cambiacuentapase.h"

#include <QMessageBox>

consret::consret(QString qusuario) : QDialog() {
    ui.setupUi(this);

comadecimal=haycomadecimal();
decimales=haydecimales();
usuario=qusuario;


QDate fecha=basedatos::instancia()->maxfecha_diario();
QString ejercicio=ejerciciodelafecha(fecha);
QDate fechaini=inicioejercicio(ejercicio);
QDate fechafin=finejercicio(ejercicio);
ui.inicialdateEdit->setDate(fechaini);
ui.finaldateEdit->setDate(fechafin);

QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
QStringList ej;
if ( query.isActive() ) {
        while ( query.next() )
              ej << query.value(0).toString();
        }
ui.ejerciciocomboBox->addItems(ej);
actufechas();


connect(ui.arrendamientoscheckBox, SIGNAL(toggled(bool)), this, SLOT(arrendamientoscambiado()));
connect(ui.excluir_arrcheckBox, SIGNAL(toggled(bool)),this, SLOT(excluir_arrendamientoscambiado()));
connect(ui.refrescarpushButton,SIGNAL(clicked()),SLOT(refrescar()));
connect(ui.inicialdateEdit,SIGNAL(dateChanged(QDate)),SLOT(activarefrescar()));
connect(ui.finaldateEdit,SIGNAL(dateChanged(QDate)),SLOT(activarefrescar()));
connect(ui.agruparcheckBox,SIGNAL(toggled(bool)),SLOT(cambiaagrup()));
connect(ui.edasientopushButton,SIGNAL(clicked()),SLOT(edit_asiento()));
connect(ui.consasientopushButton,SIGNAL(clicked()),SLOT(cons_asiento()));
connect(ui.borra_asientolineEdit,SIGNAL(clicked()),SLOT(borra_asiento()));

connect(ui.copiarespushButton,SIGNAL(clicked()),SLOT(copia_resumen()));

connect(ui.copialistapushButton,SIGNAL(clicked()),SLOT(copia_detalle()));

connect(ui.informepushButton,SIGNAL(clicked()),SLOT(imprimir()));

connect(ui.edtexpushButton,SIGNAL(clicked()),SLOT(edlatex()));

connect(ui.edret_pushButton,SIGNAL(clicked()),SLOT(ed_registro_ret()));

connect(ui.cta_pushButton,SIGNAL(clicked()),SLOT(cambiacuenpase()));

connect(ui.periodocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));
connect(ui.ejerciciocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));


QString privileg=basedatos::instancia()->privilegios(qusuario);

if (privileg[edi_asiento]=='0') ui.edasientopushButton->setEnabled(false);
if (privileg[borrar_asiento]=='0') ui.borra_asientolineEdit->setEnabled(false);

ui.resumentableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );
ui.detalletableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );


ui.resumentableWidget->setColumnWidth(0,100);
ui.resumentableWidget->setColumnWidth(1,60);
ui.resumentableWidget->setColumnWidth(3,70);

ui.detalletableWidget->setColumnWidth(0,80);
ui.detalletableWidget->setColumnWidth(3,60);
ui.detalletableWidget->setColumnWidth(4,60);

#ifdef NOEDITTEX
 ui.edtexpushButton->hide();
#endif

}


void consret::arrendamientoscambiado()
{
  if (ui.arrendamientoscheckBox->isChecked()) ui.excluir_arrcheckBox->setChecked(false);
  activarefrescar();
}


void consret::excluir_arrendamientoscambiado()
{
  if (ui.excluir_arrcheckBox->isChecked()) ui.arrendamientoscheckBox->setChecked(false);
  activarefrescar();
}


void consret::refrescar()
{
    ui.resumentableWidget->clearContents();
    ui.resumentableWidget->setRowCount(0);
    ui.detalletableWidget->clearContents();
    ui.detalletableWidget->setRowCount(0);
    QSqlQuery consulta = basedatos::instancia()->retenciones_agrup (ui.inicialdateEdit->date(),
                                               ui.finaldateEdit->date(),
                                               ui.arrendamientoscheckBox->isChecked(),
                                               ui.excluir_arrcheckBox->isChecked());

    /* "select d.cuenta, r.arrendamiento, r.clave_ret, sum(r.base_ret), "
       "sum(r.retencion), sum (r.ing_cta), sum(r.ing_cta_rep) "
       "from retenciones r, diario d " */
    // cuenta retención, arrendamiento, clave, r.especie, base, retención, ing_cta, ing_cta_rep

    QStringList claveoperaciones=basedatos::instancia()->listaoperaciones_ret();

    QStringList listaprovincias= basedatos::instancia()->listacodprovincias();


    int fila=0;
    double sumadinerario=0;
    double sumaespecie=0;
    double sumaret=0;
    double sumaa_cta=0;
    while (consulta.next())
        {
          //QMessageBox::warning( 0, QObject::tr("ret"),
          //                    QObject::tr("datos"));

           ui.resumentableWidget->insertRow(fila);
           ui.resumentableWidget->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

           // arrendamiento
           QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   consulta.value(1).toBool() ? "X":"");
           newItem1->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
           ui.resumentableWidget->setItem(fila,1,newItem1);


           // investigamos clave operaciones
           QString clave;
           for (int veces=0; veces<claveoperaciones.count(); veces++)
              {
               if (claveoperaciones.at(veces).left(1)==consulta.value(2).toString())
                  clave=claveoperaciones.at(veces);
              }
           if (consulta.value(1).toBool()) clave.clear(); // arrendamiento

           QTableWidgetItem *newItem2 = new QTableWidgetItem(clave);
           ui.resumentableWidget->setItem(fila,2,newItem2);


           // hay rendimiento en especie ? - solo si ing_cta (5) es <> 0
           bool especie=false;
           if (consulta.value(5).toDouble()>0.001 || consulta.value(5).toDouble()<-0.001)
               especie=true;

           QTableWidgetItem *newItem11 = new QTableWidgetItem(
                   especie ? "X":"");
           newItem11->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
           ui.resumentableWidget->setItem(fila,3,newItem11);


           if (especie) sumaespecie+=consulta.value(3).toDouble();
              else sumadinerario+=consulta.value(3).toDouble();

           // base retenciones
           QTableWidgetItem *newItem3 = new QTableWidgetItem(
                        formateanumero(consulta.value(3).toDouble(),comadecimal,decimales));
           newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.resumentableWidget->setItem(fila,4,newItem3);


           // retenciones
           QTableWidgetItem *newItem4 = new QTableWidgetItem(
                        formateanumero(consulta.value(4).toDouble(),comadecimal,decimales));
           newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.resumentableWidget->setItem(fila,5,newItem4);
           sumaret+=consulta.value(4).toDouble();
           // ingreso a cuenta
           QTableWidgetItem *newItem5 = new QTableWidgetItem(
                   formateanumero(consulta.value(5).toDouble(),comadecimal,decimales));
           newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.resumentableWidget->setItem(fila,6,newItem5);
           sumaa_cta+=consulta.value(5).toDouble();

           // ingreso a cuenta repercutido
           QTableWidgetItem *newItem6 = new QTableWidgetItem(
                   formateanumero(consulta.value(6).toDouble(),comadecimal,decimales));
           newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.resumentableWidget->setItem(fila,7,newItem6);


           fila++;
        }
  ui.dinerarioslineEdit->setText(formateanumero(sumadinerario,comadecimal,decimales));
  ui.especielineEdit->setText(formateanumero(sumaespecie,comadecimal,decimales));
  ui.totalretlineEdit->setText(formateanumero(sumaret,comadecimal,decimales));
  ui.total_a_ctalineEdit->setText(formateanumero(sumaa_cta,comadecimal,decimales));

  // apunte, cta_ret, cta_retenido, arrendamiento, especie,
  // clave, base, retención, ing_cta, ing_cta_repercutido, nombre, cif, provincia
  /* QString cadquery = "select d.pase, d.cuenta, r.cta_retenido, r.arrendamiento, "
                     "r.clave_ret, r.base_ret, "
                     "r.retencion, r.ing_cta, r.ing_cta_rep, nombre, cif, provincia "
                     "from retenciones r, diario d " */

  if (ui.agruparcheckBox->isChecked())
      consulta = basedatos::instancia()->listaretagrup (ui.inicialdateEdit->date(),
                                                  ui.finaldateEdit->date(),
                                                  ui.arrendamientoscheckBox->isChecked(),
                                                  ui.excluir_arrcheckBox->isChecked());
    else
     consulta = basedatos::instancia()->listaret (ui.inicialdateEdit->date(),
                                                 ui.finaldateEdit->date(),
                                                 ui.arrendamientoscheckBox->isChecked(),
                                                 ui.excluir_arrcheckBox->isChecked());
  fila=0;

  ui.edasientopushButton->setEnabled(false);
  ui.edret_pushButton->setEnabled(false);
  ui.borra_asientolineEdit->setEnabled(false);

  while (consulta.next())
      {
        //QMessageBox::warning( 0, QObject::tr("ret"),
        //                    QObject::tr("datos"));
         QString externo=consulta.value(13).toString();
         bool hayexterno=!externo.trimmed().isEmpty();
         ui.detalletableWidget->insertRow(fila);

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                 consulta.value(0).toString());
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,0,newItem1);

         // cuenta retención
         ui.detalletableWidget->setItem(fila,1,new QTableWidgetItem(consulta.value(1).toString()));

         // cuenta retenido
         if (hayexterno)
            ui.detalletableWidget->setItem(fila,2,new QTableWidgetItem(externo));
           else
              ui.detalletableWidget->setItem(fila,2,new QTableWidgetItem(consulta.value(2).toString()));

         // arrendamiento
         QTableWidgetItem *newItem2 = new QTableWidgetItem(
                 consulta.value(3).toBool() ? "X":"");
         newItem2->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,3,newItem2);

         // especie
         bool especie=false;
         if (consulta.value(7).toDouble()>0.001 || consulta.value(7).toDouble()<-0.001)
             especie=true;
         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                 especie ? "X":"");
         newItem3->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,4,newItem3);


         // investigamos clave operaciones
         QString clave;
         for (int veces=0; veces<claveoperaciones.count(); veces++)
            {
             if (claveoperaciones.at(veces).left(1)==consulta.value(4).toString())
                clave=claveoperaciones.at(veces);
            }
         if (consulta.value(3).toBool()) clave.clear(); // arrendamiento

         QTableWidgetItem *newItem22 = new QTableWidgetItem(clave);
         ui.detalletableWidget->setItem(fila,5,newItem22);
         // ---------------------------------------------------------------------------------

         // base retención
         QTableWidgetItem *newItem33 = new QTableWidgetItem(
                      formateanumero(consulta.value(5).toDouble(),comadecimal,decimales));
         newItem33->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,6,newItem33);

         // retenciones
         QTableWidgetItem *newItem4 = new QTableWidgetItem(
                      formateanumero(consulta.value(6).toDouble(),comadecimal,decimales));
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,7,newItem4);

         // ingreso a cuenta
         QTableWidgetItem *newItem5 = new QTableWidgetItem(
                 formateanumero(consulta.value(7).toDouble(),comadecimal,decimales));
         newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,8,newItem5);

         // ingreso a cuenta repercutido
         QTableWidgetItem *newItem6 = new QTableWidgetItem(
                 formateanumero(consulta.value(8).toDouble(),comadecimal,decimales));
         newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,9,newItem6);


         QString nombre,datos_nombre, datos_cif, datos_codigoprovincia;

         if (!hayexterno)
           {
            QSqlQuery q= basedatos::instancia()->select7Datossubcuentacuenta(consulta.value(2).toString());
            // select razon,cif,poblacion,codigopostal,provincia,claveidfiscal,pais
            if (q.first())
             {
              datos_nombre=q.value(0).toString();
              datos_cif=q.value(1).toString();
              datos_codigoprovincia=q.value(3).toString().left(2);
              nombre=consulta.value(9).toString();
              if (nombre.isEmpty()) nombre=datos_nombre;
             }
            if (nombre.isEmpty()) nombre=descripcioncuenta(consulta.value(2).toString());
           }
           else
               {
                QSqlQuery qex=basedatos::instancia()->datos_externo_dom(externo);
                //razon, cif, ccc, IBAN, bic, "
                //domicilio, codigopostal, poblacion, provincia, "
                //pais , ref_mandato, firma_mandato from externos
                if (qex.isActive())
                    if (qex.next())
                       {
                        nombre=qex.value(0).toString();
                        datos_cif=qex.value(1).toString();
                        datos_codigoprovincia=qex.value(6).toString().left(2);
                       }
               }

         // nombre
         ui.detalletableWidget->setItem(fila,10,new QTableWidgetItem(nombre));
         // cif
         QString cif=consulta.value(10).toString();
         if (cif.isEmpty()) cif=datos_cif;
         ui.detalletableWidget->setItem(fila,11,new QTableWidgetItem(cif));
         // provincia
         QString codigoprovincia=consulta.value(11).toString();
         if (codigoprovincia.trimmed().isEmpty() || codigoprovincia.trimmed()=="--") codigoprovincia=datos_codigoprovincia;
         QString cadena;
         for (int veces=0; veces<listaprovincias.count(); veces++)
            {
             if (listaprovincias.at(veces).left(2)==codigoprovincia)
                cadena=listaprovincias.at(veces);
            }

         ui.detalletableWidget->setItem(fila,12,new QTableWidgetItem(cadena));

         // tipo de retención
         QTableWidgetItem *newItem66 = new QTableWidgetItem(consulta.value(12).toDouble() >0.0001 ?
                 formateanumero(consulta.value(12).toDouble(),comadecimal,decimales): "");
         newItem66->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.detalletableWidget->setItem(fila,13,newItem66);

         if (!consulta.value(14).toBool()) {
             for (int v=0; v<ui.detalletableWidget->columnCount(); v++) {
                 bool darkMode=(QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
                 QTableWidgetItem* item = ui.detalletableWidget->item(fila, v);
                 item->setBackground(background_alt(darkMode));
             }

         }
         fila++;
      }
  // ui.refrescarpushButton->setEnabled(false);
}

void consret::activarefrescar()
{
  ui.refrescarpushButton->setEnabled(true);
}

void consret::edit_asiento()
{
 // necesitamos conocer número de asiento y ejercicio


    if (ui.detalletableWidget->currentItem()==0) return;
    QString pase=ui.detalletableWidget->item(ui.detalletableWidget->currentRow(),0)->text();
    QString asiento=basedatos::instancia()->asientodepase(pase);
    QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
    QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

    QString qdiario;
    qdiario=diariodeasiento(asiento,ejercicio);

    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
                               tr("ERROR: No se pueden editar asientos del diario de apertura, "
                                  "regularización o cierre\n"));
         return;
       }

    if (!existeasiento(asiento,ejercicio))
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
                               tr("ERROR: El asiento seleccionado ya no existe"));
         return;
       }

 editarasiento(asiento,usuario,ejercicio);
 refrescar();
}


void consret::cons_asiento()
{
    // necesitamos conocer número de asiento y ejercicio


       if (ui.detalletableWidget->currentItem()==0) return;
       QString pase=ui.detalletableWidget->item(ui.detalletableWidget->currentRow(),0)->text();
       QString asiento=basedatos::instancia()->asientodepase(pase);
       QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
       QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

       QString qdiario;
       qdiario=diariodeasiento(asiento,ejercicio);

    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
                               tr("ERROR: No se pueden consultar asientos del diario de apertura, "
                                  "regularización o cierre\n"));
         return;
       }

    if (!existeasiento(asiento,ejercicio))
       {
         QMessageBox::warning( this, tr("Consultas de Mayor"),
                               tr("ERROR: El asiento seleccionado ya no existe"));
         return;
       }

    consultarasiento(asiento,usuario,ejercicio);

}


void consret::borra_asiento()
{
    if (ui.detalletableWidget->currentItem()==0) return;
    QString pase=ui.detalletableWidget->item(ui.detalletableWidget->currentRow(),0)->text();
    QString asiento=basedatos::instancia()->asientodepase(pase);
    QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
    QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

    borrarasientofunc(asiento,ejercicio,true);
    refrescar();

}


void consret::cambiaagrup()
{
  if (ui.agruparcheckBox->isChecked())
    {
      ui.edasientopushButton->setEnabled(false);
      ui.consasientopushButton->setEnabled(false);
      ui.borra_asientolineEdit->setEnabled(false);
      ui.edret_pushButton->setEnabled(false);
      ui.cta_pushButton->setEnabled(false);
      ui.detalletableWidget->hideColumn(0);
    }
    else
        {
         ui.edasientopushButton->setEnabled(true);
         ui.consasientopushButton->setEnabled(true);
         ui.borra_asientolineEdit->setEnabled(true);
         ui.edret_pushButton->setEnabled(true);
         ui.cta_pushButton->setEnabled(true);
         ui.detalletableWidget->showColumn(0);
        }

  refrescar();
}


void consret::copia_resumen()
{

    QClipboard *cb = QApplication::clipboard();
    QString global;

    global=nombreempresa();
    global+="\n";
    global+=tr("FECHA INICIAL:");
    global+="\t";
    global+=ui.inicialdateEdit->date().toString("dd/MM/yyyy");
    global+="\n";
    global+=tr("FECHA FINAL:");
    global+="\t";
    global+=ui.finaldateEdit->date().toString("dd/MM/yyyy");
    global+="\n\n";
    if (ui.arrendamientoscheckBox->isChecked())
        global+="ARRENDAMIENTOS\n\n";
    if (ui.excluir_arrcheckBox->isChecked())
        global+="EXCLUIDO ARRENDAMIENTOS\n\n";

    for (int veces=0;veces<ui.resumentableWidget->columnCount();veces++)
      {
        // if (ui.resumentableWidget->horizontalHeaderItem(veces)==NULL) continue;
        global+=ui.resumentableWidget->horizontalHeaderItem(veces)->text();
        if (veces<ui.resumentableWidget->columnCount()-1) global+="\t";
      }

    global+="\n";

     int linactu=0;
     while (linactu<ui.resumentableWidget->rowCount())
           {
            for (int veces=0; veces<ui.resumentableWidget->columnCount();veces++)
                if (ui.resumentableWidget->item(linactu,veces)!=NULL)
                 {
                   global+=ui.resumentableWidget->item(linactu,veces)->text();
                   if (veces<ui.resumentableWidget->columnCount()-1) global+="\t";
                 }
            global+="\n";
            linactu++;
           }

    cb->setText(global);
    QMessageBox::information( this, tr("Consulta retenciones"),
                              tr("Se ha pasado el contenido al portapapeles") );

}



void consret::copia_detalle()
{

    QClipboard *cb = QApplication::clipboard();
    QString global;

    global=nombreempresa();
    global+="\n";
    global+=tr("FECHA INICIAL:");
    global+="\t";
    global+=ui.inicialdateEdit->date().toString("dd/MM/yyyy");
    global+="\n";
    global+=tr("FECHA FINAL:");
    global+="\t";
    global+=ui.finaldateEdit->date().toString("dd/MM/yyyy");
    global+="\n\n";
    if (ui.arrendamientoscheckBox->isChecked())
        global+="ARRENDAMIENTOS\n\n";
    if (ui.excluir_arrcheckBox->isChecked())
        global+="EXCLUIDO ARRENDAMIENTOS\n\n";

    for (int veces=0;veces<ui.detalletableWidget->columnCount();veces++)
      {
        if (ui.detalletableWidget->isColumnHidden(veces)) continue;
        global+=ui.detalletableWidget->horizontalHeaderItem(veces)->text();
        if (veces<ui.detalletableWidget->columnCount()-1) global+="\t";
      }

    global+="\n";

     int linactu=0;
     while (linactu<ui.detalletableWidget->rowCount())
           {
            for (int veces=0; veces<ui.detalletableWidget->columnCount();veces++)
                if (ui.detalletableWidget->item(linactu,veces)!=NULL)
                 {
                   global+=ui.detalletableWidget->item(linactu,veces)->text();
                   if (veces<ui.detalletableWidget->columnCount()-1) global+="\t";
                 }
            global+="\n";
            linactu++;
           }

    cb->setText(global);
    QMessageBox::information( this, tr("Consulta retenciones"),
                              tr("Se ha pasado el contenido al portapapeles") );

}



void consret::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("retenciones.tex");
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\n";
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("INFORME RETENCIONES");
    if (ui.arrendamientoscheckBox->isChecked())
        stream << " ARRENDAMIENTOS\n\n";
    if (ui.excluir_arrcheckBox->isChecked())
        stream <<" EXCLUIDO ARRENDAMIENTOS\n\n";
    stream << "}}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {" << tr("Período de ") << ui.inicialdateEdit->date().toString("dd.MM.yyyy") <<
    tr(" a ") << ui.finaldateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
    stream << "\\end{center}" << "\n";


     // ----------------------------------------------------------------------------------------------
    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|l|c|p{5cm}|c|c|r|r|r|r|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("Cuenta Ret.") << "} & {\\scriptsize " << tr("Arrend.") <<
      "} & {\\scriptsize " << tr("Clave") << "} & {\\scriptsize " << tr("R.Especie")
      << "} & {\\scriptsize " << tr("Base")
      << "} & {\\scriptsize " <<
      tr("Retención") << "} & {\\scriptsize " << tr("Ing.Cta") <<
      "} & {\\scriptsize " << tr("Ing.Cta.Rep.") << "}  \\\\\n";
    stream << "\\hline\n";
    int pos=0;
    while (pos<ui.resumentableWidget->rowCount())
        {
          stream << "{\\scriptsize " << ui.resumentableWidget->item(pos,0)->text();
          stream << "} & {\\scriptsize " << ui.resumentableWidget->item(pos,1)->text();
          stream << "} & {\\scriptsize " << ui.resumentableWidget->item(pos,2)->text();
          stream << "} & {\\scriptsize " << ui.resumentableWidget->item(pos,3)->text();
          stream << "} & {\\scriptsize " << formateanumerosep(convapunto(
                  ui.resumentableWidget->item(pos,4)->text()).toDouble(),comadecimal,decimales);
          stream << "} & {\\scriptsize " << formateanumerosep(convapunto(
                  ui.resumentableWidget->item(pos,5)->text()).toDouble(),comadecimal,decimales);
          stream << "} & {\\scriptsize " << formateanumerosep(convapunto(
                  ui.resumentableWidget->item(pos,6)->text()).toDouble(),comadecimal,decimales);
          stream << "} & {\\scriptsize " << formateanumerosep(convapunto(
                  ui.resumentableWidget->item(pos,7)->text()).toDouble(),comadecimal,decimales);

         stream << "} \\\\ \n " << "\\hline\n";
         pos++;
       }
    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";

    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|l|r|l|r|}\n";
    stream << "\\hline\n";

    stream << "{\\scriptsize " << tr("Rtos.Dinerarios") << "} & {\\scriptsize "
            << formateanumerosep(convapunto(ui.dinerarioslineEdit->text()).toDouble(),comadecimal,decimales)
            << "} & {\\scriptsize " << tr("Rtos.Especie")
            << "} & {\\scriptsize "
            << formateanumerosep(convapunto(ui.especielineEdit->text()).toDouble(), comadecimal, decimales)
            << "} \\\\ \n " << "\\hline\n";

    stream << "{\\scriptsize " << tr("Retenciones") << "} & {\\scriptsize "
            << formateanumerosep(convapunto(ui.totalretlineEdit->text()).toDouble(), comadecimal,decimales)
            << "} & {\\scriptsize " << tr("Pagos a cuenta")
            << "} & {\\scriptsize "
            << formateanumerosep(convapunto(ui.total_a_ctalineEdit->text()).toDouble(), comadecimal, decimales)
            << "} \\\\ \n " << "\\hline\n";

    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";

    stream << "\\newpage\n";

    stream << "\\begin{landscape}\n";
    stream << "\\begin{center}\n";
    stream << "\\begin{longtable}{|r|l|l|l|l|p{2.5cm}|r|r|r|r|p{2.5cm}|l|l|r|}\n"
              "\\hline\n"
              "\\multicolumn{14}{|c|}\n"
              "{\\scriptsize{DETALLE RETENCIONES}} \\\\ \n"
              "\\hline{\\tiny }\n";
    QString linea;
    for (int veces=0; veces< ui.detalletableWidget->columnCount(); veces++)
        {
         linea+="{\\tiny {";
         linea+=ui.detalletableWidget->horizontalHeaderItem(veces)->text();
         linea+="}} ";
         if (veces<ui.detalletableWidget->columnCount()-1)
            {
             linea+=" & ";
            }
        }
    linea += "\\\\ \n";
    linea += "\\hline\n";
    stream << linea;
    stream << "\\endfirsthead\n";
    stream << linea;
    stream << "\\endhead\n\\hline\n";


    int linactu=0;
    while (linactu<ui.detalletableWidget->rowCount())
          {
           for (int veces=0; veces<ui.detalletableWidget->columnCount();veces++)
               if (ui.detalletableWidget->item(linactu,veces)!=NULL)
                {
                  stream << "{\\tiny {";
                  stream << ui.detalletableWidget->item(linactu,veces)->text();
                  stream << "}} ";
                  if (veces<ui.detalletableWidget->columnCount()-1) stream << " & ";
                }
           stream << "\\\\ \n";
           stream << "\\hline \n";
           linactu++;
          }

    stream << "\\end{longtable}\n";
    stream << "\\end{center}";
    stream << "\\end{landscape}";


    /*
    \begin{center}
    \begin{longtable}{|p{5cm}|r|r|r|r|r|r|r|r|r|r|r|}\hline\multicolumn{12}{|c|} {\scriptsize{ESTADO DE CAMBIOS EN EL PATRIMONIO NETO DE PYMES CORRESPONDIENTE AL EJERCICIO TERMINADO EL 31 DE DICIEMBRE DE 2010}} \\
    \hline{\tiny } & \multicolumn{1}{|p{1.2cm}|}{\tiny{Capital Escriturado}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Capital no exigido}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Prima de emisión}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Reservas}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{(Acciones y participaciones en patrimonio propias)}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Resultados de ejercicios anteriores}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Otras aportaciones de socios}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Resultado del ejercicio}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{(Dividendo a cuenta)}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Subvenciones donaciones y legados recibidos}} & {\tiny{TOTAL}} \\
    \hline\endfirsthead
    \hline{\tiny } & \multicolumn{1}{|p{1.2cm}|}{\tiny{Capital Escriturado}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Capital no exigido}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Prima de emisión}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Reservas}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{(Acciones y participaciones en patrimonio propias)}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Resultados de ejercicios anteriores}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Otras aportaciones de socios}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Resultado del ejercicio}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{(Dividendo a cuenta)}} & \multicolumn{1}{|p{1.2cm}|}{\tiny{Subvenciones donaciones y legados recibidos}} & {\tiny{TOTAL}} \\
    \hline
    \endhead
    {\tiny{A. SALDO, FINAL DEL AÑO 2008}} & {\tiny{30.000,00}} & {\tiny{}} & {\tiny{}} & {\tiny{}} & {\tiny{}} & {\tiny{}} & {\tiny{}} & {\tiny{-993,23}} & {\tiny{}} & {\tiny{}} & {\tiny{29.006,77}} \\
    \hline

    \end{longtable}
    \end{center}
    \end{landscape}

    */

     // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();


}


void consret::imprimir()
{
   generalatex();

   int valor=imprimelatex2(tr("retenciones"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir liquidación IVA"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir liquidación IVA"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir liquidación IVA"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}

void consret::edlatex()
{
    generalatex();
    int valor=editalatex("retenciones");
    if (valor==1)
        QMessageBox::warning( this, tr("LIQUIDACIÓN IVA"),tr("PROBLEMAS al llamar al editor Latex"));

}


void consret::ed_registro_ret() {

    if (ui.detalletableWidget->currentItem()==0) return;
    QString qapunte=ui.detalletableWidget->item(ui.detalletableWidget->currentRow(),0)->text();

    QString codigo=ui.detalletableWidget->item(ui.detalletableWidget->currentRow(),1)->text();

    QSqlQuery query2 = basedatos::instancia()->datos_ret_pase (qapunte);
    if (query2.isActive())
        if (query2.next())
         {
            // SELECT pase, cta_retenido, arrendamiento, clave_ret,
            // base_ret, tipo_ret, retencion, ing_cta, ing_cta_rep,
            // nombre, cif, provincia
            QString cta_retenido=query2.value(1).toString();
            bool ret_arrendamiento=query2.value(2).toBool();
            QString clave=query2.value(3).toString();
            QString base_percepciones=query2.value(4).toString();
            QString tipo_ret=query2.value(5).toString();
            QString qretencion=query2.value(6).toString();
            QString ing_cta=query2.value(7).toString();
            QString ing_cta_repercutido=query2.value(8).toString();
            QString nombre_ret=query2.value(9).toString();
            QString cif_ret= query2.value(10).toString();
            QString provincia=query2.value(11).toString();

            retencion *r = new retencion(comadecimal,decimales);
            r->pasadatos_all(codigo,
                               cta_retenido, ret_arrendamiento,
                               clave, base_percepciones,
                               tipo_ret, qretencion,
                               ing_cta, ing_cta_repercutido,
                               nombre_ret, cif_ret, provincia);
            if (r->exec() ==QDialog::Accepted) {
                r->recuperadatos(&cta_retenido, &ret_arrendamiento,
                                 &clave, &base_percepciones,
                                 &tipo_ret, &qretencion,
                                 &ing_cta, &ing_cta_repercutido,
                                 &nombre_ret, &cif_ret, &provincia);

                basedatos::instancia()->modifica_retencion (qapunte,
                                                  cta_retenido, ret_arrendamiento,
                                                  clave, base_percepciones,
                                                  tipo_ret, qretencion,
                                                  ing_cta, ing_cta_repercutido,
                                                  nombre_ret, cif_ret, provincia);
            }
            delete(r);
        }
    refrescar();

}


void consret::cambiacuenpase()
{

    if (ui.detalletableWidget->currentItem()==0) return;
    QString qapunte=ui.detalletableWidget->item(ui.detalletableWidget->currentRow(),0)->text();


  cambiacuentapase *c = new cambiacuentapase();


  QString codigo=ui.detalletableWidget->item(ui.detalletableWidget->currentRow(),1)->text();
  QDate qfecha=basedatos::instancia()->select_fecha_diariopase (qapunte);

  QSqlQuery q1 =basedatos::instancia()->debe_haber_pase(qapunte);
  double ddebe=0, dhaber=0;
  if (q1.isActive())
      if (q1.next()) {
        ddebe=q1.value(0).toDouble();
        dhaber=q1.value(1).toDouble();
      }
  QString qdebe; qdebe.setNum(ddebe);
  QString qhaber; qhaber.setNum(dhaber);

  c->pasainfo( qapunte, codigo, qfecha, qdebe, qhaber );
  c->exec();
  delete(c);
  refrescar();
}


void consret::actufechas()
{
  QString cadcombo=ui.periodocomboBox->currentText();
  QString periodo=cadcombo.section(' ',0,0);

  QDate fechaini=inicioejercicio(ui.ejerciciocomboBox->currentText());
  QDate fechafin=finejercicio(ui.ejerciciocomboBox->currentText());
  int anyo=fechaini.year();

    if (periodo=="1T")
      {
        fechaini.setDate(anyo,1,1);
        fechafin.setDate(anyo,3,31);
      }
    if (periodo=="2T")
     {
        fechaini.setDate(anyo,4,1);
        fechafin.setDate(anyo,6,30);
     }
    if (periodo=="3T")
     {
        fechaini.setDate(anyo,7,1);
        fechafin.setDate(anyo,9,30);
     }
    if (periodo=="4T")
     {
        fechaini.setDate(anyo,10,1);
        fechafin.setDate(anyo,12,31);
     }

    if (!periodo.contains('T') && !periodo.contains("0A"))
      {
        int mes=periodo.toInt();
        if (mes>0)
          {
            fechaini.setDate(anyo,mes,1);
            if (mes<12)
              {
               fechafin.setDate(anyo,mes+1,1);
               fechafin=fechafin.addDays(-1);
              }
              else
                   {
                     fechafin.setDate(anyo,12,31);
                   }
          }
      }

  ui.inicialdateEdit->setDate(fechaini);
  ui.finaldateEdit->setDate(fechafin);
}

void consret::on_detalletableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    Q_UNUSED(currentColumn);
    Q_UNUSED(currentRow);
    int fcurrentRow=ui.detalletableWidget->currentRow();
    if (fcurrentRow>0 || fcurrentRow+1<ui.detalletableWidget->rowCount()) {
        QColor fondo= ui.detalletableWidget->item(fcurrentRow,0)->background().color();
        bool darkMode=(QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
        if (fondo==background_alt(darkMode)) {
            ui.edasientopushButton->setEnabled(true);
            ui.edret_pushButton->setEnabled(true);
            ui.borra_asientolineEdit->setEnabled(true);
            return;
        }
    }
    ui.edasientopushButton->setEnabled(false);
    ui.edret_pushButton->setEnabled(false);
    ui.borra_asientolineEdit->setEnabled(false);
}

