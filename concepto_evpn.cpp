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

#include "concepto_evpn.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"

concepto_evpn::concepto_evpn(QString qapunte, QString cuenta, QString concepto,
                             QString debe, QString haber,
                             QString codconcepto) : QDialog() {
    ui.setupUi(this);

apunte=qapunte;
ui.cuentalineEdit->setText(cuenta);
ui.descriplineEdit->setText(descripcioncuenta(cuenta));
ui.concepto_diariolineEdit->setText(concepto);
ui.debelineEdit->setText(debe);
ui.haberlineEdit->setText(haber);

connect(ui.aceptarpushButton, SIGNAL(clicked()),this, SLOT(terminar()));
ui.comboBox->addItem(" - -");
ui.comboBox->addItems(basedatos::instancia()->lista_concepto_evpn());
bool encontrado=false;
if (!codconcepto.isEmpty())
  for (int veces=0; veces<ui.comboBox->count(); veces++)
    {
     if (ui.comboBox->itemText(veces).left(1)==codconcepto.left(1))
        {
         ui.comboBox->setCurrentIndex(veces);
         encontrado=true;
         break;
        }
    }
if (!encontrado) ui.comboBox->setCurrentIndex(0);
}


void concepto_evpn::terminar()
{
  QString cadena=ui.comboBox->currentText().left(1);
  if (cadena==" ") cadena.clear();
  basedatos::instancia()->updateconcepto_evpn_pymes (apunte, cadena);
  accept();
}

