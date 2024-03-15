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

#include "planamortizaciones.h"
#include "funciones.h"
#include "basedatos.h"
#include "itemplanamort.h"
#include "buscasubcuenta.h"
#include "pidecuenta.h"
#include <QMessageBox>

CustomSqlModel2::CustomSqlModel2(QObject *parent)
        : QSqlTableModel(parent)
    {
     comadecimal=haycomadecimal(); sindecimales=!haydecimales();
    }


QVariant CustomSqlModel2::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant CustomSqlModel2::data(const QModelIndex &index, int role) const
    {
    /*"CREATE TABLE planamortizaciones ("
      "cuenta_activo         varchar(40),"
      "fecha_func            date,"
      "codigo_activo         varchar(40),"
      "cuenta_am_acum        varchar(40),"
      "cuenta_gasto          varchar(40),"
      "valor_residual        numeric(14,2),"
      "amort_personalizada   bool,"
      "coef_amort_contable   float8,"
      "coef_amort_fiscal     float8,"
      "amort_fiscal_1ejer    bool,"
      "PRIMARY KEY (cuenta_activo) )"; */
        QString vacio;
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 1)
               {
                return value.toDate().toString("dd-MM-yyyy");
               }
            if (index.column() == 5 )
               {
   	        if (value.toDouble()<0.0001 && value.toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        return value.fromValue(formateanumerosep(value.toDouble(),comadecimal, !sindecimales));
                      }
	       }
            if (index.column()==7 || index.column()==8)
               {

                if (value.toString().toDouble()<0.0001 && value.toString().toDouble()>-0.0001)
                   return value.fromValue(vacio);
                else
                      {
                        return value.fromValue(formateanumerosep(value.toDouble()*100,comadecimal,
                                                                 true));
                      }
               }
            if (index.column() == 6 || index.column()==9)
               {
                if (value.toBool()) return value.fromValue(tr("SELECCIONADO"));
                   else return value.fromValue(vacio);
               }
            if (index.column() == 10)
               {
                if (value.toBool()) return value.fromValue(tr("BAJA"));
                   else return value.fromValue(vacio);
               }
        }
        if (role == Qt::TextAlignmentRole && 
             (index.column() == 5 || index.column()==7 ||
              index.column() == 8 ))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        if (role == Qt::TextAlignmentRole && index.column()==1)
               return QVariant::fromValue(int(Qt::AlignCenter));
        return value;
    }



planamortizaciones::planamortizaciones() : QDialog() 
  {
    ui.setupUi(this);

    CustomSqlModel2 *elmodelo = new CustomSqlModel2;
    model = elmodelo;

           /*"CREATE TABLE planamortizaciones ("
             "cuenta_activo         varchar(40),"
             "fecha_func            date,"
             "codigo_activo         varchar(40),"
             "cuenta_am_acum        varchar(40),"
             "cuenta_gasto          varchar(40),"
             "valor_residual        numeric(14,2),"
             "amort_personalizada   bool,"
             "coef_amort_contable   float8,"
             "coef_amort_fiscal     float8,"
             "amort_fiscal_1ejer    bool,"
             "PRIMARY KEY (cuenta_activo) )"; */

     // model->setQuery( basedatos::instancia()->select10Planamortizacionesordercuenta_activo() );
     model->setTable("planamortizaciones");
     model->setHeaderData(0, Qt::Horizontal, tr("Cta. activo"));
     model->setHeaderData(1, Qt::Horizontal, tr("Fecha p.func."));  // Fecha
     model->setHeaderData(2, Qt::Horizontal, tr("Código activo"));
     model->setHeaderData(3, Qt::Horizontal, tr("Cta. Am.Acum."));
     model->setHeaderData(4, Qt::Horizontal, tr("Cuenta Am."));
     model->setHeaderData(5, Qt::Horizontal, tr("Valor residual"));  // Número
     model->setHeaderData(6, Qt::Horizontal, tr("Am.person."));  // lógico
     model->setHeaderData(7, Qt::Horizontal, tr("Coef.contable")); // Número
     model->setHeaderData(8, Qt::Horizontal, tr("Coef.fiscal")); // Número
     model->setHeaderData(9, Qt::Horizontal, tr("Am.fiscal 1 ej."));  // lógico
     model->select();
     ui.plantableView->setModel(model);
     ui.plantableView->setAlternatingRowColors ( true);
     ui.plantableView->setSortingEnabled(true);
     ui.plantableView->sortByColumn(0,Qt::AscendingOrder);

     comadecimal=haycomadecimal(); sindecimales=!haydecimales();

     connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevoelemento()));
     connect(ui.editarpushButton,SIGNAL(clicked()),SLOT(editarelemento()));
     connect(ui.eliminarpushButton,SIGNAL(clicked()),SLOT(eliminarelemento()));
     connect(ui.imprimirpushButton,SIGNAL(clicked()),SLOT(imprimir()));

 }



