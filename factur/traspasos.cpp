/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#include "traspasos.h"
#include "basedatos.h"
#include "factura.h"

traspasos::traspasos(QStringList lis_serie, QStringList lis_numero) : QDialog() {
    ui.setupUi(this);

lserie=lis_serie;
lnumero=lis_numero;
// tipodoc=qtipodoc;
ui.docscomboBox->addItems(basedatos::instancia()->listacoddocs());

QString cadena;
for (int i = 0; i < lserie.size(); ++i)
   {
     if (!cadena.isEmpty()) cadena+=" ";
     cadena+=lserie.at(i)+lnumero.at(i);
   }
 ui.docsplainTextEdit->appendPlainText(cadena);

 connect(ui.aceptarpushButton, SIGNAL(clicked()), this,
           SLOT (procesar()));

}


void traspasos::procesar()
{
    // básicamente creamos una nueva "factura" con el tipo de documento seleccionado
    // y el cliente
    // pasamos todas las líneas de las facturas

    // cuenta, fecha, fecha operación, retención, recargo de equivalencia
    // mejor las cogemos del último documento
    factura *f = new factura();
    // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
    // "contabilizado,contabilizable, con_ret, re, tipo_ret, retencion, "
    // "tipo_doc, notas, pie1, pie2, pase_diario_cta "
    QSqlQuery q;
    int ultimo=lserie.size()-1;
    tipodoc=ui.docscomboBox->currentText();
    q = basedatos::instancia()->select_cabecera_doc (lserie.at(ultimo), lnumero.at(ultimo));
    if (q.isActive())
       if (q.next())
        {

         f->solo_cabecera_doc(q.value(0).toString(),
                    "",
                    q.value(2).toString(),
                    QDate().currentDate(), // q.value(4).toDate(),
                    QDate().currentDate(), // q.value(25).toDate(),
                    QDate().currentDate(), // q.value(5).toDate(),
                    false,
                    q.value(8).toBool(),
                    q.value(9).toBool(),
                    q.value(10).toString(),
                    q.value(11).toString(),
                    q.value(20).toString(),
                    tipodoc); // tipo_doc
       }
    // ahora nos concentramos en cargar las líneas del documento

    for (int i = 0; i < lserie.size(); ++i)
      {
        f->carga_lineas_doc(lserie.at(i),
                            lnumero.at(i));

        // falta cerrar si procede los documentos
        if (ui.cerrarcheckBox->isChecked())
            basedatos::instancia()->cierra_doc(lserie.at(i),
                                               lnumero.at(i));
      }

    f->addnotas(ui.docsplainTextEdit->toPlainText());
      if (!(f->exec()==QDialog::Accepted)) {
          // miramos de abrir de nuevo los documentos
          for (int i = 0; i < lserie.size(); ++i) {
              basedatos::instancia()->abre_doc(lserie.at(i),lnumero.at(i));
          }
      }
    delete(f);

    accept();

}
