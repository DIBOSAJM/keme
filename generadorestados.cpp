/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#include "generadorestados.h"
#include "funciones.h"
#include "basedatos.h"
#include "pidenombre.h"
#include "notas.h"
#include <QMessageBox>
#include <QProgressDialog>

generadorestados::generadorestados() : QDialog() {
  ui.setupUi(this);

  ui.Estado1treeWidget->setColumnCount(4);
  ui.Estado2treeWidget->setColumnCount(4);

 QStringList columnas;
 columnas << tr("TÍTULO") << tr("CLAVE") << tr("NODO") << tr("FORMULA");

 ui.Estado1treeWidget->setHeaderLabels(columnas);
 ui.Estado2treeWidget->setHeaderLabels(columnas);

 cambios=false;
 ui.guardarpushButton->setEnabled(false);

 ui.tabWidget3->setTabEnabled(2,false);

 ui.diariostableWidget->verticalHeader()->hide();
 ui.diariostableWidget->horizontalHeader()->hide();
 ui.diariostableWidget->setColumnWidth(0,150);

    QCheckBox *checkbox0 = new QCheckBox(diario_apertura(), this);
    ui.diariostableWidget->insertRow(0);
    ui.diariostableWidget->setCellWidget ( 0, 0, checkbox0 );
    connect((QCheckBox*)ui.diariostableWidget->cellWidget(0,0),
         SIGNAL(stateChanged(int)),this,SLOT(activarcambiado()));
    QCheckBox *checkbox1 = new QCheckBox(diario_no_asignado(), this);
    ui.diariostableWidget->insertRow(1);
    ui.diariostableWidget->setCellWidget ( 1, 0, checkbox1 );
    connect((QCheckBox*)ui.diariostableWidget->cellWidget(1,0),
         SIGNAL(stateChanged(int)),this,SLOT(activarcambiado()));
    QCheckBox *checkbox2 = new QCheckBox(diario_regularizacion(), this);
    ui.diariostableWidget->insertRow(2);
    ui.diariostableWidget->setCellWidget ( 2, 0, checkbox2 );
    connect((QCheckBox*)ui.diariostableWidget->cellWidget(2,0),
         SIGNAL(stateChanged(int)),this,SLOT(activarcambiado()));
    numdiarios=3;
    QSqlQuery query = basedatos::instancia()->selectCodigoDiariosordercodigo();
    while (query.next())
       {
           QCheckBox *checkbox = new QCheckBox(query.value(0).toString(), this);
           ui.diariostableWidget->insertRow(numdiarios);
           ui.diariostableWidget->setCellWidget ( numdiarios, 0, checkbox );
           connect((QCheckBox*)ui.diariostableWidget->cellWidget(numdiarios,0),
             SIGNAL(stateChanged(int)),this,SLOT(activarcambiado()));
	   numdiarios++;

       }


 connect(ui.titulolineEdit,SIGNAL(textChanged(QString)),SLOT(activarcambiado()));
 connect(ui.cabeceralineEdit,SIGNAL(textChanged(QString)),SLOT(activarcambiado()));
 connect(ui.observacioneslineEdit,SIGNAL(textChanged(QString)),SLOT(activarcambiado()));
 connect(ui.baseportextEdit,SIGNAL(textChanged()),SLOT(activarcambiado()));
 connect(ui.colreflineEdit,SIGNAL(textChanged(QString)),SLOT(activarcambiado()));
 connect(ui.cabeceratextEdit,SIGNAL(textChanged()),SLOT(activarcambiado()));
 connect(ui.pietextEdit,SIGNAL(textChanged()),SLOT(activarcambiado()));
 connect(ui.medioscheckBox,SIGNAL(stateChanged(int)),SLOT(activarcambiado()));
 connect(ui.graficocheckBox,SIGNAL(stateChanged(int)),SLOT(activarcambiado()));
 connect(ui.medioscheckBox,SIGNAL(stateChanged(int)),SLOT(saldosmediospulsado()));
 // connect(ui.AperturacheckBox,SIGNAL(stateChanged(int)),SLOT(activarcambiado()));
 // connect(ui.GeneralcheckBox,SIGNAL(stateChanged(int)),SLOT(activarcambiado()));
 // connect(ui.RegularizacioncheckBox,SIGNAL(stateChanged(int)),SLOT(activarcambiado()));
 connect(ui.titulo1lineEdit,SIGNAL(textChanged(QString)),SLOT(activarcambiado()));
 connect(ui.titulo2lineEdit,SIGNAL(textChanged(QString)),SLOT(activarcambiado()));
 connect(ui.nuevoelemento1pushButton,SIGNAL(clicked()),SLOT(nuevoelemento1()));
 connect(ui.insertanodo1pushButton,SIGNAL(clicked()),SLOT(insertanodohijo1()));
 connect(ui.nuevoelemento2pushButton,SIGNAL(clicked()),SLOT(nuevoelemento2()));
 connect(ui.insertanodo2pushButton,SIGNAL(clicked()),SLOT(insertanodohijo2()));
 connect(ui.borraelemento1pushButton,SIGNAL(clicked()),SLOT(borraelemento1()));
 connect(ui.borraelemento2pushButton,SIGNAL(clicked()),SLOT(borraelemento2()));
 connect(ui.segundacheckBox,SIGNAL(stateChanged(int)),SLOT(segundacheckcambio()));
 connect(ui.Apartado1lineEdit,SIGNAL(textChanged(QString)),SLOT(apartado1cambiado()));
 connect(ui.Apartado2lineEdit,SIGNAL(textChanged(QString)),SLOT(apartado2cambiado()));
 connect(ui.Estado1treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
           SLOT(asignacamposedit1()));
 connect(ui.Estado2treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
           SLOT(asignacamposedit2()));
 connect(ui.Clave1lineEdit,SIGNAL(textChanged(QString)),SLOT(clave1cambiada()));
 connect(ui.Clave2lineEdit,SIGNAL(textChanged(QString)),SLOT(clave2cambiada()));
 connect(ui.Formula1textEdit,SIGNAL(textChanged()),SLOT(formula1cambiada()));
 connect(ui.Formula2textEdit,SIGNAL(textChanged()),SLOT(formula2cambiada()));
 connect(ui.analiticacheckBox,SIGNAL(stateChanged(int)),SLOT(activarcambiado()));
 connect(ui.referenciasgroupBox,SIGNAL(toggled(bool)),SLOT(activarcambiado()));
 connect(ui.chequea1pushButton,SIGNAL(clicked()),SLOT(parte1correcta()));
 connect(ui.chequea2pushButton,SIGNAL(clicked()),SLOT(parte2correcta()));
 connect(ui.ComprobarpushButton,SIGNAL(clicked()),SLOT(okestado()));
 connect(ui.guardarpushButton,SIGNAL(clicked()),SLOT(botongrabarpulsado()));
 connect(ui.SalirpushButton,SIGNAL(clicked()),SLOT(botonsalir()));
 connect(ui.desglosecheckBox,SIGNAL(stateChanged(int)),SLOT(desglosecheckcambio()));

 connect(ui.desglosectascheckBox,SIGNAL(stateChanged(int)),SLOT(desglose_ctas_cambiado()));
 connect(ui.cheq_cuentaspushButton, SIGNAL(clicked()),SLOT(chequeacuentas()));

}


