#include "ctas_iva_inversion.h"
#include "ui_ctas_iva_inversion.h"
#include "buscasubcuenta.h"
#include "basedatos.h"

ctas_iva_inversion::ctas_iva_inversion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ctas_iva_inversion)
{
    ui->setupUi(this);
    connect(ui->aibsopivapushButton,SIGNAL(clicked(bool)),SLOT(boton_sop_aib()));
    connect(ui->aibreppushButton,SIGNAL(clicked(bool)),SLOT(boton_rep_aib()));
    connect(ui->aissopivapushButton,SIGNAL(clicked(bool)),SLOT(boton_sop_ais()));
    connect(ui->aisreppushButton,SIGNAL(clicked(bool)),SLOT(boton_rep_ais()));
    connect(ui->as_ext_sopivapushButton,SIGNAL(clicked(bool)),SLOT(boton_sop_aeb()));
    connect(ui->as_ext_reppushButton,SIGNAL(clicked(bool)),SLOT(boton_rep_aeb()));
    connect(ui->oi_sopivapushButton,SIGNAL(clicked(bool)),SLOT(boton_sop_oisp()));
    connect(ui->oi_reppushButton,SIGNAL(clicked(bool)),SLOT(boton_rep_oisp()));
    connect(ui->aceptar_pushButton,SIGNAL(clicked(bool)),SLOT(terminar()));

    // cargamos datos
    QString cta_iva_sop_AIB, cta_iva_rep_AIB, cta_iva_sop_AIS,  cta_iva_rep_AIS,
            cta_iva_sop_AES, cta_iva_rep_AES, cta_iva_sop_OI, cta_iva_rep_OI ;
    basedatos::instancia()->recupera_cuentas_inv_sp(&cta_iva_sop_AIB, &cta_iva_rep_AIB,
                                                    &cta_iva_sop_AIS, &cta_iva_rep_AIS,
                                                    &cta_iva_sop_AES, &cta_iva_rep_AES,
                                                    &cta_iva_sop_OI,  &cta_iva_rep_OI);


    ui->AIBctasoplineEdit->setText(cta_iva_sop_AIB);
    ui->AIBcuentareperlineEdit->setText(cta_iva_rep_AIB);
    ui->AISctasoplineEdit->setText(cta_iva_sop_AIS);
    ui->AIScuentareperlineEdit->setText(cta_iva_rep_AIS);
    ui->AS_ext_ctasoplineEdit->setText(cta_iva_sop_AES);
    ui->AS_ext_cuentareperlineEdit->setText(cta_iva_rep_AES);
    ui->oi_ctasoplineEdit->setText(cta_iva_sop_OI);
    ui->oicuentareperlineEdit->setText(cta_iva_rep_OI);


    connect(ui->AIBctasoplineEdit,SIGNAL(editingFinished()), SLOT(AIBctasoplineEdit_finedic()));
    connect(ui->AIBcuentareperlineEdit,SIGNAL(editingFinished()), SLOT(AIBcuentareperlineEdit_finedic()));
    connect(ui->AISctasoplineEdit,SIGNAL(editingFinished()), SLOT(AISctasoplineEdit_finedic()));
    connect(ui->AIScuentareperlineEdit,SIGNAL(editingFinished()), SLOT(AIScuentareperlineEdit_finedic()));
    connect(ui->AS_ext_ctasoplineEdit,SIGNAL(editingFinished()), SLOT(AS_ext_ctasoplineEdit_finedic()));
    connect(ui->AS_ext_cuentareperlineEdit,SIGNAL(editingFinished()), SLOT(AS_ext_cuentareperlineEdit_finedic()));
    connect(ui->oi_ctasoplineEdit,SIGNAL(editingFinished()), SLOT(oi_ctasoplineEdit_finedic()));
    connect(ui->oicuentareperlineEdit,SIGNAL(editingFinished()), SLOT(oicuentareperlineEdit_finedic()));


}

