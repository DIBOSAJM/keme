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

#include "relop.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include <QRadioButton>
#include "datos_accesorios.h"
#include "directorio.h"
#include <QMessageBox>
#include <QProgressDialog>
#include <QFileDialog>

relop::relop() : QDialog() {
    ui.setupUi(this);

 QStringList columnas;
 columnas << tr("CUENTA") << tr("IMPORTE") << tr("T1") << tr("T2") << tr("T3") << tr("T4");
 columnas << tr("NOMBRE COM.");
 columnas << tr("RAZON") << tr("CIF") << tr("POBLACION");
 columnas << tr("C.P.") << tr("PROVINCIA");

 ui.tabla->setHorizontalHeaderLabels(columnas);

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

  comadecimal=haycomadecimal(); decimales=haydecimales();

  connect(ui.cargapushButton,SIGNAL( clicked()),this,SLOT(cargadatos()));

  connect(ui.radioButton1,SIGNAL(toggled(bool)),this,SLOT(radioadquisicionescambiado()));
  connect(ui.radioButton2,SIGNAL(toggled(bool)),this,SLOT(radioventascambiado()));
  connect(ui.importelineEdit,SIGNAL(textChanged(QString)),this,SLOT(importecambiado()));
  connect(ui.importelineEdit,SIGNAL(editingFinished()),this,SLOT(finedicimporte()));

  connect(ui.imprimepushButton,SIGNAL( clicked()),this,SLOT(imprime()));

  connect(ui.ejerciciocomboBox,SIGNAL( currentIndexChanged (int)),this,SLOT(combocambiado()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

  connect(ui.datospushButton,SIGNAL(clicked()),SLOT(datoscuenta()));

  connect(ui.borrarpushButton,SIGNAL(clicked()),SLOT(borralinea()));
  connect(ui.m182pushButton,SIGNAL(clicked()),SLOT(genera182()));
  connect(ui.copiarpushButton,SIGNAL(clicked()),SLOT(copiar()));

  ui.m182pushButton->hide();
  ui.deduclabel->hide();
  ui.deduclineEdit->hide();
  ui.naturalezacomboBox->hide();
  m182=false;
  setWindowTitle(tr("Relación de operaciones según diario para modelo 182"));
}

void relop::prepara182()
{
  ui.m182pushButton->show();
  ui.radioButton1->setChecked(false);
  ui.radioButton2->setChecked(true);
  ui.radioButton1->hide();
  ui.radioButton2->setEnabled(false);
  ui.radioButton2->setText(tr("Aportantes"));
  ui.deduclabel->show();
  ui.deduclineEdit->show();
  ui.naturalezacomboBox->show();
  m182=true;
}

void relop::cargadatos()
{
   borratabla();
   if (ui.codigoslineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("Relación de operaciones"),
			     tr("ERROR: se debe de suministrar una relación de códigos para la consulta."));
       return;
    }

   QProgressDialog progreso(tr("Cargando información ... ")
                            , 0, 0, 0);
   progreso.setWindowModality(Qt::WindowModal);
   progreso.setMinimumDuration ( 0 );
   progreso.show();
   progreso.update();
   QApplication::processEvents();

   QString cadimporte=ui.importelineEdit->text();
   cadimporte=convapunto(cadimporte);
   if (!cadnumvalido(cadimporte) || cadimporte.length()==0) cadimporte="0";

   QSqlQuery consulta = basedatos::instancia()->selectsumdiarionoaperturanocierrefechas(
           ui.radioButton1->isChecked() ,
           ui.ejerciciocomboBox->currentText() ,
           ui.codigoslineEdit->text() );


    int fila=0;
  if (consulta.isActive())
  while (consulta.next())
      {
         if (consulta.value(1).toDouble()-0.0001>=cadimporte.toDouble())
            {
              ui.tabla->insertRow(fila);
              QString codigo=consulta.value(0).toString();
              QTableWidgetItem *newItem = new QTableWidgetItem(consulta.value(0).toString());
              ui.tabla->setItem(fila,0,newItem);

              QTableWidgetItem *newItem1 = new QTableWidgetItem(
                                   formateanumero(consulta.value(1).toDouble(),comadecimal,decimales));
              newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.tabla->setItem(fila,1,newItem1);

              QTableWidgetItem *newItem11 = new QTableWidgetItem(
                                   formateanumero(
                                           basedatos::instancia()->operaciones_diario_1T(
                                                      ui.radioButton1->isChecked() ,
                                                      ui.ejerciciocomboBox->currentText() ,
                                                      codigo)
                                                      ,comadecimal,decimales));
              newItem11->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.tabla->setItem(fila,2,newItem11);

              QTableWidgetItem *newItem12 = new QTableWidgetItem(
                                   formateanumero(basedatos::instancia()->operaciones_diario_2T(
                                           ui.radioButton1->isChecked() ,
                                           ui.ejerciciocomboBox->currentText() ,
                                           codigo) ,comadecimal,decimales));
              newItem12->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.tabla->setItem(fila,3,newItem12);

              QTableWidgetItem *newItem13 = new QTableWidgetItem(
                                   formateanumero(basedatos::instancia()->operaciones_diario_3T(
                                           ui.radioButton1->isChecked() ,
                                           ui.ejerciciocomboBox->currentText() ,
                                           codigo),comadecimal,decimales));
              newItem13->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.tabla->setItem(fila,4,newItem13);

              QTableWidgetItem *newItem14 = new QTableWidgetItem(
                                   formateanumero(basedatos::instancia()->operaciones_diario_4T(
                                           ui.radioButton1->isChecked() ,
                                           ui.ejerciciocomboBox->currentText() ,
                                           codigo),comadecimal,decimales));
              newItem14->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.tabla->setItem(fila,5,newItem14);


              QSqlQuery consulta2 = basedatos::instancia()->select6Datossubcuentacuenta(consulta.value(0).toString());
              if ( (consulta2.isActive()) && (consulta2.next()) )
	        {
                 ui.tabla->setItem(fila,6,new QTableWidgetItem(consulta2.value(0).toString()));
                 ui.tabla->setItem(fila,7,new QTableWidgetItem(consulta2.value(1).toString()));
                 ui.tabla->setItem(fila,8,new QTableWidgetItem(consulta2.value(2).toString()));
                 ui.tabla->setItem(fila,9,new QTableWidgetItem(consulta2.value(3).toString()));
                 ui.tabla->setItem(fila,10,new QTableWidgetItem(consulta2.value(4).toString()));
                 ui.tabla->setItem(fila,11,new QTableWidgetItem(consulta2.value(5).toString()));
                }
              fila++;
              QApplication::processEvents();
            }
      }

}


void relop::borratabla()
{
  ui.tabla->setRowCount(0);
}


void relop::radioadquisicionescambiado()
{
  borratabla();
}


void relop::radioventascambiado()
{
  borratabla();
}



void relop::importecambiado()
{
  // importelineEdit->setText(convacoma(importelineEdit->text()));
  borratabla();
}


void relop::finedicimporte()
{
  ui.importelineEdit->setText(formateanumero(convapunto(ui.importelineEdit->text()).toDouble(),comadecimal,decimales));
}


void relop::combocambiado()
{
 borratabla();
}


void relop::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("operaciones.tex");
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

    QString mensaje=tr("EJERCICIO: ");
    mensaje+=ui.ejerciciocomboBox->currentText();
    mensaje+=tr("    - -    A PARTIR DE IMPORTE: ");
    mensaje+=ui.importelineEdit->text();
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << mensaje << "}}";
    stream << "\\end{center}";
    
    mensaje=tr("PARA CÓDIGOS: ");
    mensaje+=ui.codigoslineEdit->text();
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << mensaje << "}}";
    stream << "\\end{center}";
	    
    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|r|r|r|r|r|p{3cm}|c|p{3cm}|c|p{3cm}|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{11}{|c|} {\\textbf{";
   QString cadena=tr("VOLUMEN DE OPERACIONES ");
   if (!m182)
     {
      if (ui.radioButton1->isChecked()) cadena+=tr("POR ADQUISICIONES DE BIENES Y SERVICIOS ");
        else cadena+=tr("POR VENTAS DE BIENES Y SERVICIOS");
     }
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // --------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("CUENTA") << "}} & ";
    stream << "{\\tiny{" << tr("IMPORTE") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM1") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM2") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM3") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM4") << "}} & ";
    stream << "{\\tiny{" << tr("RAZÓN") << "}} & ";
    stream << "{\\tiny{" << tr("CIF/NIF") << "}} & ";
    stream << "{\\tiny{" << tr("POBLACIÓN") << "}} & ";
    stream << "{\\tiny{" << tr("CP") << "}} & ";
    stream << "{\\tiny{" << tr("PROVINCIA") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
    stream << "{\\tiny{" << tr("Importe") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM1") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM2") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM3") << "}} & ";
    stream << "{\\tiny{" << tr("TRIM4") << "}} & ";
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
    while (fila<ui.tabla->rowCount())
          {
             
             stream << "{\\tiny "; 
             if (ui.tabla->item(fila,0)!=0) stream << filtracad(ui.tabla->item(fila,0)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,1)!=0) stream << filtracad(ui.tabla->item(fila,1)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,2)!=0) stream << filtracad(ui.tabla->item(fila,2)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,3)!=0) stream << filtracad(ui.tabla->item(fila,3)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,4)!=0) stream << filtracad(ui.tabla->item(fila,4)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,5)!=0) stream << filtracad(ui.tabla->item(fila,5)->text());
             stream << "} & {\\tiny "; 
             if (ui.tabla->item(fila,7)!=0) stream << filtracad(ui.tabla->item(fila,7)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,8)!=0) stream << filtracad(ui.tabla->item(fila,8)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,9)!=0) stream << filtracad(ui.tabla->item(fila,9)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,10)!=0) stream << filtracad(ui.tabla->item(fila,10)->text());
             stream << "} & {\\tiny ";
             if (ui.tabla->item(fila,11)!=0) stream << filtracad(ui.tabla->item(fila,11)->text());
             stream << "} \\\\ \n  " << "\\hline\n";
             if (ui.tabla->item(fila,1)!=0)
                 suma+=convapunto(ui.tabla->item(fila,1)->text()).toDouble();
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


