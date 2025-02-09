#include "tablasii.h"
#include "network_connections.h"
#include "ui_tablasii.h"
#include "basedatos.h"
#include "funciones.h"
#include "privilegios.h"
#include "editarasiento.h"
#include "pidenombre.h"
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "notas.h"
#include "respuesta_sii.h"
#include "respuesta_sii_cons.h"
#include "respuesta_vnifv2.h"
#include <QSqlQuery>

#define VERSION_SII "1.1"

tablaSII::tablaSII(QString qusuario,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tablaSII)
{
    ui->setupUi(this);
    ui->insegurocheckBox->hide();
    QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
    QStringList ej;
    if ( query.isActive() ) {
            while ( query.next() )
                  ej << query.value(0).toString();
            }
    ui->ejerciciocomboBox->addItems(ej);
    actufechas();

    usuario=qusuario;

    comadecimal=haycomadecimal();
    decimales=haydecimales();

    // escondemos columna 17 (iva de caja)
    ui->tableWidget->hideColumn(17);

    ui->tableWidget->hideColumn(11);
    ui->tableWidget->hideColumn(12);

    ui->tableWidget->hideColumn(19);

    recibidas=true; // por defecto

    ui->RE_lineEdit->hide();
    ui->label_9->hide();

    pruebas=false;

    modificacion=false;

    xml_gen_aislado=true;

    if (!privilegios[edi_asiento]) ui->edasientopushButton->setEnabled(false);

    curl_sii=basedatos::instancia()->select_curl_sii();

    connect(ui->ejerciciocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));
    connect(ui->periodocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));
    connect(ui->refrescar_pushButton,SIGNAL(clicked(bool)),SLOT(refrescar()));

    connect(ui->marca_enviada_pushButton,SIGNAL(clicked(bool)),SLOT(marca_enviado()));
    connect(ui->desmarca_enviada_pushButton,SIGNAL(clicked(bool)),SLOT(desmarca_enviado()));

    connect(ui->aceptar_pushButton,SIGNAL(clicked(bool)),SLOT(aceptar()));

    connect(ui->cancelar_pushButton,SIGNAL(clicked(bool)),SLOT(cancelar()));

    connect(ui->consasientopushButton,SIGNAL(clicked(bool)),SLOT(cons_asiento()));

    connect(ui->edasientopushButton,SIGNAL(clicked(bool)),SLOT(edit_asiento()));

    connect(ui->borra_asientoButton,SIGNAL(clicked(bool)),SLOT(borra_asiento()));

    connect(ui->noenvio_pushButton,SIGNAL(clicked(bool)),SLOT(marca_noenvio()));

    connect(ui->xml_pushButton,SIGNAL(clicked(bool)),SLOT(genera_fich_xml()));

    connect(ui->envioPruebas_pushButton,SIGNAL(clicked(bool)),SLOT(envio_pruebas()));

    connect(ui->envioReal_pushButton,SIGNAL(clicked(bool)),SLOT(envio_real()));

    connect(ui->rectif_pruebas_pushButton,SIGNAL(clicked(bool)),SLOT(envio_pruebas_modif()));

    connect(ui->rectif_real_pushButton,SIGNAL(clicked(bool)),SLOT(envio_real_modif()));

    connect(ui->copiar_pushButton,SIGNAL(clicked(bool)),SLOT(copiar()));

    // connect(ui->anul_pushButton,SIGNAL(clicked(bool)),SLOT(envio_real_anul()));

}

tablaSII::~tablaSII()
{
    delete ui;
}


void tablaSII::config_emitidas()
{
    recibidas=false;
    setWindowTitle(tr("SII Libro de facturas emitidas"));
    ui->tableWidget->hideColumn(14);
    ui->tableWidget->hideColumn(15);
    ui->tableWidget->hideColumn(16);
    ui->tableWidget->showColumn(11);
    ui->tableWidget->showColumn(12);

    ui->RE_lineEdit->show();
    ui->label_9->show();

    ui->label_8->hide();
    ui->cuota_efectiva_lineEdit->hide();

}

void tablaSII::actufechas()
{
  // ui->inicialdateEdit->setDate(inicioejercicio(ui->ejerciciocomboBox->currentText()));
  // ui->finaldateEdit->setDate(finejercicio(ui->ejerciciocomboBox->currentText()));


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

   ui->inicialdateEdit->setDate(fechaini);
   ui->finaldateEdit->setDate(fechafin);
   ui->tableWidget->clearContents();
   ui->tableWidget->setRowCount(0);

}

void tablaSII::refrescar()
{
    if (recibidas) carga_recibidas();
      else carga_emitidas();
}

void tablaSII::carga_recibidas()
{
    int registros=0;

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    bool porseries=!ui->doc_ini_lineEdit->text().isEmpty();

    registros = basedatos::instancia()->num_registros_recibidas("",true /* fechacontable*/,
                                                                ui->inicialdateEdit->date(), ui->finaldateEdit->date(),
                                                                true /*interiores*/, true /*aib*/, true /*autofactura*/,
                                                                true /*autofacturanoue*/,
                                                                true /*rectificativa*/, porseries,
                                                                ui->doc_ini_lineEdit->text() /*serie_inicial*/, ui->doc_fin_lineEdit->text(),
                                                                false /*filtrarbinversion*/, false /*solobi*/, false /*sinbi*/,
                                                                true /*filtrarnosujetas*/, false /*solonosujetas*/,
                                                                true /*sinnosujetas*/, true /*agrario*/, true /*isp_op_interiores*/,
                                                                true /*importaciones*/, false /*solo_caja*/, "" /*cuenta_factura*/,"");

    QSqlQuery query=basedatos::instancia()->registros_recibidas_prorrata("",true /* fechacontable*/,
                                                         ui->inicialdateEdit->date(), ui->finaldateEdit->date(),
                                                         true /*interiores*/, true /*aib*/, true /*autofactura*/,
                                                         true /*autofacturanoue*/,
                                                         true /*rectificativa*/, porseries /*porseries*/,
                                                         ui->doc_ini_lineEdit->text() /*serie_inicial*/, ui->doc_fin_lineEdit->text() /*serie_final*/,
                                                         false /*filtrarbinversion*/, false /*solobi*/, false /*sinbi*/,
                                                         true /*filtrarnosujetas*/, false /*solonosujetas*/,
                                                         true /*sinnosujetas*/, true /*agrario*/, true /*isp_op_interiores*/,
                                                         true /*importaciones*/, false /*solo_caja*/, "" /*cuenta_factura*/,"");


    QProgressDialog progreso("Cargando registros ...",  0, 0, 100);
    progreso.setMaximum(registros);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(tr("PROCESANDO..."));
    // progreso.setValue(1);

    QApplication::processEvents();

    int numorden=0;

    double totalbase=0, totalcuota=0, totaltotal=0, totalcuotaefectiva=0;

    if ( query.isActive() ) {
       while ( query.next() )
         {
          progreso.setValue(numorden);
          QApplication::processEvents();
          if (ui->tableWidget->rowCount()-1<numorden) ui->tableWidget->insertRow(numorden);
          // cuenta_iva << query.value(22).toString();
          //QString cadnum; cadnum.setNum(numorden+1);
          /*
          if (query.value(21).toBool())
            {
             // criterio de caja
             // ui.variablestable->setVerticalHeaderLabels(lfilas);
             cadnum+="*";
            }
          */
          QTableWidgetItem *newItem1 = new QTableWidgetItem(query.value(17).toString());
          newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,0,newItem1);
          //apunte << query.value(17).toString();
          //hash[query.value(17).toString()] = numorden;


          QDate fechacontab=query.value(1).toDate();
          QString cadfecha=fechacontab.toString("dd-MM-yyyy");
          ui->tableWidget->setItem(numorden,1,new QTableWidgetItem(cadfecha));
          // fecha_cont << fechacontab.toString("yyyy-MM-dd");

          QDate fechafra=query.value(11).toDate();
          QString cadfechafra=fechafra.toString("dd-MM-yyyy");
          ui->tableWidget->setItem(numorden,2,new QTableWidgetItem(cadfechafra));
          // fecha_fra << fechafra.toString("yyyy-MM-dd");

          // Documento
          ui->tableWidget->setItem(numorden,3,new QTableWidgetItem(query.value(0).toString()));
          //documento << query.value(0).toString();

          // Cuenta
          ui->tableWidget->setItem(numorden,4,new QTableWidgetItem(query.value(2).toString()));
          //cuenta << query.value(2).toString();

          QString externo=query.value(23).toString();
          bool hayexterno=!externo.isEmpty();

          QString nombre;

          if (!hayexterno)
            {
              //void basedatos::razon_nif_datos(QString cuenta, QString *razon, QString *nif)
              QString razon,nif;
              basedatos::instancia()->razon_nif_datos(query.value(2).toString(),&razon,&nif);
              if (razon.isEmpty()) {
                  if (query.value(15).toString().isEmpty())
                    nombre=filtracad(query.value(3).toString()); // ojo si tiene contenido nombre libroiva
                   else nombre=filtracad(query.value(15).toString());
              }
               else nombre=razon;
             //nombre= (query.value(15).toString().isEmpty() ?
             //       filtracad(query.value(3).toString()) :
             //       filtracad(query.value(15).toString()))
             //     + " "; // nombre
            }
             else
              nombre=basedatos::instancia()->razon_externo(externo);

          ui->tableWidget->setItem(numorden,5,new QTableWidgetItem(nombre));
          //descripcion << nombre;

          QString cif;
          if (!hayexterno)
            cif = (query.value(16).toString().isEmpty() ?
                cifcta(query.value(2).toString()) :
                filtracad(query.value(16).toString())); // cif
           else
              cif=basedatos::instancia()->cif_externo(externo);

          ui->tableWidget->setItem(numorden,6,new QTableWidgetItem(cif));
          //nif << cif;


          QTableWidgetItem *newItem11 = new QTableWidgetItem(query.value(4).toString());
          newItem11->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,7,newItem11); // asiento
          //asiento << query.value(4).toString();

          // base imponible
           QTableWidgetItem *newItem21 = new QTableWidgetItem(
                   formateanumerosep(query.value(5).toDouble(),comadecimal,decimales));
           newItem21->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(numorden,8,newItem21);
           totalbase+=query.value(5).toDouble();
           //bi << formateanumerosep(query.value(5).toDouble(),comadecimal,decimales);

           // tipo
           QTableWidgetItem *newItem31 = new QTableWidgetItem(
                   formateanumerosep(query.value(6).toDouble(),comadecimal,decimales));
           newItem31->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(numorden,9,newItem31);
           //tipo << formateanumerosep(query.value(6).toDouble(),comadecimal,decimales);

          // cuota
          QTableWidgetItem *newItem41 = new QTableWidgetItem(
               formateanumerosep(query.value(7).toDouble(),comadecimal,decimales));
          newItem41->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,10,newItem41);
          //cuota << formateanumerosep(query.value(7).toDouble(),comadecimal,decimales);
          totalcuota+=query.value(7).toDouble();

          // total
          QTableWidgetItem *newItem71 = new QTableWidgetItem(
               formateanumerosep(query.value(8).toDouble(),comadecimal,decimales));
          newItem71->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,13,newItem71);
          //total << formateanumerosep(query.value(8).toDouble(),comadecimal,decimales);
          totaltotal+=query.value(8).toDouble();

          // prorrata
          QTableWidgetItem *newItem81 = new QTableWidgetItem(
                  formateanumerosep(query.value(9).toDouble()*100,comadecimal,decimales));
          newItem81->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,14,newItem81);
          //prorrata << formateanumerosep(query.value(9).toDouble()*100,comadecimal,decimales);

          // afectacion
          QTableWidgetItem *newItem91 = new QTableWidgetItem(
                  formateanumerosep(query.value(15).toDouble()*100,comadecimal,decimales));
          newItem91->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,15,newItem91);
          // afectacion << formateanumerosep(query.value(14).toDouble()*100,comadecimal,decimales);

          // cuota efectiva
          QTableWidgetItem *newItem911 = new QTableWidgetItem(
                  formateanumerosep(query.value(10).toDouble(),comadecimal,decimales));
          newItem911->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,16,newItem911);
          // cuota_efectiva << formateanumerosep(query.value(10).toDouble(),comadecimal,decimales);
          totalcuotaefectiva+=query.value(10).toDouble();

          // cuota liquidada
          QTableWidgetItem *newItem912 = new QTableWidgetItem("");
          newItem912->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,17,newItem912);

          // aquí iría enviado sii
          QString cad_enviado_sii=query.value(24).toBool() ? "X" : "";
          QTableWidgetItem *newItem913 = new QTableWidgetItem(cad_enviado_sii);
          newItem913->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,18,newItem913);
          QTableWidgetItem *newItem914 = new QTableWidgetItem(cad_enviado_sii);
          newItem914->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,19,newItem914);
          // recepcion << query.value(18).toString();

          ui->tableWidget->setItem(numorden,20,new QTableWidgetItem(""));

          QTableWidgetItem *newItem915 = new QTableWidgetItem(query.value(25).toString());
          ui->tableWidget->setItem(numorden,21,newItem915);


          numorden++;
          QApplication::processEvents();
         }
        }

    // actualizamostotales
    ui->base_lineEdit->setText(formatea_redondeado_sep(totalbase,comadecimal, decimales));
    ui->cuota_lineEdit->setText(formatea_redondeado_sep(totalcuota,comadecimal, decimales));
    ui->total_lineEdit->setText(formatea_redondeado_sep(totaltotal,comadecimal, decimales));
    ui->cuota_efectiva_lineEdit->setText(formatea_redondeado_sep(totalcuotaefectiva,comadecimal, decimales));
    progreso.close();
}


