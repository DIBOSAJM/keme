#include "hoja_cols_ci.h"
#include "qprogressdialog.h"
#include "ui_hoja_cols_ci.h"
#include <QMessageBox>
#include "basedatos.h"
#include "funciones.h"

Hoja_cols_ci::Hoja_cols_ci(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Hoja_cols_ci)
{
    ui->setupUi(this);
}

Hoja_cols_ci::~Hoja_cols_ci()
{
    delete ui;
}

void Hoja_cols_ci::pasa_titulo(QString qtitulo)
{
    titulo_estado=qtitulo;
}

void Hoja_cols_ci::pasa_columnas(QStringList cols)
{
    ui->tableWidget->setColumnCount(1+cols.count());
    QStringList nombre_columnas;
    nombre_columnas=cols;
    nombre_columnas.prepend(tr("DESCRIPCIÓN"));
    ui->tableWidget->setHorizontalHeaderLabels(nombre_columnas);
    ui->tableWidget->setColumnWidth(0,300);
}

void Hoja_cols_ci::intervalo_fechas(QDate inicial, QDate final)
{
    con_intervalo_fechas=true;
    finicial=inicial;
    ffinal=final;
}

void Hoja_cols_ci::pasa_nombre_items(QStringList lista)
{
    ui->tableWidget->setRowCount(lista.count());
    for (int v=0; v<lista.count();v++) {
        ui->tableWidget->setItem(v,0, new QTableWidgetItem(lista.at(v)));
        if (!lista.at(v).startsWith("  ")) {
            QFont font = ui->tableWidget->item(v,0)->font();
            font.setBold(true);
            ui->tableWidget->item(v,0)->setFont(font);
        }

    }
}

void Hoja_cols_ci::pasa_resultados_columna(int col, QStringList lista)
{
    for (int v=0; v<lista.count();v++) {
        QString cad_valor;
        double valor=convapunto(lista.at(v)).toDouble();
        if (valor>0.00001 || valor < -0.00001) cad_valor=lista.at(v);
        ui->tableWidget->setItem(v,col, new QTableWidgetItem(cad_valor));
        QTableWidgetItem *item=ui->tableWidget->item(v,col);
        item->setTextAlignment(Qt::AlignRight);
        if (!ui->tableWidget->item(v,0)->text().startsWith("  ")) {
            QFont font = ui->tableWidget->item(v,0)->font();
            font.setBold(true);
            ui->tableWidget->item(v,col)->setFont(font);
        }
    }
}

void Hoja_cols_ci::genera_text()
{
    QString ejercicio1="";
    QString parte1="";
    QString observaciones;
    QString cabecera;

    QProgressDialog progreso(tr("Generando Informe ..."), 0, 0, 3);
    progreso.setWindowTitle("Estados Contables");

    progreso.setValue(1);
    QApplication::processEvents();

    QSqlQuery query = basedatos::instancia()->select14Cabeceraestadostitulo(titulo_estado);

    if ( (query.isActive()) && (query.first()) ) {
        parte1=query.value(0).toString();
        ejercicio1=query.value(2).toString();
        observaciones=query.value(8).toString();
        cabecera=query.value(13).toString();
    } else return;

    QString qfichero=dirtrabajo();
    qfichero.append(QDir::separator());
    qfichero.append(tr("comparativa_analitica.tex"));
    // QMessageBox::warning( this, tr("Estados Contables"),qfichero);
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

    stream << "\\begin{landscape}" << "\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) + " - " +filtracad(ejercicio1) << "}}";
    stream << "\\end{center}";

    // investigar selección de fechas
    if (con_intervalo_fechas) {
        stream << "\\begin{center}" << "\n";
        stream << "{\\Large \\textbf {";
        stream << tr("FECHA INICIAL:" ) + finicial.toString("dd-MM-yyyy")
                      + " - " +"FECHA FINAL: " + ffinal.toString("dd-MM-yyyy") << "}}";
        stream << "\\end{center}";
    }

    stream << tr("\\begin{center}") << "\n";

    // stream << "\\begin{longtable}{|p{4.5cm}|r|r|r|r|r|r|r|r|r|r|r|r|r|}" << "\n";
    QString cad_inicio_table="\\begin{longtable}{|p{7.5cm}|";
    for (int v=0; v<ui->tableWidget->columnCount()-1;v++) cad_inicio_table+="r|";
    cad_inicio_table.append("} \n");

    stream << cad_inicio_table;
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    stream << "\\multicolumn{";

    stream << QString().setNum(ui->tableWidget->columnCount());
    stream << "}{|c|} {\\textbf{ ";
    stream << cabecera <<  "}} \\\\";
    stream << "\\hline" << "\n";
    // ---------------------------------------------------------------------------------------
    stream << "\\rowcolor{gray!30}\n";
    stream << "{\\textbf{" << parte1 << "}} & ";
    for (int v=1;v<ui->tableWidget->columnCount();v++) {
        stream << "{\\scriptsize{ " << ui->tableWidget->horizontalHeaderItem(v)->text();
        if (v<ui->tableWidget->columnCount()-1) stream << "}} & ";
          else stream << "}}";
    }
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead" << "\n";
    // ----------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}\n";
    stream << "{\\textbf{" << parte1 << "}} & ";
    for (int v=1;v<ui->tableWidget->columnCount();v++) {
        stream << "{\\scriptsize{ " << ui->tableWidget->horizontalHeaderItem(v)->text();
        if (v<ui->tableWidget->columnCount()-1) stream << "}} & ";
        else stream << "}}";
    }
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    stream << "\\hline" << "\n";

    // -------------------------------------------------------------------------------------------------------
    for (int v=0; v<ui->tableWidget->rowCount();v++) {
        for (int col=0; col<ui->tableWidget->columnCount();col++) {
            stream << "{\\scriptsize{ " << ui->tableWidget->item(v,col)->text();
            if (col<ui->tableWidget->columnCount()-1) stream << "}} & ";
            else stream << "}} \\\\" << "\n\\hline";
        }
    }
    stream << "\\hline" << "\n";
    stream << "\\end{longtable}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\\end{landscape}" << "\n";
    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();
}

