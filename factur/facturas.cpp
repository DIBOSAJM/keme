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

#include "facturas.h"
#include <QSqlQuery>
#include "busca_externo.h"
#include "buscasubcuenta.h"
#include "funciones.h"
#include "factura.h"
#include "basedatos.h"
#include "traspasos.h"
#include "editarasiento.h"
#include "notas.h"
#include "pide_fich.h"
#include <QMessageBox>
#include <QProgressDialog>
#include <qtrpt.h>

FacSqlModel::FacSqlModel(QObject *parent)
        : QSqlQueryModel(parent)
    {
     // primerasiento1=numeracionrelativa();
     comadecimal=haycomadecimal(); decimales=haydecimales();
    }


QVariant FacSqlModel::datagen(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        return value;
    }

QVariant FacSqlModel::data(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        if (value.isValid() && role == Qt::DisplayRole) {
            if (index.column() == 1)
               {
                // número de factura
               }
            if (index.column() == 3)
              {
               // Descripción
                QString externo=FacSqlModel::index(index.row(),9).data().toString();
                if (!externo.isEmpty()) return basedatos::instancia()->razon_externo(externo);

              }
            if (index.column() == 4)
               {
                // fecha
                return value.toDate().toString("dd-MM-yyyy");
               }
            if (index.column() == 5 || index.column() == 6)
               {
                    if (value.toBool())
                        return QString("*");
                     else return QString();
               }
            if (index.column() == 7)
               {
                // número de apunte en diario
                if (value.toInt()==0) return QString();
               }
            if (index.column() >= 10)
            {
                // número de apunte en diario
                if (value.toDouble()<0.0001 && value.toDouble()>-0.0001) return QString();
                else return formateanumero(value.toDouble(),comadecimal,decimales);
            }
        }
        if (role == Qt::TextAlignmentRole &&
             (index.column() == 1 || index.column() == 7 || index.column() == 10 || index.column() == 11 || index.column() == 12))
               return QVariant::fromValue(int(Qt::AlignVCenter | Qt::AlignRight));
        if (role == Qt::TextAlignmentRole && index.column()>=4 && index.column()<=6)
               return QVariant::fromValue(int(Qt::AlignCenter));
        return value;
    }



facturas::facturas(QString qusuario) : QDialog() {
    ui.setupUi(this);
comadecimal=haycomadecimal(); decimales=haydecimales();
usuario=qusuario;
ui.facstableView->setSelectionBehavior(QAbstractItemView::SelectRows);
ui.facstableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

ui.serieinicialcomboBox->addItems(basedatos::instancia()->listacodseries());
ui.seriefinalcomboBox->addItems(basedatos::instancia()->listacodseries());

ui.seriefinalcomboBox->setCurrentIndex(ui.seriefinalcomboBox->count()-1);

QDate finicial=basedatos::instancia()->selectMinaerturaejerciciosnocerrado();
QDate ffinal=basedatos::instancia()->selectMaxcierrecountcierreejercicios();

ui.inicialdateEdit->setDate(finicial);
ui.finaldateEdit->setDate(ffinal);

connect(ui.nuevapushButton, SIGNAL(clicked()), this,
          SLOT (nuevafactura()));

connect(ui.editarpushButton, SIGNAL(clicked()), this,
          SLOT (editafactura()));

connect(ui.imprimirpushButton, SIGNAL(clicked()), this,
          SLOT (latexdoc()));

connect(ui.transferirpushButton, SIGNAL(clicked()), this,
          SLOT (exectrasp()));

connect(ui.contabilizarpushButton, SIGNAL(clicked()), this,
          SLOT (contabilizar()));

connect(ui.conttodopushButton,SIGNAL(clicked()),this,SLOT(contabilizartodas()));

connect(ui.borrarpushButton, SIGNAL(clicked()), this, SLOT(borralinea()));

connect(ui.verasientopushButton, SIGNAL(clicked()), this, SLOT(consultaasiento()));

connect(ui.numiniciallineEdit, SIGNAL(textChanged(QString)), this, SLOT (numini_cambiado()));

connect(ui.cuentainilineEdit, SIGNAL(textChanged(QString)), this, SLOT (cuentaini_cambiada()));

connect(ui.cuentainilineEdit,SIGNAL(editingFinished()),this,SLOT(cuentaini_finedicion()));

connect(ui.cuentafinlineEdit,SIGNAL(editingFinished()),this,SLOT(cuentafin_finedicion()));

connect(ui.refrescarpushButton, SIGNAL(clicked()), this, SLOT(boton_refrescar()));

//connect(ui.xmlpushButton , SIGNAL(clicked()), this,
//          SLOT (xmldoc()));

connect(ui.informepushButton , SIGNAL(clicked()), this,
          SLOT (consultainforme()));

connect(ui.consultapushButton , SIGNAL(clicked()), this,
          SLOT (consulta()));

connect(ui.borra_asientopushButton, SIGNAL(clicked()), this,
          SLOT (borraasiento()));

connect(ui.edtexpushButton , SIGNAL(clicked()), this,
          SLOT (edita_tex()));

connect(ui.facturae_pushButton, SIGNAL(clicked(bool)),this, SLOT(genera_facturae()));

connect(ui.imp_documento_pushButton,SIGNAL(clicked(bool)),this,SLOT(informe_documento()));

}

void facturas::nuevafactura()
{
    factura *f = new factura();
    f->exec();
    QString qserie, qnumero;
    if (f->imprime_informe(&qserie,&qnumero)) {
        informe (qserie,  qnumero);
    }

    delete(f);
    refresca();
}


void facturas::activaconfiltro()
{
        FacSqlModel *elmodelo = new FacSqlModel;
        actufiltro();
        model = elmodelo;

        model->setQuery( basedatos::instancia()->select_cab_facturas (guardafiltro) );
        // serie,numero,cuenta, fecha_fac,
        // contabilizado,contabilizable, con_ret,
        // pase_diario_cta
        model->setHeaderData(0, Qt::Horizontal, tr("Serie"));
        model->setHeaderData(1, Qt::Horizontal, tr("Número"));
        model->setHeaderData(2, Qt::Horizontal, tr("Cuenta"));
        model->setHeaderData(3, Qt::Horizontal, tr("Descripción"));

        model->setHeaderData(4, Qt::Horizontal, tr("Fecha Doc."));
        model->setHeaderData(5, Qt::Horizontal, tr("Contab."));
        model->setHeaderData(6, Qt::Horizontal, tr("Cerrado"));
        model->setHeaderData(7, Qt::Horizontal, tr("Apunte"));
        model->setHeaderData(8, Qt::Horizontal, tr("TIPO DOC."));
        model->setHeaderData(9, Qt::Horizontal, tr("Externo"));
        model->setHeaderData(10, Qt::Horizontal, tr("Total Doc."));
        model->setHeaderData(11, Qt::Horizontal, tr("Retención"));
        model->setHeaderData(12, Qt::Horizontal, tr("Suplidos"));

        ui.facstableView->setModel(model);
        ui.facstableView->setAlternatingRowColors ( true);

        ui.facstableView->setColumnWidth(0,60);
        ui.facstableView->setColumnWidth(1,80);
        ui.facstableView->setColumnWidth(3,150);
        ui.facstableView->setColumnWidth(5,60);
        ui.facstableView->setColumnWidth(6,60);
        ui.facstableView->setColumnWidth(7,80);
        ui.facstableView->setColumnWidth(10,80);
        ui.facstableView->setColumnWidth(11,80);
        ui.facstableView->setColumnWidth(12,80);

}


void facturas::pasafiltro(QString filtro)
{
 model->setQuery( basedatos::instancia()->select_cab_facturas (filtro) );

}