void tablaSII::carga_emitidas()
{
    int registros=0;

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    bool porseries=!ui->doc_ini_lineEdit->text().isEmpty();

    registros= basedatos::instancia()->num_registros_emitidas("",
                                      true /* fechacontable*/,ui->inicialdateEdit->date(),
                                      ui->finaldateEdit->date(),true /*interiores*/, false /*aib*/, true /*eib*/,
                                      false /*autofactura*/,
                                      true /*rectificativa*/, false /*autofacturanoue*/, true /*prestserviciosue*/,
                                      porseries /*porseries*/, ui->doc_ini_lineEdit->text() /*serie_inicial*/, ui->doc_fin_lineEdit->text() /*serie_final*/, false /*filtrarnosujetas*/,
                                      false /*solonosujetas*/, true /*sinnosujetas*/, true /*exportacion*/, true /*isp_interior*/,
                                      false /*exenta_no_deduc*/, true /*ventas_fuera_tac*/,"" /*cuentap*/, "");


    QProgressDialog progreso("Cargando registros ...",  0, 0, 100);
    progreso.setMaximum(registros);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(tr("PROCESANDO..."));
    // progreso.setValue(1);
    QApplication::processEvents();

    double totalbase=0, totalcuota=0, totaltotal=0, totalre=0;

    QSqlQuery query;
      query = basedatos::instancia()->registros_emitidas("",
                                                         true /* fechacontable*/,ui->inicialdateEdit->date(),
                                                         ui->finaldateEdit->date(),true /*interiores*/, false /*aib*/, true /*eib*/,
                                                         false /*autofactura*/,
                                                         true /*rectificativa*/, false /*autofacturanoue*/, true /*prestserviciosue*/,
                                                         porseries /*porseries*/, ui->doc_ini_lineEdit->text() /*serie_inicial*/, ui->doc_fin_lineEdit->text() /*serie_final*/, false /*filtrarnosujetas*/,
                                                         false /*solonosujetas*/, true /*sinnosujetas*/, true /*exportacion*/, true /*isp_interior*/,
                                                         true /*exenta_deduc*/, true /*ventas_fuera_tac*/,"" /*cuentap*/, "",false /*orden fecha fac*/,
                                                         false /*ordendocumento*/);
    //ui.progressBar->setMaximum(registros);
    // QStringList lfilas;
    int numorden=0;
    if ( query.isActive() ) {
       while ( query.next() )
         {
          //ui.progressBar->setValue(numprog);
           progreso.setValue(numorden);
           QApplication::processEvents();

           // hay que discriminar isp_interior no exentas (tipo o cuota>0)
           if (query.value(6).toDouble()>0.0001 && query.value(19).toBool()) continue;

           QString externo= query.value(17).toString();
           bool hayexterno=!externo.isEmpty();


          if (ui->tableWidget->rowCount()-1<numorden) ui->tableWidget->insertRow(numorden);

          // cuenta_iva << query.value(17).toString();

          QString cadnum; cadnum.setNum(numorden+1);
          /*if (query.value(16).toBool())
            {
             // criterio de caja
             // ui.variablestable->setVerticalHeaderLabels(lfilas);
             cadnum+="*";
            }*/

          // lfilas << cadnum;

          QTableWidgetItem *newItem1 = new QTableWidgetItem(query.value(15).toString());
          newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,0,newItem1);
          //apunte << query.value(15).toString();
          //hash[query.value(15).toString()] = numorden;

          QDate fechacontab=query.value(1).toDate();
          QString cadfecha=fechacontab.toString("dd-MM-yyyy");
          ui->tableWidget->setItem(numorden,1,new QTableWidgetItem(cadfecha));
          //fecha_cont << fechacontab.toString("yyyy-MM-dd");

          QDate fechafra=query.value(11).toDate();
          QString cadfechafra=fechafra.toString("dd-MM-yyyy");
          ui->tableWidget->setItem(numorden,2,new QTableWidgetItem(cadfechafra));
          // fecha_fra << fechafra.toString("yyyy-MM-dd");

          if (!query.value(0).toString().isEmpty())
             ui->tableWidget->setItem(numorden,3,new QTableWidgetItem(query.value(0).toString()));
           else
             {
              if (!query.value(22).toString().isEmpty())
                 ui->tableWidget->setItem(numorden,3,new QTableWidgetItem(query.value(22).toString())); //+ " - "+query.value(23).toString()));
             }
          // documento << query.value(0).toString();

          ui->tableWidget->setItem(numorden,4,new QTableWidgetItem(query.value(2).toString()));
          // cuenta << query.value(2).toString();

          QString nombre;

          if (!hayexterno)
            {
              QString razon,nif;
              if (!query.value(13).toString().isEmpty()) razon=query.value(13).toString();
                 else
                   basedatos::instancia()->razon_nif_datos(query.value(2).toString(),&razon,&nif);
              if (razon.isEmpty()) nombre=filtracad(query.value(3).toString());
                 else nombre=razon;
             //nombre = (query.value(13).toString().isEmpty() ?
             //       filtracad(query.value(3).toString()) :
             //       filtracad(query.value(13).toString()))
             //     + " "; // nombre
            }
            else
              nombre=basedatos::instancia()->razon_externo(externo);

          ui->tableWidget->setItem(numorden,5,new QTableWidgetItem(nombre));
          //descripcion << nombre;

          QString cif;
          if (!hayexterno)
             cif = (query.value(14).toString().isEmpty() ?
                    cifcta(query.value(2).toString()) :
                    filtracad(query.value(14).toString())); // cif
            else
             cif=basedatos::instancia()->cif_externo(externo);

          ui->tableWidget->setItem(numorden,6,new QTableWidgetItem(cif));
          // nif << cif;

          QTableWidgetItem *newItem11 = new QTableWidgetItem(query.value(4).toString());
          newItem11->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,7,newItem11); // asiento
          // asiento << query.value(4).toString();

          // base imponible
           QTableWidgetItem *newItem21 = new QTableWidgetItem(
                   formateanumerosep(query.value(5).toDouble(),comadecimal,decimales));
           newItem21->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(numorden,8,newItem21);
           //bi << formateanumerosep(query.value(5).toDouble(),comadecimal,decimales);
           totalbase+=query.value(5).toDouble();

           // tipo
           QTableWidgetItem *newItem31 = new QTableWidgetItem(
                   formateanumerosep(query.value(6).toDouble(),comadecimal,decimales));
           newItem31->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(numorden,9,newItem31);
           //tipo << formateanumerosep(query.value(6).toDouble(),comadecimal,decimales);

          // cuota
          double valcuota=CutAndRoundNumberToNDecimals (query.value(7).toDouble(),2);
          QTableWidgetItem *newItem41 = new QTableWidgetItem(
               formateanumerosep(valcuota,comadecimal,decimales));
          newItem41->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,10,newItem41);
          //cuota << formateanumerosep(valcuota,comadecimal,decimales);
          totalcuota+=valcuota;

          /*// cuota liq
          QTableWidgetItem *newItem42 = new QTableWidgetItem("");
          newItem42->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,11,newItem42);*/

          // tipo re
          QTableWidgetItem *newItem51 = new QTableWidgetItem(
                  formateanumerosep(query.value(8).toDouble(),comadecimal,decimales));
          newItem51->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,11,newItem51);
          //tipore << formateanumerosep(query.value(8).toDouble(),comadecimal,decimales);

          // re
          double valre=CutAndRoundNumberToNDecimals (query.value(5).toDouble()*
                                                     query.value(8).toDouble()/100,2);
          QTableWidgetItem *newItem61 = new QTableWidgetItem(
               formateanumerosep(valre,comadecimal,decimales));
          newItem61->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,12,newItem61);
          //re << formateanumerosep(valre,comadecimal,decimales);
          totalre+=valre;

          // total
          QTableWidgetItem *newItem71 = new QTableWidgetItem(
               formateanumerosep(query.value(10).toDouble(),comadecimal,decimales));
          newItem71->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,13,newItem71);
          //total << formateanumerosep(query.value(10).toDouble(),comadecimal,decimales);
          totaltotal+=query.value(10).toDouble();

          // aquí iría enviado sii
          QString cad_enviado_sii=query.value(18).toBool() ? "X" : "";
          QTableWidgetItem *newItem913 = new QTableWidgetItem(cad_enviado_sii);
          newItem913->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,18,newItem913);
          QTableWidgetItem *newItem914 = new QTableWidgetItem(cad_enviado_sii);
          newItem914->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,19,newItem914);

          ui->tableWidget->setItem(numorden,20,new QTableWidgetItem(""));

          QTableWidgetItem *newItem915 = new QTableWidgetItem(query.value(24).toString());
          ui->tableWidget->setItem(numorden,21,newItem915);

          numorden++;
         }
        }
    ui->base_lineEdit->setText(formatea_redondeado_sep(totalbase,comadecimal, decimales));
    ui->cuota_lineEdit->setText(formatea_redondeado_sep(totalcuota,comadecimal, decimales));
    ui->total_lineEdit->setText(formatea_redondeado_sep(totaltotal,comadecimal, decimales));
    ui->RE_lineEdit->setText(formatea_redondeado_sep(totalre,comadecimal, decimales));

}


void tablaSII::marca_enviado()
{
    if (ui->tableWidget->currentItem()==0) return;
    ui->tableWidget->item(ui->tableWidget->currentRow(),18)->setText("X");

}

void tablaSII::desmarca_enviado()
{
    if (ui->tableWidget->currentItem()==0) return;
    ui->tableWidget->item(ui->tableWidget->currentRow(),18)->setText("");

}

void tablaSII::marca_noenvio()
{
    if (ui->tableWidget->currentItem()==0) return;
    if (ui->tableWidget->item(ui->tableWidget->currentRow(),20)->text().isEmpty())
       ui->tableWidget->item(ui->tableWidget->currentRow(),20)->setText("X");
     else
        ui->tableWidget->item(ui->tableWidget->currentRow(),20)->setText("");

}

void tablaSII::actualiza_libro_iva()
{
    QString contenido,contenidobd;
    int numfilas=ui->tableWidget->rowCount();
    for (int fila=0;fila<numfilas;fila++)
       {
        if (ui->tableWidget->item(fila,0)==NULL) continue;
        contenido=ui->tableWidget->item(fila,18)->text();
        contenidobd=ui->tableWidget->item(fila,19)->text();
        if (contenido!=contenidobd)
          {
           // actualizar pase, enviado_sii
           QString cadnum;
           cadnum=ui->tableWidget->item(fila,0)->text();
           int apunte=cadnum.toInt();
           bool enviado=(contenido=="X");
           basedatos::instancia()->actu_enviado_sii(apunte,
                                                    enviado);
          }

       }
}

void tablaSII::aceptar()
{
    actualiza_libro_iva();
    accept();
}

void tablaSII::cancelar()
{
    QString contenido,contenidobd;
    int numfilas=ui->tableWidget->rowCount();
    for (int fila=0;fila<numfilas;fila++)
       {
        if (ui->tableWidget->item(fila,0)==NULL) continue;
        contenido=ui->tableWidget->item(fila,18)->text();
        contenidobd=ui->tableWidget->item(fila,19)->text();
        if (contenido!=contenidobd)
          {
            if (QMessageBox::question(
                   this,
                   tr("Tabla SII"),
                   tr("Hay cambios sin actualizar ¿ Desea salir ?"))==QMessageBox::Yes )
                                {
                                 reject();
                                 break;
                                }
                else return;

          }

       }
   reject();
}


void tablaSII::cons_asiento()
{
    // necesitamos conocer número de asiento y ejercicio

       if (ui->tableWidget->currentItem()==0) return;
       QString pase=ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
       QString asiento=basedatos::instancia()->asientodepase(pase);
       QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
       QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

       QString qdiario;
       qdiario=diariodeasiento(asiento,ejercicio);

    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Consultas de asientos"),
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


void tablaSII::edit_asiento()
{
 // necesitamos conocer número de asiento y ejercicio



    if (ui->tableWidget->currentItem()==0) return;
    QString pase=ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
    QString asiento=basedatos::instancia()->asientodepase(pase);
    QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
    QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

    if (!ui->tableWidget->item(ui->tableWidget->currentRow(),19)->text().isEmpty())
       QMessageBox::warning( this, tr("Tabla SII"),
                          tr("ATENCIÓN: La factura asociada al asiento quedará como no enviada tras la edición"));


    QString qdiario;
    qdiario=diariodeasiento(asiento,ejercicio);

    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Tabla SII"),
                               tr("ERROR: No se pueden editar asientos del diario de apertura, "
                                  "regularización o cierre\n"));
         return;
       }

    if (!existeasiento(asiento,ejercicio))
       {
         QMessageBox::warning( this, tr("Tabla SII"),
                               tr("ERROR: El asiento seleccionado ya no existe"));
         return;
       }

 editarasiento(asiento,usuario,ejercicio);
 refrescar();
}


void tablaSII::borra_asiento()
{
    if (ui->tableWidget->currentItem()==0) return;
    QString pase=ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
    QString asiento=basedatos::instancia()->asientodepase(pase);
    QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
    QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

    borrarasientofunc(asiento,ejercicio,true);
    refrescar();

}


