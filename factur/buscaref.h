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

#ifndef BUSCAREF_H
#define BUSCAREF_H
#include <QSqlTableModel>

#include <QtGui>
#include "ui_buscaref.h"


class buscaref : public QDialog {
    Q_OBJECT
	public:
                buscaref(QString cadena);
                ~buscaref();
                QString seleccioncuenta( void );
                void cambiatitulo(QString titulo);
        private:
                Ui::buscaref ui;
                QSqlTableModel *modeloref;
        private slots :
                void ComienceRadioBotoncambiado();
                void ContengaRadioBotoncambiado();
                void recargartabla(QString cadena);
                void codigocambiado();
                void titulocuentacambiado();
                void fuerafococodigo();
                void tablaclickada();

};





#endif 
 
 