void planamortizaciones::nuevoelemento()
{
    pidecuenta *p=new pidecuenta();

    int resultado=p->exec();
    p->setWindowTitle(tr("Selección cuenta de inmovilizado"));
    QString cuenta=p->cuenta();
    delete(p);

    if (resultado != QDialog::Accepted) return;

    if (!esauxiliar(cuenta)) return;

     if (cuentaenplanamort(cuenta))
       {
         QMessageBox::warning( this, tr("Plan de amortizaciones"),
		       tr("ERROR: La cuenta seleccionada ya está en el plan de amortizaciones"));
        return;
       }
    // llamos a edición itemplanamort
    itemplanamort *elitem=new itemplanamort();
    elitem->pasaelemactivo(cuenta);
    elitem->exec();
    delete elitem;
    refrescar();
}


void planamortizaciones::refrescar()
{
    delete (model);
    CustomSqlModel2 *elmodelo = new CustomSqlModel2;
    model = elmodelo;

     // model->setQuery( basedatos::instancia()->select10Planamortizacionesordercuenta_activo() );
     model->setTable("planamortizaciones");
     model->setHeaderData(0, Qt::Horizontal, tr("Cuenta activo"));
     model->setHeaderData(1, Qt::Horizontal, tr("Fecha p.func."));  // Fecha
     model->setHeaderData(2, Qt::Horizontal, tr("Código activo"));
     model->setHeaderData(3, Qt::Horizontal, tr("Cuenta A.Acum."));
     model->setHeaderData(4, Qt::Horizontal, tr("Cuenta Am."));
     model->setHeaderData(5, Qt::Horizontal, tr("Valor residual"));  // Número
     model->setHeaderData(6, Qt::Horizontal, tr("Tabla am. person."));  // lógico
     model->setHeaderData(7, Qt::Horizontal, tr("Coef.amort.contable")); // Número
     model->setHeaderData(8, Qt::Horizontal, tr("Coef.amort.fiscal")); // Número
     model->setHeaderData(9, Qt::Horizontal, tr("Am.fiscal 1 ejer."));  // lógico
     model->select();
     ui.plantableView->setModel(model);


 //QSqlQuery p=model->query(); 
 //p.exec();
 //model->clear();
 //model->setQuery(p);
}

QString planamortizaciones::cuentaactivoactual()
{
 if (!ui.plantableView->currentIndex().isValid()) return "";
 int fila=ui.plantableView->currentIndex().row();
 return model->data(model->index(fila,0),Qt::DisplayRole).toString();
}

void planamortizaciones::editarelemento()
{
    QString cuentaactivo=cuentaactivoactual();
    if (cuentaactivo.length()==0) 
        {
         QMessageBox::warning( this, tr("Plan de amortizaciones"),
		       tr("No se ha seleccionado ningún elemento del plan de amortizaciones"));
          return;
        }
    /*"CREATE TABLE planamortizaciones ("
      "cuenta_activo         varchar(40),"
      "fecha_func            date,"
      "codigo_activo         varchar(40),"
      "cuenta_am_acum        varchar(40),"
      "cuenta_gasto          varchar(40),"
      "valor_residual        numeric(14,2),"
      "amort_personalizada   bool,"
      "coef_amort_contable   float8,"
      "coef_amort_fiscal     float8,"
      "amort_fiscal_1ejer    bool,"
      "PRIMARY KEY (cuenta_activo) )"; */
    // model->record(fila).value("debe").toDouble()
    int fila=ui.plantableView->currentIndex().row();
    itemplanamort *elitem=new itemplanamort();
    elitem->pasaelemactivo(cuentaactivo);
    /* void itemplanamort::pasainfo(QString codigo, QDate fecha, QString cuentaaa,
                                 QString cuentaam,QString coefcontable, QString residual,
                                 QString coeffiscal, bool acelerada,bool amortpersonalizada)*/
    elitem->pasainfo(model->datagen(model->index(fila,2),Qt::DisplayRole).toString(),
                                 model->record(fila).value("fecha_func").toDate(),
                                 model->record(fila).value("cuenta_am_acum").toString(),
                                 model->record(fila).value("cuenta_gasto").toString(),
                                 model->record(fila).value("coef_amort_contable").toString(),
                                 model->record(fila).value("valor_residual").toString(),
                                 model->record(fila).value("coef_amort_fiscal").toString(),
                                 model->record(fila).value("amort_fiscal_1ejer").toBool(),
                                 model->record(fila).value("amort_personalizada").toBool(),
                                 model->record(fila).value("baja").toBool(),
                                 model->record(fila).value("fecha_baja").toDate(),
                                 model->record(fila).value("motivo_baja").toString(),
                                 model->record(fila).value("cuenta_proveedor").toString(),
                                 model->record(fila).value("externo").toString()
                                );
    elitem->exec();
    delete elitem;
    refrescar();
}


