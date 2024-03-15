#include "informe_tipo_proveedor.h"
#include "ui_informe_tipo_proveedor.h"
#include "basedatos.h"
#include <QMessageBox>
#include <qtrpt.h>
#include "funciones.h"

informe_tipo_proveedor::informe_tipo_proveedor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::informe_tipo_proveedor)
{
    ui->setupUi(this);
    cargacombos_tipoProveedor();

    ui->fecha_sin_mov_dateEdit->setDate(QDate::currentDate());
    connect(ui->solo_activo_checkBox,SIGNAL(clicked(bool)),SLOT(solo_activo_check()));
    connect(ui->solo_no_activo_checkBox,SIGNAL(clicked(bool)),SLOT(solo_no_activo_check()));
    connect(ui->imprimir_pushButton,SIGNAL(clicked(bool)),SLOT(boton_informe_horizontal()));
    connect(ui->imprime_ver_pushButton,SIGNAL(clicked(bool)),SLOT(boton_informe_vertical()));
    connect(ui->copiar_pushButton,SIGNAL(clicked(bool)),SLOT(copiar()));
    connect(ui->sin_mov_checkBox,SIGNAL(stateChanged(int)),SLOT(check_sin_mov()));
}

informe_tipo_proveedor::~informe_tipo_proveedor()
{
    delete ui;
}

void informe_tipo_proveedor::cargacombos_tipoProveedor()
{
  QStringList lista1=basedatos::instancia()->tipos_proveedor();
  QStringList lista2=basedatos::instancia()->homologaciones();
  QStringList lista3=basedatos::instancia()->actividades();

  ui->tipo_proveedor_comboBox->addItem("- -");
  ui->homologacion_comboBox->addItem("- -");
  ui->actividad_comboBox->addItem("- -");

  ui->tipo_proveedor_comboBox->addItems(lista1);
  ui->homologacion_comboBox->addItems(lista2);
  ui->actividad_comboBox->addItems(lista3);

}

void informe_tipo_proveedor::boton_informe_horizontal() {
    if (!ui->sin_mov_checkBox->isChecked()) genera(0);
       else
        inf_ctas_sin_mov(0);
}

void informe_tipo_proveedor::boton_informe_vertical() {
    if (!ui->sin_mov_checkBox->isChecked()) genera(1);
      else
        inf_ctas_sin_mov(1);
}

void informe_tipo_proveedor::copiar() {
    if (!ui->sin_mov_checkBox->isChecked()) genera(2);
      else
        inf_ctas_sin_mov(2);
}

QString informe_tipo_proveedor::cad_filtro_lista_cuentas() {
    QString filtro;
    QString cadenacuentas=ui->cuentas_lineEdit->text();
    if (!cadenacuentas.isEmpty())
      {
       filtro += "(";

       int partes = cadenacuentas.count(',');

       for (int veces=0; veces<=partes; veces++)
        {
          if (basedatos::instancia()->cualControlador() == basedatos::instancia()->SQLITE) {
            filtro += "cuenta like '"+ cadenacuentas.section(',',veces,veces).left(-1).replace("'","''") +"%'";
            }
           else {
            filtro += "position('";
            filtro += cadenacuentas.section(',',veces,veces).left(-1).replace("'","''");
            filtro += "' in cuenta)=1";
            }
          if (veces==partes) filtro+=") ";
            else filtro+=" or ";
        }
      }
    return filtro;
}

