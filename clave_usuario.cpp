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

#include "clave_usuario.h"
#include "basedatos.h"


clave_usuario::clave_usuario(QString qusuario) : QDialog() {
    ui.setupUi(this);


 // connect(ui.generarpushButton,SIGNAL(clicked()),SLOT(generar()));
 usuario=qusuario;

 QSqlQuery q=basedatos::instancia()->recuperarusuario(usuario);
 QString imagen;
    if (q.isActive())
      if (q.next())
        {
          imagen=q.value(12).toString();
          QPixmap foto;
          if (imagen.length()>0)
                {
                 QByteArray byteshexa;
                 byteshexa.append ( imagen.toUtf8() );
                 QByteArray bytes;
                 bytes=bytes.fromHex ( byteshexa );
                 foto.loadFromData ( bytes, "PNG");
                }
          ui.fotolabel->setPixmap(foto);
        }

  connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(aplicar()));

  connect(ui.clave1lineEdit,SIGNAL(textChanged(QString)),SLOT(clave_cambiada()));
  connect(ui.clave2lineEdit,SIGNAL(textChanged(QString)),SLOT(clave_cambiada()));
//  connect(ui.descripcionlineEdit,SIGNAL( textChanged(QString)),this,SLOT(descripcambiada()));

}


void clave_usuario::clave_cambiada()
{
    if (ui.clave1lineEdit->text() != ui.clave2lineEdit->text())
        ui.aceptarpushButton->setEnabled(false);
    else
        ui.aceptarpushButton->setEnabled(true);
}


void clave_usuario::aplicar()
{
  basedatos::instancia()->cambia_clave_usuario(usuario,ui.clave1lineEdit->text());
  accept();
}
