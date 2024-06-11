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

#include "presupuesto_ed.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlRecord>
#include "presupcuenta.h"
#include <QMessageBox>
#include <QProgressDialog>

CustomSqlModeledpres::CustomSqlModeledpres(QObject *parent)
        : QSqlQueryModel(parent)
    {
     comadecimal=true; sindecimales=false;
    }

void CustomSqlModeledpres::pasainfo(bool qcomadecimal, bool qsindecimales)
{
 comadecimal=qcomadecimal;
 sindecimales=qsindecimales;
}

QVariant CustomSqlModeledpres::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 4 || index.column()==5 || index.column()==6)
	       {
   	        if (value.toDouble()<0.0001 && value.toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        return value.fromValue(formateanumerosep(value.toDouble(),comadecimal,!sindecimales));
                      }
	       }
        }
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 4 || index.column()==5 ||
              index.column() == 6))
           return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 2 || index.column()==3))
           return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignCenter));
        return value;
    }


presupuesto_ed::presupuesto_ed(bool qsubcuentas) : QDialog() {
    ui.setupUi(this);

    subcuentas=qsubcuentas;
    radioactual=3;
    QSqlTableModel *modelo = new QSqlTableModel;
    modelo->setTable("plancontable");
    QString cadena;
    if (!cod_longitud_variable())
    {
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            if (subcuentas) cadena="length(codigo)=";
            else cadena="length(codigo)<";
        }
        else {
            if (subcuentas) cadena="char_length(codigo)=";
            else cadena="char_length(codigo)<";
        }
        QString cadnum;
        cadnum.setNum(anchocuentas());
        cadena += cadnum;
    }
    else
    {
        if (subcuentas) cadena = "auxiliar";
        else cadena = "not auxiliar";
    }

     modelo->setFilter(cadena);
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("CUENTA"));

     modeloplan=modelo;
     ui.tableView->setModel(modelo);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,250);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui.tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
     ui.tableView->show();

     ui.pretableView->setColumnWidth(1,200);


        CustomSqlModeledpres *elmodelo = new CustomSqlModeledpres;
        comadecimal=haycomadecimal(); sindecimales=!haydecimales();

        modelpre = elmodelo;

        if (subcuentas) modelpre->setHeaderData(0, Qt::Horizontal, tr("SUBCUENTA"));
           else modelpre->setHeaderData(0, Qt::Horizontal, tr("CUENTA"));
        modelpre->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
        modelpre->setHeaderData(2, Qt::Horizontal, tr("Ej.BASE"));
        modelpre->setHeaderData(3, Qt::Horizontal, tr("Pres.BASE"));
        modelpre->setHeaderData(4, Qt::Horizontal, tr("BASE"));
        modelpre->setHeaderData(5, Qt::Horizontal, tr("PRESUPUESTO"));
        modelpre->setHeaderData(6, Qt::Horizontal, tr("INCREMENTO (%)"));

        ui.pretableView->setModel(modelpre);
        ui.pretableView->setAlternatingRowColors ( true);

     QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
     QStringList ej1;
     if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
           }
     // ej1 << noejercicio();
     ui.ejerciciosimportarcomboBox->addItems(ej1);
     ui.ejerciciobasecomboBox->addItems(ej1);
     ui.ejerciciopresupuestobasecomboBox->addItems(ej1);

     connect(ui.refrescarpushButton,SIGNAL(clicked()),this,SLOT(refrescalistacuentas()));
     connect(ui.adicselpushButton,SIGNAL(clicked()),this,SLOT(pasaselec()));
     connect(ui.salirpushButton,SIGNAL(clicked()),this,SLOT(aceptar()));
     connect(ui.editarpushButton,SIGNAL(clicked()),this,SLOT(presupuestocuenta()));
     connect(ui.refrescaprepushButton,SIGNAL(clicked()),this,SLOT(refrescapres()));
     connect(ui.borratodopushButton,SIGNAL(clicked()),this,SLOT(borratodo()));
     connect(ui.incrementolineEdit,SIGNAL( editingFinished()),this,SLOT(incremento_fin_edic()));
     connect(ui.borralinpushButton,SIGNAL( clicked()),this,SLOT(borralinea()));
     connect(ui.adictodaspushButton,SIGNAL( clicked()),this,SLOT(pasatodasctas()));
     connect(ui.importarpushButton,SIGNAL( clicked()),this,SLOT(importacuentaspresupuesto()));
     connect(ui.imprimirpushButton,SIGNAL( clicked()),this,SLOT(imprime()));
}

