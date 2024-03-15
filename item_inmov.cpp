#include "item_inmov.h"
#include "ui_item_inmov.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include <QMessageBox>

item_inmov::item_inmov(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::item_inmov)
{
    ui->setupUi(this);
    ui->cta_aa_lineEdit->setText(basedatos::instancia()->cuentas_aa());
    ui->cta_g_lineEdit->setText(basedatos::instancia()->cuentas_ag());
    connect(ui->busca_aa_pushButton,SIGNAL(clicked()),SLOT(botonaapulsado()));
    connect(ui->cta_aa_lineEdit,SIGNAL(editingFinished()),SLOT(finedicctaaa()));

    connect(ui->buscagpushButton,SIGNAL(clicked()),SLOT(botongcuentapulsado()));
    connect(ui->cta_g_lineEdit,SIGNAL(editingFinished()),SLOT(finedictag()));

    connect(ui->coeflineEdit,SIGNAL(editingFinished()),SLOT(finediccoef()));
    connect(ui->vadqlineEdit,SIGNAL(editingFinished()),SLOT(finedicvadq()));
    connect(ui->vamortlineEdit,SIGNAL(editingFinished()),SLOT(finedicvamort()));

    connect(ui->aceptarpushButton,SIGNAL(clicked()),SLOT(guardar_y_salir()));

    comadecimal=haycomadecimal(); decimales=haydecimales();

}

item_inmov::~item_inmov()
{
    delete ui;
}


void item_inmov::botonaapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cta_aa_lineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui->cta_aa_lineEdit->setText(cadena2);
       else ui->cta_aa_lineEdit->setText("");
    delete labusqueda;

}

void item_inmov::botongcuentapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cta_g_lineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui->cta_g_lineEdit->setText(cadena2);
       else ui->cta_g_lineEdit->setText("");
    delete labusqueda;

}

void item_inmov::finedicctaaa()
{
  ui->cta_aa_lineEdit->setText(expandepunto(ui->cta_aa_lineEdit->text(),anchocuentas()));
  if (!basedatos::instancia()->cuentas_aa().trimmed().isEmpty())
   if (!ui->cta_aa_lineEdit->text().startsWith(basedatos::instancia()->cuentas_aa().trimmed()))
    {
      QMessageBox::warning( this, tr("Inventario inmovilizado"),
                    tr("ERROR, la cuenta no es correcta"));
      ui->cta_aa_lineEdit->setText(basedatos::instancia()->cuentas_aa().trimmed());

    }


}

void item_inmov::finedictag()
{
    ui->cta_g_lineEdit->setText(expandepunto(ui->cta_g_lineEdit->text(),anchocuentas()));
    if (!basedatos::instancia()->cuentas_ag().trimmed().isEmpty())
     if (!ui->cta_g_lineEdit->text().startsWith(basedatos::instancia()->cuentas_ag().trimmed()))
      {
        QMessageBox::warning( this, tr("Plan de amortizaciones"),
                      tr("ERROR, la cuenta no es correcta"));
        ui->cta_g_lineEdit->setText(basedatos::instancia()->cuentas_ag().trimmed());
      }

}

void item_inmov::finediccoef()
{
 if (comadecimal)
  ui->coeflineEdit->setText(convacoma(ui->coeflineEdit->text()));
}

void item_inmov::finedicvadq()
{
 ui->vadqlineEdit->setText(formateanumero(convapunto(ui->vadqlineEdit->text()).toDouble(),comadecimal,decimales));
}

void item_inmov::finedicvamort()
{
    ui->vamortlineEdit->setText(formateanumero(convapunto(ui->vamortlineEdit->text()).toDouble(),comadecimal,decimales));

}

void item_inmov::pasadatos(QString num,
                           QString cta_aa,
                           QString cta_ag,
                           QString concepto,
                           QDate fecha_inicio,
                           QDate fecha_compra,
                           QDate fecha_ult_amort,
                           QString coef,
                           QString valor_adq,
                           QString valor_amortizado,
                           QString factura,
                           QString proveedor)
{
    double valcoef=coef.toDouble()*100;
    QString cadcoef; cadcoef.setNum(valcoef,'f',2);
    ui->numlabel->setText(num);
    ui->cta_aa_lineEdit->setText(cta_aa);
    ui->cta_g_lineEdit->setText(cta_ag);
    ui->conceptolineEdit->setText(concepto);
    ui->inicio_amortdateEdit->setDate(fecha_inicio);
    ui->compradateEdit->setDate(fecha_compra);
    ui->ult_amortdateEdit->setDate(fecha_ult_amort);
    ui->coeflineEdit->setText(cadcoef);
    ui->vadqlineEdit->setText(valor_adq);
    ui->vamortlineEdit->setText(valor_amortizado);
    ui->factlineEdit->setText(factura);
    ui->provlineEdit->setText(proveedor);
    finediccoef();
    finedicvadq();
    finedicvamort();

}

void item_inmov::guardar_y_salir()
{
    // "select cta_aa, cta_ag ,concepto,fecha_inicio, fecha_compra,"
    // "fecha_ult_amort, coef, valor_adq, valor_amortizado, factura, proveedor "
    // "from amoinv order by numero";
   if (ui->numlabel->text().startsWith('-'))
      {
       basedatos::instancia()->inserta_item_inmov(ui->cta_aa_lineEdit->text(),
                ui->cta_g_lineEdit->text(),
                ui->conceptolineEdit->text(),
                ui->inicio_amortdateEdit->date(),
                ui->compradateEdit->date(),
                ui->ult_amortdateEdit->date(),
                ui->coeflineEdit->text(),
                ui->vadqlineEdit->text(),
                ui->vamortlineEdit->text(),
                ui->factlineEdit->text(),
                ui->provlineEdit->text());

      }
      else
          {
          basedatos::instancia()->modifica_item_imov(ui->numlabel->text(),
                    ui->cta_aa_lineEdit->text(),
                  ui->cta_g_lineEdit->text(),
                  ui->conceptolineEdit->text(),
                  ui->inicio_amortdateEdit->date(),
                  ui->compradateEdit->date(),
                  ui->ult_amortdateEdit->date(),
                  ui->coeflineEdit->text(),
                  ui->vadqlineEdit->text(),
                  ui->vamortlineEdit->text(),
                  ui->factlineEdit->text(),
                  ui->provlineEdit->text());

          }
      accept();
}