void relop::imprime()
{

   generalatex();
   int valor=imprimelatex2(tr("operaciones"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());


}



void relop::latex()
{
    generalatex();
    int valor=editalatex("operaciones");
    if (valor==1)
        QMessageBox::warning( this, tr("Relación de operaciones"),
                              tr("PROBLEMAS al llamar al editor Latex"));

}


void relop::datoscuenta()
{
    if (ui.tabla->rowCount()==0) return;
    if (ui.tabla->currentRow()<0 || ui.tabla->currentRow()>ui.tabla->rowCount()) return;
    QString cuenta=ui.tabla->item(ui.tabla->currentRow(),0)->text();
    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    datos_accesorios *d = new datos_accesorios();
    d->cargacodigo( cuenta );
    d->exec();
    delete(d);
}


void relop::borralinea( void )
{
 int fila=ui.tabla->currentRow();
 ui.tabla->removeRow(fila);
}


void relop::genera182()
{

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
  QString filtrotexto=tr("Archivos 182 (*.182)");
  filtros << filtrotexto;
  filtros << tr("Archivos 182 (*.182)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO 182"));
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
           if (nombre.right(4)!=".182") nombre=nombre+".182";
           if (!genfich182(nombre))
              QMessageBox::information( this, tr("Fichero 182"),tr("NO se ha generado correctamente el fichero 347"));
          }
       }

}



