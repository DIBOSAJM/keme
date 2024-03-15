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

#include "introci.h"
#include <QMessageBox>
#include "funciones.h"
#include "basedatos.h"
#include "pidenombre.h"
#include <QSqlRecord>


introci::introci() : QDialog() {
    ui.setupUi(this);

 // QString cadquery;
 parafiltro=false;

 QSqlQuery query = basedatos::instancia()->selectCodigocinivelordercodigo(1);
 
 if ( query.isActive() )
      while (query.next())
          {
           ui.ci1comboBox->insertItem ( ui.ci1comboBox->count(),query.value(0).toString());
           ui.d1comboBox->insertItem(ui.d1comboBox->count(),query.value(1).toString());
          }

 query = basedatos::instancia()->selectCodigocinivelordercodigo(2);
 
 if ( query.isActive() )
      while (query.next())
       {
        ui.ci2comboBox->insertItem (ui.ci2comboBox->count(),query.value(0).toString()) ;
        ui.d2comboBox->insertItem(ui.d2comboBox->count(),query.value(1).toString());
       }
 ui.ci2comboBox->insertItem ( -1,"- -") ;
 ui.d2comboBox->insertItem( -1,"");
 ui.ci2comboBox->setCurrentIndex ( 0 );
 ui.d2comboBox->setCurrentIndex ( 0 );

 query = basedatos::instancia()->selectCodigocinivelordercodigo(3);

 if ( query.isActive() )
      while (query.next())
       {
        ui.ci3comboBox->insertItem ( ui.ci3comboBox->count(),query.value(0).toString()) ;
        ui.d3comboBox->insertItem(ui.d3comboBox->count(),query.value(1).toString());
       }

 ui.ci3comboBox->insertItem ( -1,"- -") ;
 ui.ci3comboBox->setCurrentIndex ( 0 );
 ui.d3comboBox->insertItem( -1,"");
 ui.d3comboBox->setCurrentIndex ( 0 );

 connect(ui.ci1comboBox,SIGNAL(currentIndexChanged (QString)),this,SLOT(combo1cambiado()));
 connect(ui.ci2comboBox,SIGNAL(currentIndexChanged (QString)),this,SLOT(combo2cambiado()));
 connect(ui.ci3comboBox,SIGNAL(currentIndexChanged (QString)),this,SLOT(combo3cambiado()));

 connect(ui.d1comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(d1cambiado()));
 connect(ui.d2comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(d2cambiado()));
 connect(ui.d3comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(d3cambiado()));

 connect(ui.ci1comboBox,SIGNAL(editTextChanged (QString)),this,SLOT(combo1textocambiado(QString)));
 connect(ui.ci2comboBox,SIGNAL(editTextChanged (QString)),this,SLOT(combo2textocambiado(QString)));
 connect(ui.ci3comboBox,SIGNAL(editTextChanged (QString)),this,SLOT(combo3textocambiado(QString)));
 connect(ui.ci3comboBox->lineEdit(),SIGNAL(editingFinished())
           ,this,SLOT(combo3finedictexto()));

}


void introci::hazparafiltro()
{
 parafiltro=true;
 ui.ci1comboBox->insertItem ( -1,"???") ;
 ui.d1comboBox->insertItem ( -1,"??? Cualquier contenido") ;
 ui.ci2comboBox->insertItem ( -1,"???") ;
 ui.d2comboBox->insertItem ( -1,"??? Cualquier contenido") ;
 ui.ci3comboBox->insertItem ( -1,"*") ;
 ui.d3comboBox->insertItem ( -1,"* Cualquier contenido") ;

}


void introci::combo1cambiado()
{
// QString descripcion;
// QString qnivel;
 ui.d1comboBox->setCurrentIndex(ui.ci1comboBox->currentIndex());
 //bool encontrada=buscaci(codigo,&descripcion,&qnivel);
 //int elnivel=qnivel.toInt();
 //if (encontrada && elnivel==1)
 //    {
      //ui.ci1label->setText(descripcion);
 //    }
 
}


void introci::combo2cambiado()
{
 //QString descripcion;
 //QString qnivel;
 ui.d2comboBox->setCurrentIndex(ui.ci2comboBox->currentIndex());
 //bool encontrada=buscaci(codigo,&descripcion,&qnivel);
 //int elnivel=qnivel.toInt();
 //if (encontrada && elnivel==2)
 //    {
      //ui.ci2label->setText(descripcion);
 //    }
 
}


void introci::combo3cambiado()
{
 //QString descripcion;
 //QString qnivel;
 ui.d3comboBox->setCurrentIndex(ui.ci3comboBox->currentIndex());
 //bool encontrada=buscaci(codigo,&descripcion,&qnivel);
 //int elnivel=qnivel.toInt();
 //if (encontrada && elnivel==3)
 //    {
      //ui.ci3label->setText(descripcion);
 //    }
 
}