void presupuesto_ed::refrescalistacuentas()
{
    QString cadena;
    if (!cod_longitud_variable())
    {
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            if (subcuentas) cadena="length(codigo)=";
            else cadena="length(codigo)<";
        }
        else {
            if (subcuentas) cadena="char_length(codigo)=";
            else cadena="char_length(codigo)<";
        }
        QString cadnum;
        cadnum.setNum(anchocuentas());
        cadena+=cadnum;
    }
    else
    {
        if (subcuentas) cadena="auxiliar";
        else cadena="not auxiliar";
    }

   QString cadenacuentas=ui.listacuentaslineEdit->text();
   int partes=cadenacuentas.count(',');
   if (cadenacuentas.length()>0)
   {
      for (int veces=0;veces<=partes;veces++)
       {
          if (veces==0) cadena+=" and (";
          if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            cadena += " codigo like '"+ cadenacuentas.section(',',veces,veces).replace("'","''") +"%' ";
          }
          else {
            cadena+="position('";
            cadena+=cadenacuentas.section(',',veces,veces).replace("'","''");
            cadena+="' in codigo)=1";
          }
          if (veces==partes) cadena+=")";
              else cadena+=" or ";
       }
   }

  modeloplan->clear();
  modeloplan->setTable("plancontable");
  modeloplan->setFilter(cadena);
  modeloplan->select();

  return;
}


void presupuesto_ed::pasaejercicio(QString qejercicio)
{
 ejercicio=qejercicio;
 QString cadena="PRESUPUESTO EJERCICIO: ";
 cadena+=qejercicio;
 ui.titulolabel->setText(cadena);

 QSqlQuery query = basedatos::instancia()->select4Ejercicioscodigo(qejercicio);
 if ( (query.isActive()) && (query.next()) )
   cargaespec(query.value(0).toString(),query.value(1).toString(),query.value(2).toBool(),
            query.value(3).toString());

 modelpre->pasainfo(comadecimal, sindecimales);

 QSqlQuery query2 = basedatos::instancia()->select6casepresupuestoplancontableejercicio(ejercicio, subcuentas);
 modelpre->setQuery(query2);

    if (basedatos::instancia()->siCuentaenpresupuestoejercicio(ejercicio, subcuentas)) desactivaimportpres();
    else activaimportpres();
 // ajustacolumnaspre();
}

presupuesto_ed::~presupuesto_ed()
{
  delete(modeloplan);
}

void presupuesto_ed::grabaespec()
{
    basedatos::instancia()->updateEjercicios4codigo( ui.baseejercicioradioButton->isChecked() , ui.ejerciciobasecomboBox->currentText() , ui.basepresupuestoradioButton->isChecked() , ui.ejerciciopresupuestobasecomboBox->currentText() , ui.base0radioButton->isChecked() , ui.incrementolineEdit->text() , ejercicio );
}

void presupuesto_ed::aceptar()
{
 grabaespec();
 accept();
}

void presupuesto_ed::cargaespec(QString base_ejercicio, QString base_presupuesto,
                                bool presupuesto_base_cero, QString incremento)
{
  //ui.baseejercicioradioButton->disconnect(SIGNAL(toggled(bool)));
  //ui.basepresupuestoradioButton->disconnect(SIGNAL(toggled(bool)));
  //ui.base0radioButton->disconnect(SIGNAL(toggled(bool)));
 if (base_ejercicio.length()==0) 
   {
    // ui.ejerciciobasecomboBox->setEnabled(false);
   }
   else
       {
        for (int veces=0;veces<ui.ejerciciobasecomboBox->count();veces++)
           {
            if (ui.ejerciciobasecomboBox->itemText(veces)==base_ejercicio) 
              {
               ui.ejerciciobasecomboBox->setCurrentIndex(veces);
               ui.baseejercicioradioButton->setChecked(true);
               radioactual=1;
               break;
              }
           }
       }

 if (base_presupuesto.length()==0) 
   {
    // ui.ejerciciopresupuestobasecomboBox->setEnabled(false);
   }
   else
        {
         for (int veces=0;veces<ui.ejerciciopresupuestobasecomboBox->count();veces++)
            {
             if (ui.ejerciciopresupuestobasecomboBox->itemText(veces)==base_presupuesto) 
               {
                ui.ejerciciopresupuestobasecomboBox->setCurrentIndex(veces);
                ui.basepresupuestoradioButton->setChecked(true);
                radioactual=2;
                break;
               }
            }
        }

  if (presupuesto_base_cero)
     { ui.base0radioButton->setChecked(true); radioactual=3; 
     }

  ui.incrementolineEdit->setText(formateanumero(incremento.toDouble(),comadecimal,!sindecimales));

}

