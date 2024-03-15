#ifndef EXTERNOS_CUENTA_H
#define EXTERNOS_CUENTA_H

#include <QDialog>

namespace Ui {
class externos_cuenta;
}

class externos_cuenta : public QDialog
{
    Q_OBJECT

public:
    explicit externos_cuenta(QWidget *parent = 0);
    ~externos_cuenta();
    void pasa_parametros(QString cuenta, QDate inicial, QDate final);

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::externos_cuenta *ui;
};

#endif // EXTERNOS_CUENTA_H