void generadorestados::activarcambiado()
{
 if (!cambios && !windowTitle().contains("(*)")) setWindowTitle(windowTitle()+QString(" (*)"));
 cambios=true;
 ui.guardarpushButton->setEnabled(true);
}


void generadorestados::desglose_ctas_cambiado()
{
  if (ui.desglosectascheckBox->isChecked())
     {
      ui.cabeceratextEdit->setEnabled(true);
      ui.pietextEdit->setEnabled(true);
      ui.cabeceralabel->setEnabled(true);
      ui.pielabel->setEnabled(true);
      ui.colreflineEdit->setText("");
      ui.referenciasgroupBox->setEnabled(false);
      ui.formulatextLabel->setEnabled(false);
      ui.baseportextEdit->setText("");
      ui.baseportextEdit->setEnabled(false);
     }
      else
           {
            if (!ui.desglosecheckBox->isChecked())
              {
               ui.cabeceratextEdit->setText("");
               ui.pietextEdit->setText("");
               ui.cabeceralabel->setEnabled(false);
               ui.pielabel->setEnabled(false);
               ui.cabeceratextEdit->setEnabled(false);
               ui.pietextEdit->setEnabled(false);
               ui.referenciasgroupBox->setEnabled(true);
               ui.formulatextLabel->setEnabled(true);
               ui.baseportextEdit->setEnabled(true);
              }
           }

  activarcambiado();

 if (ui.desglosectascheckBox->isChecked())
     ui.desglosecheckBox->setChecked(false);
}

void generadorestados::saldosmediospulsado()
{
 if (ui.medioscheckBox->isChecked())
    {
     for (int veces=0;veces<numdiarios;veces++)
       {
         ((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->setChecked(false);
       }
      ui.diariostableWidget->setDisabled(true);
    }
 else
    {
      ui.diariostableWidget->setDisabled(false);
    }

}


void generadorestados::nuevoelemento1()
{
  QTreeWidgetItem *elemento2;
  QStringList datos;
  datos << "Nuevo elemento" << "" << "" << ""; 
      if (ui.Estado1treeWidget->currentItem()==0)
        {
           QTreeWidgetItem *elemento1 = new QTreeWidgetItem(ui.Estado1treeWidget,datos);
       elemento1->setExpanded(true);
	   ui.Estado1treeWidget->setCurrentItem(elemento1);
	   asignacamposedit1();
        }
      else {
	     elemento2= ui.Estado1treeWidget->currentItem();
	     if (elemento2->parent()==0) 
	      {
	       QTreeWidgetItem *elemento3 = new QTreeWidgetItem(ui.Estado1treeWidget,datos);
           elemento3->setExpanded(true);
	      }
	     else
	         {
	           QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento2->parent(),datos);
               elemento4->setExpanded(true);
	         }
             }
  activarcambiado();
}


void generadorestados::asignacamposedit1()
{
    int guardacambiado=cambios;
    if (ui.Estado1treeWidget->currentItem()!=0) 
       {
	ui.Apartado1lineEdit->setText(ui.Estado1treeWidget->currentItem()->text(0));
	ui.Clave1lineEdit->setText(ui.Estado1treeWidget->currentItem()->text(1));
	ui.Formula1textEdit->setText(ui.Estado1treeWidget->currentItem()->text(3));
       }
    else
       {
	ui.Apartado1lineEdit->setText("");
	ui.Clave1lineEdit->setText("");
	ui.Formula1textEdit->setText("");
       }
    cambios=guardacambiado;
    if (!cambios) ui.guardarpushButton->setEnabled(false);
}


void generadorestados::insertanodohijo1()
{
  QStringList datos;
  datos << "Nuevo elemento" << "" << "" << ""; 
      if (ui.Estado1treeWidget->currentItem()==0)
        {
	   QTreeWidgetItem *elemento3 = new QTreeWidgetItem(ui.Estado1treeWidget,datos);
       elemento3->setExpanded(true);
        }
      else {
	     QTreeWidgetItem *elemento4 = new QTreeWidgetItem(ui.Estado1treeWidget->currentItem(),datos);
         elemento4->setExpanded(true);
             }
      activarcambiado();
}


void generadorestados::nuevoelemento2()
{
  QTreeWidgetItem *elemento2;
  QStringList datos;
  datos << tr("Nuevo elemento") << QString("") << QString("") << QString(""); 
      if (ui.Estado2treeWidget->currentItem()==0)
        {
           QTreeWidgetItem *elemento1 = new QTreeWidgetItem(ui.Estado2treeWidget,datos);
       elemento1->setExpanded(true);
	   ui.Estado2treeWidget->setCurrentItem(elemento1);
	   asignacamposedit2();
        }
      else {
	     elemento2= ui.Estado2treeWidget->currentItem();
	     if (elemento2->parent()==0) 
	      {
	       QTreeWidgetItem *elemento3 = new QTreeWidgetItem(ui.Estado2treeWidget,datos);
           elemento3->setExpanded(true);
	      }
	     else
	         {
	           QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento2->parent(),datos);
               elemento4->setExpanded(true);
	         }
             }
  activarcambiado();
}


