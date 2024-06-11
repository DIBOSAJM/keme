/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

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

#include "regularizaciones.h"
#include "basedatos.h"
#include "regul_sec.h"
#include "funciones.h"
#include "directorio.h"
#include <QMessageBox>
#include <QFileDialog>

regularizaciones::regularizaciones() : QDialog() {
    ui.setupUi(this);


    ui.codigolineEdit->setFocus();
     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("cab_regulariz");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modeloreg=modelo;
     ui.tabla->setModel(modelo);
     ui.tabla->setColumnWidth(0,60);
     ui.tabla->setColumnWidth(1,350);
     ui.tabla->setEditTriggers ( QAbstractItemView::NoEditTriggers );

     ui.tabla->show();

     connect(ui.codigolineEdit,SIGNAL(returnPressed()),this,SLOT(compruebacodigo()));
     connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado()));
     connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
     connect(ui.guardarpushButton,SIGNAL(clicked()),this,SLOT(botonguardarpulsado()));
     connect(ui.cancelarpushButton,SIGNAL(clicked()),this,SLOT(botoncancelarpulsado()));
     connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botoneliminarpulsado()));
     connect(ui.tabla,SIGNAL( clicked ( QModelIndex )),this,SLOT(tablapulsada()));
     connect(ui.fasespushButton,SIGNAL(clicked()),this,SLOT(editar_fases()));
     connect(ui.exportarpushButton,SIGNAL(clicked()),this,SLOT(exportar()));

}
 
regularizaciones::~regularizaciones()
{
  delete ui.tabla;
  delete modeloreg;
}


void regularizaciones::compruebacodigo()
{
    QString caddescrip;

 if (ui.codigolineEdit->text().contains(' '))
    {
       QMessageBox::warning( this, tr("EDICIÓN DE REGULARIZACIONES"),
      tr("En la composición del código no pueden haber espacios"));
      ui.codigolineEdit->clear();
      return;
    }

 if (ui.codigolineEdit->text().length()==0)
    {
       QMessageBox::warning( this, tr("EDICIÓN DE CONCEPTOS"),
      tr("El código no puede estar vacío"));
      ui.codigolineEdit->clear();
      return;
    }


 if (!basedatos::instancia()->existecodigoregularizacion(ui.codigolineEdit->text(),&caddescrip))
    {
       ui.cancelarpushButton->setEnabled(true);
       ui.descriplineEdit->setEnabled(true);
       ui.descriplineEdit->setFocus();
       ui.codigolineEdit->setReadOnly(true);
     }
     else // codigo correcto
            {
              disconnect(ui.descriplineEdit);
              ui.descriplineEdit->setText(caddescrip);
          ui.cancelarpushButton->setEnabled(true);
          ui.borrarpushButton->setEnabled(true);
              ui.descriplineEdit->setEnabled(true);
	      ui.descriplineEdit->setFocus();
          ui.codigolineEdit->setReadOnly(true);
              connect(ui.descriplineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcioncambiada()));
              ui.fasespushButton->setEnabled(true);
              ui.exportarpushButton->setEnabled(true);
            }
}


void regularizaciones::descripcioncambiada()
{
    if(ui.descriplineEdit->hasFocus()) ui.guardarpushButton->setEnabled(true);
    ui.fasespushButton->setEnabled(false);
    ui.exportarpushButton->setEnabled(false);
}

void regularizaciones::codigocambiado()
{
    if (ui.codigolineEdit->text().length() == 0) {
      ui.descriplineEdit->setEnabled(false);
    }
    else {
      ui.descriplineEdit->setEnabled(true);
    }
}

