#ifndef MODELO13__H
#define MODELO13__H

#include <QDialog>

namespace Ui {
class Modelo130;
}

class Modelo130 : public QDialog
{
    Q_OBJECT

public:
    explicit Modelo130(QWidget *parent = nullptr);
    ~Modelo130();
    void pasa_datos(bool qcomadecimal, bool qdecimales, QString ejercicio, QString periodo, QString ingresos, QString gastos, QString old130, QString ret);

private slots:
    void on_busca_pushButton_clicked();

    void on_auxiliar_lineEdit_editingFinished();

    void on_auxiliar_lineEdit_textChanged(const QString &arg1);

    void on_ingresos_lineEdit_textChanged(const QString &arg1);

    void on_gastos_lineEdit_textChanged(const QString &arg1);

    void on_porc20_lineEdit_textChanged(const QString &arg1);

    void on_anteriores_130_lineEdit_textChanged(const QString &arg1);

    void on_retenciones_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::Modelo130 *ui;
    bool comadecimal=true;
    bool decimales=true;
    void ctaexpandepunto();
    void calcula();
    void calcula2();
    bool genfich130(QString fichero);
};

#endif // MODELO13__H