QString informe_tipo_proveedor::cad_filtro_checks() {
    QString filtro;

    //cadena+="cuenta,razon,cif,domicilio,codigopostal,poblacion, pronvicia, ";
    //cadena+="tipo_proveedor, "; //varchar(40),";
    if (ui->tipo_proveedor_comboBox->currentIndex()>0) {
        if (! filtro.isEmpty()) filtro+=" and";
        filtro+=" tipo_proveedor='"+ui->tipo_proveedor_comboBox->currentText().section('-',0,0)+"'";
    }

    //cadena+="homologacion, "; // varchar(40),";
    if (ui->homologacion_comboBox->currentIndex()>0) {
        if (! filtro.isEmpty()) filtro+=" and";
        filtro+= " homologacion='"+ui->homologacion_comboBox->currentText().section('-',0,0)+"'";
    }

    //cadena+="actividad, "; //varchar(40),";
    if (ui->actividad_comboBox->currentIndex()>0) {
        if (! filtro.isEmpty()) filtro+=" and";
        filtro+=" actividad='"+ui->actividad_comboBox->currentText().section('-',0,0)+"'";
    }

    //cadena+="fecha_homologacion, "; // date,";

    //cadena += "proveedor_activo, "; // bool default 0,";
    if (ui->solo_activo_checkBox->isChecked()) {
        if (! filtro.isEmpty()) filtro+=" and";
        filtro+=" proveedor_activo";
    }

    if (ui->solo_no_activo_checkBox->isChecked()) {
        if (! filtro.isEmpty()) filtro+=" and";
        filtro+=" not proveedor_activo";
    }

    //cadena += "acceso_lopd, "; // bool default 0,";
        if (ui->acceso_lopd_checkBox->isChecked()) {
            if (! filtro.isEmpty()) filtro+=" and";
            filtro+=" acceso_lopd ";
        }
               // else filtro+=" and not acceso_lopd ";
    //cadena += "cumpli_lopd,"; //  bool default 0,";
        if (ui->cumplimiento_lopd_checkBox->isChecked()) {
            if (! filtro.isEmpty()) filtro+=" and";
            filtro+=" cumpli_lopd ";
        }
               // else filtro+=" and not cumpli_lopd ";
    //cadena += "acceso_prl, "; //bool default 0,";
        if (ui->acceso_prl_checkBox->isChecked()) {
            if (! filtro.isEmpty()) filtro+=" and";
            filtro+= " acceso_prl";
        }
               // else filtro+=filtro+= " and not acceso_prl";
    //cadena += "coord_prl, "; // bool default 0,";
        if (ui->coord_prl_checkBox->isChecked()) {
            if (! filtro.isEmpty()) filtro+=" and";
            filtro+= " coord_prl";
        }
               // else filtro+= " and not coord_prl";
    //cadena += "sol_cert_ss, "; // bool default 0,";
        if (ui->cert_ss_checkBox->isChecked()) {
            if (! filtro.isEmpty()) filtro+=" and";
            filtro+= " sol_cert_ss";
        }
               // else filtro+= " and not sol_cert_ss";
    //cadena += "sol_cert_hda, "; // bool default 0,";
        if (ui->cert_hda_checkBox->isChecked()) {
            if (! filtro.isEmpty()) filtro+=" and";
            filtro+=" sol_cert_hda";
        }
               // else filtro+=" and not sol_cert_hda";
    //cadena += "no_correo_pub "; // bool default 0,";
        if (ui->no_correo_pub_checkBox->isChecked()) {
            if (! filtro.isEmpty()) filtro+=" and";
            filtro+= " no_correo_pub";
        }

   return filtro;
}

