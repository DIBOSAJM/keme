#include "infovenci.h"
#include "ui_infovenci.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

infovenci::infovenci(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::infovenci)
{
    ui->setupUi(this);

    archivo=tr("informe_vencimientos");
    ui->inicial_dateEdit->setDate(QDate::currentDate());
    ui->final_dateEdit->setDate(QDate::currentDate());

    QString ejercicio=ejerciciodelafecha(QDate::currentDate());
    if (!ejercicio.isEmpty())
      {
       ui->inicial_dateEdit->setDate(inicioejercicio(ejercicio));
       ui->final_dateEdit->setDate(finejercicio(ejercicio));
      }

    ui->consulta_tableWidget->setColumnWidth(0,200);

    for (int filas=0; filas<5; filas++)
        for (int cols=0;cols<2; cols++)
            {
             QTableWidgetItem *newItem = new QTableWidgetItem("");
             if (cols==1) newItem->setTextAlignment (Qt::AlignRight); // | Qt::AlignVCenter);
             ui->consulta_tableWidget->setItem(filas,cols,newItem);
            }

    connect(ui->refrescarpushButton,SIGNAL(clicked(bool)),SLOT(refrescar()));
    connect(ui->inicial_dateEdit,SIGNAL(dateChanged(QDate)),SLOT(limpiar()));
    connect(ui->final_dateEdit,SIGNAL(dateChanged(QDate)),SLOT(limpiar()));
    connect(ui->diaslineEdit,SIGNAL(textChanged(QString)),SLOT(limpiar()));
    connect(ui->informepushButton,SIGNAL(clicked(bool)),SLOT(imprimir()));
    connect(ui->ed_latex_pushButton,SIGNAL(clicked(bool)),SLOT(ed_latex()));

    haycoma=haycomadecimal();
    decimales=haydecimales();

}

infovenci::~infovenci()
{
    delete ui;
}

void infovenci::limpiar()
{
    for (int filas=0; filas<5; filas++)
        for (int cols=0;cols<2; cols++)
            {
             ui->consulta_tableWidget->item(filas,cols)->setText("");
            }

}