void generadorestados::asignacamposedit2()
{
    int guardacambiado=cambios;
    if (ui.Estado2treeWidget->currentItem()!=0) 
       {
	ui.Apartado2lineEdit->setText(ui.Estado2treeWidget->currentItem()->text(0));
	ui.Clave2lineEdit->setText(ui.Estado2treeWidget->currentItem()->text(1));
	ui.Formula2textEdit->setText(ui.Estado2treeWidget->currentItem()->text(3));
       }
    else
       {
	ui.Apartado2lineEdit->setText("");
	ui.Clave2lineEdit->setText("");
	ui.Formula2textEdit->setText("");
       }
    cambios=guardacambiado;
    if (!cambios) ui.guardarpushButton->setEnabled(false);

}



void generadorestados::insertanodohijo2()
{
  QStringList datos;
  datos << tr("Nuevo elemento") << QString("") << QString("") << QString(""); 
      if (ui.Estado2treeWidget->currentItem()==0)
        {
	   QTreeWidgetItem *elemento3 = new QTreeWidgetItem(ui.Estado2treeWidget,datos);
       elemento3->setExpanded(true);
        }
      else {
	     QTreeWidgetItem *elemento4 = new QTreeWidgetItem(ui.Estado2treeWidget->currentItem(),datos);
         elemento4->setExpanded(true);
             }
      activarcambiado();
}


void generadorestados::borraelemento1()
{
   if (ui.Estado1treeWidget->currentItem()!=0) delete ui.Estado1treeWidget->currentItem();
   activarcambiado();
}


void generadorestados::borraelemento2()
{
   if (ui.Estado2treeWidget->currentItem()!=0) delete ui.Estado2treeWidget->currentItem();
   activarcambiado();
}


void generadorestados::segundacheckcambio()
{
  if (ui.segundacheckBox->isChecked())   ui.tabWidget3->setTabEnabled(2,true);
      else ui.tabWidget3->setTabEnabled(2,false);
  activarcambiado();
}


void generadorestados::apartado1cambiado()
{
    if (ui.Estado1treeWidget->currentItem()!=0)
	 if (ui.Estado1treeWidget->currentItem()->text(0)!=ui.Apartado1lineEdit->text())
	   {
	    ui.Estado1treeWidget->currentItem()->setText(0,ui.Apartado1lineEdit->text());
	    activarcambiado();
	   }
}

void generadorestados::apartado2cambiado()
{
  if (ui.Estado2treeWidget->currentItem()!=0)
	 if (ui.Estado2treeWidget->currentItem()->text(0)!=ui.Apartado2lineEdit->text())
	   {
	    ui.Estado2treeWidget->currentItem()->setText(0,ui.Apartado2lineEdit->text());
	    activarcambiado();
	   }
}


void generadorestados::clave1cambiada()
{
  QTreeWidgetItem  *elemento;
  QString generado;
     if (ui.Estado1treeWidget->currentItem()!=0)
        {
	 if (ui.Clave1lineEdit->text().contains(tr("LINEA")))
	     ui.Estado1treeWidget->currentItem()->setText(0,"-------------------------");
	 ui.Estado1treeWidget->currentItem()->setText(1,ui.Clave1lineEdit->text());
	 generado=ui.Clave1lineEdit->text();
	 elemento=ui.Estado1treeWidget->currentItem()->parent();
	 while (elemento!=0)
	     {
	        generado=elemento->text(1)+"."+generado;
	        elemento=elemento->parent();
	     }
	 ui.Estado1treeWidget->currentItem()->setText(2,generado);
         ui.Estado1treeWidget->sortItems ( 2, Qt::AscendingOrder);
         activarcambiado();
        }
}