void presupuesto_ed::pasaselec()
{
 QItemSelectionModel *seleccion;
 seleccion=ui.tableView->selectionModel();
 if (seleccion->hasSelection())
    {
      QList<QModelIndex> listacuentas;
      listacuentas= seleccion->selectedRows ( 0 );
      for (int veces=0;veces<listacuentas.size();veces++)
          {
           QVariant value=modeloplan->data(listacuentas.at(veces),Qt::DisplayRole);
           QString cta=value.toString();
           if (!existectapres(cta,ejercicio))
              {
               // insertamos cuenta en presupuesto
               basedatos::instancia()->insertPresupuestocuentaejercicio(cta, ejercicio);
               if (ui.baseejercicioradioButton->isChecked())
                  {
                   calcbase(cta);
                  }
               if (ui.basepresupuestoradioButton->isChecked())
                  {
                   calcbasepresupuesto(cta);
                  }
              }
           calcpresupuesto(cta);
          }
    }
 refrescapres();
 if (modelpre->rowCount()>0) desactivaimportpres();
}


void presupuesto_ed::calcbase(QString cuenta)
{
    QSqlQuery query;
    // calculamos campo base, luego base1 a base15

    query = basedatos::instancia()->selectSumdebesumhaberdiariocuentafechasnocierrenoregularizacion(cuenta, 
         inicioejercicio(ui.ejerciciobasecomboBox->currentText()),
         finejercicio(ui.ejerciciobasecomboBox->currentText()));
    if ( (query.isActive()) && (query.first()) ) {
        double valor= query.value(0).toDouble();
        QString cadnumero=formateanumero(valor,false,true);
        basedatos::instancia()->updatePresupuestobaseejerciciobaseejerciciocuenta( cadnumero,
        ui.ejerciciobasecomboBox->currentText() , ejercicio , cuenta );
    }

   query = basedatos::instancia()->selectIniciofinperiodosejercicioorderinicio(
                                          ui.ejerciciobasecomboBox->currentText() );
   int pos=1;
   if (query.isActive())
   while (query.next())
       {
        QString cadnum;
        cadnum.setNum(pos);

        QSqlQuery query2 =
              basedatos::instancia()->selectSumdebesumhaberdiariocuentafechasnocierrenoregularizacion(
                                            cuenta, query.value(0).toDate() , query.value(1).toDate() );
        if ( (query2.isActive()) && (query2.first()) )
           {
            QString valor=query2.value(0).toString();
            if (valor.length()==0) valor="0";
            basedatos::instancia()->updatePresupuestobaseejerciciocuenta(cadnum, valor , ejercicio, cuenta);
           }
        pos++;
       }
}

void presupuesto_ed::presupuestocuenta()
{
    if (!ui.pretableView->currentIndex().isValid())
       {
         QMessageBox::warning( this, tr("EDICIÓN DE PRESUPUESTO"),
         tr("Para editar presupuesto hay que seleccionar una cuenta del cuadro"));
         return;
       }
    int fila=ui.pretableView->currentIndex().row();
    QString cuenta=modelpre->record(fila).value("cuenta").toString();
    presupcuenta *p = new presupcuenta(ejercicio,cuenta);
    p->exec();
    delete(p);
    refrescapres();
    ir_fila_pre(fila);
}

void presupuesto_ed::refrescapres()
{
        delete(modelpre);
        CustomSqlModeledpres *elmodelo = new CustomSqlModeledpres;

        modelpre = elmodelo;

        if (subcuentas) modelpre->setHeaderData(0, Qt::Horizontal, tr("SUBCUENTA"));
           else modelpre->setHeaderData(0, Qt::Horizontal, tr("CUENTA"));
        modelpre->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
        modelpre->setHeaderData(2, Qt::Horizontal, tr("Ej.BASE"));
        modelpre->setHeaderData(3, Qt::Horizontal, tr("Pres.BASE"));
        modelpre->setHeaderData(4, Qt::Horizontal, tr("BASE"));
        modelpre->setHeaderData(5, Qt::Horizontal, tr("PRESUPUESTO"));
        modelpre->setHeaderData(6, Qt::Horizontal, tr("INCREMENTO (%)"));

        ui.pretableView->setModel(modelpre);
        ui.pretableView->setAlternatingRowColors ( true);

 modelpre->pasainfo(comadecimal, sindecimales);

 QSqlQuery query2 = basedatos::instancia()->select6casepresupuestoplancontableejercicio(ejercicio, subcuentas);
 modelpre->setQuery(query2);


// QSqlQuery p=modelpre->query(); 
// p.exec();
// modelpre->setQuery(p);
}


