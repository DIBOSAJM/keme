/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#ifndef MAIN_FACTUR_H
#define MAIN_FACTUR_H

#include <QMainWindow>
#include "ui_main_factur.h"
#include <QtSql>
#include <QSqlQueryModel>
#include "facturas.h"


class main_factur : public QMainWindow {
      Q_OBJECT
	  public:
                  main_factur();

          private:
                  Ui::main_factur ui;
                  bool guardaconexion;
                  facturas *p_facturas;
                  QString dirtrabajo;
                  QString aplicacionimprimir;
                  QString navegador;
                  bool sindecimales;
                  QString fichconfig;
                  QString fichconexiones;
                  QString dirconfig;
                  int estilonumerico;
                  QString usuario;
                  QString claveusuario;
                  bool cargapreferencias(bool *bdabierta, bool *hayfichpref);
                  bool conexion0();
                  bool abrebasedatos(QString vpghost,QString vpgdbname,QString vpguser,
                                    QString vpgpassword,QString vcontrolador,QString vpuerto);
              private slots:
                  void seriesfac();
                  void tiposdoc();
                  void referencias();
                  void cuentasiva();
                  void listafac();
                  void retencion();
                  bool cargausuario();
                  void conexionc();
                  void auxiliares();
                  void salir();
                  void fpredf();
                  void lotes();
                  void cuenta_anticipo();
                  void gest_externos();

                  void on_actionConfiguraci_n_Veri_Factu_triggered();

                  void on_actionContenido_Email_Facturas_triggered();

              public:
                  bool oknosalir();

                };


#endif
