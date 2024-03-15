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

#include "inicio.h"

inicio::inicio() : QDialog() {
    ui.setupUi(this);

   ui.textEdit->setText(tr(
        "Este programa es software libre; se  puede redistribuir y/o modificar "
        "bajo las condiciones de la Licencia GNU (GNU General Public License) "
        "en la forma en que la Free Software Foundation la ha publicado, ya "
        "sea bajo la versión 3 de la Licencia, o a su elección, bajo cualquier  "
        "otra versión más reciente.\n"
        "KEME-Contabilidad se suministra sin ningún tipo de garantía, ya sea implícita o "
        "explícita. El usuario asume por completo cualquier tipo de riesgo o pérdida "
        "derivada de la calidad y posible mal funcionamiento del programa. Si el "
        "programa fuera o se probara que es defectuoso, el usuario asumiría por "
        "completo el coste de cualquier servicio de mantenimiento, reparación o "
        "corrección.\n\n"
        "KEME-Contabilidad presupone, para su correcto funcionamiento, la existencia "
        "del siguiente software:\n\n"
        "En caso de utilizar alguno de los dos gestores de bases de datos POSTGRESQL ó MySQL, "
        "el que utilicemos debe de estar correctamente instalado y en "
        "funcionamiento (el manual de usuario contiene información al "
        "respecto)\n\n"
        "El controlador QPSQL de las librerías QT para la conexión con "
        "POSTGRESQL, o bien el controlador QMYSQL para MySQL. En caso de que utilicemos "
        "SQLITE, el controlador QSQLITE.\n\n"
        "El paquete Latex, que será el encargado, entre otras cosas, de generar la "
        "salida impresa de la aplicación.\n\n"));

}