void facturas::refresca()
{
 int anchos[9];
 for (int veces=0; veces<9; veces++)
     anchos[veces]=ui.facstableView->columnWidth(veces);
 actufiltro();
 model->clear();
 model->setQuery(basedatos::instancia()->select_cab_facturas (guardafiltro));
 model->setHeaderData(0, Qt::Horizontal, tr("Serie"));
 model->setHeaderData(1, Qt::Horizontal, tr("Número"));
 model->setHeaderData(2, Qt::Horizontal, tr("Cuenta"));
 model->setHeaderData(3, Qt::Horizontal, tr("Descripción"));
 model->setHeaderData(4, Qt::Horizontal, tr("Fecha Doc."));
 model->setHeaderData(5, Qt::Horizontal, tr("Contab."));
 model->setHeaderData(6, Qt::Horizontal, tr("Cerrado"));
 model->setHeaderData(7, Qt::Horizontal, tr("Apunte"));
 model->setHeaderData(8, Qt::Horizontal, tr("TIPO DOC."));
 model->setHeaderData(9, Qt::Horizontal, tr("Externo"));
 model->setHeaderData(10, Qt::Horizontal, tr("Total Doc."));
 model->setHeaderData(11, Qt::Horizontal, tr("Retención"));
 model->setHeaderData(12, Qt::Horizontal, tr("Suplidos"));

 for (int veces=0; veces<9; veces++)
     ui.facstableView->setColumnWidth(veces,anchos[veces]);

}


void facturas::boton_refrescar()
{
 refresca();
}

void facturas::editafactura()
{
    if (basedatos::instancia()->doc_cerrado(serie(),
                            numero()))
      {
          QMessageBox::warning( this, tr("Contabilizar"),tr("ERROR: "
                               "El documento seleccionado está cerrado"));
          return;

      }

    if (!ui.facstableView->currentIndex().isValid())
    {
     QMessageBox::warning( this, tr("Edición de documentos"),
                           tr("Para editar un documento hay que seleccionarlo de la lista"));
     return;
    }
    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie(), numero());
    if (q.isActive())
       if (q.next())
        {
         factura *f = new factura();
         // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
         // "contabilizado,contabilizable, con_ret, re, tipo_ret, retencion, "
         // "tipo_doc, notas, pie1, pie2, pase_diario_cta "
         f->pasa_cabecera_doc(q.value(0).toString(),
                         q.value(1).toString(),
                         q.value(2).toString(),
                         q.value(4).toDate(),
                              q.value(25).toDate(),
                         q.value(5).toDate(),
                         q.value(7).toBool(),
                         q.value(8).toBool(),
                         q.value(9).toBool(),
                         q.value(10).toString(),
                         q.value(11).toString(),
                         q.value(12).toString(),
                         q.value(13).toString(),
                         q.value(14).toString(),
                         q.value(15).toString(),
                         q.value(19).toString(),
                         q.value(20).toString(),
                         q.value(21).toString(),
                              q.value(22).toString(),
                              q.value(23).toString(),
                              q.value(24).toString());
         f->exec();
         QString qserie, qnumero;
         if (f->imprime_informe(&qserie,&qnumero)) {
             informe (qserie,  qnumero);
         }
         delete(f);
         refresca();
       }
}


QString facturas::serie()
{
 if (!ui.facstableView->currentIndex().isValid()) return "";
 int fila=ui.facstableView->currentIndex().row();
 return model->data(model->index(fila,0),Qt::DisplayRole).toString();
}


QString facturas::numero()
{
 if (!ui.facstableView->currentIndex().isValid()) return "";
 int fila=ui.facstableView->currentIndex().row();
 return model->data(model->index(fila,1),Qt::DisplayRole).toString();
}

void facturas::genera_facturae()
{
    if (!ui.facstableView->currentIndex().isValid())
      {
       QMessageBox::warning( this, tr("Generar FACTURAe"),
                           tr("Para generar un documento hay que seleccionarlo de la lista"));
       return;
      }

    ffacturae(serie(),numero());

    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie(), numero());
    QString tipo_doc;
    if (q.isActive())
    {
       if (q.next())
        {
         // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
         // "contabilizado,contabilizable, con_ret, con_re, tipo_ret, retencion, "
         // "tipo_doc, notas, pie1, pie2, pase_diario_cta, clave "
        tipo_doc=q.value(12).toString();
        } else return;
    }


    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString cadfich=serie()+numero()+".FACe.xml";
    QString pasa=qfichero+cadfich;



    QDomDocument doc("FACTURAe");
    QFile fichero(pasa);
    if (!fichero.exists())
       {
        QMessageBox::warning( this, tr("FICHERO SII"),
                             tr("ERROR: No se ha generado fichero FACTURAe"));
        return;

       }

    if ( !fichero.open( QIODevice::ReadOnly ) ) return;

    if (!doc.setContent(&fichero)) {
       fichero.close();
       return;
      }
    fichero.close();
    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->esconde_tex_imprimir();
    n->pasa_contenido(doc.toString());
    n->exec();
    delete(n);

}

void facturas::xmldoc()
{
    if (!ui.facstableView->currentIndex().isValid())
      {
       QMessageBox::warning( this, tr("Generar XML"),
                           tr("Para generar un documento hay que seleccionarlo de la lista"));
       return;
      }

    fxmldoc(serie(),numero());


    // QMessageBox::information( this, tr("Generar XML"),
    //                    tr("El fichero XML se ha generado"));

    QString cadfichjasper=trayreport();
    cadfichjasper.append(QDir::separator());

    // ------------------------------------
    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie(), numero());
    QString tipo_doc;
    bool conret=false;
    if (q.isActive())
    {
       if (q.next())
        {
         // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
         // "contabilizado,contabilizable, con_ret, con_re, tipo_ret, retencion, "
         // "tipo_doc, notas, pie1, pie2, pase_diario_cta, clave "
        tipo_doc=q.value(12).toString();
        conret=q.value(8).toBool();
        } else return;
    }
    QString fichjasp=basedatos::instancia()->fichreport(tipo_doc);
    if (!fichjasp.isEmpty()) cadfichjasper=fichjasp;
      else
         {
          if (conret) cadfichjasper+="factura_keme_retencion.jasper";
             else cadfichjasper+="factura_keme.jasper";

         }
    pide_fich *p = new pide_fich();
    p->pasa(tr("Selección plantilla"),tr("Plantilla Jasper:"));
    p->pasadir(trayreport());
    p->pasa_nombre_fichero(cadfichjasper);
    int resultado=p->exec();
    if (resultado==QDialog::Accepted)
        cadfichjasper=p->nombre_fichero();
      else  {delete(p); return;}
    delete(p);

    //QMessageBox::information( this, tr("Generar XML"),
    //                 fichjasp);

    // -------------------------------------
     QProgressDialog progreso("Generando documento ...", 0, 0, 0);
     progreso.setWindowTitle(tr("PROCESANDO..."));
     //progreso.setWindowModality(Qt::WindowModal);
     progreso.setMaximum(0);
     // progreso.setValue(30);
     progreso.setMinimumDuration ( 0 );
     // progreso.forceShow();
     progreso.show();
     QApplication::processEvents();
     progreso.show();

    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString rutagraf=qfichero+tipo_doc+".png";

    // generamos imagen del logo
    QString imagen=basedatos::instancia()->imagendoc(tipo_doc);
    if (imagen.isEmpty()) imagen=logo_facturacion_defecto();
    QPixmap foto;
    if (imagen.length()>0)
          {
           QByteArray byteshexa;
           byteshexa.append ( imagen.toUtf8() );
           QByteArray bytes;
           bytes=bytes.fromHex ( byteshexa );
           foto.loadFromData ( bytes, "PNG");
           foto.save(rutagraf,"PNG");
          }

    // -------------------------------------------
    QString cadfich=serie()+numero()+".xml";
    QString pasa=qfichero+cadfich;

    QString cadpdf=qfichero+serie()+numero()+".pdf";

    QApplication::processEvents();

    informe_jasper_xml(cadfichjasper, pasa,
                       "/Documento/Detalle/Linea", cadpdf,
                       rutagraf);

    // QMessageBox::information( this, tr("XML DIARIO"),tr("El archivo XML se ha generado"));
    QApplication::processEvents();

}

