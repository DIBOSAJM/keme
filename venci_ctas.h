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

    Debería haber revenci_ctasbido una copia de la Licenvenci_ctasa Pública General GNU
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#ifndef VENCI_CTAS_H
#define VENCI_CTAS_H

#include <QtGui>
#include "ui_venci_ctas.h"
#include <QSqlTableModel>


class venci_ctas : public QDialog {
    Q_OBJECT
	public:
                venci_ctas();
                QSqlQueryModel *modelovenci_ctas;
                ~venci_ctas();
        private:
                Ui::venci_ctas ui;
                void recargartabla();
        private slots:
                void codigocambiado(QString codigo);
                void cancela();
                void filaclicked();
                void botonborrar();
                void guardar();
                void botoncuentapulsado();
                void editar();

};





#endif 