void generadorestados::clave2cambiada()
{
  QTreeWidgetItem  *elemento;
  QString generado;
     if (ui.Estado2treeWidget->currentItem()!=0)
           {
	    if (ui.Clave2lineEdit->text().contains(tr("LINEA")))
	       ui.Estado2treeWidget->currentItem()->setText(0,"-------------------------");
	    ui.Estado2treeWidget->currentItem()->setText(1,ui.Clave2lineEdit->text());
	    generado=ui.Clave2lineEdit->text();
	    elemento=ui.Estado2treeWidget->currentItem()->parent();
	    while (elemento!=0)
	      {
	        generado=elemento->text(1)+"."+generado;
	        elemento=elemento->parent();
	      }
	    ui.Estado2treeWidget->currentItem()->setText(2,generado);
            ui.Estado2treeWidget->sortItems ( 2, Qt::AscendingOrder);
            activarcambiado();
            }
}


void generadorestados::formula1cambiada()
{
     if (ui.Estado1treeWidget->currentItem()!=0)
           {
	    ui.Estado1treeWidget->currentItem()->setText(3,ui.Formula1textEdit->toPlainText());
	    activarcambiado();
           }
}


void generadorestados::formula2cambiada()
{
     if (ui.Estado2treeWidget->currentItem()!=0)
           {
	    ui.Estado2treeWidget->currentItem()->setText(3,ui.Formula2textEdit->toPlainText());
	    activarcambiado();
           }
}


void generadorestados::parte1correcta()
{
   if (chequeanodos1())
             QMessageBox::information( this, tr("Generador de Estados Contables"),
  			   tr("La primera parte del estado se considera correcta" ));

}

bool generadorestados::chequeanodos1()
{
  QModelIndex indice,primero,indice2;
  QString cadelem;
  indice = ui.Estado1treeWidget->currentIndex();
  QStringList lista;
  if (indice.isValid())
   {
     while (ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()).isValid())
         ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()));
     primero=ui.Estado1treeWidget->currentIndex();
     while (true)
      {
         QTreeWidgetItem *elemento;
         elemento=ui.Estado1treeWidget->currentItem();
         ui.Estado1treeWidget->expandItem(elemento);
         if (elemento->text(1).length()==0) 
               {
	        QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR: Todas las claves deben tener un contenido.\n" ));
            return false;
	       }
         if (elemento->text(1).contains('.'))
               {
	        QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR: No se permiten puntos en las claves.\n" ));
            return false;
	       }
          if (!chequeaformula(elemento->text(3)))
               {
	        QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR en contenido de fórmula nodo %1\n" ).arg(elemento->text(2)));
            return false;
	       }
          lista << elemento->text(2);
          if (!ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()).isValid()) break;
          ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()));
       }
      // comprobamos valores repetidos en clave
      for (int i = 0; i < lista.size(); ++i) 
           {
            if (lista.count(lista.at(i))>1)
                {
	         QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR: nodos duplicados\n" ));
             return false;
                }
           }
   } else 
     {
      QMessageBox::warning( this, tr("Generador de Estados Contables"),
  			   tr("ERROR: La primera parte está vacía\n" ));
      return false;
      }
  return true;
}



bool generadorestados::chequeaformula(QString laformula)
{
  QString formula=laformula;
  formula.remove(cadpositivo());
  if (formula.length()==0) return 1;
  if (!parentesisbalanceados(formula)) return 0;
  if (formula.contains("(+")) return 0;
  if (formula.contains("(/")) return 0;
  if (formula.contains("(*")) return 0;
  if (formula.contains("/)")) return 0;
  if (formula.contains("*)")) return 0;
  if (formula.contains("+)")) return 0;
  if (formula.contains("-)")) return 0;
  if (formula.contains("()")) return 0;
  if (formula.contains(")(")) return 0;
  if (formula.contains("++")) return 0;
  if (formula.contains("+-")) return 0;
  if (formula.contains("+*")) return 0;
  if (formula.contains("+/")) return 0;
  if (formula.contains("--")) return 0;
  if (formula.contains("-+")) return 0;
  if (formula.contains("-*")) return 0;
  if (formula.contains("-/")) return 0;
  if (formula.contains("**")) return 0;
  if (formula.contains("*+")) return 0;
  if (formula.contains("*/")) return 0;
  if (formula.contains("//")) return 0;
  if (formula.contains("/+")) return 0;
  if (formula.contains("/*")) return 0;
  if (formula.endsWith("+")) return 0;
  if (formula.endsWith("-")) return 0;
  if (formula.endsWith("/")) return 0;
  if (formula.endsWith("*")) return 0;
  if (formula.endsWith("(")) return 0;
  
  if (formula.contains("][")) return 0;
  if (formula.contains("[[")) return 0;
  if (formula.contains("]]")) return 0;
  if (formula.contains("[]")) return 0;
    
  if (formula.indexOf(']',0)<formula.indexOf('[',0)) return 0;
  
  int indice=0;
  while (indice<formula.length() && formula.indexOf('[',indice)>=0)
    {
             int base=formula.indexOf('[',indice);
             if (formula.indexOf(']',indice)<0) return 0;
             if (base+1<formula.length() &&formula.indexOf('[',base+1)>=0)
                  if (formula.indexOf('[',base+1)<formula.indexOf(']',base)) return 0; 
             indice=formula.indexOf(']',indice)+1;
             if (indice<formula.length() && formula.indexOf(']',indice)>=0 && 
             	 (formula.indexOf(']',indice)<formula.indexOf('[',indice) || formula.indexOf('[',indice)<0)) return 0;
    }

// hacer lo propio con las llaves  
  if (formula.contains("}{")) return 0;
  if (formula.contains("{{")) return 0;
  if (formula.contains("}}")) return 0;
  if (formula.contains("{}")) return 0;
    
  if (formula.indexOf('}',0)<formula.indexOf('{',0)) return 0;
  
  indice=0;
  while (indice<formula.length() && formula.indexOf('{',indice)>=0)
    {
      int base=formula.indexOf('{',indice);
      if (formula.indexOf('}',indice)<0) return 0;
      if (base+1<formula.length() &&formula.indexOf('{',base+1)>=0)
	  if (formula.indexOf('{',base+1)<formula.indexOf('}',base)) return 0; 
      indice=formula.indexOf('}',indice)+1;
      if (indice<formula.length() && formula.indexOf('}',indice)>=0 && 
	(formula.indexOf('}',indice)<formula.indexOf('{',indice) || formula.indexOf('{',indice)<0)) return 0;
    }
  
// -------------------------------------------------------------  
  QString cadpru;
  cadpru=formula;
  int final;
  int inicio;
  indice=0;
  while (indice<cadpru.length())
      {
         if (cadpru[indice]=='[')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!=']') final++;
              cadpru.remove(inicio,final-inicio+1);
              continue;
           }
        if (cadpru[indice]=='{')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!='}') final++;
              cadpru.remove(inicio,final-inicio+1);
              continue;
           }
       indice++;
      }
    
  QString cadcods="()+-/*0123456789";
  
  indice=0;
  cadpru.remove(' ');
  while (indice<cadpru.length())
      {
        if (!cadcods.contains(cadpru[indice])) return 0;
        indice++;
      }
 
  return 1;   
    
}




