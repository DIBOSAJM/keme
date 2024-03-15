/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#ifndef CONSMAYOR_H
#define CONSMAYOR_H

#include <QtGui>
#include "ui_consmayor.h"

class consmayor : public QDialog {
    Q_OBJECT
	public:
                consmayor(bool concomadecimal, bool condecimales, QString qusuario);
                void pasadatos( QString qsubcuenta, QDate qfecha, QString externo="" );

        private:
                Ui::consmayor ui;
                bool comadecimal;
                bool decimales;
                QString usuario;
                void cargadatos();
                void ctaexpandepunto();
                void latexintervalo();
                void xmlintervalo();
                void informe_intervalo();
                QStringList fechal ,asientol, conceptol, debel, haberl, documentol, externol, saldol;
                QString cadsumadebeextracto, cadsumahaberextracto;

	private slots:
                void botonsubcuentapulsado();
                void refrescar();
                void finedicsubcuenta();
                void subcuentaprevia();
                void subcuentasiguiente();
                void imprimir();
                void imprimir_latex();
                void subcuentacambiada();
                void editarpulsado();
                void origenpercambiado();
                void finalpercambiado();
                void intervalogroupcambiado();
                void copiar();
                void genxml();
                void borraasiento();
                void visdoc();
                void latex();
                void consultaasiento();
                void datoscuenta();
                void vencimientos();
                void nuevo_asiento();
                void procesa_vencimiento();
                void acumuladoscta();
                void asigna_ficherodocumento();
                void quita_externo();
                void buscar_externo();
                void cod_externo_fin_edic();
                void cod_externo_cambiado();
                void informe();
                void concilia_ext() ;
                void importa_c43 ();
                void importa_csv ();
                void elimina_cuaderno();
                void setValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage);
                void setValue_i(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage);
                void setValueImage(const int recNo, const QString paramName, QImage &paramValue, const int reportPage);
                void editafechaasien();
                void edcondoc();
                void ed_registro_iva();
                void ed_registro_ret();
                void actu_conci_punteo();

};


#endif
