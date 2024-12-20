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

#ifndef CONCILIA_H
#define CONCILIA_H

#include <QtGui>
#include "ui_concilia.h"



class concilia : public QDialog {
    Q_OBJECT
      public:
                concilia(bool qctaexterna,QString cuenta,QDate inicial, QDate final, QString qusuario);
      private:
                Ui::concilia ui;
                QString cuenta;
                bool comadecimal,decimales,cambios,ctaexterna;
                void cargadatos();
                void actucuenta();
                QStringList diferencias;
                QString fichconta;
                QString fichexterna;
                QString fichinforme;
                QString usuario;

                // variables de contabiliza partida
                QString aux1, aux2, concepto, valorcontra;
                bool coeficiente=false;
                bool verificar=false, concepto_cuaderno=false;

                bool cambiosdifconta();
                bool cambiosdifext();
                void grabadifsext();
                void cargadatosdifext();
                void cargadatosctaext();
                void grabactaext();
                void latexcontab();
                void latexexterna();
                void latexinforme();
                void marca(int fila);
                void marcaext(int fila);
                void actualiza_saldos_conciliado();
                void actualiza_tras_edicion();
                void activa_botones_edicion(bool activa);
                void check_botones_edicion();

      private slots:
                void marcaactual();
                void marca_y_saldoconci();
                void salir();
                void editafechaasien();
                void editaasiento();
                void vencimientos();
                void refrescar();
                void noconcistatus();
                void noconciextstatus();
                void nuevopaseexterna();
                void marcaactualext();
                void editapaseexterna();
                void botonaceptar();
                void concilia_automat();
                void informecontab();
                void informeexterna();
                void informeconciliacion();
                void imprimeconciliacion();
                void quitamarcasconci();
                void borrapaseexterna();
                void latex();
                void borra_asiento();
                void consulta_asiento();
                void nuevo_asiento();
                void esconde_linea_externa();
                void ejecuta_filtro_externa();
                void quitafiltro();
                void contabiliza_externa();
                void contabiliza_externa_linea();
                void consultamayor();
                void valor1cambiado();
                void edcondoc();
                void cambiacuentaapunte();
                void on_contabtableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
};



#endif