void generadorestados::parte2correcta()
{
   if (chequeanodos2())
             QMessageBox::information( this, tr("Generador de Estados Contables"),
  			   tr("La segunda parte del estado se considera correcta" ));

}



bool generadorestados::chequeanodos2()
{
  QModelIndex indice,primero,indice2;
  QString cadelem;
  indice = ui.Estado2treeWidget->currentIndex();
  QStringList lista;
  if (indice.isValid())
   {
     while (ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()).isValid())
         ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()));
     primero=ui.Estado2treeWidget->currentIndex();
     while (true)
      {
         QTreeWidgetItem *elemento;
         elemento=ui.Estado2treeWidget->currentItem();
         ui.Estado2treeWidget->expandItem(elemento);
         if (elemento->text(1).length()==0) 
               {
	        QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR: Todas las claves deben tener un contenido.\n" ));
            return false;
	       }
         if (elemento->text(1).contains('.'))
               {
	        QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR: No se permiten puntos en las claves.\n" ));
            return false;
	       }
          if (!chequeaformula(elemento->text(3)))
               {
	        QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR en contenido de fórmula nodo %1\n" ).arg(elemento->text(2)));
            return false;
	       }
          lista << elemento->text(2);
          if (!ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()).isValid()) break;
          ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()));
       }
      // comprobamos valores repetidos en clave
      for (int i = 0; i < lista.size(); ++i) 
           {
            if (lista.count(lista.at(i))>1)
                {
	         QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR: nodos duplicados\n" ));
             return false;
                }
           }
   } else 
     {
      QMessageBox::warning( this, tr("Generador de Estados Contables"),
  			   tr("ERROR: La segunda parte está vacía\n" ));
      return false;
      }
  return true;
}


void generadorestados::okestado()
{
  if  (!estadocorrecto())
    {
             QMessageBox::warning( this, tr("Generador de Estados Contables"),
  			   tr("Hay errores pendientes" ));
     }
     else
             QMessageBox::information( this, tr("Generador de Estados Contables"),
  			   tr("El contenido del estado se considera correcto" ));

  if (ui.titulolineEdit->text().length()==0) 
    {
             QMessageBox::warning( this, tr("Generador de Estados Contables"),
  			   tr("Se le debe de asignar un título al estado contable" ));
     }
      
}


bool generadorestados::estadocorrecto()
{
  if (!chequeanodos1()) return false;
  if (ui.segundacheckBox->isChecked() && !chequeanodos2()) return false;
  if (ui.titulolineEdit->text().length()==0) return false;
 if (!chequeaformula(ui.baseportextEdit->toPlainText()))
               {
	      QMessageBox::warning( this, tr("Generador de Estados Contables"),
					tr("ERROR en contenido de fórmula base porcentaje\n" ));
         return false;
	 }
 return true;
}


