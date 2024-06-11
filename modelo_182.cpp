#include "modelo_182.h"
#include "ui_modelo_182.h"
#include "basedatos.h"
#include "funciones.h"
#include <QProgressDialog>
#include "datos_accesorios.h"
#include "externos.h"
#include "consmayor.h"
#include <QMessageBox>
#include <QFileDialog>

modelo_182::modelo_182(QString qusuario, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelo_182)
{
    ui->setupUi(this);
    usuario=qusuario;
    QStringList columnas;
    columnas << tr("CUENTA") << tr("EXTERNO") << tr("IMPORTE");
    columnas << tr("RAZON") << tr("NIF") << tr("POBLACION");
    columnas << tr("C.P.") << tr("PROVINCIA");
    columnas << tr("DED.");
    columnas << tr("CLAVE");
    columnas << tr("ESPECIE");
    columnas << tr("RECURRENTE");
    columnas << tr("COMUN.AUTON.");
    columnas << tr("DED.AUTON.");

    ui->tableWidget->setColumnCount(14);

    ui->tableWidget->setHorizontalHeaderLabels(columnas);

     QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
     QStringList ej1;

     if ( query.isActive() ) {
             while ( query.next() )
                   ej1 << query.value(0).toString();
         }
     ui->ejercicio_comboBox->addItems(ej1);

     comadecimal=haycomadecimal(); decimales=haydecimales();

     QString empresa, nif, nombre, apellidos;
     basedatos::instancia()->empresa_nif_nombre_apellidos(&empresa, &nif, &nombre, &apellidos);

     ui->niflineEdit->setText(nif);
     ui->nombrelineEdit->setText(empresa);
     if (!apellidos.isEmpty()) {
         ui->nombrelineEdit->setText(apellidos + " " + nombre);
     }

}

modelo_182::~modelo_182()
{
    delete ui;
}


/* void modelo_182::cargadatos()
{
   borratabla();

   QProgressDialog progreso(tr("Cargando información ... ")
                            , 0, 0, 0);
   progreso.setWindowModality(Qt::WindowModal);
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
} */
void modelo_182::on_genera_pushButton_clicked()
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


