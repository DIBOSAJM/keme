#ifndef DIRECTORIO_H
#define DIRECTORIO_H

#include <QDialog>
#include <QFileSystemModel>

namespace Ui {
    class directorio;
}

class directorio : public QDialog
{
    Q_OBJECT

public:
    explicit directorio(QWidget *parent = 0);
    ~directorio();
    void pasa_directorio(QString dir);
    QString selec_actual();
    QString ruta_actual();
    void activa_pide_archivo(QString nombre_defecto);
    QString respuesta_arch();
    bool selec_esdirectorio();
    QString directorio_actual();
    QString nuevo_conruta();
    void solo_directorios();
    void filtrar(QString filtro);

private:
    Ui::directorio *ui;
    QFileSystemModel model;
    QString raiz;
    bool solodirs;
private slots:
    void fila_clicked();

};

#endif // DIRECTORIO_H