void facturas::informe (QString qserie, QString qnumero, bool pdf_novis) {
    QString nombre_empresa=basedatos::instancia()->selectEmpresaconfiguracion();
    QString domicilio=basedatos::instancia()->domicilio();
    QString poblacion=basedatos::instancia()->ciudad();
    QString cp=basedatos::instancia()->cpostal();
    QString provincia=basedatos::instancia()->provincia();
    QString cif=basedatos::instancia()->cif();
    QString id_registral=basedatos::instancia()->idregistral();
    QString pais_emisor;

    QStringList lista_suplidos=basedatos::instancia()->lista_ref_suplidos();
    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (qserie, qnumero);
    QString tipo_doc, cuenta, externo, tipo_ret, retencion, notas, pie1, pie2;
    bool con_ret=false, con_re=false;
    QDate fecha, fecha_fac, fecha_op;
    int clave=0;
    if (q.isActive())
    {
       if (q.next())
        {
         // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
         // "contabilizado,contabilizable, con_ret, con_re, tipo_ret, retencion, "
         // "tipo_doc, notas, pie1, pie2, pase_diario_cta, clave "
        cuenta=q.value(2).toString();
        fecha=q.value(3).toDate();
        fecha_fac=q.value(4).toDate();
        fecha_op=q.value(5).toDate();
        con_ret=q.value(8).toBool();
        con_re=q.value(9).toBool();
        tipo_ret=q.value(10).toString();
        retencion=q.value(11).toString();
        tipo_doc=q.value(12).toString();
        notas=q.value(13).toString();
        pie1=q.value(14).toString();
        pie2=q.value(15).toString();
        clave=q.value(17).toInt();
        externo=q.value(20).toString();
        } else return;
    }

    basedatos::instancia()->datos_empresa_tipo_doc(tipo_doc,
                                &nombre_empresa,
                                &domicilio,
                                &cp,
                                &poblacion,
                                &provincia,
                                &pais_emisor,
                                &cif,
                                &id_registral
                                    );


    // int tipo_operacion=basedatos::instancia()->tipo_operacion_tipo_doc(tipo_doc);

    QString documento, cantidad, referencia, descripref, precio, totallin, totallineas, msbi;
    QString mstipoiva, mstipore, mscuota, mscuotare, totalfac, msnotas, venci;
    QString mscif_empresa, mscif_cliente, msnumero, msfecha, mscliente, msdescuento, msuplidos, mretencion ;
    basedatos::instancia()->msjs_tipo_doc(tipo_doc,
                                   &documento,
                                   &cantidad,
                                   &referencia,
                                   &descripref,
                                   &precio,
                                   &totallin,
                                   &msbi,
                                   &mstipoiva,
                                   &mstipore,
                                   &mscuota,
                                   &mscuotare,
                                   &totallineas,
                                   &totalfac,
                                   &msnotas,
                                   &venci,
                                   &mscif_empresa,
                                   &mscif_cliente,
                                   &msnumero,
                                   &msfecha,
                                   &mscliente,
                                   &msdescuento,
                                   &msuplidos, &mretencion);

    // QString descripdoc=basedatos::instancia()->descrip_doc(tipo_doc);
    QString moneda;
    if (basedatos::instancia()->es_euro_tipo_doc(tipo_doc))
      {
        moneda=tr("€");
      }


    // obtenemos información de la cuenta cliente, para los datos de la factura
    if (externo.isEmpty())
      q=basedatos::instancia()->selectTodoDatossubcuentacuenta (cuenta);
     else
        q=basedatos::instancia()->selectTodoDatosexterno (externo);

    QString razon,nombrecomercial,cif2,domicilio2,poblacion2,
      codigopostal,provincia2,pais,codvenci;
    if (q.isActive())
     {
       if (q.next())
        {
         razon=q.value(1).toString();
         nombrecomercial=q.value(2).toString();
         cif2=q.value(3).toString();
         domicilio2=q.value(5).toString();
         poblacion2=q.value(6).toString();
         codigopostal=q.value(7).toString();
         provincia2=q.value(8).toString();
         pais=q.value(26).toString();
         codvenci=q.value(10).toString();

        }
     }

    q = basedatos::instancia()->select_lin_doc(clave);
    double sumalineas=0;
    double suplidos=0;
    if (q.isActive())
       while (q.next())
        {
         double suma=redond(q.value(3).toDouble()*q.value(4).toDouble() *
                    (1-q.value(8).toDouble()/100),4);
         if (lista_suplidos.contains(q.value(1).toString()))
             suplidos+=suma;

         sumalineas+=suma;
        }
    double totalfactura=redond(sumalineas,2);


    q = basedatos::instancia()->select_iva_lin_doc (clave);
    QStringList listabase_iva, listatipo_iva, listatipo_re, listacuota_iva, listacuota_re;
    if (q.isActive())
        while (q.next())
         {
         // "sum(cantidad*precio*(1-dto/100)), "
         // "clave_iva, max(tipo_iva), max(tipo_re) "
         listabase_iva << formateanumerosep(redond(q.value(0).toDouble(),2),comadecimal,decimales);
         listatipo_iva << formateanumerosep(redond(q.value(2).toDouble(),2),comadecimal,decimales);
         if (con_re)
           {
            listatipo_re << formateanumerosep(redond(q.value(3).toDouble(),2),comadecimal,decimales);
           }
         listacuota_iva << formateanumerosep(redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2)
                                     ,comadecimal,decimales);
         totalfactura+=redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2);
         if (con_re)
           {
            listacuota_re << formateanumerosep(redond(q.value(0).toDouble()*q.value(3).toDouble()/100,2),
                                          comadecimal,decimales);
            totalfactura+=redond(q.value(0).toDouble()*q.value(3).toDouble()/100,2);
           }
       }

    double vtotal_factura=totalfactura-suplidos; // total factura sin tener en cuenta suplidos ni retenciones
    // ahora vamos a por la retención
    QString cad_vtotal_factura; cad_vtotal_factura.setNum(vtotal_factura,'f',2);

    if (con_ret)
      {
        totalfactura -= redond(retencion.toDouble(),2);
      }

    bool verifactu=basedatos::instancia()->es_verifactu_tipo_doc(tipo_doc);
    QString url_qr_verifactu;
    if (verifactu) url_qr_verifactu=basedatos::instancia()->url_val_QR();
    url_qr_verifactu.append("&nif=");
    url_qr_verifactu.append(cif);
    url_qr_verifactu.append("&numserie=");
    url_qr_verifactu.append(qserie+qnumero);
    url_qr_verifactu.append("&fecha=");
    url_qr_verifactu.append(fecha_fac.toString("dd-MM-yyyy"));
    url_qr_verifactu.append("&importe=");
    url_qr_verifactu.append(cad_vtotal_factura.trimmed());

    QStringList codigos,descripciones,descripcionsn,notasln,cantidades,precios,descuentos,totaleslin;
    q = basedatos::instancia()->select_lin_doc(clave);
    if (q.isActive())
       while (q.next())
        {
        bool precision=basedatos::instancia()->ref_precision(q.value(1).toString());
        codigos << q.value(1).toString();
        if (q.value(9).toString().isEmpty())
           descripciones << q.value(2).toString();
        else {
            descripciones << q.value(2).toString() + "\n" + q.value(9).toString();
        }
        descripcionsn << q.value(2).toString();
        notasln << q.value(9).toString();
        QString qcantidad=q.value(3).toString();
        if (qcantidad.contains(".000")) qcantidad.remove(".000");
        cantidades << qcantidad;
        if (precision)
           precios << formateanumero_ndec(redond(q.value(4).toDouble(),4),comadecimal,4); // precio
          else
            precios << formateanumerosep(redond(q.value(4).toDouble(),2),comadecimal,decimales); // precio
        descuentos << (q.value(8).toDouble() !=0 ? q.value(8).toString(): ""); // descuento
        if (precision)
            totaleslin << formateanumero_ndec(redond(q.value(3).toDouble()*q.value(4).toDouble()*
                                    (1-q.value(8).toDouble()/100),4),comadecimal,
                                    4);
           else
            totaleslin << formateanumerosep(redond(q.value(3).toDouble()*q.value(4).toDouble()*
                                    (1-q.value(8).toDouble()/100),2),comadecimal,
                                    decimales);
        }

    QByteArray img_array = basedatos::instancia()->img_tipos_doc(tipo_doc);
    QPixmap lp; lp.loadFromData(img_array);

    QString fileName;
    fileName =  ":/informes/factura.xml" ;
    if (verifactu) fileName =  ":/informes/factura_verifactu.xml" ;
    if (suplidos>0.001 || suplidos<-0.001) fileName = ":/informes/factura_suplidos.xml" ;

    if (con_ret) fileName =  ":/informes/factura_ret.xml" ;
    if ((suplidos>0.001 || suplidos<-0.001) && con_ret) fileName =  ":/informes/factura_ret_suplidos.xml" ;

    QString fichdoc=basedatos::instancia()->fichreport(tipo_doc);
    if (!fichdoc.isEmpty()) fileName=fichdoc; // dirtrabajobd()+QDir::separator()+"zz_Informes"+QDir::separator()+fichdoc;

    if (!pdf_novis) {
       pide_fich *p = new pide_fich();
       p->pasa(tr("Selección plantilla"),tr("Plantilla Informe:"));
       p->pasadir(dirtrabajobd()+QDir::separator()+"zz_Informes");
       p->pasa_nombre_fichero(fileName);
       int resultado=p->exec();
       if (resultado==QDialog::Accepted)
         fileName=p->nombre_fichero();
        else  {delete(p); return;}
       delete(p);
    }



    QtRPT *report = new QtRPT(this);

    // report->recordCount << codigos.count();

    if (report->loadReport(fileName) == false)
    {
        QMessageBox::information( this, tr("INFORME FACTURA"),
                                  tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
        return;
    }

    //report->setBackgroundImage(QPixmap(lp));
    //report->setBackgroundImageOpacity(1.0);

    QObject::connect(report, &QtRPT::setDSInfo,
                     [=](DataSetInfo &dsInfo) {
        dsInfo.recordCount=codigos.count();
    });

    QString imagen=basedatos::instancia()->imagendoc(tipo_doc);

    connect(report, &QtRPT::setValueImage,[&](const int recNo, const QString paramName,
            QImage &paramValue, const int reportPage) {
            Q_UNUSED(recNo);
            Q_UNUSED(reportPage);
            if (paramName == "LOGO") {

                //auto foto= new QImage();
                QImage foto;
                if (imagen.isEmpty()) imagen = logodefecto();
                if (imagen.length()>0)
                   {
                    QByteArray byteshexa;
                    byteshexa.append ( imagen.toUtf8() );
                    QByteArray bytes;
                    bytes=bytes.fromHex ( byteshexa );
                    foto.loadFromData( bytes, "PNG");
                   } else return;

                paramValue = foto;
            }
        });


    connect(report, &QtRPT::setValue, [=](const int recNo, const QString paramName, QVariant &paramValue,
            const int reportPage) {
        // campos: descrip, nota, cifra1, cifra2
        // recordCount es una lista, añadir también segunda página

        // QMessageBox::warning( this, tr("Estados Contables"),tr("Num %1").arg(recNo));
        Q_UNUSED(reportPage);

        /*cabecera_factura+=filtracad(msfecha)+" & \\scriptsize " +
                          fecha_fac.toString("dd-MM-yyyy") + " & \\scriptsize " ;
        cabecera_factura+=filtracad(msnumero)+" & \\scriptsize "+ serie+numero + "\\\\\n";*/

        if (paramName == "CFECHA") paramValue = msfecha;
        if (paramName == "FECHA") paramValue = fecha_fac.toString("dd-MM-yyyy");
        if (paramName == "CCOD") paramValue = msnumero;
        if (paramName == "COD") paramValue = qserie+qnumero;

        if (paramName == "DOCUMENTO") paramValue = documento;

        if (paramName == "URL_QR_VERIFACTU") paramValue = url_qr_verifactu;

        if (paramName == "EM_NOMBRE") paramValue = nombre_empresa;

        if (paramName == "EM_DIREC") paramValue = domicilio;
        if (paramName == "EM_CP" ) paramValue = cp;

        if (paramName == "EM_POBLACION") paramValue = poblacion;

        if (paramName == "EM_PROVINCIA") paramValue = provincia;

        if (paramName == "EM_PAIS") paramValue = pais_emisor;

        if (paramName == "NOMBRE") paramValue = razon;
        if (paramName == "NOMBRE_COMERCIAL") paramValue = nombrecomercial;
        if (paramName == "DIREC") paramValue = domicilio2;
        if (paramName == "CP") paramValue = codigopostal;
        if (paramName == "POBLACION") paramValue = poblacion2;
        if (paramName == "PROVINCIA") paramValue = provincia2;
        if (paramName == "PAIS") paramValue = pais;
        if (paramName == "CNIF") paramValue = mscif_cliente;
        if (paramName == "NIF") paramValue = cif2;

        QString detalle_cliente;
        detalle_cliente+=" "+razon+"\n";
        detalle_cliente+=" "+domicilio2+"\n";
        detalle_cliente+=" "+codigopostal+" - "+poblacion2+"\n";
        detalle_cliente+=" "+provincia2+"\n";
        detalle_cliente+=" "+pais+"\n";
        detalle_cliente+=" "+mscif_cliente+" "+cif2;
        if (paramName == "DETALLE_CLIENTE") paramValue = detalle_cliente;


        if (paramName == "CREFERENCIA") paramValue = referencia;
        if (paramName == "CDESCRIPCION") paramValue = descripref;
        if (paramName == "CCANTIDAD") paramValue = cantidad;
        if (paramName == "CPRECIO") paramValue = precio;
        if (paramName == "CTOTALLIN") paramValue = totallin;
        if (paramName == "CDESCUENTO") paramValue = msdescuento;

        if (paramName == "REFERENCIA") paramValue = codigos.at(recNo);
        if (paramName == "DESCRIPCION") paramValue = descripciones.at(recNo);
        if (paramName == "DESCRIPCIONSN") paramValue = descripcionsn.at(recNo);
        if (paramName == "NOTASLN") paramValue = notasln.at(recNo);
        if (paramName == "CANTIDAD") paramValue = cantidades.at(recNo);
        if (paramName == "PRECIO") paramValue = precios.at(recNo);
        if (paramName == "DESCUENTO") paramValue = descuentos.at(recNo);
        if (paramName == "TOTALLIN") paramValue = totaleslin.at(recNo);

        if (paramName == "NOTAS") paramValue = notas;
        if (paramName == "CBI") paramValue = msbi;
        if (paramName == "CTIPO") paramValue = mstipoiva;
        if (paramName == "CCUOTA") paramValue = mscuota;

        if (listabase_iva.count()>0) {
            if (paramName == "BASE1") paramValue = listabase_iva.at(0);
            if (paramName == "TIPO1") paramValue = listatipo_iva.at(0);
            if (paramName == "CUOTA1") paramValue = listacuota_iva.at(0);
        }

        if (listabase_iva.count()>1) {
            if (paramName == "BASE2") paramValue = listabase_iva.at(1);
            if (paramName == "TIPO2") paramValue = listatipo_iva.at(1);
            if (paramName == "CUOTA2") paramValue = listacuota_iva.at(1);
        }

        if (listabase_iva.count()>2) {
            if (paramName == "BASE3") paramValue = listabase_iva.at(2);
            if (paramName == "TIPO3") paramValue = listatipo_iva.at(2);
            if (paramName == "CUOTA3") paramValue = listacuota_iva.at(2);
        }

        if (paramName == "CRETENCION") paramValue = mretencion;
        if (paramName == "TIPORET") paramValue = tipo_ret;
        if (paramName == "RETENCION") paramValue = retencion;

        if (paramName == "CTOTAL") paramValue = totalfac;
        if (paramName == "TOTAL") paramValue = formateanumerosep(totalfactura,comadecimal,decimales);

        if (suplidos>0.001 || suplidos<-0.001)
           {
            //pie_factura += "\\footnotesize "+msuplidos+ " & \\footnotesize " +formateanumerosep(suplidos,coma,decimales);
            //pie_factura += moneda + "\\\\\\hline\n";
            if (paramName == "CSUPLIDOS") paramValue = msuplidos;
            if (paramName == "SUPLIDOS") paramValue = formateanumerosep(suplidos,comadecimal,decimales);

           }


        if (paramName == "PIE1") paramValue = pie1;
        if (paramName == "PIE2") paramValue = pie2;
        if (paramName == "INFO") paramValue = nombre_empresa + " - " + cif + " - " + id_registral;

    });

    if (!pdf_novis) report->printExec(true);

    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString cadfich=qserie+qnumero+".pdf";
    qfichero.append(cadfich);
    report->printPDF(qfichero,false);

}

