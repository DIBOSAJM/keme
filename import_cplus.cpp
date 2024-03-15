#include "import_cplus.h"
#include "ui_import_cplus.h"
#include "basedatos.h"
#include "funciones.h"
#include "directorio.h"
#include <QFileDialog>
#include "src-qdbf/qdbftable.h"
#include "src-qdbf/qdbfrecord.h"
#include <QMessageBox>
#include <QProgressDialog>

import_cplus::import_cplus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::import_cplus)
{
    ui->setupUi(this);
    QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
    QStringList ejs;
    if ( query.isActive() ) {
         while ( query.next() )
               ejs << query.value(0).toString();
          }
    // ej1 << noejercicio();
    ui->ejercicioscomboBox->addItems(ejs);
    connect(ui->rutapushButton, SIGNAL(clicked()),this, SLOT(ruta_pulsado()));
    connect(ui->procesarpushButton, SIGNAL(clicked()),this, SLOT(procesar()));
    connect(ui->anchopushButton,SIGNAL(clicked()),this, SLOT(mostrar_ancho_auxiliares()));

    ui->progressBar->setValue(0);

}

import_cplus::~import_cplus()
{
    delete ui;
}

void import_cplus::mostrar_ancho_auxiliares()
{
    QString cadfich=ui->rutalineEdit->text();
    // obtener lista de archivos .dbf
    QDir dirplus(cadfich);
    QStringList filters;
    filters << "*.dbf" ;
    dirplus.setNameFilters(filters);
    QStringList ficheros=dirplus.entryList();

    QString abrir;


    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="SUBCTA.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta SUBCTA.DBF"));
        return;
       }

    QString fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    QDbf::QDbfTable tabla;

    fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    // QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir subcta.dbf\n"));
        return;
       }
    // procesamos sbcta.dbf

    if (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       // if (record.isDeleted()) continue;
       QString cod;
       cod=record.value(0).toString().trimmed();
       QString cadnum; cadnum.setNum(cod.length());
       QMessageBox::information( this, tr("Importar C.plus"),
                                 tr("LONGITUD DE AUXILIARES: ")+cadnum);
      }
}

void import_cplus::ruta_pulsado()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->solo_directorios();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      QString fichero=dir->directorio_actual();
      if (fichero.endsWith(QDir::separator()))
             fichero=fichero.left(fichero.length()-1);
      ui->rutalineEdit->setText(fichero);
    }
  delete(dir);
#else

    QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory); // getenv("HOME")
    if (ui->rutalineEdit->text().isEmpty())
        dialog.setDirectory (eswindows() ? getenv("USERPROFILE"): getenv("HOME"));
      else
       dialog.setDirectory (adapta(ui->rutalineEdit->text()));
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();
       else return;
    QString fichero=fileNames.at(0);
    if (fichero.endsWith(QDir::separator()))
           fichero=fichero.left(fichero.length()-1);
    ui->rutalineEdit->setText(fichero);
#endif
}

