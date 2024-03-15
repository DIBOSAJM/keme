#ifndef IMPOR_EXTERNOS_CSV_H
#define IMPOR_EXTERNOS_CSV_H

#include <QDialog>

namespace Ui {
class impor_externos_csv;
}

class impor_externos_csv : public QDialog
{
    Q_OBJECT

public:
    explicit impor_externos_csv(QWidget *parent = 0);
    ~impor_externos_csv();

private:
    Ui::impor_externos_csv *ui;    

private slots:
    void busca_fich_pulsado();
    void procesa_csv();
};

#endif // IMPOR_EXTERNOS_CSV_H
