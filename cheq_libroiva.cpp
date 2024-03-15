#include "cheq_libroiva.h"
#include "ui_cheq_libroiva.h"
#include "basedatos.h"
#include "funciones.h"
#include "datos_accesorios.h"
#include "editarasiento.h"
#include "privilegios.h"
#include <QMessageBox>
#include "externos.h"

cheq_libroiva::cheq_libroiva(QWidget *parent, QString qusuario) :
    QDialog(parent),
    ui(new Ui::cheq_libroiva)
{
    ui->setupUi(this);
    usuario=qusuario;
    comadecimal=haycomadecimal();
    decimales=haydecimales();
    errores=false;

    ui->apuntestableWidget->setColumnWidth(0,70);
    ui->apuntestableWidget->setColumnWidth(3,250);

    ui->cuentastableWidget->setColumnWidth(1,200);
    ui->cuentastableWidget->setColumnWidth(2,200);
    ui->cuentastableWidget->setColumnWidth(4,200);

    connect(ui->datospushButton,SIGNAL(clicked()),SLOT(ed_cuenta()));
    connect(ui->ed_asientopushButton,SIGNAL(clicked()),SLOT(edit_asiento()));
    connect(ui->refrescarpushButton,SIGNAL(clicked()),SLOT(refresca()));
    connect(ui->informepushButton,SIGNAL(clicked()),SLOT(imprimir()));

    if (!privilegios[edi_asiento]) ui->ed_asientopushButton->setEnabled(false);
}

cheq_libroiva::~cheq_libroiva()
{
    delete ui;
}


void cheq_libroiva::pasafechas(QDate qfechaini, QDate qfechafin)
{
    fechaini=qfechaini;
    fechafin=qfechafin;
}