void import_cplus::procesar()
{
    if (ui->ejercicioscomboBox->count()==0)
       {
        QMessageBox::warning( this, tr("Importar C.plus"),
                              tr("ERROR: No hay ejercicios definidos"));
        return;
       }
    if (ejerciciocerrado(ui->ejercicioscomboBox->currentText()))
       {
           QMessageBox::warning( this, tr("Importación"),
                         tr("ERROR, el ejercicio seleccionado está cerrado"));
           return;
       }

    ui->progressBar->setMaximum(100);

    QApplication::processEvents();

    bool log=ui->logcheckBox->isChecked();
    QString cadfich=ui->rutalineEdit->text();
    // obtener lista de archivos .dbf
    QDir dirplus(cadfich);
    QStringList filters;
    filters << "*.dbf" ;
    dirplus.setNameFilters(filters);
    QStringList ficheros=dirplus.entryList();

    QString abrir;


    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="BALAN.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta BALAN.DBF"));
        return;
       }

    QString fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir balan.dbf\n"));
        return;
       }
    // procesamos balan.dbf
    ui->logtextEdit->append(tr("CUENTAS DE BALANCE\n"));
    while (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       if (record.isDeleted()) continue;
       QString cta,descrip;
       /*
       for (int i=0; i< record.count();i++)
          {
            if (record.fieldName(i).startsWith("CTA")) cta=record.value(i).toString().trimmed();
            if (record.fieldName(i).startsWith("DESCRIP")) descrip=record.value(i).toString().trimmed();
          }
          */
        cta=record.value(3).toString().trimmed();
        descrip=record.value(2).toString().trimmed();

        if (!cta.isEmpty())
          {
           if (log) ui->logtextEdit->insertPlainText(cta + " - " + descrip + "\n");
           QString cadena;
           if (!existecodigoplan(cta,&cadena))
               insertaenplan(cta,descrip);
          }
       }

    ui->progressBar->setValue(20);

    QApplication::processEvents();

    // PROCESAR SUBCTA

    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="SUBCTA.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta SUBCTA.DBF"));
        return;
       }

    fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    // QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir subcta.dbf\n"));
        return;
       }
    // procesamos sbcta.dbf
    ui->logtextEdit->append(tr("CUENTAS AUXILIARES\n"));
    while (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       if (record.isDeleted()) continue;
       QString cod,titulo,nif,domicilio,poblacion,provincia,codpostal,telef01, fax01, email;
       cod=record.value(0).toString().trimmed();
       titulo=record.value(1).toString().trimmed();
       nif=record.value(2).toString().trimmed();
       domicilio=record.value(3).toString().trimmed();
       poblacion=record.value(4).toString().trimmed();
       provincia=record.value(5).toString().trimmed();
       codpostal=record.value(6).toString().trimmed();
       telef01=record.value(123).toString().trimmed();
       fax01=record.value(124).toString().trimmed();
       email=record.value(125).toString().trimmed();
       if (log) ui->logtextEdit->insertPlainText(cod + " - " + titulo + " " + telef01+ "\n") ;
       QString cad;
       if (!existecodigoplan(cod,&cad))
           insertaenplan(cod,titulo);

       if (!provincia.isEmpty() || !codpostal.isEmpty() || !nif.isEmpty())
         if (!basedatos::instancia()->existen_datos_accesorios(cod))
           basedatos::instancia()->insertadatosaccesorios_cplus(cod, titulo, nif,
                                             domicilio, poblacion,
                                             provincia, codpostal,
                                             telef01, fax01, email);


       }

    QApplication::processEvents();
