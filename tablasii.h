#ifndef TABLASII_H
#define TABLASII_H

#include <QDialog>

namespace Ui {
class tablaSII;
}

class tablaSII : public QDialog
{
    Q_OBJECT

public:
    explicit tablaSII(QString qusuario,QWidget *parent = 0);
    ~tablaSII();
    void config_emitidas();

private:
    Ui::tablaSII *ui;
    bool comadecimal;
    bool decimales;
    bool recibidas;
    bool pruebas;
    bool modificacion;
    QString usuario;
    QString nombrefichero_respuesta;
    bool xml_gen_aislado;
    void carga_recibidas();
    void carga_emitidas();
    void actualiza_libro_iva();
    bool fich_sii_recibidas(QString nombrefich);
    bool fich_sii_emitidas(QString nombrefich);
    bool envio_SII(bool pruebas);

    bool fich_sii_anular_envio_emitidas(QString nombrefich);

    bool envio_SII_anular_envio(bool pruebas);

    bool consulta_SII(bool pruebas);
    bool fich_cons_sii_recibidas(QString nombrefich);
    bool fich_cons_sii_emitidas(QString nombrefich);




private slots:
    void actufechas();
    void refrescar();
    void marca_enviado();
    void desmarca_enviado();
    void aceptar();
    void cancelar();
    void cons_asiento();
    void edit_asiento();
    void borra_asiento();
    void marca_noenvio();
    void genera_fich_xml();
    void envio_real();
    void envio_real_anul();
    void envio_pruebas();
    void envio_real_modif();
    void envio_pruebas_modif();
    void copiar();

    void on_comprueba_nifs_pushButton_clicked();
    void on_doc_ini_lineEdit_textChanged(const QString &arg1);
    void on_consulta_pruebas_pushButton_clicked();
    void on_consulta_real_pushButton_clicked();
};

#endif // TABLASII_H
