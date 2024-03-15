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

#include "inv_inmovilizado.h"
#include "funciones.h"
#include "basedatos.h"
#include "item_inmov.h"
#include "buscasubcuenta.h"
#include <QMessageBox>


CustomSqlModel25::CustomSqlModel25(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal(); sindecimales=!haydecimales();
    }


QVariant CustomSqlModel25::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModel25::data(const QModelIndex &index, int role) const
    {
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() >= 4 && index.column()<=6)
               {
                return value.toDate().toString("dd-MM-yyyy");
               }
            if (index.column() == 7 )
               {
   	        if (value.toDouble()<0.0001 && value.toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        return value.fromValue(formateanumerosep(value.toDouble()*100,comadecimal, !sindecimales));
                      }
	       }
            if (index.column()==8 || index.column()==9)
               {

                if (value.toString().toDouble()<0.0001 && value.toString().toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        return value.fromValue(formateanumerosep(value.toString().toDouble(),comadecimal,
                                                                 !sindecimales));
                      }
               }
        }
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 4 || index.column()==5 ||
              index.column() == 6 ))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignCenter));
        if (role == Qt::TextAlignmentRole &&
             (index.column() == 0 || index.column()==7 ||
              index.column() == 8 || index.column()==9))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        return value;
    }



inv_inmovilizado::inv_inmovilizado() : QDialog()
  {
    ui.setupUi(this);

// -------------------------------------------------------------

    CustomSqlModel25 *elmodelo = new CustomSqlModel25;
    model = elmodelo;
    // "select cta_aa, cta_ag ,concepto,fecha_inicio, fecha_compra,"
    // "fecha_ult_amort, coef, valor_adq, valor_amortizado, factura, proveedor "
    // "from amoinv order by numero";

     // model->setQuery( basedatos::instancia()->select_inv_inmovilizado() );
     model->setTable("amoinv");
     model->setHeaderData(0, Qt::Horizontal, tr("Núm."));
     model->setHeaderData(1, Qt::Horizontal, tr("Cta am.acum."));
     model->setHeaderData(2, Qt::Horizontal, tr("Cta amortiz."));
     model->setHeaderData(3, Qt::Horizontal, tr("Concepto"));
     model->setHeaderData(4, Qt::Horizontal, tr("Fecha inicio"));
     model->setHeaderData(5, Qt::Horizontal, tr("Fecha compra"));
     model->setHeaderData(6, Qt::Horizontal, tr("Ult.Amort."));
     model->setHeaderData(7, Qt::Horizontal, tr("Coeficiente"));
     model->setHeaderData(8, Qt::Horizontal, tr("Valor adq."));
     model->setHeaderData(9, Qt::Horizontal, tr("Valor amort."));
     model->setHeaderData(10, Qt::Horizontal, tr("Factura"));
     model->setHeaderData(11, Qt::Horizontal, tr("Proveedor"));
     model->select();
     // ui.latablavencipase->setModel(model);
     ui.plantableView->setModel(model);
     ui.plantableView->setAlternatingRowColors ( true);
     ui.plantableView->setEditTriggers (QAbstractItemView::NoEditTriggers);
     ui.plantableView->setSortingEnabled(true);
     ui.plantableView->sortByColumn(0,Qt::AscendingOrder);

     comadecimal=haycomadecimal(); sindecimales=!haydecimales();

     connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevoelemento()));
     connect(ui.editarpushButton,SIGNAL(clicked()),SLOT(editarelemento()));
     connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprimir()));
     connect(ui.eliminarpushButton,SIGNAL(clicked()),SLOT(borraelemento()));

 }



void inv_inmovilizado::nuevoelemento()
{
    item_inmov *elitem=new item_inmov();
    elitem->exec();
    delete elitem;
    refrescar();
}


void inv_inmovilizado::refrescar()
{
    delete (model);
    CustomSqlModel25 *elmodelo = new CustomSqlModel25;
    model = elmodelo;

    // model->setQuery( basedatos::instancia()->select_inv_inmovilizado() );
    model->setTable("amoinv");
    model->setHeaderData(0, Qt::Horizontal, tr("Núm."));
    model->setHeaderData(1, Qt::Horizontal, tr("Cta am.acum."));
    model->setHeaderData(2, Qt::Horizontal, tr("Cta amortiz."));
    model->setHeaderData(3, Qt::Horizontal, tr("Concepto"));
    model->setHeaderData(4, Qt::Horizontal, tr("Fecha inicio"));
    model->setHeaderData(5, Qt::Horizontal, tr("Fecha compra"));
    model->setHeaderData(6, Qt::Horizontal, tr("Ult.Amort."));
    model->setHeaderData(7, Qt::Horizontal, tr("Coeficiente"));
    model->setHeaderData(8, Qt::Horizontal, tr("Valor adq."));
    model->setHeaderData(9, Qt::Horizontal, tr("Valor amort."));
    model->setHeaderData(10, Qt::Horizontal, tr("Factura"));
    model->setHeaderData(11, Qt::Horizontal, tr("Proveedor"));
    model->select();

     ui.plantableView->setModel(model);


 //QSqlQuery p=model->query(); 
 //p.exec();
 //model->clear();
 //model->setQuery(p);
}

