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

#include "ci.h"
#include <QMessageBox>
#include "funciones.h"
#include "basedatos.h"
#include <QSqlRecord>


ci::ci() : QDialog() {
    ui.setupUi(this);

    nivel=0;
    maxchar=0;

     QSqlTableModel *modelo = new QSqlTableModel;
     modelo->setTable("ci");
     modelo->setSort(0,Qt::AscendingOrder);
     modelo->select();
     modelo->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     modelo->setHeaderData(1, Qt::Horizontal, tr("DESCRIPCIÓN"));
     modelo->setHeaderData(2, Qt::Horizontal, tr("NIVEL"));
     modeloci=modelo;
     ui.tableView->setModel(modelo);
     ui.tableView->setColumnWidth(0,100);
     ui.tableView->setColumnWidth(1,300);
     ui.tableView->setColumnWidth(2,60);
     ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );


     ui.tableView->show();


     connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigocambiado(QString)));
     connect(ui.tableView,SIGNAL( clicked ( QModelIndex )),this,SLOT(filaclicked()));
     connect(ui.descripcionlineEdit,SIGNAL(textChanged(QString)),this,SLOT(descripcambiada()));
     connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));
     connect(ui.borrarpushButton,SIGNAL(clicked()),this,SLOT(botonborrar()));
     connect(ui.codigolineEdit,SIGNAL(editingFinished()),this,SLOT(codigofinedicion()));
     connect(ui.imprimirpushButton,SIGNAL(clicked()),this,SLOT(imprimir()));

}


void ci::fijanivel(int qnivel)
{
 nivel=qnivel;
 if (nivel==1)
      {
       setWindowTitle(tr("Códigos de imputación primer nivel"));
       maxchar=3;
       ui.codigolineEdit->setMaxLength ( 3 );
      }

 if (nivel==2)
      {
       setWindowTitle(tr("Códigos de imputación segundo nivel"));
       ui.codigolineEdit->setMaxLength ( 3 );
       maxchar=3;
      }

 if (nivel==3)
      {
       setWindowTitle(tr("Códigos de imputación tercer nivel"));
       maxchar=10;
       ui.codigolineEdit->setMaxLength ( 10 );
      }
 QString cadena="nivel=";
 QString cadnum;
 cadnum.setNum(nivel);
 cadena+=cadnum;
     modeloci->clear();
     modeloci->setTable("ci");
     modeloci->select();
 modeloci->setFilter(cadena);

}



ci::~ci()
{
  delete ui.tableView;
  delete modeloci;
}