void generadorestados::grabarestado()
{
    QSqlDatabase::database().transaction();
    // borramos el estado anterior de mismo título
    basedatos::instancia()->deleteEstadostitulo( ui.titulolineEdit->text() );
    basedatos::instancia()->deleteCabeceraestadostitulo( ui.titulolineEdit->text() );

    // grabamos estado modificado
 QProgressDialog progreso(tr("Grabando Estado Contable ..."), 0, 0, 4);
 progreso.setWindowTitle("Estados Contables");
 progreso.setMinimumDuration ( 0 );
 progreso.setWindowModality(Qt::WindowModal);
 progreso.show();

 progreso.setValue(1);
 QApplication::processEvents();

    QString caddir;
    for (int veces=0;veces<numdiarios;veces++)
    {
        if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->isChecked())
        {
            if (caddir.length()>0) caddir+=" ";
            caddir+=((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->text().remove('&');
        }
    }
    QString titulo2 = ui.segundacheckBox->isChecked() ? ui.titulo2lineEdit->text() : "";
    basedatos::instancia()->insert20Cabeceraestados( ui.titulolineEdit->text() ,
                                                     ui.cabeceralineEdit->text(),
                                                     ui.titulo1lineEdit->text() ,
                                                     titulo2,
                                                     ui.observacioneslineEdit->text(),
                                                     ui.baseportextEdit->toPlainText() ,
                                                     "2000/01/01" , "", "",
                                                     ui.analiticacheckBox->isChecked(),
                                                     ui.graficocheckBox->isChecked(),
                                                     ui.referenciasgroupBox->isChecked(),
                                                     ui.colreflineEdit->text(),
                                                     caddir,
                                                     ui.medioscheckBox->isChecked(), 0, 0,
                                                     ui.desglosecheckBox->isChecked(),
                                                     ui.desglosectascheckBox->isChecked(),
                                                     ui.cabeceratextEdit->toPlainText(),
                                                     ui.pietextEdit->toPlainText() );

 progreso.setValue(2);
 QApplication::processEvents();



  // grabamos árbol izquierdo

  QModelIndex indice;
  indice = ui.Estado1treeWidget->currentIndex();
  if (indice.isValid())
   {
     QApplication::processEvents();
     while (ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()).isValid())
         ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()));
     while (true)
       {
         QTreeWidgetItem *elemento;
         elemento=ui.Estado1treeWidget->currentItem();
         ui.Estado1treeWidget->expandItem(elemento);

         basedatos::instancia()->insert8Estados( ui.titulolineEdit->text() , elemento->text(0) , true, elemento->text(1) , elemento->text(2) , elemento->text(3) , 0, 0);

         if (!ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()).isValid()) break;
         ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()));
       }
   }

 progreso.setValue(3);
 QApplication::processEvents();

  // grabamos árbol derecho
  indice = ui.Estado2treeWidget->currentIndex();
  if (indice.isValid() && ui.segundacheckBox->isChecked())
   {
      QApplication::processEvents();
     while (ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()).isValid())
         ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()));
     while (true)
      {
         QTreeWidgetItem *elemento;
         elemento=ui.Estado2treeWidget->currentItem();
         ui.Estado2treeWidget->expandItem(elemento);

         basedatos::instancia()->insert6Estados( ui.titulolineEdit->text() , elemento->text(0) , false, elemento->text(1) , elemento->text(2) , elemento->text(3) );

         if (!ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()).isValid()) break;
         ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()));
       }
    }

 progreso.setValue(4);
 QApplication::processEvents();

    if (QSqlDatabase::database().commit())
        {
         cambios=false;
         setWindowTitle("Generador de Estados Contables");
         ui.guardarpushButton->setEnabled(false);
        }
        else
          QMessageBox::warning( this, tr("Generador de Estados Contables"),
  			   tr("ERROR: no se ha podido grabar el Estado Contable\n" ));

}


void generadorestados::botongrabarpulsado()
{
  if (!estadocorrecto())
         QMessageBox::warning( this, tr("Generador de Estados Contables"),
  			   tr("No es posible grabar el Estado Contable" ));
    else grabarestado();
}

generadorestados::~generadorestados()
{
    if (cambios) {
        switch( QMessageBox::information( this, tr("Generador de Estados Contables"),
          tr("Se han realizado cambios no guardados\n"
          "¿ desea grabar los cambios antes de salir ?"),
          tr("&Grabar"), tr("&Descartar"), tr("Cancelar"),
          0,      // Enter == button 0
          2 ) ) { // Escape == button 2
      case 0: // Save clicked or Alt+G pressed or Enter pressed.
          // save
           if (estadocorrecto()) {grabarestado(); accept();}
          break;
      case 1: // Discard clicked or Alt+D pressed
          reject();
          break;
      case 2: // Cancel clicked or Escape pressed
          // don't exit
          break;
      }
    } //else accept(); // no cambiado

}



void generadorestados::botonsalir()
{
  accept();
}

