#ifndef TRASPASOBORRADOR_H
#define TRASPASOBORRADOR_H

#include <QDialog>

namespace Ui {
class TraspasoBorrador;
}

class TraspasoBorrador : public QDialog
{
    Q_OBJECT

public:
    explicit TraspasoBorrador(QWidget *parent = nullptr);
    ~TraspasoBorrador();
    bool renum();

private:
    Ui::TraspasoBorrador *ui;
};

#endif // TRASPASOBORRADOR_H
