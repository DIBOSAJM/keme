/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#ifndef EXECASIENTOMODELO_H
#define EXECASIENTOMODELO_H

#include <QtGui>
#include "ui_execasientomodelo.h"
#include "etiqueta_fich.h"

class execasientomodelo : public QDialog {
    Q_OBJECT
	public:
                execasientomodelo(QString usuario, bool borrador=true);
                void cargadatos( QString asientomodelo );
                QString fichdocumento();
                void aspiradora();
                void activar_msj_tabla();

        private:
                Ui::execasientomodelo ui;
                int numvariables;
                bool comadecimal;
                bool decimales;
                QStringList mensajes;
                QStringList valdefecto;
                bool guardar[50];
                bool okvariables();
                bool aib;
                bool autofactura_ue;
                bool autofactura_no_ue;
                bool eib;
                bool eib_servicios;
                bool isp_op_interiores;
                bool importacion;
                bool exportacion;
                bool con_aspiradora;
                bool chequeado;
                bool activa_msj_tabla;
                QString diario;
                void borracontenidos();
                QString asignacodigocuenta( QString expresion );
                QString asignaci( QString expresion );
                QString contenidovariable( QString variable );
                QString asignaconcepto( QString expresion );
                QString asignavalorexpresion( QString qexpresion, bool dobleprecis=false );
                QDate asignafechaasiento( QString expresion );
                QString usuario;
                void generaasiento(bool verificar);
                QString concepto_sii;
                CustomQLabel *etiqueta;
                bool diario_borrador;

        private slots:
                void valorcambiado(int fila,int columna);
                void botongenerar();
                void botonverificar();
                void posicioncambiada( int row, int col );
                void botonsubcuenta();
                void botonci();
                void buscacopiadoc();
                void visdoc();
                void borraasignaciondoc();
                void buscar_externo();
                void borra_externo();
                void ver_externo();
                void exec_aspiradora();
                void fichero_soltado();
};



#endif 
