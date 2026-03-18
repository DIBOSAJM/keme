#include "emitida_exenta_aut.h"
#include "aux_express.h"
#include "busca_externo.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "ui_emitida_exenta_aut.h"

Emitida_exenta_aut::Emitida_exenta_aut(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Emitida_exenta_aut)
{
    ui->setupUi(this);

    etiqueta = new CustomQLabel;
    etiqueta->setHidden(false);
    etiqueta->setMinimumWidth(150);
    etiqueta->setMaximumWidth(150);
    etiqueta->setMinimumHeight(60);
    etiqueta->setText(tr("Soltar archivo aquí"));
    etiqueta->setAlignment(Qt::AlignHCenter);
    etiqueta->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    ui->horizontalLayout_10->addWidget(etiqueta);

    cargacombooperaciones();
    ui->tipo_rectificativa_comboBox->addItems(tipos_rectificativas);
    ui->donacion_groupBox->hide();
    ui->FechafradateEdit->setDate(QDate().currentDate());
    ui->actividad_comboBox->addItems(basedatos::instancia()->activ_ecas());

    connect(etiqueta, SIGNAL(filedroped()),SLOT(fichero_soltado()));


}

Emitida_exenta_aut::~Emitida_exenta_aut()
{
    delete ui;
}

QJsonObject Emitida_exenta_aut::info_export()
{
    // QJsonObject reg_donacion;
    // reg_donacion.insert("clave",ui.clave_donacion_comboBox->currentText().left(1));
    // reg_donacion.insert("especie",ui.donacion_especie_checkBox->isChecked());
    // reg_donacion.insert("recurrente",ui.recurrente_checkBox->isChecked());
    // reg_donacion.insert("ca",ui.ca_comboBox->currentText().left(2));
    // reg_donacion.insert("deduccion_ca",convapunto(ui.porcentaje_deduccion_ca_lineEdit->text()).toDouble());
    QJsonObject info;
    info.insert("cuenta_base",ui->CtabaselineEdit->text());
    info.insert("base_imponible",ui->baselineEdit->text());
    info.insert("fecha_fra",ui->FechafradateEdit->date().toString("yyyy-MM-dd"));
    info.insert("fecha_cont",ui->fecha_cont_dateEdit->date().toString("yyyy-MM-dd"));
    info.insert("fecha_op",ui->fechaoperaciondateEdit->date().toString("yyyy-MM-dd"));
    info.insert("cuenta_factura",ui->CtafralineEdit->text());
    info.insert("documento",ui->documento_lineEdit->text());
    info.insert("concepto",ui->concepto_lineEdit->text());
    info.insert("clave_op",ui->clavecomboBox->currentText().section("-",0,0).trimmed());
    info.insert("actividad",ui->actividad_comboBox->currentText().section("-",0,0).trimmed());
    info.insert("externo",ui->externo_lineEdit->text());
    info.insert("cuenta_ret",ui->cta_ret_lineEdit->text());
    info.insert("importe_ret",ui->retenido_lineEdit->text());
    info.insert("nombre",ui->nombrelineEdit->text());
    info.insert("cif",ui->ciflineEdit->text());
    info.insert("num_facts",ui->numfacturaslineEdit->text());
    info.insert("inicial",ui->iniciallineEdit->text());
    info.insert("final",ui->finallineEdit->text());
    info.insert("rectificativa",ui->rectificativagroupBox->isChecked());
    info.insert("tipo_rectificativa",ui->tipo_rectificativa_comboBox->currentText().section("-",0,0).trimmed());
    info.insert("rectificada",ui->rectificada_lineEdit->text());
    info.insert("no_sujeta",ui->nosujetacheckBox->isChecked());
    info.insert("export_ext",ui->exportacionheckBox->isChecked());
    info.insert("isp",ui->expedida_isp_checkBox->isChecked());
    info.insert("fuera_tac",ui->ventas_fuera_tac_checkBox->isChecked());
    info.insert("no_deduc",ui->exenta_no_deduccheckBox->isChecked());
    info.insert("donacion",ui->donacion_checkBox->isChecked());
    info.insert("donacion_clave",ui->clave_donacion_comboBox->currentText().section("-",0,0).trimmed());
    info.insert("donacion_ca",ui->ca_comboBox->currentText().section("-",0,0).trimmed());
    info.insert("donacion_especie",ui->donacion_especie_checkBox->isChecked());
    info.insert("fichdoc",ui->fichdoclineEdit->text());
    return info;
}

bool Emitida_exenta_aut::verifica()
{
    return ui->verificar_checkBox->isChecked();
}