void Hoja_cols_ci::on_informe_pushButton_clicked()
{
    QString qfichero;
        qfichero=tr("comparativa_analitica");

    genera_text();


    int valor=imprimelatex2(qfichero);
    if (valor==1)
        QMessageBox::warning( this, tr("Consulta de estado contable"),tr("PROBLEMAS al llamar a Latex"));
    if (valor==2)
        QMessageBox::warning( this, tr("Consulta de estado contable"),
                             tr("PROBLEMAS al llamar a 'dvips'"));
    if (valor==3)
        QMessageBox::warning( this, tr("Consulta de estado contable"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void Hoja_cols_ci::on_pushButton_clicked()
{
    QString qfichero;
    qfichero=tr("comparativa_analitica");

    genera_text();

    int valor=editalatex(qfichero);
    if (valor==1)
        QMessageBox::warning( this, tr("ESTADOS CONTABLES"),tr("PROBLEMAS al llamar al editor Latex"));

}


void Hoja_cols_ci::on_copiar_pushButtom_clicked()
{
    QClipboard *cb = QApplication::clipboard();
    QString global;

    QString ejercicio1="";
    QString parte1="";
    QString observaciones;
    QString cabecera;

    QProgressDialog progreso(tr("Generando Informe ..."), 0, 0, 3);
    progreso.setWindowTitle("Estados Contables");

    progreso.setValue(1);
    QApplication::processEvents();

    QSqlQuery query = basedatos::instancia()->select14Cabeceraestadostitulo(titulo_estado);

    if ( (query.isActive()) && (query.first()) ) {
        parte1=query.value(0).toString();
        ejercicio1=query.value(2).toString();
        observaciones=query.value(8).toString();
        cabecera=query.value(13).toString();
    } else return;

    global += filtracad(nombreempresa()) + "\t" +filtracad(ejercicio1) + "\n\n";

    // investigar selección de fechas
    if (con_intervalo_fechas) {
        global += tr("FECHA INICIAL:\t" ) + finicial.toString("dd-MM-yyyy")
                      + "\t" +"FECHA FINAL:\t" + ffinal.toString("dd-MM-yyyy") + "\n\n";
    }

    global += cabecera + "\n\n";
    global += "\t";
    for (int v=1;v<ui->tableWidget->columnCount();v++) {
        global +=  ui->tableWidget->horizontalHeaderItem(v)->text();
        if (v<ui->tableWidget->columnCount()-1) global+="\t";
    }
    global+="\n";
    // -------------------------------------------------------------------------------------------------------
    for (int v=0; v<ui->tableWidget->rowCount();v++) {
        for (int col=0; col<ui->tableWidget->columnCount();col++) {
            global +=  ui->tableWidget->item(v,col)->text();
            if (col<ui->tableWidget->columnCount()-1) global+="\t";
        }
        global+="\n";
    }
    cb->setText(global);
    QMessageBox::information( this, tr("Cálculo de Estado Contable"),
                             tr("Se ha pasado el contenido al portapapeles") );


}

