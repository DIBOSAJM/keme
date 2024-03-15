#ifndef CARTA347_H
#define CARTA347_H

#include <QDialog>

namespace Ui {
class carta347;
}

class carta347 : public QDialog
{
    Q_OBJECT

public:
    explicit carta347(QWidget *parent = 0);
    ~carta347();

private:
    Ui::carta347 *ui;

private slots:
    void aceptar();
};

#endif // CARTA347_H
