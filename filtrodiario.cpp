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

#include "filtrodiario.h"
#include "funciones.h"
#include "basedatos.h"

filtrodiario::filtrodiario(bool qcomadecimal) : QDialog()
 {
    ui.setupUi(this);

    comadecimal=qcomadecimal;
    // cargamos combo
    QStringList nombres;
    nombres << trad_fecha();
    nombres << trad_usuario();
    nombres << trad_asiento();
    nombres << trad_cuenta();
    nombres << trad_concepto();
    nombres << trad_documento();
    nombres << trad_debe();
    nombres << trad_haber();
    nombres << trad_diario();
    nombres << trad_pase();
    nombres << trad_ejercicio();
    nombres << trad_revisado();
    nombres << trad_externo();

    ui.camposcomboBox->addItems(nombres);
    ui.fechadateEdit->setDate(QDate::currentDate());
    connect(ui.contenidolineEdit,SIGNAL(textChanged(QString)),this,SLOT(contenidocambiado()));
    connect(ui.filtrotextEdit,SIGNAL(textChanged()),this,SLOT(textocambiado()));
    connect(ui.adicpushButton,SIGNAL(clicked()),this,SLOT(botonadicpulsado()));
    connect(ui.camposcomboBox,SIGNAL( currentIndexChanged (int)),this,SLOT(combocambiado()));

    connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(borrafiltro()));
    connect(ui.inicialpushButton,SIGNAL(clicked()),this,SLOT(filtroinicial()));

    connect(ui.contenidolineEdit,SIGNAL( editingFinished ()),this,SLOT(finediccontenido()));

 }


void filtrodiario::pasafiltro(QString qfiltro)
{
 if (qfiltro.length()>0)
    {
     ui.filtrotextEdit->setText(qfiltro);
     activayo();
    } else desactivayo();
}


void filtrodiario::activayo()
{
     ui.yogroupBox->setEnabled(true);
     /* label->setEnabled(false);
     camposcomboBox->setEnabled(false);
     operadoresgroupBox->setEnabled(false);
     contenidolineEdit->setEnabled(false); */

}

void filtrodiario::desactivayo()
{
     ui.yogroupBox->setEnabled(false);
     /* label->setEnabled(true);
     camposcomboBox->setEnabled(true);
     operadoresgroupBox->setEnabled(true);
     contenidolineEdit->setEnabled(true); */

}


void filtrodiario::botonadicpulsado()
{
 // componemos cláusula del filtro
 QString campo=ui.camposcomboBox->currentText();
 if (campo==trad_fecha()) campo="fecha";
 if (campo==trad_usuario()) campo="usuario";
 if (campo==trad_asiento()) campo="asiento";
 if (campo==trad_cuenta()) campo="cuenta";
 if (campo==trad_concepto()) campo="concepto";
 if (campo==trad_documento()) campo="documento";
 if (campo==trad_debe()) campo="debe";
 if (campo==trad_haber()) campo="haber";
 if (campo==trad_diario()) campo="diario";
 if (campo==trad_pase()) campo="pase";
 if (campo==trad_ejercicio()) campo="ejercicio";
 if (campo==trad_externo()) campo="externo";
 if (campo==trad_revisado()) campo= ui.verdaderoradioButton->isChecked() ? "revisado" : " not revisado";

 QString cadena;
 if (ui.yogroupBox->isEnabled())
     {
       if (ui.YradioButton->isChecked()) cadena+=" AND ";
           else cadena+=" OR ";
     }
 // cadena+=camposcomboBox->currentText();
 if (!campo.contains("revisado"))
 {
  if (ui.contengaradioButton->isChecked())
   {
    if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
        cadena += " "+ campo +" like '%" +ui.contenidolineEdit->text()+ "%' ";
    }
    else {
        cadena += "position('";
        cadena += ui.contenidolineEdit->text();
        cadena += "' in ";
        cadena += campo;
        cadena += " )>0";
    }
   }
    else
         {
          cadena+=campo;
          if (ui.igualradioButton->isChecked()) cadena+=" = ";
          if (ui.menorradioButton->isChecked()) cadena+=" < ";
          if (ui.mayorradioButton->isChecked()) cadena+=" > ";
          if (ui.menoroigualradioButton->isChecked()) cadena+=" <= ";
          if (ui.mayoroigualradioButton->isChecked()) cadena+=" >= ";
          if (ui.distintoradioButton->isChecked()) cadena+=" != ";
          if (ui.camposcomboBox->currentText()==trad_fecha())
              {
               cadena+="'";
               cadena+=ui.fechadateEdit->date().toString("yyyy-MM-dd");
               cadena+="'";
              }
             else
                 {
                  if (ui.camposcomboBox->currentText()==trad_usuario() ||
                      ui.camposcomboBox->currentText()==trad_concepto() ||
                      ui.camposcomboBox->currentText()==trad_documento() ||
                      ui.camposcomboBox->currentText()==trad_diario() ||
                      ui.camposcomboBox->currentText()==trad_ejercicio() ||
                      ui.camposcomboBox->currentText()==trad_cuenta() ||
                      ui.camposcomboBox->currentText()==trad_externo())
                      {
                       cadena+="'";
                       cadena+=ui.contenidolineEdit->text();
                       cadena+="'";
                      }
                      else cadena+=convapunto(ui.contenidolineEdit->text());
                  }

         }
 } else cadena+=campo;

  ui.filtrotextEdit->setText(ui.filtrotextEdit->toPlainText()+cadena);
  if (ui.filtrotextEdit->toPlainText().length()>0) activayo(); else desactivayo();
}


