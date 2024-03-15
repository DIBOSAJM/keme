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

#include "modelo340.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "directorio.h"
#include <QFileDialog>
#include <QMessageBox>

modelo340::modelo340() : QDialog() {
    ui.setupUi(this);


  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej;
  if ( query.isActive() ) {
          while ( query.next() )
                ej << query.value(0).toString();
          }
  ui.ejerciciocomboBox->addItems(ej);
  actufechas();
  ui.niflineEdit->setText(basedatos::instancia()->cif());
  ui.nombrelineEdit->setText(nombreempresa());

  connect(ui.periodocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));
  connect(ui.ejerciciocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));
  connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(genera340()));

}

void modelo340::actufechas()
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
    if (!periodo.contains('T'))
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


void modelo340::genera340()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajobd());
  dir->activa_pide_archivo(tr("modelo.340"));
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
           if (nombre.right(4)!=".340") nombre=nombre+".340";
           // QMessageBox::information( this, tr("selección"),
           //                         nombre );
           QString rutadir=dir->directorio_actual();
           if (!rutadir.endsWith(QDir::separator())) rutadir.append(QDir::separator());
           nombre=rutadir+nombre;
           if (genfich340(nombre))
              QMessageBox::information( this, tr("Fichero 340"),tr("Se ha generado correctamente el fichero 340"));
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
  QString filtrotexto=tr("Archivos 340 (*.340)");
  filtros << filtrotexto;
  filtros << tr("Archivos 340 (*.340)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO 340"));
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
           if (nombre.right(4)!=".340") nombre=nombre+".340";
           if (genfich340(nombre))
              QMessageBox::information( this, tr("Fichero 340"),tr("Se ha generado correctamente el fichero 340"));
          }
       }
#endif
}