void infovenci::refrescar()
{
 QSqlQuery q=basedatos::instancia()->vencimientos_info(ui->inicial_dateEdit->date(),
                                                          ui->final_dateEdit->date(),
                                                          ui->cuentaslineEdit->text());

 //select importe, fecha_operacion, fecha_vencimiento, fecha_realizacion, pendiente, derecho
  double suma=0;
  if (q.isActive())
     while (q.next())
       {
         // sumamos no pendientes en plazo legal
         if (!q.value(4).toBool() && !q.value(5).toBool())
           {
            if (q.value(1).toDate().daysTo(q.value(3).toDate()) <= ui->diaslineEdit->text().toInt())
                suma+=q.value(0).toDouble();
           }
       }

 ui->consulta_tableWidget->item(0,0)->setText(tr("En plazo legal abonadas: "));
 //QString cadnum;
 //cadnum.setNum(suma,'f',2);
 ui->consulta_tableWidget->item(0,1)->setText(formateanumero(suma,haycoma,decimales));

 // abonadas y en plazo legal excedido
suma=0;
 if (q.isActive())
   {
     q.first();
     do
       {
         // sumamos no pendientes fuera de plazo legal
         if (!q.value(4).toBool() && !q.value(5).toBool())
           {
            if (q.value(1).toDate().daysTo(q.value(3).toDate()) > ui->diaslineEdit->text().toInt())
                suma+=q.value(0).toDouble();
           }

       } while (q.next());
   }

 ui->consulta_tableWidget->item(1,0)->setText("Fuera de plazo legal abonadas: ");
 //cadnum.setNum(suma,'f',2);
 ui->consulta_tableWidget->item(1,1)->setText(formateanumero(suma,haycoma,decimales));


 // ahora en plazo legal excedido y no abonadas
 suma=0;
  if (q.isActive())
    {
      q.first();
      do
        {
          // sumamos pendientes con fecha actual - fecha de operación mayor que plazo legal
          // PENDIENTE
          if (q.value(4).toBool() && !q.value(5).toBool())
            {
             if (q.value(1).toDate().daysTo(ui->final_dateEdit->date()) > ui->diaslineEdit->text().toInt())
                 suma+=q.value(0).toDouble();
            }

        } while (q.next());
    }

  ui->consulta_tableWidget->item(2,0)->setText("Fuera de plazo legal no abonadas: ");
  //cadnum.setNum(suma,'f',2);
  ui->consulta_tableWidget->item(2,1)->setText(formateanumero(suma,haycoma,decimales));

 // número de días de pago cuentas a pagar
  suma=0;
  int dias=0;
  double valor=0;
   if (q.isActive())
     {
       q.first();
       do
         {
           // sumamos pagadas y días de plazo
           // PENDIENTE
           if (!q.value(4).toBool() && !q.value(5).toBool())
             {
                valor+=q.value(0).toDouble();
                suma+=q.value(0).toDouble()*q.value(1).toDate().daysTo(q.value(3).toDate());
                dias+=q.value(1).toDate().daysTo(q.value(3).toDate());
             }

         } while (q.next());
     }

   ui->consulta_tableWidget->item(3,0)->setText("Período medio de pago: ");
   //cadnum.setNum(suma/valor,'f',2);
   ui->consulta_tableWidget->item(3,1)->setText(formateanumero(suma/valor,haycoma,decimales));

 // número de días de cobro a clientes
    suma=0;
    dias=0;
    valor=0;
     if (q.isActive())
       {
         q.first();
         do
           {
             // sumamos pagadas y días de plazo
             // PENDIENTE
             if (!q.value(4).toBool() && q.value(5).toBool())
               {
                  valor+=q.value(0).toDouble();
                  suma+=q.value(0).toDouble()*q.value(1).toDate().daysTo(q.value(3).toDate());
                  dias+=q.value(1).toDate().daysTo(q.value(3).toDate());
               }

           } while (q.next());
       }

     ui->consulta_tableWidget->item(4,0)->setText("Período medio de cobro: ");
     //cadnum.setNum(suma/valor,'f',2);
     ui->consulta_tableWidget->item(4,1)->setText(formateanumero(suma/valor,haycoma,decimales));

}



void infovenci::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+archivo+".tex";
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    // stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\n";
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("INFORME VENCIMIENTOS") <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {" << tr("Período de ") << ui->inicial_dateEdit->date().toString("dd/MM/yyyy") <<
    tr(" a ") << ui->final_dateEdit->date().toString("dd/MM/yyyy") << "}}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {" << tr("Plazo máximo de pago: ") << ui->diaslineEdit->text() << "}}" << "\n";
    stream << "\\end{center}" << "\n";

    if (!ui->cuentaslineEdit->text().isEmpty())
     {
      stream << "\\begin{center}" << "\n";
      stream << "{\\textbf {" << tr("Cuentas: ") << ui->cuentaslineEdit->text() << "}}" << "\n";
      stream << "\\end{center}" << "\n";
     }

    stream << "\\\n";

     // --------------------------------------------------------------------------------------------------
    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|l|r|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("CONCEPTO") << "} & {\\scriptsize " << tr("VALOR") << "}  \\\\\n";
    stream << "\\hline\n";

    for (int filas=0; filas<5; filas++)
            {
             stream << "{\\scriptsize " << ui->consulta_tableWidget->item(filas,0)->text()
                    << "} & {\\scriptsize " <<
                       formatea_redondeado_sep(convapunto(ui->consulta_tableWidget->item(filas,1)->text()).toDouble(),
                       haycoma,decimales);
             stream << "} \\\\ \n " << "\\hline\n";
            }

    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";

     // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}



void infovenci::imprimir()
{
   generalatex();

   int valor=imprimelatex2(archivo);
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir informe"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir informe"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir informe"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}

void infovenci::ed_latex()
{
    generalatex();
    int valor=editalatex(archivo);
    if (valor==1)
        QMessageBox::warning( this, tr("Imprimer informe"),tr("PROBLEMAS al llamar al editor Latex"));

}
