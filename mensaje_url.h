#ifndef MENSAJE_URL_H
#define MENSAJE_URL_H

#include <QDialog>

namespace Ui {
class mensaje_url;
}

class mensaje_url : public QDialog
{
    Q_OBJECT

public:
    explicit mensaje_url(QWidget *parent = 0);
    ~mensaje_url();
    void get_line(QString line);
    void get_url_line(QString line);

private:
    Ui::mensaje_url *ui;
};

#endif // MENSAJE_URL_H