ctas_iva_inversion::~ctas_iva_inversion()
{
    delete ui;
}

void ctas_iva_inversion::terminar()
{
    basedatos::instancia()->guarda_cuentas_inv_sp(
       ui->AIBctasoplineEdit->text(),
       ui->AIBcuentareperlineEdit->text(),
       ui->AISctasoplineEdit->text(),
       ui->AIScuentareperlineEdit->text(),
       ui->AS_ext_ctasoplineEdit->text(),
       ui->AS_ext_cuentareperlineEdit->text(),
       ui->oi_ctasoplineEdit->text(),
       ui->oicuentareperlineEdit->text());
    accept();
}

//Función común para la busqueda de cuentas
void ctas_iva_inversion::buscacta(QLineEdit * lineedit)
{
    buscasubcuenta *labusqueda=new buscasubcuenta(lineedit->text());
    int rdo=labusqueda->exec();
    QString cadena2=labusqueda->seleccioncuenta();
    if (rdo==QDialog::Accepted)
      {
       if (esauxiliar(cadena2)) lineedit->setText(cadena2);
        else lineedit->setText("");
      }
    delete labusqueda;
}

void ctas_iva_inversion::boton_sop_aib()
{
    buscacta(ui->AIBctasoplineEdit);
}

void ctas_iva_inversion::boton_rep_aib()
{
    buscacta(ui->AIBcuentareperlineEdit);

}

void ctas_iva_inversion::boton_sop_ais()
{
    buscacta(ui->AISctasoplineEdit);

}

void ctas_iva_inversion::boton_rep_ais()
{
    buscacta(ui->AIScuentareperlineEdit);

}

void ctas_iva_inversion::boton_sop_aeb()
{
    buscacta(ui->AS_ext_ctasoplineEdit);

}

void ctas_iva_inversion::boton_rep_aeb()
{
    buscacta(ui->AS_ext_cuentareperlineEdit);

}

void ctas_iva_inversion::boton_sop_oisp()
{
    buscacta(ui->oi_ctasoplineEdit);

}

void ctas_iva_inversion::boton_rep_oisp()
{
    buscacta(ui->oicuentareperlineEdit);

}


void ctas_iva_inversion::AIBctasoplineEdit_finedic()
{
ui->AIBctasoplineEdit->setText(expandepunto(ui->AIBctasoplineEdit->text(),anchocuentas()));
}

void ctas_iva_inversion::AIBcuentareperlineEdit_finedic()
{
ui->AIBcuentareperlineEdit->setText(expandepunto(ui->AIBcuentareperlineEdit->text(),anchocuentas()));
}

void ctas_iva_inversion::AISctasoplineEdit_finedic()
{
ui->AISctasoplineEdit->setText(expandepunto(ui->AISctasoplineEdit->text(),anchocuentas()));
}

void ctas_iva_inversion::AIScuentareperlineEdit_finedic()
{
ui->AIScuentareperlineEdit->setText(expandepunto(ui->AIScuentareperlineEdit->text(),anchocuentas()));
}

void ctas_iva_inversion::AS_ext_ctasoplineEdit_finedic()
{
ui->AS_ext_ctasoplineEdit->setText(expandepunto(ui->AS_ext_ctasoplineEdit->text(),anchocuentas()));
}

void ctas_iva_inversion::AS_ext_cuentareperlineEdit_finedic()
{
ui->AS_ext_cuentareperlineEdit->setText(expandepunto(ui->AS_ext_cuentareperlineEdit->text(),anchocuentas()));
}

void ctas_iva_inversion::oi_ctasoplineEdit_finedic()
{
ui->oi_ctasoplineEdit->setText(expandepunto(ui->oi_ctasoplineEdit->text(),anchocuentas()));
}

void ctas_iva_inversion::oicuentareperlineEdit_finedic()
{
ui->oicuentareperlineEdit->setText(expandepunto(ui->oicuentareperlineEdit->text(),anchocuentas()));
}
