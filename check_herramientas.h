#ifndef CHECK_HERRAMIENTAS_H
#define CHECK_HERRAMIENTAS_H

#include <QDialog>

namespace Ui {
class Check_herramientas;
}

class Check_herramientas : public QDialog
{
    Q_OBJECT

public:
    explicit Check_herramientas(QWidget *parent = nullptr);
    ~Check_herramientas();

private slots:
    void on_comprobar_pushButton_clicked();

private:
    Ui::Check_herramientas *ui;
    void evalua();

};

#endif // CHECK_HERRAMIENTAS_H