void informe_tipo_proveedor::inf_ctas_sin_mov(int tipo) {
    // primero determinamos lista de cuentas y fecha máxima de movimiento
    QList<QDate> fechas;
    QStringList cuentas;
    QSqlQuery q=basedatos::instancia()->cuenta_max_fecha_diario(cad_filtro_lista_cuentas());
    if (q.isActive())
        while (q.next()) {
            // solo pasamos registros con fecha inferior a la de referencia
            if (q.value(1).toDate()<ui->fecha_sin_mov_dateEdit->date()) {
               cuentas<<q.value(0).toString();
               fechas<<q.value(1).toDate();
            }
        }
    // obtenemos lista para el informe
    QString filtro=cad_filtro_checks();

    QStringList cuenta,razon,cif,domicilio,codigopostal,poblacion, provincia;
    QStringList tipo_proveedor;
    QStringList homologacion;
    QStringList actividad;
    QStringList fecha_homologacion;
    QStringList proveedor_activo;
    QStringList acceso_lopd;
    QStringList cumpli_lopd;
    QStringList acceso_prl;
    QStringList coord_prl;
    QStringList sol_cert_ss;
    QStringList sol_cert_hda;
    QStringList no_correo_pub;
    QStringList saldo;

    QStringList fecha_ul_mov;

    int pos=0;
    while (pos<cuentas.count()) {
        QSqlQuery q = basedatos::instancia()->lista_datos_tipo_proveedor_cta(filtro, cuentas.at(pos));
        if (q.isActive())
            if (q.next()) {
                cuenta << q.value(0).toString();
                razon << q.value(1).toString();
                cif << q.value(2).toString();
                domicilio << q.value(3).toString();
                codigopostal << q.value(4).toString();
                poblacion << q.value(5).toString();
                provincia << q.value(6).toString();
                tipo_proveedor << q.value(7).toString();
                homologacion << q.value(8).toString();
                actividad << q.value(9).toString();
                fecha_homologacion << q.value(10).toDate().toString("dd-MM-yyyy");
                proveedor_activo << (q.value(11).toBool() ? "X" : "");
                acceso_lopd << (q.value(12).toBool() ? "X" : "");
                cumpli_lopd << (q.value(13).toBool() ? "X" : "");
                acceso_prl << (q.value(14).toBool() ? "X" : "");
                coord_prl << (q.value(15).toBool() ? "X" : "");
                sol_cert_ss << (q.value(16).toBool() ? "X" : "");
                sol_cert_hda << (q.value(17).toBool() ? "X" : "");
                no_correo_pub << (q.value(18).toBool() ? "X" : "");

                fecha_ul_mov<<fechas.at(pos).toString("dd-MM-yyyy");
                saldo << formateanumerosep(saldocuentaendiario(cuentas.at(pos)),haycomadecimal(),haydecimales());
            }
          pos++;
    }

    QString contenido;
    if (ui->tipo_proveedor_comboBox->currentIndex()>0) contenido=ui->tipo_proveedor_comboBox->currentText()+"\n";
    if (ui->homologacion_comboBox->currentIndex()>0) contenido+=ui->homologacion_comboBox->currentText()+"\n";
    if (ui->actividad_comboBox->currentIndex()>0) contenido+=ui->actividad_comboBox->currentText();

    // copiar
    if (tipo==2) {
        QString cadena;
        cadena+=basedatos::instancia()->selectEmpresaconfiguracion();
        cadena+="\n";
        cadena+=contenido;
        cadena+="\n";
        cadena+="\n";
        cadena+="CUENTA\tNOMBRE/RAZÓN\tNIF\tÚlt.Apunte\tSaldo\tDIRECCIÓN\tCP\tPOBLACIÓN\tPROVINCIA\t"
                "LOPD\tContrato LOPD\tAcceso PRL\tCoord.PRL\tCertificado SS\tCertif.AEAT\tNo correo publ\n";
        for (int recNo=0;recNo<cuenta.count();recNo++) {
          cadena+= cuenta.at(recNo);
          cadena+="\t";
          cadena+= razon.at(recNo);
          cadena+="\t";
          cadena+= cif.at(recNo);
          cadena+="\t";
          cadena+= fecha_ul_mov.at(recNo);
          cadena+="\t";
          cadena+= saldo.at(recNo);
          cadena+="\t";
          cadena+= domicilio.at(recNo);
          cadena+="\t";
          cadena+= codigopostal.at(recNo);
          cadena+="\t";
          cadena+= poblacion.at(recNo);
          cadena+="\t";
          cadena+= provincia.at(recNo);
          cadena+="\t";
          cadena+= acceso_lopd.at(recNo);
          cadena+="\t";
          cadena+= cumpli_lopd.at(recNo);
          cadena+="\t";
          cadena+= acceso_prl.at(recNo);
          cadena+="\t";
          cadena+= coord_prl.at(recNo);
          cadena+="\t";
          cadena+= sol_cert_ss.at(recNo);
          cadena+="\t";
          cadena+= sol_cert_hda.at(recNo);
          cadena+="\t";
          cadena+= no_correo_pub.at(recNo);
          cadena+="\n";
        }
        QClipboard *cb = QApplication::clipboard();
        cb->setText(cadena);
        QMessageBox::information( this, tr("Tipos de Proveedor"),
                      tr("Se ha pasado el contenido al portapales"));
        return;
    }
    // pasamos al informe

    QString fileName;
    fileName =  ":/informes/tipo_proveedor_actividad.xml" ;
    if (tipo==1) fileName =  ":/informes/tipo_proveedor_actividad_vertical.xml" ;
    QtRPT *report = new QtRPT(this);
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("INFORME"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=cuenta.count();;
    });

    QString imagen=basedatos::instancia()->logo_empresa();

      connect(report, &QtRPT::setValueImage,[&](const int recNo, const QString paramName, QImage &paramValue, const int reportPage) {
        Q_UNUSED(recNo);
        Q_UNUSED(reportPage);
        if (paramName == "logo") {

            auto foto= new QImage();
            if (imagen.isEmpty()) imagen = logodefecto();
            if (imagen.length()>0)
               {
                QByteArray byteshexa;
                byteshexa.append ( imagen.toUtf8() );
                QByteArray bytes;
                bytes=bytes.fromHex ( byteshexa );
                foto->loadFromData( bytes, "PNG");
               } else return;

            paramValue = *foto;
        }
    });


    connect(report, &QtRPT::setValue, [&](const int recNo, const QString paramName, QVariant &paramValue, const int reportPage) {
        // campos: descrip, nota, cifra1, cifra2
        // recordCount es una lista, añadir también segunda página

        // QMessageBox::warning( this, tr("Estados Contables"),tr("Num %1").arg(recNo));
        Q_UNUSED(reportPage);
        if (paramName == "contenido") paramValue = contenido;

        if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();

        if (paramName == "cuenta") paramValue = cuenta.at(recNo);

        if (paramName == "razon") paramValue = razon.at(recNo);

        if (paramName == "nif") paramValue = cif.at(recNo);
        if (paramName == "direccion" ) paramValue = domicilio.at(recNo);
        if (paramName == "cp" ) paramValue = codigopostal.at(recNo);
        if (paramName == "saldo" ) paramValue = saldo.at(recNo);
        if (paramName == "fechaul" ) paramValue =fecha_ul_mov.at(recNo);
        if (paramName == "poblacion" ) paramValue = poblacion.at(recNo);
        if (paramName == "provincia" ) paramValue = provincia.at(recNo);
        if (paramName == "lopd" ) paramValue = acceso_lopd.at(recNo);
        if (paramName == "clopd" ) paramValue = cumpli_lopd.at(recNo);
        if (paramName == "prl" ) paramValue = acceso_prl.at(recNo);
        if (paramName == "cprl" ) paramValue = coord_prl.at(recNo);
        if (paramName == "css" ) paramValue = sol_cert_ss.at(recNo);
        if (paramName == "caeat" ) paramValue = sol_cert_hda.at(recNo);
        if (paramName == "nopub" ) paramValue = no_correo_pub.at(recNo);


    });


    //report->printExec(true);
    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+(tr("informe_tipo_proveedor.pdf"));
           report->printPDF(qfichero);
    }
}