void introci::chequeabotonaceptar()
{
 // cada vez que varíe algo
 // que todos los textos existan o 2 y/o 3 sean - -
 // que 2 no sea -- y tres con contenido
 // desactivar/activar botón aceptar
 QString descripcion;
 QString qnivel;
 bool encontrada=buscaci(ui.ci1comboBox->lineEdit()->text(),&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (parafiltro && ui.ci1comboBox->lineEdit()->text()=="???") { encontrada=true; elnivel=1; }
 if (!encontrada || elnivel!=1) 
   {
    ui.aceptarpushButton->setEnabled(false);
    return;
   }
 if (ui.ci2comboBox->lineEdit()->text()=="- -" && ui.ci3comboBox->lineEdit()->text()!="- -")
   {
    ui.aceptarpushButton->setEnabled(false);
    return;
   }
 elnivel=0;
 encontrada=buscaci(ui.ci2comboBox->lineEdit()->text(),&descripcion,&qnivel);
 elnivel=qnivel.toInt();

 if (parafiltro && ui.ci2comboBox->lineEdit()->text()=="???") { encontrada=true; elnivel=2;}

 if (ui.ci2comboBox->lineEdit()->text()!="- -" && (!encontrada || elnivel!=2))
   {
    ui.aceptarpushButton->setEnabled(false);
    return;
   }

 elnivel=0;
 encontrada=buscaci(ui.ci3comboBox->lineEdit()->text(),&descripcion,&qnivel);
 elnivel=qnivel.toInt();

 if (parafiltro && ui.ci3comboBox->lineEdit()->text()=="*") { encontrada=true; elnivel=3;}

 if (ui.ci3comboBox->lineEdit()->text()!="- -" && (!encontrada || elnivel!=3))
   {
    ui.aceptarpushButton->setEnabled(false);
    return;
   }

 ui.aceptarpushButton->setEnabled(true);
}


void introci::combo1textocambiado(QString cadena)
{
 if (cadena=="- -") return;
 if (ui.ci1comboBox->lineEdit()->text().length()<3)
     {
      //ui.ci1label->setText("");
      chequeabotonaceptar();
      return;
     }
 if (cadena.length()>3)
    {
      ui.ci1comboBox->lineEdit()->setText(cadena.left(3));
      chequeabotonaceptar();
      return;
    }
 // QMessageBox::warning( this, tr("ci"),
 //                      cadena);

 if (cadena=="???" && parafiltro) 
    {
      chequeabotonaceptar();
      //ui.ci1label->setText("");
      return;
    }
 QString descripcion;
 QString qnivel;
 bool encontrada=buscaci(cadena,&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (encontrada && elnivel==1)
     {
      //ui.ci1label->setText(descripcion);

     }
  else if (!encontrada)
           {
            QMessageBox msgBox;
            QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
            msgBox.addButton(tr("No"), QMessageBox::ActionRole);
            msgBox.setWindowTitle(tr("ADVERTENCIA"));
            msgBox.setIcon(QMessageBox::Warning);
            QString TextoAviso = tr("El código suministrado no existe \n"
                         "¿ desea añadir el C.I. ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton)
                {
                  pidenombre *p = new pidenombre;
                  p->asignanombreventana(QObject::tr("C.I. primer nivel"));
                  p->asignaetiqueta(QObject::tr("Descripción del código '%1':").arg(cadena));
                  int resultado=p->exec();
                  if (resultado==QDialog::Accepted)
                      {
                        if (!buscaci(cadena,&descripcion,&qnivel))
                           insertaenci(cadena,p->contenido(),1);
                        //ui.ci1label->setText(descripcion);
                        ui.ci1comboBox->insertItem ( -1,cadena) ;
                        ui.d1comboBox->insertItem(-1,p->contenido());
                        ui.ci1comboBox->setCurrentIndex ( 0 );
                        ui.d1comboBox->setCurrentIndex(0);
 	              }
                  delete p;
                }
           }
 chequeabotonaceptar();
}


void introci::combo2textocambiado(QString cadena)
{
 if (cadena=="- -") {chequeabotonaceptar(); return;}
 if (ui.ci2comboBox->lineEdit()->text().length()<3)
     {
      //ui.ci2label->setText("");
      chequeabotonaceptar();
      return;
     }
 if (ui.ci2comboBox->lineEdit()->text().length()>3)
    {
      ui.ci2comboBox->lineEdit()->setText(cadena.left(3));
      chequeabotonaceptar();
      return;
    }
 if (cadena==QString("???") && parafiltro)
    { chequeabotonaceptar();
     //ui.ci2label->setText("");
     return;}
 QString descripcion;
 QString qnivel;
 bool encontrada=buscaci(cadena,&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (encontrada && elnivel==2)
     {
      // ui.ci2label->setText(descripcion);
     }
  else if (!encontrada)
           {
            QMessageBox msgBox;
            QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
            msgBox.addButton(tr("No"), QMessageBox::ActionRole);
            msgBox.setWindowTitle(tr("ADVERTENCIA"));
            msgBox.setIcon(QMessageBox::Warning);
            QString TextoAviso = tr("El código suministrado no existe \n"
                         "¿ desea añadir el C.I. ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton)
                {
                  pidenombre *p = new pidenombre;
                  p->asignanombreventana(QObject::tr("C.I. segundo nivel"));
                  p->asignaetiqueta(QObject::tr("Descripción del código '%1':").arg(cadena));
                  int resultado=p->exec();
                  if (resultado==QDialog::Accepted)
                      {
                        if (!buscaci(cadena,&descripcion,&qnivel))
                           insertaenci(cadena,p->contenido(),2);
                        //ui.ci2label->setText(descripcion);
                        ui.ci2comboBox->insertItem ( -1,cadena) ;
                        ui.d2comboBox->insertItem(-1,p->contenido());
                        ui.ci2comboBox->setCurrentIndex ( 0 );
                        ui.d2comboBox->setCurrentIndex(0);
 	              }
                  delete p;
                }
           }
 chequeabotonaceptar();

}

void introci::combo3textocambiado(QString cadena)
{
 /*QString descripcion;
 QString qnivel;
 bool encontrada=buscaci(cadena,&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (encontrada && elnivel==3)
     {
      //ui.ci3label->setText(descripcion);
     }
    // else ; //ui.ci3label->setText("");*/
 chequeabotonaceptar();

}


void introci::combo3finedictexto()
{
 QString cadena=ui.ci3comboBox->lineEdit()->text();
 if (cadena=="- -") { chequeabotonaceptar(); return; }
 if (cadena=="*" && parafiltro) { chequeabotonaceptar(); return; }
 QString descripcion;
 QString qnivel;
 bool encontrada=buscaci(cadena,&descripcion,&qnivel);
 if (!encontrada)
           {
            QMessageBox msgBox;
            QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
            msgBox.addButton(tr("No"), QMessageBox::ActionRole);
            msgBox.setWindowTitle(tr("ADVERTENCIA"));
            msgBox.setIcon(QMessageBox::Warning);
            QString TextoAviso = tr("El código suministrado no existe \n"
                         "¿ desea añadir el C.I. ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton)
                {
                  pidenombre *p = new pidenombre;
                  p->asignanombreventana(QObject::tr("C.I. tercer nivel"));
                  p->asignaetiqueta(QObject::tr("Descripción del código '%1':").arg(cadena));
                  int resultado=p->exec();
                  if (resultado==QDialog::Accepted)
                      {
                        if (!buscaci(cadena,&descripcion,&qnivel))
                           insertaenci(cadena,p->contenido(),3);
                        //ui.ci3label->setText(descripcion);
                        ui.ci3comboBox->insertItem ( -1,cadena) ;
                        ui.d3comboBox->insertItem(-1,p->contenido());
                        ui.ci3comboBox->setCurrentIndex ( 0 );
                        ui.d3comboBox->setCurrentIndex(0);
 	              }
                  delete p;
                } else ui.ci3comboBox->setEditText("- -");
           }
 chequeabotonaceptar();

}


QString introci::cadenaci()
{
 QString cadena=ui.ci1comboBox->lineEdit()->text();
 if (ui.ci2comboBox->lineEdit()->text()=="- -") return cadena;
     else cadena+=ui.ci2comboBox->lineEdit()->text();
 if (ui.ci3comboBox->lineEdit()->text()=="- -") return cadena;
     else cadena+=ui.ci3comboBox->lineEdit()->text();
 return cadena;
}

void introci::pasaci(QString codigo)
{

 if (codigo.length()>0)
   {
    //ui.ci1comboBox->setEditText(codigo.left(3));
     int pos=ui.ci1comboBox->findText(codigo.left(3),Qt::MatchExactly);
     ui.ci1comboBox->setCurrentIndex(pos);
     //ui.ci1comboBox->setCurrentText(codigo.left(3));
    combo1cambiado();
    // ui.ci2comboBox->setCurrentIndex ( 0 );
   }
 if (codigo.length()>3)
   {
    //ui.ci2comboBox->setEditText(codigo.mid(3,3));
    //ui.ci2comboBox->setCurrentText(codigo.mid(3,3));
    int pos=ui.ci2comboBox->findText(codigo.mid(3,3),Qt::MatchExactly);
    ui.ci2comboBox->setCurrentIndex(pos);
    combo2cambiado();
   }
 if (codigo.length()>6)
   {
    //ui.ci3comboBox->setEditText(codigo.right(codigo.length()-6));
    //ui.ci3comboBox->setCurrentText(codigo.right(codigo.length()-6));
     int pos=ui.ci3comboBox->findText(codigo.right(codigo.length()-6),Qt::MatchExactly);
     ui.ci3comboBox->setCurrentIndex(pos);
    combo3cambiado();
   }
}


void introci::d1cambiado()
{
 ui.ci1comboBox->setCurrentIndex(ui.d1comboBox->currentIndex());

}


void introci::d2cambiado()
{
 ui.ci2comboBox->setCurrentIndex(ui.d2comboBox->currentIndex());

}

void introci::d3cambiado()
{
 ui.ci3comboBox->setCurrentIndex(ui.d3comboBox->currentIndex());

}
