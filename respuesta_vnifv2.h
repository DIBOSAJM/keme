#ifndef RESPUESTA_VNIFV2_H
#define RESPUESTA_VNIFV2_H

#include <QDialog>
#include <QDomDocument>

namespace Ui {
class respuesta_vnifv2;
}

class respuesta_vnifv2 : public QDialog
{
    Q_OBJECT

public:
    explicit respuesta_vnifv2(QWidget *parent = 0);
    ~respuesta_vnifv2();
    void pasa_dom(QDomDocument qdom);

private slots:
    void on_xml_pushButton_clicked();

    void on_copiar_pushButton_clicked();

private:
    Ui::respuesta_vnifv2 *ui;
    QDomDocument doc;
};

#endif // RESPUESTA_VNIFV2_H