bool modelo_182::genfich182(QString nombre) {
    QFile plan( adapta(nombre)  );
    if (plan.exists() && QMessageBox::question(this,
          tr("¿ Sobreescribir ?"),
          tr("'%1' ya existe."
         "¿ Desea sobreescribirlo ?")
          .arg( nombre )) == QMessageBox::No)
              return false;

      QFile doc( adapta(nombre)  );

      if ( !doc.open( QIODevice::WriteOnly ) )
         {
          QMessageBox::warning( this, tr("Fichero 182"),tr("Error: Imposible grabar fichero"));
          return false;
         }
      QTextStream docstream( &doc );
      docstream.setEncoding(QStringConverter::Latin1);
      QString contenido="1182";

      QString cadanyo;
      cadanyo.setNum(inicioejercicio(ui->ejercicio_comboBox->currentText()).year());
      contenido+=cadanyo.trimmed();

      QString nif=completacadnum(basedatos::instancia()->cif(),9);
      contenido+=nif;

      QString nombred=completacadcad(adaptacad_aeat(
              basedatos::instancia()->selectEmpresaconfiguracion ()),40);
      contenido+=nombred;

      contenido+="T"; // Presentación telemática

      QString tfno=completacadnum(adaptacad_aeat(ui->telefonolineEdit->text()),9);
      contenido+=tfno;
      //QString str; str.fill('0', 9); contenido+=str;

      QString nombrerel=completacadcad(adaptacad_aeat(ui->nombrerellineEdit->text()),40);
      contenido+=nombrerel;
      //str.fill(' ', 40); contenido+=str;

      contenido+="1820000000000";


      if (ui->compl_sus_groupBox->isChecked()) {
          if (ui->sustitutiva_radioButton) {
              contenido+=" S";
              contenido+=completacadnum(ui->anteriorlineEdit->text().trimmed(),13);
          } else {
              contenido+="C ";
              contenido+="0000000000000";
          }
      }
      else {
             contenido+="  "; // declaración complementaria - declaración sustitutiva
             contenido+="0000000000000"; // número identificativo de la declaración anterior
      }
      // Número total de personas y entidades
      QString numregistros;
      numregistros.setNum(ui->tableWidget->rowCount());
      numregistros=completacadnum(numregistros.trimmed(),9);
      contenido+=numregistros;

      double total_operaciones=0;
      for (int veces=0; veces<ui->tableWidget->rowCount(); veces++)
         {
          total_operaciones+=convapunto(ui->tableWidget->item(veces,2)->text()).toDouble();
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
      QString cadnum; cadnum.setNum(ui->naturaleza_comboBox->currentIndex()+1);
      contenido+=cadnum.trimmed();

      QString str;
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

      for (int veces=0; veces<ui->tableWidget->rowCount(); veces++)
          {
           contenido+="2182";

           cadanyo.setNum(inicioejercicio(ui->ejercicio_comboBox->currentText()).year());
           contenido+=cadanyo.trimmed();
           nif=completacadnum(basedatos::instancia()->cif(),9); // nif declarante
           contenido+=nif;

           // nif del declarado
           nif=completacadnum(ui->tableWidget->item(veces,4)->text().trimmed().remove(' '),9);
           contenido+=nif;

           // nif del representate legal (lo suponemos vacío)
           // sólo sirve para s.p. menores de 14 años

           str.clear();
           str.fill(' ',9);
           contenido+=str;

           QString denominacion;
           denominacion=ui->tableWidget->item(veces,3)->text();
           denominacion.replace(',',' ');
           denominacion.replace("  "," ");
           contenido+=completacadcad(adaptacad_aeat(denominacion),40);

           // CÓDIGO PROVINCIA
           contenido+=completacadnum(ui->tableWidget->item(veces,6)->text().left(2),2);

           // Clave operación
           contenido+=ui->tableWidget->item(veces,9)->text().left(1);// -----------------------------------------------------

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

           QString cadporcentaje=convapunto(ui->tableWidget->item(veces,8)->text()); //convapunto(ui.deduclineEdit->text());
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
           QString cad_vol_op=convapunto(ui->tableWidget->item(veces,2)->text().trimmed().remove('-'));

           QString cad_entero=cad_vol_op.section(".",0,0).trimmed().remove('-');
           QString cad_decimal=cad_vol_op.section(".",1,1).trimmed();
           cad_entero=completacadnum(cad_entero,11);
           contenido+=cad_entero;
           cad_decimal=completacadnum(cad_decimal,2);
           contenido+=cad_decimal;

           // DONATIVO NO EN ESPECIE
           if (ui->tableWidget->item(veces,10)->text()=="X") contenido+="X";
            else contenido+=" ";

          // COMUNIDAD AUTÓNOMA --- /***********************************
           if (ui->tableWidget->item(veces,12)->text()=="--")
            contenido+="00";
           else contenido+=ui->tableWidget->item(veces,12)->text();

          // DEDUCCIÓN COMUNIDAD AUTÓNOMA
          cadporcentaje=convapunto(ui->tableWidget->item(veces,13)->text());
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

           // TIPO DE BIEN (SOLO SI ESPECIE) y claves C Ó D
           contenido+=" ";

           // IDENTIFICACIÓN DEL BIEN (SOLO SI ESPECIE)
           str.clear();
           str.fill(' ',20);
           contenido+=str;

           // RECURRENCIA DONATIVOS
           if (ui->tableWidget->item(veces,11)->text()=="X") contenido+="1";
             else contenido+="2"; // no hay recurrencia

           // BLANCOS 118
           str.clear();
           str.fill(' ',118);
           contenido+=str;
           if (veces<ui->tableWidget->rowCount()-1) contenido+="\r\n";
      // ----------------------------------------------------------------------------------------
       }


      docstream << contenido;
      doc.close();

      QMessageBox::information( this, tr("Fichero 182"),tr("182 generado en archivo: ")+nombre);

      return true;

}


void modelo_182::on_carga_pushButton_clicked()
{
    ui->tableWidget->setRowCount(0);
    QProgressDialog progreso(tr("Cargando información ... ")
                             , 0, 0, 0);
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.show();
    progreso.update();
    QApplication::processEvents();

    QDate fecha_inicial=basedatos::instancia()->selectAperturaejercicios(ui->ejercicio_comboBox->currentText());
    QDate fecha_final=basedatos::instancia()->selectCierreejercicios(ui->ejercicio_comboBox->currentText());

    QSqlQuery q = basedatos::instancia()->datos_donaciones(fecha_inicial, fecha_final);

    /*columnas << tr("CUENTA") << tr("EXTERNO") << tr("IMPORTE");
    columnas << tr("RAZON") << tr("CIF") << tr("POBLACION");
    columnas << tr("C.P.") << tr("PROVINCIA");
    columnas << tr("DED.");
    columnas << tr("CLAVE");
    columnas << tr("ESPECIE");
    columnas << tr("RECURRENTE");
    columnas << tr("COMUN.AUTON.");
    columnas << tr("DED.AUTON.");*/

    int fila=0;
    while (q.next())
       {
          ui->tableWidget->insertRow(fila);
          // l.cuenta_fra, sum(l.base_iva), d.externo, l.clave_donacion, l.donacion_especie, l.donacion_2ejer,
          // l.comunidad_autonoma, l.porcent_deduc_autonomia
          ui->tableWidget->setItem(fila,0,new QTableWidgetItem(q.value(0).toString()));
          ui->tableWidget->setItem(fila,1,new QTableWidgetItem(q.value(2).toString()));
          QTableWidgetItem *newItem1 = new QTableWidgetItem(
                               formateanumero(q.value(1).toDouble(),comadecimal,decimales));
          newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(fila,2,newItem1);
          if (ui->naturaleza_comboBox->currentText().left(1)=="1") { // -----------------------

              if (q.value(5).toBool()) ui->tableWidget->setItem(fila,8, new QTableWidgetItem("40"));
                 else ui->tableWidget->setItem(fila,8,new QTableWidgetItem("35"));
          }

          ui->tableWidget->setItem(fila,9,new QTableWidgetItem(q.value(3).toString()));
          if (q.value(4).toBool()) // especie
             ui->tableWidget->setItem(fila,10, new QTableWidgetItem("X"));
            else ui->tableWidget->setItem(fila,10, new QTableWidgetItem(""));
          if (q.value(5).toBool()) // recurrente
             ui->tableWidget->setItem(fila,11, new QTableWidgetItem("X"));
            else ui->tableWidget->setItem(fila,11, new QTableWidgetItem(""));
          ui->tableWidget->setItem(fila,12,new QTableWidgetItem(q.value(6).toString()));
          ui->tableWidget->setItem(fila,13,new QTableWidgetItem(formateanumero(q.value(7).toDouble(),comadecimal,decimales)));
          fila++;
          QApplication::processEvents();
        }

    for (int linea=0; linea<ui->tableWidget->rowCount(); linea++)
       {
          QString externo=ui->tableWidget->item(linea,1)->text().trimmed();
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
              ui->tableWidget->setItem(linea,3,new QTableWidgetItem(identif));
              QString vat_id=consulta2.value(1).toString();
              ui->tableWidget->setItem(linea,4,new QTableWidgetItem(vat_id));

              // NATURALEZA DEL DECLARADO
              // Estudiamos la composición del NIF
              //   si es persona física empezará por número o X, Y, Z, K, L, M
              //   si empieza por letra E = Entidad atribución rentas
              // bool persona_fisica=true;
              bool persona_juridica=false;
              bool entidad_atrib_rentas=false;

              if (!vat_id.isEmpty())
                if (!QString("0123456789XYZKLM").contains(vat_id.left(1)))
                  {
                   //persona_fisica=false;
                   if (vat_id.left(1)=="E")
                      entidad_atrib_rentas=true;
                    else
                       persona_juridica=true;
                  }
              if (!persona_juridica && !entidad_atrib_rentas) {
                 if (ui->naturaleza_comboBox->currentText().left(1)=="2")
                     ui->tableWidget->setItem(linea,8,new QTableWidgetItem("10"));
                 if (ui->naturaleza_comboBox->currentText().left(1)=="1") {
                     if (convapunto(ui->tableWidget->item(linea,2)->text()).toDouble()<=150.0)
                         ui->tableWidget->setItem(linea,8,new QTableWidgetItem("80"));
                 }
              }


              ui->tableWidget->setItem(linea,5,new QTableWidgetItem(consulta2.value(2).toString())); // población
              ui->tableWidget->setItem(linea,6,new QTableWidgetItem(consulta2.value(3).toString())); // código postal
              ui->tableWidget->setItem(linea,7,new QTableWidgetItem(consulta2.value(4).toString())); // provincia
             }
       }

}