void planamortizaciones::eliminarelemento()
{
    QString cuenta=cuentaactivoactual();
    if (cuenta.length()==0) return;

    if (QMessageBox::question(
            this,
            tr("Plan de amortizaciones"),
            tr("¿ Desea dar de baja el elemento seleccionado ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) )
        return ;

    basedatos::instancia()->deletePlanamortizacionescuenta_activo(cuenta);

    basedatos::instancia()->deleteAmortperscuenta(cuenta);

    basedatos::instancia()->deleteCi_amortcuenta(cuenta);

    refrescar();
}


void planamortizaciones::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=tr("plan_amortizaciones.tex");
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
    stream << "\\begin{longtable}{|c|c|p{2cm}|c|c|r|r|r|c|}" << "\n";
    stream << "\\hline" << "\n";

   stream << "\\multicolumn{9}{|c|} {\\textbf{";
   QString cadena;
   cadena=tr("PLAN AMORTIZACIONES DEL INMOVILIZADO");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // ----------------------------------------------------------------------------------------------------------------------------------------
    stream << "{\\tiny{" << tr("Cuenta activo") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha p. func.") << "}} & ";
    stream << "{\\tiny{" << tr("Cód. activo") << "}} & ";
    stream << "{\\tiny{" << tr("Cta. A.Acum") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta Am.") << "}} & ";
    stream << "{\\tiny{" << tr("Valor residual") << "}} & ";
    stream << "{\\tiny{" << tr("Coef.cont.") << "}} & ";
    stream << "{\\tiny{" << tr("Coef.fiscal") << "}} & ";
    stream << "{\\tiny{" << tr("Am.fiscal 1 ej") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // ----------------------------------------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
   stream << "\\multicolumn{9}{|c|} {\\textbf{";
   
   cadena=tr("PLAN AMORTIZACIONES DEL INMOVILIZADO");
   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // stream << tr("\\hline") << "\n";
    // stream << tr(" \\\\") << "\n";
    stream << "{\\tiny{" << tr("Cta. activo") << "}} & ";
    stream << "{\\tiny{" << tr("Fecha p. func.") << "}} & ";
    stream << "{\\tiny{" << tr("Cód. activo") << "}} & ";
    stream << "{\\tiny{" << tr("Cta. A.Acum") << "}} & ";
    stream << "{\\tiny{" << tr("Cuenta Am.") << "}} & ";
    stream << "{\\tiny{" << tr("Valor residual") << "}} & ";
    stream << "{\\tiny{" << tr("Coef.cont.") << "}} & ";
    stream << "{\\tiny{" << tr("Coef.fiscal") << "}} & ";
    stream << "{\\tiny{" << tr("Am.fiscal 1 ej") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ----------------------------------------------------------------------------------------------------------

    QSqlQuery query = basedatos::instancia()->select9Planamortizacionesordercuenta_activo();
    if ( query.isActive() ) {
          while ( query.next() )
                      {
	        stream << "{\\tiny " << query.value(0).toString() << "} & {\\tiny "; 
	        stream << query.value(1).toString() << "} & {\\tiny "; 
	        stream << filtracad(query.value(2).toString()) << "} & {\\tiny "; 
	        stream <<  query.value(3).toString() << "} & {\\tiny "; 
	        stream <<  query.value(4).toString() << "} & {\\tiny "; 
	        stream <<  formateanumerosep(query.value(5).toString().toDouble(),
                           comadecimal,!sindecimales ) << "} & {\\tiny ";
	        stream <<  formateanumero(query.value(6).toString().toDouble(),
                           comadecimal,!sindecimales) << "} & {\\tiny ";
	        stream <<  formateanumero(query.value(7).toString().toDouble(),
                           comadecimal,!sindecimales) << "} & {\\tiny ";
	        if (query.value(8).toBool()) stream << tr("SELEC.");
	        stream << "} \\\\ \n  " << "\\hline\n";
	      }
      }

   stream << "\\end{longtable}" << "\n";
   stream << "\\end{center}" << "\n";

   stream << "% FIN_CUERPO\n";
   stream << "\\end{document}" << "\n";
    
    fichero.close();


}


void planamortizaciones::imprimir()
{

   QString fichero=tr("plan_amortizaciones");

    generalatex();

   int valor=imprimelatex2(fichero);
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir plan de amortizaciones"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir plan de amortizaciones"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir plan de amortizaciones"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());
}


planamortizaciones::~planamortizaciones()
{
 delete ui.plantableView;
 delete model;
}
