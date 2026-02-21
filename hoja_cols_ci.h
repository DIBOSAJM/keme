#ifndef HOJA_COLS_CI_H
#define HOJA_COLS_CI_H

#include "qdatetime.h"
#include <QDialog>

namespace Ui {
class Hoja_cols_ci;
}

class Hoja_cols_ci : public QDialog
{
    Q_OBJECT

public:
    explicit Hoja_cols_ci(QWidget *parent = nullptr);
    ~Hoja_cols_ci();
    void pasa_titulo(QString qtitulo);
    void pasa_columnas(QStringList cols);
    void intervalo_fechas(QDate inicial, QDate final);
    void pasa_nombre_items(QStringList lista);
    void pasa_resultados_columna(int col, QStringList lista);

private slots:
    void on_informe_pushButton_clicked();

    void on_pushButton_clicked();

    void on_copiar_pushButtom_clicked();

private:
    Ui::Hoja_cols_ci *ui;
    QString titulo_estado;
    bool con_intervalo_fechas=false;
    QDate finicial,ffinal;
    void genera_text();
};

#endif // HOJA_COLS_CI_H
