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

#include "ed_bloqueo_cta.h"
#include "basedatos.h"

ed_bloqueo_cta::ed_bloqueo_cta() : QDialog() {
    ui.setupUi(this);


connect(ui.todopushButton,SIGNAL(clicked()),SLOT(chequeatodo()));
connect(ui.nadapushButton,SIGNAL(clicked()),SLOT(chequeanada()));
connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(aceptar()));

}

void ed_bloqueo_cta::pasa_cta_descrip(QString qcodigo, QString descrip)
{
    codigo=qcodigo;
    ui.cuentalabel->setText(tr("CUENTA: ")+ qcodigo);
    ui.descriplabel->setText(descrip.left(42));
    bool enero=false;
    bool febrero=false;
    bool marzo=false;
    bool abril=false;
    bool mayo=false;
    bool junio=false;
    bool julio=false;
    bool agosto=false;
    bool septiembre=false;
    bool octubre=false;
    bool noviembre=false;
    bool diciembre=false;
    basedatos::instancia()->bloqueos_mes_cta(qcodigo,&enero,&febrero,&marzo,&abril,&mayo,
                                             &junio,&julio,&agosto,&septiembre,&octubre,
                                             &noviembre,&diciembre);
    ui.checkBox_1->setChecked(enero);
    ui.checkBox_2->setChecked(febrero);
    ui.checkBox_3->setChecked(marzo);
    ui.checkBox_4->setChecked(abril);
    ui.checkBox_5->setChecked(mayo);
    ui.checkBox_6->setChecked(junio);
    ui.checkBox_7->setChecked(julio);
    ui.checkBox_8->setChecked(agosto);
    ui.checkBox_9->setChecked(septiembre);
    ui.checkBox_10->setChecked(octubre);
    ui.checkBox_11->setChecked(noviembre);
    ui.checkBox_12->setChecked(diciembre);

}

void ed_bloqueo_cta::chequeatodo()
{
 ui.checkBox_1->setChecked(true);
 ui.checkBox_2->setChecked(true);
 ui.checkBox_3->setChecked(true);
 ui.checkBox_4->setChecked(true);
 ui.checkBox_5->setChecked(true);
 ui.checkBox_6->setChecked(true);
 ui.checkBox_7->setChecked(true);
 ui.checkBox_8->setChecked(true);
 ui.checkBox_9->setChecked(true);
 ui.checkBox_10->setChecked(true);
 ui.checkBox_11->setChecked(true);
 ui.checkBox_12->setChecked(true);
}

void ed_bloqueo_cta::chequeanada()
{
    ui.checkBox_1->setChecked(false);
    ui.checkBox_2->setChecked(false);
    ui.checkBox_3->setChecked(false);
    ui.checkBox_4->setChecked(false);
    ui.checkBox_5->setChecked(false);
    ui.checkBox_6->setChecked(false);
    ui.checkBox_7->setChecked(false);
    ui.checkBox_8->setChecked(false);
    ui.checkBox_9->setChecked(false);
    ui.checkBox_10->setChecked(false);
    ui.checkBox_11->setChecked(false);
    ui.checkBox_12->setChecked(false);
}


void ed_bloqueo_cta::aceptar()
{
    basedatos::instancia()->ed_bloqueos_mes_cta(codigo,
                                                ui.checkBox_1->isChecked(),
                                                ui.checkBox_2->isChecked(),
                                                ui.checkBox_3->isChecked(),
                                                ui.checkBox_4->isChecked(),
                                                ui.checkBox_5->isChecked(),
                                                ui.checkBox_6->isChecked(),
                                                ui.checkBox_7->isChecked(),
                                                ui.checkBox_8->isChecked(),
                                                ui.checkBox_9->isChecked(),
                                                ui.checkBox_10->isChecked(),
                                                ui.checkBox_11->isChecked(),
                                                ui.checkBox_12->isChecked()
                                                );
    accept();

}