void facturas::gen_email(QString qserie, QString qnumero)
{
    QString email_from, email_to, email_subject, email_content;
    basedatos::instancia()->campos_email_cli(&email_from, &email_to, &email_subject, &email_content);

    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (qserie, qnumero);
    QString cuenta, externo, tipo_doc;
    QDate fecha_fac;
    if (q.isActive())
    {
        if (q.next())
        {
            // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
            // "contabilizado,contabilizable, con_ret, con_re, tipo_ret, retencion, "
            // "tipo_doc, notas, pie1, pie2, pase_diario_cta, clave "
            cuenta=q.value(2).toString();
            fecha_fac=q.value(4).toDate();
            externo=q.value(20).toString();
            tipo_doc=q.value(12).toString();
        } else return;
    }

    if (!externo.isEmpty()) {
        QString email_externo=basedatos::instancia()->email_externo(externo);
        if (!email_externo.isEmpty()) email_to=email_externo;
    } else {
        // consultamos email de cuenta
        QString email_externo=basedatos::instancia()->email_cuenta(cuenta);
        if (!email_externo.isEmpty()) email_to=email_externo;
    }

    /*
From: remitente@dominio.com
To: destinatario@dominio.com
Subject: Factura emitida
MIME-Version: 1.0
Content-Type: multipart/mixed; boundary="limite123"

--limite123
Content-Type: text/plain; charset="UTF-8"

Adjunto encontrará su factura en PDF.

--limite123
Content-Type: application/pdf; name="factura.pdf"
Content-Transfer-Encoding: base64
Content-Disposition: attachment; filename="factura.pdf"

JVBERi0xLjQKJcfs... (contenido PDF en base64)
--limite123--
     */
    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString cadfich=qserie+qnumero+".pdf";
    qfichero.append(cadfich);

    QString cad_email;
    cad_email="FROM: ";
    cad_email.append(email_from);
    cad_email.append("\n");
    cad_email.append("To: ");
    cad_email.append(email_to);
    cad_email.append("\n");
    cad_email.append("Subject: ");
    cad_email.append(email_subject.replace("{empresa}",basedatos::instancia()->selectEmpresaconfiguracion())
                         .replace("{fecha_fac}",fecha_fac.toString("dd-MM-yyyy")).replace("{factura}",qserie+qnumero));


    cad_email.append("\n");
    cad_email.append("MIME-Version: 1.0\n");
    cad_email.append("Content-Type: multipart/mixed; boundary=""limite123""\n");
    cad_email.append("\n");
    cad_email.append("--limite123\n");
    cad_email.append("Content-Type: text/plain; charset=""UTF-8""\n");
    cad_email.append("\n");
    cad_email.append(email_content.replace("{empresa}",basedatos::instancia()->selectEmpresaconfiguracion())
                         .replace("{fecha_fac}",fecha_fac.toString("dd-MM-yyyy")).replace("{factura}",qserie+qnumero));
    cad_email.append("\n\n");
    cad_email.append("--limite123\n");
    cad_email.append("Content-Type: application/pdf; name=""");
    cad_email.append(cadfich);
    cad_email.append("""\n");
    cad_email.append("Content-Transfer-Encoding: base64\n");
    cad_email.append("Content-Disposition: attachment; filename=""");
    cad_email.append(cadfich);
    cad_email.append("""\n");
    cad_email.append("\n");
    QFile pdfFile(qfichero);
    if (pdfFile.open(QIODevice::ReadOnly)) {
        QByteArray pdfData = pdfFile.readAll();
        QByteArray base64Data = pdfData.toBase64();
        pdfFile.close();
        cad_email.append(QString::fromLatin1(base64Data));
    }
    cad_email.append("\n--limite123\n");

    qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    cadfich=qserie+qnumero+".eml";
    qfichero.append(cadfich);

    QFile fichero(qfichero);
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cad_email;
    fichero.close();

    QString url=qfichero;
    QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

}

void facturas::carga_combos_series()
{
    ui.serieinicialcomboBox->addItems(basedatos::instancia()->listacodseries());
    ui.seriefinalcomboBox->addItems(basedatos::instancia()->listacodseries());

    ui.seriefinalcomboBox->setCurrentIndex(ui.seriefinalcomboBox->count()-1);

}
void facturas::informe_documento() {
    if (!ui.facstableView->currentIndex().isValid())
      {
       QMessageBox::warning( this, tr("Impresión de documentos"),
                           tr("Para generar un documento hay que seleccionarlo de la lista"));
       return;
      }

    QItemSelectionModel *seleccion;
    seleccion=ui.facstableView->selectionModel();
    if (seleccion->hasSelection())
        if (seleccion->selectedRows().count()>1) {
            QList<QModelIndex> lista_serie=seleccion->selectedRows(0);
            QList<QModelIndex> lista_numero=seleccion->selectedRows(1);
            for (int i=0; i<lista_serie.count(); i++) {
                QString qserie=model->datagen(lista_serie.at(i),Qt::DisplayRole).toString();
                QString qnumero=model->datagen(lista_numero.at(i),Qt::DisplayRole).toString();
                informe(qserie,qnumero,true);
            }
            QMessageBox::information(this,tr("Informe documento"),tr("Se han generado los documentos"));
            return;
        }

    informe(serie(),numero());

}

void facturas::latexdoc()
{
    if (!ui.facstableView->currentIndex().isValid())
      {
       QMessageBox::warning( this, tr("Impresión de documentos"),
                           tr("Para generar un documento hay que seleccionarlo de la lista"));
       return;
      }

    QString fichero=latex_doc(serie(),numero());
    if (fichero.isEmpty())
    {
     QMessageBox::warning( this, tr("Edición de documentos"),
                           tr("ERROR: No se ha podido generar Latex"));
     return;
    }

    if (!genera_pdf_latex(fichero))
       {
        QMessageBox::information( this, tr("FACTURA EN PDF"),
                                  tr("ERROR: no ha sido posible generar el archivo PDF"));
        return;
       }
    QString archivopdf=fichero;
    archivopdf.truncate(archivopdf.length()-4);
    archivopdf.append(".pdf");
    QString url=archivopdf;
    QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

    //if (!ejecuta(aplicacionabrirfich(extensionfich(archivopdf)),archivopdf))
    //      QMessageBox::warning( this, tr("FACTURA EN PDF"),
    //                          tr("No se puede abrir ")+archivopdf+tr(" con ")+
    //                          aplicacionabrirfich(extensionfich(archivopdf)) + "\n" +
    //                          tr("Verifique el diálogo de preferencias"));

}

void facturas::edita_tex()
{
    if (!ui.facstableView->currentIndex().isValid())
      {
       QMessageBox::warning( this, tr("Editar latex documento"),
                           tr("Para editar un documento hay que seleccionarlo de la lista"));
       return;
      }

    QString fichero=latex_doc(serie(),numero());
    if (fichero.isEmpty())
    {
     QMessageBox::warning( this, tr("Edición latex documento"),
                           tr("ERROR: No se ha podido generar Latex"));
     return;
    }

    int valor=editalatex_abs(fichero);

  if (valor==1)
    QMessageBox::warning( this, tr("Consulta de mayor"),
                          tr("PROBLEMAS al llamar al editor Latex"));
}


void facturas::exectrasp()
{
    QItemSelectionModel *seleccion;
    seleccion=ui.facstableView->selectionModel();
    QStringList lserie,lnumero;
    if (seleccion->hasSelection())
       {
         QList<QModelIndex> listaserie;
         QList<QModelIndex> listanumero;
         listaserie= seleccion->selectedRows ( 0 );
         listanumero=seleccion->selectedRows (1);


         if (listaserie.size()<1)
          {
             QMessageBox::warning( this, tr("Traspasos"),tr("ERROR: "
                                "se debe de seleccionar al menos un documento"));
           return;
          }

         // comprobamos que no haya ningún documento ya cerrado
         for (int i = 0; i < listaserie.size(); ++i)
            {
             if (basedatos::instancia()->doc_cerrado(model->datagen(listaserie.at(i),Qt::DisplayRole).toString(),
                                                     model->datagen(listanumero.at(i),Qt::DisplayRole).toString()))
             {
               QMessageBox::warning( this, tr("Traspasos"),tr("ERROR: "
                                    "alguno/s de los documentos seleccionados está cerrado"));
               return;

             }
            }

         // comprobamos que todos los clientes sean los mismos
         QString cliente=(basedatos::instancia()->doc_cliente(model->datagen(listaserie.at(0),Qt::DisplayRole).toString(),
                                                              model->datagen(listanumero.at(0),Qt::DisplayRole).toString()));
         for (int i = 1; i < listaserie.size(); ++i)
            {
             if (basedatos::instancia()->doc_cliente(model->datagen(listaserie.at(i),Qt::DisplayRole).toString(),
                                                     model->datagen(listanumero.at(i),Qt::DisplayRole).toString()) != cliente)
             {
               QMessageBox::warning( this, tr("Traspasos"),tr("ERROR: "
                                    "Los documentos no pertenecen a la misma cuenta cliente"));
               return;

             }
            }

         // cuenta, fecha, fecha operación, retención, recargo de equivalencia
         // mejor las cogemos del último documento

         // pasamos contenidos a listas para la plantilla
         for (int i = 0; i < listaserie.size(); ++i)
              {
               lserie << model->datagen(listaserie.at(i),Qt::DisplayRole).toString();
               lnumero << model->datagen(listanumero.at(i),Qt::DisplayRole).toString();
              }
       }


    traspasos *t = new traspasos(lserie,lnumero);
    t->exec();
    delete(t);
    refresca();

}

void facturas::contabilizar()
{

    // está ya contabilizada ??
    if (basedatos::instancia()->doc_contabilizado(serie(),numero()))
    {
        QMessageBox::warning( this, tr("Contabilizar"),tr("ERROR: "
                             "El documento seleccionado ya está contabilizado"));
        return;

    }
    // realizar función aparte
    contabilizar_factura(serie(), numero(), usuario, false);
    refresca();
}

int facturas::numero_de_filas()
{
   return model->rowCount();
}


void facturas::contabilizartodas()
{
  int filas = numero_de_filas();
  for (int veces=filas-1; veces>=0; veces--)
     {
      QString cadnum; cadnum.setNum(veces);
      // QMessageBox::warning( this, tr("Contabilizar"),cadnum);
      QString qserie=model->data(model->index(veces,0),Qt::DisplayRole).toString();
      QString qnumero=model->data(model->index(veces,1),Qt::DisplayRole).toString();
      if (!basedatos::instancia()->doc_contabilizable(qserie,
                              qnumero)) continue;
      if (basedatos::instancia()->doc_contabilizado(qserie,qnumero)) continue;
      contabilizar_factura(qserie, qnumero, usuario, true);
     }
  refresca();
}

void facturas::borralinea()
{
    // está contabilizada ??
    if (basedatos::instancia()->doc_contabilizado(serie(),numero()))
    {
        QMessageBox::warning( this, tr("Borrar registro"),tr("ERROR: "
                             "El documento seleccionado está contabilizado"));
        return;

    }

    if (QMessageBox::question(
             this,
             tr("Borrar registro"),
            tr("¿ Desea borrar el documento seleccionado ?")) == QMessageBox::No )
                           return;


    // borrar
    int clave=basedatos::instancia()->clave_doc_fac(serie(),numero());
    // borramos registros de detalle
    basedatos::instancia()->borralineas_doc(clave);
    // borramos cabecera
    // contabilizar_factura(serie(), numero(), usuario);
    basedatos::instancia()->borra_cabecera_doc(serie(),numero());
    QMessageBox::information( this, tr("Borrar registro"),tr("Se ha eliminado el registro seleccionado"));

    refresca();

}


void facturas::consultaasiento()
{
 if (apunte_diario()=="")
       {
         QMessageBox::warning( this, tr("CONSULTAR ASIENTO"),
         tr("Para consultar asiento debe de seleccionar un registro con apunte en el diario"));
         return;
       }
QString elasiento=basedatos::instancia()->selectAsientodiariopase(apunte_diario());
QString ejercicio=ejerciciodelafecha(basedatos::instancia()->select_fecha_diariopase(apunte_diario()));
consultarasiento(elasiento,usuario,ejercicio);
}


QString facturas::apunte_diario()
{
 if (!ui.facstableView->currentIndex().isValid()) return "";
 int fila=ui.facstableView->currentIndex().row();
 return model->data(model->index(fila,7),Qt::DisplayRole).toString();
}

void facturas::numini_cambiado()
{
  ui.numfinallineEdit->setText(ui.numiniciallineEdit->text());
}

void facturas::cuentaini_cambiada()
{
 ui.cuentafinlineEdit->setText(ui.cuentainilineEdit->text());
}

void facturas::cuentaini_finedicion()
{
  ui.cuentainilineEdit->setText(expandepunto(ui.cuentainilineEdit->text(),anchocuentas()));
}

void facturas::cuentafin_finedicion()
{
 ui.cuentafinlineEdit->setText(expandepunto(ui.cuentafinlineEdit->text(),anchocuentas()));
}

void facturas::actufiltro()
{
 // la variable es guardafiltro
 guardafiltro=" where fecha_fac>='";
 guardafiltro+=ui.inicialdateEdit->date().toString("yyyy-MM-dd");
 guardafiltro+="' and fecha_fac<='";
 guardafiltro+=ui.finaldateEdit->date().toString("yyyy-MM-dd");
 guardafiltro+="' and serie>='";
 guardafiltro+=ui.serieinicialcomboBox->currentText();
 guardafiltro+="' and serie<='";
 guardafiltro+=ui.seriefinalcomboBox->currentText();
 guardafiltro+="'";
 if (!ui.numfinallineEdit->text().isEmpty() || !ui.numiniciallineEdit->text().isEmpty())
   {
    guardafiltro+=" and numero>=";
    guardafiltro+=ui.numiniciallineEdit->text();
    guardafiltro+=" and numero<=";
    guardafiltro+=ui.numfinallineEdit->text();
   }
 if (!ui.cuentafinlineEdit->text().isEmpty() || !ui.cuentainilineEdit->text().isEmpty())
   {
    guardafiltro+=" and cuenta>='";
    guardafiltro+=ui.cuentainilineEdit->text();
    guardafiltro+="' and cuenta<='";
    guardafiltro+=ui.cuentafinlineEdit->text();
    guardafiltro+="'";
   }
 if (ui.nocontaradioButton->isChecked())
     guardafiltro+=" and not contabilizado";
 if (ui.contaradioButton->isChecked())
     guardafiltro+=" and contabilizado";

 if (ui.no_cerradas_radioButton->isChecked())
     guardafiltro+=" and not cerrado";
 if (ui.cerradas_radioButton->isChecked())
     guardafiltro+=" and cerrado";
 if (!ui.externo_lineEdit->text().isEmpty()) {
     guardafiltro+=" and externo='";
     guardafiltro+=ui.externo_lineEdit->text();
     guardafiltro+="'";
 }

 // guardafiltro+=" order by clave desc";
}


void facturas::latexinforme()
{
    bool coma=haycomadecimal();
    bool decimales=haydecimales();
    refresca();

    // vamos a imprimir condiciones del filtro
    // por línea: serie, numero, cuenta, descripcuenta, fecha factura, importe
    QString qfichero=dirtrabajo();
    qfichero.append(QDir::separator());
    qfichero=qfichero+tr("documentos.tex");
    QFile fichero(adapta(qfichero));

    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}\n"
              "{\\Large \\textbf {";
    stream << filtracad(nombreempresa());
    stream << "}}\n";
    stream << "\\end{center}\n\n";

    stream << "\\begin{flushleft}\n";
    stream << "{\\tiny{";
    stream << filtracad(tr("FILTRO: ")) + filtracad(guardafiltro);
    stream << "}}";
    stream << "\\end{flushleft}\n\n";


    stream << "\\begin{center}" << "\n";
    stream << "\\begin{longtable}{|l|l|c|p{5cm}|c|r|}" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}" << "\n";

   stream << "\\multicolumn{6}{|c|} {\\textbf{";
   QString cadena;
   cadena=tr("LISTADO DE DOCUMENTOS");

   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
   stream << "\\hline" << "\n";
   stream << "\\rowcolor{gray!30}" << "\n";

    // -------------------------------------------------------------------------------------------------------
    stream << "{\\textbf{" << tr("Serie") << "}} & ";
    stream << "{\\textbf{" << tr("Número") << "}} & ";
    stream << "{\\textbf{" << tr("Cuenta") << "}} & ";
    stream << "{\\textbf{" << tr("Descripción cuenta") << "}} & ";
    stream << "{\\textbf{" << tr("Fecha") << "}} & ";
    stream << "{\\textbf{" << tr("Importe") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endfirsthead";
    // --------------------------------------------------------------------------------------------------------
    stream << "\\hline" << "\n";
   // stream << tr(" \\\\") << "\n";
    stream << "\\rowcolor{gray!30}" << "\n";
   stream << "\\multicolumn{6}{|c|} {\\textbf{";
   cadena=tr("LISTADO DE DOCUMENTOS");

   // --------------------------------------------------------------------------------------
   stream << cadena;
   stream <<  "}} \\\\";
    stream << "\\hline" << "\n";
    stream << "\\rowcolor{gray!30}" << "\n";
    stream << "{\\textbf{" << tr("Serie") << "}} & ";
    stream << "{\\textbf{" << tr("Número") << "}} & ";
    stream << "{\\textbf{" << tr("Cuenta") << "}} & ";
    stream << "{\\textbf{" << tr("Descripción cuenta") << "}} & ";
    stream << "{\\textbf{" << tr("Fecha") << "}} & ";
    stream << "{\\textbf{" << tr("Importe") << "}}";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";
    stream << "\\endhead" << "\n";
    // ---------------------------------------------------------------------------------------------------------
    double suma_total=0;
    int veces=0;
          while ( veces<model->rowCount())
               {
                double importe_factura=total_documento(
                         model->data(model->index(veces,0),Qt::DisplayRole).toString(),
                         model->data(model->index(veces,1),Qt::DisplayRole).toString());
     suma_total+=importe_factura;
                stream << "{\\footnotesize { " <<
                        filtracad(model->data(model->index(veces,0),Qt::DisplayRole).toString())
                     << "}} & {\\footnotesize {";
                stream << filtracad(model->data(model->index(veces,1),Qt::DisplayRole).toString())
                        << "}} & {\\footnotesize {";
                stream << filtracad(model->data(model->index(veces,2),Qt::DisplayRole).toString())
                        << "}} & {\\footnotesize {";
                QString cad;
                if (model->data(model->index(veces,9),Qt::DisplayRole).toString().isEmpty())
                  {
                   cad=filtracad(descripcioncuenta(model->data(model->index(veces,2),Qt::DisplayRole).toString()));
                  }
                  else
                    {
                      cad=filtracad(basedatos::instancia()->razon_externo(model->data(model->index(veces,9),Qt::DisplayRole).toString()));
                    }
                stream << cad;
                stream << "}} & {\\footnotesize {";
                stream << filtracad(model->data(model->index(veces,4),Qt::DisplayRole).toString())
                        << "}} & {\\footnotesize {";
                stream << formateanumerosep(importe_factura,coma,decimales);
                stream << "}} \\\\ \n  " << "\\hline\n";
                veces++;
              }

    stream << "\\end{longtable}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << tr("TOTAL SELECCIÓN: ") << formateanumerosep(suma_total,coma,decimales);
    stream << "}}\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";
    fichero.close();

}


void facturas::consultainforme()
{
    latexinforme();
    QString fichero=tr("documentos");

    int valor=consultalatex2(fichero);
    if (valor==1)
        QMessageBox::warning( this, tr("Informe documentos"),tr("PROBLEMAS al llamar a Latex"));
    if (valor==2)
        QMessageBox::warning( this, tr("Informe documentos"),
                                 tr("PROBLEMAS al llamar a 'dvips'"));
    if (valor==3)
        QMessageBox::warning( this, tr("Informe documentos"),
                              tr("PROBLEMAS al llamar a ")+visordvi());


}


void facturas::consulta()
{
    if (!ui.facstableView->currentIndex().isValid())
    {
     QMessageBox::warning( this, tr("Edición de documentos"),
                           tr("Para editar un documento hay que seleccionarlo de la lista"));
     return;
    }
    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie(), numero());
    if (q.isActive())
       if (q.next())
        {
         factura *f = new factura();
         // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
         // "contabilizado,contabilizable, con_ret, re, tipo_ret, retencion, "
         // "tipo_doc, notas, pie1, pie2, pase_diario_cta "
         f->pasa_cabecera_doc(q.value(0).toString(),
                         q.value(1).toString(),
                         q.value(2).toString(),
                         q.value(4).toDate(),
                              q.value(25).toDate(),
                         q.value(5).toDate(),
                         q.value(7).toBool(),
                         q.value(8).toBool(),
                         q.value(9).toBool(),
                         q.value(10).toString(),
                         q.value(11).toString(),
                         q.value(12).toString(),
                         q.value(13).toString(),
                         q.value(14).toString(),
                         q.value(15).toString(),
                         q.value(19).toString(),
                         q.value(20).toString(),
                         q.value(21).toString(),
                              q.value(22).toString(),
                              q.value(23).toString(),
                              q.value(24).toString()
                              );
         f->activa_consulta();
         f->exec();
         QString qserie, qnumero;
         if (f->imprime_informe(&qserie,&qnumero)) {
             informe (qserie,  qnumero);
         }
         delete(f);
       }

}


void facturas::borraasiento()
{
 if (apunte_diario()=="")
       {
         QMessageBox::warning( this, tr("BORRAR ASIENTO"),
         tr("Para borrar un asiento debe de seleccionar un registro con apunte en el diario"));
         return;
       }
 if (basedatos::instancia()->apunte_cotabilizado(apunte_diario())) {
           QMessageBox::warning( this, tr("BORRAR ASIENTO"),
                                tr("ERROR: La factura está contabilizada en diario definitivo"));
           return;
 }
QString elasiento=basedatos::instancia()->selectAsientodiariopase(apunte_diario());
QString ejercicio=ejerciciodelafecha(basedatos::instancia()->select_fecha_diariopase(apunte_diario()));
// consultarasiento(elasiento,usuario,ejercicio);
borrarasientofunc(elasiento,ejercicio,false);
refresca();
}


void facturas::escondesalir()
{
  // ui.salirpushButton->setEnabled(false);
}

void facturas::on_busca_externo_pushButton_clicked()
{
  busca_externo *b = new busca_externo();
  int rdo=b->exec();
  if (rdo==QDialog::Accepted) {
         ui.externo_lineEdit->setText(b->codigo_elec());
  }
  else ui.externo_lineEdit->clear();
  delete(b);
}


void facturas::on_externo_lineEdit_textChanged(const QString &arg1)
{
  QString codigo=arg1;
  if (basedatos::instancia()->existe_externo(codigo))
  {
         ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(codigo));
  }
  else ui.descrip_externo_lineEdit->clear();
}


void facturas::on_cuenta_inicial_pushButton_2_clicked()
{
  QString cadena2;
  buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentainilineEdit->text());
  int cod=labusqueda->exec();
  cadena2=labusqueda->seleccioncuenta();
  if (cod==QDialog::Accepted && existesubcuenta(cadena2))
         ui.cuentainilineEdit->setText(cadena2);
  else ui.cuentainilineEdit->setText("");
  delete labusqueda;

}


void facturas::on_cuenta_final_pushButton_clicked()
{
  QString cadena2;
  buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentafinlineEdit->text());
  int cod=labusqueda->exec();
  cadena2=labusqueda->seleccioncuenta();
  if (cod==QDialog::Accepted && existesubcuenta(cadena2))
         ui.cuentafinlineEdit->setText(cadena2);
  else ui.cuentafinlineEdit->setText("");
  delete labusqueda;

}


void facturas::on_quita_externo_pushButton_clicked()
{
  ui.externo_lineEdit->clear();
}


void facturas::on_copia_informe_pushButton_clicked()
{
  QClipboard *cb = QApplication::clipboard();
  QString copy_string;
  copy_string=filtracad(nombreempresa());
  copy_string+="\n\n";
  copy_string+="FILTRO: ";
  copy_string+=guardafiltro;
  copy_string+="\n\n";
  for (int i=0; i < model->columnCount();i++) {
      copy_string+=model->headerData(i,Qt::Horizontal).toString();
         if (i<(model->columnCount()-1)) copy_string+="\t";
         else copy_string+="\n";
  }
  for (int i=0; i < model->rowCount(); i++) {
      for (int j=0; j < model->columnCount();j++) {
         copy_string+=model->index(i,j).data().toString();
         if (j<(model->columnCount()-1)) copy_string+="\t";
         else copy_string+="\n";

      }
  }

  cb->setText(copy_string);

  QMessageBox::information( this, tr("KEME-FACTUR"),
                           tr("Se ha pasado el contenido al portapapeles") );

  //  qDebug() << copy_string;
}

void facturas::on_habilitar_doc_pushButton_clicked()
{

  QItemSelectionModel *seleccion;
  seleccion=ui.facstableView->selectionModel();
  if (seleccion->hasSelection())
  {
      QList<QModelIndex> listaserie;
      QList<QModelIndex> listanumero;
      listaserie= seleccion->selectedRows ( 0 );
      listanumero=seleccion->selectedRows (1);


      if (listaserie.size()<1)
      {
         QMessageBox::warning( this, tr("Habilitar documento(s)"),tr("ERROR: "
                                                        "se debe de seleccionar al menos un documento"));
         return;
      }

      // comprobamos que no haya ningún documento contabilizado
      for (int i = 0; i < listaserie.size(); ++i)
      {
         if (basedatos::instancia()->doc_contabilizado(model->datagen(listaserie.at(i),Qt::DisplayRole).toString(),
                                                 model->datagen(listanumero.at(i),Qt::DisplayRole).toString()))
         {
             QMessageBox::warning( this, tr("KEME-FACTUR"),tr("ERROR: "
                                                            "no se pueden habilitar documentos contabilizados"));
                 return;

         }
      }

      for (int i = 0; i < listaserie.size(); ++i)
      {
         if (basedatos::instancia()->doc_cerrado(model->datagen(listaserie.at(i),Qt::DisplayRole).toString(),
                                                 model->datagen(listanumero.at(i),Qt::DisplayRole).toString()))
             basedatos::instancia()->abre_doc(model->datagen(listaserie.at(i),Qt::DisplayRole).toString(),
                                          model->datagen(listanumero.at(i),Qt::DisplayRole).toString());
         else
             basedatos::instancia()->cierra_doc(model->datagen(listaserie.at(i),Qt::DisplayRole).toString(),
                                              model->datagen(listanumero.at(i),Qt::DisplayRole).toString());
      }

      QMessageBox::information( this, tr("KEME-FACTUR"),tr("Operación realizada"));
   refresca();
  }
}


void facturas::on_email_pushButton_clicked()
{
    if (!ui.facstableView->currentIndex().isValid())
    {
        QMessageBox::warning( this, tr("Envío de documentos"),
                             tr("Para generar un email hay que seleccionarlo de la lista"));
        return;
    }

    QItemSelectionModel *seleccion;
    seleccion=ui.facstableView->selectionModel();
    if (seleccion->hasSelection())
        if (seleccion->selectedRows().count()>1) {
            QList<QModelIndex> lista_serie=seleccion->selectedRows(0);
            QList<QModelIndex> lista_numero=seleccion->selectedRows(1);
            for (int i=0; i<lista_serie.count(); i++) {
                QString qserie=model->datagen(lista_serie.at(i),Qt::DisplayRole).toString();
                QString qnumero=model->datagen(lista_numero.at(i),Qt::DisplayRole).toString();
                informe(qserie,qnumero,true);
                // generamos ahora archivo de texto con email
                gen_email(qserie,qnumero);
            }
            QMessageBox::information(this,tr("Informe documento"),tr("Se han generado los documentos"));
            return;
        }

    informe(serie(),numero(),true);
    // generamos ahora archivo de texto con email
    gen_email(serie(), numero());
}

