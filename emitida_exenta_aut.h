#ifndef EMITIDA_EXENTA_AUT_H
#define EMITIDA_EXENTA_AUT_H

#include "etiqueta_fich.h"
#include <QDialog>

namespace Ui {
class Emitida_exenta_aut;
}

class Emitida_exenta_aut : public QDialog
{
    Q_OBJECT

public:
    explicit Emitida_exenta_aut(QWidget *parent = nullptr);
    ~Emitida_exenta_aut();
    QJsonObject info_export();
    bool verifica();

private slots:
    void on_ctabasepushButton_clicked();

    void on_ctafrapushButton_clicked();

    void on_cta_ret_pushButton_clicked();

    void on_externo_pushButton_clicked();

    void on_CtabaselineEdit_textChanged(const QString &arg1);

    void on_CtabaselineEdit_editingFinished();

    void on_FechafradateEdit_dateChanged(const QDate &date);

    void on_CtafralineEdit_textChanged(const QString &arg1);

    void on_CtafralineEdit_editingFinished();

    void on_externo_lineEdit_textChanged(const QString &arg1);

    void on_externo_lineEdit_editingFinished();

    void on_cta_ret_lineEdit_textChanged(const QString &arg1);

    void on_cta_ret_lineEdit_editingFinished();

    void on_donacion_checkBox_stateChanged(int arg1);

    void on_exportacionheckBox_toggled(bool checked);

    void on_ventas_fuera_tac_checkBox_toggled(bool checked);

    void on_AceptarButton_clicked();

    void fichero_soltado();

    void on_fichdocpushButton_clicked();

    void on_visdocpushButton_clicked();

    void on_borraasdocpushButton_clicked();

    void on_documento_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Emitida_exenta_aut *ui;
    void cargacombooperaciones();
    CustomQLabel *etiqueta;
    void actualizar_concepto();

};

#endif // EMITIDA_EXENTA_AUT_H
