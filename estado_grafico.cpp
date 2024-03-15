#include "estado_grafico.h"
#include "ui_estado_grafico.h"
#include <QMessageBox>
#include <QtCharts>


estado_grafico::estado_grafico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::estado_grafico)
{
    ui->setupUi(this);
    ui->graf2_groupBox->hide();

}

estado_grafico::~estado_grafico()
{
    delete ui;    

}

void estado_grafico::pasa_valores (QString qnombreEmpresa,QString qejercicio,
                                   QString qtitulo1,QString qtitulo2,
                                   QStringList qetiquetas_parte1,
                                   QStringList qetiquetas_parte2,
                                   QList<double> qvalores_parte1,
                                   QList<double> qvalores_parte2,
                                   QString qtitulo, bool segundo) {
    nombreEmpresa=qnombreEmpresa;
    ejercicio=qejercicio;
    titulo1=qtitulo1;
    titulo2=qtitulo2;
    etiquetas_parte1=qetiquetas_parte1;
    etiquetas_parte2=qetiquetas_parte2;
    valores_parte1=qvalores_parte1;
    valores_parte2=qvalores_parte2;
    titulo=qtitulo;

    if (qetiquetas_parte2.count()==0)
        grafico_tarta(segundo);
      else
         grafico_barras(segundo);
}

void estado_grafico::grafico_tarta(bool segundo) {
   QPieSeries *series = new QPieSeries();
   int num=0;
   for (int veces=0;veces<etiquetas_parte1.count();veces++) {
       if (valores_parte1.at(veces)>0.001 || valores_parte1.at(veces<-0.001)) {
         series->append(etiquetas_parte1.at(veces),valores_parte1.at(veces));
         num++;
       }
   }
  /* for (int veces=0;veces<etiquetas_parte1.count();veces++) {
       series->slices().at(veces)->setLabelVisible();
   }*/

   QChart *chart = new QChart();
   chart->addSeries(series);
   chart->setTitle(titulo+" - "+ejercicio);
   //chart->legend()->hide();
   chart->legend()->setAlignment(Qt::AlignRight);

   QPieSlice *slice = series->slices().at(num-1);
       slice->setExploded();
       slice->setLabelVisible();
       slice->setPen(QPen(Qt::darkGreen, 2));
       slice->setBrush(Qt::green);


   QChartView *chartView = new QChartView(chart);

   QVBoxLayout *vbox_graf = new QVBoxLayout;
   vbox_graf->addWidget(chartView);
   if (!segundo)
     ui->graf_groupBox->setLayout(vbox_graf);
    else
       ui->graf2_groupBox->setLayout(vbox_graf);

   chartView->setRenderHint(QPainter::Antialiasing);

   if (!segundo)
     ui->graf_groupBox->show();
    else
       ui->graf2_groupBox->show();
}

void estado_grafico::grafico_barras(bool segundo) {

    if (etiquetas_parte1.count()>10) return;
    if (etiquetas_parte2.count()>10) return;
    QBarSet *set1[10], *set2[10];
    for (int veces=0;veces<10;veces++) set1[veces]=set2[veces]=NULL;
    for (int veces=0;veces<etiquetas_parte1.count();veces++) {
        set1[veces]=new QBarSet(etiquetas_parte1.at(veces));
        // QMessageBox::warning( this, tr("Estados Contables"),etiquetas_parte1.at(veces));
        *set1[veces] << valores_parte1.at(veces) << 0;
    }
    for (int veces=0;veces<etiquetas_parte2.count();veces++) {
        set2[veces]=new QBarSet(etiquetas_parte2.at(veces));
        *set2[veces] << 0 << valores_parte2.at(veces);
    }

    QStackedBarSeries *series = new QStackedBarSeries();
    for (int veces=0;veces<etiquetas_parte1.count();veces++) series->append(set1[veces]);
    for (int veces=0;veces<etiquetas_parte2.count();veces++) series->append(set2[veces]);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(titulo+" - "+ejercicio);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << titulo1 << titulo2;

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);


    QChartView *chartView = new QChartView(chart);

    QVBoxLayout *vbox_graf = new QVBoxLayout;
    vbox_graf->addWidget(chartView);
    if (!segundo)
      ui->graf_groupBox->setLayout(vbox_graf);
     else
        ui->graf2_groupBox->setLayout(vbox_graf);

    chartView->setRenderHint(QPainter::Antialiasing);

    if (!segundo)
      ui->graf_groupBox->show();
      else
        ui->graf2_groupBox->show();

}
