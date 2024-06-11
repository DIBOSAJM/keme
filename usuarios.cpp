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

#include "usuarios.h"
#include "funciones.h"
#include "basedatos.h"
#include "pidenombre.h"
#include "usuario.h"
#include <QMessageBox>


usuarios::usuarios() : QDialog() {
    ui.setupUi(this);


    QSqlQueryModel *elmodelo = new QSqlQueryModel;
    model = elmodelo;

     model->setQuery( basedatos::instancia()->select_codigo_nombre_usuarios() );
     model->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     model->setHeaderData(1, Qt::Horizontal, tr("NOMBRE"));

     ui.usuariostableView->setModel(model);
     ui.usuariostableView->setAlternatingRowColors ( true);

     connect(ui.eliminarpushButton,SIGNAL(clicked()),SLOT(borrausuario()));
     connect(ui.editarpushButton,SIGNAL(clicked()),SLOT(editausuario()));
     connect(ui.nuevopushButton,SIGNAL(clicked()),SLOT(nuevousuario()));

}

usuarios::~usuarios()
{
  delete ui.usuariostableView;
  delete model;
}


void usuarios::refrescar()
{
    delete(model);

    QSqlQueryModel *elmodelo = new QSqlQueryModel;
    model = elmodelo;

     model->setQuery( basedatos::instancia()->select_codigo_nombre_usuarios() );
     model->setHeaderData(0, Qt::Horizontal, tr("CÓDIGO"));
     model->setHeaderData(1, Qt::Horizontal, tr("NOMBRE"));

     ui.usuariostableView->setModel(model);
     ui.usuariostableView->setAlternatingRowColors ( true);
}

void usuarios::nuevousuario()
{
  // pidenombre
  pidenombre *p = new pidenombre();
  p->asignaetiqueta(tr("Introduzca nuevo código:"));
  p->asignanombreventana(tr("Nuevo usuario"));
  p->exec();
  QString contenido=p->contenido();
  delete(p);
  if (contenido.contains(' '))
    {
       QMessageBox::warning( this, tr("GESTIÓN DE USUARIOS"),
      tr("ERROR: En la composición del código no pueden haber espacios"));
      return;
    }

  if (contenido.isEmpty())
    {
       QMessageBox::warning( this, tr("GESTIÓN DE USUARIOS"),
      tr("El código no puede estar vacío"));
      return;
    }


 if (basedatos::instancia()->existecodigousuario(contenido))
    {
       QMessageBox::warning( this, tr("GESTIÓN DE USUARIOS"),
      tr("ERROR: CÓDIGO EXISTENTE"));
      return;
    }

  basedatos::instancia()->adcodigousuario(contenido);

  refrescar();
}

void usuarios::borrausuario()
{
  QString eleccion=usuarioactual();
  if (eleccion.isEmpty()) return;
  if (eleccion=="admin")
   {
      QMessageBox::warning( this, tr("GESTIÓN DE USUARIOS"),
                            tr("ERROR: No es posible eliminar el usuario administrador"));
     return;
   }
  if (QMessageBox::question(
            this,
            tr("¿ Borrar usuario ?"),
            tr("¿ Desea borrar '%1' ?").arg(eleccion )) ==QMessageBox::Yes )
                          basedatos::instancia()->borrausuario(eleccion);

    refrescar();
}




QString usuarios::usuarioactual()
{

 if (!ui.usuariostableView->currentIndex().isValid()) return "";
 int fila=ui.usuariostableView->currentIndex().row();
 return model->data(model->index(fila,0),Qt::DisplayRole).toString();

}


void usuarios::editausuario()
{
    QString elcodigo;
    elcodigo=usuarioactual();
    if (elcodigo=="")
       {
         QMessageBox::warning( this, tr("usuarios"),
         tr("Para editar la fecha de cierre de un ejercicio debe de seleccionarlo de la lista"));
         return;
       }
  usuario *u = new usuario();
  u->cargar(elcodigo);
  u->exec();
  delete u;
  refrescar();
}

