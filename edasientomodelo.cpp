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

#include "edasientomodelo.h"
#include "funciones.h"
#include "basedatos.h"
#include <QCheckBox>
#include <QMessageBox>

edasientomodelo::edasientomodelo() : QDialog() {
    ui.setupUi(this);


 QStringList columnas;
 columnas << tr("VARIABLE") << tr("TIPO") << tr("CONCEPTO") << tr("VAL.DEFECTO") << tr("GUARD.");
 ui.variablestable->setHorizontalHeaderLabels(columnas);

  QStringList tipos;
  tipos << tipoNumerico() << tipoFecha() << tipoCaracteres() << tipoSubcuenta() << tipoNIF();
  if (conanalitica()) tipos << tipoCI();
  tipos << tipoCALC(); tipos << tipoConceptoSII();

  for (int veces=0;veces<50;veces++)
     {
       QComboBox  *comboitem = new QComboBox();
       comboitem->addItems(tipos);
       ui.variablestable->setCellWidget ( veces, 1, comboitem );
       connect((QComboBox*)ui.variablestable->cellWidget(veces,1),
         SIGNAL(currentIndexChanged ( int )),this,SLOT(activacambiado()));

       QCheckBox *checkitem = new QCheckBox();
       ui.variablestable->setCellWidget ( veces, 4, checkitem );
       connect((QCheckBox*)ui.variablestable->cellWidget(veces,4),
         SIGNAL(stateChanged ( int )),this,SLOT(activacambiado()));

       ui.variablestable->setColumnWidth(4,50);  // subcuenta

       /*QTableWidgetItem *newItem = new QTableWidgetItem("");
       ui.variablestable->setItem(veces,0,newItem);
       QTableWidgetItem *newItem2 = new QTableWidgetItem("");
       ui.variablestable->setItem(veces,2,newItem2);*/
     }


 QStringList columnas2;
 columnas2 << tr("CUENTA") << tr("CONCEPTO") << tr("EXPRESIÓN");
 columnas2 << tr("D/H") << tr("C.I.") << tr("BASE IVA");
 columnas2 << tr("CUENTA B.I.") << tr("CUENTA FRA.") << tr("CLAVE IVA");
 columnas2 << tr("DOCUMENTO") << tr("FECHA FRA.");
 columnas2 << tr("PRORRATA E.") << tr("RECTIFICATIVA"); // << tr("AUTOFACTURA");
 columnas2 << tr("FECHA OP.") << tr("CLAVE OP.") << tr("BI a COSTE");
 columnas2 << tr("TIPO:RECTIFICADA") << tr("NUM.DOCS") << tr("DOC.INI") <<  tr("DOC.FIN");
 columnas2 << tr("BIEN INVERSIÓN") << tr("AFECTACIÓN") << tr("AGR/PESC")
           << tr("NOMBRE") << tr("CIF");
 columnas2 << tr("CTA_RETENIDO") << tr("ARRENDAM.") << tr ("CLAVE");
 columnas2 << tr("BASE_RET") << tr("TIPO_RET") << tr ("RETENCIÓN");
 columnas2 << tr("ING_A_CTA") << tr("ING_A_CTA_REP") << tr ("NOMBRE_RET");
 columnas2 << tr("CIF_RET") << tr("PROVINCIA");

 ui.detalletable->setHorizontalHeaderLabels(columnas2);

  if (!conanalitica()) ui.detalletable->hideColumn(4);
  QStringList dh;
  dh << tipoDEBE() << tipoHABER();
  for (int veces=0;veces<50;veces++)
     {
       QComboBox  *comboitem2 = new QComboBox();
       comboitem2->addItems(dh);
       ui.detalletable->setCellWidget(veces,3,comboitem2);
       connect((QComboBox*)ui.detalletable->cellWidget(veces,3),
         SIGNAL(currentIndexChanged ( int )),this,SLOT(activacambiado()));

       QCheckBox *checkitem = new QCheckBox(tr("Activado"));
       ui.detalletable->setCellWidget ( veces, 12, checkitem );
       connect((QCheckBox*)ui.detalletable->cellWidget(veces,12),
         SIGNAL(stateChanged ( int )),this,SLOT(activacambiado()));

       /* QCheckBox *checkitem2 = new QCheckBox(tr("Activado"));
       ui.detalletable->setCellWidget ( veces, 13, checkitem2 );
       connect((QCheckBox*)ui.detalletable->cellWidget(veces,13),
         SIGNAL(stateChanged ( int )),this,SLOT(activacambiado())); */

       QCheckBox *checkitem3 = new QCheckBox(tr("Activado"));
       ui.detalletable->setCellWidget ( veces, 20, checkitem3 );
       connect((QCheckBox*)ui.detalletable->cellWidget(veces,20),
         SIGNAL(stateChanged ( int )),this,SLOT(activacambiado()));


       QCheckBox *checkitem2 = new QCheckBox(tr("Activado"));
       ui.detalletable->setCellWidget ( veces, 22, checkitem2 );
       connect((QCheckBox*)ui.detalletable->cellWidget(veces,22),
         SIGNAL(stateChanged ( int )),this,SLOT(activacambiado()));

       QCheckBox *checkitem4 = new QCheckBox(tr("Arrendamiento"));
       ui.detalletable->setCellWidget ( veces, 26, checkitem4 );
       connect((QCheckBox*)ui.detalletable->cellWidget(veces,26),
         SIGNAL(stateChanged ( int )),this,SLOT(activacambiado()));


       QComboBox  *comboitem22 = new QComboBox();
       comboitem22->addItems(basedatos::instancia()->listaoperaciones_ret());
       ui.detalletable->setCellWidget(veces,27,comboitem22);
       connect((QComboBox*)ui.detalletable->cellWidget(veces,27),
         SIGNAL(currentIndexChanged ( int )),this,SLOT(activacambiado()));

     }

  QStringList tipos_automaticos=basedatos::instancia()->tipos_automaticos();
  ui.tipocomboBox->addItems(tipos_automaticos);
  tipo_combo_cambiado();

   QStringList tiposiva;
   QSqlQuery query = basedatos::instancia()->selectClavetiposiva();
   if (query.isActive())
   while ( query.next() )
    {
      tiposiva << query.value(0).toString();
    }
   tiposiva << tipo_exento();
   tiposiva << tipo_nosujeto();
   tiposiva << tipo_exento_nodeduc();

   for (int veces=0;veces<50;veces++)
     {
       QComboBox  *comboitem3 = new QComboBox();
       comboitem3->addItems(tiposiva);
       ui.detalletable->setCellWidget(veces,8,comboitem3);
       connect((QComboBox*)ui.detalletable->cellWidget(veces,8),
         SIGNAL(currentIndexChanged ( int )),this,SLOT(activacambiado()));
     }

   cambios=false;

   comadecimal=haycomadecimal();

   connect(ui.variablestable,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(celdavarcambiada ( int)));

   connect(ui.borrarlineavarpushButton,SIGNAL(clicked()),this,SLOT(borralineavar()));

   connect(ui.detalletable,SIGNAL( currentCellChanged ( int , int, int, int )),this,
           SLOT(detalleposcambiada ( int, int )));

   connect(ui.borrardetlinpushButton,SIGNAL( clicked()),this,
           SLOT(borralineadetalle()));

   connect(ui.formulatextEdit,SIGNAL(textChanged()),this,SLOT(formulacambiada()));

   connect(ui.detalletable,SIGNAL(cellChanged ( int, int )),this,SLOT(
             detallecontenidocambiado( int, int)));

   connect(ui.guardarpushButton,SIGNAL( clicked()),this,
           SLOT(guardarinfo()));

   connect(ui.salirpushButton,SIGNAL( clicked()),this,
           SLOT(botonsalir()));

   connect(ui.variablestable,SIGNAL(cellChanged ( int, int )),this,SLOT(
             cambiosvar(int,int)));

   connect(ui.modelolineEdit,SIGNAL(textChanged(QString)),this,SLOT(activacambiado()));

   connect(ui.fechalineEdit,SIGNAL(textChanged(QString)),this,SLOT(activacambiado()));

   connect(ui.arribapushButton,SIGNAL( clicked()),this,
           SLOT(muevearriba()));

   connect(ui.abajopushButton,SIGNAL( clicked()),this,
           SLOT(mueveabajo()));

   connect(ui.tipocomboBox,SIGNAL( currentIndexChanged(int)),this,
           SLOT(tipo_combo_cambiado()));

   connect(ui.entregasgroupBox,SIGNAL(toggled(bool)),this,SLOT(entregasboxpulsado()));

   connect(ui.adqgroupBox,SIGNAL(toggled(bool)),this,SLOT(adqboxpulsado()));
 // ui.guardarpushButton->setEnabled(true);

   connect(ui.eibradioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));
   connect(ui.eib_serv_radioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));
   connect(ui.AIBradioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));
   connect(ui.autofactura_radioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));
   connect(ui.autofactura_no_ue_radioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));
   connect(ui.diariolineEdit,SIGNAL(textChanged(QString)),this,SLOT(activacambiado()));


   connect(ui.exportradioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));
   connect(ui.importradioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));
   connect(ui.isp_inter_radioButton,SIGNAL(toggled(bool)),this,SLOT(activacambiado()));

}