void cheq_libroiva::refrescar()
{
    ui->apuntestableWidget->setRowCount(0);
    ui->cuentastableWidget->setRowCount(0);
    errores=false;
    QString cad_lista_pagar=basedatos::instancia()->selectCuentasapagarconfiguracion().remove(' ');
    QString cad_lista_cobrar=basedatos::instancia()->selectCuentasacobrarconfiguracion().remove(' ');

    QSqlQuery q=basedatos::instancia()->registros_libro_facturas(fechaini, fechafin);
    QStringList listacuentas;
    if (q.isActive())
        while (q.next())
           {
            /*QString cadena="select d.asiento, l.pase, l.cuenta_fra, l.base_iva+l.cuota_iva, "
                       "l.soportado, l.aib, l.eib, l.autofactura, l.autofactura_no_ue, "
                       "l.pr_servicios_ue, l.isp_op_interiores, l.importacion, "
                       "l.exportacion, d.fecha, l.nombre,l.cif, d.cuenta from libroiva l, diario d where d.fecha>='";
            cadena+=inicio.toString("yyyy-MM-dd");
            cadena+="' and d.fecha<='";
            cadena+=final.toString("yyyy-MM-dd");
            cadena+="' order by d.fecha";*/
            if ((q.value(5).toBool() || q.value(7).toBool() || q.value(8).toBool()
                || q.value(10).toBool()) && ! q.value(4).toBool()) continue;
            QString cuenta=q.value(2).toString();
            bool correcta=false;
            if (q.value(4).toBool())
               // soportado
               {
                QStringList lista=cad_lista_pagar.split(",");
                for (int veces=0; veces<lista.count(); veces++)
                  {
                    if (cuenta.startsWith(lista.at(veces)))
                      {
                       correcta=true;
                       break;
                      }
                  }
                }
                else
                    {
                     QStringList lista=cad_lista_cobrar.split(",");
                     for (int veces=0; veces<lista.count(); veces++)
                        {
                         if (cuenta.startsWith(lista.at(veces)))
                           {
                            correcta=true;
                            break;
                           }
                        }
                    }
            if (q.value(17).toDouble()>0.001 || q.value(17).toDouble()<-0.001) // cuota distinta de 0
              {
                if (q.value(4).toBool()) // de nuevo IVA soportado
                  {
                    if (!escuentadeivasoportado(q.value(16).toString()))
                       {
                        correcta=false;
                       }
                  }
                  else
                      {
                       if (!escuentadeivarepercutido(q.value(16).toString()))
                         {
                          correcta=false;
                         }
                      }
               }
            if (!correcta)
                 {
                    // insertamos datos en apuntesTableWidget
                    errores=true;
                    int fila=ui->apuntestableWidget->rowCount();
                    ui->apuntestableWidget->insertRow(fila);
                    QTableWidgetItem *newItem1 = new QTableWidgetItem(q.value(1).toString());
                    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui->apuntestableWidget->setItem(fila,0,newItem1);
                    QTableWidgetItem *newItem2 = new QTableWidgetItem(q.value(13).toDate().toString("dd-MM-yyyy"));
                    newItem2->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
                    ui->apuntestableWidget->setItem(fila,1,newItem2);
                    ui->apuntestableWidget->setItem(fila,2,new QTableWidgetItem(cuenta));
                    ui->apuntestableWidget->setItem(fila,3,new QTableWidgetItem(descripcioncuenta(cuenta)));

                    QTableWidgetItem *newItem3 = new QTableWidgetItem(
                            formateanumero(q.value(3).toDouble(),comadecimal,decimales));
                    newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui->apuntestableWidget->setItem(fila,4,newItem3);
                    ui->apuntestableWidget->setItem(fila,5,new QTableWidgetItem(q.value(16).toString()));
                    continue;
                 }
              QString externo=q.value(18).toString();
              if (!externo.isEmpty())
                 {
                  if (listacuentas.contains(cuenta)) continue;
                    else listacuentas << cuenta;
                 }
             // ahora toca cuentas sin datos auxiliares, y nif erróneo - sin razón social
              if (!externo.isEmpty())
                {
                  QString razon=basedatos::instancia()->razon_externo(externo);
                  QString nif=basedatos::instancia()->cif_externo(externo);
                  QString concepto;
                  bool nif_correcto=isNifCifNie(nif)>0;
                  concepto=!nif_correcto ? tr("NIF incorrecto") : "";
                  if (razon.isEmpty())
                    {
                     if (!concepto.isEmpty()) concepto+=" - ";
                     concepto+=tr("Externo sin razón");
                    }
                  if (!nif_correcto || razon.isEmpty())
                    {
                      errores=true;
                      int fila=ui->cuentastableWidget->rowCount();
                      ui->cuentastableWidget->insertRow(fila);
                      ui->cuentastableWidget->setItem(fila,0,new QTableWidgetItem(cuenta));
                      ui->cuentastableWidget->setItem(fila,1,new QTableWidgetItem(descripcioncuenta(cuenta)));
                      ui->cuentastableWidget->setItem(fila,2,new QTableWidgetItem(razon));
                      ui->cuentastableWidget->setItem(fila,3,new QTableWidgetItem(nif));
                      ui->cuentastableWidget->setItem(fila,4,new QTableWidgetItem(concepto));
                      ui->cuentastableWidget->setItem(fila,5,new QTableWidgetItem(externo));
                    }
                }
              if (externo.isEmpty() && (!q.value(14).toString().isEmpty() ||
                  !q.value(15).toString().isEmpty())) // 14 y 15
               {
                  QString razon=q.value(14).toString();
                  QString nif=q.value(15).toString();
                  QString concepto;
                  bool nif_correcto=isNifCifNie(nif)>0;
                  concepto=!nif_correcto ? tr("NIF incorrecto") : "";
                  if (razon.isEmpty())
                    {
                     if (!concepto.isEmpty()) concepto+=" - ";
                     concepto+=tr("Cuenta sin razón");
                    }
                  if (!nif_correcto || razon.isEmpty())
                    {
                      errores=true;
                      int fila=ui->cuentastableWidget->rowCount();
                      ui->cuentastableWidget->insertRow(fila);
                      ui->cuentastableWidget->setItem(fila,0,new QTableWidgetItem(cuenta));
                      ui->cuentastableWidget->setItem(fila,1,new QTableWidgetItem(descripcioncuenta(cuenta)));
                      ui->cuentastableWidget->setItem(fila,2,new QTableWidgetItem(razon));
                      ui->cuentastableWidget->setItem(fila,3,new QTableWidgetItem(nif));
                      ui->cuentastableWidget->setItem(fila,4,new QTableWidgetItem(concepto));
                    }
               }
              if (!basedatos::instancia()->hay_datos_accesorios(cuenta) && externo.isEmpty())
                {
                  errores=true;
                  int fila=ui->cuentastableWidget->rowCount();
                  ui->cuentastableWidget->insertRow(fila);
                  ui->cuentastableWidget->setItem(fila,0,new QTableWidgetItem(cuenta));
                  ui->cuentastableWidget->setItem(fila,1,new QTableWidgetItem(descripcioncuenta(cuenta)));
                      ui->cuentastableWidget->setItem(fila,2,new QTableWidgetItem(""));
                      ui->cuentastableWidget->setItem(fila,3,new QTableWidgetItem(""));
                  ui->cuentastableWidget->setItem(fila,4,new QTableWidgetItem(tr("Cuenta sin datos accesorios")));
                  continue;
                }
              QString razon,nif;
              basedatos::instancia()->razon_nif_datos(cuenta,&razon,&nif);
              QString concepto;
              bool nif_correcto=true;
              if (q.value(5).toBool() || q.value(6).toBool() || q.value(7).toBool() ||
                  q.value(8).toBool()
                  || q.value(9).toBool() || q.value(11).toBool() || q.value(12).toBool())
                 nif_correcto=true;
                else
                  nif_correcto=isNifCifNie(nif)>0;
              concepto=!nif_correcto ? tr("NIF incorrecto") : "";
              if (razon.isEmpty())
                {
                 if (!concepto.isEmpty()) concepto+=" - ";
                 concepto+=tr("Cuenta sin razón");
                }
              if (!nif_correcto || razon.isEmpty())
                {
                  errores=true;
                  int fila=ui->cuentastableWidget->rowCount();
                  ui->cuentastableWidget->insertRow(fila);
                  ui->cuentastableWidget->setItem(fila,0,new QTableWidgetItem(cuenta));
                  ui->cuentastableWidget->setItem(fila,1,new QTableWidgetItem(descripcioncuenta(cuenta)));
                  ui->cuentastableWidget->setItem(fila,2,new QTableWidgetItem(razon));
                  ui->cuentastableWidget->setItem(fila,3,new QTableWidgetItem(nif));
                  ui->cuentastableWidget->setItem(fila,4,new QTableWidgetItem(concepto));
                }
            } // while q.next()

}

