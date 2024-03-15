#ifndef MODELO_182_H
#define MODELO_182_H

#include <QDialog>

namespace Ui {
class modelo_182;
}

class modelo_182 : public QDialog
{
    Q_OBJECT

public:
    explicit modelo_182(QString qusuario, QWidget *parent = 0);
    ~modelo_182();

private slots:
    void on_genera_pushButton_clicked();

    void on_carga_pushButton_clicked();

    void on_borrarpushButton_clicked();

    void on_datospushButton_clicked();

    void on_mayorpushButton_clicked();

    void on_copiarpushButton_clicked();

private:
    Ui::modelo_182 *ui;
    bool comadecimal, decimales;
    QString usuario;
    bool genfich182(QString nombre);
};

#endif // MODELO_182_H
