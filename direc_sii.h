#ifndef DIREC_SII_H
#define DIREC_SII_H

#include <QDialog>

namespace Ui {
class direc_sii;
}

class direc_sii : public QDialog
{
    Q_OBJECT

public:
    explicit direc_sii(QWidget *parent = 0);
    ~direc_sii();

private:
    Ui::direc_sii *ui;

private slots:
    void aceptar();
};

#endif // DIREC_SII_H