void presupuesto_ed::borratodo()
{
    if (QMessageBox::question(this,
           tr("Borrar presupuesto"),
                           tr("¿ Desea borrar toda la información del presupuesto ?")) == QMessageBox::No )
    return ;

    basedatos::instancia()->deletePresupuestoejerciciosubcuentas(ejercicio,subcuentas);

    refrescapres();
    // ui.cargarimportespushButton->setEnabled(true);
    if (modelpre->rowCount()==0) activaimportpres();

}


void presupuesto_ed::incremento_fin_edic()
{
 ui.incrementolineEdit->setText(formateanumero(ui.incrementolineEdit->text().toDouble(),
                                comadecimal,!sindecimales));
}

void presupuesto_ed::borralinea()
{
   if (!ui.pretableView->currentIndex().isValid())
       {
         QMessageBox::warning( this, tr("BORRAR LÍNEAS DE PRESUPUESTO"),
         tr("Para borrar una línea hay que seleccionarla del cuadro"));
         return;
       }
  if (QMessageBox::question(this,
           tr("Borrar líneas"),
           tr("¿ Desea borrar la línea seleccionada ?")) == QMessageBox::No )
     return ;

  int fila=ui.pretableView->currentIndex().row();
  QString cuenta=modelpre->record(fila).value("cuenta").toString();

  basedatos::instancia()->deletePresupuestoejerciciocuenta(ejercicio, cuenta);
  refrescapres();
  if (modelpre->rowCount()==0) activaimportpres();
}

void presupuesto_ed::pasatodasctas()
{
 int fila=0;

 QProgressDialog progreso(tr("Cargando información ..."), 0, 0, modeloplan->rowCount()-1);
 progreso.setWindowModality(Qt::WindowModal);

 while (fila<modeloplan->rowCount())
      {
       progreso.setValue(fila);
       QApplication::processEvents();
       QString cta=modeloplan->record(fila).value("codigo").toString();
       if (!existectapres(cta,ejercicio))
              {
               // insertamos cuenta en presupuesto
               QString cadquery;
               basedatos::instancia()->insertPresupuestocuentaejercicio(cta,ejercicio);

               if (ui.baseejercicioradioButton->isChecked())
                  {
                   calcbase(cta);
                  }
               if (ui.basepresupuestoradioButton->isChecked())
                  {
                   calcbasepresupuesto(cta);
                  }
               calcpresupuesto(cta);
              }
       fila++;
      }
 refrescapres();
 if (modelpre->rowCount()>0) desactivaimportpres();
}


void presupuesto_ed::desactivaimportpres()
{
 ui.importarlabel->setEnabled(false);
 ui.ejerciciosimportarcomboBox->setEnabled(false);
 ui.importarpushButton->setEnabled(false);
}

void presupuesto_ed::activaimportpres()
{
 ui.importarlabel->setEnabled(true);
 ui.ejerciciosimportarcomboBox->setEnabled(true);
 ui.importarpushButton->setEnabled(true);
}


void presupuesto_ed::importacuentaspresupuesto()
{
 QSqlQuery query = basedatos::instancia()->selectCuentapresupuestoejercicio( ui.ejerciciosimportarcomboBox->currentText() );
 if (query.isActive())
 while (query.next())
   {
    QString cta=query.value(0).toString();
    if (!existectapres(cta,ejercicio))
        {
          // insertamos cuenta en presupuesto
          QString cadquery;
          basedatos::instancia()->insertPresupuestocuentaejercicio(cta,ejercicio);
          if (ui.baseejercicioradioButton->isChecked())
                  {
                   calcbase(cta);
                  }
          if (ui.basepresupuestoradioButton->isChecked())
                 {
                   calcbasepresupuesto(cta);
                 }
          calcpresupuesto(cta);
         }
    }
 refrescapres();
}


