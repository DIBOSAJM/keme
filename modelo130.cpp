#include "modelo130.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "ui_modelo130.h"
#include "basedatos.h"
#include "buscasubcuenta.h"

Modelo130::Modelo130(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Modelo130)
{
    ui->setupUi(this);
    ui->NIF_lineEdit->setText(basedatos::instancia()->cif());
    QString empresa, nif, nombre, apellidos;
    basedatos::instancia()->empresa_nif_nombre_apellidos(&empresa, &nif, &nombre, &apellidos);
    if (nombre.isEmpty())
        ui->nombre_lineEdit->setText(empresa);
    else {
        ui->nombre_lineEdit->setText(nombre);
        ui->apellidos_lineEdit->setText(apellidos);
    }
}

Modelo130::~Modelo130()
{
    delete ui;
}

void Modelo130::pasa_datos(bool qcomadecimal, bool qdecimales ,QString ejercicio, QString periodo, QString ingresos, QString gastos, QString old130, QString ret)
{
    ui->ejercicio_lineEdit->setText(ejercicio);
    ui->periodo_lineEdit->setText(periodo);
    ui->ingresos_lineEdit->setText(ingresos);
    ui->gastos_lineEdit->setText(gastos);
    ui->anteriores_130_lineEdit->setText(old130);
    ui->retenciones_lineEdit->setText(ret);
    comadecimal=qcomadecimal;
    decimales=qdecimales;
}

void Modelo130::on_busca_pushButton_clicked()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->auxiliar_lineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
        ui->auxiliar_lineEdit->setText(cadena2);
    else ui->auxiliar_lineEdit->setText("");
    delete labusqueda;

}


void Modelo130::on_auxiliar_lineEdit_editingFinished()
{
    ctaexpandepunto();
    if (ui->auxiliar_lineEdit->text().length()<anchocuentas() && !cod_longitud_variable()) on_busca_pushButton_clicked();

}

void Modelo130::ctaexpandepunto()
{
    ui->auxiliar_lineEdit->setText(expandepunto(ui->auxiliar_lineEdit->text(),anchocuentas()));
}

void Modelo130::calcula()
{
    double rendimiento_neto=convapunto(ui->ingresos_lineEdit->text()).toDouble()-convapunto(ui->gastos_lineEdit->text()).toDouble();
    ui->rn_lineEdit->setText(formateanumero(rendimiento_neto,comadecimal,decimales));
    double por20=rendimiento_neto*0.2;
    if (por20<0) por20=0;
    ui->porc20_lineEdit->setText(formateanumero(por20,comadecimal,decimales));
    double pago_frac=por20-convapunto(ui->anteriores_130_lineEdit->text()).toDouble()-convapunto(ui->retenciones_lineEdit->text()).toDouble();
    if (pago_frac<0) pago_frac=0;
    ui->pago_frac_lineEdit->setText(formateanumero(pago_frac,comadecimal,decimales));
}

void Modelo130::calcula2()
{
    double pago_frac=convapunto(ui->porc20_lineEdit->text()).toDouble() -convapunto(ui->anteriores_130_lineEdit->text()).toDouble()-convapunto(ui->retenciones_lineEdit->text()).toDouble();
    if (pago_frac<0) pago_frac=0;
    ui->pago_frac_lineEdit->setText(formateanumero(pago_frac,comadecimal,decimales));

}


void Modelo130::on_auxiliar_lineEdit_textChanged(const QString &arg1)
{
    if (existesubcuenta(ui->auxiliar_lineEdit->text()))
        ui->iban_lineEdit->setText(basedatos::instancia()->iban_cuenta_banco(arg1));

}



void Modelo130::on_ingresos_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    calcula();
}


void Modelo130::on_gastos_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    calcula();
}


void Modelo130::on_porc20_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    calcula2();
}


void Modelo130::on_anteriores_130_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    calcula2();
}


void Modelo130::on_retenciones_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    calcula2();
}