void modelo_182::on_borrarpushButton_clicked()
{
    int fila=ui->tableWidget->currentRow();
    if (fila>-1 && fila<ui->tableWidget->rowCount()) ui->tableWidget->removeRow(fila);

}

void modelo_182::on_datospushButton_clicked()
{
    if (ui->tableWidget->rowCount()==0) return;
    if (ui->tableWidget->currentRow()<0 ||
        ui->tableWidget->currentRow()>ui->tableWidget->rowCount()) return;
    QString cuenta=ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
    if (cuenta.isEmpty()) return;
    if (!existesubcuenta(cuenta)) return;
    QString externo=ui->tableWidget->item(ui->tableWidget->currentRow(),1)->text();
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

void modelo_182::on_mayorpushButton_clicked()
{
    if (ui->tableWidget->rowCount()<=0) return;
    if (ui->tableWidget->currentRow()>=ui->tableWidget->rowCount()) return;
    QString cuenta=ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
    if (cuenta.length()>0)
    {
        QDate fecha;
        consmayor *elmayor=new consmayor(comadecimal,decimales,usuario);
        fecha=basedatos::instancia()->selectAperturaejercicios(ui->ejercicio_comboBox->currentText());
        QString externo=ui->tableWidget->item(ui->tableWidget->currentRow(),1)->text();
        elmayor->pasadatos(cuenta,fecha,externo);
        elmayor->exec();
        delete(elmayor);
    }
}

void modelo_182::on_copiarpushButton_clicked()
{
    QString cadena;
    cadena+=nombreempresa();
    cadena+="\n\n";

    cadena+=tr("EJERCICIO:\t");
    cadena+=ui->ejercicio_comboBox->currentText();
    cadena+="\n\n";


    cadena+=tr("REGISTROS MODELO 182");
    // --------------------------------------------------------------------------------------
    cadena+="\n\n";
     // -----------------------------------------------------------------------------------------
    for (int veces=0; veces<ui->tableWidget->columnCount(); veces++) {
        cadena+=ui->tableWidget->horizontalHeaderItem(veces)->text();
        if (veces<(ui->tableWidget->columnCount()-1)) cadena+="\t";
    }
    cadena+="\n";
     // -------------------------------------------------------------------------------------------
     int fila=0;

     while (fila<ui->tableWidget->rowCount())
           {
              for (int veces=0; veces<ui->tableWidget->columnCount(); veces++) {
                  cadena+=ui->tableWidget->item(fila, veces)->text();
                  if (veces<(ui->tableWidget->columnCount()-1)) cadena+="\t";
              }
              cadena+="\n";
              fila++;
           }

    // --------------------------------------------------------------------------------------
     QClipboard *cb = QApplication::clipboard();
     cb->setText(cadena);
     QMessageBox::information( this, tr("Modelo 182"),
                               tr("Se ha pasado el contenido al portapapeles") );

}