void presupuesto_ed::calcbasepresupuesto(QString cuenta)
{
    // calculamos campo base, luego base1 a base15, después según el campo
    // porcentaje presupuesto y presupuesto1 a presupuesto15

    QSqlQuery query = basedatos::instancia()->selectPresupuestospresupuestocuentaejercicio(cuenta,
        ui.ejerciciopresupuestobasecomboBox->currentText());

    if ( (query.isActive()) && (query.first()) ) {
        QStringList lista;
        for (int i=0;i<16;i++)
           {
             double valor= query.value(i).toDouble();
             QString cadnumero=formateanumero(valor,false,true);
             lista << cadnumero;
           }
        basedatos::instancia()->updatePresupuestobasespresupuestobaseejerciciocuenta(lista,
          ui.ejerciciopresupuestobasecomboBox->currentText() ,ejercicio, cuenta);
    }
}


void presupuesto_ed::calcpresupuesto(QString cuenta)
{
    if (ui.incrementolineEdit->text().length()==0) return;
    basedatos::instancia()->updatePresupuestopresupuestoincrementoejerciciocuenta( ui.incrementolineEdit->text() , ejercicio, cuenta );

}



void presupuesto_ed::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("def_presupuesto.tex");
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
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << filtracad(mensaje) << "}}";
    stream << "\\end{center}";
// ***************************************************************************************** 
    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|p{5.5cm}|l|l|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";
   QString cadena;
   stream << "\\multicolumn{7}{|c|} {\\textbf{";
   cadena=tr("DEFINICIÓN PRESUPUESTO");
    // --------------------------------------------------------------------------------------
    stream << cadena;
    stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // --------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("CUENTA") << "}} & ";
    stream << "{\\tiny{" << tr("DESCRIPCIÓN") << "}} & ";
    stream << "{\\tiny{" << tr("Ej.BASE") << "}} & ";
    stream << "{\\tiny{" << tr("Pres.BASE") << "}} & ";
    stream << "{\\tiny{" << tr("BASE") << "}} & ";
    stream << "{\\tiny{" << tr("PRESUP.") << "}} & ";
    stream << "{\\tiny{" << tr("INC(\\%)") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\tiny{" << tr("CUENTA") << "}} & ";
    stream << "{\\tiny{" << tr("DESCRIPCIÓN") << "}} & ";
    stream << "{\\tiny{" << tr("Ej.BASE") << "}} & ";
    stream << "{\\tiny{" << tr("Pres.BASE") << "}} & ";
    stream << "{\\tiny{" << tr("BASE") << "}} & ";
    stream << "{\\tiny{" << tr("PRESUP.") << "}} & ";
    stream << "{\\tiny{" << tr("INC(\\%)") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    while (fila<modelpre->rowCount())
          {
             stream << "{\\tiny ";
             stream << modelpre->record(fila).value("cuenta").toString();
             stream << "} & {\\tiny ";
             stream << filtracad(descripcioncuenta(modelpre->record(fila).value("cuenta").toString()));
             stream << "} & {\\tiny ";
             stream << filtracad(modelpre->record(fila).value("ejerciciobase").toString());
             stream << "} & {\\tiny ";
             stream << filtracad(modelpre->record(fila).value("presupuestobase").toString());
             stream << "} & {\\tiny ";
             stream << formateanumerosep(modelpre->record(fila).value("base").toDouble(),true,true);
             stream << "} & {\\tiny ";
             stream << formateanumerosep(modelpre->record(fila).value("presupuesto").toDouble(),true,true);
             stream << "} & {\\tiny ";
             double base=modelpre->record(fila).value("base").toDouble();
             double presupuesto=modelpre->record(fila).value("presupuesto").toDouble();
             QString cadnum;
             if (base<-0.001 || base>0.001) formateanumero((presupuesto-base)/base*100,true,true);
             stream << cadnum;
             stream << "} \\\\ \n  " << "\\hline\n";
             fila++;
          }

    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}

void presupuesto_ed::imprime()
{

   generalatex();
   int valor=imprimelatex2(tr("def_presupuesto"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir presupuesto"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir presupuesto"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir presupuesto"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


int presupuesto_ed::fila_tabla_pre() {
    int fila=0;
    if (!ui.pretableView->currentIndex().isValid()) return 0;
    fila=ui.pretableView->currentIndex().row();
    return fila;
}


void presupuesto_ed::ir_fila_pre(int fila) {
    if (fila<modelpre->rowCount()) ui.pretableView->selectRow(fila);
}