bool cheq_libroiva::hay_errores()
{
    return errores;
}

void cheq_libroiva::ed_cuenta()
{
    if (ui->cuentastableWidget->rowCount()<=0) return;
    QString cuenta=ui->cuentastableWidget->item(
            ui->cuentastableWidget->currentRow(),0)->text();
    QString externo;
    if (ui->cuentastableWidget->item(ui->cuentastableWidget->currentRow(),5)!=NULL)
      {
       externo=ui->cuentastableWidget->item(ui->cuentastableWidget->currentRow(),5)->text();
       if (!externo.isEmpty())
          {
           externos *e = new externos();
           e->pasa_codigo(externo);
           e->exec();
           delete(e);
           return;
          }
      }
    if (cuenta.length()>0)
      {
        datos_accesorios *d = new datos_accesorios;
        d->cargacodigo(cuenta);
        d->exec();
        delete(d);
      }
}


void cheq_libroiva::edit_asiento()
{
 // necesitamos conocer número de asiento y ejercicio


    if (ui->apuntestableWidget->currentItem()==0) return;
    QString pase=ui->apuntestableWidget->item(ui->apuntestableWidget->currentRow(),0)->text();
    QString asiento=basedatos::instancia()->asientodepase(pase);
    QDate fecha_asiento= basedatos::instancia()->fecha_pase_diario(pase);
    QString ejercicio=basedatos::instancia()->selectEjerciciodelafecha (fecha_asiento);

    QString qdiario;
    qdiario=diariodeasiento(asiento,ejercicio);

    if (qdiario==diario_apertura() || qdiario==diario_cierre() || qdiario==diario_regularizacion())
       {
         QMessageBox::warning( this, tr("Chequeo libro facturas"),
                               tr("ERROR: No se pueden editar asientos del diario de apertura, "
                                  "regularización o cierre\n"));
         return;
       }

    if (!existeasiento(asiento,ejercicio))
       {
         QMessageBox::warning( this, tr("Chequeo libro facturas"),
                               tr("ERROR: El asiento seleccionado ya no existe"));
         return;
       }

 editarasiento(asiento,usuario,ejercicio);
}

void cheq_libroiva::refresca()
{
    refrescar();
}


