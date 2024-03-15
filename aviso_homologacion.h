#ifndef AVISO_HOMOLOGACION_H
#define AVISO_HOMOLOGACION_H

#include <QDialog>

namespace Ui {
class aviso_homologacion;
}

class aviso_homologacion : public QDialog
{
    Q_OBJECT

public:
    explicit aviso_homologacion(QWidget *parent = 0);
    ~aviso_homologacion();

private:
    Ui::aviso_homologacion *ui;
};

#endif // AVISO_HOMOLOGACION_H