bool relop::genfich182(QString nombre)
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
       QMessageBox::warning( this, tr("Fichero 182"),tr("Error: Imposible grabar fichero"));
       return false;
      }
   QTextStream docstream( &doc );
   docstream.setEncoding(QStringConverter::Utf8);
   QString contenido="1182";

   QString cadanyo;
   cadanyo.setNum(inicioejercicio(ui.ejerciciocomboBox->currentText()).year());
   contenido+=cadanyo.trimmed();

   QString nif=completacadnum(basedatos::instancia()->cif(),9);
   contenido+=nif;

   QString nombred=completacadcad(adaptacad_aeat(
           basedatos::instancia()->selectEmpresaconfiguracion ()),40);
   contenido+=nombred;

   contenido+="T"; // Presentación telemática

   //QString tfno=completacadnum(adaptacad_aeat(ui.telefonolineEdit->text()),9);
   //contenido+=tfno;
   QString str; str.fill('0', 9); contenido+=str;

   // QString nombrerel=completacadcad(adaptacad_aeat(ui.nombrerellineEdit->text()),40);
   // contenido+=nombrerel;
   str.fill(' ', 40); contenido+=str;

   contenido+="1820000000000";

  /* if (ui.complgroupBox->isChecked())
      {
        if (ui.complementariaradioButton->isChecked())
           contenido+="C ";
         else
             contenido+="S ";
        contenido+=completacadnum(ui.anteriorlineEdit->text(),13);
      }
      else
          {
           contenido+="  ";
           contenido+="0000000000000";
          } */

   contenido+="  "; // declaración complementaria - declaración sustitutiva
   contenido+="0000000000000"; // número identificativo de la declaración anterior


  // Número total de personas y entidades
  QString numregistros;
  numregistros.setNum(ui.tabla->rowCount());
  numregistros=completacadnum(numregistros.trimmed(),9);
  contenido+=numregistros;

  // importe anual de las operaciones
  // hay que sumar la columna 1

  double total_operaciones=0;
  for (int veces=0; veces<ui.tabla->rowCount(); veces++)
     {
      total_operaciones+=convapunto(ui.tabla->item(veces,1)->text()).toDouble();
     }
  QString cad_total_op;
  cad_total_op.setNum(total_operaciones,'f',2);
  cad_total_op=cad_total_op.trimmed();

 /* if (total_operaciones<0)
     contenido+="N";
    else
        contenido+=" ";*/

  QString cad_tot_entera=cad_total_op.section(".",0,0).trimmed().remove('-');
  QString cad_tot_decimal=cad_total_op.section(".",1,1).trimmed();
  cad_tot_entera=completacadnum(cad_tot_entera,13);
  contenido+=cad_tot_entera;
  cad_tot_decimal=completacadnum(cad_tot_decimal,2);
  contenido+=cad_tot_decimal;

  // NATURALEZA DEL DECLARANTE
  QString cadnum; cadnum.setNum(ui.naturalezacomboBox->currentIndex()+1);
  contenido+=cadnum.trimmed();

  str.clear(); str.fill(' ',9); // nif del titular del patrimonio progegido
  contenido+=str;
  str.clear(); str.fill(' ',40); // apellidos y nombre del titular del patrimonio protegido
  contenido+=str;

  // 28 blancos
  str.clear();; str.fill(' ', 28);
  contenido+=str;

  // 13 blancos
  str.clear();; str.fill(' ', 13);
  contenido+=str;
  contenido+="\r\n";
  // -----------------------------------
  // FIN REGISTRO TIPO I
  // -----------------------------------

  // REGISTRO DE DETALLE
  // TIPO DE REGISTRO 2

  for (int veces=0; veces<ui.tabla->rowCount(); veces++)
      {
       contenido+="2182";

       cadanyo.setNum(inicioejercicio(ui.ejerciciocomboBox->currentText()).year());
       contenido+=cadanyo.trimmed();
       nif=completacadnum(basedatos::instancia()->cif(),9); // nif declarante
       contenido+=nif;

       // nif del declarado
       nif=completacadnum(ui.tabla->item(veces,8)->text().trimmed().remove(' '),9);
       contenido+=nif;

       // nif del representate legal (lo suponemos vacío)
       // sólo sirve para s.p. menores de 14 años

       str.clear();
       str.fill(' ',9);
       contenido+=str;

       QString denominacion;
       denominacion=ui.tabla->item(veces,7)->text();
       denominacion.replace(',',' ');
       denominacion.replace("  "," ");
       contenido+=completacadcad(adaptacad_aeat(denominacion),40);

       // CÓDIGO PROVINCIA
       contenido+=completacadnum(ui.tabla->item(veces,10)->text().left(2),2);

       // Clave operación
       contenido+="A";// -----------------------------------------------------

       // NATURALEZA DEL DECLARADO
       // Estudiamos la composición del NIF
       //   si es persona física empezará por número o X, Y, Z, K, L, M
       //   si empieza por letra E = Entidad atribución rentas
       // bool persona_fisica=true;
       bool persona_juridica=false;
       bool entidad_atrib_rentas=false;

       if (!nif.isEmpty())
         if (!QString("0123456789XYZKLM").contains(nif.left(1)))
           {
            //persona_fisica=false;
            if (nif.left(1)=="E")
               entidad_atrib_rentas=true;
             else
                persona_juridica=true;
           }
       // porcentaje deducción

       QString cadporcentaje=convapunto(ui.deduclineEdit->text());
       // if (persona_juridica) cadporcentaje="35";
       QString cadporcentaje_entero, cadporcentaje_decimales;
       if (cadporcentaje.contains('.'))
          {
            cadporcentaje_entero=cadporcentaje.section(".",0,0).trimmed();
            cadporcentaje_decimales=cadporcentaje.section(".",1,1).trimmed();
          }
          else
              {
               cadporcentaje_entero=cadporcentaje;
               cadporcentaje_decimales="0";
              }
       cadporcentaje_entero=completacadnum(cadporcentaje_entero,3);
       cadporcentaje_decimales=completacadnum(cadporcentaje_decimales,2);
       contenido+=cadporcentaje_entero;
       contenido+=cadporcentaje_decimales;
       // -----------------------------------------

       // Importe anual de las operaciones
       QString cad_vol_op=convapunto(ui.tabla->item(veces,1)->text().trimmed().remove('-'));
       /*if (importe<0)
          contenido+="N";
         else
             contenido+=" ";*/

       QString cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
       QString cad_decimal=cad_vol_op.section(".",1,1).trimmed();
       cad_entero=completacadnum(cad_entero,11);
       contenido+=cad_entero;
       cad_decimal=completacadnum(cad_decimal,2);
       contenido+=cad_decimal;

       // DONATIVO NO EN ESPECIE
       contenido+=" ";

      // COMUNIDAD AUTÓNOMOA
       contenido+="00";

      // DEDUCCIÓN COMUNIDAD AUTÓNOMA
       str.clear();
       str.fill('0',5);
       contenido+=str;

       QString cadnaturaleza="F";
       if (persona_juridica) cadnaturaleza="J";
       if (entidad_atrib_rentas) cadnaturaleza="E";
       contenido+=cadnaturaleza;

       // REVOCACION
       contenido+=" ";

       // EJERCICIO DONACIÓN REVOCADA
       str.clear();
       str.fill('0',4);
       contenido+=str;

       // TIPO DE BIEN (SOLO SI ESPECIE)
       contenido+=" ";

       // IDENTIFICACIÓN DEL BIEN (SOLO SI ESPECIE)
       str.clear();
       str.fill(' ',20);
       contenido+=str;

       // RECURRENCIA DONATIVOS
       contenido+="2"; // por defecto no hay recurrencia

       // BLANCOS 118
       str.clear();
       str.fill(' ',118);
       contenido+=str;
       if (veces<ui.tabla->rowCount()-1) contenido+="\r\n";
  // ----------------------------------------------------------------------------------------
   }


  docstream << contenido;
  doc.close();

  QMessageBox::information( this, tr("Fichero 182"),tr("182 generado en archivo: ")+nombre);

  return true;
}

