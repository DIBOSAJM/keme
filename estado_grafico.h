#ifndef ESTADO_GRAFICO_H
#define ESTADO_GRAFICO_H

#include <QDialog>
#include <QtCharts>

namespace Ui {
class estado_grafico;
}

class estado_grafico : public QDialog
{
    Q_OBJECT

public:
    explicit estado_grafico(QWidget *parent = 0);
    ~estado_grafico();
    void pasa_valores (QString qnombreEmpresa, QString qejercicio,
            QString qtitulo1, QString qtitulo2,
            QStringList qetiquetas_parte1,
            QStringList qetiquetas_parte2,
            QList<double> qvalores_parte1,
            QList<double> qvalores_parte2,
            QString qtitulo, bool segundo=false);

private:
    Ui::estado_grafico *ui;
    QString nombreEmpresa;
    QString ejercicio;
    QString titulo1;
    QString titulo2;
    QStringList etiquetas_parte1;
    QStringList etiquetas_parte2;
    QList<double> valores_parte1;
    QList<double> valores_parte2;
    QString titulo;
    void grafico_barras(bool segundo=false);
    void grafico_tarta(bool segundo=false);
};

#endif // ESTADO_GRAFICO_H
