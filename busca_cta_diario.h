#ifndef BUSCA_CTA_DIARIO_H
#define BUSCA_CTA_DIARIO_H

#include <QDialog>

namespace Ui {
class Busca_cta_diario;
}

class Busca_cta_diario : public QDialog
{
    Q_OBJECT

public:
    explicit Busca_cta_diario(QWidget *parent = nullptr);
    ~Busca_cta_diario();
    QString cuenta_seleccionada();

private slots:
    void on_fecha_checkBox_stateChanged(int arg1);

    void on_refrescar_pushButton_clicked();

    void on_diario_tableWidget_cellClicked(int row, int column);

private:
    Ui::Busca_cta_diario *ui;
    QString cuenta_sel;
};

#endif // BUSCA_CTA_DIARIO_H
