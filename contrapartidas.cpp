#include "contrapartidas.h"
#include "ui_contrapartidas.h"
#include "funciones.h"
#include "aux_express.h"
#include "buscasubcuenta.h"
#include <QMessageBox>
#include "busca_cta_diario.h"

contrapartidas::contrapartidas(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::contrapartidas)
{
    ui->setupUi(this);

    connect(ui->auxiliarlineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctacambiada()));
    connect(ui->auxiliarlineEdit,SIGNAL(editingFinished()),this,SLOT(ctafinedicion()));
    connect(ui->buscapushButton,SIGNAL(clicked()),this,SLOT(buscacta()));

    connect(ui->auxiliarlineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(ctacambiada2()));
    connect(ui->auxiliarlineEdit_2,SIGNAL(editingFinished()),this,SLOT(ctafinedicion2()));
    connect(ui->buscapushButton_2,SIGNAL(clicked()),this,SLOT(buscacta2()));

    connect(ui->valorlineEdit,SIGNAL(editingFinished()),this,SLOT(campo_valor_fin_edicion()));

    connect(ui->concepto_cuaderno_checkBox,SIGNAL(stateChanged(int)),this,SLOT(check_concepto()));

    connect(ui->aceptar_pushButton,SIGNAL(clicked(bool)),SLOT(boton_aceptar()));

    haycoma=haycomadecimal();

    ui->guarda_valores_checkBox->hide();

}

contrapartidas::~contrapartidas()
{
    delete ui;
}

void contrapartidas::activa_una_linea() {
   ui->guarda_valores_checkBox->show();
   ui->verificar_checkBox->setText(tr("Verificar asiento"));
}

bool contrapartidas::guardar_valores()
{
    return ui->guarda_valores_checkBox->isChecked();
}

void contrapartidas::ctacambiada()
{

   QString cadena;
    if (ui->auxiliarlineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
    if (!existecodigoplan(ui->auxiliarlineEdit->text(),&cadena))
      {
       // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
           if (QMessageBox::question(
              this,
              tr("Crear cuenta auxiliar"),
              tr("Esa cuenta no existe, ¿ Desea añadirla ?")) ==QMessageBox::Yes )
             {
               aux_express *lasubcuenta2=new aux_express();
               lasubcuenta2->pasacodigo(ui->auxiliarlineEdit->text());
               lasubcuenta2->exec();
               delete(lasubcuenta2);
               if (!existecodigoplan(ui->auxiliarlineEdit->text(),&cadena))
               ui->auxiliarlineEdit->clear();
         }
          else
                ui->auxiliarlineEdit->clear();
      }
        }
    // chequeabotonaceptar();
    if (esauxiliar(ui->auxiliarlineEdit->text()))
       {
        ui->auxiliarlabel->setText(descripcioncuenta(ui->auxiliarlineEdit->text()));
       }
    else
        {
         ui->auxiliarlabel->setText("- -");
        }
    chequea_boton_aceptar();
}

void contrapartidas::ctafinedicion()
{
   ui->auxiliarlineEdit->setText(expandepunto(ui->auxiliarlineEdit->text(),anchocuentas()));
    QString cadena;
    if (ui->auxiliarlineEdit->text().length()>3 && cod_longitud_variable()) {
    if (!existecodigoplan(ui->auxiliarlineEdit->text(),&cadena))
      {
     // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
           if (QMessageBox::question(
              this,
              tr("Crear cuenta auxiliar"),
              tr("Esa cuenta no existe, ¿ Desea añadirla ?")) == QMessageBox::Yes )
              {
               aux_express *lasubcuenta2=new aux_express();
               lasubcuenta2->pasacodigo(ui->auxiliarlineEdit->text());
               lasubcuenta2->exec();
               delete(lasubcuenta2);
               if (!existecodigoplan(ui->auxiliarlineEdit->text(),&cadena))
               ui->auxiliarlineEdit->clear();
              }
              else
                ui->auxiliarlineEdit->clear();
       }

     if (esauxiliar(ui->auxiliarlineEdit->text()))
         {
             ui->auxiliarlabel->setText(descripcioncuenta(ui->auxiliarlineEdit->text()));
         }
        else
            {
             ui->auxiliarlabel->setText("- -");
            }
    }

  chequea_boton_aceptar();

}


void contrapartidas::buscacta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->auxiliarlineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui->auxiliarlineEdit->setText(cadena2);
       else ui->auxiliarlineEdit->clear();
    delete(labusqueda);
}



void contrapartidas::ctacambiada2()
{

   QString cadena;
    if (ui->auxiliarlineEdit_2->text().length()==anchocuentas() && !cod_longitud_variable()) {
    if (!existecodigoplan(ui->auxiliarlineEdit_2->text(),&cadena))
      {
       // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
           if (QMessageBox::question(
              this,
              tr("Crear cuenta auxiliar"),
              tr("Esa cuenta no existe, ¿ Desea añadirla ?")) == QMessageBox::Yes )
               {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->auxiliarlineEdit_2->text());
                lasubcuenta2->exec();
                delete(lasubcuenta2);
                if (!existecodigoplan(ui->auxiliarlineEdit_2->text(),&cadena))
                ui->auxiliarlineEdit_2->clear();
               }
             else
                ui->auxiliarlineEdit_2->clear();
      }
    }
    // chequeabotonaceptar();
    if (esauxiliar(ui->auxiliarlineEdit_2->text()))
       {
        ui->auxiliarlabel_2->setText(descripcioncuenta(ui->auxiliarlineEdit_2->text()));
       }
    else
        {
         ui->auxiliarlabel_2->setText("- -");
        }
    chequea_boton_aceptar();
}