QString inv_inmovilizado::numactivoactual()
{
 if (!ui.plantableView->currentIndex().isValid()) return "";
 int fila=ui.plantableView->currentIndex().row();
 return model->data(model->index(fila,0),Qt::DisplayRole).toString();
}

void inv_inmovilizado::borraelemento()
{
   QString numactivo=numactivoactual();

   if (numactivo.isEmpty()) return;

   if (QMessageBox::question(
          this,
          tr("Inventario de inmovilizado"),
          tr("¿ Desea borrar el elemento seleccionado ?"),
          tr("&Sí"), tr("&No"),
           QString(), 0, 1 ) ==1 )
                        return;

   basedatos::instancia()->borra_reg_inv_inmovilizado(numactivo);
   refrescar();
}

void inv_inmovilizado::editarelemento()
{
    // cargamos elemento
    QSqlQuery q=basedatos::instancia()->select_reg_inv_inmovilizado (numactivoactual());
    if (q.isActive())
        if (q.next())
           {

            item_inmov *elitem=new item_inmov();

            elitem->pasadatos(q.value(0).toString(),
                              q.value(1).toString(),
                              q.value(2).toString(),
                              q.value(3).toString(),
                              q.value(4).toDate(),
                              q.value(5).toDate(),
                              q.value(6).toDate(),
                              q.value(7).toString(),
                              q.value(8).toString(),
                              q.value(9).toString(),
                              q.value(10).toString(),
                              q.value(11).toString());
            elitem->exec();
            delete elitem;

            refrescar();
           }
}




void inv_inmovilizado::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("amoinv.tex");
   QFile fichero(adapta(qfichero));

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
    stream << "\\begin{longtable}{|r|c|c|c|c|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{8}{|c|} {\\textbf{";
   QString cadena;
   cadena=tr("INVENTARIO DE INMOVILIZADO");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // ----------------------------------------------------------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("Núm") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("Cta.A.Acum") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta Am.") << "}} & ";
    stream << "{\\tiny{" << tr("Inicio") << "}} & ";
    stream << "{\\tiny{" << tr("Coef.") << "}} & ";
    stream << "{\\tiny{" << tr("Valor adq.") << "}} & ";
    stream << "{\\tiny{" << tr("Valor neto") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
   stream << "\\multicolumn{8}{|c|} {\\textbf{";
   
   cadena=tr("INVENTARIO DE INMOVILIZADO");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // stream << tr("\\hline") << "\n";
    // stream << tr(" \\\\") << "\n";
    stream << "{\\tiny{" << tr("Núm") << "}} & ";
    stream << "{\\tiny{" << tr("Concepto") << "}} & ";
    stream << "{\\tiny{" << tr("Cta.A.Acum") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta Am.") << "}} & ";
    stream << "{\\tiny{" << tr("Inicio") << "}} & ";
    stream << "{\\tiny{" << tr("Coef.") << "}} & ";
    stream << "{\\tiny{" << tr("Valor adq.") << "}} & ";
    stream << "{\\tiny{" << tr("Valor neto") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ----------------------------------------------------------------------------------------------------------

    QSqlQuery query = basedatos::instancia()->select_inv_inmovilizado ();
    if ( query.isActive() ) {
          while ( query.next() )
                      {
	        stream << "{\\tiny " << query.value(0).toString() << "} & {\\tiny "; 
                stream << filtracad(query.value(3).toString()) << "} & {\\tiny ";
                stream << query.value(1).toString() << "} & {\\tiny ";
                stream <<  query.value(2).toString() << "} & {\\tiny ";
                stream <<  query.value(4).toDate().toString("dd/MM/yyyy") << "} & {\\tiny ";
                stream <<  formateanumerosep(query.value(7).toString().toDouble()*100,
                           comadecimal,!sindecimales ) << "} & {\\tiny ";
                stream <<  formateanumero(query.value(8).toDouble(),
                           comadecimal,!sindecimales) << "} & {\\tiny ";
                stream <<  formateanumero(query.value(8).toDouble()-query.value(9).toDouble(),
                           comadecimal,!sindecimales);
	        stream << "} \\\\ \n  " << "\\hline\n";
	      }
      }

   stream << "\\end{longtable}" << "\n";
   stream << "\\end{center}" << "\n";

   stream << "% FIN_CUERPO\n";
   stream << "\\end{document}" << "\n";
    
    fichero.close();


}


void inv_inmovilizado::imprimir()
{

   QString fichero=tr("amoinv");

    generalatex();

   int valor=imprimelatex2(fichero);
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir inventario inmovilizado"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir inventario inmovilizado"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir inventario inmovilizado"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());
}


inv_inmovilizado::~inv_inmovilizado()
{
 delete ui.plantableView;
 delete model;
}
