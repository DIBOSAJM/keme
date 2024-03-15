#ifndef PAPELERA_H
#define PAPELERA_H

#include <QDialog>

namespace Ui {
class papelera;
}

class papelera : public QDialog
{
    Q_OBJECT

public:
    explicit papelera(QWidget *parent = 0);
    ~papelera();

private:
    Ui::papelera *ui;
    void cargar();

private slots:
    void borrar_todo();
    void refrescar();
    void recupera();
};

#endif // PAPELERA_H