bool modelo340::genfich340(QString nombre)
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
       QMessageBox::warning( this, tr("Fichero 340"),tr("Error: Imposible grabar fichero"));
       return false;
      }
   QTextStream docstream( &doc );
   //docstream.setCodec("ISO 8859-1");
   docstream.setEncoding(QStringConverter::Latin1);
   QString contenido="1340";

   QString cadanyo;
   cadanyo.setNum(ui.finaldateEdit->date().year());
   contenido+=cadanyo.trimmed();

   QString nif=completacadnum(ui.niflineEdit->text(),9);
   contenido+=nif;

   QString nombred=completacadcad(adaptacad_aeat(ui.nombrelineEdit->text()),40);
   contenido+=nombred;

   contenido+="T"; // Presentación telemática

   QString tfno=completacadnum(adaptacad_aeat(ui.telefonolineEdit->text()),9);
   contenido+=tfno;

   QString nombrerel=completacadcad(adaptacad_aeat(ui.nombrerellineEdit->text()),40);
   contenido+=nombrerel;

   contenido+="340";
   contenido+=cadanyo;

   QString periodo=ui.periodocomboBox->currentText().section(" ",0,0).trimmed();
   if (periodo=="1T") periodo="03";
   if (periodo=="2T") periodo="06";
   if (periodo=="3T") periodo="09";
   if (periodo=="4T") periodo="12";
   contenido+=periodo;

   QString cadnumsec;
   cadnumsec.setNum(ui.numerospinBox->value());
   cadnumsec=completacadnum(cadnumsec.trimmed(),4);
   contenido+=adaptacad_aeat(cadnumsec);

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

   contenido+=ui.periodocomboBox->currentText().section(" ",0,0).trimmed();

  QDate inicial, final;
  inicial=ui.inicialdateEdit->date();
  final=ui.finaldateEdit->date();
  int nregistros;
  double sumabi;
  double sumacuota;
  double itotal;

  basedatos::instancia()->agregados340(inicial, final, &nregistros,
                                       &sumabi, &sumacuota, &itotal);

  QString cadnumreg;
  cadnumreg.setNum(nregistros);
  cadnumreg=completacadnum(cadnumreg.trimmed(),9);
  contenido+=cadnumreg;

  // Signo del total bi
  if (sumabi<0) contenido+="N"; else contenido+=" ";

  // Importe bi - separamos parte entera de la decimal

  QString cadbi;
  cadbi.setNum(sumabi,'f',2);
  QString cadbientera=cadbi.section(".",0,0).trimmed();
  QString cadbidecimal=cadbi.section(".",1,1).trimmed();
  cadbientera=completacadnum(cadbientera,15);
  contenido+=cadbientera;
  cadbidecimal=completacadnum(cadbidecimal,2);
  contenido+=cadbidecimal;

  // Signo del total cuota
  if (sumacuota<0) contenido+="N"; else contenido+=" ";

  // Cuota del impuesto
  QString cadcuota;
  cadcuota.setNum(sumacuota,'f',2);
  QString cadentera=cadcuota.section(".",0,0).trimmed();
  QString caddecimal=cadcuota.section(".",1,1).trimmed();
  cadentera=completacadnum(cadentera,15);
  contenido+=cadentera;
  caddecimal=completacadnum(caddecimal,2);
  contenido+=caddecimal;

  // Signo del total facturas
  if (itotal<0) contenido+="N"; else contenido+=" ";

  // Cuota del impuesto
  QString cadtotal;
  cadtotal.setNum(itotal,'f',2);
  cadentera=cadtotal.section(".",0,0).trimmed();
  caddecimal=cadtotal.section(".",1,1).trimmed();
  cadentera=completacadnum(cadentera,15);
  contenido+=cadentera;
  caddecimal=completacadnum(caddecimal,2);
  contenido+=caddecimal;

  QString str; str.fill(' ', 190);
  contenido+=str;

  // nif representante legal
  str.clear();
  str.fill(' ',9);
  contenido+=str;

  // código electrónico autoliquidación IVA
  str.clear();
  str.fill(' ',16);
  contenido+=str;

  // blancos al final del registro
  str.clear();
  str.fill(' ',85);
  contenido+=str;


  // REGISTRO DE DETALLE
  // Libro de facturas expedidas

  QSqlQuery query=basedatos::instancia()->datoslibroiva(
                                                        inicial, final);
  int numrecibidas=0;
  int numemitidas=0;
  double sumabisoportado=0;
  double sumabirepercutido=0;
  double sumacuotasoportado=0;
  double sumacuotarepercutido=0;
  double sumacuotare=0;
  if ( query.isActive() )
    while (query.next())
      {
       contenido+="\r\n";
       contenido+="2340";

       cadanyo.setNum(ui.finaldateEdit->date().year());
       contenido+=cadanyo.trimmed();

       nif=completacadnum(ui.niflineEdit->text(),9);
       contenido+=nif;

       QString externo=query.value(24).toString();
       bool hayexterno=!externo.isEmpty();

       QString cif;
       if (hayexterno)
          {
           cif=basedatos::instancia()->cif_externo(externo);
           if (cif.isEmpty()) cif=query.value(23).toString();
          }
         else
             {
              if (query.value(23).toString().trimmed().isEmpty())
                cif = query.value(0).toString();
               else cif=query.value(23).toString();
             }

       if (query.value(1)=="1" && cif.length()>0) // corresponde a un nif
         {
           cif=completacadnum(cif,9);
           contenido+=cif;
         }
         else
             {
               str.clear();
               str.fill(' ',9);
               contenido+=str;
             }

       if (query.value(2).toString().length()>0) // nif rpr legal
         {
           contenido+=completacadnum(query.value(2).toString(),9);
         }
         else
             {
               str.clear();
               str.fill(' ',9);
               contenido+=str;
             }

       QString razon; // razon, apellidos-nombre
       if (hayexterno)
         {
           razon=adaptacad_aeat(basedatos::instancia()->razon_externo(externo));
           if (razon.isEmpty()) razon=adaptacad_aeat(query.value(22).toString());
         }
         else
             {
              if (!query.value(22).toString().trimmed().isEmpty())
                razon=adaptacad_aeat(query.value(22).toString());
              else
                razon=adaptacad_aeat(query.value(3).toString());
             }

       contenido+=completacadcad(razon,40);
       contenido+=completacadcad(query.value(4).toString(),2); // pais
       contenido+=completacadcad(adaptacad_aeat(query.value(1).toString()),1); // clave identificación fiscal
       if (query.value(1)!="1")
          {
           if (!hayexterno)
             contenido+=completacadcad(adaptacad_aeat(!query.value(23).toString().trimmed().isEmpty() ?
                                                        query.value(23).toString().trimmed(): query.value(0).toString()),20); // id fiscal país residencia
             else contenido+=completacadcad(adaptacad_aeat(cif),20);
          }
          else
              {
               str.clear();
               str.fill(' ',20);
               contenido+=str;
              }
       // ahora viene la clave de tipo LIBRO
       if (!query.value(19).toBool())
          {
           // estamos en expedidas
           if (conigic()) contenido+="F";
             else contenido+="E";
          }
           else
               {
                // estamos en recibidas
                if (conigic()) contenido+="S";
                 else contenido+="R";
               }
       contenido+=query.value(5).toString().length()==1 ? query.value(5).toString() : " "; // clave operación
       contenido+=query.value(6).toDate().toString("yyyyMMdd");
       contenido+=query.value(7).toDate().toString("yyyyMMdd");

       QString cadtipo;
       cadtipo.setNum(query.value(8).toDouble(),'f',2);
       QString cadentera=cadtipo.section(".",0,0).trimmed();
       QString caddecimal=cadtipo.section(".",1,1).trimmed();
       cadentera=completacadnum(cadentera,3);
       contenido+=cadentera;
       caddecimal=completacadnum(caddecimal,2);
       contenido+=caddecimal;

       QString cadbase;
       cadbase.setNum(query.value(9).toDouble(),'f',2);
       if (query.value(9).toDouble()<0)
          contenido+="N";
         else contenido+=" ";
       cadentera=cadbase.section(".",0,0).trimmed();
       cadentera=cadentera.remove("-");
       caddecimal=cadbase.section(".",1,1).trimmed();
       cadentera=completacadnum(cadentera,11);
       contenido+=cadentera;
       caddecimal=completacadnum(caddecimal,2);
       contenido+=caddecimal;

       QString cadcuota;
       cadcuota.setNum(
        CutAndRoundNumberToNDecimals(query.value(9).toDouble()*query.value(8).toDouble()/100,2),
           'f',2);
       // QMessageBox::information( this, tr("340"), query.value(8).toString());
       if (query.value(9).toDouble()<0)
          contenido+="N";
         else contenido+=" ";
       cadentera=cadcuota.section(".",0,0).trimmed();
       cadentera=cadentera.remove("-");
       caddecimal=cadcuota.section(".",1,1).trimmed();
       cadentera=completacadnum(cadentera,11);
       contenido+=cadentera;
       caddecimal=completacadnum(caddecimal,2);
       contenido+=caddecimal;

       QString cadre;
       cadre.setNum(
         CutAndRoundNumberToNDecimals(query.value(9).toDouble()*query.value(16).toDouble()/100,2)
            ,'f',2);


       double total=cadbase.toDouble()+cadcuota.toDouble()+cadre.toDouble();
       QString cadtotal;
       cadtotal.setNum(total,'f',2);
       if (total<0)
          contenido+="N";
         else contenido+=" ";
       cadentera=cadtotal.section(".",0,0).trimmed();
       cadentera=cadentera.remove("-");
       caddecimal=cadtotal.section(".",1,1).trimmed();
       cadentera=completacadnum(cadentera,11);
       contenido+=cadentera;
       caddecimal=completacadnum(caddecimal,2);
       contenido+=caddecimal;

       QString cadbicosto;
       cadbicosto.setNum(query.value(11).toDouble(),'f',2);
       if (query.value(11).toDouble()<0)
          contenido+="N";
         else contenido+=" ";
       cadentera=cadbicosto.section(".",0,0).trimmed();
       cadentera=cadentera.remove("-");
       caddecimal=cadbicosto.section(".",1,1).trimmed();
       cadentera=completacadnum(cadentera,11);
       contenido+=cadentera;
       caddecimal=completacadnum(caddecimal,2);
       contenido+=caddecimal;

       QString idfactura=completacadcad(adaptacad_aeat(query.value(17).toString()),40); // id factura
       contenido+=idfactura;

       // número de registro
       int numero=0;
       if (query.value(19).toBool())
          {
           numero=++numrecibidas;
          }
           else
            {
             numero=++numemitidas;
            }
       QString cadnum;
       cadnum.setNum(numero);
       cadnum=cadnum.trimmed();
       contenido+=completacadcad(cadnum,18);


       if (!query.value(19).toBool())  // contenido específico para libro facturas expedidas
          {
           // Número de facturas
           bool asientoresumen=false;
           if (query.value(5).toString()=="A" || query.value(5).toString()=="B")
               asientoresumen=true;
           if (asientoresumen)
             {
              cadnum.setNum(query.value(12).toInt());
              cadnum=cadnum.trimmed();
              contenido+=completacadnum(cadnum,8);
             }
             else contenido+="00000001";

           contenido+="01"; // siempre un asiento por apunte

           // intervalo acumulación
           contenido+=completacadcad(adaptacad_aeat(query.value(13).toString()),40);
           contenido+=completacadcad(adaptacad_aeat(query.value(14).toString()),40);

           // factura rectificada
           contenido+=completacadcad(adaptacad_aeat(query.value(15).toString()),40);

           // Recargo de equivalencia
           QString cadtipore;
           cadtipore.setNum(query.value(16).toDouble(),'f',2);
           cadentera=cadtipore.section(".",0,0).trimmed();
           cadentera=cadentera.remove("-");
           caddecimal=cadtipore.section(".",1,1).trimmed();
           cadentera=completacadnum(cadentera,3);
           contenido+=cadentera;
           caddecimal=completacadnum(caddecimal,2);
           contenido+=caddecimal;

           // cuota del RE

           QString cadcuotare;
           cadcuotare.setNum(
            CutAndRoundNumberToNDecimals(query.value(9).toDouble()*query.value(16).toDouble()/100,2)
            ,'f',2);
           if (query.value(9).toDouble()<0)
             contenido+="N";
            else contenido+=" ";
           QString cadentera=cadcuotare.section(".",0,0).trimmed();
           cadentera.remove('-');
           QString caddecimal=cadcuotare.section(".",1,1).trimmed();
           cadentera=completacadnum(cadentera,11);
           contenido+=cadentera;
           caddecimal=completacadnum(caddecimal,2);
           contenido+=caddecimal;

           sumabirepercutido+=query.value(9).toDouble();
           sumacuotarepercutido+=cadcuota.toDouble();
           sumacuotare+=cadcuotare.toDouble();
           // 384 último - comienza 385
           contenido+="0"; // 385 Situación inmueble
           // 386 a 410 --> 25 caracteres -- referencia catastral
           QString str;
           str.fill(' ', 25);
           contenido+=str;
           // 411 a 425 importe percibido en metálico 15 caracteres - número
           str.clear();str.fill('0',15);
           contenido+=str;
           // 426 a 429 ejercicio operaciones en metálico - 4 caracteres - número
           contenido+="0000";
           // 430 a 444 importe transmisiones sujetas a IVA 15 caracteres - número
           str.clear();str.fill('0',15);
           contenido+=str;
           // 445 a 500 blancos -- 56 caracteres
           str.clear();str.fill(' ',56);
           contenido+=str;

          }
          else // contenido específico para facturas recibidas
              {
               // Número de facturas
               bool asientoresumen=false;
               if (query.value(5).toString()=="A" || query.value(5).toString()=="B")
                 asientoresumen=true;
               if (asientoresumen)
                {
                 cadnum.setNum(query.value(12).toInt());
                 cadnum=cadnum.trimmed();
                 contenido+=completacadnum(cadnum,18);
                }
                else contenido+="000000000000000001";

              contenido+="01"; // siempre un asiento por apunte 254 a 255

              // intervalo acumulación
              contenido+=completacadcad(adaptacad_aeat(query.value(13).toString()),40);
              contenido+=completacadcad(adaptacad_aeat(query.value(14).toString()),40);

              // 336 a 349
              cadcuota.setNum(
                CutAndRoundNumberToNDecimals(query.value(9).toDouble()*query.value(8).toDouble()/100*
                              query.value(21).toDouble(),2),'f',2); // ésta es la prorrata
              if (query.value(9).toDouble()<0)
                contenido+="N";
              else contenido+=" ";
              cadentera=cadcuota.section(".",0,0).trimmed();
              cadentera=cadentera.remove('-');
              caddecimal=cadcuota.section(".",1,1).trimmed();
              cadentera=completacadnum(cadentera,11);
              contenido+=cadentera;
              caddecimal=completacadnum(caddecimal,2);
              contenido+=caddecimal;

              str.fill(' ', 151);
              contenido+=str;
              sumabisoportado+=query.value(9).toDouble();;
              sumacuotasoportado+=cadcuota.toDouble();

              }

      } // fin del while
           /*QString cad="select s.cif, s.claveidfiscal, s.nifrprlegal, s.razon, s.pais, "
                "l.clave_operacion, l.fecha_fra, l.fecha_operacion, l.tipo_iva, "
                "l.base_iva, l.cuota_iva, l.bi_costo, l.nfacturas, l.finicial, l.ffinal,"
                "l.rectificada, l.tipo_re, d.documento, d.fecha, l.soportado, l.prorrata from "
                "diario d, libroiva l, datossubcuenta s where "
                "s.cuenta=d.cuenta and d.pase=l.pase and "; */

  QString cadsumabisoportado;
  QString cadsumabirepercutido;
  QString cadsumacuotasoportado;
  QString cadsumacuotarepercutido;
  QString cadsumacuotare;

  cadsumabisoportado.setNum(sumabisoportado,'f',2);
  cadsumabirepercutido.setNum(sumabirepercutido,'f',2);
  cadsumacuotasoportado.setNum(sumacuotasoportado,'f',2);
  cadsumacuotarepercutido.setNum(sumacuotarepercutido,'f',2);
  cadsumacuotare.setNum(sumacuotare,'f',2);

  QMessageBox::information( this, tr("FICHERO 340"),tr("BASE IMPONIBLE IVA SOPORTADO: ")+cadsumabisoportado+"\n"+
                            tr("CUOTA IVA SOPORTADO: ")+cadsumacuotasoportado+"\n"+
                            tr("BASE IMPONIBLE IVA REPERCUTIDO: ")+cadsumabirepercutido+"\n"+
                            tr("CUOTA IVA REPERCUTIDO: ")+cadsumacuotarepercutido+"\n"+
                            tr("RECARGO DE EQUIVALENCIA: ")+cadsumacuotare
                            );

  docstream << contenido;
  doc.close();

  return true;
}