void contrapartidas::ctafinedicion2()
{
   ui->auxiliarlineEdit_2->setText(expandepunto(ui->auxiliarlineEdit_2->text(),anchocuentas()));
    QString cadena;
    if (ui->auxiliarlineEdit_2->text().length()>3 && cod_longitud_variable()) {
    if (!existecodigoplan(ui->auxiliarlineEdit_2->text(),&cadena))
      {
     // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
           if (QMessageBox::question(
              this,
              tr("Crear cuenta auxiliar"),
              tr("Esa cuenta no existe, ¿ Desea añadirla ?")) == QMessageBox::Yes )
               {
                aux_express *lasubcuenta2=new aux_express();
                lasubcuenta2->pasacodigo(ui->auxiliarlineEdit_2->text());
                lasubcuenta2->exec();
               delete(lasubcuenta2);
               if (!existecodigoplan(ui->auxiliarlineEdit_2->text(),&cadena))
               ui->auxiliarlineEdit_2->clear();
              }
          else
                ui->auxiliarlineEdit_2->clear();
       }
     if (esauxiliar(ui->auxiliarlineEdit_2->text()))
        {
         ui->auxiliarlabel_2->setText(descripcioncuenta(ui->auxiliarlineEdit_2->text()));
        }
       else
            {
             ui->auxiliarlabel_2->setText("- -");
            }
        }
    chequea_boton_aceptar();
}


void contrapartidas::buscacta2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui->auxiliarlineEdit_2->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui->auxiliarlineEdit_2->setText(cadena2);
       else ui->auxiliarlineEdit_2->clear();
    delete(labusqueda);
}


void contrapartidas::chequea_boton_aceptar()
{
    if (!esauxiliar(ui->auxiliarlineEdit->text()))
       {
        ui->aceptar_pushButton->setEnabled(false);
        return;
       }

    if (ui->auxiliarlineEdit_2->text().isEmpty() && ui->valorlineEdit->text().isEmpty())
       {
        ui->aceptar_pushButton->setEnabled(true);
        return;
       }

    if (!esauxiliar(ui->auxiliarlineEdit_2->text()))
       {
        ui->aceptar_pushButton->setEnabled(false);
        return;
       }

    if (convapunto(ui->valorlineEdit->text()).toDouble()>-0.001 && convapunto(ui->valorlineEdit->text()).toDouble()<0.001)
    {
     ui->aceptar_pushButton->setEnabled(false);
     return;
    }

    ui->aceptar_pushButton->setEnabled(true);

}


void contrapartidas::campo_valor_fin_edicion()
{
    if (haycoma) ui->valorlineEdit->setText(convacoma(ui->valorlineEdit->text()));
    chequea_boton_aceptar();
}

void contrapartidas::valores(QString *aux1, QString *aux2, QString *concepto, QString *valorcontra,
                             bool *coeficiente, bool *verificar, bool *concepto_cuaderno)
{
    *aux1=ui->auxiliarlineEdit->text();
    *aux2=ui->auxiliarlineEdit_2->text();
    *concepto=ui->conceptolineEdit->text();
    *valorcontra=ui->valorlineEdit->text();
    *coeficiente=ui->comboBox->currentIndex()==1;
    *verificar=ui->verificar_checkBox->isChecked();
    *concepto_cuaderno=ui->concepto_cuaderno_checkBox->isChecked();
}

void contrapartidas::pasa_valores(QString aux1, QString aux2, QString concepto, QString valorcontra,
                             bool coeficiente, bool verificar, bool concepto_cuaderno)
{
    ui->auxiliarlineEdit->setText(aux1);
    ui->auxiliarlineEdit_2->setText(aux2);
    ui->conceptolineEdit->setText(concepto);
    ui->valorlineEdit->setText(valorcontra);
    if (coeficiente) ui->comboBox->setCurrentIndex(1);
    ui->verificar_checkBox->setChecked(verificar);
    if (concepto_cuaderno) ui->concepto_cuaderno_checkBox->setChecked(true);
    if (!aux1.isEmpty()) ui->guarda_valores_checkBox->setChecked(true);
}

void contrapartidas::check_concepto()
{
  if (ui->concepto_cuaderno_checkBox->isChecked()) ui->conceptolineEdit->setEnabled(false);
    else ui->conceptolineEdit->setEnabled(true);
}

void contrapartidas::boton_aceptar()
{
    if (ui->conceptolineEdit->text().isEmpty() && !ui->concepto_cuaderno_checkBox->isChecked())
       {
        if (QMessageBox::question(
          this,
          tr("Generación de asientos"),
          tr("El campo concepto está vacío. ¿ Desea continuar ?")) == QMessageBox::No ) return;
       }

    accept();
}

void contrapartidas::on_busca_diario_pushButton_clicked()
{
    Busca_cta_diario *b = new Busca_cta_diario();
    if (b->exec()==QDialog::Accepted) {
        ui->auxiliarlineEdit->setText(b->cuenta_seleccionada());
    }
    delete (b);
}


void contrapartidas::on_buscapush_diario2_pushButton_clicked()
{
    Busca_cta_diario *b = new Busca_cta_diario();
    if (b->exec()==QDialog::Accepted) {
        ui->auxiliarlineEdit_2->setText(b->cuenta_seleccionada());
    }
    delete (b);

}

