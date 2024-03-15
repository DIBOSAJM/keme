/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#include "consplan.h"
#include "funciones.h"
#include "basedatos.h"
#include "informe_tipo_proveedor.h"
#include <QMessageBox>



consplan::consplan() : QDialog() {
  ui.setupUi(this);

#ifdef NOEDITTEX
  ui.texpushButton->hide();
#endif

  cargaarbol();
  ui.treeView->setModel(&modeloarbol);
  ui.treeView->expandAll();
  ui.treeView->resizeColumnToContents(0);
  ui.treeView->setAlternatingRowColors ( true);
  ui.treeView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

  connect(ui.imprimirpushButton,SIGNAL(clicked()),this,SLOT(imprimir()));
  connect(ui.texpushButton,SIGNAL(clicked()),SLOT(latex()));
  connect (ui.tipo_proveedor_pushButton, SIGNAL(clicked(bool)),SLOT(inf_tipo_proveed()));

}

void consplan::cargaarbol()
{

 if (!cod_longitud_variable())
  {
   QSqlQuery query = basedatos::instancia()->selectCodigodescripcionplancontablefiltro("");
   if ( query.isActive() ) {
          while ( query.next() )
              {
                insertacuentaarbol(query.value(0).toString(),query.value(1).toString());
	      }

          }
  }
  else
      {
       // creamos primero el árbol con cuentas agregadoras
       QSqlQuery query = basedatos::instancia()->selectCodigodescripcionplancontableauxiliarordercodigo(false);
       if ( query.isActive() ) {
           while ( query.next() )
              {
                insertacuentaarbol(query.value(0).toString(),query.value(1).toString());
	      }
          }
       // insertamos ahora las cuentas axiliares
       query = basedatos::instancia()->selectCodigodescripcionplancontableauxiliarordercodigo(true);
       if ( query.isActive() ) {
           while ( query.next() )
              {
                insertacuentaauxiliararbol(query.value(0).toString(),query.value(1).toString());
	      }
          }
      }

}

void consplan::insertacuentaarbol(QString cadena1,QString cadena2)
{
  QString cadactual;
	        cadactual=cadena1;
	        bool seencuentra=false;
                TreeItem *elemento;
	        // QListViewItem *elemento;
	        for (int veces=1; veces<cadactual.length(); veces++)
		{
		    QString extract;
		    extract=cadactual.left(cadactual.length()-veces);
                    bool encontrada=false;
                    elemento=modeloarbol.buscacadena(modeloarbol.raiz(),extract,&encontrada);
		    if (encontrada)
		        {
                         QList<QVariant> datos;
                         datos << cadena1 << cadena2;
                          elemento->appendChild(new TreeItem(datos, elemento));
		          seencuentra=true;
		          break;
	                        }
		}
	        if (!seencuentra)
		{
                            QList<QVariant> datos;
                            datos << cadena1 << cadena2;
                            modeloarbol.raiz()->appendChild(new TreeItem(datos, modeloarbol.raiz()));
		}
}



void consplan::insertacuentaauxiliararbol(QString cadena1,QString cadena2)
{
  QString cadactual;
	        cadactual=cadena1;
	        bool seencuentra=false;
                TreeItem *elemento;
	        // QListViewItem *elemento;
	        for (int veces=1; veces<cadactual.length(); veces++)
		{
		    QString extract;
		    extract=cadactual.left(cadactual.length()-veces);
                    bool encontrada=false;
                    elemento=modeloarbol.buscacadena(modeloarbol.raiz(),extract,&encontrada);
                    if (esauxiliar(extract)) encontrada=false;
		    if (encontrada)
		        {
                         QList<QVariant> datos;
                         datos << cadena1 << cadena2;
                          elemento->appendChild(new TreeItem(datos, elemento));
		          seencuentra=true;
		          break;
	                        }
		}
	        if (!seencuentra)
		{
                            QList<QVariant> datos;
                            datos << cadena1 << cadena2;
                            modeloarbol.raiz()->appendChild(new TreeItem(datos, modeloarbol.raiz()));
		}
}



void consplan::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("plancontable.tex");
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
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
   stream << tr("PLAN CONTABLE");
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

  QSqlQuery query = basedatos::instancia()->selectCodigodescripcionplancontablefiltro("");
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


void consplan::imprimir()
{
   generalatex();
   int valor=imprimelatex2(tr("plancontable"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir plan"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir plan"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir plan"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void consplan::latex()
{
    int valor=0;
    generalatex();
    valor=editalatex(tr("plancontable"));
    if (valor==1)
        QMessageBox::warning( this, tr("Consulta de mayor"),tr("PROBLEMAS al llamar al editor Latex"));

}

void consplan::inf_tipo_proveed() {
 informe_tipo_proveedor * i = new informe_tipo_proveedor;
 i->exec();
 delete(i);
}
