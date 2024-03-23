#include "modelo349.h"
#include "ui_modelo349.h"
#include "basedatos.h"
#include "funciones.h"
#include <QProgressDialog>
#include "datos_accesorios.h"
#include "consmayor.h"
#include <QMessageBox>
#include <QFileDialog>
#include "externos.h"

modelo349::modelo349(QString qusuario, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelo349)
{
    ui->setupUi(this);
    usuario=qusuario;
    comadecimal=haycomadecimal(); decimales=haydecimales();
    QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
    QStringList ej;
    if ( query.isActive() ) {
            while ( query.next() )
                  ej << query.value(0).toString();
            }
    ui->ejerciciocomboBox->addItems(ej);
    actufechas();

    QString empresa, nif, nombre, apellidos;
    basedatos::instancia()->empresa_nif_nombre_apellidos(&empresa, &nif, &nombre, &apellidos);

    ui->niflineEdit->setText(nif);
    ui->nombrelineEdit->setText(empresa);
    if (!apellidos.isEmpty()) {
        ui->nombrelineEdit->setText(apellidos + " " + nombre);
    }


    //ui->niflineEdit->setText(basedatos::instancia()->cif());
    //ui->nombrelineEdit->setText(nombreempresa());

    QStringList columnas;
    columnas << tr("CUENTA") << tr("IMPORTE");
    columnas << tr("RAZÓN");
    columnas << tr("CÓD.PAÍS") << tr("NIF") << tr("OPERACIÓN") << tr("EXTERNO");

    ui->tableWidget->setColumnCount(7);

    ui->tableWidget->setHorizontalHeaderLabels(columnas);

     ui->tableWidget->setColumnWidth(2,200);

    connect(ui->periodocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));
    connect(ui->ejerciciocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));

    connect(ui->cargapushButton,SIGNAL(clicked(bool)),SLOT(cargadatos()));
    // connect(ui->borrarpushButton,SIGNAL(clicked(bool)),SLOT(borratabla()));
    connect(ui->datospushButton,SIGNAL(clicked(bool)),SLOT(datoscuenta()));
    connect(ui->mayorpushButton,SIGNAL(clicked(bool)),SLOT(consultamayor()));
    connect(ui->borrarpushButton,SIGNAL(clicked(bool)),SLOT(borralinea()));

    connect(ui->imprimepushButton,SIGNAL(clicked(bool)),SLOT(imprime()));
    connect(ui->latexpushButton,SIGNAL(clicked(bool)),SLOT(edlatex()));

    connect(ui->copiarpushButton,SIGNAL(clicked(bool)),SLOT(copiar()));

    connect(ui->gen349pushButton,SIGNAL(clicked(bool)),SLOT(genera349()));

}

modelo349::~modelo349()
{
    delete ui;
}


void modelo349::actufechas()
{
  QString cadcombo=ui->periodocomboBox->currentText();
  QString periodo=cadcombo.section(' ',0,0);

  QDate fechaini=inicioejercicio(ui->ejerciciocomboBox->currentText());
  QDate fechafin=finejercicio(ui->ejerciciocomboBox->currentText());
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

  ui->inicialdateEdit->setDate(fechaini);
  ui->finaldateEdit->setDate(fechafin);
}