/*
    // PROCESAR DEPART

    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="DEPART.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta DEPART.DBF"));
        return;
       }

    fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    // QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir depart.dbf\n"));
        return;
       }
    // procesamos depart.dbf
    ui->logtextEdit->append(tr("DEPARTAMENTOS\n"));
    while (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       if (record.isDeleted()) continue;
       QString codigo,descrip;
       codigo=record.value(0).toString().trimmed();
       descrip=record.value(1).toString().trimmed();
       if (log) ui->logtextEdit->insertPlainText(codigo + " - " + descrip + "\n") ;
       }

    QApplication::processEvents();


    // PROCESAR PROYEC

    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="PROYEC.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta PROYEC.DBF"));
        return;
       }

    fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    // QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir proyec.dbf\n"));
        return;
       }
    // procesamos proyec.dbf
    ui->logtextEdit->append(tr("PROYECTOS\n"));
    while (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       if (record.isDeleted()) continue;
       QString proye,descrip;
       proye=record.value(0).toString().trimmed();
       descrip=record.value(1).toString().trimmed();
       if (log) ui->logtextEdit->insertPlainText(proye + " - " + descrip + "\n") ;
       }
    */

    // QApplication::processEvents();


    // PROCESAR DIARIO

    ui->progressBar->setValue(40);

    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="DIARIO.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta DIARIO.DBF"));
        return;
       }

    fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    // QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir diario.dbf\n"));
        return;
       }
    // procesamos diario.dbf
    QStringList lista_documento, lista_subcta, lista_fecha, lista_debe, lista_haber, lista_cadpase;
    qlonglong vnumasiento=0;
    QString ejercicio=ui->ejercicioscomboBox->currentText();
    vnumasiento = basedatos::instancia()->proximoasiento(ejercicio);
    qlonglong vnumpase = basedatos::instancia()->selectProx_paseconfiguracion();
    if (vnumasiento==0 || vnumasiento==2) vnumasiento=1;
    if (vnumpase==0) vnumpase=1;

    QApplication::processEvents();

    ui->logtextEdit->append(tr("DIARIO\n"));

    // deberíamos corregir si en configuración no están definidas
    // las cuentas de IVA soportado/repercutido
    int pase=vnumpase;
    bool primer_registro=true;
    qlonglong asiento=0;
    qlonglong asientomax=0;
    qlonglong maxfactura=0;
    while (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       if (record.isDeleted()) continue;
       QString asien, fecha, subcta, contra, concepto, factura, documento, eurodebe, eurohaber;
       QString baseeuro, iva, rec, departa, clave;
       int prueba=record.value(0).toInt();
       QString cadnum; cadnum.setNum(prueba);
       // QMessageBox::warning( this, tr("Importar C.plus"),cadnum);
       asien=record.value(0).toString().trimmed();
       fecha=record.value(1).toString().trimmed();
       subcta=record.value(2).toString().trimmed();
       contra=record.value(3).toString().trimmed();
       concepto=record.value(5).toString().trimmed();
       //concepto.remove('�');
       concepto.remove(QChar(0xFFFD));
       factura=record.value(7).toString().trimmed();
       documento=record.value(11).toString().trimmed();
       eurodebe=convapunto(record.value(27).toString().trimmed());
       eurohaber=convapunto(record.value(28).toString().trimmed());
       baseeuro=convapunto(record.value(29).toString().trimmed());
       iva=convapunto(record.value(9).toString().trimmed());
       rec=convapunto(record.value(10).toString().trimmed());
       // QMessageBox::warning( this, tr("Importar C.plus"),asien + "debe-haber"+eurodebe + "**" + eurohaber);
       departa=record.value(12).toString().trimmed();
       clave=record.value(13).toString().trimmed();
       QDate fecha_asiento; fecha_asiento=fecha_asiento.fromString(fecha,"yyyy-MM-dd");
       if (primer_registro)
           if (ejerciciodelafecha(fecha_asiento)!=ui->ejercicioscomboBox->currentText())
              {
               QMessageBox::warning( this, tr("Importar C.plus"),
                                     tr("ERROR: la fecha no corresponde al ejercicio seleccionado"));
              return;

              }
       primer_registro=false;
       if (log) ui->logtextEdit->insertPlainText(fecha + " - " + subcta + " - " + eurodebe + " - " +
                                         eurohaber + " - " + concepto+ " - " + iva +"\n");
       /*void basedatos::insertDiario10 (QString cadnumasiento, QString cadnumpase, QString cadinicioej1,
                                       QString cuenta, QString debe, QString haber, QString concepto,
                                       QString documento, QString usuario, QString ejercicio) */
        asiento=vnumasiento+asien.toLongLong()-1;
        if (asiento>asientomax) asientomax=asiento;
        QString cadnumasiento; cadnumasiento.setNum(asiento);
        QString cadpase; cadpase.setNum(pase);
        // QStringList lista_documento, lista_subcta, lista_fecha, lista_debe, lista_haber,
        //   lista_cadpase;
        QString caddocumento=documento;
        /*
        if (factura.toInt()>0)
           {
            caddocumento+=(documento.isEmpty() ? factura : (" - " + factura));
           }*/
        if (factura.toLongLong()>maxfactura) maxfactura=factura.toLongLong();
        basedatos::instancia()->insertDiario_ic(cadnumasiento,cadpase,fecha, subcta, eurodebe, eurohaber,
                                               concepto, caddocumento, "", ejercicio,factura);
        lista_subcta << subcta;
        lista_fecha << fecha;
        lista_documento <<  documento;
        lista_debe << eurodebe;
        lista_haber << eurohaber;
        lista_cadpase << cadpase;

        if (baseeuro.toDouble()>0.001 || baseeuro.toDouble()<-0.001)
          {
            /*void basedatos::insertLibroiva
            (QString pase, QString cta_base_iva, QString base_iva, QString clave_iva,
             QString tipo_iva, QString tipo_re, QString cuota_iva, QString cuenta_fra,
             QDate fecha_fra, bool soportado, bool aib, bool eib, QString prorrata,
             bool rectificativa, bool autofactura,
             QDate fechaop, QString claveop, QString bicoste,
             QString rectificada, int nfacturas, QString finicial, QString ffinal,
             bool autofactura_no_ue, bool pr_servicios_ue, bool binversion,
             bool opnosujeta, QString afecto, bool agrario,
             QString nombre, QString cif)*/
            QString cuota_iva=eurodebe;
            if (eurodebe.toDouble()<0.001 && eurodebe.toDouble()>-0.001)
                cuota_iva=eurohaber;
            QDate fechafra;
            bool soportado=false;
            if (escuentadeivasoportado(subcta)) soportado=true;
            if (soportado && eurodebe.toDouble()<0.001 && eurodebe.toDouble()>-0.001
                && !(eurohaber.toDouble()<0.001 && eurohaber.toDouble()>-0.001))
               {
                 double base=baseeuro.toDouble()*-1;
                 baseeuro.setNum(base,'f',2);
               }
            if (!soportado && eurohaber.toDouble()<0.001 && eurohaber.toDouble()>-0.001
                && !(eurodebe.toDouble()<0.001 && eurodebe.toDouble()>-0.001))
               {
                 double base=baseeuro.toDouble()*-1;
                 baseeuro.setNum(base,'f',2);
               }
            fechafra=fechafra.fromString(fecha,"yyyy-MM-dd");
            QDate fechaop; fechaop=fechaop.fromString(fecha,"yyyy-MM-dd");
            basedatos::instancia()->insertLibroiva(cadpase, "", baseeuro, "GN", iva, rec, cuota_iva,
                                                   contra,fechafra, soportado, false, false, "",
                                                   false, false, fechaop, "", "", "", 0, "","",
                                                   false, false, false, false, "", false,
                                                   "","",false,false,true,false,false);
          }

        pase++;

       }
    QString cadfac; cadfac.setNum(maxfactura+1);
    basedatos::instancia()->pasa_prox_nrecepcion_ejercicio(ejercicio,cadfac);
    // falta rectificar saldos
    // corregir último pase en configuración
    // corregir último asiento en ejercicios
    QString cadproxasiento; asientomax++; cadproxasiento.setNum(asientomax);
    basedatos::instancia()->update_ejercicio_prox_asiento(ejercicio, cadproxasiento);
    QString cadnumpase; cadnumpase.setNum(pase);
    basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
    // asignamos asiento de apertura
    basedatos::instancia()->asignadiarios_segun_concepto();

    ui->progressBar->setValue(60);

    QApplication::processEvents();

    // rectificamos saldos
    actualizasaldos();

    // PROCESAR VENCI
 if (ui->vencicheckBox->isChecked())
    {
    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="VENCI.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta VENCI.DBF"));
        return;
       }

    fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    // QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir venci.dbf\n"));
        return;
       }
    qlonglong proxvenci=basedatos::instancia()->proximovencimiento();
    if (proxvenci==0) proxvenci=1;
    // procesamos venci.dbf
    ui->logtextEdit->append(tr("VENCIMIENTOS\n"));
    if (tabla.size()>0)
      while (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       if (record.isDeleted()) continue;
       QString fecha, fecha_pag, cod, acpa, contra, concepto, estado, documento, euro;
       /* for (int i=0; i< record.count();i++)
          {
            if (record.fieldName(i)=="FECHA") fecha=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="COD") cod=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="ACPA") acpa=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="CONTRA") contra=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="CONCEPTO") concepto=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="ESTADO") estado=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="DOCUMENTO") documento=record.value(i).toString().trimmed();
          } */
       fecha=record.value(0).toString().trimmed();
       cod=record.value(1).toString().trimmed();  // cuenta de cliente/proveedor
       acpa=record.value(2).toString().trimmed(); // P o G =Pago, A o I= Cobro
       contra=record.value(3).toString().trimmed(); // cuenta de tesorería
       concepto=record.value(4).toString().trimmed(); // concepto al vencimiento ?
       estado=record.value(8).toString().trimmed(); // verdadero si procesado
       documento=record.value(9).toString().trimmed();
       fecha_pag=record.value(14).toString().trimmed(); // fecha de pago
       euro=convapunto(record.value(18).toString()).trimmed(); // importe
       if (log) ui->logtextEdit->insertPlainText(fecha + " - " + cod + " - " + acpa + " - " +
                                         contra + " - " + concepto+ " - " + estado +
                                         " - " + documento + "\n");
       // QStringList lista_documento, lista_subcta, lista_fecha, lista_debe, lista_haber,
       //   lista_cadpase;
       //
       // tabla vencimientos
       // ---------------------------------------------------
       // "num                     bigint,"
       // "cta_ordenante           varchar(40),"
       // "fecha_operacion         date,"
       // "importe                 numeric(14,2),"
       // "cuenta_tesoreria        varchar(40),"
       // "fecha_vencimiento       date,"
       // "derecho                 bool,"
       // "pendiente               bool,"
       // "anulado                 bool,"
       // "pase_diario_operacion   bigint,"
       // "pase_diario_vencimiento bigint,"
       // "concepto                varchar(160),"
       // "usuario                 varchar(80),"

       // tratamos de localizar apunte de la operación origen del vencimiento
       QString pase_documento;
       QString pase_nodocumento;
       QString pase_documento_proc;
       QString pase_nodocumento_proc;
       bool pago=true;
       if (acpa.toUpper().contains('A') || acpa.toUpper().contains('I')) pago=false;
       // ui->logtextEdit->insertPlainText(estado );
       double valorvenci=euro.toDouble();
       for (int veces=0; veces<lista_documento.count(); veces++)
          {
           if (!(lista_subcta.at(veces)==cod)) continue;
           bool espagoendiario=false;
           QString enlistahaber=lista_haber.at(veces);
           QString enlistadebe=lista_debe.at(veces);
           double valor_diario=lista_debe.at(veces).toDouble()+lista_haber.at(veces).toDouble();
           if (valor_diario < 0) valor_diario=valor_diario*-1;
           double diferencia=valorvenci-valor_diario;
           bool igual_valores=false;
           if (diferencia<0.001 && diferencia>-0.001) igual_valores=true;
           if (enlistahaber.toDouble()>0.001 || enlistadebe.toDouble()<-0.001)
               espagoendiario=true;

           if ((pago && espagoendiario) || (!pago && !espagoendiario))
             {
               if (lista_fecha.at(veces)==fecha && igual_valores)
                 pase_nodocumento=lista_cadpase.at(veces);
              // ui->logtextEdit->insertPlainText("iguales a venci");
              // if (lista_fecha.at(veces)==fecha) pase_nodocumento=lista_cadpase.at(veces);
              if (!documento.isEmpty())
               {
                if (lista_fecha.at(veces)==fecha && lista_documento.at(veces)==documento)
                  pase_documento=lista_cadpase.at(veces);
               }
                else // buscamos coincidencia por valor
                    {
                    if (lista_fecha.at(veces)==fecha && igual_valores)
                      pase_documento=lista_cadpase.at(veces);
                    }

             }
           if (((!pago && espagoendiario) || (pago && !espagoendiario)) && estado=="true")
             {
              // if (lista_fecha.at(veces)==fecha_pag) pase_nodocumento_proc=lista_cadpase.at(veces);
               if (lista_fecha.at(veces)==fecha_pag && igual_valores)
                  pase_nodocumento_proc=lista_cadpase.at(veces);
              if (!documento.isEmpty())
               {
                if (lista_fecha.at(veces)==fecha_pag && lista_documento.at(veces)==documento)
                  pase_documento_proc=lista_cadpase.at(veces);
               }
                else
                    {
                     if (lista_fecha.at(veces)==fecha_pag && igual_valores)
                        pase_documento_proc=lista_cadpase.at(veces);
                    }
              }
          }
       ui->progressBar->setValue(80);

       QApplication::processEvents();

        /*void basedatos::insertVencimientos ( QString num, QString cta_ordenante,
                                            QDate fecha_operacion,
                                            QString importe, QString cuenta_tesoreria,
                                            QDate fecha_vencimiento, bool derecho, bool pendiente,
                                            bool anulado, QString pase_diario_operacion,
                                            QString pase_diario_vencimiento, QString concepto) */
        QString cadproxvenci; cadproxvenci.setNum(proxvenci);
        QDate fecha_operacion; fecha_operacion=fecha_operacion.fromString(fecha,"yyyy-MM-dd");
        QDate fecha_vencimiento;
        fecha_vencimiento=fecha_vencimiento.fromString(fecha_pag,"yyyy-M-dd");
        bool pendiente;
        // pendiente=(pase_documento_proc.isEmpty() && pase_nodocumento_proc.isEmpty());
        pendiente=pase_documento_proc.isEmpty();
        QString pase_diario_operacion=pase_documento;
        if (pase_diario_operacion.isEmpty()) pase_diario_operacion=pase_nodocumento;
        QString pase_diario_vencimiento=pase_documento_proc;
        if (pase_diario_vencimiento.isEmpty()) pase_diario_vencimiento=pase_documento_proc;
        if (!pase_diario_operacion.isEmpty())
          {
           if (euro.toDouble()<0) euro.setNum(-1*euro.toDouble(),'f',2);
           basedatos::instancia()->insertVencimientos(cadproxvenci,cod,fecha_operacion,euro,contra,
                                                   fecha_vencimiento, !pago, pendiente,
                                                   false, pase_diario_operacion,
                                                   pase_diario_vencimiento, concepto);
           proxvenci++;
          }
       }
    QString cproxvenci; cproxvenci.setNum(proxvenci);
    basedatos::instancia()->updateConfiguracionprox_vencimiento(cproxvenci);
   }

 QApplication::processEvents();

 // PROCESAR AMOINV
  if (ui->amortcheckBox->isChecked())
   {
    for (int veces=0; veces<ficheros.count(); veces++)
        if (ficheros.at(veces).toUpper()=="AMOINV.DBF") abrir=ficheros.at(veces);

    if (abrir.isEmpty())
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: falta AMOINV.DBF"));
        return;
       }

    fichdbf=cadfich;
    fichdbf.append(QDir::separator());
    fichdbf+=abrir;
    // QDbf::QDbfTable tabla;
    if (!tabla.open(fichdbf))
       {
        QMessageBox::warning( this, tr("Importar C.plus"),tr("ERROR: no se puede abrir amoinv.dbf\n"));
        return;
       }
    // procesamos amoinv.dbf
    ui->logtextEdit->append(tr("PLAN DE AMORTIZACIONES\n"));
    if (tabla.size()>0)
      while (tabla.next())
      {
       QDbf::QDbfRecord record=tabla.record();
       if (record.isDeleted()) continue;
       QString fecpriam, fechacomp, subctaam, subctado, tpca, concepto;
       QString fecultam, impcoeuro, impameuro, factura, proveedor;
       // fecpriam, fecha primera amortización
       // fechacomp, fecha de compra
       // subctaam, cuenta de amortización acumulada
       // subctado, cuenta de amortización gasto
       // tpca, coeficiente amortización anual
       /*for (int i=0; i< record.count();i++)
          {
            if (record.fieldName(i)=="FECPRIAM") fecpriam=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="SUBCTAAM") subctaam=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="SUBCTADO") subctado=record.value(i).toString().trimmed();
            if (record.fieldName(i)=="TPCA") tpca=record.value(i).toString().trimmed();
          }*/
       fechacomp=record.value(2).toString().trimmed();
       fecpriam=record.value(3).toString().trimmed();
       subctaam=record.value(10).toString().trimmed();
       subctado=record.value(11).toString().trimmed();
       tpca=convapunto(record.value(14).toString().trimmed());
       concepto=record.value(7).toString().trimmed();
       fecultam=record.value(13).toString().trimmed();
       impcoeuro=convapunto(record.value(22).toString().trimmed());
       impameuro=convapunto(record.value(21).toString().trimmed());
       factura=record.value(6).toString().trimmed();
       proveedor=record.value(16).toString().trimmed();
       if (log) ui->logtextEdit->insertPlainText(fechacomp+ " - " + fecpriam + " - " + subctaam + " - " + subctado + " - " +
                                         tpca + "\n");
       /*"insert into amoinv (cta_aa, cta_ag ,concepto,fecha_inicio, fecha_compra,"
          "fecha_ult_amort, coef, valor_adq, valor_amortizado, factura, proveedor)*/
       QDate fecha_inicio; fecha_inicio=fecha_inicio.fromString(fecpriam,"yyyy-MM-dd");
       QDate fecha_compra; fecha_compra=fecha_compra.fromString(fechacomp,"yyyy-MM-dd");
       if (fecpriam.isEmpty()) fecha_inicio=fecha_compra;
       QDate fecha_ult_amort; fecha_ult_amort=fecha_ult_amort.fromString(fecultam,"yyyy-MM-dd");
       if (fecultam.isEmpty()) fecha_ult_amort=fecha_compra;
       basedatos::instancia()->inserta_item_inmov(subctaam,
                                          subctado,
                                          concepto,
                                          fecha_inicio,
                                          fecha_compra,
                                          fecha_ult_amort,
                                          tpca,
                                          impcoeuro,
                                          impameuro,
                                          factura,
                                          proveedor);
       }
  }
   // void basedatos::updateConfiguracionprox_vencimiento (QString cadnumpase)
   // proxpase, proxasiento

  ui->progressBar->setValue(100);

  QApplication::processEvents();
  QMessageBox::information( this, tr("Importación"),
                tr("Se han concluido las operaciones"));

}
