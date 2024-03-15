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

#include "ajuste_imagen.h"

ajuste_imagen::ajuste_imagen(QPixmap imagen) : QDialog() {
  ui.setupUi(this);

// -------------------------------------------------------------

original=imagen;
copiaescalada=imagen;
extracto=imagen;

escala=100;
factorescala=20*ui.tamhorizontalSlider->value()/100;
supx=0;
supy=0;
incremento=20*ui.movhorizontalSlider->value()/100;

ui.label->setPixmap(original);

connect(ui.disminuirpushButton,SIGNAL(clicked()),SLOT(disminuir()));
connect(ui.aumentarpushButton,SIGNAL(clicked()),SLOT(aumentar()));
connect(ui.inicialpushButton,SIGNAL(clicked()),SLOT(asignaoriginal()));
connect(ui.derechapushButton,SIGNAL(clicked()),SLOT(derecha()));
connect(ui.izquierdapushButton,SIGNAL(clicked()),SLOT(izquierda()));
connect(ui.arribapushButton,SIGNAL(clicked()),SLOT(arriba()));
connect(ui.abajopushButton,SIGNAL(clicked()),SLOT(abajo()));


connect(ui.tamhorizontalSlider,SIGNAL(sliderMoved ( int ) ),SLOT(fijafactorescala()));
connect(ui.movhorizontalSlider,SIGNAL(sliderMoved ( int ) ),SLOT(fijaincremento()));

}

void ajuste_imagen::disminuir()
{
    int alto=original.height();
    escala-=factorescala;
    QPixmap copia=original.scaledToHeight(alto*escala/100);
    ui.label->setPixmap(copia);
    copiaescalada=copia;
    extracto=copia;
supx=0;
supy=0;
}

void ajuste_imagen::aumentar()
{
    int alto=original.height();
    escala+=factorescala;
    QPixmap copia=original.scaledToHeight(alto*escala/100);
    ui.label->setPixmap(copia);
    copiaescalada=copia;
    extracto=copia;
supx=0;
supy=0;
}

void ajuste_imagen::asignaoriginal()
{
    ui.label->setPixmap(original);
    copiaescalada=original;
    escala=100;
    extracto=original;
supx=0;
supy=0;

}


void ajuste_imagen::derecha()
{
 if (supx+incremento>=copiaescalada.width()) return;
 supx+=incremento;
 QPixmap copia=copiaescalada.copy(supx, supy, copiaescalada.width()-supx, copiaescalada.height()-supy);
 ui.label->setPixmap(copia);
 // copiaescalada=copia;
}

void ajuste_imagen::izquierda()
{
 if (supx-incremento<0) return;
 supx-=incremento;
 QPixmap copia=copiaescalada.copy(supx, supy, copiaescalada.width()-supx, copiaescalada.height()-supy);
 ui.label->setPixmap(copia);
 // copiaescalada=copia;
}


void ajuste_imagen::abajo()
{
 if (supy+incremento>=copiaescalada.height()) return;
 supy+=incremento;
 QPixmap copia=copiaescalada.copy(supx, supy, copiaescalada.width()-supx, copiaescalada.height()-supy);
 ui.label->setPixmap(copia);
 // copiaescalada=copia;
}

void ajuste_imagen::arriba()
{
 if (supy-incremento<0) return;
 supy-=incremento;
 QPixmap copia=copiaescalada.copy(supx, supy, copiaescalada.width()-supx, copiaescalada.height()-supy);
 ui.label->setPixmap(copia);
 // copiaescalada=copia;
}

void ajuste_imagen::fijafactorescala()
{
 factorescala=20*ui.tamhorizontalSlider->value()/100;
}

void ajuste_imagen::fijaincremento()
{
 incremento=20*ui.movhorizontalSlider->value()/100;
}

QPixmap ajuste_imagen::seleccion()
{
    return copiaescalada.copy(supx,supy,copiaescalada.width()-supx > 90 ? 90:copiaescalada.width()-supx,
                              copiaescalada.height()-supy > 100 ? 100:copiaescalada.height()-supy);
}