void tablaSII::genera_fich_xml()
{

    QString nombre;


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
    filtros << tr("Archivos xml (*.xml)");
    dialogofich.setNameFilters(filtros);
    dialogofich.setDirectory(adapta(dirtrabajobd()));
    dialogofich.setWindowTitle(tr("FICHERO SII"));
    QStringList fileNames;
    if (dialogofich.exec())
       {
        fileNames = dialogofich.selectedFiles();
        if (fileNames.at(0).length()>0)
            {
             // QString nombre=nombre.fromLocal8bit(fileNames.at(0));
             nombre=fileNames.at(0);
            }
       }


    if (nombre.isEmpty()) return;

    if (nombre.right(4)!=".xml") nombre=nombre+".xml";
     QFile estado( adapta(nombre)  );
     if (estado.exists() && QMessageBox::question(this,
         tr("¿ Sobreescribir ? -- Exportar asiento automático"),
         tr("'%1' ya existe."
            "¿ Desea sobreescribirlo ?")
           .arg( nombre )) == QMessageBox::No )
       return ;

     if (recibidas)
        {
         if (fich_sii_recibidas(nombre))
           {
            QMessageBox::information( this, tr("GENERAR ARCHIVO SII"),
                      tr("El archivo se ha generado correctamente"));
           }
          else
           QMessageBox::warning( this, tr("GENERAR ARCHIVO SII"),
                      tr("ERROR al generar el archivo"));
       }
       else
           {
            if (fich_sii_emitidas(nombre))
              {
               QMessageBox::information( this, tr("GENERAR ARCHIVO SII"),
                   tr("El archivo se ha generado correctamente"));
              }
              else
                 QMessageBox::warning( this, tr("GENERAR ARCHIVO SII"),
                   tr("ERROR al generar el archivo"));
           }
}