void regularizaciones::botonguardarpulsado()
{
    QString caddescrip;
  
      if  (basedatos::instancia()->existecodigoregularizacion(ui.codigolineEdit->text(),&caddescrip))
	basedatos::instancia()->guardacambiosregulariz(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
      else 
	basedatos::instancia()->insertcab_regulariz(ui.codigolineEdit->text(),
              ui.descriplineEdit->text());
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     modeloreg->setTable("cab_regulariz");
     modeloreg->setSort(0,Qt::AscendingOrder);
     modeloreg->select();
}


void regularizaciones::botoncancelarpulsado()
{
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.fasespushButton->setEnabled(false);
     ui.exportarpushButton->setEnabled(false);
}

void regularizaciones::botoneliminarpulsado()
{
   if (QMessageBox::question(
            this,
            tr("REGULARIZACIONES"),
             tr("¿ Desea borrar el elemento seleccionado ?")) == QMessageBox::No ) return;
     basedatos::instancia()->eliminaregulariz(ui.codigolineEdit->text());
     modeloreg->select();
     ui.codigolineEdit->clear();
     ui.codigolineEdit->setReadOnly(false);
     ui.codigolineEdit->setFocus();
     ui.descriplineEdit->clear();
     ui.guardarpushButton->setDisabled(true);
     ui.cancelarpushButton->setDisabled(true);
     ui.borrarpushButton->setDisabled(true);
     ui.fasespushButton->setEnabled(false);
     ui.exportarpushButton->setEnabled(false);
}

void regularizaciones::tablapulsada()
{
     QModelIndex indiceactual=ui.tabla->currentIndex();
     if (indiceactual.isValid())
       {
        ui.codigolineEdit->setText(
           modeloreg->record(indiceactual.row()).value("codigo").toString());
        ui.descriplineEdit->setText(
           modeloreg->record(indiceactual.row()).value("descripcion").toString());
        ui.cancelarpushButton->setEnabled(true);
        ui.borrarpushButton->setEnabled(true);
        ui.descriplineEdit->setEnabled(true);
        ui.descriplineEdit->setFocus();
        ui.codigolineEdit->setReadOnly(true);
        ui.fasespushButton->setEnabled(true);
        ui.exportarpushButton->setEnabled(true);
       }
}

void regularizaciones::editar_fases()
{
  regul_sec *r = new regul_sec(ui.codigolineEdit->text(),ui.descriplineEdit->text());
  r->exec();
  delete(r);
}


void regularizaciones::exportar()
{
 QString xml;
 xml=xmlAbreTagLin(0,"REGULARIZ");
   xml+=xmlAbreTag(1,"Code");
     xml+=ui.codigolineEdit->text();
   xml+=xmlCierraTagLin(0,"Code");

   xml+=xmlAbreTag(1,"Name");
     xml+=ui.descriplineEdit->text();
   xml+=xmlCierraTagLin(0,"Name");

   xml+=xmlAbreTagLin(1,"Phases");
 
   QSqlQuery query = basedatos::instancia()->carga_regul_sec(ui.codigolineEdit->text());
   if ( query.isActive() ) {
     while (query.next())
      {
       xml+=xmlAbreTagLin(2,"Phase");

       xml+=xmlAbreTag(3,"Description");
        xml+=query.value(0).toString();
       xml+=xmlCierraTagLin(0,"Description");

       xml+=xmlAbreTag(3,"DebitAccount");
        xml+=query.value(1).toString();
       xml+=xmlCierraTagLin(0,"DebitAccount");

       xml+=xmlAbreTag(3,"CreditAccount");
        xml+=query.value(2).toString();
       xml+=xmlCierraTagLin(0,"CreditAccount");

       xml+=xmlAbreTag(3,"Accounts");
        xml+=query.value(3).toString();
       xml+=xmlCierraTagLin(0,"Accounts");

       xml+=xmlAbreTag(3,"Condition");
        // ojo: convertir símbolos < >
        QString convert=query.value(4).toString();
        convert=convert.replace(">","&gt;");
        convert=convert.replace("<","&lt;");
        xml+=convert;
       xml+=xmlCierraTagLin(0,"Condition");

       xml+=xmlCierraTagLin(2,"Phase");
      }
     }
   xml+=xmlCierraTagLin(1,"Phases");

  xml+=xmlCierraTagLin(0,"REGULARIZ");

QString nombre;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajo());
  dir->filtrar("*.reg.xml");
  dir->activa_pide_archivo("");
  if (dir->exec() == QDialog::Accepted)
    {
      nombre=dir->nuevo_conruta();
    }
   delete(dir);

#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::AnyFile);
  dialogofich.setOption(QFileDialog::DontConfirmOverwrite,true);
  dialogofich.setAcceptMode (QFileDialog::AcceptSave );
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Archivos con regularizaciones (*.reg.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO PARA EXPORTAR REGULARIZACIÓN"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
           // QString nombre=nombre.fromLocal8bit(fileNames.at(0));
           nombre=fileNames.at(0);
     }

#endif

if (nombre.isEmpty()) return;

      if (nombre.right(8)!=".reg.xml") nombre.append(".reg.xml");
      QFile regul( adapta(nombre)  );
      if (regul.exists() && QMessageBox::question(this,
           tr("¿ Sobreescribir ? -- Exportar asiento regularización"),
           tr("'%1' ya existe."
              "¿ Desea sobreescribirlo ?")
             .arg( nombre )) == QMessageBox::No)
         return ;
      if ( !regul.open( QIODevice::WriteOnly ) )
        {
         QMessageBox::warning( this, tr("REGULARIZACIONES"),tr("No ha sido posible exportar"));
         return;
        }
      QTextStream stream( &regul );
      stream.setEncoding(QStringConverter::Utf8);
      stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
      stream << "<?xml-stylesheet type=\"text/xsl\" href=\"regulariz2html.xslt\"?>";
      stream << "<!DOCTYPE REGULARIZ>\n";
      stream << xml;
      regul.close();
      QMessageBox::information( this, tr("REGULARIZACIONES"),tr("Información exportada en fichero"));
      // vemos si hace falta pasar el fichero .xslt
      QString fichxslt=dirtrabajo();
      fichxslt.append(QDir::separator());
      fichxslt.append("regulariz2html.xslt");
      // QMessageBox::information( this, tr("REGULARIZACIONES"),fichxslt);
      if (!QFile::exists(fichxslt))
         {
          QString pasa2;
          pasa2=traydoc();
          pasa2.append(QDir::separator());
          pasa2.append("regulariz2html.xslt");
          if (eswindows()) pasa2=QFile::encodeName(pasa2);
          // QMessageBox::information( this, tr("REGULARIZACIONES"),pasa2);
          // QMessageBox::information( this, tr("REGULARIZACIONES"),fichxslt);
          QFile fichero(pasa2);
          if (eswindows()) fichero.copy(QFile::encodeName(fichxslt));
             else fichero.copy(fichxslt);
         }

}