void generadorestados::cargaestado( QString titulo )
{
 /*
 QProgressDialog progreso(tr("Cargando Estado Contable ..."), 0, 0, 3);
 progreso.setMinimumDuration(100);
 progreso.setMaximum(3);
 progreso.setWindowTitle("Estados Contables");
 progreso.setWindowModality(Qt::WindowModal);

 progreso.setValue(1);
 */
 QApplication::processEvents();

 QSqlQuery query = basedatos::instancia()->select15Cabeceraestadostitulo(titulo);
 if ( (query.isActive()) && (query.first()) ) {
	   ui.titulolineEdit->setText(query.value(0).toString());
	   ui.cabeceralineEdit->setText(query.value(1).toString());
	   ui.titulo1lineEdit->setText(query.value(2).toString());
	   ui.titulo2lineEdit->setText(query.value(3).toString());
	   ui.observacioneslineEdit->setText(query.value(4).toString());
	   ui.baseportextEdit->setText(query.value(5).toString());
           ui.analiticacheckBox->setChecked(query.value(6).toBool());
           ui.graficocheckBox->setChecked(query.value(7).toBool());
           ui.referenciasgroupBox->setChecked(query.value(8).toBool());
           if (query.value(8).toBool())
               ui.colreflineEdit->setText(query.value(9).toString());
	   // ui.AperturacheckBox->setChecked(query.value(9).toBool());
	   // ui.GeneralcheckBox->setChecked(query.value(10).toBool());
	   // ui.RegularizacioncheckBox->setChecked(query.value(11).toBool());
           QString linea=query.value(10).toString();
           int ndiarios=linea.count(' ')+1;
           for (int veces=0;veces<ndiarios;veces++)
               {
                QString trozo=linea.section(' ',veces,veces);
                if (((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->text()==trozo)
                   ((QCheckBox*)ui.diariostableWidget->cellWidget(veces,0))->setChecked(true);
               }
	   ui.medioscheckBox->setChecked(query.value(11).toBool());
           ui.desglosecheckBox->setChecked(query.value(12).toBool());
           ui.desglosectascheckBox->setChecked(query.value(13).toBool());
           ui.cabeceratextEdit->setText(query.value(14).toString());
           ui.pietextEdit->setText(query.value(15).toString());
	  }
 if (ui.titulo2lineEdit->text().length()>0) ui.segundacheckBox->setChecked(true);

 cambios=false;
 query = basedatos::instancia()->select5apartadoEstadostituloordernodo(titulo);
 QString qnodo,qnodo2;
 QTreeWidgetItem  *elemento;
 ui.Estado1treeWidget->expandAll();
 ui.Estado2treeWidget->expandAll();

 // progreso.setValue(2);
 QCoreApplication::processEvents();


 if ( query.isActive() ) {
     while ( query.next() ) 
           {
            QCoreApplication::processEvents();
            QStringList datos;
            datos << query.value(0).toString() << query.value(2).toString() << 
                 query.value(3).toString() << query.value(4).toString(); 
            qnodo=query.value(3).toString();
            if (!qnodo.contains('.'))
             {
	         if (query.value(1).toBool()) {
      	           QTreeWidgetItem *elemento = new QTreeWidgetItem(ui.Estado1treeWidget,datos);
                   elemento->setExpanded(true);
                   ui.Estado1treeWidget->setCurrentItem(elemento);
	          }
              else  {
      	                 QTreeWidgetItem *elemento = new QTreeWidgetItem(ui.Estado2treeWidget,datos);
                     elemento->setExpanded(true);
                         ui.Estado2treeWidget->setCurrentItem(elemento);
                    }
              }
              else 
                  {
                  int pos;
                  pos=qnodo.lastIndexOf('.');
                  qnodo2=qnodo;
                  qnodo2.truncate(pos);
                  if (query.value(1).toBool())
                    {
                          /*QList<QTreeWidgetItem *> lista = ui.Estado1treeWidget->findItems(
                                                           qnodo2,Qt::MatchExactly,2 );
                         elemento=0;
                          if (lista.count()>0) elemento=lista.first(); */
                          elemento=buscanodo1(qnodo2);
                      if (elemento!=0) {
                        QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento,datos);
                     elemento4->setExpanded(true); }
                    }
		          else
		                 {
                                   /* QList<QTreeWidgetItem *> lista = ui.Estado2treeWidget->findItems(
                                                           qnodo2,Qt::MatchExactly,2 );
			           elemento=0;
                                   if (lista.count()>0) elemento=lista.first(); */
                                   elemento=buscanodo2(qnodo2);
	  	                   if (elemento!=0) {
		                   QTreeWidgetItem *elemento4 = new QTreeWidgetItem(elemento,datos);
                       elemento4->setExpanded(true); }
			         }
                 }
           }
     }

 //progreso.setValue(3);
 QApplication::processEvents();

 cambios=false;
 setWindowTitle("Generador de Estados Contables");

 ui.guardarpushButton->setEnabled(false);
}


void generadorestados::desglosecheckcambio()
{
  if (ui.desglosecheckBox->isChecked())
     {
      ui.cabeceratextEdit->setEnabled(true);
      ui.pietextEdit->setEnabled(true);
      ui.cabeceralabel->setEnabled(true);
      ui.pielabel->setEnabled(true);
      ui.colreflineEdit->setText("");
      ui.referenciasgroupBox->setEnabled(false);
      ui.formulatextLabel->setEnabled(false);
      ui.baseportextEdit->setText("");
      ui.baseportextEdit->setEnabled(false);
     }
      else 
           {
            if (!ui.desglosectascheckBox->isChecked())
               {
                ui.cabeceratextEdit->setText("");
                ui.pietextEdit->setText("");
                ui.cabeceralabel->setEnabled(false);
                ui.pielabel->setEnabled(false);
                ui.cabeceratextEdit->setEnabled(false);
                ui.pietextEdit->setEnabled(false);
                ui.referenciasgroupBox->setEnabled(true);
                ui.formulatextLabel->setEnabled(true);
                ui.baseportextEdit->setEnabled(true);
               }
           }
 if (ui.desglosecheckBox->isChecked())
     ui.desglosectascheckBox->setChecked(false);

  activarcambiado();
}


QTreeWidgetItem *generadorestados::buscanodo1(QString cadena)
{
  QModelIndex indice,elemento;
  // QTreeWidgetItem *guarda=ui.Estado1treeWidget->currentItem();
  indice = ui.Estado1treeWidget->currentIndex();
  if (indice.isValid())
   {
     while (ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()).isValid())
         ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()));
     while (true)
       {
         QTreeWidgetItem *elemento;
         elemento=ui.Estado1treeWidget->currentItem();
         ui.Estado1treeWidget->expandItem(elemento);
         if (cadena==elemento->text(2))
             return ui.Estado1treeWidget->currentItem();
         if (!ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()).isValid()) break;
         ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()));
       }
   }
  return 0;
}