void Emitida_exenta_aut::on_ctabasepushButton_clicked()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->CtabaselineEdit->text());
    int rdo=labusqueda->exec();
    if (rdo==QDialog::Accepted) {
       cadena2=labusqueda->seleccioncuenta();
       if (cadena2.length()>0) ui->CtabaselineEdit->setText(cadena2);
        else ui->CtabaselineEdit->setText("");
    }
    delete(labusqueda);

}


void Emitida_exenta_aut::on_ctafrapushButton_clicked()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->CtafralineEdit->text());
    int rdo=labusqueda->exec();
    if (rdo==QDialog::Accepted) {
       cadena2=labusqueda->seleccioncuenta();
       if (cadena2.length()>0) ui->CtafralineEdit->setText(cadena2);
       else ui->CtafralineEdit->setText("");
    }
    delete(labusqueda);

}


void Emitida_exenta_aut::on_cta_ret_pushButton_clicked()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cta_ret_lineEdit->text());
    int rdo=labusqueda->exec();
    if (rdo==QDialog::Accepted) {
       cadena2=labusqueda->seleccioncuenta();
       if (cadena2.length()>0) ui->cta_ret_lineEdit->setText(cadena2);
        else ui->cta_ret_lineEdit->clear();
    }
    delete(labusqueda);

}


void Emitida_exenta_aut::on_externo_pushButton_clicked()
{
    busca_externo *b = new busca_externo();
    int rdo=b->exec();
    if (rdo==QDialog::Accepted)
    {
        ui->externo_lineEdit->setText(b->codigo_elec());
    }
    else ui->externo_lineEdit->clear();
    delete(b);

}


void Emitida_exenta_aut::on_CtabaselineEdit_textChanged(const QString &arg1)
{
    QMessageBox msgBox;
    QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
    msgBox.addButton(tr("No"), QMessageBox::ActionRole);
    msgBox.setWindowTitle(tr("ADVERTENCIA"));
    msgBox.setIcon(QMessageBox::Warning);

    QString cadena;
    if (ui->CtabaselineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
        if (!existecodigoplan(ui->CtabaselineEdit->text(),&cadena))
        {
            // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
            {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->CtabaselineEdit->text());
                lasubcuenta2->exec();
                delete(lasubcuenta2);
                if (!existecodigoplan(ui->CtabaselineEdit->text(),&cadena))
                    ui->CtabaselineEdit->setText("");
            }
            else
                ui->CtabaselineEdit->setText("");
        }
    }
    if (esauxiliar(ui->CtabaselineEdit->text()))
        ui->basetextLabel->setText(descripcioncuenta(ui->CtabaselineEdit->text()));
    else ui->basetextLabel->setText("");

}


void Emitida_exenta_aut::on_CtabaselineEdit_editingFinished()
{
    ui->CtabaselineEdit->setText(expandepunto(ui->CtabaselineEdit->text(),anchocuentas()));
    QMessageBox msgBox;
    QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
    msgBox.addButton(tr("No"), QMessageBox::ActionRole);
    msgBox.setWindowTitle(tr("ADVERTENCIA"));
    msgBox.setIcon(QMessageBox::Warning);
    QString cadena;
    if (ui->CtabaselineEdit->text().length()>3 && cod_longitud_variable()) {
        if (!existecodigoplan(ui->CtabaselineEdit->text(),&cadena))
        {
            // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
            {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->CtabaselineEdit->text());
                lasubcuenta2->exec();
                delete(lasubcuenta2);
                if (!esauxiliar(ui->CtabaselineEdit->text()))
                    ui->CtabaselineEdit->setText("");
            }
            else
                ui->CtabaselineEdit->setText("");
        }
        if (esauxiliar(ui->CtabaselineEdit->text()))
            ui->basetextLabel->setText(descripcioncuenta(ui->CtabaselineEdit->text()));
        else ui->basetextLabel->setText("");
    }
}


void Emitida_exenta_aut::on_FechafradateEdit_dateChanged(const QDate &date)
{
    ui->fechaoperaciondateEdit->setDate(date);
}


void Emitida_exenta_aut::on_CtafralineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    QMessageBox msgBox;
    QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
    msgBox.addButton(tr("No"), QMessageBox::ActionRole);
    msgBox.setWindowTitle(tr("ADVERTENCIA"));
    msgBox.setIcon(QMessageBox::Warning);

    QString cadena;
    if (ui->CtafralineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
        if (!existecodigoplan(ui->CtafralineEdit->text(),&cadena))
        {
            // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
            {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->CtafralineEdit->text());
                lasubcuenta2->exec();
                delete(lasubcuenta2);
                if (!existecodigoplan(ui->CtafralineEdit->text(),&cadena))
                    ui->CtafralineEdit->setText("");
            }
            else
                ui->CtafralineEdit->setText("");
        }
    }
    if (esauxiliar(ui->CtafralineEdit->text()))
        ui->fratextLabel->setText(descripcioncuenta(ui->CtafralineEdit->text()));
    else ui->fratextLabel->setText("");

}