void relop::copiar()
{
    QString cadena=tr("EJERCICIO:")+"\t";
    cadena+=ui.ejerciciocomboBox->currentText();
    cadena+="\t"+tr("A PARTIR IMPORTE:")+"\t";
    cadena+=ui.importelineEdit->text();
    cadena+="\n";

    cadena+=tr("PARA CÓDIGOS: ");
    cadena+="\t";
    cadena+=ui.codigoslineEdit->text();
    cadena+="\n";

    cadena+="\n";

    cadena+=tr("VOLUMEN DE OPERACIONES ");
   if (!m182)
     {
      if (ui.radioButton1->isChecked()) cadena+=tr("POR ADQUISICIONES DE BIENES Y SERVICIOS ");
        else cadena+=tr("POR VENTAS DE BIENES Y SERVICIOS");
     }
   // --------------------------------------------------------------------------------------
   cadena+="\n";
   cadena+="\n";
    // --------------------------------------------------------------------------------------
    cadena+= tr("CUENTA");
    cadena+="\t";
    cadena+= tr("IMPORTE");
    cadena+="\t";
    cadena+= tr("TRIM1");
    cadena+="\t";
    cadena+= tr("TRIM2");
    cadena+="\t";
    cadena+= tr("TRIM3");
    cadena+="\t";
    cadena+= tr("TRIM4");
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
    while (fila<ui.tabla->rowCount())
          {
             if (ui.tabla->item(fila,0)!=0) cadena+=ui.tabla->item(fila,0)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,1)!=0) cadena+=ui.tabla->item(fila,1)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,2)!=0) cadena+=ui.tabla->item(fila,2)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,3)!=0) cadena+=ui.tabla->item(fila,3)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,4)!=0) cadena+=ui.tabla->item(fila,4)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,5)!=0) cadena+=ui.tabla->item(fila,5)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,7)!=0) cadena+=ui.tabla->item(fila,7)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,8)!=0) cadena+=ui.tabla->item(fila,8)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,9)!=0) cadena+=ui.tabla->item(fila,9)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,10)!=0) cadena+=ui.tabla->item(fila,10)->text();
             cadena+="\t";
             if (ui.tabla->item(fila,11)!=0) cadena+=ui.tabla->item(fila,11)->text();
             cadena+="\n";
             if (ui.tabla->item(fila,1)!=0)
                 suma+=convapunto(ui.tabla->item(fila,1)->text()).toDouble();
             fila++;
          }

   // --------------------------------------------------------------------------------------
    cadena+= "\n\t" + tr("SUMA OPERACIONES:  ") +"\t";
    cadena+= formateanumerosep(suma,comadecimal,decimales);
    cadena+= "\n";

    QClipboard *cb = QApplication::clipboard();
    cb->setText(cadena);
    QMessageBox::information( this, tr("Consulta operaciones"),
                              tr("Se ha pasado el contenido al portapapeles") );

}
