#ifndef CONTENIDO_MAIL_FACTUR_H
#define CONTENIDO_MAIL_FACTUR_H

#include <QDialog>

namespace Ui {
class Contenido_mail_factur;
}

class Contenido_mail_factur : public QDialog
{
    Q_OBJECT

public:
    explicit Contenido_mail_factur(QWidget *parent = nullptr);
    ~Contenido_mail_factur();

private slots:
    void on_accept_pushButton_clicked();

private:
    Ui::Contenido_mail_factur *ui;
};

#endif // CONTENIDO_MAIL_FACTUR_H