void edasientomodelo::celdavarcambiada( int row)
{
    ui.variablestable->setFocus();
    // if (ui.variablestable->currentColumn()==1)
    //   QMessageBox::warning( this, tr("Edición de asiento modelo"),
    //			tr("HOLA 1" ));

    if  (row>0 && ui.variablestable->item(row-1,0)==0) 
          { ui.variablestable->setCurrentCell(row-1,0); return; }

    if  (row>0 && ui.variablestable->item(row-1,0)->text().length()==0) 
          { ui.variablestable->setCurrentCell(row-1,0); return; }
}


void edasientomodelo::borralineavar()
{
    int fila;
    fila=ui.variablestable->currentRow();
    ui.variablestable->removeRow(fila);
    if (fila<ui.variablestable->rowCount()) ui.variablestable->setCurrentCell(fila,0);
    activacambiado();
}


void edasientomodelo::cambiosvar(int row,int col)
{
    if (row>=0 && col==3)
       {
        QComboBox *comboitem;
        comboitem=(QComboBox*)ui.variablestable->cellWidget(row,1);
        if (comboitem->currentText()==tipoNumerico())
           {
             QString contenido;
             if (ui.variablestable->item(row,col)!=0)
                if (ui.variablestable->item(row,col)->text().length()>0)
                   {
                    if (comadecimal)
                       {
                        contenido=ui.variablestable->item(row,col)->text();
                        ui.variablestable->item(row,col)->setText(convacoma(contenido));
                       }
                   }
           }
       }

    if (row>=0 && col==3)
       {
        QComboBox *comboitem;
        comboitem=(QComboBox*)ui.variablestable->cellWidget(row,1);
        if (comboitem->currentText()==tipoFecha())
           {
             QString contenido;
             if (ui.variablestable->item(row,col)!=0)
                if (ui.variablestable->item(row,col)->text().length()>0)
                   {
                     contenido=ui.variablestable->item(row,col)->text();
                     // comprobamos que la fecha sea correcta
                     QDate fecha;
                     fecha=fecha.fromString(contenido,"dd/MM/yyyy");
                     if (!fecha.isValid())
                       {
                        QMessageBox::warning( this, tr("Edición de asiento automático"),
                          tr("ERROR: Fecha incorrecta: """)+contenido+tr(""" el formato ha de ser dd/MM/aaaa"));
                        ui.variablestable->item(row,col)->setText("");
                       }

                   }
           }
       }

     activacambiado();
}

void edasientomodelo::activacambiado()
{

 if (!cambios && !windowTitle().contains("(*)")) setWindowTitle(windowTitle()+QString(" (*)"));
 cambios=true;
 ui.guardarpushButton->setEnabled(true);

}


void edasientomodelo::detalleposcambiada( int row, int col)
{
   // if (col==3 || col==8) activacambiado();
   ui.detalletable->disconnect(SIGNAL(cellChanged(int,int)));
   ui.formulatextEdit->disconnect(SIGNAL(textChanged()));
    ui.detalletable->setFocus();
    if  (row>0 && ui.detalletable->item(row-1,0)==0) 
        ui.detalletable->setCurrentCell(row-1,0);
      else
          if  (row>0 && ui.detalletable->item(row-1,0)->text().length()==0) 
        	ui.detalletable->setCurrentCell(row-1,0);
    if (col==2 || col==5)
       {
          // habilitamos editor de fórmulas
         ui.formulatextEdit->setEnabled(true);
         // pasamos contenido al editor
         if (ui.detalletable->item(row,col)!=0)
             ui.formulatextEdit->setText(ui.detalletable->item(row,col)->text());
           else ui.formulatextEdit->setText("");
       }
       else
             {
               ui.formulatextEdit->setText("");
               ui.formulatextEdit->setEnabled(false);
             }
   connect(ui.formulatextEdit,SIGNAL(textChanged()),this,SLOT(formulacambiada()));

   connect(ui.detalletable,SIGNAL(cellChanged ( int, int )),this,SLOT(
             detallecontenidocambiado( int, int)));
}


void edasientomodelo::borralineadetalle()
{
    int fila;
    fila=ui.detalletable->currentRow();
    ui.detalletable->removeRow(fila);
    if (fila<ui.detalletable->rowCount()) ui.detalletable->setCurrentCell(fila,0);
    activacambiado();
}


void edasientomodelo::formulacambiada()
{
   if (ui.formulatextEdit->toPlainText().length()==0 && 
       ui.detalletable->item(ui.detalletable->currentRow(),ui.detalletable->currentColumn())==0) return;

   if (ui.detalletable->item(ui.detalletable->currentRow(),ui.detalletable->currentColumn())!=0)
       {
        if (ui.detalletable->item(ui.detalletable->currentRow(),
               ui.detalletable->currentColumn())->text()==ui.formulatextEdit->toPlainText())
            return;
       }

   ui.detalletable->disconnect(SIGNAL(cellChanged(int,int)));

    if (ui.detalletable->currentColumn()==2 || ui.detalletable->currentColumn()==5)
      {
       if (ui.detalletable->item(ui.detalletable->currentRow(),ui.detalletable->currentColumn())==0)
           {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              ui.detalletable->setItem(ui.detalletable->currentRow(),ui.detalletable->currentColumn(),newItem);
           }
       QString cadena;
       if (comadecimal) cadena=convacoma(ui.formulatextEdit->toPlainText());
           else cadena=convapunto(ui.formulatextEdit->toPlainText());
       ui.detalletable->item(ui.detalletable->currentRow(),ui.detalletable->currentColumn())->setText(cadena);
      }
    activacambiado();

   connect(ui.detalletable,SIGNAL(cellChanged ( int, int )),this,SLOT(
             detallecontenidocambiado( int, int)));

}

void edasientomodelo::detallecontenidocambiado( int row, int col )
{

    if (col==2 || col==5)
       {
         if (comadecimal) ui.detalletable->item(row,col)->setText(convacoma(
             ui.detalletable->item(row,col)->text()));
            else ui.detalletable->item(row,col)->setText(convapunto(ui.detalletable->item(row,col)->text()));
         // pasamos contenido al editor 
         ui.formulatextEdit->setText(ui.detalletable->item(row,col)->text());
       }
    if (col==0 || col==6 || col==7 || col==25)
       {
          // expandimos punto si se trata de subcuenta
	if (ui.detalletable->item(row,col)->text().left(1)!="{")
	      ui.detalletable->item(row,col)->setText(
                expandepunto(ui.detalletable->item(row,col)->text(),anchocuentas()));
       }

    if (col==11 || col==15)
       {
         if (comadecimal) ui.detalletable->item(row,col)->setText(convacoma(
             ui.detalletable->item(row,col)->text()));
            else ui.detalletable->item(row,col)->setText(convapunto(ui.detalletable->item(row,col)->text()));
       }
    activacambiado();
}


bool edasientomodelo::chequeadatos()
{
    if (ui.modelolineEdit->text().length()==0) 
    {
       QMessageBox::warning( this, tr("Edición de asiento modelo"),
                        tr("Ha de asignar un nombre al asiento" ));
       return false;
    }
    if (ui.detalletable->item(0,0)==0)
    {
       QMessageBox::warning( this, tr("Edición de asiento modelo"),
			tr("La tabla de detalle de asiento está incompleta" ));
       return false;
    }
    else
          {
           if (ui.detalletable->item(0,0)->text().length()==0)
               {
                QMessageBox::warning( this, tr("Edición de asiento modelo"),
			tr("La tabla de detalle de asiento está incompleta" ));
                return false;
               }
          }

    QComboBox *comboitem;
    int veces=0;
    while (veces<ui.variablestable->rowCount())
       {
          if (ui.variablestable->item(veces,0)==0) break;
          if (ui.variablestable->item(veces,0)->text().length()==0) break;

          comboitem=(QComboBox*)ui.variablestable->cellWidget(veces,1);
          if (comboitem->currentText()==tipoCALC())
             {
              if (ui.variablestable->item(veces,3)!=NULL)
                if (!chequeaformula(convapunto(ui.variablestable->item(veces,3)->text())))
                 {
                  QString cadnum; cadnum.setNum(veces+1);
                  QMessageBox::warning( this, tr("Edición de asiento automático"),
                                        tr("ATENCIÓN: Contenido de fórmula no correcto en variable: %1").arg(cadnum));
                 }
             }
          veces++;
       }

    int linea=0;
    while (ui.detalletable->item(linea,0)!=0 && linea<ui.detalletable->rowCount())
        {
          if (ui.detalletable->item(linea,0)->text().length()==0)
             { linea++; continue;}
          if (ui.detalletable->item(linea,2)!=0)
               {
                if (!chequeaformula(convapunto(ui.detalletable->item(linea,2)->text())))
                    {
                     QString cadnum; cadnum.setNum(linea+1);
                     QMessageBox::warning( this, tr("Edición de asiento automático"),
                                           tr("ATENCIÓN: Contenido de fórmula no correcto en expresión, línea: %1").arg(cadnum));
                     // return false;
                    }
               }
           if (ui.detalletable->item(linea,5)!=0)
               {
                 if (ui.detalletable->item(linea,5)->text().length()>0 &&
                    !chequeaformula(convapunto(ui.detalletable->item(linea,5)->text())))
                     {
                      QString cadnum; cadnum.setNum(linea+1);
                      QMessageBox::warning( this, tr("Edición de asiento modelo"),
                                            tr("ATENCIÓN: Contenido de fórmula no correcto en base imponible, línea: %1" ).arg(cadnum));
                      // return false;
                     }
               }
           linea++; 
        }
    return true;
}


void edasientomodelo::guardarinfo()
{
    if (!chequeadatos())
    {
        QMessageBox::warning( this, tr("Edición de asiento modelo"),
	      tr("No es posible grabar el asiento modelo.\n" ));
	return;
    }
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();
    // borramos información anterior del asiento modelo
    basedatos::instancia()->deleteVar_as_modeloasientomodelo( ui.modelolineEdit->text() );
    basedatos::instancia()->deleteDet_as_modeloasientomodelo( ui.modelolineEdit->text() );
    basedatos::instancia()->deleteCab_as_modeloasientomodelo( ui.modelolineEdit->text() );

    // grabamos asiento modelo modificado
    /* "aib                   bool,"
    "autofactura_ue        bool,"
    "autofactura_no_ue     bool,"
    "eib                   bool,"
    "eib_servicios         bool," */

    bool aib, autofactura_ue, autofactura_no_ue, eib, eib_servicios;
    aib=false;
    autofactura_ue=false;
    autofactura_no_ue=false;
    eib=false;
    eib_servicios=false;
    bool isp_op_interiores=false;
    bool importacion=false;
    bool exportacion=false;

    if (ui.adqgroupBox->isChecked())
      {
       if (ui.AIBradioButton->isChecked()) aib=true;
       if (ui.autofactura_radioButton->isChecked()) autofactura_ue=true;
       if (ui.autofactura_no_ue_radioButton->isChecked()) autofactura_no_ue=true;
       if (ui.isp_inter_radioButton->isChecked()) isp_op_interiores=true;
       if (ui.importradioButton->isChecked()) importacion=true;
      }

    if (ui.entregasgroupBox->isChecked())
     {
      if (ui.eibradioButton->isChecked()) eib=true;
      if (ui.eib_serv_radioButton->isChecked()) eib_servicios=true;
      if (ui.exportradioButton->isChecked()) exportacion=true;
     }

    basedatos::instancia()->insertCab_as_modelo( ui.modelolineEdit->text().trimmed(),
                                                 ui.fechalineEdit->text(),
                                                 ui.tipocomboBox->currentText(), aib,
                                                 autofactura_ue, autofactura_no_ue, eib,
                                                 eib_servicios,
                                                 isp_op_interiores,
                                                 importacion,
                                                 exportacion,
                                                 ui.diariolineEdit->text());

    int veces=0;
    QComboBox *comboitem;
    QComboBox *comboitem2;
    while (veces<ui.variablestable->rowCount())
       {
          if (ui.variablestable->item(veces,0)==0) break;
          if (ui.variablestable->item(veces,0)->text().length()==0) break;

          comboitem=(QComboBox*)ui.variablestable->cellWidget(veces,1);

          QString desc;
          if (ui.variablestable->item(veces,2)!=0) { desc = ui.variablestable->item(veces,2)->text(); }

          QString valor;
          if (ui.variablestable->item(veces,3)!=0) { valor = ui.variablestable->item(veces,3)->text(); }

          QCheckBox *checkitem;
          checkitem=(QCheckBox*)ui.variablestable->cellWidget(veces,4);
          bool ini=checkitem->isChecked();

          basedatos::instancia()->insertVar_as_modelo( ui.modelolineEdit->text().trimmed(),
                                                       ui.variablestable->item(veces,0)->text() ,
                                                       comboitem->currentText() , desc , valor, ini,veces );
          veces++;
       }

    veces=0;
    while (veces<ui.detalletable->rowCount())
       {
          if (ui.detalletable->item(veces,0)==0) break;
          // if (ui.detalletable->item(veces,0)->text().length()==0) break;

          QString cuenta, concepto, expresion, ci, baseiva, cuentabaseiva, cuentafra, documento, fecha_fra;
          QString prorrata, fechaop, claveop, bicoste;
          QString rectificada, numfacts, facini, facfinal,afecto;
          bool rectificativa, binversion;

          if (ui.detalletable->item(veces,0)!=0) { cuenta = ui.detalletable->item(veces,0)->text().trimmed(); }
          if (ui.detalletable->item(veces,1)!=0) { concepto = ui.detalletable->item(veces,1)->text(); }
          if (ui.detalletable->item(veces,2)!=0) { expresion = ui.detalletable->item(veces,2)->text(); }
          if (ui.detalletable->item(veces,4)!=0) { ci = ui.detalletable->item(veces,4)->text(); }
          if (ui.detalletable->item(veces,5)!=0) { baseiva = ui.detalletable->item(veces,5)->text(); }
          if (ui.detalletable->item(veces,6)!=0) { cuentabaseiva = ui.detalletable->item(veces,6)->text(); }
          if (ui.detalletable->item(veces,7)!=0) { cuentafra = ui.detalletable->item(veces,7)->text(); }
          if (ui.detalletable->item(veces,9)!=0) { documento = ui.detalletable->item(veces,9)->text(); }
          if (ui.detalletable->item(veces,10)!=0) { fecha_fra = ui.detalletable->item(veces,10)->text(); }

          comboitem=(QComboBox*)ui.detalletable->cellWidget(veces,3);  // debe/haber
          comboitem2=(QComboBox*)ui.detalletable->cellWidget(veces,8);

          if (ui.detalletable->item(veces,11)!=0) { prorrata = ui.detalletable->item(veces,11)->text(); }

          QCheckBox *checkitem;
          checkitem=(QCheckBox*)ui.detalletable->cellWidget(veces,12);
          rectificativa=checkitem->isChecked();

          /* QCheckBox *checkitem2;
          checkitem2=(QCheckBox*)ui.detalletable->cellWidget(veces,13);
          autofactura=checkitem2->isChecked(); */

          if (ui.detalletable->item(veces,13)!=0) { fechaop = ui.detalletable->item(veces,13)->text(); }
          if (ui.detalletable->item(veces,14)!=0) { claveop = ui.detalletable->item(veces,14)->text(); }
          if (ui.detalletable->item(veces,15)!=0) { bicoste = ui.detalletable->item(veces,15)->text(); }

          if (ui.detalletable->item(veces,16)!=0) { rectificada = ui.detalletable->item(veces,16)->text(); }
          if (ui.detalletable->item(veces,17)!=0) { numfacts = ui.detalletable->item(veces,17)->text(); }
          if (ui.detalletable->item(veces,18)!=0) { facini = ui.detalletable->item(veces,18)->text(); }
          if (ui.detalletable->item(veces,19)!=0) { facfinal = ui.detalletable->item(veces,19)->text(); }

          QCheckBox *checkitem3;
          checkitem3=(QCheckBox*)ui.detalletable->cellWidget(veces,20);
          binversion=checkitem3->isChecked();

          if (ui.detalletable->item(veces,21)!=0)
           { afecto = ui.detalletable->item(veces,21)->text(); }

          QCheckBox *checkitem4;
          checkitem4=(QCheckBox*)ui.detalletable->cellWidget(veces,22);
          bool agrario=checkitem4->isChecked();

          QString nombre,cif;
          if (ui.detalletable->item(veces,23)!=0)
           { nombre = ui.detalletable->item(veces,23)->text(); }

          if (ui.detalletable->item(veces,24)!=0)
           { cif = ui.detalletable->item(veces,24)->text(); }

          QString cta_retenido;
          bool arrendamiento;
          QString clave;
          QString base_ret;
          QString tipo_ret;
          QString retencion;
          QString ing_a_cta;
          QString ing_a_cta_rep;
          QString nombre_ret;
          QString cif_ret;
          QString provincia;

          if (ui.detalletable->item(veces,25)!=0)
           { cta_retenido = ui.detalletable->item(veces,25)->text(); }

          QCheckBox *checkitem5;
          checkitem5=(QCheckBox*)ui.detalletable->cellWidget(veces,26);
          arrendamiento=checkitem5->isChecked();

          QComboBox *comboitem22;
          comboitem22=(QComboBox*)ui.detalletable->cellWidget(veces,27);
          clave = comboitem22->currentText().left(1);

          if (ui.detalletable->item(veces,28)!=0)
           { base_ret = ui.detalletable->item(veces,28)->text(); }
          if (ui.detalletable->item(veces,29)!=0)
           { tipo_ret = ui.detalletable->item(veces,29)->text(); }
          if (ui.detalletable->item(veces,30)!=0)
           { retencion = ui.detalletable->item(veces,30)->text(); }
          if (ui.detalletable->item(veces,31)!=0)
           { ing_a_cta = ui.detalletable->item(veces,31)->text(); }
          if (ui.detalletable->item(veces,32)!=0)
           { ing_a_cta_rep = ui.detalletable->item(veces,32)->text(); }
          if (ui.detalletable->item(veces,33)!=0)
           { nombre_ret = ui.detalletable->item(veces,33)->text(); }
          if (ui.detalletable->item(veces,34)!=0)
           { cif_ret = ui.detalletable->item(veces,34)->text(); }
          if (ui.detalletable->item(veces,35)!=0)
           { provincia = ui.detalletable->item(veces,35)->text(); }


          basedatos::instancia()->insertDet_as_modelo (ui.modelolineEdit->text().trimmed() , cuenta ,
                concepto , expresion , comboitem->currentText() , ci , baseiva , cuentabaseiva , cuentafra ,
                comboitem2->currentText() , documento, fecha_fra, prorrata, rectificativa,
                fechaop,claveop, bicoste, rectificada, numfacts, facini, facfinal,
                binversion, afecto, agrario, nombre,cif, cta_retenido, arrendamiento,
                clave, base_ret, tipo_ret, retencion, ing_a_cta, ing_a_cta_rep, nombre_ret,
                cif_ret, provincia,
                veces);

          veces++;
        }

    //----

    if (contabilidad.commit())
        {
         cambios=false;
         setWindowTitle(tr("Edición de asiento automático"));
         ui.guardarpushButton->setEnabled(false);
        }
        else
          QMessageBox::warning( this, tr("Asientos automáticos"),
  			   tr("ERROR: no se ha podido grabar el asiento modelo\n" ));

}


void edasientomodelo::botonsalir()
{
  if (cambios) {
      switch( QMessageBox::information( this, tr("Edición de asientos automáticos"),
        tr("Se han realizado cambios no guardados\n"
        "¿ desea grabar los cambios antes de salir ?"),
        tr("&Grabar"), tr("&Descartar"), tr("Cancelar"),
        0,      // Enter == button 0
        2 ) ) { // Escape == button 2
    case 0: // Grabar o intro
        // save
        if (chequeadatos()) {guardarinfo(); accept();}
        break;
    case 1: // Descartar o Alt+D
        reject();
        break;
    case 2: // Cancelar o Escape
        // no salir
        break;
    }
  } else accept(); // no cambiado
}


void edasientomodelo::cargadatos( QString asientomodelo )
{
   ui.modelolineEdit->setText(asientomodelo);

   QSqlQuery query = basedatos::instancia()->selectFechacab_as_modeloasientomodelo(asientomodelo);
   if ( (query.isActive()) && (query.next()) )
   {
      ui.fechalineEdit->setText(query.value(0).toString());
      /* "aib                   bool,"
      "autofactura_ue        bool,"
      "autofactura_no_ue     bool,"
      "eib                   bool,"
      "eib_servicios         bool," */
      bool aib=query.value(2).toBool();
      bool autofactura_ue=query.value(3).toBool();
      bool autofactura_no_ue=query.value(4).toBool();
      bool eib=query.value(5).toBool();
      bool eib_servicios=query.value(6).toBool();
      QString diario=query.value(7).toString();
      bool isp_op_interiores=query.value(8).toBool();
      bool importacion=query.value(9).toBool();
      bool exportacion=query.value(10).toBool();
      ui.diariolineEdit->setText(diario);
      if (aib || autofactura_ue || autofactura_no_ue || isp_op_interiores
          || importacion)
        {
         ui.adqgroupBox->setChecked(true);
         if (aib) ui.AIBradioButton->setChecked(true);
         if (autofactura_ue) ui.autofactura_radioButton->setChecked(true);
         if (autofactura_no_ue) ui.autofactura_no_ue_radioButton->setChecked(true);
         if (isp_op_interiores) ui.isp_inter_radioButton->setChecked(true);
         if (importacion) ui.importradioButton->setChecked(true);
        }
      if (eib || eib_servicios || exportacion)
        {
         ui.entregasgroupBox->setChecked(true);
         if (eib) ui.entregasgroupBox->setChecked(true);
         if (eib_servicios) ui.eib_serv_radioButton->setChecked(true);
         if (exportacion) ui.exportradioButton->setChecked(true);
        }
      for (int veces=0;veces<ui.tipocomboBox->count();veces++)
           {
            if (ui.tipocomboBox->itemText(veces)== query.value(1).toString())
                { ui.tipocomboBox->setCurrentIndex(veces); break; }
           }

   } else return;

   query = basedatos::instancia()->selectTablaVariable(asientomodelo);
   int fila=0;
   QComboBox *comboitem;
   if ( query.isActive() ) {
          while  ( query.next() ) 
                      {
                         QTableWidgetItem *newItem = new QTableWidgetItem(query.value(0).toString());
                         ui.variablestable->setItem(fila,0,newItem);
                         comboitem=(QComboBox*)ui.variablestable->cellWidget(fila,1);
                         for (int veces=0;veces<comboitem->count();veces++)
                              {
                               if (comboitem->itemText(veces)== query.value(1).toString())
                                   { comboitem->setCurrentIndex(veces); break; }
                              }
                         QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(2).toString());
                         ui.variablestable->setItem(fila,2,newItem2);
                         QTableWidgetItem *newItem3 = new QTableWidgetItem(query.value(3).toString());
                         ui.variablestable->setItem(fila,3,newItem3);

                         QCheckBox *checkitem;
                         checkitem=(QCheckBox*)ui.variablestable->cellWidget(fila,4);
                         if (query.value(4).toBool()) checkitem->setChecked(true);
                            else checkitem->setChecked(false);

                         fila++;
                      }
	  }


   query = basedatos::instancia()->selectDet_as_modeloasientomodeloorderorden(asientomodelo);
    fila=0;
   if ( query.isActive() ) {
          while  ( query.next() ) 
                      {
                       QTableWidgetItem *newItem = new QTableWidgetItem(query.value(0).toString());
                       ui.detalletable->setItem(fila,0,newItem);
                       QTableWidgetItem *newItem1 = new QTableWidgetItem(query.value(1).toString());
                       ui.detalletable->setItem(fila,1,newItem1);
                       QTableWidgetItem *newItem2 = new QTableWidgetItem(query.value(2).toString());
                       ui.detalletable->setItem(fila,2,newItem2);
                       if (comadecimal)
	                   ui.detalletable->item(fila,2)->setText(convacoma(query.value(2).toString()));
                         else ui.detalletable->item(fila,2)->setText(query.value(2).toString());
	               comboitem=(QComboBox*)ui.detalletable->cellWidget(fila,3);
                       for (int veces=0;veces<comboitem->count();veces++)
                              {
                               if (comboitem->itemText(veces)== query.value(3).toString())
                                   { comboitem->setCurrentIndex(veces); break; }
                              }

                       QTableWidgetItem *newItem4 = new QTableWidgetItem(query.value(4).toString());
                       ui.detalletable->setItem(fila,4,newItem4);

                       QTableWidgetItem *newItem5 = new QTableWidgetItem(query.value(5).toString());
                       ui.detalletable->setItem(fila,5,newItem5);
                       if (comadecimal)
	                   ui.detalletable->item(fila,5)->setText(convacoma(query.value(5).toString()));
                         else ui.detalletable->item(fila,5)->setText(query.value(5).toString());

                       QTableWidgetItem *newItem6 = new QTableWidgetItem(query.value(6).toString());
                       ui.detalletable->setItem(fila,6,newItem6);
                       QTableWidgetItem *newItem7 = new QTableWidgetItem(query.value(7).toString());
                       ui.detalletable->setItem(fila,7,newItem7);

	               comboitem=(QComboBox*)ui.detalletable->cellWidget(fila,8);
                       for (int veces=0;veces<comboitem->count();veces++)
                              {
                               if (comboitem->itemText(veces)== query.value(8).toString())
                                   { comboitem->setCurrentIndex(veces); break; }
                              }

                       QTableWidgetItem *newItem9 = new QTableWidgetItem(query.value(9).toString());
                       ui.detalletable->setItem(fila,9,newItem9);

                       QTableWidgetItem *newItem10 = new QTableWidgetItem(query.value(10).toString());
                       ui.detalletable->setItem(fila,10,newItem10);

                       QTableWidgetItem *newItem11 = new QTableWidgetItem(query.value(11).toString());
                       ui.detalletable->setItem(fila,11,newItem11); // prorrata

                       QCheckBox *checkitem;
                       checkitem=(QCheckBox*)ui.detalletable->cellWidget(fila,12);
                         if (query.value(12).toBool()) checkitem->setChecked(true);
                            else checkitem->setChecked(false);

                       /*QCheckBox *checkitem2;
                       checkitem2=(QCheckBox*)ui.detalletable->cellWidget(fila,13);
                         if (query.value(13).toBool()) checkitem2->setChecked(true);
                            else checkitem2->setChecked(false); */

                       QTableWidgetItem *newItem14 = new QTableWidgetItem(query.value(13).toString());
                       ui.detalletable->setItem(fila,13,newItem14); // fechaop

                       QTableWidgetItem *newItem15 = new QTableWidgetItem(query.value(14).toString());
                       ui.detalletable->setItem(fila,14,newItem15); // claveop

                       QTableWidgetItem *newItem16 = new QTableWidgetItem(query.value(15).toString());
                       ui.detalletable->setItem(fila,15,newItem16); // bicoste

                       // "rectificada, numfacts, facini, facfinal ";
                       QTableWidgetItem *newItem17 = new QTableWidgetItem(query.value(16).toString());
                       ui.detalletable->setItem(fila,16,newItem17); // rectificada

                       QTableWidgetItem *newItem18 = new QTableWidgetItem(query.value(17).toString());
                       ui.detalletable->setItem(fila,17,newItem18); // numfacts

                       QTableWidgetItem *newItem19 = new QTableWidgetItem(query.value(18).toString());
                       ui.detalletable->setItem(fila,18,newItem19); // facini

                       QTableWidgetItem *newItem20 = new QTableWidgetItem(query.value(19).toString());
                       ui.detalletable->setItem(fila,19,newItem20); // facfinal

                       QCheckBox *checkitem3;
                       checkitem3=(QCheckBox*)ui.detalletable->cellWidget(fila,20);
                         if (query.value(20).toBool()) checkitem3->setChecked(true);
                            else checkitem3->setChecked(false);

                       QTableWidgetItem *newItem22 = new QTableWidgetItem(query.value(21).toString());
                       ui.detalletable->setItem(fila,21,newItem22); // afecto

                       QCheckBox *checkitem2;
                       checkitem2=(QCheckBox*)ui.detalletable->cellWidget(fila,22);
                         if (query.value(22).toBool()) checkitem2->setChecked(true);
                            else checkitem2->setChecked(false);

                       QTableWidgetItem *newItem23 = new QTableWidgetItem(query.value(23).toString());
                       ui.detalletable->setItem(fila,23,newItem23); // nombre

                       QTableWidgetItem *newItem24 = new QTableWidgetItem(query.value(24).toString());
                       ui.detalletable->setItem(fila,24,newItem24); // cif

                       // cta_retenido, arrendamiento, clave, base_ret, "
                       // "tipo_ret, retencion, ing_a_cta, ing_a_cta_rep, nombre_ret, "
                       // "cif_ret, provincia
                       ui.detalletable->setItem(fila,25,new QTableWidgetItem(query.value(25).toString()));

                       QCheckBox *checkitem22;
                       checkitem22=(QCheckBox*)ui.detalletable->cellWidget(fila,26);
                       if (query.value(26).toBool()) checkitem22->setChecked(true);
                          else checkitem22->setChecked(false);

                       comboitem=(QComboBox*)ui.detalletable->cellWidget(fila,27);
                       for (int veces=0;veces<comboitem->count();veces++)
                            {
                              if (comboitem->itemText(veces).left(1)== query.value(27).toString().left(1))
                                 { comboitem->setCurrentIndex(veces); break; }
                            }

                       ui.detalletable->setItem(fila,28,new QTableWidgetItem(query.value(28).toString()));
                       ui.detalletable->setItem(fila,29,new QTableWidgetItem(query.value(29).toString()));
                       ui.detalletable->setItem(fila,30,new QTableWidgetItem(query.value(30).toString()));
                       ui.detalletable->setItem(fila,31,new QTableWidgetItem(query.value(31).toString()));
                       ui.detalletable->setItem(fila,32,new QTableWidgetItem(query.value(32).toString()));
                       ui.detalletable->setItem(fila,33,new QTableWidgetItem(query.value(33).toString()));
                       ui.detalletable->setItem(fila,34,new QTableWidgetItem(query.value(34).toString()));
                       ui.detalletable->setItem(fila,35,new QTableWidgetItem(query.value(35).toString()));

                       fila++;
	              }
      }
  cambios=false;
  setWindowTitle(tr("Edición de asiento automático"));
  ui.guardarpushButton->setEnabled(false);
}


void edasientomodelo::muevearriba()
{
  if (ui.variablestable->currentRow()==0) return;

  // guardamos contenidos de la fila superior
  QString col0,col2,col3;
  int poscombo;
  if (ui.variablestable->item(ui.variablestable->currentRow(),0)==0) return;
  col0=ui.variablestable->item(ui.variablestable->currentRow()-1,0)->text();
  QComboBox *comboitem;
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()-1,1);
  poscombo=comboitem->currentIndex();
  if ( ui.variablestable->item(ui.variablestable->currentRow()-1,2)!=0 )
      col2=ui.variablestable->item(ui.variablestable->currentRow()-1,2)->text();
  if ( ui.variablestable->item(ui.variablestable->currentRow()-1,3)!=0 )
      col3=ui.variablestable->item(ui.variablestable->currentRow()-1,3)->text();
  QCheckBox *checkitem;
  checkitem=(QCheckBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()-1,4);
  bool guardacheck=checkitem->isChecked();

  // asignamos valores a la línea previa
  ui.variablestable->item(ui.variablestable->currentRow()-1,0)->setText(
		ui.variablestable->item(ui.variablestable->currentRow(),0)->text());
  int poscombo2;
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow(),1);
  poscombo2=comboitem->currentIndex();
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()-1,1);
  comboitem->setCurrentIndex(poscombo2);
  if (ui.variablestable->item(ui.variablestable->currentRow()-1,2)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow()-1,2,newItem);
      }
  QString pasocol2;
  if (ui.variablestable->item(ui.variablestable->currentRow(),2)!=0)
     pasocol2=ui.variablestable->item(ui.variablestable->currentRow(),2)->text();
  ui.variablestable->item(ui.variablestable->currentRow()-1,2)->setText(pasocol2);
  if (ui.variablestable->item(ui.variablestable->currentRow()-1,3)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow()-1,3,newItem);
      }
  QString pasocol3;
  if (ui.variablestable->item(ui.variablestable->currentRow(),3)!=0)
      pasocol3=ui.variablestable->item(ui.variablestable->currentRow(),3)->text();
  ui.variablestable->item(ui.variablestable->currentRow()-1,3)->setText(pasocol3);
  QCheckBox *checkitem2;
  checkitem2=(QCheckBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()-1,4);
  QCheckBox *checkitempost;
  checkitempost=(QCheckBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow(),4);
  bool checkpost=checkitempost->isChecked();
  checkitem2->setChecked(checkpost);

  
  // asignamos valores a la línea siguiente
  ui.variablestable->item(ui.variablestable->currentRow(),0)->setText(col0);
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow(),1);
  comboitem->setCurrentIndex(poscombo);

  if (ui.variablestable->item(ui.variablestable->currentRow(),2)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow(),2,newItem);
      }
  ui.variablestable->item(ui.variablestable->currentRow(),2)->setText(col2);

  if (ui.variablestable->item(ui.variablestable->currentRow(),3)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow(),3,newItem);
      }
  ui.variablestable->item(ui.variablestable->currentRow(),3)->setText(col3);

  checkitempost->setChecked(guardacheck);

  int fila=ui.variablestable->currentRow()-1;
  ui.variablestable->setCurrentCell(fila,0);
  activacambiado();
}


void edasientomodelo::mueveabajo()
{
    if (ui.variablestable->currentRow()>=ui.variablestable->rowCount()-1) return;
    if (ui.variablestable->item(ui.variablestable->currentRow()+1,0)==0) return;
        else if (ui.variablestable->item(ui.variablestable->currentRow()+1,0)->text().length()==0) return;
    if (ui.variablestable->item(ui.variablestable->currentRow(),0)==0) return;

  // guardamos contenidos de la fila inferior
  QString col0,col2,col3;
  int poscombo;
  col0=ui.variablestable->item(ui.variablestable->currentRow()+1,0)->text();
  QComboBox *comboitem;
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()+1,1);
  poscombo=comboitem->currentIndex();

  if (ui.variablestable->item(ui.variablestable->currentRow()+1,2)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow()+1,2,newItem);
      }
  col2=ui.variablestable->item(ui.variablestable->currentRow()+1,2)->text();

  if (ui.variablestable->item(ui.variablestable->currentRow()+1,3)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow()+1,3,newItem);
      }
  col3=ui.variablestable->item(ui.variablestable->currentRow()+1,3)->text();

  QCheckBox *checkitem;
  checkitem=(QCheckBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()+1,4);
  bool guardacheck=checkitem->isChecked();

  // asignamos valores a la fila inferior
  ui.variablestable->item(ui.variablestable->currentRow()+1,0)->setText(
		ui.variablestable->item(ui.variablestable->currentRow(),0)->text());
  int poscombo2;
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow(),1);
  poscombo2=comboitem->currentIndex();
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()+1,1);
  comboitem->setCurrentIndex(poscombo2);

  if (ui.variablestable->item(ui.variablestable->currentRow(),2)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow(),2,newItem);
      }
  ui.variablestable->item(ui.variablestable->currentRow()+1,2)->setText(
		ui.variablestable->item(ui.variablestable->currentRow(),2)->text());
  
  if (ui.variablestable->item(ui.variablestable->currentRow(),3)==0)
      {
        QTableWidgetItem *newItem = new QTableWidgetItem("");
        ui.variablestable->setItem(ui.variablestable->currentRow(),3,newItem);
      }
  ui.variablestable->item(ui.variablestable->currentRow()+1,3)->setText(
		ui.variablestable->item(ui.variablestable->currentRow(),3)->text());
  QCheckBox *checkitem2;
  checkitem2=(QCheckBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow(),4);
  QCheckBox *checkitempost;
  checkitempost=(QCheckBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow()+1,4);
  bool checkant=checkitem2->isChecked();
  checkitempost->setChecked(checkant);


  // asignamos valores (guardados) a la fila en curso
  ui.variablestable->item(ui.variablestable->currentRow(),0)->setText(col0);
  comboitem=(QComboBox*)ui.variablestable->cellWidget(ui.variablestable->currentRow(),1);
  comboitem->setCurrentIndex(poscombo);
  ui.variablestable->item(ui.variablestable->currentRow(),2)->setText(col2);
  ui.variablestable->item(ui.variablestable->currentRow(),3)->setText(col3);
  checkitem2->setChecked(guardacheck);

  int fila=ui.variablestable->currentRow()+1;
  ui.variablestable->setCurrentCell(fila,0);
  activacambiado();

}

void edasientomodelo::tipo_combo_cambiado()
{
  QString caddescrip;
  basedatos::instancia()->existecodigotipo_automatico(ui.tipocomboBox->currentText(),&caddescrip);
  ui.tipolineEdit->setText(caddescrip);
  activacambiado();
}


void edasientomodelo::entregasboxpulsado()
{
  if (ui.entregasgroupBox->isChecked()) ui.adqgroupBox->setChecked(false);
  activacambiado();
}


void edasientomodelo::adqboxpulsado()
{
  if (ui.adqgroupBox->isChecked()) ui.entregasgroupBox->setChecked(false);
  activacambiado();
}