void Emitida_exenta_aut::on_CtafralineEdit_editingFinished()
{
    ui->CtafralineEdit->setText(expandepunto(ui->CtafralineEdit->text(),anchocuentas()));
    QMessageBox msgBox;
    QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
    msgBox.addButton(tr("No"), QMessageBox::ActionRole);
    msgBox.setWindowTitle(tr("ADVERTENCIA"));
    msgBox.setIcon(QMessageBox::Warning);
    QString cadena;
    ui->CtafralineEdit->setText(expandepunto(ui->CtafralineEdit->text(),anchocuentas()));
    if (ui->CtafralineEdit->text().length()>3 && cod_longitud_variable()) {
        if (!existecodigoplan(ui->CtafralineEdit->text(),&cadena))
        {
            // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
            {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->CtafralineEdit->text());
                lasubcuenta2->exec();
                delete(lasubcuenta2);
                if (!esauxiliar(ui->CtafralineEdit->text()))
                    ui->CtafralineEdit->setText("");
            }
            else
                ui->CtafralineEdit->setText("");
        }
        if (esauxiliar(ui->CtafralineEdit->text()))
            ui->fratextLabel->setText(descripcioncuenta(ui->CtafralineEdit->text()));
        else ui->fratextLabel->clear();
    }

}


void Emitida_exenta_aut::on_externo_lineEdit_textChanged(const QString &arg1)
{
    if (!ui->externo_lineEdit->text().isEmpty())
    {
        ui->externo_label->setText(basedatos::instancia()->razon_externo(ui->externo_lineEdit->text()));
        QString cod=basedatos::instancia()->cuenta_externo(ui->externo_lineEdit->text());
        if (!cod.isEmpty())
            ui->CtafralineEdit->setText(cod);
    }

}


void Emitida_exenta_aut::on_externo_lineEdit_editingFinished()
{
    if (!ui->externo_lineEdit->text().isEmpty())
    {
        ui->externo_label->setText(basedatos::instancia()->razon_externo(ui->externo_lineEdit->text()));
        QString cod=basedatos::instancia()->cuenta_externo(ui->externo_lineEdit->text());
        if (!cod.isEmpty())
            ui->CtafralineEdit->setText(cod);
    }
}


void Emitida_exenta_aut::on_cta_ret_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    QMessageBox msgBox;
    QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
    msgBox.addButton(tr("No"), QMessageBox::ActionRole);
    msgBox.setWindowTitle(tr("ADVERTENCIA"));
    msgBox.setIcon(QMessageBox::Warning);

    QString cadena;
    if (ui->cta_ret_lineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
        if (!existecodigoplan(ui->cta_ret_lineEdit->text(),&cadena))
        {
            // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
            {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->cta_ret_lineEdit->text());
                lasubcuenta2->exec();
                delete(lasubcuenta2);
                if (!existecodigoplan(ui->cta_ret_lineEdit->text(),&cadena))
                    ui->cta_ret_lineEdit->setText("");
            }
            else
                ui->cta_ret_lineEdit->setText("");
        }
    }
    if (esauxiliar(ui->cta_ret_lineEdit->text()))
        ui->cuenta_ret_Label->setText(descripcioncuenta(ui->cta_ret_lineEdit->text()));
    else ui->cuenta_ret_Label->setText("");

}


void Emitida_exenta_aut::on_cta_ret_lineEdit_editingFinished()
{
    ui->cta_ret_lineEdit->setText(expandepunto(ui->cta_ret_lineEdit->text(),anchocuentas()));
    QMessageBox msgBox;
    QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
    msgBox.addButton(tr("No"), QMessageBox::ActionRole);
    msgBox.setWindowTitle(tr("ADVERTENCIA"));
    msgBox.setIcon(QMessageBox::Warning);
    QString cadena;
    ui->cta_ret_lineEdit->setText(expandepunto(ui->cta_ret_lineEdit->text(),anchocuentas()));
    if (ui->cta_ret_lineEdit->text().length()>3 && cod_longitud_variable()) {
        if (!existecodigoplan(ui->cta_ret_lineEdit->text(),&cadena))
        {
            // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
            {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->cta_ret_lineEdit->text());
                lasubcuenta2->exec();
                delete(lasubcuenta2);
                if (!esauxiliar(ui->cta_ret_lineEdit->text()))
                    ui->cta_ret_lineEdit->setText("");
            }
            else
                ui->cta_ret_lineEdit->setText("");
        }
        if (esauxiliar(ui->cta_ret_lineEdit->text()))
            ui->cuenta_ret_Label->setText(descripcioncuenta(ui->cta_ret_lineEdit->text()));
        else ui->cuenta_ret_Label->clear();
    }

}



