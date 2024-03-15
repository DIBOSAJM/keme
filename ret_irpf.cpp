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

#include "ret_irpf.h"
#include "basedatos.h"
#include "funciones.h"
#include "buscasubcuenta.h"

ret_irpf::ret_irpf(bool qcomadecimal) : QDialog() {
    ui.setupUi(this);

// cargamos datos de la tabla configuración
comadecimal=qcomadecimal;
QString cuenta;
double tipo;
basedatos::instancia()->carga_irpf(&cuenta,&tipo);
ui.cuentalineEdit->setText(cuenta);
ui.tipolineEdit->setText(formateanumero(tipo,comadecimal,true));

connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(terminar()));

connect(ui.ctapushButton,SIGNAL(clicked()),this,SLOT(buscacta()));
connect(ui.cuentalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaexpandepunto()));

}

void ret_irpf::terminar()
{
    basedatos::instancia()->actuconfig_irpf(ui.cuentalineEdit->text(),
                                            convapunto(ui.tipolineEdit->text()));
    accept();
}



void ret_irpf::buscacta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuentalineEdit->setText(cadena2);
       else ui.cuentalineEdit->setText("");
    delete labusqueda;

}


void ret_irpf::ctaexpandepunto()
{
  ui.cuentalineEdit->setText(expandepunto(ui.cuentalineEdit->text(),anchocuentas()));
}