void Modelo130::on_pushButton_clicked()
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
    QString filtrotexto=tr("Archivos 130 (*.130)");
    filtros << filtrotexto;
    filtros << tr("Archivos 130 (*.130)");
    dialogofich.setNameFilters(filtros);
    dialogofich.setDirectory(adapta(dirtrabajo()));
    dialogofich.setWindowTitle(tr("ARCHIVO 130"));
    QStringList fileNames;
    if (dialogofich.exec())
    {
        fileNames = dialogofich.selectedFiles();
        if (fileNames.at(0).length()>0)
        {
            QString nombre=fileNames.at(0);
            if (nombre.right(4)!=".130") nombre=nombre+".130";
            QFile doc( adapta(nombre)  );
            if (doc.exists() && QMessageBox::question(this,
                                                      tr("¿ Sobreescribir ?"),
                                                      tr("'%1' ya existe."
                                                         "¿ Desea sobreescribirlo ?")
                                                          .arg( nombre ))==QMessageBox::No)
                return;
            if (!genfich130(nombre))
                QMessageBox::information( this, tr("Fichero 303"),tr("ERROR: NO se ha generado correctamente el fichero 130"));
        }
    }

}


bool Modelo130::genfich130(QString fichero)
{
    QFile doc( adapta(fichero)  );
    if ( !doc.open( QIODevice::WriteOnly ) )
    {
        QMessageBox::warning( this, tr("Fichero 303"),tr("Error: Imposible grabar fichero"));
        return false;
    }
    QTextStream docstream( &doc );
    docstream.setEncoding(QStringConverter::Latin1);
    QString ejercicio=ui->ejercicio_lineEdit->text().trimmed();
    QString contenido="<T1300";
    contenido+=ejercicio;
    contenido+=ui->periodo_lineEdit->text().trimmed().left(2);
    contenido+="0000>";
    contenido+="<AUX>";
    QString str; str.fill(' ',70);
    contenido+=str;
    contenido+="1.00"; // versión del programa
    str.clear(); str.fill(' ',4);
    contenido+=str;
    contenido+="Q2826000H";
    str.clear(); str.fill(' ',213);
    contenido+=str;
    contenido+="</AUX>";

    contenido+="<T13001000> ";
    if (ui->domiciliacion_groupBox->isChecked()) contenido+="U";
      else if (convapunto(ui->pago_frac_lineEdit->text()).toDouble() >0.004) contenido+="I";
         else contenido+="N";
    contenido+=completacadnum(ui->NIF_lineEdit->text().trimmed(),9);

    QString apellidos=ui->apellidos_lineEdit->text().trimmed();
    apellidos=completacadcad(adaptacad_aeat(apellidos),60);
    contenido+=apellidos;

    QString nombre=ui->nombre_lineEdit->text().trimmed();
    nombre=completacadcad(adaptacad_aeat(nombre),20);
    contenido+=nombre;

    contenido+=ejercicio;

    contenido+=ui->periodo_lineEdit->text().trimmed().left(2);

    contenido+=cadnum2dec_cadena_signo(convapunto(ui->ingresos_lineEdit->text()).toDouble(),17);
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->gastos_lineEdit->text()).toDouble(),17);
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->rn_lineEdit->text()).toDouble(),17);
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->porc20_lineEdit->text()).toDouble(),17);
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->anteriores_130_lineEdit->text()).toDouble(),17);
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->retenciones_lineEdit->text()).toDouble(),17);
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->pago_frac_lineEdit->text()).toDouble(),17);
    str.clear(); str.fill('0',17);
    contenido+=str;
    contenido+=str;
    contenido+=str;
    contenido+=str;
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->pago_frac_lineEdit->text()).toDouble(),17);
    contenido+=str;
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->pago_frac_lineEdit->text()).toDouble(),17);
    contenido+=str;
    contenido+=str;
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->pago_frac_lineEdit->text()).toDouble(),17);
    contenido+=str;
    contenido+=cadnum2dec_cadena_signo(convapunto(ui->pago_frac_lineEdit->text()).toDouble(),17);
    contenido+=" "; // no complementaria
    str.clear(); str.fill(' ',13);
    contenido+=str; // número justificante declaración anterior
    QString iban=ui->iban_lineEdit->text().trimmed();
    iban=completacadcad(iban,34);
    contenido+=iban;
    str.clear(); str.fill(' ',96);
    contenido+=str; // espacio reservado aeat
    str.clear(); str.fill(' ',13);
    contenido+=str; // espacio reservado aeat sello electrónico
    contenido+="</T13001000>"; // fin de registro

    contenido+="</T1300";
    contenido+=ejercicio;
    contenido+=ui->periodo_lineEdit->text().trimmed().left(2);
    contenido+="0000>";

    docstream << contenido;
    doc.close();

    QMessageBox::information( this, tr("Fichero 130"),tr("130 generado en archivo: ")+fichero);

    return true;
}