void Emitida_exenta_aut::on_donacion_checkBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    if (ui->donacion_checkBox->isChecked())
        ui->donacion_groupBox->show();
    else ui->donacion_groupBox->hide();
}

void Emitida_exenta_aut::cargacombooperaciones()
{
    QStringList lista=basedatos::instancia()->listaoperaciones_expedidas();
    QStringList lista2=basedatos::instancia()->listaoperaciones_recibidas();
    for (int veces=0;veces<lista2.count();veces++)
    {
        QString cadenaclave=lista2.at(veces);
        QString extracto=cadenaclave.section(" ",0,0);
        extracto=extracto.remove(' ');
        int veces2;
        for (veces2=0;veces2<lista.count();veces2++)
        {
            QString extracto2=lista.at(veces2).section(" ",0,0);
            if (extracto2==extracto) break;
        }
        if (veces2>=lista.count())
            // añadimos cadenaclave a lista
            lista << cadenaclave;
    }
    lista.sort();
    ui->clavecomboBox->addItems (lista);

}


void Emitida_exenta_aut::on_exportacionheckBox_toggled(bool checked)
{
    Q_UNUSED(checked)
    if (ui->exportacionheckBox->isChecked())
        ui->ventas_fuera_tac_checkBox->setChecked(false);

}


void Emitida_exenta_aut::on_ventas_fuera_tac_checkBox_toggled(bool checked)
{
    Q_UNUSED(checked)
    if (ui->ventas_fuera_tac_checkBox->isChecked())
        ui->exportacionheckBox->setChecked(false);

}


void Emitida_exenta_aut::on_AceptarButton_clicked()
{
    QString msj;
    if (ui->CtabaselineEdit->text().isEmpty()) msj+=tr("ERROR: cuenta base imponible vacía\n");
       else
         if (!existesubcuenta(ui->CtabaselineEdit->text())) msj+=tr("ERROR: cuenta base imponible no existe\n");

    if (ui->CtafralineEdit->text().isEmpty()) msj+=tr("ERROR: cuenta factura vacía\n");
       else
        if (!existesubcuenta(ui->CtafralineEdit->text())) msj+=tr("ERROR: cuenta factura no existe\n");

    if (!ui->externo_lineEdit->text().isEmpty())
           if (!basedatos::instancia()->existe_externo(ui->externo_lineEdit->text())) msj+=tr("ERROR: código de externo no existe\n");

    if (!ui->cta_ret_lineEdit->text().isEmpty())
        if (!existesubcuenta(ui->cta_ret_lineEdit->text())) msj+=tr("ERROR:cuenta de retención no existe\n");

    if (!msj.isEmpty()) {
        QMessageBox::warning(this,tr("Operación exenta"),msj);
        return;
    }

    accept();
}

void Emitida_exenta_aut::fichero_soltado()
{
    ui->fichdoclineEdit->setText(etiqueta->nombre_fich());
}


void Emitida_exenta_aut::on_fichdocpushButton_clicked()
{
    QFileDialog dialogofich(this);
    dialogofich.setFileMode(QFileDialog::ExistingFile);
    dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
    dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
    dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
    dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
    dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

    QStringList filtros;
    filtros << tr("Todos los archivos (*)");
    filtros << tr("Archivos jpg (*.jpg)");
    filtros << tr("Archivos jpg (*.jpeg)");
    filtros << tr("Archivos png (*.png)");
    filtros << tr("Archivos pdf (*.pdf)");
    dialogofich.setNameFilters(filtros);
    if (rutacargadocs().isEmpty())
        dialogofich.setDirectory(adapta(dirtrabajo()));
    else
        dialogofich.setDirectory(adapta(rutacargadocs()));
    dialogofich.setWindowTitle(tr("SELECCIÓN DE COPIA DE DOCUMENTO"));
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
            ui->fichdoclineEdit->setText(fileNames.at(0));
        }
    }

}


void Emitida_exenta_aut::on_visdocpushButton_clicked()
{
    if (ui->fichdoclineEdit->text().isEmpty()) return;

    QString url=ui->fichdoclineEdit->text();
    QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

    // if (!ejecuta(aplicacionabrirfich(extensionfich(ui->fichdoclineEdit->text())),ui->fichdoclineEdit->text()))
    //     QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
    //                          tr("No se puede abrir ")+ui->fichdoclineEdit->text()+tr(" con ")+
    //                              aplicacionabrirfich(extensionfich(ui->fichdoclineEdit->text())));

}


void Emitida_exenta_aut::on_borraasdocpushButton_clicked()
{
  ui->fichdoclineEdit->clear();
}