void ci::codigocambiado(QString codigo)
{
 if ((nivel<3 && codigo.length()==3 && !codigo.contains(' '))
      || (nivel==3 && codigo.length()>0 && !codigo.contains(' ') ) ) 
  {
   ui.descripcionlineEdit->setEnabled(true);
   if (nivel<3) ui.descripcionlineEdit->setFocus();
  }
     else
          {
           ui.descripcionlineEdit->setText("");
           ui.descripcionlineEdit->setEnabled(false);
           return;
          }

 QString descripcion;
 QString qnivel;
 if (codigo==ciamort())
     {
       QMessageBox::warning( this, tr("Edición códigos de imputación"),
       tr("ERROR: EL CÓDIGO %1 ESTÁ RESERVADO").arg(codigo) );
       ui.borrarpushButton->setEnabled(false);
       ui.descripcionlineEdit->setText("");
       return;
     }
 bool encontrada=buscaci(codigo,&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (encontrada && nivel<3 && elnivel!=nivel)
     {
       QMessageBox::warning( this, tr("Edición códigos de imputación"),
       tr("ERROR: EL CÓDIGO %1 EXISTE Y PERTENECE A OTRO NIVEL").arg(codigo) );
       ui.borrarpushButton->setEnabled(false);
       ui.descripcionlineEdit->setText("");
       return;
     }

// if ((encontrada && nivel<3) || (encontrada && nivel==3 && codigo.length()>3))
 if (encontrada)
     {
      ui.descripcionlineEdit->setText(descripcion);
      ui.borrarpushButton->setEnabled(true);
      ui.aceptarpushButton->setEnabled(false);
     }
     else 
          { 
            ui.borrarpushButton->setEnabled(false);
            ui.descripcionlineEdit->setText("");
          }

}


void ci::codigofinedicion()
{
 if (nivel<3) return;
 if (ui.codigolineEdit->text().length()==0) return;
 QString descripcion;
 QString qnivel=0;
 bool encontrada=buscaci(ui.codigolineEdit->text(),&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (encontrada && elnivel!=nivel)
     {
       QMessageBox::warning( this, tr("Edición códigos de imputación"),
       tr("ERROR: EL CÓDIGO %1 EXISTE Y PERTENECE A OTRO NIVEL").arg(ui.codigolineEdit->text()) );
       ui.borrarpushButton->setEnabled(false);
       ui.descripcionlineEdit->setText("");
       ui.codigolineEdit->setText("");
       return;
     }
  ui.descripcionlineEdit->setFocus();
}


void ci::filaclicked()
{

 QModelIndex indiceactual=ui.tableView->currentIndex();
 if (indiceactual.isValid())
     {
      ui.codigolineEdit->setText(modeloci->record(indiceactual.row()).value("codigo").toString());
      ui.descripcionlineEdit->setText(modeloci->record(indiceactual.row()).value("descripcion").toString());
      ui.aceptarpushButton->setEnabled(false);
     }
}

void ci::descripcambiada()
{
 if (ui.descripcionlineEdit->text().length()==0) ui.aceptarpushButton->setEnabled(false);
   else ui.aceptarpushButton->setEnabled(true);
}

void ci::botonaceptar()
{
 QString cadena;
 int qnivel=0;
 QString cadnivel;
 bool encontrada=buscaci(ui.codigolineEdit->text(),&cadena,&cadnivel);
 if (cadnivel.length()>0) qnivel=cadnivel.toInt();
 if (encontrada && qnivel!=nivel)
     {
       QMessageBox::warning( this, tr("Edición de códigos de imputación"),
                             tr("ERROR, EL CÓDIGO EXISTE BAJO OTRO NIVEL"));

       ui.codigolineEdit->setText("");
       ui.descripcionlineEdit->setText("");
       return;
     }
 if (encontrada)
      {
       guardacambiosci(ui.codigolineEdit->text(),ui.descripcionlineEdit->text(),nivel);
       
      }
  else
      {
       insertaenci(ui.codigolineEdit->text(),ui.descripcionlineEdit->text(),nivel);
      }

 QMessageBox::information( this, tr("Edición de códigos de imputación"),
                             tr("Se han efectuado los cambios"));

 cadena="nivel=";
 QString cadnum;
 cadnum.setNum(nivel);
 cadena+=cadnum;
     modeloci->clear();
     modeloci->setTable("ci");
     modeloci->select();
 modeloci->setFilter(cadena);
 
 ui.codigolineEdit->setText("");
 ui.descripcionlineEdit->setText("");
 ui.borrarpushButton->setEnabled(false);

}

void ci::botonborrar()
{
   if (existeciendiario(ui.codigolineEdit->text(),nivel))
       {
        QMessageBox::warning( this, tr("Edición de códigos de imputación"),
                             tr("ERROR, EL CÓDIGO EXISTE EN EL DIARIO"));

        ui.codigolineEdit->setText("");
        ui.descripcionlineEdit->setText("");
        return;
       }

   if (existecienplanamort(ui.codigolineEdit->text(),nivel))
       {
        QMessageBox::warning( this, tr("Edición de códigos de imputación"),
                             tr("ERROR, EL CÓDIGO EXISTE EN EL PLAN DE AMORTIZACIONES"));

        ui.codigolineEdit->setText("");
        ui.descripcionlineEdit->setText("");
        return;
       }

   eliminaci(ui.codigolineEdit->text());

   QMessageBox::information( this, tr("Edición de plan contable"),
                             tr("Se ha eliminado el código seleccionado"));

   ui.codigolineEdit->setText("");
   ui.descripcionlineEdit->setText("");
 QString cadena="nivel=";
 QString cadnum;
 cadnum.setNum(nivel);
 cadena+=cadnum;
     modeloci->clear();
     modeloci->setTable("ci");
     modeloci->select();
 modeloci->setFilter(cadena);
 ui.borrarpushButton->setEnabled(false);

}

void ci::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("ci.tex");
   QString pasa;
   if (eswindows()) pasa=QFile::encodeName(qfichero);
       else pasa=qfichero;
   QFile fichero(pasa);
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}";

    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|l|p{15cm}|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{2}{|c|} {\\textbf{";
   stream << tr("CODIGOS DE IMPUTACIÓN NIVEL: ");
   QString cadnum; cadnum.setNum(nivel);
   stream << cadnum;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // --------------------------------------------------------------------------------------------------
    stream << "{\\textbf{" << tr("CÓDIGO") << "}} & ";
    stream << "{\\textbf{" << tr("DESCRIPCIÓN") << "}} ";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // --------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "{\\textbf{" << tr("CÓDIGO") << "}} & ";
    stream << "{\\textbf{" << tr("DESCRIPCIÓN") << "}} ";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // --------------------------------------------------------------------------------------------------


  QSqlQuery query = basedatos::instancia()->selectCodigodescripcioncinivelordercodigo(cadnum);
  if ( query.isActive() ) {
          while ( query.next() )
              {
	        stream << query.value(0).toString() << " & " << filtracad(query.value(1).toString());
 	        stream << " \\\\" << "\n";
                stream << "\\hline" << "\n";
	      }
      }
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}


void ci::imprimir()
{
   generalatex();
   int valor=imprimelatex2(tr("ci"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir códigos de imputación"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir códigos de imputación"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir códigos de imputación"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}
