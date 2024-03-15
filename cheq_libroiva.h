#ifndef CHEQ_LIBROIVA_H
#define CHEQ_LIBROIVA_H

#include <QDialog>
#include <QDate>

namespace Ui {
    class cheq_libroiva;
}

class cheq_libroiva : public QDialog
{
    Q_OBJECT

public:
    explicit cheq_libroiva(QWidget *parent = 0, QString qusuario=QString());
    ~cheq_libroiva();
    void pasafechas(QDate qfechaini, QDate qfechafin);
    void refrescar();
    bool hay_errores();

private:
    Ui::cheq_libroiva *ui;
    QDate fechaini, fechafin;
    bool comadecimal,decimales;
    bool errores;
    QString usuario;
    bool latex_informe();

private slots:
    void ed_cuenta();
    void edit_asiento();
    void refresca();
    void imprimir();
};

#endif // CHEQ_LIBROIVA_H
