#include "pide_info_unif.h"
#include "ui_pide_info_unif.h"
#include "funciones.h"
#include "buscasubcuenta.h"
#include "busca_externo.h"
#include "basedatos.h"
#include "externos.h"

pide_info_unif::pide_info_unif(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pide_info_unif)
{
    ui->setupUi(this);
    comadecimal=haycomadecimal();
    decimales=haydecimales();
    connect(ui->ctatesorerialineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuentacambiada()));
    connect(ui->ctatesorerialineEdit,SIGNAL(editingFinished ()),this,SLOT(finediccuenta()));
    connect(ui->buscapushButton,SIGNAL( clicked()),this,SLOT(buscacuenta()));
    connect(ui->busca2pushButton,SIGNAL( clicked()),this,SLOT(buscacuenta2()));
    connect(ui->gastos_lineEdit,SIGNAL(editingFinished()),this,SLOT(finedic_gastos()));
    connect(ui->cta_gastos_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuenta_gasto_cambiada()));
    connect(ui->cta_gastos_lineEdit,SIGNAL(editingFinished()),this,SLOT(finediccuenta_gastos()));

    connect(ui->busca_externo_pushButton,SIGNAL(clicked(bool)),SLOT(buscar_externo()));
    connect(ui->externo_lineEdit,SIGNAL(textChanged(QString)),SLOT(campo_externo_cambiado()));
    connect(ui->quita_externo_pushButton,SIGNAL(clicked(bool)),SLOT(borra_externo()));
    connect(ui->datos_externo_pushButton,SIGNAL(clicked(bool)),SLOT(ver_externo()));

}

pide_info_unif::~pide_info_unif()
{
    delete ui;
}


void pide_info_unif::asignaetiqueta(QString nombre)
{
    ui->label->setText(nombre);
}

QDate pide_info_unif::fecha(void)
{
    return ui->calendarWidget->selectedDate();
}

void pide_info_unif::pasafecha(QDate fecha)
{
  ui->calendarWidget->setSelectedDate(fecha);
}

void pide_info_unif::asigna_concepto(QString concepto)
{
   ui->conceptolineEdit->setText(concepto);
}

void pide_info_unif::asigna_documento(QString documento)
{
  ui->documentolineEdit->setText(documento);
}

QString pide_info_unif::recupera_concepto()
{
  return ui->conceptolineEdit->text();
}

QString pide_info_unif::recupera_documento()
{
  return ui->documentolineEdit->text();
}


void pide_info_unif::cuentacambiada()
{
   QString cadena;
   if (!existesubcuenta(ui->ctatesorerialineEdit->text()))
     {
       ui->aceptarpushButton->setEnabled(false);
       ui->descriptesorlineEdit->setText("");
       return;
     }
     else
            {
               ui->aceptarpushButton->setEnabled(true);
               if (existecodigoplan(ui->ctatesorerialineEdit->text(),&cadena))
                  ui->descriptesorlineEdit->setText(cadena);
               else ui->descriptesorlineEdit->setText("");
            }
}


void pide_info_unif::finediccuenta()
{
 QString expandida=expandepunto(ui->ctatesorerialineEdit->text(),anchocuentas());
 ui->ctatesorerialineEdit->setText(expandida);
 /*
 QString ccc, iban, cedente1, cedente2, nriesgo, sufijo;
 basedatos::instancia()->datos_cuenta_banco(ui->ctatesorerialineEdit->text(),
                                             &ccc,
                                             &iban,
                                             &cedente1,
                                             &cedente2,
                                             &nriesgo,
                                             &sufijo);
 if (iban.isEmpty())
      ui->cuenta_bancolineEdit->setText(ccc);
   else
      ui->cuenta_bancolineEdit->setText(iban);
*/
}


void pide_info_unif::cuenta_gasto_cambiada()
{
   QString cadena;
   if (!existesubcuenta(ui->cta_gastos_lineEdit->text()))
     {
       ui->descrip_gastoslineEdit->clear();
       return;
     }
     else
            {
               if (existecodigoplan(ui->cta_gastos_lineEdit->text(),&cadena))
                  ui->descrip_gastoslineEdit->setText(cadena);
               else ui->descrip_gastoslineEdit->setText("");
            }
}


void pide_info_unif::finediccuenta_gastos()
{
 QString expandida=expandepunto(ui->cta_gastos_lineEdit->text(),anchocuentas());
 ui->cta_gastos_lineEdit->setText(expandida);
}



void pide_info_unif::buscacuenta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->ctatesorerialineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2)) ui->ctatesorerialineEdit->setText(cadena2);
       else ui->ctatesorerialineEdit->setText("");
    delete labusqueda;
}

void pide_info_unif::buscacuenta2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->cta_gastos_lineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2)) ui->cta_gastos_lineEdit->setText(cadena2);
       else ui->cta_gastos_lineEdit->setText("");
    delete labusqueda;
}

void pide_info_unif::finedic_gastos()
{
  if (comadecimal) ui->gastos_lineEdit->setText(convacoma(ui->gastos_lineEdit->text()));
     else
         ui->gastos_lineEdit->setText(convapunto(ui->gastos_lineEdit->text()));
}


void pide_info_unif::buscar_externo()
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

void pide_info_unif::campo_externo_cambiado()
{
 QString codigo=ui->externo_lineEdit->text();
 if (basedatos::instancia()->existe_externo(codigo))
   {
     ui->descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(codigo));
   }
   else ui->descrip_externo_lineEdit->clear();
}

void pide_info_unif::borra_externo()
{
  ui->externo_lineEdit->clear();
}

void pide_info_unif::ver_externo()
{
    externos *e = new externos();
    e->pasa_codigo(ui->externo_lineEdit->text());
    e->exec();
    delete(e);
}


void pide_info_unif::pasa_externo(QString externo) {
    ui->externo_lineEdit->setText(externo);
}

void pide_info_unif::pasa_cuenta_tesoreria(QString cuenta) {
    ui->ctatesorerialineEdit->setText(cuenta);
}

QString pide_info_unif::cuenta_tesoreria() {
  return ui->ctatesorerialineEdit->text();
}

QString pide_info_unif::externo() {
  return ui->externo_lineEdit->text();
}

QString pide_info_unif::cuenta_gasto() {
 return ui->cta_gastos_lineEdit->text();
}

QString pide_info_unif::importe_gasto() {
    return ui->gastos_lineEdit->text();
}