QTreeWidgetItem *generadorestados::buscanodo2(QString cadena)
{
  QModelIndex indice;
  // QTreeWidgetItem *guarda=ui.Estado1treeWidget->currentItem();
  indice = ui.Estado2treeWidget->currentIndex();
  if (indice.isValid())
   {
     while (ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()).isValid())
         ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()));
     while (true)
       {
         QTreeWidgetItem *elemento;
         elemento=ui.Estado2treeWidget->currentItem();
         ui.Estado2treeWidget->expandItem(elemento);
         if (cadena==elemento->text(2))
             return ui.Estado2treeWidget->currentItem();
         if (!ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()).isValid()) break;
         ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()));
       }
   }
  return 0;
}

void generadorestados::chequeacuentas()
{
    pidenombre *p = new pidenombre();
    p->asignaetiqueta(tr("Grupos separados por comas: "));
    int cod=p->exec();
    if (cod!=QDialog::Accepted) return;
    QString cad=p->contenido();
    cad.remove(' ');
    QStringList grupos=cad.split(",");

    QProgressDialog progreso(tr("Comprobando Estado Contable ..."), 0, 0, 4);
    progreso.setWindowTitle("Estados Contables");
    progreso.setWindowModality(Qt::WindowModal);

    progreso.setValue(1);
    QApplication::processEvents();

    QModelIndex indice,primero;
    indice = ui.Estado1treeWidget->currentIndex();
    QStringList lista_cuentas;
    if (indice.isValid())
     {
       while (ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()).isValid())
           ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexAbove(ui.Estado1treeWidget->currentIndex()));
       primero=ui.Estado1treeWidget->currentIndex();
       while (true)
        {
           QTreeWidgetItem *elemento;
           elemento=ui.Estado1treeWidget->currentItem();
           ui.Estado1treeWidget->expandItem(elemento);
           QString formula=elemento->text(3);
           lista_cuentas << listacuentas(formula);
           if (!ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()).isValid()) break;
           ui.Estado1treeWidget->setCurrentIndex(ui.Estado1treeWidget->indexBelow(ui.Estado1treeWidget->currentIndex()));
        }
      }


    indice = ui.Estado2treeWidget->currentIndex();
    if (indice.isValid())
     {
       while (ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()).isValid())
           ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexAbove(ui.Estado2treeWidget->currentIndex()));
       primero=ui.Estado2treeWidget->currentIndex();
       while (true)
        {
           QTreeWidgetItem *elemento;
           elemento=ui.Estado2treeWidget->currentItem();
           ui.Estado2treeWidget->expandItem(elemento);
           QString formula=elemento->text(3);
           lista_cuentas << listacuentas(formula);
           if (!ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()).isValid()) break;
           ui.Estado2treeWidget->setCurrentIndex(ui.Estado2treeWidget->indexBelow(ui.Estado2treeWidget->currentIndex()));
         }
     }

    // QString respuesta1=listacuentas("-[700]-[701]-[702]-[703]-[704]-[705]-[706]-[708]-[709]").join(",");

    // QMessageBox::information( this, tr("Generador de Estados Contables"),respuesta);

    // consultamos todas las cuentas auxiliares en diario

    progreso.setValue(2);
    QApplication::processEvents();

    QSqlQuery q=basedatos::instancia()->cuentas_diario_grupos(grupos);

    QString respuesta;

    progreso.setValue(3);
    QApplication::processEvents();

    if (q.isActive())
        while (q.next())
        {
          bool encontrado=false;
          for (int veces=0; veces<lista_cuentas.count(); veces++)
            {
             if (q.value(0).toString().startsWith(lista_cuentas.at(veces)))
                { encontrado=true; break; }
            }
          if (!encontrado) respuesta+=q.value(0).toString()+"\n";
          QApplication::processEvents();
        }

    progreso.setValue(4);
    QApplication::processEvents();
    progreso.close();

    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->cambia_titulo(tr("Cuentas en diario sin representación en Estado"));
    n->pasa_contenido(respuesta);
    n->exec();
    delete(n);

    // QMessageBox::information( this, tr("Generador de Estados Contables"),respuesta);

}

QStringList generadorestados::listacuentas(QString cadena)
{
  QStringList lista;
  for (int veces=0; veces<cadena.length(); veces++)
     {
       if (cadena[veces]=='[')
         {
           int posini=veces+1;
           while (veces<cadena.length() && cadena[veces]!=']' && cadena[veces]!=':') veces++;
           QString cuenta=cadena.mid(posini,veces-posini).trimmed();
           cuenta=cuenta.remove('+');
           cuenta=cuenta.remove('-');
           lista << cuenta;
           // QMessageBox::information( this, tr("Generador de Estados Contables"),cuenta);
         }       
     }
  return lista;
}