void filtrodiario::combocambiado()
{
 ui.contenidolineEdit->setText("");
 if (ui.camposcomboBox->currentText()==trad_fecha())
     {
      ui.contenidolineEdit->setEnabled(false);
      ui.contenidolabel->setEnabled(false);
      ui.fechadateEdit->setEnabled(true);
      ui.fechalabel->setEnabled(true);
      ui.contengaradioButton->setEnabled(false);
      ui.verdaderoradioButton->setEnabled(false);
      ui.operadoresgroupBox->setEnabled(true);
      return;
     }
 if (ui.camposcomboBox->currentText()==trad_revisado())
     {
      ui.contenidolineEdit->setEnabled(false);
      ui.contenidolabel->setEnabled(false);
      ui.fechadateEdit->setEnabled(false);
      ui.fechalabel->setEnabled(false);
      ui.contengaradioButton->setEnabled(false);
      ui.verdaderoradioButton->setEnabled(true);
      ui.operadoresgroupBox->setEnabled(false);
      return;
     }
 if (ui.camposcomboBox->currentText()==trad_usuario() ||
     ui.camposcomboBox->currentText()==trad_concepto() ||
     ui.camposcomboBox->currentText()==trad_documento() ||
     ui.camposcomboBox->currentText()==trad_diario() ||
     ui.camposcomboBox->currentText()==trad_ejercicio() ||
     ui.camposcomboBox->currentText()==trad_cuenta() ||
     ui.camposcomboBox->currentText()==trad_externo())
     {
      ui.contenidolineEdit->setEnabled(true);
      ui.contenidolabel->setEnabled(true);
      ui.fechadateEdit->setEnabled(false);
      ui.fechalabel->setEnabled(false);
      ui.contengaradioButton->setEnabled(true);
      ui.verdaderoradioButton->setEnabled(false);
      ui.operadoresgroupBox->setEnabled(true);
      return;
     }
     else
           {
            ui.contenidolineEdit->setEnabled(true);
            ui.contenidolabel->setEnabled(true);
            ui.fechadateEdit->setEnabled(false);
            ui.fechalabel->setEnabled(false);
            ui.contengaradioButton->setEnabled(false);
            ui.verdaderoradioButton->setEnabled(false);
            ui.operadoresgroupBox->setEnabled(true);
            return;
           }

}

void filtrodiario::contenidocambiado()
{
 QString cadena=ui.contenidolineEdit->text();
 QString digitos="0123456789";
 if (ui.camposcomboBox->currentText()==trad_asiento() || 
      ui.camposcomboBox->currentText()== trad_pase())
    {
     for (int veces=0; veces<cadena.length(); veces++)
        if (!digitos.contains(cadena[veces]))
           {
            cadena.remove(cadena[veces]);
            ui.contenidolineEdit->setText(cadena);
            break;
           }
     return;
    }
 digitos="0123456789.,";
 if (ui.camposcomboBox->currentText()==trad_debe() || 
      ui.camposcomboBox->currentText()== trad_haber())
    {
     for (int veces=0; veces<cadena.length(); veces++)
       {
        if (!digitos.contains(cadena[veces]))
           {
            cadena.remove(cadena[veces]);
            break;
           }
        }
     if (cadena.count('.')>1) cadena.remove('.');
     if (cadena.count(',')>1) cadena.remove(',');
     if (comadecimal) cadena.replace('.',','); else cadena.replace(',','.');
     ui.contenidolineEdit->setText(cadena);
    }
}

void filtrodiario::finediccontenido()
{
  if (ui.camposcomboBox->currentText()==trad_cuenta())
     ui.contenidolineEdit->setText(expandepunto(ui.contenidolineEdit->text(),anchocuentas()));
}

void filtrodiario::textocambiado()
{
 if (ui.filtrotextEdit->toPlainText().length()>0) activayo(); else desactivayo();
}

QString filtrodiario::filtro()
{
 return ui.filtrotextEdit->toPlainText();
}

void filtrodiario::borrafiltro()
{
  ui.filtrotextEdit->clear();
}


void filtrodiario::pasacondicionesfiltrodefecto(QString filtro)
{
 condicionesfiltrodefecto=filtro;
}

void filtrodiario::filtroinicial()
{
 ui.filtrotextEdit->setText(condicionesfiltrodefecto);
}