void modelo349::cargadatos()
{
   borratabla();

   QProgressDialog progreso(tr("Cargando información ... ")
                            , 0, 0, 0);
   progreso.setMinimumDuration ( 0 );
   progreso.show();
   progreso.update();
   QApplication::processEvents();


    QSqlQuery consulta = basedatos::instancia()->info349_recibidas(ui->inicialdateEdit->date(),
                                                                   ui->finaldateEdit->date());

   int fila=0;
   while (consulta.next())
      {
         ui->tableWidget->insertRow(fila);
         ui->tableWidget->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         ui->tableWidget->setItem(fila,6,new QTableWidgetItem(consulta.value(4).toString()));
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                              formateanumero(consulta.value(1).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui->tableWidget->setItem(fila,1,newItem1);
         if (consulta.value(2).toBool())
            ui->tableWidget->setItem(fila,5, new QTableWidgetItem("A"));
         if (consulta.value(3).toBool())
            ui->tableWidget->setItem(fila,5, new QTableWidgetItem("I"));

         /*
              QSqlQuery consulta2 = basedatos::instancia()->select7Datossubcuentacuenta( consulta.value(0).toString() );
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
             */
              fila++;
              QApplication::processEvents();
            }


   consulta = basedatos::instancia()->info349_emitidas(ui->inicialdateEdit->date(),
                                                       ui->finaldateEdit->date());

   while (consulta.next())
      {
         ui->tableWidget->insertRow(fila);
         ui->tableWidget->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         ui->tableWidget->setItem(fila,6,new QTableWidgetItem(consulta.value(4).toString()));
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                              formateanumero(consulta.value(1).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui->tableWidget->setItem(fila,1,newItem1);
         if (consulta.value(2).toBool())
            ui->tableWidget->setItem(fila,5, new QTableWidgetItem("E"));
         if (consulta.value(3).toBool())
            ui->tableWidget->setItem(fila,5, new QTableWidgetItem("S"));
            fila++;
          QApplication::processEvents();
       }

   for (int linea=0; linea<ui->tableWidget->rowCount(); linea++)
      {
         QString externo=ui->tableWidget->item(linea,6)->text().trimmed();
         QSqlQuery consulta2;
         if (externo.isEmpty())
            consulta2 = basedatos::instancia()->select7Datossubcuentacuenta(ui->tableWidget->item(linea,0)->text());
           else
             consulta2 = basedatos::instancia()->select7datos_externo(externo);
         if ( (consulta2.isActive()) && (consulta2.next()) )
            {
             // razon,cif,poblacion,codigopostal,provincia,claveidfiscal,pais,domicilio
             QString identif=consulta2.value(0).toString();
             if (!externo.isEmpty())
                if (!consulta2.value(8).toString().isEmpty() &&
                     !consulta2.value(9).toString().isEmpty())
                 identif= consulta2.value(8).toString()+ " " + consulta2.value(9).toString();
             ui->tableWidget->setItem(linea,2,new QTableWidgetItem(identif));
             QString vat_id=consulta2.value(1).toString();
             QString cad_paises="DE AT BE BG CY DK SI EE FI FR EL GB NL HU IT IE LV LT LU MT PL PT CZ SK RO SE HR";
             QString pais=vat_id.left(2).toUpper();
             if (cad_paises.contains(pais))
                {
                  ui->tableWidget->setItem(linea,3,new QTableWidgetItem(pais));
                  vat_id=vat_id.mid(2);
                }
                 else ui->tableWidget->setItem(linea,3,new QTableWidgetItem(""));
             ui->tableWidget->setItem(linea,4,new QTableWidgetItem(vat_id));
            }
      }

   // progreso.reset();
}


void modelo349::borratabla()
{
  ui->tableWidget->setRowCount(0);
}

void modelo349::datoscuenta()
{
    if (ui->tableWidget->rowCount()==0) return;
    if (ui->tableWidget->currentRow()<0 ||
        ui->tableWidget->currentRow()>ui->tableWidget->rowCount()) return;
    QString cuenta=ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    QString externo=ui->tableWidget->item(ui->tableWidget->currentRow(),6)->text();
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
          }
}


void modelo349::consultamayor()
{
    if (ui->tableWidget->rowCount()<=0) return;
    if (ui->tableWidget->currentRow()>=ui->tableWidget->rowCount()) return;
    QString cuenta=ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
    if (cuenta.length()>0)
    {
        QDate fecha;
        consmayor *elmayor=new consmayor(comadecimal,decimales,usuario);
        fecha=ui->inicialdateEdit->date();
        QString externo=ui->tableWidget->item(ui->tableWidget->currentRow(),6)->text();
        elmayor->pasadatos(cuenta,fecha,externo);
        elmayor->exec();
        delete(elmayor);
    }

}


void modelo349::borralinea( void )
{
 int fila=ui->tableWidget->currentRow();
 ui->tableWidget->removeRow(fila);
}


void modelo349::generalatex()
{
    QProgressDialog progreso(tr("Generando informe ... ")
                             , 0, 0, 0);
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.show();
    progreso.update();
    QApplication::processEvents();

   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("modelo_349.tex");
   QFile fichero(adapta(qfichero));
    if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    // stream << "\\begin{landscape}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    stream << "\n";
    QString mensaje=tr("FECHA INICIAL: ");
    mensaje+=ui->inicialdateEdit->date().toString("dd.MM.yyyy");
    mensaje+=tr("  -  FECHA FINAL: ");
    mensaje+=ui->finaldateEdit->date().toString("dd.MM.yyyy");
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << mensaje << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|c|r|p{5cm}|c|c|c|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{6}{|c|} {\\textbf{";
   QString cadena=tr("RELACIÓN DE DECLARADOS PARA 349");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // -----------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("CUENTA") << "}} & ";
    stream << "{\\tiny{" << tr("IMPORTE") << "}} & ";
    stream << "{\\tiny{" << tr("RAZÓN") << "}} & ";
    stream << "{\\tiny{" << tr("PAIS") << "}} & ";
    stream << "{\\tiny{" << tr("NIF-IVA") << "}} & ";
    stream << "{\\tiny{" << tr("TIPO") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // -------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\tiny{" << tr("CUENTA") << "}} & ";
    stream << "{\\tiny{" << tr("IMPORTE") << "}} & ";
    stream << "{\\tiny{" << tr("RAZÓN") << "}} & ";
    stream << "{\\tiny{" << tr("PAIS") << "}} & ";
    stream << "{\\tiny{" << tr("NIF-IVA") << "}} & ";
    stream << "{\\tiny{" << tr("TIPO") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // --------------------------------------------------------------------------------------------
    int fila=0;
    double suma=0;

    while (fila<ui->tableWidget->rowCount())
          {
             stream << "{\\tiny " << filtracad(ui->tableWidget->item(fila,0)->text()) << "} & {\\tiny ";
             stream << formateanumerosep(convapunto(ui->tableWidget->item(fila,1)->text()).toDouble(),
                                      comadecimal,decimales) << "} & {\\tiny ";
             stream << filtracad(ui->tableWidget->item(fila,2)->text()) << "} & {\\tiny ";
             stream << filtracad(ui->tableWidget->item(fila,3)->text()) << "} & {\\tiny ";
             stream << filtracad(ui->tableWidget->item(fila,4)->text()) << "} & {\\tiny ";
             stream << filtracad(ui->tableWidget->item(fila,5)->text());
             stream << "} \\\\ \n  " << "\\hline\n";
             suma+=convapunto(ui->tableWidget->item(fila,1)->text()).toDouble();
             fila++;
             QApplication::processEvents();
          }

    stream << "\\multicolumn{6}{|c|} {\\textbf{";
   // --------------------------------------------------------------------------------------
    stream << tr("SUMA OPERACIONES:  ");
    stream << formateanumerosep(suma,comadecimal,decimales) << "}}";
    stream <<  " \\\\ \n";
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    // stream << "\\end{landscape}\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}


void modelo349::imprime()
{
    generalatex();

   int valor=imprimelatex2(tr("modelo_349"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir volumen de operaciones"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void modelo349::edlatex()
{
    generalatex();
    int valor=editalatex("modelo_349");
    if (valor==1)
        QMessageBox::warning( this, tr("Relación de operaciones"),
                              tr("PROBLEMAS al llamar al editor Latex"));

}


void modelo349::copiar()
{
   QString cadena;
   cadena+=nombreempresa();
   cadena+="\n\n";

   cadena+=tr("FECHA INICIAL:\t");
   cadena+=ui->inicialdateEdit->date().toString("dd.MM.yyyy");
   cadena+=tr("\tFECHA FINAL:\t");
   cadena+=ui->finaldateEdit->date().toString("dd.MM.yyyy");
   cadena+="\n\n";


   cadena+=tr("REGISTROS MODELO 349");
   // --------------------------------------------------------------------------------------
   cadena+="\n\n";
    // -----------------------------------------------------------------------------------------
   cadena+= tr("CUENTA");
   cadena+="\t";
   cadena+= tr("IMPORTE");
   cadena+="\t";
   cadena+= tr("RAZÓN");
   cadena+="\t";
   cadena+= tr("PAIS");
   cadena+="\t";
   cadena+= tr("NIF-IVA");
   cadena+="\t";
   cadena+= tr("TIPO");
   cadena+="\n";
    // -------------------------------------------------------------------------------------------
    int fila=0;
    double suma=0;

    while (fila<ui->tableWidget->rowCount())
          {
             cadena+=ui->tableWidget->item(fila,0)->text();
             cadena+="\t";
             cadena+=formateanumero(convapunto(ui->tableWidget->item(fila,1)->text()).toDouble(),
                                      comadecimal,decimales);
             cadena+="\t";
             cadena+=ui->tableWidget->item(fila,2)->text();
             cadena+="\t";
             cadena+=ui->tableWidget->item(fila,3)->text();
             cadena+="\t";
             cadena+=ui->tableWidget->item(fila,4)->text();
             cadena+="\t";
             cadena+=ui->tableWidget->item(fila,5)->text();
             cadena+="\n";
             suma+=convapunto(ui->tableWidget->item(fila,1)->text()).toDouble();
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


bool modelo349::genfich349(QString nombre)
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
   QString contenido="1349";

   QString cadanyo;
   cadanyo.setNum(ui->finaldateEdit->date().year());
   contenido+=cadanyo.trimmed();

   QString nif=completacadnum(ui->niflineEdit->text(),9);
   contenido+=nif;

   QString nombred=completacadcad(adaptacad_aeat(ui->nombrelineEdit->text()),40);
   contenido+=nombred;

   contenido+="T"; // Presentación telemática

   QString tfno=completacadnum(adaptacad_aeat(ui->telefonolineEdit->text()),9);
   contenido+=tfno;

   QString nombrerel=completacadcad(adaptacad_aeat(ui->nombrerellineEdit->text()),40);
   contenido+=nombrerel;

   contenido+="3490000000000";

   if (ui->complgroupBox->isChecked())
      {
        if (ui->complementariaradioButton->isChecked())
           contenido+="C ";
         else
             contenido+=" S";
        contenido+=completacadnum(ui->anteriorlineEdit->text(),13);
      }
      else
          {
           contenido+="  ";
           contenido+="0000000000000";
          }

  // Período: dos posiciones

  contenido+=ui->periodocomboBox->currentText().left(2);

  // Número total de personas y entidades
  QString numregistros;
  numregistros.setNum(ui->tableWidget->rowCount());
  numregistros=completacadnum(numregistros.trimmed(),9);
  contenido+=numregistros;

  // importe anual de las operaciones
  // hay que sumar la columna 1

  double total_operaciones=0;
  for (int veces=0; veces<ui->tableWidget->rowCount(); veces++)
     {
      total_operaciones+=convapunto(ui->tableWidget->item(veces,1)->text()).toDouble();
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

  // Número operadores con rectificaciones
  contenido+="000000000";

  // Importe de las rectificaciones
  contenido+="000000000000000";

  // indicador cambio periodicidad
  contenido+=" ";

  // 204 blancos
  QString str; str.fill(' ', 204);
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

  for (int veces=0; veces<ui->tableWidget->rowCount(); veces++)
      {

       if (convapunto(ui->tableWidget->item(veces,1)->text().trimmed()).toDouble()<0)
          {
           QString cadnum; cadnum.setNum(veces+1);
           QMessageBox::warning( this, tr("MODELO 349"),tr("El contenido de la línea ")+cadnum + tr(" es negativo\n")
                                 +tr("Este registro no lo admite el Modelo 349, por lo que se omite"));
           continue;
          }

       contenido+="2349";

       cadanyo.setNum(ui->finaldateEdit->date().year());
       contenido+=cadanyo.trimmed();

       nif=completacadnum(ui->niflineEdit->text(),9);
       contenido+=nif;

       // Blancos 58 posiciones
       str.clear(); str.fill(' ',58);
       contenido+=str;


       // nif operador intracomunitario
       contenido+=completacadcad(ui->tableWidget->item(veces,3)->text(),2);
       nif=completacadcad(ui->tableWidget->item(veces,4)->text().trimmed(),15);
       contenido+=nif;

       QString denominacion;
       denominacion=ui->tableWidget->item(veces,2)->text();
       denominacion.replace(',',' ');
       denominacion.replace("  "," ");
       contenido+=completacadcad(adaptacad_aeat(denominacion),40);

       // CLAVE DE OPERACIÓN
       contenido+=completacadcad(ui->tableWidget->item(veces,5)->text(),1);

       // Importe base imponible
       QString cad_vol_op=convapunto(ui->tableWidget->item(veces,1)->text().trimmed().remove('-'));
       // double importe=cad_vol_op.toDouble();

       QString cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
       QString cad_decimal=cad_vol_op.section(".",1,1).trimmed();
       cad_entero=completacadnum(cad_entero,11);
       contenido+=cad_entero;
       cad_decimal=completacadnum(cad_decimal,2);
       contenido+=cad_decimal;

       // BLANCOS 354
       str.clear();
       str.fill(' ',354);
       //contenido+=" ";
       contenido+=str;
       if (veces<ui->tableWidget->rowCount()-1) contenido+="\r\n";
  // ----------------------------------------------------------------------------------------
   }


  docstream << contenido;
  doc.close();

  QMessageBox::information( this, tr("Fichero 349"),tr("349 generado en archivo: ")+nombre);

  return true;
}



void modelo349::genera349()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajobd());
  dir->activa_pide_archivo(tr("modelo.349"));
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
           if (nombre.right(4)!=".347") nombre=nombre+".349";
           // QMessageBox::information( this, tr("selección"),
           //                         nombre );
           QString rutadir=dir->directorio_actual();
           if (!rutadir.endsWith(QDir::separator())) rutadir.append(QDir::separator());
           nombre=rutadir+nombre;
           // QMessageBox::information( this, tr("selección"),
           //                          nombre );
           // if (!genfich347(nombre))
           //   QMessageBox::information( this, tr("Fichero 347"),tr("NO se ha generado correctamente el fichero 347"));
           if (nombre.right(4)!=".349") nombre=nombre+".349";
           if (!genfich349(nombre))
              QMessageBox::information( this, tr("Fichero 349"),tr("NO se ha generado correctamente el fichero 349"));
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
  QString filtrotexto=tr("Archivos 349 (*.349)");
  filtros << filtrotexto;
  filtros << tr("Archivos 349 (*.349)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO 349"));
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
           if (nombre.right(4)!=".349") nombre=nombre+".349";
           if (!genfich349(nombre))
              QMessageBox::information( this, tr("Fichero 349"),tr("NO se ha generado correctamente el fichero 349"));
          }
       }
#endif

}