bool tablaSII::fich_sii_recibidas(QString nombrefich)
{
    QDomDocument doc("ENVIO");
    QDomElement root = doc.createElement("soapenv:Body");
    doc.appendChild(root);

    QDomElement tag = doc.createElement("siiLR:SuministroLRFacturasRecibidas");
    root.appendChild(tag);

    QDomElement tag1 = doc.createElement("sii:Cabecera");
    tag.appendChild(tag1);

    addElementoTextoDom(doc,tag1,"sii:IDVersionSii",VERSION_SII);

    QDomElement tag11 = doc.createElement("sii:Titular");
    tag1.appendChild(tag11);

    addElementoTextoDom(doc,tag11,"sii:NombreRazon",nombreempresa());
    addElementoTextoDom(doc,tag11,"sii:NIF",basedatos::instancia()->cif());

    QString tipcom="A0";
    if (modificacion) tipcom="A1";
    addElementoTextoDom(doc,tag1,"sii:TipoComunicacion",tipcom);
    // A0 Alta
    // A1 Modificación/Corrección de errores suministrados
    // A4 Modificación factura régimen de viajeros

    // Ya tenemos la cabecera  LISTA

    // aquí un blucle
    for (int fila=0; fila<ui->tableWidget->rowCount(); fila++)
        {
        if (!ui->tableWidget->item(fila,20)->text().isEmpty()) continue;
        if (!modificacion) {
           if (!ui->tableWidget->item(fila,18)->text().isEmpty()) continue;
        }
        else {
            if (ui->tableWidget->item(fila,18)->text().isEmpty()) continue;
        }
         bool importacion=false;
         QDate fecha_operacion;
         bool agrario=false;
         bool aib_ais=false;
         bool autofactura_no_ue=false;
         bool isp_op_interiores=false;
         bool oro_inversion=false, arrto_local_ret=false, arrto_local_sin_ret=false;
         QString dua;
         QString tipo_rectificativa;
         bool rectificativa=false;
         fecha_operacion=fecha_operacion.fromString(ui->tableWidget->item(fila,21)->text(),"dd-MM-yyyy");
         QSqlQuery q=basedatos::instancia()->campos_libroiva(ui->tableWidget->item(fila,0)->text());
         QString cta_operacion;
         if (q.isActive())
             if (q.next())
                {
                 importacion=q.value(0).toBool();
                 fecha_operacion=q.value(1).toDate();
                 agrario=q.value(2).toBool();
                 aib_ais=q.value(3).toBool() || q.value(4).toBool();
                 autofactura_no_ue=q.value(5).toBool();
                 isp_op_interiores=q.value(6).toBool();
                 cta_operacion=q.value(7).toString();
                 oro_inversion=q.value(13).toBool();
                 arrto_local_ret=q.value(14).toBool();
                 arrto_local_sin_ret=q.value(15).toBool();
                 dua=q.value(16).toString();
                 rectificativa=q.value(21).toBool();
                 tipo_rectificativa=q.value(22).toString().left(2);
                }
         QDomElement tag2 = doc.createElement("siiLR:RegistroLRFacturasRecibidas");
         tag.appendChild(tag2);

         QDomElement tag21;
         tag21 = doc.createElement("sii:PeriodoLiquidacion");
         tag2.appendChild(tag21);

         QString cadyear; cadyear.setNum(ui->inicialdateEdit->date().year());
         cadyear=cadyear.trimmed();
         addElementoTextoDom(doc,tag21,"sii:Ejercicio",cadyear);
         addElementoTextoDom(doc,tag21,"sii:Periodo",ui->periodocomboBox->currentText().left(2));

         QDomElement tag22 = doc.createElement("siiLR:IDFactura");
         tag2.appendChild(tag22);

         QDomElement tag221 = doc.createElement("sii:IDEmisorFactura");
         tag22.appendChild(tag221);
         // contemplamos aquí IDOtro
         bool ue=false;
         QString pais;


         QString externo=basedatos::instancia()->externo_pase(ui->tableWidget->item(fila,0)->text());
         if (externo.isEmpty())
            {
             //select7Datossubcuentacuenta (QString cuenta)
             QSqlQuery q=basedatos::instancia()->select7Datossubcuentacuenta(ui->tableWidget->item(fila,4)->text());
             if (q.isActive())
                 if (q.next())
                     pais=q.value(6).toString().left(2);
            }
             else
                  {
                    //select7datos_externo (QString externo)
                   QSqlQuery q=basedatos::instancia()->select7datos_externo(externo);
                   if (q.isActive())
                      if (q.next())
                        pais=q.value(6).toString().left(2);
                  }


         if ((!importacion && !aib_ais && ! autofactura_no_ue) || ((importacion && pais=="ES") || (importacion && !dua.isEmpty())))
            {
              if (importacion && !dua.isEmpty()) // TIPO F5
                  addElementoTextoDom(doc,tag221,"sii:NIF",basedatos::instancia()->cif());
                else
                  addElementoTextoDom(doc,tag221,"sii:NIF",ui->tableWidget->item(fila,6)->text());
            }
          else
              {
               QString nif_iva=ui->tableWidget->item(fila,6)->text();
               ue=false;
               QString paises="AT BE BG CY CZ DE DK EE EL ES FI FR GB HR HU IE IT LT LU LV MT NL PL PT RO SE SI SK GR";

               if ((nif_iva.length()>2) && (paises.contains(nif_iva.left(2))) && paises.contains(pais))
                  { pais=nif_iva.left(2); ue=true;
                    if (pais=="EL") pais="GR";}
                else
                    {
                     // se trata de un externo ?
                     // QString basedatos::externo_pase(QString pase)
                     QString externo=basedatos::instancia()->externo_pase(ui->tableWidget->item(fila,0)->text());
                     if (externo.isEmpty())
                        {
                         //select7Datossubcuentacuenta (QString cuenta)
                         QSqlQuery q=basedatos::instancia()->select7Datossubcuentacuenta(ui->tableWidget->item(fila,4)->text());
                         if (q.isActive())
                             if (q.next())
                                 pais=q.value(6).toString().left(2);
                        }
                         else
                              {
                                //select7datos_externo (QString externo)
                               QSqlQuery q=basedatos::instancia()->select7datos_externo(externo);
                               if (q.isActive())
                                  if (q.next())
                                    pais=q.value(6).toString().left(2);
                              }

                     }
               // añadimos etiqueta IDOtro
               QDomElement tag2211 = doc.createElement("sii:IDOtro");
               tag221.appendChild(tag2211);
               addElementoTextoDom(doc,tag2211,"sii:CodigoPais",pais);
               if (ue)
                 addElementoTextoDom(doc,tag2211,"sii:IDType","02");
                else
                   addElementoTextoDom(doc,tag2211,"sii:IDType","04"); // DOC OF. IDENTIF. PAIS RESIDENCIA
               if (!ui->tableWidget->item(fila,6)->text().isEmpty())
                 addElementoTextoDom(doc,tag2211,"sii:ID",ui->tableWidget->item(fila,6)->text());
                 else
                   addElementoTextoDom(doc,tag2211,"sii:ID",basedatos::instancia()->cif());
              }

         QString numserie;
         if (importacion && !dua.isEmpty()) numserie=dua;
           else numserie=ui->tableWidget->item(fila,3)->text();
         addElementoTextoDom(doc,tag22,"sii:NumSerieFacturaEmisor",numserie);
         addElementoTextoDom(doc,tag22,"sii:FechaExpedicionFacturaEmisor",ui->tableWidget->item(fila,2)->text());

         QDomElement tag23 = doc.createElement("siiLR:FacturaRecibida");
         tag2.appendChild(tag23);

         if (!rectificativa) addElementoTextoDom(doc,tag23,"sii:TipoFactura", importacion ? "F5" : "F1");
           else
               {
                addElementoTextoDom(doc,tag23,"sii:TipoFactura", tipo_rectificativa.left(2));
                addElementoTextoDom(doc,tag23,"sii:TipoRectificativa", "I"); // siempre por diferencias
               }

         addElementoTextoDom(doc,tag23,"sii:FechaOperacion",fecha_operacion.toString("dd-MM-yyyy"));
         QString clave_trasc="01";
         if (agrario) clave_trasc="02";
         if (aib_ais) clave_trasc="09";
         if (arrto_local_ret || arrto_local_sin_ret ) clave_trasc="12";
         if (oro_inversion) clave_trasc="04";
         if (importacion && dua.isEmpty()) clave_trasc="13";

         addElementoTextoDom(doc,tag23,"sii:ClaveRegimenEspecialOTrascendencia",clave_trasc);

         // AQUÍ IRÍA ImporteTotal
         // total factura -- recoger problemática varias líneas de IVA
         double total_factura=convapunto(ui->tableWidget->item(fila,13)->text().remove(".")).toDouble();
         int filapos=fila;
         filapos++;
         bool multireg=false;
         while (filapos<ui->tableWidget->rowCount() &&
                ui->tableWidget->item(fila,2)->text()==ui->tableWidget->item(filapos,2)->text()
                               && ui->tableWidget->item(fila,3)->text()==ui->tableWidget->item(filapos,3)->text()
                               && ui->tableWidget->item(fila,7)->text()==ui->tableWidget->item(filapos,7)->text())
           {
            // col2 es fecha fra.
            // col3 es documento
            // col7 es asiento
            total_factura+=convapunto(ui->tableWidget->item(filapos,13)->text().remove(".")).toDouble();
            multireg=true;
            filapos++;
           }

          // ahora miramos por arriba

         filapos=fila;
         filapos--;
         while (filapos>0 &&
                ui->tableWidget->item(fila,2)->text()==ui->tableWidget->item(filapos,2)->text()
                               && ui->tableWidget->item(fila,3)->text()==ui->tableWidget->item(filapos,3)->text()
                               && ui->tableWidget->item(fila,7)->text()==ui->tableWidget->item(filapos,7)->text())
           {
            // col2 es fecha fra.
            // col3 es documento
            // col7 es asiento
            total_factura+=convapunto(ui->tableWidget->item(filapos,13)->text().remove(".")).toDouble();
            filapos--;
           }

         QString cadtotal;
         total_factura=CutAndRoundNumberToNDecimals (total_factura, 2);
         cadtotal.setNum(total_factura,'f',2);
         addElementoTextoDom(doc,tag23,"sii:ImporteTotal",cadtotal);

         // Fin importe total

         QString descripcion;
         QString concepto_sii=basedatos::instancia()->concepto_sii_pase(ui->tableWidget->item(fila,0)->text());
         if (concepto_sii.isEmpty())
             descripcion=cta_operacion.isEmpty() ? tr("Objeto de la factura") : descripcioncuenta(cta_operacion);
           else descripcion=concepto_sii;
         addElementoTextoDom(doc,tag23,"sii:DescripcionOperacion",descripcion);

         double suma_deducible=0;
         QDomElement tag231 = doc.createElement("sii:DesgloseFactura");
         tag23.appendChild(tag231);
         // if (aib_ais || autofactura_no_ue || isp_op_interiores)
         if (autofactura_no_ue || isp_op_interiores)
           {
             QDomElement tag2311 = doc.createElement("sii:InversionSujetoPasivo");
             tag231.appendChild(tag2311);
             QDomElement tag23111 = doc.createElement("sii:DetalleIVA");
             tag2311.appendChild(tag23111);
             addElementoTextoDom(doc,tag23111,"sii:TipoImpositivo",convapunto(ui->tableWidget->item(fila,9)->text()));
             addElementoTextoDom(doc,tag23111,"sii:BaseImponible",convapunto(ui->tableWidget->item(fila,8)->text().remove(".")));
             addElementoTextoDom(doc,tag23111,"sii:CuotaSoportada",convapunto(ui->tableWidget->item(fila,10)->text().remove(".")));
             QString cadnum=convapunto(ui->tableWidget->item(fila,16)->text().remove("."));
             suma_deducible+=cadnum.toDouble();
           }
           else
               {
                QDomElement tag2311 = doc.createElement("sii:DesgloseIVA");
                tag231.appendChild(tag2311);

                QString asiento=ui->tableWidget->item(fila,7)->text();
                QString fecha_fra=ui->tableWidget->item(fila,2)->text();
                QString documento=ui->tableWidget->item(fila,3)->text();


                while(fila<ui->tableWidget->rowCount())
                  {
                   QDomElement tag23111 = doc.createElement("sii:DetalleIVA");
                   tag2311.appendChild(tag23111);
                   if (!agrario)
                     addElementoTextoDom(doc,tag23111,"sii:TipoImpositivo",convapunto(ui->tableWidget->item(fila,9)->text()));
                   addElementoTextoDom(doc,tag23111,"sii:BaseImponible",convapunto(ui->tableWidget->item(fila,8)->text().remove(".")));
                   if (!agrario)
                     addElementoTextoDom(doc,tag23111,"sii:CuotaSoportada",convapunto(ui->tableWidget->item(fila,10)->text().remove(".")));
                   if (agrario)
                     {
                      addElementoTextoDom(doc,tag23111,"sii:PorcentCompensacionREAGYP",convapunto(ui->tableWidget->item(fila,9)->text()));
                      addElementoTextoDom(doc,tag23111,"sii:ImporteCompensacionREAGYP",convapunto(ui->tableWidget->item(fila,10)->text().remove(".")));
                     }
                   QString cadnum=convapunto(ui->tableWidget->item(fila,16)->text().remove("."));
                   suma_deducible+=cadnum.toDouble();
                   // Esto solo cuando se emite el XML solo -- es decir sin botón de envío
                   if (!pruebas && xml_gen_aislado) ui->tableWidget->item(fila,18)->setText("X"); // ----------------
                   if (fila+1>=ui->tableWidget->rowCount()) break;
                   if (ui->tableWidget->item(fila+1,7)->text()==asiento &&
                      ui->tableWidget->item(fila+1,2)->text()==fecha_fra &&
                      ui->tableWidget->item(fila+1,3)->text()==documento) fila++;
                    else break;
                  }
               }
         // fin del DesgloseFactura
         // ahora toca contraparte
         QDomElement tag232 = doc.createElement("sii:Contraparte");
         tag23.appendChild(tag232);

         if (importacion && !dua.isEmpty())
            addElementoTextoDom(doc,tag232,"sii:NombreRazon",filtracadxml( nombreempresa()));
           else
            addElementoTextoDom(doc,tag232,"sii:NombreRazon",filtracadxml( ui->tableWidget->item(fila,5)->text()));

         if ((!importacion && !aib_ais && ! autofactura_no_ue) || ((importacion && pais=="ES") || (importacion && !dua.isEmpty())))
            {
              if (importacion && !dua.isEmpty()) // TIPO F5
                addElementoTextoDom(doc,tag232,"sii:NIF",basedatos::instancia()->cif());
               else
                 addElementoTextoDom(doc,tag232,"sii:NIF",ui->tableWidget->item(fila,6)->text());
            }
          else
              {
               // añadimos etiqueta IDOtro
               QDomElement tag2321 = doc.createElement("sii:IDOtro");
               tag232.appendChild(tag2321);
               addElementoTextoDom(doc,tag2321,"sii:CodigoPais",pais);
               if (ue)
                 addElementoTextoDom(doc,tag2321,"sii:IDType","02");
                else
                   addElementoTextoDom(doc,tag2321,"sii:IDType","04"); // DOC OF. IDENTIF. PAIS RESIDENCIA
               if (!ui->tableWidget->item(fila,6)->text().isEmpty())
                 addElementoTextoDom(doc,tag2321,"sii:ID",ui->tableWidget->item(fila,6)->text());
                 else
                   addElementoTextoDom(doc,tag2321,"sii:ID",basedatos::instancia()->cif());
              }

         // Fecha Registro Contable
         addElementoTextoDom(doc,tag23,"sii:FechaRegContable",ui->tableWidget->item(fila,1)->text());
         QString cadnum; cadnum.setNum(redond(suma_deducible,2),'f',2);
         addElementoTextoDom(doc,tag23,"sii:CuotaDeducible",cadnum);
         if (!pruebas && xml_gen_aislado) ui->tableWidget->item(fila,18)->setText("X");
        }

    QSqlQuery q=basedatos::instancia()->select_direc_SII();
    QString siiLR, sii;
    if (q.isActive())
        if (q.next()) {
           siiLR=q.value(4).toString();
           sii=q.value(5).toString();
        }
    QString cadini="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    cadini+="<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
    if (!siiLR.isEmpty() && !sii.isEmpty()) {
        cadini+="xmlns:siiLR=\"";
        cadini+=siiLR;
        cadini+="\" ";
        cadini+="xmlns:sii=\"";
        cadini+=sii;
        cadini+="\">\n";
    } else
        cadini+="xmlns:siiLR=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroLR.xsd\" "
                "xmlns:sii=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n";

    QString xml = doc.toString();
    xml.remove("<!DOCTYPE ENVIO>");
    xml.prepend(cadini);
    /* xml.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                "xmlns:siiLR=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroLR.xsd\" "
                "xmlns:sii=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n");*/

    xml.append("</soapenv:Envelope>\n");
    // -----------------------------------------------------------------------------------

    QFile fichero( adapta(nombrefich)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    // stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

    // bool esmysql = ( basedatos::instancia()->cualControlador() == basedatos::MYSQL );

    // ------------------------------------------------------------------------------------
    stream << xml;
    fichero.close();


    return true;
}



bool tablaSII::fich_sii_emitidas(QString nombrefich)
{

    QDomDocument doc("ENVIO");
    QDomElement root = doc.createElement("soapenv:Body");
    doc.appendChild(root);

    QDomElement tag = doc.createElement("siiLR:SuministroLRFacturasEmitidas");
    root.appendChild(tag);

    QDomElement tag1 = doc.createElement("sii:Cabecera");
    tag.appendChild(tag1);

    addElementoTextoDom(doc,tag1,"sii:IDVersionSii",VERSION_SII);

    QDomElement tag11 = doc.createElement("sii:Titular");
    tag1.appendChild(tag11);

    addElementoTextoDom(doc,tag11,"sii:NombreRazon",nombreempresa());
    addElementoTextoDom(doc,tag11,"sii:NIF",basedatos::instancia()->cif());

    QString tipcom="A0";
    if (modificacion) tipcom="A1";
    addElementoTextoDom(doc,tag1,"sii:TipoComunicacion",tipcom);
    // A0 Alta
    // A1 Modificación/Corrección de errores suministrados
    // A4 Modificación factura régimen de viajeros

    // Ya tenemos la cabecera  LISTA

    // aquí un blucle
    for (int fila=0; fila<ui->tableWidget->rowCount(); fila++)
        {
         if (!ui->tableWidget->item(fila,20)->text().isEmpty()) continue;
         if (!modificacion) {
           if (!ui->tableWidget->item(fila,18)->text().isEmpty()) continue;
         }
         else {
             if (ui->tableWidget->item(fila,18)->text().isEmpty()) continue;
         }
         //bool importacion=false;
         QDate fecha_operacion;
         //bool agrario=false;
         bool aib_ais=false;
         bool autofactura_no_ue=false;
         bool isp_op_interiores=false;
         bool eib, pr_servicios_ue, op_no_sujeta, exportacion, ventas_fuera_tac;
         bool oro_inversion=false, arrto_local_ret=false, arrto_local_sin_ret=false;
         QString dua;
         QString cta_operacion;
         QString clave_operacion, finicial, ffinal, tipo_rectificativa;
         bool rectificativa=false;
         int nfacturas=0;
         fecha_operacion=fecha_operacion.fromString(ui->tableWidget->item(fila,2)->text(),"dd-MM-yyy");
         QSqlQuery q=basedatos::instancia()->campos_libroiva(ui->tableWidget->item(fila,0)->text());
         if (q.isActive())
             if (q.next())
                {
                 //importacion=q.value(0).toBool();
                 fecha_operacion=q.value(1).toDate();
                 //agrario=q.value(2).toBool();
                 aib_ais=q.value(3).toBool() || q.value(4).toBool();
                 autofactura_no_ue=q.value(5).toBool();
                 isp_op_interiores=q.value(6).toBool();
                 cta_operacion=q.value(7).toString();
                 eib=q.value(8).toBool();
                 pr_servicios_ue=q.value(9).toBool();
                 op_no_sujeta=q.value(10).toBool();
                 exportacion=q.value(11).toBool();
                 ventas_fuera_tac=q.value(12).toBool();
                 if (ventas_fuera_tac) exportacion=true;
                 oro_inversion=q.value(13).toBool();
                 arrto_local_ret=q.value(14).toBool();
                 arrto_local_sin_ret=q.value(15).toBool();
                 dua=q.value(16).toString();
                 clave_operacion=q.value(17).toString();//.toString().left(1);
                 nfacturas=q.value(18).toInt();
                 finicial=q.value(19).toString();
                 ffinal=q.value(20).toString();
                 rectificativa=q.value(21).toBool();
                 tipo_rectificativa=q.value(22).toString().left(2);
                }

         if (aib_ais || autofactura_no_ue ) continue;
         //if (op_no_sujeta && !eib && !pr_servicios_ue && !exportacion && !ventas_fuera_tac) continue;
         QDomElement tag2 = doc.createElement("siiLR:RegistroLRFacturasEmitidas");
         tag.appendChild(tag2);

         QDomElement tag21;
         tag21 = doc.createElement("sii:PeriodoLiquidacion");
         tag2.appendChild(tag21);

         QString cadyear; cadyear.setNum(ui->inicialdateEdit->date().year());
         cadyear=cadyear.trimmed();
         addElementoTextoDom(doc,tag21,"sii:Ejercicio",cadyear);
         addElementoTextoDom(doc,tag21,"sii:Periodo",ui->periodocomboBox->currentText().left(2));

         QDomElement tag22 = doc.createElement("siiLR:IDFactura");
         tag2.appendChild(tag22);

         QDomElement tag221 = doc.createElement("sii:IDEmisorFactura");
         tag22.appendChild(tag221);

         addElementoTextoDom(doc,tag221,"sii:NIF",basedatos::instancia()->cif());

         if (finicial.isEmpty())
             addElementoTextoDom(doc,tag22,"sii:NumSerieFacturaEmisor",ui->tableWidget->item(fila,3)->text());
           else
                {
                 addElementoTextoDom(doc,tag22,"sii:NumSerieFacturaEmisor",finicial);
                 addElementoTextoDom(doc,tag22,"sii:NumSerieFacturaEmisorResumenFin",ffinal);
                }
         addElementoTextoDom(doc,tag22,"sii:FechaExpedicionFacturaEmisor",ui->tableWidget->item(fila,2)->text());

         QDomElement tag23 = doc.createElement("siiLR:FacturaExpedida");
         tag2.appendChild(tag23);
         // tipo factura
         //   F1 Factura normal
         //   F2 Factura simplificada
         if (clave_operacion=="J")
           {
            if (ffinal.isEmpty())
               addElementoTextoDom(doc,tag23,"sii:TipoFactura", "F2");
             else
                addElementoTextoDom(doc,tag23,"sii:TipoFactura", "F4");
           }
          else
              {
               if (!rectificativa) addElementoTextoDom(doc,tag23,"sii:TipoFactura", "F1");
                 else
                     {
                      addElementoTextoDom(doc,tag23,"sii:TipoFactura", tipo_rectificativa.left(2));
                      addElementoTextoDom(doc,tag23,"sii:TipoRectificativa", "I"); // siempre por diferencias
                     }
              }

         //addElementoTextoDom(doc,tag23,"sii:FechaOperacion",fecha_operacion.toString("dd-MM-yyyy"));
         QString clave_trasc="01";
         if (exportacion && !op_no_sujeta) clave_trasc="02";
         if (arrto_local_ret) clave_trasc="11";
         if (arrto_local_sin_ret) clave_trasc="12";
         // 02 Exportación;
         // 03 Bienes usados
         // 04 Oro de inversión
         // 11 arrendamiento con retención
         // 12 arrendamiento sin retención
         // 13 arrentamiento con y sin retención

         addElementoTextoDom(doc,tag23,"sii:ClaveRegimenEspecialOTrascendencia",clave_trasc);

         // total factura -- recoger problemática varias líneas de IVA
         double total_factura=convapunto(ui->tableWidget->item(fila,13)->text().remove(".")).toDouble();
         int filapos=fila;
         filapos++;
         bool multireg=false;
         while (filapos<ui->tableWidget->rowCount() &&
                ui->tableWidget->item(fila,2)->text()==ui->tableWidget->item(filapos,2)->text()
                               && ui->tableWidget->item(fila,3)->text()==ui->tableWidget->item(filapos,3)->text()
                               && ui->tableWidget->item(fila,7)->text()==ui->tableWidget->item(filapos,7)->text())
           {
            // col2 es fecha fra.
            // col3 es documento
            // col7 es asiento
            total_factura+=convapunto(ui->tableWidget->item(filapos,13)->text().remove(".")).toDouble();
            multireg=true;
            filapos++;
           }

          // ahora miramos por arriba

         filapos=fila;
         filapos--;
         while (filapos>0 &&
                ui->tableWidget->item(fila,2)->text()==ui->tableWidget->item(filapos,2)->text()
                               && ui->tableWidget->item(fila,3)->text()==ui->tableWidget->item(filapos,3)->text()
                               && ui->tableWidget->item(fila,7)->text()==ui->tableWidget->item(filapos,7)->text())
           {
            // col2 es fecha fra.
            // col3 es documento
            // col7 es asiento
            total_factura+=convapunto(ui->tableWidget->item(filapos,13)->text().remove(".")).toDouble();
            filapos--;
           }

         QString cadtotal;
         total_factura=CutAndRoundNumberToNDecimals (total_factura, 2);
         cadtotal.setNum(total_factura,'f',2);
         addElementoTextoDom(doc,tag23,"sii:ImporteTotal",cadtotal);

         // en descripción operación podríamos poner descripción cuenta ingreso contrapartida
         QString descripcion;
         QString concepto_sii=basedatos::instancia()->concepto_sii_pase(ui->tableWidget->item(fila,0)->text());
         if (concepto_sii.isEmpty())
           descripcion=(cta_operacion.isEmpty() || multireg) ? tr("Objeto de la factura") : descripcioncuenta(cta_operacion);
          else
             descripcion=concepto_sii;
         addElementoTextoDom(doc,tag23,"sii:DescripcionOperacion",descripcion);



         QString qpais;


         QString externo=basedatos::instancia()->externo_pase(ui->tableWidget->item(fila,0)->text());
         if (externo.isEmpty())
            {
             //select7Datossubcuentacuenta (QString cuenta)
             QSqlQuery q=basedatos::instancia()->select7Datossubcuentacuenta(ui->tableWidget->item(fila,4)->text());
             if (q.isActive())
                 if (q.next())
                     qpais=q.value(6).toString().left(2);
            }
             else
                  {
                    //select7datos_externo (QString externo)
                   QSqlQuery q=basedatos::instancia()->select7datos_externo(externo);
                   if (q.isActive())
                      if (q.next())
                        qpais=q.value(6).toString().left(2);
                  }


         // ahora toca contraparte
         if (clave_operacion!="J" || !ui->tableWidget->item(fila,6)->text().isEmpty()) {
         QDomElement tag232 = doc.createElement("sii:Contraparte");
         tag23.appendChild(tag232);
         addElementoTextoDom(doc,tag232,"sii:NombreRazon",filtracadxml( ui->tableWidget->item(fila,5)->text()));
         if (!pr_servicios_ue && !eib && !exportacion)
            addElementoTextoDom(doc,tag232,"sii:NIF",ui->tableWidget->item(fila,6)->text());
          else
              {
               QString pais;
               QString nif_iva=ui->tableWidget->item(fila,6)->text();
               bool ue=false;
               QString paises="AT BE BG CY CZ DE DK EE EL ES FI FR GB HR HU IE IT LT LU LV MT NL PL PT RO SE SI SK GR";
               if ((nif_iva.length()>2) && (paises.contains(nif_iva.left(2))) &&paises.contains(qpais) )
                   { pais=nif_iva.left(2);
                    if (pais=="EL") pais="GR"; ue=true; }
                  else {
                       pais=qpais;
                      }
               if (pais!="ES")
                 {
                  // añadimos etiqueta IDOtro
                  QDomElement tag2321 = doc.createElement("sii:IDOtro");
                  tag232.appendChild(tag2321);
                  addElementoTextoDom(doc,tag2321,"sii:CodigoPais",pais);
                  if (ue)
                    addElementoTextoDom(doc,tag2321,"sii:IDType","02");
                  else
                    addElementoTextoDom(doc,tag2321,"sii:IDType","04"); // DOC OF. IDENTIF. PAIS RESIDENCIA
                  if (!ui->tableWidget->item(fila,6)->text().isEmpty())
                    addElementoTextoDom(doc,tag2321,"sii:ID",ui->tableWidget->item(fila,6)->text());
                  else
                    addElementoTextoDom(doc,tag2321,"sii:ID",basedatos::instancia()->cif());
                 }
                 else addElementoTextoDom(doc,tag232,"sii:NIF",ui->tableWidget->item(fila,6)->text());

              }
         }

         QDomElement tag231 = doc.createElement("sii:TipoDesglose");
         tag23.appendChild(tag231);

         if (eib || pr_servicios_ue || exportacion || ventas_fuera_tac)
            {
             QDomElement tag2311 = doc.createElement("sii:DesgloseTipoOperacion");
             tag231.appendChild(tag2311);
             if (pr_servicios_ue || op_no_sujeta)
                {
                 QDomElement tag23111A = doc.createElement("sii:PrestacionServicios");
                 tag2311.appendChild(tag23111A);
                 // solo nos vamos a centrar en no sujetos (op fuera TAI)
                 QDomElement tag23111A1 = doc.createElement("sii:NoSujeta");
                 tag23111A.appendChild(tag23111A1);
                 addElementoTextoDom(doc,tag23111A1,"sii:ImporteTAIReglasLocalizacion",
                                     convapunto(ui->tableWidget->item(fila,8)->text().remove(".")));
                }
                else
                    {
                     QDomElement tag23111A = doc.createElement("sii:Entrega");
                     tag2311.appendChild(tag23111A);
                     QDomElement tag23111A1 = doc.createElement("sii:Sujeta");
                     tag23111A.appendChild(tag23111A1);

                     QDomElement tag23111A11 = doc.createElement("sii:Exenta");
                     tag23111A1.appendChild(tag23111A11);

                     QDomElement tag23111A11D = doc.createElement("sii:DetalleExenta");
                     tag23111A11.appendChild(tag23111A11D);

                     if (exportacion || ventas_fuera_tac)
                         addElementoTextoDom(doc,tag23111A11D,"sii:CausaExencion","E2");
                       else
                         addElementoTextoDom(doc,tag23111A11D,"sii:CausaExencion","E5");
                     addElementoTextoDom(doc,tag23111A11D,"sii:BaseImponible",
                                         convapunto(ui->tableWidget->item(fila,8)->text().remove(".")));
                    }

            } // fin desglose tipo operación
            else
                {
                 QDomElement tag2311 = doc.createElement("sii:DesgloseFactura");
                 tag231.appendChild(tag2311);

                 // --------------------------------------------------------------
                 //
                 QDomElement tag23111N;
                 bool con_no_sujeta=false;
                 if (op_no_sujeta)
                   {
                    // operaciones interiores en factura
                    // ojo esto no funciona -- saltamos el registro
                    con_no_sujeta=true;
                    tag23111N = doc.createElement("sii:NoSujeta");
                    // tag2311.appendChild(tag23111N);
                    addElementoTextoDom(doc,tag23111N,"sii:ImportePorArticulos7_14_Otros",
                                 convapunto(ui->tableWidget->item(fila,8)->text().remove(".")));
                    if (!multireg)
                       {
                         tag2311.appendChild(tag23111N);
                       }
                    //if (!pruebas) ui->tableWidget->item(fila,18)->setText("X");
                    if (fila+1<ui->tableWidget->rowCount())
                      {
                       // col2 es fecha fra.
                       // col3 es documento
                       // col7 es asiento
                       if (ui->tableWidget->item(fila,2)->text()==ui->tableWidget->item(fila+1,2)->text()
                          && ui->tableWidget->item(fila,3)->text()==ui->tableWidget->item(fila+1,3)->text()
                          && ui->tableWidget->item(fila,7)->text()==ui->tableWidget->item(fila+1,7)->text())
                       fila++;
                        else continue;
                      } else continue;
                   } // fin op_no_sujeta
                QDomElement tag23111 = doc.createElement("sii:Sujeta");
                tag2311.appendChild(tag23111);
                if (con_no_sujeta) tag2311.appendChild(tag23111N);

                // procesamos operacion exenta (tipo cero)
                //if (convapunto(ui->tableWidget->item(fila,9)->text()).toDouble()<0.0001)
                if (basedatos::instancia()->clave_iva_apunte(ui->tableWidget->item(fila,0)->text()).isEmpty())
                   {
                    QDomElement tag231111 = doc.createElement("sii:Exenta");
                    tag23111.appendChild(tag231111);
                    QDomElement tag231111D = doc.createElement("sii:DetalleExenta");
                    tag231111.appendChild(tag231111D);
                    QString causa_exencion="E1";
                    if (exportacion) causa_exencion="E2";
                    if (eib) causa_exencion="E5";
                    if (isp_op_interiores) causa_exencion="E6";
                    addElementoTextoDom(doc,tag231111D,"sii:CausaExencion",causa_exencion);
                    addElementoTextoDom(doc,tag231111D,"sii:BaseImponible",
                                 convapunto(ui->tableWidget->item(fila,8)->text().remove(".")));
                    // if (!pruebas) ui->tableWidget->item(fila,18)->setText("X");
                    // vemos si en la fila siguiente sigue el mismo documento
                    if (eib) continue;
                    if (fila+1<ui->tableWidget->rowCount())
                      {
                       // col2 es fecha fra.
                       // col3 es documento
                       // col7 es asiento
                       if (ui->tableWidget->item(fila,2)->text()==ui->tableWidget->item(fila+1,2)->text()
                          && ui->tableWidget->item(fila,3)->text()==ui->tableWidget->item(fila+1,3)->text()
                          && ui->tableWidget->item(fila,7)->text()==ui->tableWidget->item(fila+1,7)->text()) {
                       fila++;
                       }
                        else continue;
                      } else continue;
                     // continue;
                    }

                QDomElement tag231111 = doc.createElement("sii:NoExenta");
                tag23111.appendChild(tag231111);

                // S1 No exenta - Sin/Con inversión sujeto pasivo
                if (isp_op_interiores) addElementoTextoDom(doc,tag231111,"sii:TipoNoExenta","S2");
                  else
                      addElementoTextoDom(doc,tag231111,"sii:TipoNoExenta","S1");

                QDomElement tag2311111 = doc.createElement("sii:DesgloseIVA");
                tag231111.appendChild(tag2311111);

                QString asiento=ui->tableWidget->item(fila,7)->text();
                QString fecha_fra=ui->tableWidget->item(fila,2)->text();
                QString documento=ui->tableWidget->item(fila,3)->text();

                while(fila<ui->tableWidget->rowCount())
                  {
                   QDomElement tag23111111 = doc.createElement("sii:DetalleIVA");
                   tag2311111.appendChild(tag23111111);

                   addElementoTextoDom(doc,tag23111111,"sii:TipoImpositivo",convapunto(ui->tableWidget->item(fila,9)->text()));
                   addElementoTextoDom(doc,tag23111111,"sii:BaseImponible",convapunto(ui->tableWidget->item(fila,8)->text().remove(".")));
                   addElementoTextoDom(doc,tag23111111,"sii:CuotaRepercutida",convapunto(ui->tableWidget->item(fila,10)->text().remove(".")));
                   double tipo_re= convapunto(ui->tableWidget->item(fila,11)->text().remove(".")).toDouble();
                   //qDebug() << convapunto(ui->tableWidget->item(fila,11)->text().remove("."));
                   //QString cadprun; cadprun.setNum(tipo_re,'f',2);
                   //qDebug() << cadprun;
                   if (tipo_re>0.0001)
                     {
                      addElementoTextoDom(doc,tag23111111,"sii:TipoRecargoEquivalencia",convapunto(ui->tableWidget->item(fila,11)->text().remove(".")));
                      addElementoTextoDom(doc,tag23111111,"sii:CuotaRecargoEquivalencia",convapunto(ui->tableWidget->item(fila,12)->text().remove(".")));
                     }
                   // if (!pruebas && xml_gen_aislado) ui->tableWidget->item(fila,18)->setText("X");
                   if (fila+1>=ui->tableWidget->rowCount()) break;
                   if (ui->tableWidget->item(fila+1,7)->text()==asiento &&
                      ui->tableWidget->item(fila+1,2)->text()==fecha_fra &&
                      ui->tableWidget->item(fila+1,3)->text()==documento) fila++;
                    else break;
                  }
               } // fin desglose factura

         // fin del DesgloseFactura
         // Fecha Registro Contable
         // if (!pruebas && xml_gen_aislado) ui->tableWidget->item(fila,18)->setText("X");
        }

    QSqlQuery q=basedatos::instancia()->select_direc_SII();
    QString siiLR, sii;
    if (q.isActive())
        if (q.next()) {
            siiLR=q.value(4).toString();
            sii=q.value(5).toString();
        }
    QString cadini="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    cadini+="<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
    if (!siiLR.isEmpty() && !sii.isEmpty()) {
        cadini+="xmlns:siiLR=\"";
        cadini+=siiLR;
        cadini+="\" ";
        cadini+="xmlns:sii=\"";
        cadini+=sii;
        cadini+="\">\n";
        cadini+="<soapenv:Header/>\n";
    }
     else
        cadini+= "xmlns:siiLR=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroLR.xsd\" "
                 "xmlns:sii=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n"
                 "<soapenv:Header/>\n";

    QString xml = doc.toString();
    xml.remove("<!DOCTYPE ENVIO>");
    xml.prepend(cadini);
    /* xml.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                "xmlns:siiLR=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroLR.xsd\" "
                "xmlns:sii=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n"
                "<soapenv:Header/>\n");*/

    xml.append("</soapenv:Envelope>\n");
    // -----------------------------------------------------------------------------------

    QFile fichero( adapta(nombrefich)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    // stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

    // bool esmysql = ( basedatos::instancia()->cualControlador() == basedatos::MYSQL );

    // ------------------------------------------------------------------------------------
    stream << xml;
    fichero.close();


    return true;
}

bool tablaSII::envio_SII(bool pruebas)
{
    bool enviar_info=false;
    for (int fila=0; fila<ui->tableWidget->rowCount(); fila++)
      {

        if (!modificacion) {
           if (ui->tableWidget->item(fila,20)->text().isEmpty() &&
              ui->tableWidget->item(fila,18)->text().isEmpty()) {enviar_info=true; break;}
        }
        else {
            if (ui->tableWidget->item(fila,20)->text().isEmpty() &&
               !ui->tableWidget->item(fila,18)->text().isEmpty()) {enviar_info=true; break;}
        }
      }

    if (!enviar_info) {
        QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: No hay información para enviar"));
        return false;
    }



    // cargamos fichero certificado
        QString fich_certificado;
        QFileDialog dialogofich(this);
        dialogofich.setFileMode(QFileDialog::ExistingFile);
        dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
        dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
        dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
        dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
        dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

        QStringList filtros;
        filtros << tr("Archivos de certificado usuario (*.pem *.pfx *.p12)");
        dialogofich.setNameFilters(filtros);
        dialogofich.setDirectory(adapta(dirtrabajo_certificados()));
        dialogofich.setWindowTitle(tr("SELECCIÓN DE CERTIFICADO"));
        // dialogofich.exec();
        //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
        //                                              dirtrabajo,
        //                                              tr("Ficheros de texto (*.txt)"));
        QStringList fileNames;
        if (dialogofich.exec())
           {
            fileNames = dialogofich.selectedFiles();
            if (fileNames.at(0).length()>0)
                {
                 // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
                 fich_certificado=fileNames.at(0);
                }
           }
           else return false;


         pidenombre *p = new pidenombre();
         p->tipo_password();
         p->asignaetiqueta(tr("CLAVE DEL CERTIFICADO:"));
         p->asignanombreventana(tr("PETICIÓN DE CLAVE"));
         int cod=p->exec();
         if (!(cod==QDialog::Accepted))
            {
             delete p;
             return false;
            }
         QString clave=p->contenido();
         delete p;

   QProgressDialog progress(tr("Enviando información ..."), 0, 0, 0, this);
   progress.show();
   QApplication::processEvents();


    QString nombrefichero;
    nombrefichero=dirtrabajo_sii();
    nombrefichero.append(QDir::separator());
    if (recibidas) nombrefichero+="RECIBIDAS";
       else nombrefichero+="EMITIDAS";
    if (pruebas) nombrefichero+="-PRUEBAS";
    QDate fecha;
    nombrefichero+="-";
    nombrefichero+=fecha.currentDate().toString("yyMMdd");
    nombrefichero+="-";
    QTime hora;
    hora=hora.currentTime();
    nombrefichero+=hora.toString("hhmm");
    nombrefichero+=".xml";

    // GENERAMOS xml
    if (recibidas)
     {
      if (!fich_sii_recibidas(nombrefichero))
        {
         progress.close();
         QMessageBox::warning( this, tr("FICHERO SII"),
                               tr("ERROR: No se ha podido generar el archivo XML"));
         return false;
        }
     }
    else
       {
        if (!fich_sii_emitidas(nombrefichero))
           {
            progress.close();
            QMessageBox::warning( this, tr("FICHERO SII"),
                                  tr("ERROR: No se ha podido generar el archivo XML"));
            return false;
           }
       }

    QString emitidas_test, recibidas_test, emitidas_real, recibidas_real;
    QSqlQuery q=basedatos::instancia()->select_direc_SII();
    if (q.isActive())
     if (q.next())
       {
         emitidas_test=q.value(0).toString();
         recibidas_test=q.value(1).toString();
         emitidas_real=q.value(2).toString();
         recibidas_real=q.value(3).toString();
       }


    if (curl_sii) {
        // llamamos a curl
        //
       QStringList arguments;
       arguments << "--connect-timeout";
       arguments << "50";
       arguments << "-d";
       arguments << "@"+nombrefichero;
       arguments << "-o";

       nombrefichero_respuesta=nombrefichero;
       nombrefichero_respuesta=nombrefichero_respuesta.remove(".xml");
       nombrefichero_respuesta+="_RESPUESTA.xml";

       arguments << nombrefichero_respuesta;
       arguments << "-E";

       // fichero certificado
       arguments << fich_certificado+":"+clave;

       // solo falta ya la dirección web

       if (recibidas)
         {
          if (pruebas) arguments << recibidas_test ;
           else arguments << recibidas_real;
         }
         else
          {
            if (pruebas) arguments << emitidas_test;
            else arguments << emitidas_real;
          }

       if (ui->insegurocheckBox->isChecked())
         arguments << "-k";

       QApplication::processEvents();

       QProcess *myProcess = new QProcess(NULL);

       QString rutacurl="curl";
       if (!global_ruta_curl.trimmed().isEmpty()) rutacurl=global_ruta_curl;

       // QMessageBox::information( this, "prueba SII",rutacurl);

       myProcess->start(rutacurl,arguments);

       if (!myProcess->waitForStarted ())
         {
          progress.close();
          delete myProcess;
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: ¿ Está instalada la aplicación CURL ?"));
          return false;
         }
       if (!myProcess->waitForFinished (50000))
         {
          progress.close();
          delete myProcess;
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: Problemas con la conexión"));
          return false;
         }

       delete myProcess;

       progress.close();
    }
    else { // NO CURL
          // -------------------------------------
        QString wurl;
        if (recibidas)
          {
           if (pruebas) wurl = recibidas_test ;
            else wurl = recibidas_real;
          }
          else
           {
             if (pruebas) wurl = emitidas_test;
             else wurl = emitidas_real;
           }
          aeat_soap validation;
          validation.process(nombrefichero,wurl,fich_certificado,clave);
          bool processed=validation.processed();
          if (!processed) {
              QMessageBox::warning( this, tr("FICHERO SII"),
                                  tr("ERROR: Problemas con la conexión"));
              return false;
          }

          // -------------------------------------
          QString xml=validation.response().toString();
          nombrefichero_respuesta=nombrefichero;
          nombrefichero_respuesta=nombrefichero_respuesta.remove(".xml");
          nombrefichero_respuesta+="_RESPUESTA.xml";
          QFile fichero( adapta(nombrefichero_respuesta)  );

          if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);

          // stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

          // bool esmysql = ( basedatos::instancia()->cualControlador() == basedatos::MYSQL );

          // ------------------------------------------------------------------------------------
          stream << xml;
          fichero.close();

     }

    return true;
}


void tablaSII::envio_real()
{

    pruebas=false;
    xml_gen_aislado=false;

    if (envio_SII(false))
      {
       QDomDocument doc("RESPUESTA");
       QFile fichero(adapta(nombrefichero_respuesta));
       if (!fichero.exists())
          {
           QMessageBox::warning( this, tr("FICHERO SII"),
                                tr("ERROR: La conexión no ha devuelto fichero de respuesta"));
           xml_gen_aislado=true;
           return;

          }

       QProgressDialog progress(tr("Procesando respuesta ..."), 0, 0, 0, this);
       progress.show();
       QApplication::processEvents();

       if ( !fichero.open( QIODevice::ReadOnly ) )
          {
           QMessageBox::warning( this, tr("FICHERO SII"),
                                tr("ERROR: La conexión no ha devuelto respuesta"));
           xml_gen_aislado=true;
           return;
          }
       if (!doc.setContent(&fichero)) {
          fichero.close();
          QMessageBox::warning( this, tr("FICHERO SII"),
                               tr("ERROR: La conexión no ha devuelto respuesta"));
          xml_gen_aislado=true;
          return;
         }
       fichero.close();

       respuesta_sii *r = new respuesta_sii();
       if (recibidas) r->pararecibidas();
       QApplication::processEvents();
       r->pasa_dom(doc);
       r->exec();
       // columna 0 Emisior
       // columna 1 Factura
       // columna 2 Fecha
       // columna 3 resultado
       // con recibidas vale el emisor; con emitidas no vale- siempre es el mismo
       /* int lineas=r->filas();
       for (int veces=0; veces<lineas; veces++)
          {
           // contenido_celda(int fila, int columna)
           QString emisor=r->contenido_celda(veces,0);
           QString factura=r->contenido_celda(veces,1);
           QString fecha=r->contenido_celda(veces,2);
           QString resultado=r->contenido_celda(veces,3);
           for (int veces2=0; veces2<ui->tableWidget->rowCount(); veces2++)
               {
                if (resultado=="Incorrecto") continue;
                if (recibidas && !emisor.isEmpty())
                   {
                     if (ui->tableWidget->item(veces2,2)->text()==fecha && ui->tableWidget->item(veces2,3)->text()==factura && ui->tableWidget->item(veces2,6)->text()==emisor)
                        ui->tableWidget->item(veces2,18)->setText("X");
                   }
                   else
                       {
                         if (ui->tableWidget->item(veces2,2)->text()==fecha && ui->tableWidget->item(veces2,3)->text()==factura)
                            ui->tableWidget->item(veces2,18)->setText("X");

                       }
               }
          } */
       delete(r);
       QMessageBox::information(this,tr("SII"),tr("El proceso ha terminado. Pulse una tecla para refrescar"));
       refrescar();

      }

    xml_gen_aislado=true;

    //envio_SII(false);
}


void tablaSII::envio_pruebas()
{
    pruebas=true;
    if (envio_SII(true))
      {
       QDomDocument doc("RESPUESTA");
       QFile fichero(adapta(nombrefichero_respuesta));
       if (!fichero.exists())
         {
          QMessageBox::warning( this, tr("FICHERO SII"),
                             tr("ERROR: La conexión no ha devuelto fichero de respuesta"));
          pruebas=false;
          return;

         }

       if ( !fichero.open( QIODevice::ReadOnly ) ) return ;

       QProgressDialog progress(tr("Procesando respuesta ..."), 0, 0, 0, this);
       progress.show();
       QApplication::processEvents();


       if (!doc.setContent(&fichero)) {
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: La conexión no ha devuelto respuesta"));
          fichero.close();
          pruebas=false;
          return;
         }
       fichero.close();

       respuesta_sii *r = new respuesta_sii();
       if (recibidas) r->pararecibidas();
       QApplication::processEvents();
       r->activa_pruebas();
       r->pasa_dom(doc);
       r->exec();
       delete(r);

      }
    // envio_SII(true) ;
    pruebas=false;
}


void tablaSII::copiar() {
    QString contenido;
    for (int veces=0;veces<ui->tableWidget->columnCount();veces++) {
       contenido+=ui->tableWidget->horizontalHeaderItem(veces)->text();
       if (veces!=(ui->tableWidget->columnCount()-1)) contenido+="\t";
    }
    contenido+="\n";
    for (int fila=0; fila<ui->tableWidget->rowCount();fila++) {
        for (int columna=0; columna<ui->tableWidget->columnCount(); columna++) {
            if (ui->tableWidget->item(fila,columna)!=NULL)
              contenido+=ui->tableWidget->item(fila,columna)->text();
            if (columna!=(ui->tableWidget->columnCount()-1)) contenido+="\t";
        }
        contenido+="\n";
    }
    QClipboard *cb = QApplication::clipboard();
    cb->setText(contenido);
    QMessageBox::information( this, tr("TABLA SII"),
                             tr("Se ha pasado el contenido al portapapeles") );
}


bool tablaSII::fich_sii_anular_envio_emitidas(QString nombrefich)
{

    QDomDocument doc("ENVIO");
    QDomElement root = doc.createElement("soapenv:Body");
    doc.appendChild(root);

    QDomElement tag = doc.createElement("siiLR:BajaLRFacturasEmitidas");
    root.appendChild(tag);

    QDomElement tag1 = doc.createElement("sii:Cabecera");
    tag.appendChild(tag1);

    addElementoTextoDom(doc,tag1,"sii:IDVersionSii",VERSION_SII);

    QDomElement tag11 = doc.createElement("sii:Titular");
    tag1.appendChild(tag11);

    addElementoTextoDom(doc,tag11,"sii:NombreRazon",nombreempresa());
    addElementoTextoDom(doc,tag11,"sii:NIF",basedatos::instancia()->cif());



    // Ya tenemos la cabecera  LISTA - vamos a por IDFactura

    // aquí un blucle
    for (int fila=0; fila<ui->tableWidget->rowCount(); fila++)
        {
         if (!ui->tableWidget->item(fila,20)->text().isEmpty()) continue;
         if (ui->tableWidget->item(fila,18)->text().isEmpty()) continue;

         QDomElement tag2 = doc.createElement("siiLR:RegistroLRBajaExpedidas");
         tag.appendChild(tag2);

         QDomElement tag21 = doc.createElement("sii:PeriodoLiquidacion");
         tag2.appendChild(tag21);
         // completar con ejercicio y periodo
         QString cadyear; cadyear.setNum(ui->inicialdateEdit->date().year());
         cadyear=cadyear.trimmed();
         addElementoTextoDom(doc,tag21,"sii:Ejercicio",cadyear);
         addElementoTextoDom(doc,tag21,"sii:Periodo",ui->periodocomboBox->currentText().left(2));

         QDomElement tag22 = doc.createElement("siiLR:IDFactura");
         tag2.appendChild(tag22);

         QDomElement tag221 = doc.createElement("sii:IDEmisorFactura");
         tag22.appendChild(tag221);

         addElementoTextoDom(doc,tag221,"sii:NIF",basedatos::instancia()->cif());

         QString finicial, ffinal;

         QSqlQuery q=basedatos::instancia()->campos_libroiva(ui->tableWidget->item(fila,0)->text());
         if (q.isActive())
             if (q.next())
                {
                 finicial=q.value(19).toString();
                 ffinal=q.value(20).toString();
                }


         if (finicial.isEmpty())
             addElementoTextoDom(doc,tag22,"sii:NumSerieFacturaEmisor",ui->tableWidget->item(fila,3)->text());
           else
                {
                 addElementoTextoDom(doc,tag22,"sii:NumSerieFacturaEmisor",finicial);
                 addElementoTextoDom(doc,tag22,"sii:NumSerieFacturaEmisorResumenFin",ffinal);
                }

         addElementoTextoDom(doc,tag22,"sii:FechaExpedicionFacturaEmisor",ui->tableWidget->item(fila,2)->text());

         tag2.appendChild(tag22);

         if (fila+1<ui->tableWidget->rowCount())
           {
            // col2 es fecha fra.
            // col3 es documento
            // col7 es asiento
            if (ui->tableWidget->item(fila,2)->text()==ui->tableWidget->item(fila+1,2)->text()
               && ui->tableWidget->item(fila,3)->text()==ui->tableWidget->item(fila+1,3)->text()
               && ui->tableWidget->item(fila,7)->text()==ui->tableWidget->item(fila+1,7)->text()) {
            fila++;
            continue;
            }
           }
        }

    QSqlQuery q=basedatos::instancia()->select_direc_SII();
    QString siiLR, sii;
    if (q.isActive())
        if (q.next()) {
            siiLR=q.value(4).toString();
            sii=q.value(5).toString();
        }
    QString cadini="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    cadini+="<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
    if (!siiLR.isEmpty() && !sii.isEmpty()) {
        cadini+="xmlns:siiLR=\"";
        cadini+=siiLR;
        cadini+="\" ";
        cadini+="xmlns:sii=\"";
        cadini+=sii;
        cadini+="\">\n";
        cadini+="<soapenv:Header/>\n";
    }
     else
        cadini+= "xmlns:siiLR=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroLR.xsd\" "
                 "xmlns:sii=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n"
                 "<soapenv:Header/>\n";

    QString xml = doc.toString();
    xml.remove("<!DOCTYPE ENVIO>");
    xml.prepend(cadini);
    /* xml.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                "xmlns:siiLR=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroLR.xsd\" "
                "xmlns:sii=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n"
                "<soapenv:Header/>\n");*/

    xml.append("</soapenv:Envelope>\n");
    // -----------------------------------------------------------------------------------

    QFile fichero( adapta(nombrefich)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    // stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

    // bool esmysql = ( basedatos::instancia()->cualControlador() == basedatos::MYSQL );

    // ------------------------------------------------------------------------------------
    stream << xml;
    fichero.close();


    return true;
}



bool tablaSII::envio_SII_anular_envio(bool pruebas)
    {
        bool enviar_info=false;
        for (int fila=0; fila<ui->tableWidget->rowCount(); fila++)
          {
           if (ui->tableWidget->item(fila,20)->text().isEmpty() &&
               !ui->tableWidget->item(fila,18)->text().isEmpty()) {enviar_info=true; break;}
          }

        if (!enviar_info) {
            QMessageBox::warning( this, tr("FICHERO SII"),
                                  tr("ERROR: No hay información para enviar"));
            return false;
        }

        // cargamos fichero certificado
            QString fich_certificado;
            QFileDialog dialogofich(this);
            dialogofich.setFileMode(QFileDialog::ExistingFile);
            dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
            dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
            dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
            dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
            dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

            QStringList filtros;
            filtros << tr("Archivos de certificado usuario (*.pem)");
            dialogofich.setNameFilters(filtros);
            dialogofich.setDirectory(adapta(dirtrabajo_certificados()));
            dialogofich.setWindowTitle(tr("SELECCIÓN DE CERTIFICADO"));
            // dialogofich.exec();
            //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
            //                                              dirtrabajo,
            //                                              tr("Ficheros de texto (*.txt)"));
            QStringList fileNames;
            if (dialogofich.exec())
               {
                fileNames = dialogofich.selectedFiles();
                if (fileNames.at(0).length()>0)
                    {
                     // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
                     fich_certificado=fileNames.at(0);
                    }
               }
               else return false;


             pidenombre *p = new pidenombre();
             p->tipo_password();
             p->asignaetiqueta(tr("CLAVE DEL CERTIFICADO:"));
             p->asignanombreventana(tr("PETICIÓN DE CLAVE"));
             int cod=p->exec();
             if (!(cod==QDialog::Accepted))
                {
                 delete p;
                 return false;
                }
             QString clave=p->contenido();
             delete p;

        QString nombrefichero;
        nombrefichero=dirtrabajo_sii();
        nombrefichero.append(QDir::separator());
        if (recibidas) nombrefichero+="RECIBIDAS-ANUL";
           else nombrefichero+="EMITIDAS-ANUL";
        if (pruebas) nombrefichero+="-PRUEBAS";
        QDate fecha;
        nombrefichero+="-";
        nombrefichero+=fecha.currentDate().toString("yyMMdd");
        nombrefichero+="-";
        QTime hora;
        hora=hora.currentTime();
        nombrefichero+=hora.toString("hhmm");
        nombrefichero+=".xml";

        // GENERAMOS xml
        if (recibidas)
         {
          //if (!fich_sii_recibidas(nombrefichero))
          //  {
          //   QMessageBox::warning( this, tr("FICHERO SII"),
          //                         tr("ERROR: No se ha podido generar el archivo XML"));
          //   return false;
          //  }
         }
        else
           {
            if (!fich_sii_anular_envio_emitidas(nombrefichero))
               {
                QMessageBox::warning( this, tr("FICHERO SII"),
                                      tr("ERROR: No se ha podido generar el archivo XML"));
                return false;
               }
           }



        // llamamos a curl
        //




        QStringList arguments;
        arguments << "--connect-timeout";
        arguments << "50";
        arguments << "-d";
        arguments << "@"+nombrefichero;
        arguments << "-o";

        nombrefichero_respuesta=nombrefichero;
        nombrefichero_respuesta=nombrefichero_respuesta.remove(".xml");
        nombrefichero_respuesta+="_RESPUESTA.xml";

        arguments << nombrefichero_respuesta;
        arguments << "-E";

        // fichero certificado
        arguments << fich_certificado+":"+clave;

        // solo falta ya la dirección web
        QString emitidas_test, recibidas_test, emitidas_real, recibidas_real;
        QSqlQuery q=basedatos::instancia()->select_direc_SII();
        if (q.isActive())
            if (q.next())
              {
                emitidas_test=q.value(0).toString();
                recibidas_test=q.value(1).toString();
                emitidas_real=q.value(2).toString();
                recibidas_real=q.value(3).toString();
              }

        if (recibidas)
          {
           if (pruebas) arguments << recibidas_test ;
            else arguments << recibidas_real;
          }
          else
              {
                if (pruebas) arguments << emitidas_test;
                else arguments << emitidas_real;
              }

        if (ui->insegurocheckBox->isChecked())
            arguments << "-k";

        QProcess *myProcess = new QProcess(NULL);

        QString rutacurl="curl";
        if (!global_ruta_curl.trimmed().isEmpty()) rutacurl=global_ruta_curl;

        // QMessageBox::information( this, "prueba SII",rutacurl);

        myProcess->start(rutacurl,arguments);

        if (!myProcess->waitForStarted ())
           {
             delete myProcess;
            QMessageBox::warning( this, tr("FICHERO SII"),
                                  tr("ERROR: ¿ Está instalada la aplicación CURL ?"));
             return false;
           }
        if (!myProcess->waitForFinished (50000))
           {
             delete myProcess;
             QMessageBox::warning( this, tr("FICHERO SII"),
                                  tr("ERROR: Problemas con la conexión"));
             return false;
           }

        delete myProcess;

        return true;
}


bool tablaSII::consulta_SII(bool pruebas)
{
    // cargamos fichero certificado
        QString fich_certificado;
        QFileDialog dialogofich(this);
        dialogofich.setFileMode(QFileDialog::ExistingFile);
        dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
        dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
        dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
        dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
        dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

        QStringList filtros;
        filtros << tr("Archivos de certificado usuario (*.pem *.pfx *.p12)");
        dialogofich.setNameFilters(filtros);
        dialogofich.setDirectory(adapta(dirtrabajo_certificados()));
        dialogofich.setWindowTitle(tr("SELECCIÓN DE CERTIFICADO"));
        // dialogofich.exec();
        //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
        //                                              dirtrabajo,
        //                                              tr("Ficheros de texto (*.txt)"));
        QStringList fileNames;
        if (dialogofich.exec())
           {
            fileNames = dialogofich.selectedFiles();
            if (fileNames.at(0).length()>0)
                {
                 // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
                 fich_certificado=fileNames.at(0);
                }
           }
           else return false;


         pidenombre *p = new pidenombre();
         p->tipo_password();
         p->asignaetiqueta(tr("CLAVE DEL CERTIFICADO:"));
         p->asignanombreventana(tr("PETICIÓN DE CLAVE"));
         int cod=p->exec();
         if (!(cod==QDialog::Accepted))
            {
             delete p;
             return false;
            }
         QString clave=p->contenido();
         delete p;

    QString nombrefichero;
    nombrefichero=dirtrabajo_sii();
    nombrefichero.append(QDir::separator());
    if (recibidas) nombrefichero+="RECIBIDAS-CONS";
       else nombrefichero+="EMITIDAS-CONS";
    if (pruebas) nombrefichero+="-PRUEBAS";
    QDate fecha;
    nombrefichero+="-";
    nombrefichero+=fecha.currentDate().toString("yyMMdd");
    nombrefichero+="-";
    QTime hora;
    hora=hora.currentTime();
    nombrefichero+=hora.toString("hhmm");
    nombrefichero+=".xml";

    // GENERAMOS xml
    if (recibidas) {
        if (!fich_cons_sii_recibidas(nombrefichero)) {
            QMessageBox::warning(this, tr("TABLA SII"), tr("ERROR AL GENERAR EL ARCHIVO"));
            return false;
        }
     }
    else {
        if (!fich_cons_sii_emitidas(nombrefichero)) {
            QMessageBox::warning(this, tr("TABLA SII"), tr("ERROR AL GENERAR EL ARCHIVO"));
            return false;
        }
    }

    QString emitidas_test, recibidas_test, emitidas_real, recibidas_real;
    QSqlQuery q=basedatos::instancia()->select_direc_SII();
    if (q.isActive())
     if (q.next())
       {
         emitidas_test=q.value(0).toString();
         recibidas_test=q.value(1).toString();
         emitidas_real=q.value(2).toString();
         recibidas_real=q.value(3).toString();
       }


    if (curl_sii) {
        // llamamos a curl
        //
       QStringList arguments;
       arguments << "--connect-timeout";
       arguments << "50";
       arguments << "-d";
       arguments << "@"+nombrefichero;
       arguments << "-o";

       nombrefichero_respuesta=nombrefichero;
       nombrefichero_respuesta=nombrefichero_respuesta.remove(".xml");
       nombrefichero_respuesta+="_RESPUESTA.xml";

       arguments << nombrefichero_respuesta;
       arguments << "-E";

       // fichero certificado
       arguments << fich_certificado+":"+clave;

       // solo falta ya la dirección web

       if (recibidas)
         {
          if (pruebas) arguments << recibidas_test ;
           else arguments << recibidas_real;
         }
         else
          {
            if (pruebas) arguments << emitidas_test;
            else arguments << emitidas_real;
          }

       if (ui->insegurocheckBox->isChecked())
         arguments << "-k";

       QApplication::processEvents();

       QProcess *myProcess = new QProcess(NULL);

       QString rutacurl="curl";
       if (!global_ruta_curl.trimmed().isEmpty()) rutacurl=global_ruta_curl;

       // QMessageBox::information( this, "prueba SII",rutacurl);

       myProcess->start(rutacurl,arguments);

       if (!myProcess->waitForStarted ())
         {
          delete myProcess;
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: ¿ Está instalada la aplicación CURL ?"));
          return false;
         }
       if (!myProcess->waitForFinished (50000))
         {
          delete myProcess;
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: Problemas con la conexión"));
          return false;
         }

       delete myProcess;

    }
    else { // NO CURL
          // -------------------------------------
        QString wurl;
        if (recibidas)
          {
           if (pruebas) wurl = recibidas_test ;
            else wurl = recibidas_real;
          }
          else
           {
             if (pruebas) wurl = emitidas_test;
             else wurl = emitidas_real;
           }
          aeat_soap validation;
          validation.process(nombrefichero,wurl,fich_certificado,clave);
          bool processed=validation.processed();
          if (!processed) {
              QMessageBox::warning( this, tr("FICHERO SII"),
                                  tr("ERROR: Problemas con la conexión"));
              return false;
          }

          // -------------------------------------
          QString xml=validation.response().toString();
          nombrefichero_respuesta=nombrefichero;
          nombrefichero_respuesta=nombrefichero_respuesta.remove(".xml");
          nombrefichero_respuesta+="_RESPUESTA.xml";
          QFile fichero( adapta(nombrefichero_respuesta)  );

          if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);


          // ------------------------------------------------------------------------------------
          stream << xml;
          fichero.close();

     }



  return true;
}

bool tablaSII::fich_cons_sii_recibidas(QString nombrefich)
{
    QDomDocument doc("ENVIO");
    QDomElement root = doc.createElement("soapenv:Body");
    doc.appendChild(root);

    QDomElement tag = doc.createElement("con:ConsultaLRFacturasRecibidas");
    root.appendChild(tag);

    QDomElement tag1 = doc.createElement("sum:Cabecera");
    tag.appendChild(tag1);

    addElementoTextoDom(doc,tag1,"sum:IDVersionSii",VERSION_SII);

    QDomElement tag11 = doc.createElement("sum:Titular");
    tag1.appendChild(tag11);

    addElementoTextoDom(doc,tag11,"sum:NombreRazon",nombreempresa());
    addElementoTextoDom(doc,tag11,"sum:NIF",basedatos::instancia()->cif());

    QDomElement tag111 = doc.createElement("con:FiltroConsulta");
    tag.appendChild(tag111);

    QDomElement tag1111 = doc.createElement("sum:PeriodoLiquidacion");
    tag111.appendChild(tag1111);

   // vamos por aquí
    // completar con ejercicio y periodo
    QString cadyear; cadyear.setNum(ui->inicialdateEdit->date().year());
    cadyear=cadyear.trimmed();
    addElementoTextoDom(doc,tag1111,"sum:Ejercicio",cadyear);
    addElementoTextoDom(doc,tag1111,"sum:Periodo",ui->periodocomboBox->currentText().left(2));

    QString cadini="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    cadini+="<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
    cadini+="xmlns:con=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/ConsultaLR.xsd\" "
            "xmlns:sum=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n";

    QString xml = doc.toString();
    xml.remove("<!DOCTYPE ENVIO>");
    xml.prepend(cadini);

    xml.append("</soapenv:Envelope>\n");
    // -----------------------------------------------------------------------------------

    QFile fichero( adapta(nombrefich)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << xml;
    fichero.close();


    return true;

}

bool tablaSII::fich_cons_sii_emitidas(QString nombrefich)
{
    QDomDocument doc("ENVIO");
    QDomElement root = doc.createElement("soapenv:Body");
    doc.appendChild(root);

    QDomElement tag = doc.createElement("con:ConsultaLRFacturasEmitidas");
    root.appendChild(tag);

    QDomElement tag1 = doc.createElement("sum:Cabecera");
    tag.appendChild(tag1);

    addElementoTextoDom(doc,tag1,"sum:IDVersionSii",VERSION_SII);

    QDomElement tag11 = doc.createElement("sum:Titular");
    tag1.appendChild(tag11);

    addElementoTextoDom(doc,tag11,"sum:NombreRazon",nombreempresa());
    addElementoTextoDom(doc,tag11,"sum:NIF",basedatos::instancia()->cif());

    QDomElement tag111 = doc.createElement("con:FiltroConsulta");
    tag.appendChild(tag111);

    QDomElement tag1111 = doc.createElement("sum:PeriodoLiquidacion");
    tag111.appendChild(tag1111);

   // vamos por aquí
    // completar con ejercicio y periodo
    QString cadyear; cadyear.setNum(ui->inicialdateEdit->date().year());
    cadyear=cadyear.trimmed();
    addElementoTextoDom(doc,tag1111,"sum:Ejercicio",cadyear);
    addElementoTextoDom(doc,tag1111,"sum:Periodo",ui->periodocomboBox->currentText().left(2));

    QString cadini="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    cadini+="<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
    cadini+="xmlns:con=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/ConsultaLR.xsd\" "
            "xmlns:sum=\"https://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/ssii/fact/ws/SuministroInformacion.xsd\">\n";

    QString xml = doc.toString();
    xml.remove("<!DOCTYPE ENVIO>");
    xml.prepend(cadini);

    xml.append("</soapenv:Envelope>\n");
    // -----------------------------------------------------------------------------------

    QFile fichero( adapta(nombrefich)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << xml;
    fichero.close();


    return true;

}



void tablaSII::envio_real_anul()
{


    if (envio_SII_anular_envio(false))
      {
       QFile fichero(adapta(nombrefichero_respuesta));
       if (!fichero.exists())
          {
           QMessageBox::warning( this, tr("FICHERO SII"),
                                tr("ERROR: La conexión no ha devuelto fichero de respuesta"));
           return;

          }

       if ( !fichero.open( QIODevice::ReadOnly ) )
          {
           return;
          }


       notas *n = new notas(false);


       QString data;
       data = fichero.readAll();

       fichero.close();

       n->pasa_contenido(data);

       n->exec();

       delete(n);
     }
    //envio_SII(false);
}


void tablaSII::envio_real_modif() {

modificacion=true;
envio_real();
modificacion=false;

}


void tablaSII::envio_pruebas_modif() {
    modificacion=true;
    envio_pruebas();
    modificacion=false;
}



void tablaSII::on_comprueba_nifs_pushButton_clicked()
{
    QStringList nif_list, name_list;
    int rows=ui->tableWidget->rowCount();
    if (rows<1) return;
    for (int linea=0; linea<rows; linea ++) {
        nif_list << ui->tableWidget->item(linea, 6)->text();
        name_list << ui->tableWidget->item(linea,5)->text();
    }
    QString xml=xml_aeat_ok_ids(nif_list,name_list);
    // qDebug() << xml;


    // cargamos fichero certificado
        QString fich_certificado;
        QFileDialog dialogofich(this);
        dialogofich.setFileMode(QFileDialog::ExistingFile);
        dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
        dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
        dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
        dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
        dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

        QStringList filtros;
        filtros << tr("Archivos de certificado usuario (*.pem *.pfx *.p12)");
        dialogofich.setNameFilters(filtros);
        dialogofich.setDirectory(adapta(dirtrabajo_certificados()));
        dialogofich.setWindowTitle(tr("SELECCIÓN DE CERTIFICADO"));
        // dialogofich.exec();
        //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
        //                                              dirtrabajo,
        //                                              tr("Ficheros de texto (*.txt)"));
        QStringList fileNames;
        if (dialogofich.exec())
           {
            fileNames = dialogofich.selectedFiles();
            if (fileNames.at(0).length()>0)
                {
                 // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
                 fich_certificado=fileNames.at(0);
                }
           }
           else return;


         pidenombre *p = new pidenombre();
         p->tipo_password();
         p->asignaetiqueta(tr("CLAVE DEL CERTIFICADO:"));
         p->asignanombreventana(tr("PETICIÓN DE CLAVE"));
         int cod=p->exec();
         if (!(cod==QDialog::Accepted))
            {
             delete p;
             return ;
            }
         QString clave=p->contenido();
         delete p;

   QProgressDialog progress(tr("Enviando información ..."), 0, 0, 0, this);
   progress.show();
   QApplication::processEvents();


    QString nombrefichero;
    nombrefichero=dirtrabajo_sii();
    nombrefichero.append(QDir::separator());
    nombrefichero+="CHECK_TAX_IDS";
    QDate fecha;
    nombrefichero+="-";
    nombrefichero+=fecha.currentDate().toString("yyMMdd");
    nombrefichero+="-";
    QTime hora;
    hora=hora.currentTime();
    nombrefichero+=hora.toString("hhmm");
    nombrefichero+=".xml";

    QFile fichero( adapta(nombrefichero)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << xml;
    fichero.close();

    if (curl_sii) {
        // llamamos a curl
        //
       QStringList arguments;
       arguments << "--connect-timeout";
       arguments << "50";
       arguments << "-d";
       arguments << "@"+nombrefichero;
       arguments << "-o";

       nombrefichero_respuesta=nombrefichero;
       nombrefichero_respuesta=nombrefichero_respuesta.remove(".xml");
       nombrefichero_respuesta+="_RESPUESTA.xml";

       arguments << nombrefichero_respuesta;
       arguments << "-E";

       // fichero certificado
       arguments << fich_certificado+":"+clave;

       // solo falta ya la dirección web

       arguments << "https://www1.agenciatributaria.gob.es/wlpl/BURT-JDIT/ws/VNifV2SOAP";

       if (ui->insegurocheckBox->isChecked())
         arguments << "-k";

       QApplication::processEvents();

       QProcess *myProcess = new QProcess(NULL);

       QString rutacurl="curl";
       if (!global_ruta_curl.trimmed().isEmpty()) rutacurl=global_ruta_curl;

       // QMessageBox::information( this, "prueba SII",rutacurl);

       myProcess->start(rutacurl,arguments);

       if (!myProcess->waitForStarted ())
         {
          progress.close();
          delete myProcess;
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: ¿ Está instalada la aplicación CURL ?"));
          return;
         }
       if (!myProcess->waitForFinished (50000))
         {
          progress.close();
          delete myProcess;
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: Problemas con la conexión"));
          return;
         }

       delete myProcess;

       progress.close();
    }
    else { // NO CURL
          // -------------------------------------
          QString wurl = "https://www1.agenciatributaria.gob.es/wlpl/BURT-JDIT/ws/VNifV2SOAP";
          aeat_soap validation;
          validation.process(nombrefichero,wurl,fich_certificado,clave);
          bool processed=validation.processed();
          if (!processed) {
              QMessageBox::warning( this, tr("FICHERO SII"),
                                  tr("ERROR: Problemas con la conexión"));
              return;
          }

          // -------------------------------------
          QString xml=validation.response().toString();
          nombrefichero_respuesta=nombrefichero;
          nombrefichero_respuesta=nombrefichero_respuesta.remove(".xml");
          nombrefichero_respuesta+="_RESPUESTA.xml";
          QFile fichero( adapta(nombrefichero_respuesta)  );

          if ( !fichero.open( QIODevice::WriteOnly ) ) return;

          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);

          stream << xml;
          fichero.close();
          progress.close();

     }

    QDomDocument doc("RESPUESTA");
    QFile fichero2(adapta(nombrefichero_respuesta));
    if (!fichero2.exists())
       {
        QMessageBox::warning( this, tr("FICHERO SII"),
                             tr("ERROR: La conexión no ha devuelto fichero de respuesta"));
        return;

       }

    QProgressDialog progress2(tr("Procesando respuesta ..."), 0, 0, 0, this);
    progress2.show();
    QApplication::processEvents();

    if ( !fichero2.open( QIODevice::ReadOnly ) )
       {
        QMessageBox::warning( this, tr("FICHERO SII"),
                             tr("ERROR: La conexión no ha devuelto respuesta"));
        return;
       }
    if (!doc.setContent(&fichero2)) {
       fichero2.close();
       QMessageBox::warning( this, tr("FICHERO SII"),
                            tr("ERROR: La conexión no ha devuelto respuesta"));
       return;
      }
    fichero2.close();

    respuesta_vnifv2 *r = new respuesta_vnifv2();
    QApplication::processEvents();
    progress2.close();
    r->pasa_dom(doc);
    r->exec();
}


void tablaSII::on_doc_ini_lineEdit_textChanged(const QString &arg1)
{
    ui->doc_fin_lineEdit->setText(arg1);
}


void tablaSII::on_consulta_pruebas_pushButton_clicked()
{

 if (consulta_SII(true))
   {
    QDomDocument doc("RESPUESTA");
    QFile fichero(adapta(nombrefichero_respuesta));
    if (!fichero.exists())
      {
       QMessageBox::warning( this, tr("FICHERO SII"),
                          tr("ERROR: La conexión no ha devuelto fichero de respuesta"));
       return;

      }

    if ( !fichero.open( QIODevice::ReadOnly ) ) return ;

    if (!doc.setContent(&fichero)) {
       QMessageBox::warning( this, tr("FICHERO SII"),
                           tr("ERROR: La conexión no ha devuelto respuesta"));
       fichero.close();
       return;
      }
    fichero.close();

    respuesta_sii_cons *r = new respuesta_sii_cons();
    if (recibidas) r->pararecibidas();
    QApplication::processEvents();
    r->pasa_dom(doc);
    r->exec();
    delete(r);

   }

}

void tablaSII::on_consulta_real_pushButton_clicked()
{
    if (consulta_SII(false))
      {
       QDomDocument doc("RESPUESTA");
       QFile fichero(adapta(nombrefichero_respuesta));
       if (!fichero.exists())
         {
          QMessageBox::warning( this, tr("FICHERO SII"),
                             tr("ERROR: La conexión no ha devuelto fichero de respuesta"));
          return;

         }

       if ( !fichero.open( QIODevice::ReadOnly ) ) return ;

       if (!doc.setContent(&fichero)) {
          QMessageBox::warning( this, tr("FICHERO SII"),
                              tr("ERROR: La conexión no ha devuelto respuesta"));
          fichero.close();
          return;
         }
       fichero.close();

       respuesta_sii_cons *r = new respuesta_sii_cons();
       if (recibidas) r->pararecibidas();
       QApplication::processEvents();
       r->pasa_dom(doc);
       r->exec();
       delete(r);

      }

}
