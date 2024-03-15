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

    Debería haber rebloqueactabido una copia de la Licenbloqueactaa Pública General GNU
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#ifndef BLOQUEACTA_H
#define BLOQUEACTA_H

#include <QtGui>
#include "ui_bloqueacta.h"
#include <QSqlTableModel>


class bloqueacta : public QDialog {
    Q_OBJECT
	public:
                bloqueacta();
                QSqlQueryModel *modelobloqueacta;
                ~bloqueacta();
        private:
                Ui::bloqueacta ui;
                void recargartabla();
        private slots:
                void codigocambiado(QString codigo);
                void filaclicked();
                void botonborrar();
                void adicion();
                void codigofinedicion();
                void botonsubcuentapulsado();
                void editar_bloqueo();
                // añadir botón añadir

};





#endif 