bool cheq_libroiva::latex_informe()
{
QString qfichero=dirtrabajo();
qfichero.append(QDir::separator());
qfichero+=tr("incidencias_iva");
qfichero+=".tex";
QFile fichero(adapta(qfichero));
if (! fichero.open( QIODevice::WriteOnly ) ) return false;

 QTextStream stream( &fichero );
 stream.setEncoding(QStringConverter::Utf8);
 stream << cabeceralatex();
 stream << margen_extra_latex();

 stream << "\\begin{center}" << "\n";
 stream << "{\\Large \\textbf {";
 stream << filtracad(nombreempresa()) << "}}";
 stream << "\\end{center}";

 stream << "\\begin{center}" << "\n";
 stream << "\\begin{longtable}{|r|c|c|p{4cm}|r|}" << "\n";
 stream << "\\hline" << "\n";

 stream << "\\multicolumn{5}{|c|} {\\textbf{";

QString cadena=filtracad(tr("Apuntes con cuentas de factura no correctas - "));
cadena+=ejerciciodelafecha(fechaini);
 // --------------------------------------------------------------------------------------
 stream << cadena;
 stream <<  "}} \\\\";
 stream << "\\hline" << "\n";
 // ---------------------------------------------------------------------------------------
 stream << "{\\tiny{" << tr("Apunte") << "}} & ";
 stream << "{\\tiny{" << tr("Fecha") << "}} & ";
 stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
 stream << "{\\tiny{" << tr("Descripción") << "}} & ";
 stream << "{\\tiny{" << tr("Importe") << "}}";
 stream << " \\\\" << "\n";
 stream << "\\hline" << "\n";
 stream << "\\endfirsthead";
 // --------------------------------------------------------------------------------------------------
 stream << "\\hline" << "\n";
 stream << "{\\tiny{" << tr("Apunte") << "}} & ";
 stream << "{\\tiny{" << tr("Fecha") << "}} & ";
 stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
 stream << "{\\tiny{" << tr("Descripción") << "}} & ";
 stream << "{\\tiny{" << tr("Importe") << "}}";
 stream << " \\\\" << "\n";
 stream << "\\hline" << "\n";
 stream << "\\endhead" << "\n";
 // -----------------------------------------------------------------------------------------------
 for (int veces=0; veces<ui->apuntestableWidget->rowCount(); veces++)
    {
     stream << "{\\tiny " << ui->apuntestableWidget->item(veces,0)->text() << "} & {\\tiny ";
     stream << ui->apuntestableWidget->item(veces,1)->text() << "} & {\\tiny ";
     stream << ui->apuntestableWidget->item(veces,2)->text() << "} & {\\tiny ";
     stream << ui->apuntestableWidget->item(veces,3)->text() << "} & {\\tiny ";
     stream << ui->apuntestableWidget->item(veces,4)->text() << "} \\\\ \n  " << "\\hline\n";
    }

stream << "\\end{longtable}" << "\n";
stream << "\\end{center}" << "\n";

// segunda tabla

stream << "\\begin{center}" << "\n";
stream << "\\begin{longtable}{|c|p{3cm}|p{3cm}|c|p{3cm}|}" << "\n";
stream << "\\hline" << "\n";

stream << "\\multicolumn{5}{|c|} {\\textbf{";

cadena=filtracad(tr("Incidencias en cuentas de factura - "));
cadena+=ejerciciodelafecha(fechaini);
// --------------------------------------------------------------------------------------
stream << cadena;
stream <<  "}} \\\\";
stream << "\\hline" << "\n";
// ---------------------------------------------------------------------------------------
stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
stream << "{\\tiny{" << tr("Descripción") << "}} & ";
stream << "{\\tiny{" << tr("Razón") << "}} & ";
stream << "{\\tiny{" << tr("NIF") << "}} & ";
stream << "{\\tiny{" << tr("Incidencia") << "}}";
stream << " \\\\" << "\n";
stream << "\\hline" << "\n";
stream << "\\endfirsthead";
// --------------------------------------------------------------------------------------------------
stream << "\\hline" << "\n";
stream << "{\\tiny{" << tr("Cuenta") << "}} & ";
stream << "{\\tiny{" << tr("Descripción") << "}} & ";
stream << "{\\tiny{" << tr("Razón") << "}} & ";
stream << "{\\tiny{" << tr("NIF") << "}} & ";
stream << "{\\tiny{" << tr("Incidencia") << "}}";
stream << " \\\\" << "\n";
stream << "\\hline" << "\n";
stream << "\\endhead" << "\n";
// -----------------------------------------------------------------------------------------------
for (int veces=0; veces<ui->cuentastableWidget->rowCount(); veces++)
   {
    stream << "{\\tiny " << ui->cuentastableWidget->item(veces,0)->text() << "} & {\\tiny ";
    stream << ui->cuentastableWidget->item(veces,1)->text() << "} & {\\tiny ";
    stream << ui->cuentastableWidget->item(veces,2)->text() << "} & {\\tiny ";
    stream << ui->cuentastableWidget->item(veces,3)->text() << "} & {\\tiny ";
    stream << ui->cuentastableWidget->item(veces,4)->text() << "} \\\\ \n  " << "\\hline\n";
   }

stream << "\\end{longtable}" << "\n";
stream << "\\end{center}" << "\n";

 stream << "% FIN_CUERPO\n";
 stream << "\\end{document}" << "\n";

 fichero.close();

 return true;
}

void cheq_libroiva::imprimir()
{
    if (!latex_informe()) return;
    int valor=consultalatex2(tr("incidencias_iva"));
    if (valor==1)
        QMessageBox::warning( this, tr("Libros de FACTURAS"),tr("PROBLEMAS al llamar a Latex"));
    if (valor==2)
        QMessageBox::warning( this, tr("Libros de FACTURAS"),
                              tr("PROBLEMAS al llamar a ")+visordvi());
}