void informe_tipo_proveedor::genera(int tipo) {
    QString filtro=cad_filtro_lista_cuentas()+cad_filtro_checks();
    //QMessageBox::information( this, tr("tipo proveedor"),filtro);

    QStringList cuenta,razon,cif,domicilio,codigopostal,poblacion, provincia;
    QStringList tipo_proveedor;
    QStringList homologacion;
    QStringList actividad;
    QStringList fecha_homologacion;
    QStringList proveedor_activo;
    QStringList acceso_lopd;
    QStringList cumpli_lopd;
    QStringList acceso_prl;
    QStringList coord_prl;
    QStringList sol_cert_ss;
    QStringList sol_cert_hda;
    QStringList no_correo_pub;
    QSqlQuery q = basedatos::instancia()->lista_datos_tipo_proveedor_cta(filtro);
    if (q.isActive())
        while (q.next()) {
            cuenta << q.value(0).toString();
            razon << q.value(1).toString();
            cif << q.value(2).toString();
            domicilio << q.value(3).toString();
            codigopostal << q.value(4).toString();
            poblacion << q.value(5).toString();
            provincia << q.value(6).toString();
            tipo_proveedor << q.value(7).toString();
            homologacion << q.value(8).toString();
            actividad << q.value(9).toString();
            fecha_homologacion << q.value(10).toDate().toString("dd-MM-yyyy");
            proveedor_activo << (q.value(11).toBool() ? "X" : "");
            acceso_lopd << (q.value(12).toBool() ? "X" : "");
            cumpli_lopd << (q.value(13).toBool() ? "X" : "");
            acceso_prl << (q.value(14).toBool() ? "X" : "");
            coord_prl << (q.value(15).toBool() ? "X" : "");
            sol_cert_ss << (q.value(16).toBool() ? "X" : "");
            sol_cert_hda << (q.value(17).toBool() ? "X" : "");
            no_correo_pub << (q.value(18).toBool() ? "X" : "");
        }

    QString contenido;
    if (ui->tipo_proveedor_comboBox->currentIndex()>0) contenido=ui->tipo_proveedor_comboBox->currentText()+"\n";
    if (ui->homologacion_comboBox->currentIndex()>0) contenido+=ui->homologacion_comboBox->currentText()+"\n";
    if (ui->actividad_comboBox->currentIndex()>0) contenido+=ui->actividad_comboBox->currentText();

    // copiar
    if (tipo==2) {
        QString cadena;
        cadena+=basedatos::instancia()->selectEmpresaconfiguracion();
        cadena+="\n";
        cadena+=contenido;
        cadena+="\n";
        cadena+="\n";
        cadena+="CUENTA\tNOMBRE/RAZÓN\tNIF\tDIRECCIÓN\tCP\tPOBLACIÓN\tPROVINCIA\t"
                "LOPD\tContrato LOPD\tAcceso PRL\tCoord.PRL\tCertificado SS\tCertif.AEAT\tNo correo publ\n";
        for (int recNo=0;recNo<cuenta.count();recNo++) {
          cadena+= cuenta.at(recNo);
          cadena+="\t";
          cadena+= razon.at(recNo);
          cadena+="\t";
          cadena+= cif.at(recNo);
          cadena+="\t";
          cadena+= domicilio.at(recNo);
          cadena+="\t";
          cadena+= codigopostal.at(recNo);
          cadena+="\t";
          cadena+= poblacion.at(recNo);
          cadena+="\t";
          cadena+= provincia.at(recNo);
          cadena+="\t";
          cadena+= acceso_lopd.at(recNo);
          cadena+="\t";
          cadena+= cumpli_lopd.at(recNo);
          cadena+="\t";
          cadena+= acceso_prl.at(recNo);
          cadena+="\t";
          cadena+= coord_prl.at(recNo);
          cadena+="\t";
          cadena+= sol_cert_ss.at(recNo);
          cadena+="\t";
          cadena+= sol_cert_hda.at(recNo);
          cadena+="\t";
          cadena+= no_correo_pub.at(recNo);
          cadena+="\n";
        }
        QClipboard *cb = QApplication::clipboard();
        cb->setText(cadena);
        QMessageBox::information( this, tr("Tipos de Proveedor"),
                      tr("Se ha pasado el contenido al portapales"));
        return;
    }
    // pasamos al informe

    QString fileName;
    fileName =  ":/informes/tipo_proveedor.xml" ;
    if (tipo==1) fileName =  ":/informes/tipo_proveedor_vertical.xml" ;
    QtRPT *report = new QtRPT(this);
    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("INFORME"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    QObject::connect(report, &QtRPT::setDSInfo,
                     [&](DataSetInfo &dsInfo) {
        dsInfo.recordCount=cuenta.count();;
    });

    QString imagen=basedatos::instancia()->logo_empresa();

      connect(report, &QtRPT::setValueImage,[&](const int recNo, const QString paramName, QImage &paramValue, const int reportPage) {
        Q_UNUSED(recNo);
        Q_UNUSED(reportPage);
        if (paramName == "logo") {

            auto foto= new QImage();
            if (imagen.isEmpty()) imagen = logodefecto();
            if (imagen.length()>0)
               {
                QByteArray byteshexa;
                byteshexa.append ( imagen.toUtf8() );
                QByteArray bytes;
                bytes=bytes.fromHex ( byteshexa );
                foto->loadFromData( bytes, "PNG");
               } else return;

            paramValue = *foto;
        }
    });


    connect(report, &QtRPT::setValue, [&](const int recNo, const QString paramName, QVariant &paramValue, const int reportPage) {
        // campos: descrip, nota, cifra1, cifra2
        // recordCount es una lista, añadir también segunda página

        // QMessageBox::warning( this, tr("Estados Contables"),tr("Num %1").arg(recNo));
        Q_UNUSED(reportPage);
        if (paramName == "contenido") paramValue = contenido;

        if (paramName == "empresa") paramValue = basedatos::instancia()->selectEmpresaconfiguracion();

        if (paramName == "cuenta") paramValue = cuenta.at(recNo);

        if (paramName == "razon") paramValue = razon.at(recNo);

        if (paramName == "nif") paramValue = cif.at(recNo);
        if (paramName == "direccion" ) paramValue = domicilio.at(recNo);
        if (paramName == "cp" ) paramValue = codigopostal.at(recNo);
        if (paramName == "poblacion" ) paramValue = poblacion.at(recNo);
        if (paramName == "provincia" ) paramValue = provincia.at(recNo);
        if (paramName == "lopd" ) paramValue = acceso_lopd.at(recNo);
        if (paramName == "clopd" ) paramValue = cumpli_lopd.at(recNo);
        if (paramName == "prl" ) paramValue = acceso_prl.at(recNo);
        if (paramName == "cprl" ) paramValue = coord_prl.at(recNo);
        if (paramName == "css" ) paramValue = sol_cert_ss.at(recNo);
        if (paramName == "caeat" ) paramValue = sol_cert_hda.at(recNo);
        if (paramName == "nopub" ) paramValue = no_correo_pub.at(recNo);


    });


    //report->printExec(true);
    if (hay_visualizador_extendido) report->printExec(true);
    else {
           QString qfichero=dirtrabajo();
           qfichero.append(QDir::separator());
           qfichero=qfichero+(tr("informe_tipo_proveedor.pdf"));
           report->printPDF(qfichero);
    }


}


void informe_tipo_proveedor::solo_activo_check() {
    if (ui->solo_activo_checkBox->isChecked()) ui->solo_no_activo_checkBox->setChecked(false);
}

void informe_tipo_proveedor::solo_no_activo_check() {
    if (ui->solo_no_activo_checkBox->isChecked()) ui->solo_activo_checkBox->setChecked(false);
}

void informe_tipo_proveedor::check_sin_mov() {
    ui->fecha_sin_mov_dateEdit->setEnabled(ui->sin_mov_checkBox->isChecked());
}
