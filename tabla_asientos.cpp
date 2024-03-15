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



#include "tabla_asientos.h"
#include "funciones.h"
#include "subcuentas.h"
#include "aux_express.h"
#include "buscasubcuenta.h"
#include "buscaconcepto.h"
#include "conceptos.h"
#include "ivasoportado.h"
#include "ivarepercutido.h"
#include "aib.h"
#include "eib.h"
#include "introci.h"
#include "consmayor.h"
#include "ivadatosadic.h"
#include "ivasop_aut.h"
#include "ivarep_aut.h"
#include "basedatos.h"
#include "exento.h"
#include "aritmetica.h"
#include "consultavencipase.h"
#include "procesavencimiento.h"
#include "pidenombre.h"
#include "asignainputdiario.h"
#include "retencion.h"
#include "directorio.h"
#include "privilegios.h"
#include "busca_externo.h"
#include "externos.h"
#include <QProgressDialog>
#include <QFileDialog>
#include <buscasubcuenta_input.h>

#define COLUMNAS 59  //
                     // en verdad son 60 columnas - 59 es el último índice
#define FILAS 1999 // 2000 filas, 1999 es el último índice

tabla_asientos::tabla_asientos(bool concomadecimal, bool condecimales, QString qusuario) : QDialog() {
  ui.setupUi(this);

  etiqueta = new CustomQLabel;
  etiqueta->setHidden(false);
  etiqueta->setMinimumWidth(90);
  etiqueta->setMinimumHeight(75);
  etiqueta->setText(tr("Soltar archivo\naquí"));
  etiqueta->setAlignment(Qt::AlignHCenter);
  etiqueta->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  ui.horizontalLayout_5->addWidget(etiqueta);

  comadecimal=concomadecimal;
  decimales=condecimales;
  esaib=false;
  eseib=false;
  esaibautonomo=false;
  eseibautonomo=false;
  esautofactura=false;
  esautofactura_no_ue=false;
  espr_servicios_ue=false;
  es_isp_op_interiores=false;
  nocerrar=false;
  forzarcontado=false;
  sinvencimientos=false;
  espredefinido=false;

  hay_fecha_factura=false;

  evitar_pregunta_venci=false;

  pasar_fecha_iva_aut=false;

  avisar_cta_libro_fact_no_diario=true;

  isp_oroinversion=false;

  emitidas_int_isp=false;

  // tener en cuenta coma decimal y sindecimales
  // columnas debe y haber con justificación a la derecha
  aib_autof_fecha_fra=QDate::currentDate();
  eib_fecha_fra=QDate::currentDate();
 QStringList columnas;
 columnas << tr("CUENTA AUX.") << tr("CONCEPTO") << tr("DEBE");
 columnas << tr("HABER") << tr("DOCUMENTO") << tr("CTABASEIVA");
 columnas << tr("BASEIVA") << tr("CLAVEIVA") << tr("TIPOIVA");
 columnas << tr("TIPORE") << tr("CUOTAIVA") << tr("CUENTAFRA");
 columnas << tr("DIA") << tr("MES") << tr("AÑO");
 columnas << tr("SOPORTADO") << tr("CI");

 columnas << tr("PRORRATA") << tr("RECTIFICATIVA");
 columnas << tr("AUTOFACTURA") << tr("FECHA OP.");
 columnas << tr("CLAVE OP.") << tr("BI COSTE");
 columnas << tr("F.RECTIFIC") << tr("NUM.FACT");
 columnas << tr("FAC.INI") << tr("FAC.FINAL");
 columnas << tr("PR.SERV.UE") << tr("AUTOFACTURA_NO_UE") << tr("BIEN INVERSIÓN");
 columnas << tr("OP.NO SUJETA") << tr("AFECCIÓN");
 columnas << tr("AGRARIO") << tr("NOMBRE") << tr("CIF");

 columnas << tr("CTA_RETENIDO") << tr("RET_ARRENDAMIENTO");
 columnas << tr("CLAVE_RET") << tr("BASE_RET");
 columnas << tr("TIPO_RET") << tr("RETENCION");
 columnas << tr("ING_A_CTA") << tr("ING_A_CTA_REP");
 columnas << tr("NOMBRE_RET") << tr("CIF_RET");
 columnas << tr("PROVINCIA");

 ui.Tablaapuntes->setHorizontalHeaderLabels(columnas);

 if (conanalitica() || conanalitica_parc()) ui.Tablaapuntes->setColumnWidth(1,260);
    else ui.Tablaapuntes->setColumnWidth(1,365); // concepto
 ui.Tablaapuntes->setColumnWidth(0,125);  // subcuenta
 ui.Tablaapuntes->setColumnWidth(2,100);  // debe
 ui.Tablaapuntes->setColumnWidth(3,100);  // haber
 ui.Tablaapuntes->setColumnWidth(4,130);  // documento

 ui.Tablaapuntes->hideColumn(5); // ctabaseiva
 ui.Tablaapuntes->hideColumn(6); // baseiva
 ui.Tablaapuntes->hideColumn(7); // claveiva
 ui.Tablaapuntes->hideColumn(8); // tipoiva
 ui.Tablaapuntes->hideColumn(9); // tipore
 ui.Tablaapuntes->hideColumn(10); // cuotaiva
 ui.Tablaapuntes->hideColumn(11); //cuentafra
 ui.Tablaapuntes->hideColumn(12); //día
 ui.Tablaapuntes->hideColumn(13); //mes
 ui.Tablaapuntes->hideColumn(14); //año
 ui.Tablaapuntes->hideColumn(15); //soportado


 if (!conanalitica() && !conanalitica_parc()) ui.Tablaapuntes->hideColumn(16); // CI


 ui.Tablaapuntes->hideColumn(17); //prorrata

 ui.Tablaapuntes->hideColumn(18); //rectificativa
 ui.Tablaapuntes->hideColumn(19); //autofactura

 ui.Tablaapuntes->hideColumn(20); //fecha operación aaaa/mm/dd
 ui.Tablaapuntes->hideColumn(21); //clave operación
 ui.Tablaapuntes->hideColumn(22); //bi a coste
 ui.Tablaapuntes->hideColumn(23); //F.RECTIF
 ui.Tablaapuntes->hideColumn(24); //NUM.FACTURAS
 ui.Tablaapuntes->hideColumn(25); //FAC INI
 ui.Tablaapuntes->hideColumn(26); //FAC FINAL
 ui.Tablaapuntes->hideColumn(27); //prestación servicio ue
 ui.Tablaapuntes->hideColumn(28); //autofactura no ue
 ui.Tablaapuntes->hideColumn(29); //bien_inversion
 ui.Tablaapuntes->hideColumn(30); //operación no sujeta
 ui.Tablaapuntes->hideColumn(31); //afeccion

 ui.Tablaapuntes->hideColumn(32); //agrario
 ui.Tablaapuntes->hideColumn(33); //nombre
 ui.Tablaapuntes->hideColumn(34); //cif


 ui.Tablaapuntes->hideColumn(35); //cuenta retenido
 ui.Tablaapuntes->hideColumn(36); //retención de arrendamiento
 ui.Tablaapuntes->hideColumn(37); //clave retención
 ui.Tablaapuntes->hideColumn(38); //base percepciones
 ui.Tablaapuntes->hideColumn(39); //tipo retención
 ui.Tablaapuntes->hideColumn(40); //retención
 ui.Tablaapuntes->hideColumn(41); //ingreso a cuenta
 ui.Tablaapuntes->hideColumn(42); //ingreso a cuenta repercutido
 ui.Tablaapuntes->hideColumn(43); //nombre retenido
 ui.Tablaapuntes->hideColumn(44); //cif retenido
 ui.Tablaapuntes->hideColumn(45); //provincia
 ui.Tablaapuntes->hideColumn(46); // importación
 ui.Tablaapuntes->hideColumn(47); // exportación
 ui.Tablaapuntes->hideColumn(48); // exenta_no_deducc
 ui.Tablaapuntes->hideColumn(49); // isp_op_interiores

 ui.Tablaapuntes->hideColumn(50); // criterio caja
 ui.Tablaapuntes->hideColumn(51); // no 347
 ui.Tablaapuntes->hideColumn(52); // ventas nacionales fuera territorio aduanero comun

 ui.Tablaapuntes->hideColumn(53); // ORO_INVERSIÓN
 ui.Tablaapuntes->hideColumn(54); // ARRENDAMIENTO LOCAL RETENCIÓN
 ui.Tablaapuntes->hideColumn(55); // ARRENDAMIENTO LOCAL SIN RETENCIÓN
 ui.Tablaapuntes->hideColumn(56); // DUA
 ui.Tablaapuntes->hideColumn(57); // TIPO_INMUEBLE
 ui.Tablaapuntes->hideColumn(58); // RC_INMUEBLE
 ui.Tablaapuntes->hideColumn(59); // DONACIÓN


 if (conigic())
     {
       // modificamos mensajes en botones
       ui.IVApushButton->setText("&Editar IGIC");
       ui.aibcheckBox->setEnabled(false);
       ui.eibcheckBox->setEnabled(false);
     }
 QDate fecha;
 ui.FechaApunte->setDate(fecha.currentDate());
 ui.pegarpushButton-> setShortcut(QKeySequence(tr("CTRL+KEY_V")));

  QStringList diarios;

 diarios << diario_no_asignado();

    QSqlQuery query = basedatos::instancia()->selectCodigoDiariosordercodigo();
    if (query.isActive())
    while (query.next())
       {
           diarios << query.value(0).toString();
       }

    ui.diariocomboBox->addItems(diarios);


    connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
           SLOT(contenidocambiado(int,int )));
    connect(ui.Tablaapuntes,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posceldacambiada ( int, int, int, int  )));
    connect(ui.BotonBorraLinea,SIGNAL(  clicked()),this,SLOT(borralinea( void )));
    connect(ui.BotonBorrar,SIGNAL(  clicked()),this,SLOT(botonborrartodo( void )));
    connect(ui.IVApushButton,SIGNAL(  clicked()),this,SLOT(botonivapulsado()));
    connect(ui.BotonIncorporar,SIGNAL(  clicked()),this,SLOT(incorporar()));
    connect(ui.aibcheckBox,SIGNAL(stateChanged(int)),SLOT(aibcambiado()));
    connect(ui.eibcheckBox,SIGNAL(stateChanged(int)),SLOT(eibcambiado()));
    connect(ui.EsperapushButton,SIGNAL(clicked()),SLOT(botonesperapulsado()));
    connect(ui.pegarpushButton,SIGNAL(clicked()),SLOT(pegar()));
    connect(ui.editarcipushButton,SIGNAL(clicked()),SLOT(procesabotonci()));
    connect(ui.mayorpushButton,SIGNAL(clicked()),SLOT(mayor()));
    connect(ui.BotonCancelar,SIGNAL(clicked()),SLOT(cancelar()));
    connect(ui.FechaApunte,SIGNAL(dateChanged(QDate)),SLOT(fechacambiada()));

    connect(ui.exentacheckBox,SIGNAL(stateChanged(int)),SLOT(exentacambiado()));

    connect(ui.regIVAcheckBox,SIGNAL(stateChanged(int)),SLOT(regIVAcambiado()));

    connect(ui.arribapushButton,SIGNAL(clicked()),SLOT(muevearriba()));
    connect(ui.abajopushButton ,SIGNAL(clicked()),SLOT(mueveabajo()));

    connect(ui.fichdocpushButton ,SIGNAL(clicked()),SLOT(buscacopiadoc()));
    connect(ui.visdocpushButton ,SIGNAL(clicked()),SLOT(visdoc()));

    connect(ui.visdocdefpushButton ,SIGNAL(clicked()),SLOT(visdocdef()));
    connect(ui.impriasigpushButton, SIGNAL(clicked()),SLOT(impridocdef()));

    connect(ui.borravisdocpushButton ,SIGNAL(clicked()),SLOT(suprimedocdef()));

    connect(ui.borraasdocpushButton ,SIGNAL(clicked()),SLOT(borraasignaciondoc()));

    connect(ui.retpushButton ,SIGNAL(clicked()),SLOT(boton_retencion_pulsado()));
    connect(ui.busca_externo_pushButton,SIGNAL(clicked(bool)),SLOT(buscar_externo()));
    connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),SLOT(campo_externo_cambiado()));
    connect(ui.quita_externo_pushButton,SIGNAL(clicked(bool)),SLOT(borra_externo()));
    connect(ui.datos_externo_pushButton,SIGNAL(clicked(bool)),SLOT(ver_externo()));

    connect(ui.input_predef_pushButton,SIGNAL(clicked(bool)),SLOT(imputctas()));

    connect(etiqueta, SIGNAL(filedroped()),SLOT(fichero_soltado()));

    if (es_sqlite())
    {
        cverdadero="1";
        cfalso="0";
    }
    else
    {
        cverdadero="true";
        cfalso="false";
    }

  ivaexentorecibidas=false;

  ui.Tablaapuntes->setFocus();

  usuario=qusuario;

  aib_binversion=false;
  binversionexento=false;
  importacionexento=false;
  exportacionexento=false;
  nodeduccionexento=false;
  opnosujeta=false;
  modoconsulta=false;

  for (int veces=0; veces<=FILAS; veces++)
     inputaciones << NULL;

  lineafactura=0;
  pasefactura=0;
  eib_exterior=false;

  ret_procesada=false;

  no_msj_recepcion=false;

  if (!copiardoc) ui.copiadoccheckBox->setChecked(false);

  if (basedatos::instancia()->gestiondeusuarios() && !privilegios[asigna_fichero])
  { ui.fichdocpushButton->setEnabled(false); ui.borraasdocpushButton->setEnabled(false);}

  if (!basedatos::instancia()->analitica_tabla()) ui.input_predef_pushButton->hide();

  if (!basedatos::instancia()->hay_externos())
     {
      ui.externo_label->hide();
      ui.busca_externo_pushButton->hide();
      ui.externo_lineEdit->hide();
      ui.datos_externo_pushButton->hide();
      ui.quita_externo_pushButton->hide();
      ui.descrip_externo_lineEdit->hide();
     }
}

void tabla_asientos::evita_msj_recepcion()
{
  no_msj_recepcion=true;
}

void tabla_asientos::pasa_num_recepcion(QString cadena)
{
  ui.numreceplineEdit->setText(cadena);
}

void tabla_asientos::activa_eib_exterior()
{
  eib_exterior=true;
}

void tabla_asientos::desactiva_eib_exterior()
{
    eib_exterior=false;
}

void tabla_asientos::activa_fila_nosujeta(int fila)
{
  ui.Tablaapuntes->item(fila,30)->setText("1");
}

void tabla_asientos::activa_fila_prservicios_ue(int fila)
{
  ui.Tablaapuntes->item(fila,27)->setText("1");
}

void tabla_asientos::pasa_fecha_iva_aut(QDate fecha)
{
    pasar_fecha_iva_aut=true;
    fecha_iva_aut=fecha;
}

// IGIC y analítica
void tabla_asientos::contenidocambiado(int fila,int columna)
{

 if (ui.Tablaapuntes->currentColumn()!=columna || ui.Tablaapuntes->currentRow()!=fila) return;
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

if (columna==0) {
    //
    for (int veces=1;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila,veces,newItem);
              if (veces==4 && !docini.isEmpty())
                ui.Tablaapuntes->item(fila,4)->setText(docini);
             }
    //
    if (ui.Tablaapuntes->item(fila,columna)!=0 && !cod_longitud_variable())
       ui.Tablaapuntes->item(fila,columna)->setText(
                              expandepunto(ui.Tablaapuntes->item(fila,columna)->text(),
					       anchocuentas()));
    // EL CÓDIGO SE CORRESPONDE CON EXTERNO ?
    if (basedatos::instancia()->existe_externo(ui.Tablaapuntes->item(fila,columna)->text()))
       {
         ui.externo_lineEdit->setText(ui.Tablaapuntes->item(fila,columna)->text());
         ui.Tablaapuntes->item(fila,columna)->setText(
                     basedatos::instancia()->cuenta_externo(ui.Tablaapuntes->item(fila,columna)->text()));
       }
    // ------------------------------------------------
    if (!existesubcuenta(ui.Tablaapuntes->item(fila,columna)->text()))
       {
        // tratamos de verificar si es un NIF de cuenta auxiliar
        QString codigo_cuenta=basedatos::instancia()->selectCuentaCifDatossubcuenta(
                    ui.Tablaapuntes->item(fila,columna)->text());
        if (!codigo_cuenta.isEmpty()) ui.Tablaapuntes->item(fila,columna)->setText(codigo_cuenta);
       }
    // ------------------------------------------------
    if ((ui.Tablaapuntes->item(fila,columna)->text().length()==anchocuentas() && !cod_longitud_variable())
        || (cod_longitud_variable() && ui.Tablaapuntes->item(fila,columna)->text().length()>3)) {
     if (!existesubcuenta(ui.Tablaapuntes->item(fila,columna)->text()))
       {
           // evaluamos si el código introducido pudiera ser cuenta agregadora (cod_longitud_variable)
           if (cod_longitud_variable() && es_long_var_agregadora(ui.Tablaapuntes->item(fila,columna)->text()))
              {
               QMessageBox::warning( this, tr("Tabla de apuntes"),
                  tr("El código introducido no es de cuenta auxiliar"));
               ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
               if (ui.Tablaapuntes->item(fila,columna)==0)
                  ui.Tablaapuntes->setItem(fila,columna,new QTableWidgetItem(""));
                else
                   ui.Tablaapuntes->item(fila,columna)->setText("");
               connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
                  SLOT(contenidocambiado(int,int )));
               return;
              }
	   // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("El código suministrado no existe \n"
                         "¿ desea añadir la subcuenta ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) {
                       /*
	               subcuentas *lasubcuenta2=new subcuentas();
		       lasubcuenta2->pasacodigo(ui.Tablaapuntes->item(fila,columna)->text());
		       lasubcuenta2->exec();
                       delete(lasubcuenta2);*/

                       aux_express *lasubcuenta2=new aux_express();
                       lasubcuenta2->pasacodigo(ui.Tablaapuntes->item(fila,columna)->text());
                       lasubcuenta2->exec();
                       delete(lasubcuenta2);

	               if (!existesubcuenta(ui.Tablaapuntes->item(fila,columna)->text()))
                           {
                            ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
                            if (ui.Tablaapuntes->item(fila,columna)==0)
                                ui.Tablaapuntes->setItem(fila,columna,new QTableWidgetItem(""));
                              else
                                  ui.Tablaapuntes->item(fila,columna)->setText("");
                            connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
                            SLOT(contenidocambiado(int,int )));
                            return;
                           }
		     }
		  else
                       {
                        ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
                        if (ui.Tablaapuntes->item(fila,columna)==0)
                                ui.Tablaapuntes->setItem(fila,columna,new QTableWidgetItem(""));
                              else
                                  ui.Tablaapuntes->item(fila,columna)->setText("");
                        ui.Tablaapuntes->item(fila,0)->setText("");
                        connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
                           SLOT(contenidocambiado(int,int )));
                        return;
                       }
	   }
        }
    else { // la longitud de la subcuenta no es la correcta
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.Tablaapuntes->item(fila,columna)->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && ((cadena2.length()==anchocuentas() && !cod_longitud_variable()) ||
        (cod_longitud_variable() && esauxiliar(cadena2))))
      ui.Tablaapuntes->item(fila,columna)->setText(cadena2);
       else 
             {
              ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
              ui.Tablaapuntes->item(fila,columna)->setText("");
              connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
                SLOT(contenidocambiado(int,int )));
             }
    delete(labusqueda);
    }
   if (ui.Tablaapuntes->item(fila,columna)->text().length()==0)
     {
       /*
       No borramos línea ya que da problemas de violación de segmento
       borralinea();
        */
     }
    else
    {
     // verificamos limpiariva y retenciones
     if (!escuentadeivasoportado(ui.Tablaapuntes->item(fila,0)->text()) &&
          !escuentadeivarepercutido(ui.Tablaapuntes->item(fila,0)->text()))
         limpiaiva(fila);
     if (!basedatos::instancia()->escuenta_de_ret_ing(ui.Tablaapuntes->item(fila,0)->text()))
         limpiaret(fila);

     QString qconcepto;
     bool asociado=false;

     if (!ui.externo_lineEdit->text().isEmpty())
         asociado=basedatos::instancia()->hayconcepto_externo_asociado(ui.externo_lineEdit->text(), qconcepto);

     if (!asociado)
       asociado=basedatos::instancia()->hayconceptoasociado(ui.Tablaapuntes->item(fila,columna)->text(),
                                                               qconcepto);
     if (!asociado)
         asociado=basedatos::instancia()->hayconceptoasociado_cta(
                 ui.Tablaapuntes->item(fila,columna)->text(),
                 qconcepto);

     if (asociado)
       ui.Tablaapuntes->item(fila,1)->setText(qconcepto);
      else
       {
        // copiamos concepto del pase anterior
        if (ui.Tablaapuntes->item(fila,1)->text().length()==0
          && ui.Tablaapuntes->item(fila,0)->text().length()>0 && fila>0)
          {
           if (ui.Tablaapuntes->item(fila-1,1)!=0)
             {
              ui.Tablaapuntes->item(fila,1)->setText(ui.Tablaapuntes->item(fila-1,1)->text());
             }
          }
       }
    }
  }

if (columna==1)
      {
        ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
        // conceptos
        /*
        if (ui.Tablaapuntes->item(fila,columna)->text().length()==0)
             {
	        QString cadena3;
                buscaconcepto *labusqueda2=new buscaconcepto("");
                labusqueda2->exec();
                cadena3=labusqueda2->seleccionconcepto();
                if (cadena3.length()>0)
		     {
                         QString cadena;
			 existecodigoconcepto(cadena3,&cadena);
			 ui.Tablaapuntes->item(fila,columna)->setText(cadena);
		     }
                  else ui.Tablaapuntes->item(fila,columna)->setText("");
                delete(labusqueda2);
              } */ // es redundante ?

        if (ui.Tablaapuntes->item(fila,columna)->text()=="+" ||
            ui.Tablaapuntes->item(fila,columna)->text()=="-")
          {
            if (fila==0 || escuentadeivasoportado(ui.Tablaapuntes->item(fila,0)->text()) ||
                escuentadeivarepercutido(ui.Tablaapuntes->item(fila,0)->text()))
                 ui.Tablaapuntes->item(fila,columna)->setText("");
               else
                  {
                   // fijamos el valor del debe o el haber
                   double sumadebe=0; bool ok;
                   for (int veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
                    {
                      if (ui.Tablaapuntes->item(veces,0)==0) break;
                      double valor=0;
                      if (ui.Tablaapuntes->item(veces,2)->text().length()>0)
                      valor=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble(&ok);
                      if (ok) sumadebe+=valor;
                    }
                   double sumahaber=0;
                   for (int veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
                      {
                        if (ui.Tablaapuntes->item(veces,0)==0) break;
                        double valor=0;
                        if (ui.Tablaapuntes->item(veces,3)->text().length()>0)
                        valor=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble(&ok);
                        if (ok) sumahaber+=valor;
                      }
                   double saldar=sumadebe-sumahaber;
                   if (saldar>0.0001)
                      {
                       // QString formateanumero(double numero,bool comadecimal, bool decimales)
                       if (ui.Tablaapuntes->item(fila,columna)->text()=="-")
                          {
                           ui.Tablaapuntes->item(fila,2)->setText(formateanumero(-saldar,comadecimal,decimales));
                           contenidocambiado(fila,2);
                           procesa_celda_analitica(fila,2);
                          }
                         else
                             {
                              ui.Tablaapuntes->item(fila,3)->setText(formateanumero(saldar,comadecimal,decimales));
                              contenidocambiado(fila,3);
                              procesa_celda_analitica(fila,3);
                             }
                      }
                   if (saldar<0.0001)
                      {
                       if (ui.Tablaapuntes->item(fila,columna)->text()=="-")
                          {
                            ui.Tablaapuntes->item(fila,3)->setText(formateanumero(saldar,comadecimal,decimales));
                            contenidocambiado(fila,3);
                            procesa_celda_analitica(fila,3);
                          }
                          else
                            {
                             ui.Tablaapuntes->item(fila,2)->setText(formateanumero(-saldar,comadecimal,decimales));
                             contenidocambiado(fila,2);
                             procesa_celda_analitica(fila,2);
                            }
                      }
                   ui.Tablaapuntes->item(fila,columna)->setText(ui.Tablaapuntes->item(fila-1,columna)->text());
                   ui.BotonIncorporar->setFocus();
                  }
          }
    
        if (ui.Tablaapuntes->item(fila,columna)->text().length()<4 && 
            ui.Tablaapuntes->item(fila,columna)->text().length()>0 &&
            ui.Tablaapuntes->item(fila,columna)->text()!="+" &&
            ui.Tablaapuntes->item(fila,columna)->text()!="-")
            {
             QString cadena;
	     if (existecodigoconcepto(ui.Tablaapuntes->item(fila,columna)->text(),&cadena))
	       ui.Tablaapuntes->item(fila,columna)->setText(cadena);
                 else
	          {
                   QString TextoAviso = tr("El código suministrado no existe \n"
                         "¿ desea añadir el concepto ?");
                   msgBox.setText(TextoAviso);
                   msgBox.exec();
	           if (msgBox.clickedButton() == aceptarButton) 
	                 {
			  conceptos *elconcepto=new conceptos();
			  elconcepto->pasacodigo(ui.Tablaapuntes->item(fila,columna)->text());
			  elconcepto->exec();
			  delete(elconcepto);
	                  if (existecodigoconcepto(ui.Tablaapuntes->item(fila,columna)->text(),&cadena))
	                            ui.Tablaapuntes->item(fila,columna)->setText(cadena);
		                else ui.Tablaapuntes->item(fila,columna)->setText("");
		         }
		    else
	               ui.Tablaapuntes->item(fila,columna)->setText("");
	
                  }
            
           }
    connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
                SLOT(contenidocambiado(int,int )));
    }


double valor;
bool ok;


if (columna==0)
{
  // vemos si la cuenta de IVA tiene tipo asociado
  QString tipoiva;
  if (basedatos::instancia()->hayivaasociado_cta_iva(ui.Tablaapuntes->item(fila,0)->text(),
                                                     tipoiva))
            {
             // procesamos esto si la siguiente línea está vacía
             ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));

             procesaiva_cta(fila,columna, tipoiva);
             if (!ui.Tablaapuntes->item(fila,0)->text().isEmpty())
                 ui.Tablaapuntes->setCurrentCell(fila+1,0);
               else ui.Tablaapuntes->setCurrentCell(fila-1,4);
             connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
                  SLOT(contenidocambiado(int,int )));
            }

  double tipo_ret=0;
  QString tipo_operacion_ret;
  bool ret_arrendamiento=false;
  if (fila>0 && !ui.regretencionescheckBox->isChecked() &&
      basedatos::instancia()->hay_ret_asociada_cta_ret(ui.Tablaapuntes->item(fila,0)->text(),
                                                       &tipo_ret,
                                                       &tipo_operacion_ret,
                                                       &ret_arrendamiento) && !ret_procesada)
    {
      ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
      procesa_ret_apunte(fila,columna, ui.Tablaapuntes->item(fila,0)->text(),
                         ui.Tablaapuntes->item(0,2)->text(),
                         tipo_ret, tipo_operacion_ret, ret_arrendamiento);
      if (!ui.Tablaapuntes->item(fila,0)->text().isEmpty())
          ui.Tablaapuntes->setCurrentCell(fila+1,0);
        else ui.Tablaapuntes->setCurrentCell(fila-1,4);
      // ret_procesada=true;
      connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
           SLOT(contenidocambiado(int,int )));
    }
}



if (columna==2 || columna==3) 
  {
   ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
   // VEMOS si hay expresión a evaluar
   QString cadcel=ui.Tablaapuntes->item(fila,columna)->text();
   if (cadcel.contains('+') || cadcel.contains('-') || cadcel.contains('*') || cadcel.contains('/'))
     {
       // reemplazamos todas las comas por puntos
       cadcel.replace(',','.');
       // ahora evaluamos la expresión
       exparitmetica expresion;
       cadcel.remove(' ');
       cadcel.remove('=');
       expresion.inicializa(cadcel);
       double valor=0;
       int okeval;
       valor=expresion.resultado(&okeval);
       if (!okeval) valor=0;
       QString cadval;
       if (valor!=0) cadval.setNum(valor,'f',2);
       ui.Tablaapuntes->item(fila,columna)->setText(cadval);
     }
   valor=convapunto(ui.Tablaapuntes->item(fila,columna)->text()).toDouble(&ok);
   if (ok )
      {
       int ndecimales=2;
       if (!decimales) ndecimales=0;
       if (comadecimal)
           ui.Tablaapuntes->item(fila,columna)->setText(
                    convacoma(QString( "%1" ).arg(
                                valor, 0, 'f',ndecimales ) ));
          else
           ui.Tablaapuntes->item(fila,columna)->setText(
                                QString( "%1" ).arg(
                                valor, 0, 'f',ndecimales ) );
      }
    else
         ui.Tablaapuntes->item(fila,columna)->setText("");
 
    //----------------------------------------------------------------
    // comprobamos si la cuenta de la línea posee cta. IVA asociado
      QString cuentaiva,tipoiva, cta_base_iva;
      bool hayivaasoc=basedatos::instancia()->hayivaasociado_externo(ui.externo_lineEdit->text(),
                                                             cuentaiva, tipoiva, cta_base_iva);
      if (!hayivaasoc)
        hayivaasoc=basedatos::instancia()->hayivaasociado(ui.Tablaapuntes->item(fila,0)->text(),
                                                             cuentaiva, tipoiva, cta_base_iva);
      // si no hayivaasoc, comprobar hayivaasociado_Cta
      bool hayivaasociado_Cta=false;
      if (!hayivaasoc)
          hayivaasociado_Cta=basedatos::instancia()->hayivaasociado_cta(
                  ui.Tablaapuntes->item(fila,0)->text(),
                  cuentaiva, tipoiva, cta_base_iva);

      // -------------------------------------------------

      bool esivabase= escuentadegasto(ui.Tablaapuntes->item(fila,0)->text()) ||
                      escuentadeingreso(ui.Tablaapuntes->item(fila,0)->text());
      if ((hayivaasoc || hayivaasociado_Cta) && esivabase)
        {
         // procesamos esto si la siguiente línea está vacía
         procesaivaasociado(fila,columna,ui.Tablaapuntes->item(fila,0)->text(),
                                                 cuentaiva, tipoiva,
                                                 ui.Tablaapuntes->item(fila,columna)->text());
         ui.Tablaapuntes->setCurrentCell(fila,4);
        }

      if ((hayivaasoc || hayivaasociado_Cta) && !esivabase)
        {
          procesaivaasociado_financ(fila, columna,ui.Tablaapuntes->item(fila,0)->text(),
                                    cuentaiva, tipoiva, ui.Tablaapuntes->item(fila,columna)->text(),cta_base_iva);

        }


    //----------------------------------------------------------
    // AQUÍ iría el procesado de analítica en tabla
    procesa_celda_analitica(fila,columna);


    //------------------------------------------------------------

    if (ui.Tablaapuntes->item(fila,0)->text().length()>0 && 
        escuentadeivasoportado(ui.Tablaapuntes->item(fila,0)->text())
	&& ui.Tablaapuntes->item(fila,columna)->text().length()>0 )
        procesaivasoportado(fila,columna);
       

    if (ui.Tablaapuntes->item(fila,0)->text().length()>0 
        && escuentadeivarepercutido(ui.Tablaapuntes->item(fila,0)->text())
	&& ui.Tablaapuntes->item(fila,columna)->text().length()>0 )
        procesaivarepercutido(fila,columna);

    if (ui.Tablaapuntes->item(fila,0)->text().length()>0
        && basedatos::instancia()->escuenta_de_ret_ing(ui.Tablaapuntes->item(fila,0)->text())
        && ui.Tablaapuntes->item(fila,columna)->text().length()>0 && !ret_procesada)
        procesaretencion(fila,columna);

   connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
      SLOT(contenidocambiado(int,int )));
   }    

ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
if (columna==2)
   {
     if (ui.Tablaapuntes->item(fila,3)!=0)
        {
         QString guarda;
         guarda=ui.Tablaapuntes->item(fila,columna)->text();
         ui.Tablaapuntes->item(fila,3)->setText("");
         ui.Tablaapuntes->item(fila,columna)->setText(guarda);
        }
  }

if (columna==3)
   {
     if (ui.Tablaapuntes->item(fila,2)!=0)
      {
         QString guarda;
         guarda=ui.Tablaapuntes->item(fila,columna)->text();
         ui.Tablaapuntes->item(fila,2)->setText("");
         ui.Tablaapuntes->item(fila,columna)->setText(guarda);
      }
   }
connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
   SLOT(contenidocambiado(int,int )));

if (columna==16)
   {
     if (ui.Tablaapuntes->item(fila,16)!=0)
         {
          QString codci=ui.Tablaapuntes->item(fila,16)->text();
          if (codci.length()>1)
             {
              if (!ciok(codci))
                {
	         QMessageBox::warning( this, tr("Tabla de apuntes"),tr("ERROR: CI incorrecto"));
                 ui.Tablaapuntes->item(fila,16)->setText("");
                }
             }
         }
   }


// Escribimos número en LCD de debe
double sumadebe=0;
 for (int veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
	    {
                if (ui.Tablaapuntes->item(veces,0)==0) break;
                valor=0;
                if (ui.Tablaapuntes->item(veces,2)->text().length()>0)
		   valor=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble(&ok);
		if (ok) sumadebe+=valor;
	    }
 int ndecimales=2;
 if (!decimales) ndecimales=0;
 QString cadena=QString( "%1" ).arg(
                               sumadebe, 0, 'f',ndecimales ) ;
 ui.LCDSumadebe->display(cadena);
   

 // Escribimos número en LCD de haber
double sumahaber=0;
 for (int veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
	    {
                if (ui.Tablaapuntes->item(veces,0)==0) break;
                valor=0;
                if (ui.Tablaapuntes->item(veces,3)->text().length()>0)
		   valor=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble(&ok);
		if (ok) sumahaber+=valor;
	    }
 cadena=QString( "%1" ).arg(
                               sumahaber, 0, 'f',ndecimales ) ;
 ui.LCDSumahaber->display(cadena);

 // Escribimos número en LCD de descuadre
 cadena=QString( "%1" ).arg(
                               sumadebe-sumahaber, 0, 'f',ndecimales ) ;

 if (sumadebe-sumahaber>-0.0001)
      descuadreazul();
 else
      descuadrerojo();;

 ui.LCDDescuadre->display(cadena);
 

if (sumadebe-sumahaber<0.0001 && sumadebe-sumahaber>-0.0001)
   //  && (sumadebe>0.0001 || sumadebe<-0.0001)
   //  && (sumahaber>0.0001 || sumahaber<-0.0001))
    {
     bool incorporar=true;
     int veces;
     if (ui.Tablaapuntes->item(0,1)==0) incorporar=false;
        else
           if (ui.Tablaapuntes->item(0,1)->text().length()==0) incorporar=false;
     for (veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
         {
           if (ui.Tablaapuntes->item(veces,0)==0) break;
           if (ui.Tablaapuntes->item(veces,0)->text().length()==0) break;
           if ((ui.Tablaapuntes->item(veces,2)==0) || (ui.Tablaapuntes->item(veces,3)==0))
              { incorporar=false; break; }
           /* PERMITIMOS APUNTES CON VALOR CERO PERO CON SEGUNDA LÍNEA CON CONTENIDO
           double valordebe=0;
           if (ui.Tablaapuntes->item(veces,2)->text().length()>0)
	     valordebe=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble();
           double valorhaber=0;
           if (ui.Tablaapuntes->item(veces,3)->text().length()>0)
              valorhaber=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble();
           if (valordebe==0 && valorhaber==0) { incorporar=false; break; }
           */
         }
     if (ui.Tablaapuntes->item(1,0)==0) incorporar=false;
         else if (ui.Tablaapuntes->item(1,0)->text().isEmpty()) incorporar=false;
     if (incorporar) 
       {
        ui.BotonIncorporar->setEnabled(true);
        ui.BotonIncorporar->setFocus();
        return;
       }
        else ui.BotonIncorporar->setEnabled(false);
    }
     else ui.BotonIncorporar->setEnabled(false);


 ui.Tablaapuntes->setFocus();
// si estamos en el último campo (documento o código de imputación)
// pasar a la línea inferior, columna 0
 if (ui.Tablaapuntes->currentItem()==0) return;
 if (ui.Tablaapuntes->currentRow()>ui.Tablaapuntes->rowCount()-2) return;
 if (ui.Tablaapuntes->currentColumn()==16)
     { ui.Tablaapuntes->setCurrentCell(ui.Tablaapuntes->currentRow()+1,0); return;}
 if ((conanalitica() || conanalitica_parc()) &&
     !basedatos::instancia()->analitica_tabla())
     {
       QString codcuenta=ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow(),0)->text();
       if (escuentadegasto(codcuenta)
        || escuentadeingreso(codcuenta)) return;
     }

 if (!ui.copiadoccheckBox->isChecked() && ui.Tablaapuntes->currentColumn()>3)
     {
      ui.Tablaapuntes->setCurrentCell(ui.Tablaapuntes->currentRow()+1,0); 
      return;
     }
 if (ui.copiadoccheckBox->isChecked() && ui.Tablaapuntes->currentItem()->text().length()>0
     && ui.Tablaapuntes->currentColumn()>1)
     { ui.Tablaapuntes->setCurrentCell(ui.Tablaapuntes->currentRow()+1,0); }


}


void tabla_asientos::procesa_celda_analitica(int fila, int columna)
{
    bool procesar_analitica=false;

    if (basedatos::instancia()->analitica_tabla() && conanalitica() &&
                          (escuentadegasto(ui.Tablaapuntes->item(fila,0)->text()) ||
                           escuentadeingreso(ui.Tablaapuntes->item(fila,0)->text())))
        procesar_analitica=true;

    if (basedatos::instancia()->analitica_tabla() && conanalitica_parc() &&
        codigo_en_analitica(ui.Tablaapuntes->item(fila,0)->text()))
       procesar_analitica=true;

    if (ui.Tablaapuntes->item(fila,0)->text().length()>0 &&
        ui.Tablaapuntes->item(fila,columna)->text().length()>0
        && procesar_analitica)
      {
        if (inputaciones[fila]==0)
           {
            asignainputdiario *a = new asignainputdiario(comadecimal,decimales);
            inputaciones[fila]=a;
           }

       // primero investigamos si imputaciones no está vacía
        if (!inputaciones[fila]->tablavacia()) inputaciones[fila]->vaciatabla();
        // cargamos  CI y porcentaje correspondiente a la cuenta de la columna 0

        QSqlQuery query = basedatos::instancia()->ci_input(ui.Tablaapuntes->item(fila,0)->text());
        if (query.isActive())
        while (query.next())
           {
             inputaciones[fila]->pasainputacion(query.value(0).toString(),
                                                ui.Tablaapuntes->item(fila,columna)->text(),
                                                query.value(1).toDouble());
           }
        inputaciones[fila]->actualiza_total();
        inputaciones[fila]->pasatotal(ui.Tablaapuntes->item(fila,columna)->text());
        marca_ci(fila);
      }
  chequeatotales();
}

void tabla_asientos::procesaivasoportado(int fila,int columna)
{
 /* QMessageBox msgBox2;
 QPushButton *apunte = msgBox2.addButton(tr("&Apunte"), QMessageBox::ActionRole);
 QPushButton *regularizacion = msgBox2.addButton(tr("&Regularizacion"), QMessageBox::ActionRole);
 QPushButton *cancelar = msgBox2.addButton(tr("&Cancelar"), QMessageBox::ActionRole);
 msgBox2.setWindowTitle(tr("IVA SOPORTADO"));
 msgBox2.setIcon(QMessageBox::Information);

 int z;

 if (ui.Tablaapuntes->item(fila,5)->text().length()>0) z=0;   // columna 5 cta base iva
  else
        {
           QString TextoAviso = tr("¿ Qué operación desea realizar ?");
           msgBox2.setText(TextoAviso);
           msgBox2.exec();
           if (msgBox2.clickedButton() == apunte) z=0;
           if (msgBox2.clickedButton() == regularizacion) z=1;
           if (msgBox2.clickedButton() == cancelar) z=2;
        }
 */
 if (!ui.regIVAcheckBox->isChecked() || ui.Tablaapuntes->item(fila,5)->text().length()>0)
       {
        ivasoportado *qivasoportado = new ivasoportado(comadecimal,decimales);
        QString qcuentabase="";
        QString qbase="";
        QString qcuentaiva;
        QString qcuota;
        QDate qfechafact;
        if (fila>0) 
        if (ui.Tablaapuntes->item(fila-1,0)!=0 && prorrata_iva()<0.001)   
         { // ------------------------------------------------------------------------------
	  qcuentabase=ui.Tablaapuntes->item(fila-1,0)->text();
	  if (ui.Tablaapuntes->item(fila-1,2)->text().length()>=0) qbase=ui.Tablaapuntes->item(fila-1,2)->text();
             else
	     {
	        if (ui.Tablaapuntes->item(fila-1,3)->text().length()>0 && 
                     ui.Tablaapuntes->item(fila-1,3)->text().left(1)!="-")
	          {
	            qbase="-";
	            qbase+=ui.Tablaapuntes->item(fila-1,3)->text();
	          } else qbase=ui.Tablaapuntes->item(fila-1,3)->text().right(
                        ui.Tablaapuntes->item(fila-1,3)->text().length()-1);
	    }
         }
        qcuentaiva=ui.Tablaapuntes->item(fila,0)->text();
        if (ui.Tablaapuntes->item(fila,2)!=0)
            if (ui.Tablaapuntes->item(fila,2)->text().length()>0) qcuota=ui.Tablaapuntes->item(fila,2)->text();
        if (ui.Tablaapuntes->item(fila,3)!=0)
            if (ui.Tablaapuntes->item(fila,3)->text().length()>0) 
	     {
	        if (ui.Tablaapuntes->item(fila,3)->text().left(1)!="-")
	          {
	            qcuota="-";
	            qcuota+=ui.Tablaapuntes->item(fila,3)->text();
	          } else qcuota=ui.Tablaapuntes->item(fila,3)->text().right(
                                      ui.Tablaapuntes->item(fila,3)->text().length()-1);
	    }
       qfechafact=ui.FechaApunte->date();
       qivasoportado->pasadatos(qcuentabase,qbase,qcuentaiva,qcuota,"",qfechafact);
       int resultado=qivasoportado->exec();
       if (resultado==QDialog::Rejected) 
          {
	   ui.Tablaapuntes->item(fila,columna)->setText("");
	   for (int qveces=5;qveces<=15;qveces++) ui.Tablaapuntes->item(fila,qveces)->setText("");
           ui.Tablaapuntes->item(fila,17)->setText("");
          }
       if (resultado==QDialog::Accepted)
               {
                 QString qclaveiva,qtipoiva,qctafra,soportado,qctaiva,qprorrata,qrectificativa,qclaveop,qbicoste;
                 QString frectif,numfact,facini,facfinal,binversion, qafectacion;
                 QString qagrario,qnombre,qcif, import, cajaiva;
                 QDate qfechafact,qfechaop;
                 bool no347;
                 bool arrto_ret, arrto_sin_ret;
                 QString dua;
   	         qivasoportado->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
		  			        &qtipoiva,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qprorrata,
                               &qrectificativa, &qfechaop, &qclaveop,
                               &qbicoste,&frectif,&numfact,
                               &facini, &facfinal, &binversion, &qafectacion,
                               &qagrario, &qnombre, &qcif, &import, &cajaiva, &no347,
                                          &arrto_ret, &arrto_sin_ret, &dua);
                 fecha_factura=qfechafact;
                 hay_fecha_factura=true;
                 // QMessageBox::warning( this, tr("Tabla de apuntes"),fecha_factura.toString("dd-MM-yyyy"));
                 double valprorrata=0;
                 double valafectacion=0;
                 if (qprorrata.length()>0) valprorrata=convapunto(qprorrata).toDouble()/100;
                 if (qafectacion.length()>0) valafectacion=convapunto(qafectacion).toDouble()/100;
                 if (valafectacion<0) valafectacion=1;
                 if (valprorrata<0.00001) valprorrata=1;
	         ui.Tablaapuntes->item(fila,0)->setText(qctaiva);
		  // actualizamos cuota en DEBE ó HABER
	         if (qcuota.left(1)=="-")
		   {
		      if (ui.Tablaapuntes->item(fila,3)->text().length() >0)
                        {
                         double valor;
                         bool ok;
                         valor=CutAndRoundNumberToNDecimals(convapunto(qcuota).right(qcuota.length()-1).toDouble(&ok)*valprorrata*valafectacion,2);
		         if (ok) 
                             ui.Tablaapuntes->item(fila,3)->setText(formateanumero(valor,comadecimal,decimales));
                          else ui.Tablaapuntes->item(fila,3)->setText("");
                        }
		     else 
                          {
                           double valor=CutAndRoundNumberToNDecimals(convapunto(qcuota).toDouble()*valprorrata*valafectacion,2);
                           ui.Tablaapuntes->item(fila,2)->setText(formateanumero(valor,comadecimal,decimales));
                          }
		    }
		     else 
			  {
                double valor=CutAndRoundNumberToNDecimals(convapunto(qcuota).toDouble()*valprorrata*valafectacion,2);
			    if (ui.Tablaapuntes->item(fila,2)->text().length()>0)
			      ui.Tablaapuntes->item(fila,2)->setText(formateanumero(valor,comadecimal,decimales));
			    else {
				ui.Tablaapuntes->item(fila,3)->setText(
                                                       formateanumero(valor*-1,comadecimal,decimales)); }
			  }
		     // actualizamos columnas 5 a 15 y 17
		     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
		     ui.Tablaapuntes->item(fila,6)->setText(qbase);
		     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
		     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
		     ui.Tablaapuntes->item(fila,9)->setText("");
		     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
		     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
		     QString caddia; caddia.setNum(qfechafact.day());
		     QString cadmes; cadmes.setNum(qfechafact.month());
		     QString cadyear; cadyear.setNum(qfechafact.year());
		     ui.Tablaapuntes->item(fila,12)->setText(caddia);
		     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
		     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
		     ui.Tablaapuntes->item(fila,15)->setText(soportado);
		     ui.Tablaapuntes->item(fila,17)->setText(qprorrata);
                     ui.Tablaapuntes->item(fila,18)->setText(qrectificativa);
                     ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                     ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
                     ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
                     ui.Tablaapuntes->item(fila,23)->setText(frectif);
                     ui.Tablaapuntes->item(fila,24)->setText(numfact);
                     ui.Tablaapuntes->item(fila,25)->setText(facini);
                     ui.Tablaapuntes->item(fila,26)->setText(facfinal);
                     ui.Tablaapuntes->item(fila,29)->setText(binversion);
                     ui.Tablaapuntes->item(fila,30)->setText("");
                     ui.Tablaapuntes->item(fila,31)->setText(qafectacion);
                     ui.Tablaapuntes->item(fila,32)->setText(qagrario);
                     ui.Tablaapuntes->item(fila,33)->setText(qnombre);
                     ui.Tablaapuntes->item(fila,34)->setText(qcif);
                     ui.Tablaapuntes->item(fila,46)->setText(import);
                     ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
                     ui.Tablaapuntes->item(fila,51)->setText(no347 ? "1" : "");
                     ui.Tablaapuntes->item(fila,54)->setText(arrto_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,56)->setText(dua);

                     // actualizamos línea siguiente con ctafra si procede
		     int actualizar=1;
		     for (int veces=0;veces<fila;veces++)
			 if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) actualizar=0;
                     if (actualizar && ui.Tablaapuntes->item(fila+1,0)==0)
                         {
                           // *************************************************************
                           for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(fila+1,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+1,veces,newItem);
                              }
                         }
		     if (actualizar && ui.Tablaapuntes->item(fila+1,0)->text().length()==0)
		            {
			     ui.Tablaapuntes->item(fila+1,0)->setText(qctafra);
			     ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
		            }
	          }
         delete(qivasoportado);
        }
 // if (z==2) ui.Tablaapuntes->item(fila,columna)->setText("");
	
}

void tabla_asientos::procesaretencion(int fila,int columna)
{
    if (ui.regretencionescheckBox->isChecked()) return;
    retencion *qretencion = new retencion(comadecimal,decimales);
    // pasamos cuenta retención, base y retención

    QString qcuentaretencion;
    QString qbase_percepciones;
    QString retencion;
    if (fila>0)
    if (ui.Tablaapuntes->item(fila-1,0)!=0)
     { // ------------------------------------------------------------------------------
      if (ui.Tablaapuntes->item(fila-1,2)->text().length()>=0) // debe
          qbase_percepciones=ui.Tablaapuntes->item(fila-1,2)->text();
         else
         {
            if (ui.Tablaapuntes->item(fila-1,3)->text().length()>0 &&
                 !ui.Tablaapuntes->item(fila-1,3)->text().contains("-"))
              {
                qbase_percepciones="-";
                qbase_percepciones+=ui.Tablaapuntes->item(fila-1,3)->text();
              } else qbase_percepciones=ui.Tablaapuntes->item(fila-1,3)->text().remove("-");
          }
     }

    qcuentaretencion=ui.Tablaapuntes->item(fila,0)->text();
    // QMessageBox::warning( this, tr("Tabla de apuntes"),
    //   qcuentaretencion);
    if (ui.Tablaapuntes->item(fila,3)!=0)
        if (ui.Tablaapuntes->item(fila,3)->text().length()>0)
            retencion=ui.Tablaapuntes->item(fila,3)->text();
    if (ui.Tablaapuntes->item(fila,2)!=0)
        if (ui.Tablaapuntes->item(fila,2)->text().length()>0)
         {
            if (!ui.Tablaapuntes->item(fila,2)->text().contains("-"))
              {
                retencion="-";
                retencion+=ui.Tablaapuntes->item(fila,2)->text();
              } else retencion=ui.Tablaapuntes->item(fila,2)->text().remove("-");
        }

   qretencion->pasadatos(qcuentaretencion, qbase_percepciones, retencion);
   int resultado=qretencion->exec();
   if (resultado==QDialog::Rejected)
      {
       ui.Tablaapuntes->item(fila,columna)->setText("");
       for (int qveces=5;qveces<=15;qveces++) ui.Tablaapuntes->item(fila,qveces)->setText("");
       for (int qveces=17;qveces<=COLUMNAS;qveces++)
           ui.Tablaapuntes->item(fila,qveces)->setText("");
      }

   if (resultado==QDialog::Accepted)
           {
             QString cta_retenido, clave, base_percepciones, tipo_ret, retencion, ing_cta;
             QString ing_cta_repercutido, nombre_ret, cif_ret, provincia;
             bool ret_arrendamiento;
             qretencion->recuperadatos(&cta_retenido, &ret_arrendamiento, &clave,
                                            &base_percepciones, &tipo_ret, &retencion,
                                            &ing_cta, &ing_cta_repercutido, &nombre_ret,
                                            &cif_ret, &provincia);
             //ui.Tablaapuntes->item(fila,0)->setText(----); no cambia la cuenta de retención
              // actualizamos retención en DEBE ó HABER
             QString numretencion=ing_cta.isEmpty() ? retencion : ing_cta;
             if (numretencion.contains("-"))
               {
                  if (ui.Tablaapuntes->item(fila,2)->text().length() >0)
                    {
                     double valor;
                     bool ok;
                     valor=convapunto(numretencion.remove("-")).toDouble(&ok);
                     if (ok)
                         ui.Tablaapuntes->item(fila,2)->setText(formateanumero(valor,comadecimal,decimales));
                      else ui.Tablaapuntes->item(fila,2)->setText("");
                    }
                 else ui.Tablaapuntes->item(fila,3)->setText(numretencion);
               }
                 else
                      {
                        if (ui.Tablaapuntes->item(fila,3)->text().length()>0)
                            ui.Tablaapuntes->item(fila,3)->setText(numretencion);
                        else { QString pasar="-"; pasar+=numretencion;
                            ui.Tablaapuntes->item(fila,2)->setText(pasar); }
                      }
             // Actualizamos columnas en tabla para retenciones

             ui.Tablaapuntes->item(fila,35)->setText(cta_retenido); //cuenta retenido
             ui.Tablaapuntes->item(fila,36)->setText(ret_arrendamiento ? "1" : "0"); //retención de arrendamiento
             ui.Tablaapuntes->item(fila,37)->setText(clave); //clave arrendamiento
             ui.Tablaapuntes->item(fila,38)->setText(base_percepciones); //base percepciones
             ui.Tablaapuntes->item(fila,39)->setText(tipo_ret); //tipo retención
             ui.Tablaapuntes->item(fila,40)->setText(retencion); //retención
             ui.Tablaapuntes->item(fila,41)->setText(ing_cta); //ingreso a cuenta
             ui.Tablaapuntes->item(fila,42)->setText(ing_cta_repercutido); //ingreso a cuenta repercutido
             ui.Tablaapuntes->item(fila,43)->setText(nombre_ret); //nombre retenido
             ui.Tablaapuntes->item(fila,44)->setText(cif_ret); //cif retenido
             ui.Tablaapuntes->item(fila,45)->setText(provincia); //provincia

             int actualizar=1;
             for (int veces=0;veces<fila;veces++)
                 if (ui.Tablaapuntes->item(veces,0)->text()==cta_retenido) actualizar=0;
             if (actualizar && ui.Tablaapuntes->item(fila+1,0)==0)
                 {
                   // *************************************************************
                   for (int veces=0;veces<=COLUMNAS;veces++)
                   if (ui.Tablaapuntes->item(fila+1,veces)==0)
                      {
                       QTableWidgetItem *newItem = new QTableWidgetItem("");
                       if (veces==2 || veces==3)
                          newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                       ui.Tablaapuntes->setItem(fila+1,veces,newItem);
                      }
                 }
             if (actualizar && ui.Tablaapuntes->item(fila+1,0)->text().length()==0)
                    {
                     ui.Tablaapuntes->item(fila+1,0)->setText(cta_retenido);
                     ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                    }

            }

    delete(qretencion);
}

void tabla_asientos::procesaivarepercutido(int fila, int columna)
{
/*
 QMessageBox msgBox2;
 QPushButton *apunte = msgBox2.addButton(tr("&Apunte"), QMessageBox::ActionRole);
 QPushButton *regularizacion = msgBox2.addButton(tr("&Regularizacion"), QMessageBox::ActionRole);
 QPushButton *cancelar = msgBox2.addButton(tr("&Cancelar"), QMessageBox::ActionRole);
 msgBox2.setWindowTitle(tr("IVA SOPORTADO"));
 msgBox2.setIcon(QMessageBox::Information);

 int z;

 if (ui.Tablaapuntes->item(fila,5)->text().length()>0) z=0;   // columna 5 cta base iva
  else
        {
           QString TextoAviso = tr("¿ Qué operación desea realizar ?");
           msgBox2.setText(TextoAviso);
           msgBox2.exec();
           if (msgBox2.clickedButton() == apunte) z=0;
           if (msgBox2.clickedButton() == regularizacion) z=1;
           if (msgBox2.clickedButton() == cancelar) z=2;
        }
*/
 if (!ui.regIVAcheckBox->isChecked() || ui.Tablaapuntes->item(fila,5)->text().length()>0)
       {
        ivarepercutido *qivarepercutido = new ivarepercutido(comadecimal,decimales);
        QString qcuentabase="";
        QString qbase="";
        QString qcuentaiva;
        QString qcuota;
        QDate qfechafact;
        if (fila>0) 
        if (ui.Tablaapuntes->item(fila-1,0)!=0)   
         { // ------------------------------------------------------------------------------
	  qcuentabase=ui.Tablaapuntes->item(fila-1,0)->text();
	  if (ui.Tablaapuntes->item(fila-1,3)->text().length()>=0) qbase=ui.Tablaapuntes->item(fila-1,3)->text();
             else
	     {
	        if (ui.Tablaapuntes->item(fila-1,2)->text().length()>0 && 
                     ui.Tablaapuntes->item(fila-1,2)->text().left(1)!="-")
	          {
	            qbase="-";
	            qbase+=ui.Tablaapuntes->item(fila-1,2)->text();
	          } else qbase=ui.Tablaapuntes->item(fila-1,2)->text().right(
                        ui.Tablaapuntes->item(fila-1,2)->text().length()-1);
	    }
         }

        qcuentaiva=ui.Tablaapuntes->item(fila,0)->text();
        if (ui.Tablaapuntes->item(fila,3)!=0)
            if (ui.Tablaapuntes->item(fila,3)->text().length()>0) qcuota=ui.Tablaapuntes->item(fila,3)->text();
        if (ui.Tablaapuntes->item(fila,2)!=0)
            if (ui.Tablaapuntes->item(fila,2)->text().length()>0) 
	     {
	        if (ui.Tablaapuntes->item(fila,2)->text().left(1)!="-")
	          {
	            qcuota="-";
	            qcuota+=ui.Tablaapuntes->item(fila,2)->text();
	          } else qcuota=ui.Tablaapuntes->item(fila,2)->text().right(
                                      ui.Tablaapuntes->item(fila,2)->text().length()-1);
	    }

       qfechafact=ui.FechaApunte->date();
       QString qrectificativa=ui.Tablaapuntes->item(fila,18)->text();
       qivarepercutido->pasadatos(qcuentabase,qbase,qcuentaiva,qcuota,"",qfechafact,qrectificativa);
       int resultado=qivarepercutido->exec();
       if (resultado==QDialog::Rejected) 
          {
	   ui.Tablaapuntes->item(fila,columna)->setText("");
	   for (int qveces=5;qveces<=15;qveces++) ui.Tablaapuntes->item(fila,qveces)->setText("");
           for (int qveces=17;qveces<=COLUMNAS;qveces++) ui.Tablaapuntes->item(fila,qveces)->setText("");
          }

       if (resultado==QDialog::Accepted)
               {
                 QString qclaveiva,qtipoiva,qtipore,qctafra,soportado,qctaiva,qrectif,qclaveop,qbicoste;
                 QDate qfechafact,qfechaop;
                 QString frectif,numfact,facini,facfinal,nombre,cif, cajaiva;
                 QString arrto_ret, arrto_sin_ret;
                 qivarepercutido->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
		  			        &qtipoiva,&qtipore,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qrectif,
                                                &qfechaop, &qclaveop,
                                                &qbicoste,&frectif,&numfact,
                                                &facini, &facfinal,&nombre,&cif, &cajaiva,
                                                &arrto_ret, &arrto_sin_ret);
                 ui.Tablaapuntes->item(fila,0)->setText(qctaiva);
                 hay_fecha_factura=true;
                 fecha_factura=qfechafact;
                 // actualizamos cuota en DEBE ó HABER
                 if (qcuota.left(1)=="-")
                   {
                    if (ui.Tablaapuntes->item(fila,2)->text().length() >0)
                        {
                         double valor;
                         bool ok;
                         valor=convapunto(qcuota.right(qcuota.length()-1)).toDouble(&ok);
                         if (ok)
                             ui.Tablaapuntes->item(fila,2)->setText(formateanumero(valor,comadecimal,decimales));
                          else ui.Tablaapuntes->item(fila,2)->setText("");
                        }
                     else ui.Tablaapuntes->item(fila,3)->setText(qcuota);
                   }
                   else
                       {
                        if (ui.Tablaapuntes->item(fila,3)->text().length()>0)
                           ui.Tablaapuntes->item(fila,3)->setText(qcuota);
                        else { QString pasar="-"; pasar+=qcuota;
                        ui.Tablaapuntes->item(fila,2)->setText(pasar); }
                       }
		     // actualizamos columnas 5 a 13

		     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
		     ui.Tablaapuntes->item(fila,6)->setText(qbase);
		     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
		     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
		     ui.Tablaapuntes->item(fila,9)->setText(qtipore);
		     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
		     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
		     QString caddia; caddia.setNum(qfechafact.day());
		     QString cadmes; cadmes.setNum(qfechafact.month());
		     QString cadyear; cadyear.setNum(qfechafact.year());
		     ui.Tablaapuntes->item(fila,12)->setText(caddia);
		     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
		     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
		     ui.Tablaapuntes->item(fila,15)->setText(soportado);
		     ui.Tablaapuntes->item(fila,18)->setText(qrectif);
             ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
             ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
             ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
             ui.Tablaapuntes->item(fila,23)->setText(frectif);
             ui.Tablaapuntes->item(fila,24)->setText(numfact);
             ui.Tablaapuntes->item(fila,25)->setText(facini);
             ui.Tablaapuntes->item(fila,26)->setText(facfinal);
             ui.Tablaapuntes->item(fila,27)->setText("");
             ui.Tablaapuntes->item(fila,28)->setText("");
             ui.Tablaapuntes->item(fila,29)->setText("");
             ui.Tablaapuntes->item(fila,30)->setText("");
             ui.Tablaapuntes->item(fila,32)->setText("0");
             ui.Tablaapuntes->item(fila,33)->setText(nombre);
             ui.Tablaapuntes->item(fila,34)->setText(cif);
             ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
             ui.Tablaapuntes->item(fila,54)->setText(arrto_ret);
             ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret);
           }
         delete(qivarepercutido);
        }
 // if (z==2) ui.Tablaapuntes->item(fila,columna)->setText("");

}


void tabla_asientos::descuadreazul()
{
    QPalette palette3;
    QBrush brush9(QColor(255, 255, 255, 255));
    brush9.setStyle(Qt::SolidPattern);
    palette3.setBrush(QPalette::Active, QPalette::Base, brush9);
    QBrush brush10(QColor(0, 170, 255, 255));
    brush10.setStyle(Qt::SolidPattern);
    palette3.setBrush(QPalette::Active, QPalette::Window, brush10);
    QBrush brush11(QColor(255, 255, 255, 255));
    brush11.setStyle(Qt::SolidPattern);
    palette3.setBrush(QPalette::Inactive, QPalette::Base, brush11);
    QBrush brush12(QColor(0, 170, 255, 255));
    brush12.setStyle(Qt::SolidPattern);
    palette3.setBrush(QPalette::Inactive, QPalette::Window, brush12);
    QBrush brush13(QColor(0, 170, 255, 255));
    brush13.setStyle(Qt::SolidPattern);
    palette3.setBrush(QPalette::Disabled, QPalette::Base, brush13);
    QBrush brush14(QColor(0, 170, 255, 255));
    brush14.setStyle(Qt::SolidPattern);
    palette3.setBrush(QPalette::Disabled, QPalette::Window, brush14);
    ui.LCDDescuadre->setPalette(palette3);

}

void tabla_asientos::descuadrerojo()
{
    QPalette palette4;
    QBrush brush15(QColor(255, 255, 255, 255));
    brush15.setStyle(Qt::SolidPattern);
    palette4.setBrush(QPalette::Active, QPalette::Base, brush15);
    QBrush brush16(QColor(255, 85, 0, 255));
    brush16.setStyle(Qt::SolidPattern);
    palette4.setBrush(QPalette::Active, QPalette::Window, brush16);
    QBrush brush17(QColor(255, 255, 255, 255));
    brush17.setStyle(Qt::SolidPattern);
    palette4.setBrush(QPalette::Inactive, QPalette::Base, brush17);
    QBrush brush18(QColor(255, 85, 0, 255));
    brush18.setStyle(Qt::SolidPattern);
    palette4.setBrush(QPalette::Inactive, QPalette::Window, brush18);
    QBrush brush19(QColor(255, 85, 0, 255));
    brush19.setStyle(Qt::SolidPattern);
    palette4.setBrush(QPalette::Disabled, QPalette::Base, brush19);
    QBrush brush20(QColor(255, 85, 0, 255));
    brush20.setStyle(Qt::SolidPattern);
    palette4.setBrush(QPalette::Disabled, QPalette::Window, brush20);
    ui.LCDDescuadre->setPalette(palette4);

}

void tabla_asientos::posceldacambiada ( int row , int col, int previousRow, int previousColumn )
{
    QString cadena;
    if (ui.Tablaapuntes->item(row,0)==0) ui.Tablaapuntes->setCurrentCell(ultimafila(),0);
      else
          if (ui.Tablaapuntes->item(row,0)->text().length()==0
              && ui.Tablaapuntes->item(row,1)->text().length()==0
              && ui.Tablaapuntes->item(row,2)->text().length()==0
              && ui.Tablaapuntes->item(row,3)->text().length()==0)
        	ui.Tablaapuntes->setCurrentCell(ultimafila(),0);

    ui.CodigoEd->setText("");
    ui.NombreEd->setText("");
    ui.saldolineEdit->setText("");


    if (previousColumn==1)
       {
        if (ui.Tablaapuntes->item(previousRow,previousColumn)!=0 &&
          ui.Tablaapuntes->item(previousRow,previousColumn)->text().length()==0)
             {
                QString cadena3;
                buscaconcepto *labusqueda2=new buscaconcepto("");
                labusqueda2->exec();
                cadena3=labusqueda2->seleccionconcepto();
                if (cadena3.length()>0)
                     {
                         QString cadena;
                         existecodigoconcepto(cadena3,&cadena);
                         ui.Tablaapuntes->item(previousRow,previousColumn)->setText(cadena);
                     }
                  else ui.Tablaapuntes->item(previousRow,previousColumn)->setText("");
                delete(labusqueda2);
              }
       }


    if (ui.Tablaapuntes->item(row,0)!=0)
        {
         ui.CodigoEd->setText(ui.Tablaapuntes->item(row,0)->text());
         existecodigoplan(ui.Tablaapuntes->item(row,0)->text(),&cadena);
         ui.NombreEd->setText(cadena);
         ui.saldolineEdit->setText("");

         if (col==16 && conanalitica() && !escuentadegasto(ui.Tablaapuntes->item(row,0)->text())
              && !escuentadeingreso(ui.Tablaapuntes->item(row,0)->text()))
             {
              /* if (escuentadegasto(ui.Tablaapuntes->item(previousRow,0)->text()) ||
                 escuentadeingreso(ui.Tablaapuntes->item(previousRow,0)->text()))
                 ui.editarcipushButton->setEnabled(true); else ui.editarcipushButton->setEnabled(false); */
              ui.Tablaapuntes->setCurrentCell(previousRow,previousColumn);
              row=previousRow;
             }

         if (col==16 && conanalitica_parc() && !codigo_en_analitica(ui.Tablaapuntes->item(row,0)->text()))
             {
              /* if (codigo_en_analitica(ui.Tablaapuntes->item(previousRow,0)->text()))
                 ui.editarcipushButton->setEnabled(true); else ui.editarcipushButton->setEnabled(false); */
              ui.Tablaapuntes->setCurrentCell(previousRow,previousColumn);
              row=previousRow;
             }

         // ojo no se verifca que sea NULL el puntero

         if (ui.Tablaapuntes->item(row,0)!=0 && ui.Tablaapuntes->item(row,0)->text().length()>0)
            {
             // bool ok;
             QString zej=ejerciciodelafecha(ui.FechaApunte->date());
             double valor=saldocuentaendiarioejercicio(ui.Tablaapuntes->item(row,0)->text(),zej);
             // double valor=basedatos::instancia()->selectSaldosaldossubcuentacodigo(
             //                                     ui.Tablaapuntes->item(row,0)->text() , &ok );
             ui.saldolineEdit->setText(formateanumero(valor,comadecimal,decimales));
            }
         }
    if  (row>0 && ui.Tablaapuntes->item(row-1,0)==0)
        	ui.Tablaapuntes->setCurrentCell(ultimafila(),0);
        else
             if  (row>0 && ui.Tablaapuntes->item(row-1,0)->text().length()==0)
        	ui.Tablaapuntes->setCurrentCell(ultimafila(),0);

    if (ui.Tablaapuntes->item(row,0)!=0)
      {
       if ((escuentadeivasoportado(ui.Tablaapuntes->item(row,0)->text()) ||
	   escuentadeivarepercutido(ui.Tablaapuntes->item(row,0)->text()))
	   && ui.Tablaapuntes->item(row,6)->text().length()>0)
    ui.IVApushButton->setEnabled(true); else ui.IVApushButton->setEnabled(false);
       if (ui.Tablaapuntes->item(row,35)->text().length()>0)
           ui.retpushButton->setEnabled(true); else ui.retpushButton->setEnabled(false);
       }

    if (conanalitica() && ui.Tablaapuntes->item(row,0)!=0)
      {
       if (escuentadegasto(ui.Tablaapuntes->item(row,0)->text()) ||
	   escuentadeingreso(ui.Tablaapuntes->item(row,0)->text()))
         {
          ui.editarcipushButton->setEnabled(true);
          if (basedatos::instancia()->analitica_tabla() &&
              inputaciones[row]==0)
             {
              asignainputdiario *a = new asignainputdiario(comadecimal,decimales);
              inputaciones[row]=a;
             }
         }
        else ui.editarcipushButton->setEnabled(false);
      }
      else if (!conanalitica_parc()) ui.editarcipushButton->setEnabled(false);

    if (conanalitica_parc() && ui.Tablaapuntes->item(row,0)!=0)
      {
       if (codigo_en_analitica(ui.Tablaapuntes->item(row,0)->text()))
        {
          ui.editarcipushButton->setEnabled(true);
          if (basedatos::instancia()->analitica_tabla() &&
              inputaciones[row]==0)
             {
              asignainputdiario *a = new asignainputdiario(comadecimal,decimales);
              inputaciones[row]=a;
             }
        }
        else ui.editarcipushButton->setEnabled(false);
      }
      else if (!conanalitica()) ui.editarcipushButton->setEnabled(false);


    if (ui.Tablaapuntes->item(0,0)!=0)
      {
       if (ui.Tablaapuntes->item(0,0)->text().length()>0 && !ui.EsperapushButton->isEnabled())
         ui.EsperapushButton->setEnabled(true);
       if (ui.Tablaapuntes->item(0,0)->text().length()==0 && ui.EsperapushButton->isEnabled())
         ui.EsperapushButton->setEnabled(false);
      } else ui.EsperapushButton->setEnabled(false);

}



int tabla_asientos::ultimafila()
{
  int veces=0;
  while (veces<ui.Tablaapuntes->rowCount ())
    {
      if (ui.Tablaapuntes->item(veces,0)==0) break;
      if (ui.Tablaapuntes->item(veces,0)->text().length()==0
          && ui.Tablaapuntes->item(veces,2)->text().length()==0
          && ui.Tablaapuntes->item(veces,3)->text().length()==0) break;
      veces++;
    }
  if (veces==ui.Tablaapuntes->rowCount()) return veces-1;
 return veces;
}


void tabla_asientos::borralinea( void )
{
 if (!ui.Tablaapuntes->currentIndex().isValid()) return;
 int fila=ui.Tablaapuntes->currentRow();
 if (ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow(),0)==0) return;
 if (ui.Tablaapuntes->item(fila,35)->text().length()>0) ret_procesada=false;
 if (fila<0 || fila >= ui.Tablaapuntes->rowCount()) return;
 ui.Tablaapuntes->removeRow(fila);
 if (inputaciones.at(fila)!=NULL) delete(inputaciones.at(fila));
 inputaciones.removeAt(fila);
actualizaestado();
}

void tabla_asientos::botonborrartodo()
{
  borratodo(true);
}

void tabla_asientos::borratodo(bool preguntar)
{
   if (modoconsulta) return;

   if (preguntar)
     if (QMessageBox::question(
            this,
            tr("Tabla de asientos"),
            tr("¿ Desea borrar todo el contenido ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==1 )
                          return;
 ret_procesada=false;
 ui.Tablaapuntes->clearContents();
 descuadreazul();
 ui.LCDSumadebe->display("0");
 ui.LCDSumahaber->display("0");
 ui.LCDDescuadre->display("0");
 ui.BotonIncorporar->setEnabled(false);
 ui.EsperapushButton->setEnabled(false);
 ui.Tablaapuntes->setCurrentCell(0,0);
 int filas=inputaciones.count();
 for (int veces=0; veces<filas; veces++)
   {
    if (inputaciones[veces] != NULL)
     {
        delete (inputaciones[veces]);
        inputaciones[veces]=NULL;
     }
   }

}



void tabla_asientos::botonivapulsado()
{
   ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));

   int fila=ui.Tablaapuntes->currentRow();

   if (escuentadeivarepercutido(ui.Tablaapuntes->item(fila,0)->text()))
      {
        procesabotonivarepercutido();
        if (modoconsulta) return;
        connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
        SLOT(contenidocambiado(int,int )));
        actualizaestado();
        return;
      }
    
   ivasoportado *qIVAsoportado = new ivasoportado(comadecimal,decimales);
   QDate fecha( ui.Tablaapuntes->item(fila,14)->text().toInt(),
	         ui.Tablaapuntes->item(fila,13)->text().toInt(),
	         ui.Tablaapuntes->item(fila,12)->text().toInt());
   QDate fechaoperacion=QDate::fromString(ui.Tablaapuntes->item(fila,20)->text()
                                          , "yyyy-MM-dd");

   qIVAsoportado->pasadatos2( ui.Tablaapuntes->item(fila,5)->text(), 
			      ui.Tablaapuntes->item(fila,6)->text(), 
			      ui.Tablaapuntes->item(fila,7)->text(), 
			      ui.Tablaapuntes->item(fila,8)->text(), 
			      ui.Tablaapuntes->item(fila,0)->text(), 
			      ui.Tablaapuntes->item(fila,10)->text(), 
			      ui.Tablaapuntes->item(fila,11)->text(),
		                     fecha,
                         ui.Tablaapuntes->item(fila,17)->text(), // prorrata
                         ui.Tablaapuntes->item(fila,18)->text(), // rectificativa
                         fechaoperacion,
                         ui.Tablaapuntes->item(fila,21)->text(), // clave operacion
                         ui.Tablaapuntes->item(fila,22)->text(), // bi a coste
                         ui.Tablaapuntes->item(fila,23)->text(), // factura rectificada
                         ui.Tablaapuntes->item(fila,24)->text(), // Número de facturas
                         ui.Tablaapuntes->item(fila,25)->text(), // Inicial
                         ui.Tablaapuntes->item(fila,26)->text(), // Final
                         ui.Tablaapuntes->item(fila,29)->text(), // Bien inversion
                         ui.Tablaapuntes->item(fila,31)->text(), //  Afectación
                         ui.Tablaapuntes->item(fila,32)->text(), //  Agrario
                         ui.Tablaapuntes->item(fila,33)->text(), //  Nombre
                         ui.Tablaapuntes->item(fila,34)->text(), //  CIF
                         ui.Tablaapuntes->item(fila,46)->text(), // importación
                         ui.Tablaapuntes->item(fila,50)->text(), // criterio de caja
                         ui.Tablaapuntes->item(fila,51)->text()=="1" ? true : false, // no347
                         ui.Tablaapuntes->item(fila,54)->text()=="1" ? true : false, // arrto
                         ui.Tablaapuntes->item(fila,55)->text()=="1" ? true : false, // arrto ret.
                              ui.Tablaapuntes->item(fila,56)->text() // DUA
                              );
   // activar modo consulta
   if (modoconsulta) qIVAsoportado->modoconsulta();
   int resultado=qIVAsoportado->exec();
   if (resultado==QDialog::Rejected)
            {
            }
   if (resultado==QDialog::Accepted && !modoconsulta)
           {
             QString qclaveiva,qtipoiva,qctafra,soportado,qctaiva,qcuentabase,qbase,qcuota,
                   qprorrata, qrectificativa, qclaveop, qbicoste;
             QString frectif,numfact,facini,facfinal, binversion, afectacion;
             QString agrario, nombre, cif, import, cajaiva;
             QDate qfechafact, qfechaop;//-------------------------------------------------
             bool no347;
             bool arrto_ret, arrto_sin_ret;
             QString dua;
	     qIVAsoportado->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
					&qtipoiva,&qctaiva, &qcuota,&qctafra,
                                        &qfechafact,&soportado,&qprorrata,
                               &qrectificativa, &qfechaop, &qclaveop,
                               &qbicoste,&frectif,&numfact,
                               &facini, &facfinal, &binversion, &afectacion,
                               &agrario, &nombre, &cif, &import, &cajaiva, &no347,
                               &arrto_ret, &arrto_sin_ret, &dua);
             double valprorrata=0;
             double valafectacion=0;
             if (qprorrata.length()>0) valprorrata=convapunto(qprorrata).toDouble()/100;
             if (afectacion.length()>0) valafectacion=convapunto(afectacion).toDouble()/100;
             if (valprorrata<0.00001) valprorrata=1;
             if (valafectacion<0) valafectacion=1;
	     ui.Tablaapuntes->item(fila,0)->setText(qctaiva);
	     // actualizamos cuota en DEBE ó HABER
	     if (qcuota.left(1)=="-")
			   {
			      if (ui.Tablaapuntes->item(fila,3)->text().length() >0)
                                 {
                                  double valor=convapunto(qcuota).toDouble()*-1*valprorrata*valafectacion;
				  ui.Tablaapuntes->item(fila,3)->setText(
                                    formateanumero(valor,comadecimal,decimales));
                                 }
			      else {
                                    double valor=convapunto(qcuota).toDouble()*valprorrata*valafectacion;
                                    ui.Tablaapuntes->item(fila,2)->setText(
                                      formateanumero(valor,comadecimal,decimales));
                                   }
			  }
		        else 
			  { 
			    if (ui.Tablaapuntes->item(fila,2)->text().length()>0)
                               {
                                double valor=convapunto(qcuota).toDouble()*valprorrata*valafectacion;
				ui.Tablaapuntes->item(fila,2)->setText(
                                      formateanumero(valor,comadecimal,decimales));
                               }
			    else {
                                  double valor=convapunto(qcuota).toDouble()*valprorrata*valafectacion-1;
				  ui.Tablaapuntes->item(fila,3)->setText(
                                      formateanumero(valor,comadecimal,decimales)); 
                                 }
			  }
             // actualizamos columnas 5 a 15 y 17,18 a 22
	     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
	     ui.Tablaapuntes->item(fila,6)->setText(qbase);
	     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
	     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
	     ui.Tablaapuntes->item(fila,9)->setText("");
	     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
	     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
	     QString caddia; caddia.setNum(qfechafact.day());
	     QString cadmes; cadmes.setNum(qfechafact.month());
	     QString cadyear; cadyear.setNum(qfechafact.year());
	     ui.Tablaapuntes->item(fila,12)->setText(caddia);
	     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
	     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
	     ui.Tablaapuntes->item(fila,15)->setText(soportado);
           ui.Tablaapuntes->item(fila,17)->setText(qprorrata);
           ui.Tablaapuntes->item(fila,18)->setText(qrectificativa);
           ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
           ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
           ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
             ui.Tablaapuntes->item(fila,23)->setText(frectif);
             ui.Tablaapuntes->item(fila,24)->setText(numfact);
             ui.Tablaapuntes->item(fila,25)->setText(facini);
             ui.Tablaapuntes->item(fila,26)->setText(facfinal);
             ui.Tablaapuntes->item(fila,29)->setText(binversion);
             ui.Tablaapuntes->item(fila,31)->setText(afectacion);
             ui.Tablaapuntes->item(fila,32)->setText(agrario);
             ui.Tablaapuntes->item(fila,33)->setText(nombre);
             ui.Tablaapuntes->item(fila,34)->setText(cif);
             ui.Tablaapuntes->item(fila,46)->setText(import);
             ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
             ui.Tablaapuntes->item(fila,51)->setText(no347 ? "1" : "");
             ui.Tablaapuntes->item(fila,54)->setText(arrto_ret ? "1" : "");
             ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret ? "1" : "");
             ui.Tablaapuntes->item(fila,56)->setText(dua);

             // actualizamos línea siguiente con ctafra si procede
	     int actualizar=1;
	     for (int veces=0;veces<fila;veces++)
		 if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) actualizar=0;
	     if (actualizar && ui.Tablaapuntes->item(fila+1,0)->text().length()==0)
		            {
			     ui.Tablaapuntes->item(fila+1,0)->setText(qctafra);
			     ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
		            }
	  }
    delete(qIVAsoportado);

   connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
      SLOT(contenidocambiado(int,int )));
   actualizaestado();
}


void tabla_asientos::procesabotonci()
{
 int fila=ui.Tablaapuntes->currentRow();

 if (basedatos::instancia()->analitica_tabla() && inputaciones[fila]==NULL)
    {
     asignainputdiario *a = new asignainputdiario(comadecimal,decimales);
     inputaciones[fila]=a;
    }

  if (basedatos::instancia()->analitica_tabla() && inputaciones[fila]!=NULL)
   {
      // QMessageBox::warning( this, tr("Tabla de apuntes"),
      //   tr("El código introducido no es de cuenta auxiliar"));
      QString qimporte;
      qimporte=ui.Tablaapuntes->item(fila,2)->text().isEmpty() ?
               ui.Tablaapuntes->item(fila,3)->text() :
               ui.Tablaapuntes->item(fila,2)->text();
      inputaciones[fila]->pasatotal(qimporte);
      inputaciones[fila]->exec();
      marca_ci(fila);
      fechacambiada();
      return;
   }


 introci *c = new introci();
 c->pasaci(ui.Tablaapuntes->item(fila,16)->text());
 int resultado=c->exec();
 if (resultado==QDialog::Accepted)
    ui.Tablaapuntes->item(fila,16)->setText(c->cadenaci());
 delete(c);
}

void tabla_asientos::procesabotonivarepercutido()
{
   int fila=ui.Tablaapuntes->currentRow();
   ivarepercutido *qIVArepercutido = new ivarepercutido(comadecimal,decimales);
   QDate fecha( ui.Tablaapuntes->item(fila,14)->text().toInt(),
	         ui.Tablaapuntes->item(fila,13)->text().toInt(),
	         ui.Tablaapuntes->item(fila,12)->text().toInt());
   QDate fechaoperacion=QDate::fromString(ui.Tablaapuntes->item(fila,20)->text()
                                          , "yyyy-MM-dd");
   qIVArepercutido->pasadatos2( ui.Tablaapuntes->item(fila,5)->text(),
			        ui.Tablaapuntes->item(fila,6)->text(), 
			        ui.Tablaapuntes->item(fila,7)->text(), 
			        ui.Tablaapuntes->item(fila,8)->text(), 
			        ui.Tablaapuntes->item(fila,9)->text(), 
			        ui.Tablaapuntes->item(fila,0)->text(), 
			        ui.Tablaapuntes->item(fila,10)->text(), 
			        ui.Tablaapuntes->item(fila,11)->text(),
		                     fecha,
                                ui.Tablaapuntes->item(fila,18)->text(),  // rectificativa
                                fechaoperacion,
                                ui.Tablaapuntes->item(fila,21)->text(), // clave operacion
                                ui.Tablaapuntes->item(fila,22)->text(), // bi a coste
                                ui.Tablaapuntes->item(fila,23)->text(), // factura rectificada
                                ui.Tablaapuntes->item(fila,24)->text(), // Número de facturas
                                ui.Tablaapuntes->item(fila,25)->text(), // Inicial
                                ui.Tablaapuntes->item(fila,26)->text(), // Final
                                ui.Tablaapuntes->item(fila,33)->text(), // Nombre
                                ui.Tablaapuntes->item(fila,34)->text(), // Cif
                                ui.Tablaapuntes->item(fila,50)->text(),
                                ui.Tablaapuntes->item(fila,54)->text(),
                                ui.Tablaapuntes->item(fila,55)->text()
                               );
   // activar modo consulta
   if (modoconsulta) qIVArepercutido->modoconsulta();
   int resultado=qIVArepercutido->exec();
   if (resultado==QDialog::Rejected) 
            {
            }
   if (resultado==QDialog::Accepted && !modoconsulta)
            {
             QString qclaveiva,qtipoiva,qtipore,qctafra,soportado,qctaiva,qcuentabase,qbase,qcuota,
                     qrectif,qclaveop,qbicoste,frectif,numfact,facini,facfinal,nombre,cif,cajaiva,
                     arrto_ret, arrto_sin_ret;
             QDate qfechafact,qfechaop;
	     qIVArepercutido->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
					&qtipoiva,&qtipore,&qctaiva, &qcuota,&qctafra,
                                        &qfechafact,&soportado,&qrectif, &qfechaop, &qclaveop,
                                        &qbicoste,&frectif,&numfact,
                                        &facini, &facfinal, &nombre, &cif, &cajaiva, &arrto_ret, &arrto_sin_ret);
	     ui.Tablaapuntes->item(fila,0)->setText(qctaiva);
	     // actualizamos cuota en DEBE ó HABER
	     if (qcuota.left(1)=="-")
			   {
			      if (ui.Tablaapuntes->item(fila,2)->text().length() >0)
				  ui.Tablaapuntes->item(fila,2)->setText(qcuota.right(qcuota.length()-1));
			      else ui.Tablaapuntes->item(fila,3)->setText(qcuota);
			  }
		        else 
			  { 
			    if (ui.Tablaapuntes->item(fila,3)->text().length()>0)
				ui.Tablaapuntes->item(fila,3)->setText(qcuota);
			    else { QString pasar="-"; pasar+=qcuota;
				ui.Tablaapuntes->item(fila,2)->setText(pasar); }
			  }
	     // actualizamos columnas 5 a --
	     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
	     ui.Tablaapuntes->item(fila,6)->setText(qbase);
	     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
	     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
	     ui.Tablaapuntes->item(fila,9)->setText(qtipore);
	     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
	     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
	     QString caddia; caddia.setNum(qfechafact.day());
	     QString cadmes; cadmes.setNum(qfechafact.month());
	     QString cadyear; cadyear.setNum(qfechafact.year());
	     ui.Tablaapuntes->item(fila,12)->setText(caddia);
	     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
	     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
	     ui.Tablaapuntes->item(fila,15)->setText(soportado);
             ui.Tablaapuntes->item(fila,18)->setText(qrectif);
             ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
             ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
             ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
             ui.Tablaapuntes->item(fila,23)->setText(frectif);
             ui.Tablaapuntes->item(fila,24)->setText(numfact);
             ui.Tablaapuntes->item(fila,25)->setText(facini);
             ui.Tablaapuntes->item(fila,26)->setText(facfinal);
             ui.Tablaapuntes->item(fila,33)->setText(nombre);
             ui.Tablaapuntes->item(fila,34)->setText(cif);
             ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
             ui.Tablaapuntes->item(fila,54)->setText(arrto_ret);
             ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret);
            }
    delete(qIVArepercutido);
}



void tabla_asientos::incorporar()
        // procesa también la consulta de eib aib y exentas
        // para el modo consulta
{
   int numfilas=ultimafila();
   // comprobamos que la cuenta de factura en campos de IVA esté representada en el primer campo
   // de la tabla
   QString compro_cuenta;
   for (int veces=0; veces<numfilas; veces++)
      {
        if (ui.Tablaapuntes->item(veces,0) ==0) continue;
        QString cuenta_factura=ui.Tablaapuntes->item(veces,11) !=0 ?
                               ui.Tablaapuntes->item(veces,11)->text() : QString();
        if (!cuenta_factura.isEmpty())
           {
            bool encontrada=false;
            for (int veces2=0; veces2<numfilas; veces2++)
               {
                QString comparar=ui.Tablaapuntes->item(veces2,0) !=0 ?
                                 ui.Tablaapuntes->item(veces2,0)->text() : QString();
                if (comparar==cuenta_factura)
                   {
                    encontrada=true;
                    break;
                   }
               }
            if (!encontrada && avisar_cta_libro_fact_no_diario)
              {
                if (!QMessageBox::question(this,
                    tr("Tabla de asientos"),
                    tr("La cuenta '%1' se registra en el libro de facturas (IVA) \n"
                       "pero no está representada en la tabla de asientos\n"
                       "¿ Desea volver a la edición ?")
                      .arg( cuenta_factura ),
                    tr("&Sí"), tr("&No"),
                    QString(), 0, 1 ) )
                  return ;
              }
           }
      }

   if (conanalitica() && !modoconsulta && !basedatos::instancia()->analitica_tabla())
     {
      int veces=0;
      while (veces<ultimafila())
            {
              QString cuenta=ui.Tablaapuntes->item(veces,0)->text();
	      if (escuentadegasto(cuenta) || escuentadeingreso(cuenta))
                  if (ui.Tablaapuntes->item(veces,16)->text().length()==0)
                     {
	              QMessageBox::warning( this, tr("Tabla de apuntes"),
                       tr("ERROR: Faltan códigos de imputación para ingresos/gastos"));
	              return;
                     }
	      veces++;
            }
     }

   if (conanalitica_parc() && !modoconsulta  && !basedatos::instancia()->analitica_tabla() )
     {
      int veces=0;
      while (veces<ultimafila())
            {
              QString cuenta=ui.Tablaapuntes->item(veces,0)->text();
	      if (codigo_en_analitica(cuenta))
                  if (ui.Tablaapuntes->item(veces,16)->text().length()==0)
                     {
	              QMessageBox::warning( this, tr("Tabla de apuntes"),
                       tr("ERROR: Faltan códigos de imputación para ingresos/gastos"));
	              return;
                     }
	      veces++;
            }
     }


   // bool procesar_analitica=false;

   if (basedatos::instancia()->analitica_tabla() &&
       !modoconsulta && (conanalitica() || conanalitica_parc()))
      {
       for (int veces=0;veces<ultimafila();veces++)
         {
          if ((conanalitica() && (escuentadegasto(ui.Tablaapuntes->item(veces,0)->text()) ||
              escuentadeingreso(ui.Tablaapuntes->item(veces,0)->text())))
             || (conanalitica_parc() &&
                 codigo_en_analitica(ui.Tablaapuntes->item(veces,0)->text())))
           {
              if (inputaciones[veces]==0)
               {
                QMessageBox::warning( this, tr("Tabla de apuntes"),
                 // ui.Tablaapuntes->item(veces,0)->text()+
                 tr("ERROR: Las imputaciones no son correctas"));
                return;
               }
             if (!inputaciones[veces]->input_OK())
              {
               QMessageBox::warning( this, tr("Tabla de apuntes"),
                // ui.Tablaapuntes->item(veces,0)->text()+
                tr("ERROR: Las imputaciones no son correctas"));
               return;
              }
           }
         }
      }

    if (fechadejerciciocerrado(ui.FechaApunte->date()) && !modoconsulta)
       {
        QMessageBox::warning( this, tr("Tabla de apuntes"),tr("ERROR: La fecha del asiento corresponde a un ejercicio cerrado"));
        return;
       }
    if (!fechacorrespondeaejercicio(ui.FechaApunte->date()) && !modoconsulta)
      {
       QMessageBox::warning( this, tr("Tabla de apuntes"),tr("ERROR: La fecha del asiento no corresponde a ningún ejercicio definido"));
       return;
      }
    if (basedatos::instancia()->fecha_periodo_bloqueado(ui.FechaApunte->date()) && !modoconsulta)
       {
        QMessageBox::warning( this, tr("Tabla de apuntes"),tr("ERROR: La fecha del asiento corresponde "
                                                              "a un periodo bloqueado"));
        return;
       }
    if (basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(ui.FechaApunte->date())) && !modoconsulta)
       {
        QMessageBox::warning( this, tr("Tabla de apuntes"),tr("ERROR: La fecha del asiento corresponde "
                                                              "a un ejercicio bloqueado"));
        return;
       }

     int veces=0;
     while (veces<ultimafila())
            {
             if (ui.Tablaapuntes->item(veces,0)->text().isEmpty() &&
                 (!ui.Tablaapuntes->item(veces,2)->text().isEmpty()
                 || !ui.Tablaapuntes->item(veces,3)->text().isEmpty()))
               {
                 QMessageBox::warning( this, tr("Tabla de apuntes"),
                                       tr("ERROR: Fila sin contenido en cuenta"));
                 return;
               }
              // comprobamos si hay alguna cuenta bloqueada
             if (basedatos::instancia()->existe_bloqueo_cta(ui.Tablaapuntes->item(veces,0)->text(),
                                                            ui.FechaApunte->date())
                 && !modoconsulta)
                {
                 QMessageBox::warning( this, tr("Tabla de apuntes"),tr("ERROR: La cuenta %1 "
                                               "está bloqueada").arg(ui.Tablaapuntes->item(veces,0)->text()));
                 return;
                }
             veces++;
            }
    // asignar documento a todas las líneas del asiento
    QString documento;
    veces=0;
    bool astoiva=false;
    while (veces<ultimafila())
        {
         if (modoconsulta) break;
         if (ui.Tablaapuntes->item(veces,4)->text().length()>0)
                  documento=ui.Tablaapuntes->item(veces,4)->text();
         if (ui.Tablaapuntes->item(veces,6)->text().length()>0)
            {
             // hay iva asociado en el asiento
             astoiva=true;
            }
         veces++;
        }

   /* if ((astoiva || ui.exentacheckBox->isChecked()) && documento.isEmpty() && !modoconsulta)
       {
        // pedimos documento
        pidenombre p;
        int resultado=0;
        p.asignanombreventana(tr("ASIENTO SIN DOCUMENTO"));
        p.asignaetiqueta(tr("Código documento:"));
        resultado=p.exec();
        if (resultado==QDialog::Accepted)
           {
             if (p.contenido().length()>0) documento=p.contenido();
             for (int veces=0; veces<ultimafila(); veces++)
               {
                 if (ui.Tablaapuntes->item(veces,6)==NULL) continue;
                 if (ui.Tablaapuntes->item(veces,4)==NULL) continue;
                 if (ui.Tablaapuntes->item(veces,6)->text().length()>0)
                    {
                     // hay iva asociado en el asiento
                     ui.Tablaapuntes->item(veces,4)->setText(documento);
                    }
               }
           }
        // delete p;
       }*/
    if (ui.copiadoccheckBox->isChecked() && !modoconsulta)
    {
     veces=0;
     while (veces<ultimafila())
            {
             if (ui.Tablaapuntes->item(veces,4)==NULL) continue;
             if (ui.Tablaapuntes->item(veces,4)->text().isEmpty())
                ui.Tablaapuntes->item(veces,4)->setText(documento);
             veces++;
            } 
     }

    // -------------------------------------------------------------------
    // comprobar documento repetido ************************************
    // si es....
    if (control_docs_repetidos && ui.AsientolineEdit->text().isEmpty())
      {
        QString cad_lista_pagar=basedatos::instancia()->selectCuentasapagarconfiguracion().remove(' ');
        QString cad_lista_cobrar=basedatos::instancia()->selectCuentasacobrarconfiguracion().remove(' ');
        QStringList lista_p=cad_lista_pagar.split(",");
        QStringList lista_c=cad_lista_cobrar.split(",");
       for (int veces=0; veces<numfilas; veces++)
          {
           if (ui.Tablaapuntes->item(veces,4)==NULL) continue;
           if (ui.Tablaapuntes->item(veces,4)->text().isEmpty()) continue;
           for (int veces2=0; veces2<lista_p.count(); veces2++)
             {
               if (ui.Tablaapuntes->item(veces,0)==NULL) continue;
               if (ui.Tablaapuntes->item(veces,0)->text().startsWith(lista_p.at(veces2)))
                 {
                   // comprobar duplicado en cuenta
                   if (basedatos::instancia()->hay_reg_duplicado_diario(
                           ui.Tablaapuntes->item(veces,0)->text(),
                           ui.Tablaapuntes->item(veces,4)->text()))
                     {
                       if (QMessageBox::question(
                              this,
                              tr("Tabla de asientos"),
                              tr("El documento ")+ui.Tablaapuntes->item(veces,4)->text()+
                              tr(" en la cuenta: ")+ui.Tablaapuntes->item(veces,0)->text()+
                              tr(" está repetido. ¿ Desea continuar ?"),
                              tr("&Sí"), tr("&No"),
                              QString(), 0, 1 ) ==1 )
                                            return;
                     }

                 }
             }
           for (int veces2=0; veces2<lista_c.count(); veces2++)
             {
               if (ui.Tablaapuntes->item(veces,0)==NULL) continue;
               if (ui.Tablaapuntes->item(veces,4)==NULL) continue;
               if (ui.Tablaapuntes->item(veces,0)->text().startsWith(lista_c.at(veces2)))
                 {
                   // comprobar duplicado en cuenta
                   if (basedatos::instancia()->hay_reg_duplicado_diario(
                           ui.Tablaapuntes->item(veces,0)->text(),
                           ui.Tablaapuntes->item(veces,4)->text()))
                     {
                       if (QMessageBox::question(
                              this,
                              tr("Tabla de asientos"),
                              tr("El documento ")+ui.Tablaapuntes->item(veces,4)->text()+
                              tr(" en la cuenta: ")+ui.Tablaapuntes->item(veces,0)->text()+
                              tr(" está repetido. ¿ Desea continuar ?"),
                              tr("&Sí"), tr("&No"),
                              QString(), 0, 1 ) ==1 )
                                            return;
                     }
                 }
             }
          }
      } // control docs repetidos

    // procesado de iva por eib si procede
    if (eseib && !eseibautonomo)
    {
       eib *e = new eib(comadecimal,decimales);
       // --------------------------------------------------------------------------
       // pasamos datos a plantilla eib
       e->esconde_externo();
      int fila=0;
      fila=ultimafila()-1;
      QString concepto;
      if (fila>0 && eib_cbase.length()==0)
         {
            QString qcuentabase=ui.Tablaapuntes->item(fila,0)->text();
            QString qctafra=ui.Tablaapuntes->item(fila-1,0)->text();
            QString qbaseimponible=ui.Tablaapuntes->item(fila,3)->text();
            if (qbaseimponible.length()==0) qbaseimponible="-"+ui.Tablaapuntes->item(fila,2)->text();
            concepto=ui.Tablaapuntes->item(fila,1)->text();
            e->pasadatos(qcuentabase,
			qbaseimponible,
			qctafra,
                        ui.FechaApunte->date(),ui.FechaApunte->date(),"");
         }
      if (eib_cbase.length()>0)
            e->pasadatos(eib_cbase,
                        eib_bi,
                        eib_cuenta_fra,
                        eib_fecha_fra,
                        eib_fechaop,
                        eib_claveop);
      if (espr_servicios_ue) e->selec_prservicios();
      // poner en modo consulta eib
      // *********************************************************
      if (modoconsulta) e->modoconsulta();
      int resultado=e->exec();
      if (modoconsulta) return;
      if (resultado==QDialog::Rejected) {delete(e); return; }
      // asignamos campos de IVA
      QString qcuentabase,qbase,qctafra,qclaveop,qprservicios;
      QDate qfechafact, qfechaop;
      e->recuperadatos(&qcuentabase,&qbase,
                        &qctafra,&qfechafact,&qfechaop,
                        &qclaveop,&qprservicios);
      hay_fecha_factura=true;
      fecha_factura=qfechafact;
     // hay que encontrar la cuenta de ctafra en la tabla y asignarles los campos del IVA
      veces=0;
      while (veces<ultimafila())
           { 
	    if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) break;
	    veces++;
           } 
       if (veces==ultimafila()/*ui.Tablaapuntes->item(veces,0)->text()!=qctafra*/)
           {
	    QMessageBox::warning( this, tr("Tabla de apuntes"),tr("ERROR: El código de cuenta factura suministrado no existe en la tabla de asientos"));
	    delete(e);
	    return;
           }
      fila=veces;
     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
     ui.Tablaapuntes->item(fila,6)->setText(qbase);
     ui.Tablaapuntes->item(fila,7)->setText("");
     ui.Tablaapuntes->item(fila,8)->setText("0");
     ui.Tablaapuntes->item(fila,9)->setText("");
     ui.Tablaapuntes->item(fila,10)->setText("0");
     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
     QString caddia,cadmes,cadyear;
     caddia.setNum(qfechafact.day());
     cadmes.setNum(qfechafact.month());
     cadyear.setNum(qfechafact.year());
     ui.Tablaapuntes->item(fila,12)->setText(caddia);
     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
     ui.Tablaapuntes->item(fila,15)->setText("0"); //repercutido
     ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
     ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
     ui.Tablaapuntes->item(fila,27)->setText(qprservicios);
     delete(e);
    }

    if (ui.exentacheckBox->isChecked()) {
        if (!procesa_op_exenta()) return;
    }
    if (ui.exentacheckBox->isChecked() && modoconsulta) return;
    // **********************************************************************************
    // ---------------------------------------------------------------------------------------------
    // añadir aquí procesado de iva por aib si procede
    if (esaib && !esaibautonomo)
      {
       if (!procesa_aib(documento)) return;
      }
    // -------------------------------------------------------------
    if (modoconsulta) return; // no grabamos nada ni salimos
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();
    
    if (ui.AsientoLabel->isEnabled()) incorporaedicion(); // borra asiento antiguo

    veces=0;
    bool recibidas=false;
    while (veces<ultimafila())
        {
         if (ui.Tablaapuntes->item(veces,15)->text()=="1" ||
             ui.Tablaapuntes->item(veces,15)->text()=="AIB0") recibidas=true;
         veces++;
        }

    QString zejercicio=ejerciciodelafecha(ui.FechaApunte->date());

    QString cadnumrecibidas;
    if (recibidas && basedatos::instancia()->hay_secuencia_recibidas())
       {
        if (!ui.numreceplineEdit->text().trimmed().isEmpty())
            cadnumrecibidas=ui.numreceplineEdit->text();
        else
            {
             qlonglong recep=basedatos::instancia()->proximo_nrecepcion(zejercicio);
             cadnumrecibidas.setNum(recep);
             QString cadpaso; cadpaso.setNum(recep+1);
             basedatos::instancia()->pasa_prox_nrecepcion_ejercicio(zejercicio,cadpaso);
            }
       }

    
    basedatos::instancia()->bloquea_para_tabla_asientos();

    qlonglong vnum=0;


    if (ui.AsientoLabel->isEnabled() &&
        guardaejercicio==ejerciciodelafecha(ui.FechaApunte->date()))
        vnum=cadasientoreal.toLongLong();
    else
        {
            vnum = basedatos::instancia()->proximoasiento(zejercicio);
            QString cadnum; cadnum.setNum(vnum+1);
            basedatos::instancia()->update_ejercicio_prox_asiento(zejercicio, cadnum);
         }
    
    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);
    

    if (!ui.AsientoLabel->isEnabled())
        basedatos::instancia()->update_ejercicio_prox_asiento(zejercicio, cadnum);
    
    qlonglong vnum2 = basedatos::instancia()->selectProx_paseconfiguracion();
    
    // QString caddia; caddia.setNum(ui.FechaApunte->date().day());
    // QString cadmes; cadmes.setNum(ui.FechaApunte->date().month());
    // QString cadanyo; cadanyo.setNum(ui.FechaApunte->date().year());
        
    int fila=0;

    QString cadnumpase;
    qlonglong pase=vnum2;

    double ddebe=0, dhaber=0;
    QString qmas, qmenos;
    QString diario;
    pasesvto.clear();
    bool criterio_caja_iva=false;
    QStringList pasesiva;
    QString cadpasefactura, cad_importe_factura;
    while (fila<ultimafila())
      {
        cadnumpase.setNum(pase);
        if (fila==lineafactura) pasefactura=pase;
        // debe
        if (ui.Tablaapuntes->item(fila,2)->text().length()==0) {
            ddebe = 0;
            qmas = "0";
        }
        else {
            ddebe = convapunto(ui.Tablaapuntes->item(fila,2)->text()).toDouble();
            qmas = convapunto(ui.Tablaapuntes->item(fila,2)->text());
        }

        // haber
        if (ui.Tablaapuntes->item(fila,3)->text().length()==0) {
            dhaber = 0;
            qmenos = "0";
        }
        else {
            dhaber = convapunto(ui.Tablaapuntes->item(fila,3)->text()).toDouble();
            qmenos = convapunto(ui.Tablaapuntes->item(fila,3)->text());
        }

        // diario
        if (ui.diariocomboBox->currentText()!=diario_no_asignado())
            diario = ui.diariocomboBox->currentText().left(-1).replace("'","''");

        // procesamos aquí contabilidad analítica
        QString cadproxci="0";
        if (basedatos::instancia()->analitica_tabla())
           {
            if (cuenta_para_analitica(ui.Tablaapuntes->item(fila,0)->text()))
               {
                QStringList lista_ci;
                QStringList lista_total;
                lista_ci=inputaciones[fila]->lista_ci();
                lista_total=inputaciones[fila]->lista_total();
                qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
                cadproxci.setNum(proxnumci);
                for (int veces=0; veces<lista_ci.count(); veces++)
                   {
                    basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                              lista_ci.at(veces),
                                                              convapunto(lista_total.at(veces)));
                   }
                basedatos::instancia()->incrementa_prox_num_ci();
               }
           }
        QString elusuario;
        if (basedatos::instancia()->gestiondeusuarios()) elusuario=usuario;
        QString qexterno=ui.externo_lineEdit->text();
        if (!qexterno.isEmpty())
            if (!basedatos::instancia()->existe_externo(qexterno)) qexterno.clear();
        basedatos::instancia()->insertDiario(cadnumasiento, pase,
             ui.FechaApunte->date().toString("yyyy-MM-dd"), ui.Tablaapuntes->item(fila,0)->text(),
             ddebe, dhaber, ui.Tablaapuntes->item(fila,1)->text(),
             ui.Tablaapuntes->item(fila,4)->text(),
             diario, ui.Tablaapuntes->item(fila,16)->text(),elusuario, cadproxci,
             zejercicio, cadnumrecibidas,hay_fecha_factura, fecha_factura, qexterno,
                                             ui.concepto_sii_lineEdit->text());
        if (basedatos::instancia()->hayvenciasociado(ui.Tablaapuntes->item(fila,0)->text()) ||
           basedatos::instancia()->hay_venci_cta_asociado_ver(ui.Tablaapuntes->item(fila,0)->text()) ||
           (!ui.externo_lineEdit->text().isEmpty() &&
            basedatos::instancia()->hayvenciasociado_externo(ui.externo_lineEdit->text())
            && ui.Tablaapuntes->item(fila,0)->text()==basedatos::instancia()->cuenta_externo(ui.externo_lineEdit->text()) ) )
          {
            pasesvto << cadnumpase;
            cadpasefactura=cadnumpase;
            if (ddebe>-0.001 && ddebe<0.001) cad_importe_factura.setNum(dhaber,'f',2);
            if (dhaber>-0.001 && dhaber<0.001) cad_importe_factura.setNum(ddebe,'f',2);
          }

        basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo( qmenos , qmas , ui.Tablaapuntes->item(fila,0)->text() );

	    // Examinamos columnas para el libroiva
	    if (ui.Tablaapuntes->item(fila,6)->text().length()>0)
	     {
              compro_cuenta=ui.Tablaapuntes->item(fila,11)->text();
              int dia=ui.Tablaapuntes->item(fila,12)->text().toInt();
              int mes=ui.Tablaapuntes->item(fila,13)->text().toInt();
              int anyo=ui.Tablaapuntes->item(fila,14)->text().toInt();
              QDate fechaf(anyo,mes,dia);
              QDate fechaoperacion=QDate::fromString(ui.Tablaapuntes->item(fila,20)->text()
                                          , "yyyy-MM-dd");
              if (ui.Tablaapuntes->item(fila,15)->text() == "AIB0")
                  basedatos::instancia()->insertLibroivaAIB0 ( cadnumpase /*QString pase*/,
                                    ui.Tablaapuntes->item(fila,5)->text() /*QString cta_base_iva*/,
                                    ui.Tablaapuntes->item(fila,6)->text() /*QString base_iva*/,
                                    ui.Tablaapuntes->item(fila,7)->text() /*QString clave_iva*/,
                                    ui.Tablaapuntes->item(fila,8)->text() /*QString tipo_iva*/,
                                    ui.Tablaapuntes->item(fila,9)->text() /*QString tipo_re*/,
                                    ui.Tablaapuntes->item(fila,10)->text() /*QString cuota_iva*/,
                                    ui.Tablaapuntes->item(fila,11)->text() /*QString cuenta_fra*/,
                                    fechaf /*QDate fecha_fra*/,
                                    ui.Tablaapuntes->item(fila,17)->text() /*QString prorrata*/,
                                    (ui.Tablaapuntes->item(fila,18)->text()=="1") /*bool rectificativa*/,
                                    (ui.Tablaapuntes->item(fila,19)->text()=="1") /*bool autofactura*/,
                                    fechaoperacion,
                                    ui.Tablaapuntes->item(fila,21)->text() /*clave operación*/,
                                    ui.Tablaapuntes->item(fila,22)->text(), /*bi a coste*/
                                    ui.Tablaapuntes->item(fila,28)->text()=="1" ? true : false, // autofactura_no_ue
                                    ui.Tablaapuntes->item(fila,29)->text()=="1" ? true : false // bien inversión
                                    ); // AIB o autofactura exentas
                else
                   {
                     if (ui.Tablaapuntes->item(fila,50)->text()=="1")
                        {
                         pasesiva<<cadnumpase; // después de añadir asiento, buscamos registros
                                               // para completar información
                         criterio_caja_iva=true;
                        }
                     basedatos::instancia()->insertLibroiva( cadnumpase ,
                           ui.Tablaapuntes->item(fila,5)->text() /*cta_base_iva*/,
                           ui.Tablaapuntes->item(fila,6)->text() /*base_iva*/ ,
                           ui.Tablaapuntes->item(fila,7)->text() /*clave_iva*/ ,
                           ui.Tablaapuntes->item(fila,8)->text() /*tipo_iva*/ ,
                           ui.Tablaapuntes->item(fila,9)->text() /*tipo_re*/ ,
                           ui.Tablaapuntes->item(fila,10)->text() /*cuota_iva*/ ,
                           ui.Tablaapuntes->item(fila,11)->text() /*cuenta_fra*/ ,
                           fechaf ,
                           (ui.Tablaapuntes->item(fila,15)->text() == "1") /*soportado*/,
                           ui.aibcheckBox->isChecked() /*aib*/ ,
                           ui.eibcheckBox->isChecked() || eib_exterior /*eib*/ ,
                           ui.Tablaapuntes->item(fila,17)->text() /*prorrata*/,
                           (ui.Tablaapuntes->item(fila,18)->text()=="1") /*rectificativa*/,
                           (ui.Tablaapuntes->item(fila,19)->text()=="1") /*autofactura*/ ,
                                    fechaoperacion,
                                    ui.Tablaapuntes->item(fila,21)->text() /*clave operación*/,
                                    convapunto(ui.Tablaapuntes->item(fila,22)->text()), /*bi a coste*/
                           ui.Tablaapuntes->item(fila,23)->text(), // rectificada
                           ui.Tablaapuntes->item(fila,24)->text().toInt(), // número facturas
                           ui.Tablaapuntes->item(fila,25)->text(), // factura inicial
                           ui.Tablaapuntes->item(fila,26)->text(),  // factura final
                           ui.Tablaapuntes->item(fila,28)->text()=="1" ? true : false,  // autofactura_no_ue
                           ui.Tablaapuntes->item(fila,27)->text()=="1" ? true : false, // pr_servicios_ue
                           ui.Tablaapuntes->item(fila,29)->text()=="1" ? true : false, // bien inversión
                           ui.Tablaapuntes->item(fila,30)->text()=="1" ? true : false, // op. no sujeta
                           ui.Tablaapuntes->item(fila,31)->text(), // afección IVA
                           ui.Tablaapuntes->item(fila,32)->text()=="1" ? true : false, // agrario
                           ui.Tablaapuntes->item(fila,33)->text(), // nombre
                           ui.Tablaapuntes->item(fila,34)->text(), // cif
                           ui.Tablaapuntes->item(fila,46)->text()=="1" ? true : false, // importación
                           ui.Tablaapuntes->item(fila,47)->text()=="1" ? true : false, // exportación
                           ui.Tablaapuntes->item(fila,48)->text()=="1" ? true : false, //exenta no ded
                           ui.Tablaapuntes->item(fila,49)->text()=="1" ? true : false,  // isp_interior
                           ui.Tablaapuntes->item(fila,50)->text()=="1" ? true : false,
                           ui.Tablaapuntes->item(fila,51)->text()=="1" ? true : false,
                           ui.Tablaapuntes->item(fila,52)->text()=="1" ? true : false,
                           ui.Tablaapuntes->item(fila,53)->text()=="1" ? true : false,
                           ui.Tablaapuntes->item(fila,54)->text()=="1" ? true : false,
                           ui.Tablaapuntes->item(fila,55)->text()=="1" ? true : false,
                           ui.Tablaapuntes->item(fila,56)->text(), // DUA
                           ui.Tablaapuntes->item(fila,59)->text()  // DATOS DONACIÓN
                           );
                     //QMessageBox::warning( this, tr("Tabla de apuntes"),
                     // tr("GRABA REG"));

                   }
             }
         if (ui.Tablaapuntes->item(fila,35)->text().length()>0)
            {
             basedatos::instancia()->insert_retencion (cadnumpase,
                                    ui.Tablaapuntes->item(fila,35)->text(),
                                    ui.Tablaapuntes->item(fila,36)->text() == "1" ? true : false,
                                    ui.Tablaapuntes->item(fila,37)->text(),
                                    ui.Tablaapuntes->item(fila,38)->text(),
                                    ui.Tablaapuntes->item(fila,39)->text(),
                                    ui.Tablaapuntes->item(fila,40)->text(),
                                    ui.Tablaapuntes->item(fila,41)->text(),
                                    ui.Tablaapuntes->item(fila,42)->text(),
                                    ui.Tablaapuntes->item(fila,43)->text(),
                                    ui.Tablaapuntes->item(fila,44)->text(),
                                    ui.Tablaapuntes->item(fila,45)->text());

            }

	pase++;
	fila++;
       }

    cadnumpase.setNum(pase);
    basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
    // calculaprimerasientoejercicio(ejerciciodelafecha(ui.FechaApunte->date()));

    // procesamos copia documento
    // solo si fichdoclineEdit tiene contenido
    bool gestor_bd=basedatos::instancia()->gestor_doc_bd();
    if (!ui.fichdoclineEdit->text().isEmpty())
       {
        QProgressDialog progreso(tr("Cargando información del documento... "),0, 0, 0);
        // progreso.setWindowModality(Qt::WindowModal);
        progreso.setMinimumDuration ( 0 );
        progreso.setWindowTitle(tr("Cargando información ..."));
        progreso.show();
        progreso.update();
        QApplication::processEvents();
        // primero borramos documento antiguo si hay

        directorio_doc=ui.fichdoclineEdit->text();
        directorio_doc=directorio_doc.left(directorio_doc.lastIndexOf(QDir::separator()));

        QString fichdoc=ui.docasignadolineEdit->text();

        if (gestor_bd) {
            // borramos documento antiguo si hay
            if (!ui.docasignadolineEdit->text().isEmpty()) {
               QString cadnum;
               QString fichdoc=ui.docasignadolineEdit->text();
               if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
                  else cadnum=fichdoc.section('-',0,0);
               qlonglong num=cadnum.toLongLong();
               basedatos::instancia()->borra_doc_gestor(num);
            }

            // construimos nombre de fichero para guardarlo en campo
            qlonglong vnumdoc=basedatos::instancia()->select_prox_doc();
            QString cadnumdoc;
            cadnumdoc.setNum(vnumdoc);

            QString copiadocfich=cadnumdoc.trimmed();
            if (documento.length()>0)
                {
                 copiadocfich+="-";
                 documento.remove(' ');
                 documento.remove('/');
                 documento.remove('\\');
                 documento.remove(':');
                 documento.remove('*');
                 documento.remove('?');
                 documento.remove('|');
                 documento.remove('<');
                 documento.remove('>');
                 copiadocfich+=documento;
                }
            copiadocfich.append('.');
            copiadocfich.append(extensionfich(ui.fichdoclineEdit->text()));

            QFile file(ui.fichdoclineEdit->text());
            if (file.open(QIODevice::ReadOnly)) {
               QByteArray inByteArray=file.readAll();
               basedatos::instancia()->inserta_doc_gestor(vnumdoc, copiadocfich, inByteArray);

               // incrementamos contador de documento
               vnumdoc++;
               cadnumdoc.setNum(vnumdoc);
               basedatos::instancia()->updateConfiguracionprox_documento(cadnumdoc);
               basedatos::instancia()->updatecopia_docdiario(cadnumasiento, copiadocfich,
                                                          ejerciciodelafecha(ui.FechaApunte->date()));
            } else QMessageBox::warning( this, tr("Tabla de apuntes"),tr("No ha sido posible cargar ")+ui.fichdoclineEdit->text());
        }


     if (!gestor_bd) {

        if (!fichdoc.isEmpty())
           {
            fichdoc=adapta(expanderutadocfich(fichdoc));
               // QMessageBox::warning( this, tr("Tabla de apuntes"),fichdoc);
            QFile file(fichdoc);
            file.remove();
            QApplication::processEvents();
           }

        // ----------------------------------------------------------------
        qlonglong vnumdoc=basedatos::instancia()->select_prox_doc();
        QString cadnumdoc;
        cadnumdoc.setNum(vnumdoc);


        // construimos cadena con la ruta y nombre del archivo del documento
        QString caddestino=rutadocs();
        caddestino+=QDir::separator();
        // añadimos nombre de empresa para completar la ruta

        if (nombrebd().length()>0)
          {
           if (!es_sqlite()) caddestino.append(nombrebd());
              else
                   {
                     // el nombre de la base de datos guarda toda la ruta
                     QString nbd;
                     nbd=nombrebd().right(nombrebd().length()-nombrebd().lastIndexOf(QDir::separator())-1);
                     caddestino.append(nbd);
                   }
          }
        QString caddestinoprev;
        //if (eswindows()) caddestinoprev=QFile::encodeName(caddestino);
        //   else caddestinoprev=caddestino;
        caddestinoprev=caddestino;
        QDir d(caddestinoprev);
        if (!d.exists())
            {
             d.mkdir(caddestinoprev);
             fijapermisos(caddestinoprev, "0770");
            }
        QApplication::processEvents();
        int numdir=(vnumdoc/100+1)*100;
        QString cadnumdir;
        cadnumdir.setNum(numdir);
        cadnumdir=cadnumdir.trimmed();
        caddestino+=QDir::separator();
        caddestino+=cadnumdir;
        //if (eswindows()) caddestinoprev=QFile::encodeName(caddestino);
        //   else caddestinoprev=caddestino;
        caddestinoprev=caddestino;
        d.setPath(caddestinoprev);
        if (!d.exists())
           {
            d.mkdir(caddestinoprev);
            fijapermisos(caddestinoprev, "0770");
           }
        QApplication::processEvents();

        caddestino+=QDir::separator();
        QString copiadocfich=cadnumdoc.trimmed();
        if (documento.length()>0)
            {
             copiadocfich+="-";
             documento.remove(' ');
             documento.remove('/');
             documento.remove('\\');
             documento.remove(':');
             documento.remove('*');
             documento.remove('?');
             documento.remove('|');
             documento.remove('<');
             documento.remove('>');
             copiadocfich+=documento;
            }
        copiadocfich.append('.');
        copiadocfich.append(extensionfich(ui.fichdoclineEdit->text()));
        caddestino.append(copiadocfich);
        bool copiado=false;
        // copiamos documento y actualizamos campo del diario
        if (eswindows())
           {
            QFile fichero(ui.fichdoclineEdit->text());
            copiado=fichero.copy(caddestino);
           }
          else
              {
               QFile fichero(ui.fichdoclineEdit->text());
               copiado=fichero.copy(caddestino);
               fijapermisos(caddestino, "0770");
              }
          if (!copiado)
            {
               QMessageBox::warning( this, tr("Asignar documento"),
                                     tr("ERROR: No se ha podido copiar archivo")+ui.fichdoclineEdit->text());

          } else {
                  // incrementamos contador de documento no
                  vnumdoc++;
                  cadnumdoc.setNum(vnumdoc);
                  basedatos::instancia()->updateConfiguracionprox_documento(cadnumdoc);
                  basedatos::instancia()->updatecopia_docdiario(cadnumasiento, copiadocfich,
                                                      ejerciciodelafecha(ui.FechaApunte->date()));
          }
       }
    }

    // QMessageBox::warning( this, tr("Tabla de apuntes"),"stop");

    if (ui.fichdoclineEdit->text().isEmpty() && !ui.docasignadolineEdit->text().isEmpty())
       {
        // tenemos que comprobar si ha cambiado el documento
        // tendríamos que separar el documento de la cadena del archivo
        // y comprobar que sea distinta al documento del asiento
        // cadena entre primer guión y último punto
        int indexinit=ui.docasignadolineEdit->text().indexOf('-');
        int indexfin=ui.docasignadolineEdit->text().lastIndexOf('.');
        QString eldoc=ui.docasignadolineEdit->text().mid(indexinit,indexfin-indexinit+1);
        if (indexinit<0) eldoc.clear();
        // ha cambiado el documento ?
        if (documento.length()>0)
            {
             documento.remove(' ');
             documento.remove('/');
             documento.remove('\\');
             documento.remove(':');
             documento.remove('*');
             documento.remove('?');
             documento.remove('|');
             documento.remove('<');
             documento.remove('>');
            }

        if (eldoc!=documento.remove(' '))
           {
            // construimos nuevo fichero documento y borramos el antiguo

            QString fichero=adapta(expanderutadocfich(ui.docasignadolineEdit->text()));
            // QMessageBox::warning( this, tr("Tabla de apuntes"),fichero);
            QFile file(fichero);
            // file.remove();
            // creamos el nuevo fichero con el nuevo documento
            QString nuevofich=ui.docasignadolineEdit->text().left(indexinit);
            if (indexinit<0) nuevofich=ui.docasignadolineEdit->text().left(indexfin);
            nuevofich+="-";
            nuevofich+=documento.remove(' ');
            nuevofich+=ui.docasignadolineEdit->text().mid(indexfin);
            QString nombrefich=nuevofich;

            if (gestor_bd) {
                QString cadnum;
                QString elfich=ui.docasignadolineEdit->text();
                if (!elfich.contains('-')) cadnum=elfich.section('.',0,0);
                    else cadnum=elfich.section('-',0,0);
                qlonglong num=cadnum.toLongLong();
                basedatos::instancia()->actualiza_nombrefich_doc_gestor(num, nombrefich);
            }
            else {
              nuevofich=expanderutadocfich(nuevofich);
              // QMessageBox::warning( this, tr("Tabla de apuntes"),nuevofich);
              if (eswindows()) file.rename(nuevofich);
              else { file.rename(nuevofich); fijapermisos(nuevofich, "0770");}
              // file.remove();
              // QMessageBox::warning( this, tr("Tabla de apuntes"),nuevofich);
            }
            basedatos::instancia()->updatecopia_docdiario(cadnumasiento, nombrefich,
                                                          ejerciciodelafecha(ui.FechaApunte->date()));
           }
          else
              basedatos::instancia()->updatecopia_docdiario(cadnumasiento,
                                                            ui.docasignadolineEdit->text(),
                                                            ejerciciodelafecha(ui.FechaApunte->date()));
       }


    basedatos::instancia()->desbloquea_y_commit();

    if (basedatos::instancia()->tipo_proveedor_config()) {
      // comprobamos si homologado en prueba
      QSqlQuery q = basedatos::instancia()->select_datos_tipo_proveedor_cta(compro_cuenta);
      // QMessageBox::information( this, tr("Tabla de apuntes"),compro_cuenta);
      if (q.isActive())
        if (q.next()) {
            if (q.value(0).toString()==basedatos::instancia()->cod_homol_pruebas_config().trimmed()) {
                // contamos número de facturas de ese proveedor en libroiva
                QSqlQuery q2 = basedatos::instancia()->asientos_cuenta_libroiva(compro_cuenta);
                int num_facturas=0;
                if (q2.isActive())
                    while (q2.next()) num_facturas++;
                if (num_facturas == 3)
                    QMessageBox::information( this, tr("Tabla de apuntes"),tr("Cuenta homologada en pruebas. \n"
                                                                              "Hay tres facturas contabilizadas,\n"
                                                                              "La cuenta debe de pasar a homologada"));
            }
        }
     }


    // procesamos vencimientos asociados: lista  pasesvto


    for (int veces=0;veces<pasesvto.count();veces++)
        procesavenci( pasesvto.at(veces) );

    // asignamos campos a libroiva si hay criterio de caja
    if (criterio_caja_iva)
      {
        for (int veces=0; veces<pasesiva.count(); veces++)
           {
            if (cad_importe_factura.isEmpty())
               {
                QMessageBox::warning( this, tr("Tabla de apuntes"),
                                      tr("ERROR: No hay vencimientos predefinidos para el criterio de caja en IVA"));
                break;
               }
            basedatos::instancia()->actu_libro_iva(pasesiva.at(veces), cadpasefactura,convapunto(cad_importe_factura));
            // buscar pase libroiva y meterle pasefactura y cad_importe_factura
           }
      }
    // -------------------------------------------------------

    // variable global
    copiardoc=ui.copiadoccheckBox->isChecked();
    // -------------------------------------------------------
    if (nocerrar)
       {
        QString mensaje=tr("Asiento: ");
        mensaje+=cadnumasiento;
        if (!cadnumrecibidas.isEmpty())
           {
            mensaje+=tr(" - Número de recepción: ") ;
            mensaje+=cadnumrecibidas;
           }
        QMessageBox::information( this, tr("Tabla de asientos"),mensaje);
        // borratodo(false);
        // ui.Tablaapuntes->setFocus();
        accept();
       }
      else
         {
          if (!cadnumrecibidas.isEmpty() && ui.AsientolineEdit->text().isEmpty())
              QMessageBox::information( this, tr("Tabla de asientos"),
                                        tr("Asiento: %1").arg(cadnumasiento) +
                                        tr(" - Número de recepción: %1").arg(cadnumrecibidas));

          accept();
         }
}

void tabla_asientos::procesavenci(QString pase)
{
 /*  QString num,
   QString cta_ordenante,
   QDate fecha_operacion,
   QString importedebe,importehaber,
   QString cuenta_tesoreria,
   QDate fecha_vencimiento,
   bool derecho,
   bool pendiente,
   bool anulado,
   QString pase_diario_operacion,
   QString pase_diario_vencimiento,
   QString concepto */
   if (sinvencimientos) return;
   QString cta_ordenante;
   QDate fecha_operacion;
   QString importedebe,importehaber;
   QString documento;
   if (basedatos::instancia()->cargadatospase(pase,cta_ordenante,fecha_operacion,
                                              importedebe,importehaber,documento))
      {
       if (!basedatos::instancia()->pase_en_vencimientos(pase))
         {
           // añadimos registros de pases de vencimientos
           // primero vemos qué vencimientos tiene asignados la cuenta ordenante
           QString cadena;
           int vto_dia_fijo;
           // bool asoc_auxiliar=false;
           bool procesavenci=false;
           bool domiciliable=false;
           QString cta_tesoreria;

           if (!ui.externo_lineEdit->text().isEmpty() &&
                   basedatos::instancia()->hayvenciasociado_externo(ui.externo_lineEdit->text()))
              {
               cadena=basedatos::instancia()->cadvenci_externo(ui.externo_lineEdit->text());
               vto_dia_fijo=basedatos::instancia()->dia_fijo_vto_externo(ui.externo_lineEdit->text());
               cta_tesoreria=basedatos::instancia()->cta_tesoreria_venci_externo(ui.externo_lineEdit->text());
               procesavenci=basedatos::instancia()->hayprocvenci_externo(ui.externo_lineEdit->text());
               if (cta_tesoreria.isEmpty()) cadena.clear();
              }

           if (cadena.isEmpty())
             {
              if (basedatos::instancia()->hayvenciasociado(cta_ordenante)
                && ! basedatos::instancia()->cta_tesoreria_venci(cta_ordenante).isEmpty())
               {
                cadena=basedatos::instancia()->cadvenci_subcuenta(cta_ordenante);
                vto_dia_fijo=basedatos::instancia()->dia_fijo_vto_subcuenta(cta_ordenante);
                cta_tesoreria=basedatos::instancia()->cta_tesoreria_venci(cta_ordenante);
                procesavenci=basedatos::instancia()->hayprocvenci(cta_ordenante);
                // asoc_auxiliar=true;
               }
             }
           // QMessageBox::warning( this, tr("Tabla de apuntes"),"contenido:"+cadena);

            if (cadena.isEmpty())
              basedatos::instancia()->hay_venci_cta_asociado(cta_ordenante,
                                                              &cadena,
                                                              &cta_tesoreria, &procesavenci,
                                                              &vto_dia_fijo);
           if (!cadena.isEmpty())
            {
             domiciliable=false;
             if (!ui.externo_lineEdit->text().isEmpty())
                 domiciliable=basedatos::instancia()->externo_dom_bancaria(ui.externo_lineEdit->text());
               else
                  domiciliable=basedatos::instancia()->cuenta_dom_bancaria(cta_ordenante);
             if (forzarcontado) cadena="0";
             int numregistros=cadena.count(',')+1;

            double importe=importedebe.toDouble();
            if (importe>-0.001 && importe <0.001)
              importe=importehaber.toDouble()*-1;
            double importetotal=importe;
            importe=importe/numregistros;
            bool derecho=true;
            if (importe<0) { importe=importe*-1; importetotal=importetotal*-1; derecho=false; }
            QString cadimporte;
            cadimporte.setNum(importe,'f',2);
            double arrastre=0;
            double resto=0;
            QString cadnumregistros; cadnumregistros.setNum(numregistros);
            for (int veces=0;veces<numregistros;veces++)
              {
               QString cadvez; cadvez.setNum(veces+1);
               if (veces==numregistros-1) // estamos en el último registro
                {
                  resto=importetotal-arrastre;
                  cadimporte.setNum(resto,'f',2);
                }
                 else
                  arrastre+=cadimporte.toDouble();
               // QMessageBox::warning( this, tr("Tabla de apuntes"),cadena.section(',',veces,veces));
               int dias=cadena.section(',',veces,veces).toInt();
               QSqlDatabase::database().transaction();
               basedatos::instancia()->bloquea_para_nuevo_vencimiento();
               qlonglong vnum = basedatos::instancia()->proximovencimiento();
               QString cadnum; cadnum.setNum(vnum+1);
               // ***********************************************************************
               QDate fechavenci;
               fechavenci=fecha_operacion;
               if (hay_fecha_factura) fechavenci=fecha_factura;
               // QMessageBox::warning( this, tr("Tabla de apuntes"),
               //                      fecha_factura.toString("dd-MM-yyyy"));
               fechavenci=fechavenci.addDays(dias);
               int mesvenci=fechavenci.month();
               int anyovenci=fechavenci.year();
               if (vto_dia_fijo>0)
                  {
                    if (mesvenci==2 && vto_dia_fijo>28) vto_dia_fijo=28;
                    QList<int> meses30; meses30 << 11 << 4 << 6 << 9;
                    if (meses30.contains(mesvenci) && vto_dia_fijo>30 )
                        vto_dia_fijo=30;
                  }
               if (fechavenci.day()<vto_dia_fijo && vto_dia_fijo>0)
                   fechavenci.setDate(anyovenci,mesvenci,vto_dia_fijo);
               if (fechavenci.day()>vto_dia_fijo && vto_dia_fijo>0)
                  {
                   fechavenci=fechavenci.addMonths(1) ;
                   mesvenci=fechavenci.month();
                   anyovenci=fechavenci.year();
                   fechavenci.setDate(anyovenci,mesvenci,vto_dia_fijo);
                  }
               QString cadnumvto; cadnumvto.setNum(vnum);
               basedatos::instancia()->updateConfiguracionprox_vencimiento(cadnum);
               // QString num, QString cta_ordenante, QDate fecha_operacion, QString importe,
               // QString cuenta_tesoreria, QDate fecha_vencimiento, bool derecho, bool pendiente,
               // bool anulado, QString pase_diario_operacion, QString pase_diario_vencimiento,
               // QString concepto
               QString concepto;
               if (derecho) concepto=basedatos::instancia()->msj_vto_dcho();
                    else concepto=basedatos::instancia()->msj_vto_obl();
               if (concepto.isEmpty())
                 concepto=tr("Vencimiento: ");
                else concepto+=" ";

               concepto+=documento;
               concepto+=" ";
               concepto+=cadvez+"/"+cadnumregistros;
               if (!domiciliable)
                  {
                   if (ui.descrip_externo_lineEdit->text().isEmpty())
                     concepto+=" "+descripcioncuenta(cta_ordenante);
                    else
                       concepto+=ui.descrip_externo_lineEdit->text();
                  }
               basedatos::instancia()->insertVencimientos ( cadnumvto , cta_ordenante,
                                                            fecha_operacion ,
                                                            cadimporte ,
                                                            cta_tesoreria,
                                                            fechavenci,
                                                            derecho,
                                                            true , false , pase, "0",
                                                            concepto, domiciliable,
                                                            ui.externo_lineEdit->text());
               basedatos::instancia()->desbloquea_y_commit();
              }


            // StandardButton QMessageBox::question ( QWidget * parent,
            // const QString & title, const QString & text,
            // StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )

            int resultado=0;

            if (!evitar_pregunta_venci)
                if (espredefinido || ui.ed_venci_checkBox->isChecked())
              resultado=QMessageBox::question(this,tr("Tabla de asientos"),
                   tr("¿ Son correctos los vencimientos asociados \n"
                      "automáticamente a la cuenta %1 ?").arg(cta_ordenante),
                   tr("&Sí"),
                   tr("&Editar"),tr("&Descartar vencimientos"));

            if (resultado==1)
              {
                // Editamos vencimientos
                // accedemos a la edición de vencimientos
                // consultavencipase(QString qpase, QString asiento, QString debe,
                // QString haber, QString qcuentadiario,QDate qfechadiario);
                consultavencipase *losvenci=new consultavencipase(pase,
                           basedatos::instancia()->asientodepase(pase),
                           importedebe,
                           importehaber,
                           cta_ordenante,
                           fecha_operacion);
               losvenci->exec();
               delete(losvenci);
              }

             if (resultado==2)
              {
                 // Borramos = descartamos vencimientos asignados
                 basedatos::instancia()->borra_venci_pase_diario_operacion(pase);
                 QMessageBox::information( this, tr("Tabla asientos"),
                                           "Se han borrado los vencimientos asignados");
              }
           }
           // comprobamos si hay procesamiento inmediato del pago
           if (procesavenci)
             {
               QSqlQuery query = basedatos::instancia()->select9Vencimientospase_diario_operacion(pase);
               if ( (query.isActive()) && (query.first()) )
                  {
                    if (query.value(8).toBool()) // PENDIENTE
                       {
                        procesavencimiento *procvenci=new procesavencimiento(usuario);
                        procvenci->cargadatos(query.value(0).toString());
                        int resultado=procvenci->exec();
                        if (resultado!=QDialog::Accepted)
                         {
                          QMessageBox::information( this, tr("Vencimientos"),tr("El procesado del vencimiento se ha cancelado"));
                         }
                        delete procvenci;
                       }
                  }
             }
         }
      }

}

void tabla_asientos::aibcambiado()
{
  if (ui.aibcheckBox->isChecked())
    {
      ui.eibcheckBox->setChecked(false);
      ui.exentacheckBox->setChecked(false);
      ui.regIVAcheckBox->setChecked(false);
      eseib=false;
      preparaaib();
    }
   else 
           {
             esaib=false;
             setWindowTitle(tr("tabla apuntes"));
           }
}


void tabla_asientos::eibcambiado()
{
  if (ui.eibcheckBox->isChecked())
    {
      ui.aibcheckBox->setChecked(false);
      ui.exentacheckBox->setChecked(false);
      ui.regIVAcheckBox->setChecked(false);
      esaib=false;
      preparaeib();
    }
  else
           {
             eseib=false;
             setWindowTitle(tr("tabla apuntes"));
           }
}

void tabla_asientos::exentacambiado()
{
  if (ui.exentacheckBox->isChecked())
    {
      ui.aibcheckBox->setChecked(false);
      ui.eibcheckBox->setChecked(false);
      ui.regIVAcheckBox->setChecked(false);
      esaib=false;
      eseib=false;
    }
}


void tabla_asientos::regIVAcambiado()
{
  if (ui.regIVAcheckBox->isChecked())
    {
      ui.aibcheckBox->setChecked(false);
      ui.eibcheckBox->setChecked(false);
      ui.exentacheckBox->setChecked(false);
      esaib=false;
      eseib=false;
    }
}


void tabla_asientos::incorporaedicion()
{
    QString cadnumasiento=cadasientoreal;
    basedatos::instancia()->borraasientosnotransac_no_copiadoc(cadnumasiento,cadnumasiento,
                                                               guardaejercicio);
                             // ejerciciodelafecha(ui.FechaApunte->date()));
}


void tabla_asientos::preparaaib()
{
  esaib=true;
  setWindowTitle(tr("tabla apuntes: AIB / Autofactura"));
  ui.aibcheckBox->setChecked(true);
  ui.eibcheckBox->setChecked(false);
}

void tabla_asientos::preparaautofactura()
{
  esautofactura=true;
  setWindowTitle(tr("tabla apuntes: AIB / Autofactura"));
  ui.aibcheckBox->setChecked(true);
  ui.eibcheckBox->setChecked(false);
}

void tabla_asientos::prepara_isp_op_interiores()
{
  es_isp_op_interiores=true;
  setWindowTitle(tr("tabla apuntes: AIB / Autofactura"));
  ui.aibcheckBox->setChecked(true);
  ui.eibcheckBox->setChecked(false);
}


void tabla_asientos::preparaautofactura_no_ue()
{
  esautofactura_no_ue=true;
  setWindowTitle(tr("tabla apuntes: AIB / Autofactura"));
  ui.aibcheckBox->setChecked(true);
  ui.eibcheckBox->setChecked(false);
}

void tabla_asientos::preparapr_servicios_ue()
{
  espr_servicios_ue=true;
  setWindowTitle(tr("tabla apuntes: EIB / Prestación servicios UE"));
  ui.aibcheckBox->setChecked(false);
  ui.eibcheckBox->setChecked(true);
}

void tabla_asientos::preparaeib()
{
  eseib=true;
  setWindowTitle(tr("tabla apuntes: Entrega intracomunitaria de bienes"));
  ui.aibcheckBox->setChecked(false);
  ui.eibcheckBox->setChecked(true);
}

void tabla_asientos::pasanumasiento( QString  cadasiento)
{
  ui.AsientolineEdit->setText(cadasiento);
  cadasientoreal=cadasiento;
}


void tabla_asientos::botonesperapulsado()
{
    QSqlDatabase contabilidad=QSqlDatabase::database();
    contabilidad.transaction();

    basedatos::instancia()->bloquea_para_borrador();

    int vnum = basedatos::instancia()->selectMaxasientoborrador();

    QString cadnum; cadnum.setNum(vnum);
    int fila=0;
    while (fila<ultimafila())
    {
        basedatos::instancia()->insertBorrador(cadnum /*asiento*/,
                           ui.Tablaapuntes->item(fila,0)->text() /*cuenta*/,
                           ui.Tablaapuntes->item(fila,1)->text() /*concepto*/,
                           ui.Tablaapuntes->item(fila,2)->text() /*debe*/,
                           ui.Tablaapuntes->item(fila,3)->text() /*haber*/,
                           ui.Tablaapuntes->item(fila,4)->text() /*documento*/,
                           ui.Tablaapuntes->item(fila,5)->text() /*ctabaseiva*/,
                           ui.Tablaapuntes->item(fila,6)->text() /*baseiva*/,
                           ui.Tablaapuntes->item(fila,7)->text() /*claveiva*/,
                           ui.Tablaapuntes->item(fila,8)->text() /*tipoiva*/,
                           ui.Tablaapuntes->item(fila,9)->text() /*tipore*/,
                           ui.Tablaapuntes->item(fila,10)->text() /*cuotaiva*/,
                           ui.Tablaapuntes->item(fila,11)->text() /*cuentafra*/,
                           ui.Tablaapuntes->item(fila,12)->text() /*diafra*/,
                           ui.Tablaapuntes->item(fila,13)->text() /*mesfra*/,
                           ui.Tablaapuntes->item(fila,14)->text() /*anyofra*/,
                           ui.Tablaapuntes->item(fila,15)->text() /*soportado*/,
                           ui.Tablaapuntes->item(fila,16)->text() /*ci*/,
                           ui.Tablaapuntes->item(fila,17)->text() /*prorrata*/,
                           ui.Tablaapuntes->item(fila,18)->text() /*rectificativa*/,
                           ui.Tablaapuntes->item(fila,19)->text() /*autofactura*/,
                           ui.Tablaapuntes->item(fila,20)->text() /*fecha operación*/,
                           ui.Tablaapuntes->item(fila,21)->text() /*clave operación*/,
                           ui.Tablaapuntes->item(fila,22)->text() /*bi a coste*/,
                           ui.Tablaapuntes->item(fila,23)->text() /*rectificada*/,
                           ui.Tablaapuntes->item(fila,24)->text() /*nfacturas*/,
                           ui.Tablaapuntes->item(fila,25)->text() /*finicial*/,
                           ui.Tablaapuntes->item(fila,26)->text() /*ffinal*/,
                           ui.Tablaapuntes->item(fila,27)->text(), //prestación servicio ue
                           ui.Tablaapuntes->item(fila,28)->text(), //autofactura no ue
                           ui.Tablaapuntes->item(fila,29)->text(), //bien_inversion
                           ui.Tablaapuntes->item(fila,30)->text(), //no sujeta
                           ui.Tablaapuntes->item(fila,31)->text(), //afección
                           ui.Tablaapuntes->item(fila,32)->text(),
                           ui.Tablaapuntes->item(fila,33)->text(),
                           ui.Tablaapuntes->item(fila,34)->text(),
                           ui.Tablaapuntes->item(fila,35)->text(),
                           ui.Tablaapuntes->item(fila,36)->text(),
                           ui.Tablaapuntes->item(fila,37)->text(),
                           ui.Tablaapuntes->item(fila,38)->text(),
                           ui.Tablaapuntes->item(fila,39)->text(),
                           ui.Tablaapuntes->item(fila,40)->text(),
                           ui.Tablaapuntes->item(fila,41)->text(),
                           ui.Tablaapuntes->item(fila,42)->text(),
                           ui.Tablaapuntes->item(fila,43)->text(),
                           ui.Tablaapuntes->item(fila,44)->text(),
                           ui.Tablaapuntes->item(fila,45)->text(),
                           ui.Tablaapuntes->item(fila,46)->text(),
                           ui.Tablaapuntes->item(fila,47)->text(),
                           ui.Tablaapuntes->item(fila,48)->text(),
                           ui.Tablaapuntes->item(fila,49)->text(),
                           ui.Tablaapuntes->item(fila,50)->text(),
                           ui.Tablaapuntes->item(fila,51)->text(),
                           ui.Tablaapuntes->item(fila,52)->text(),
                           ui.Tablaapuntes->item(fila,53)->text(),
                           ui.Tablaapuntes->item(fila,54)->text(),
                           ui.Tablaapuntes->item(fila,55)->text(),
                           ui.Tablaapuntes->item(fila,56)->text(),
                           ui.Tablaapuntes->item(fila,57)->text(),
                           ui.Tablaapuntes->item(fila,58)->text(),
                           ui.externo_lineEdit->text(),
                           ui.concepto_sii_lineEdit->text(),
                           usuario,
                           ui.FechaApunte->date()
                          );
        fila++;
    }
    basedatos::instancia()->desbloquea_y_commit();
    borratodo(true);
}


void tabla_asientos::pegar()
{
    if (ui.Tablaapuntes->currentColumn()==0) return;
    QClipboard *cb = QApplication::clipboard();
    QString texto = cb->text(QClipboard::Clipboard);
    if ( !texto.isNull() && texto.length()<80)
    {
        if (ui.Tablaapuntes->currentColumn()==2 || ui.Tablaapuntes->currentColumn()==3)
        {
            if (comadecimal) texto=convacoma(texto);
        }

         if (ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow(),ui.Tablaapuntes->currentColumn())==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (ui.Tablaapuntes->currentColumn()==2 || ui.Tablaapuntes->currentColumn()==3)
                 newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(ui.Tablaapuntes->currentRow(),ui.Tablaapuntes->currentColumn(),newItem);
             }


        ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow(),ui.Tablaapuntes->currentColumn())->setText(texto);
        // valueChanged(Tablaapuntes->currentRow(),Tablaapuntes->currentColumn());
    }
}

void tabla_asientos::mayor()
{
    if (ui.CodigoEd->text().length()>0)
    {
        QString subcuenta="";
        QDate fecha;
        consmayor *elmayor=new consmayor(comadecimal,decimales,usuario);
        subcuenta=ui.CodigoEd->text();
        fecha=ui.FechaApunte->date();
        elmayor->pasadatos(subcuenta,fecha);
        elmayor->exec();
        delete(elmayor);
    }
}

// pasa los datos a la tabla sin ninguna transformación para cargar borrador
void tabla_asientos::pasadatos( int fila, QString col0, QString col1, QString col2, QString col3,
                                QString col4, QString col5, QString col6, QString col7,
                                QString col8, QString col9, QString col10, QString col11,
                                QString col12, QString col13, QString col14, QString col15,
                                QString col16, QString col17,QString col18,QString col19,
                                QString col20, QString col21, QString col22,
                                QString col23, QString col24,QString col25,QString col26,
                                QString col27, QString col28, QString col29,
                                QString col30, QString col31,
                                QString col32, QString col33, QString col34, QString col35,
                                QString col36, QString col37, QString col38, QString col39,
                                QString col40, QString col41, QString col42, QString col43,
                                QString col44, QString col45, QString col46, QString col47,
                                QString col48, QString col49, QString col50,
                                QString col51, QString col52, QString col53, QString col54,
                                QString col55, QString col56, QString col57, QString col58,
                                QString externo, QString concepto_sii
                                )
{
    for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila,veces,newItem);
             }

  ui.Tablaapuntes->item(fila,0)->setText(col0);
  ui.Tablaapuntes->item(fila,1)->setText(col1);

  QString cadnum;

  double num=convapunto(col2).toDouble();
  if (num<-0.001 || num>0.001)
     {
      cadnum=formateanumero(num,comadecimal,decimales);
      ui.Tablaapuntes->item(fila,2)->setText(cadnum);
     }
  num=convapunto(col3).toDouble();
  if (num<-0.001 || num>0.001)
     {
      cadnum=formateanumero(num,comadecimal,decimales);
      ui.Tablaapuntes->item(fila,3)->setText(cadnum);
     }
  ui.Tablaapuntes->item(fila,4)->setText(col4);
  ui.Tablaapuntes->item(fila,5)->setText(col5);

  num=convapunto(col6).toDouble();
  if (num<-0.001 || num>0.001)
      {
       cadnum=formateanumero(num,comadecimal,decimales);
       ui.Tablaapuntes->item(fila,6)->setText(cadnum);
      }
  ui.Tablaapuntes->item(fila,7)->setText(col7);

  num=convapunto(col8).toDouble();
  cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,8)->setText(cadnum);

  num=convapunto(col9).toDouble();
  cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,9)->setText(cadnum);

  num=convapunto(col10).toDouble();
  cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,10)->setText(cadnum);

  ui.Tablaapuntes->item(fila,11)->setText(col11);
  ui.Tablaapuntes->item(fila,12)->setText(col12);
  ui.Tablaapuntes->item(fila,13)->setText(col13);
  ui.Tablaapuntes->item(fila,14)->setText(col14);
  ui.Tablaapuntes->item(fila,15)->setText(col15);
  ui.Tablaapuntes->item(fila,16)->setText(col16);
  ui.Tablaapuntes->item(fila,17)->setText(col17);
  ui.Tablaapuntes->item(fila,18)->setText(col18);
  ui.Tablaapuntes->item(fila,19)->setText(col19);
  ui.Tablaapuntes->item(fila,20)->setText(col20);
  ui.Tablaapuntes->item(fila,21)->setText(col21);
  ui.Tablaapuntes->item(fila,22)->setText(col22);

  ui.Tablaapuntes->item(fila,23)->setText(col23);
  ui.Tablaapuntes->item(fila,24)->setText(col24);
  ui.Tablaapuntes->item(fila,25)->setText(col25);
  ui.Tablaapuntes->item(fila,26)->setText(col26);

  ui.Tablaapuntes->item(fila,27)->setText(col27);
  ui.Tablaapuntes->item(fila,28)->setText(col28);
  ui.Tablaapuntes->item(fila,29)->setText(col29);

  ui.Tablaapuntes->item(fila,30)->setText(col30);
  ui.Tablaapuntes->item(fila,31)->setText(col31);

  ui.Tablaapuntes->item(fila,32)->setText(col32);
  ui.Tablaapuntes->item(fila,33)->setText(col33);
  ui.Tablaapuntes->item(fila,34)->setText(col34);
  ui.Tablaapuntes->item(fila,35)->setText(col35);
  ui.Tablaapuntes->item(fila,36)->setText(col36);
  ui.Tablaapuntes->item(fila,37)->setText(col37);
  ui.Tablaapuntes->item(fila,38)->setText(col38);
  ui.Tablaapuntes->item(fila,39)->setText(col39);
  ui.Tablaapuntes->item(fila,40)->setText(col40);
  ui.Tablaapuntes->item(fila,41)->setText(col41);
  ui.Tablaapuntes->item(fila,42)->setText(col42);
  ui.Tablaapuntes->item(fila,43)->setText(col43);
  ui.Tablaapuntes->item(fila,44)->setText(col44);
  ui.Tablaapuntes->item(fila,45)->setText(col45);
  ui.Tablaapuntes->item(fila,46)->setText(col46);
  ui.Tablaapuntes->item(fila,47)->setText(col47);
  ui.Tablaapuntes->item(fila,48)->setText(col48);
  ui.Tablaapuntes->item(fila,49)->setText(col49);
  ui.Tablaapuntes->item(fila,50)->setText(col50);

  ui.Tablaapuntes->item(fila,51)->setText(col51);
  ui.Tablaapuntes->item(fila,52)->setText(col52);
  ui.Tablaapuntes->item(fila,53)->setText(col53);
  ui.Tablaapuntes->item(fila,54)->setText(col54);
  ui.Tablaapuntes->item(fila,55)->setText(col55);
  ui.Tablaapuntes->item(fila,56)->setText(col56);
  ui.Tablaapuntes->item(fila,57)->setText(col57);
  ui.Tablaapuntes->item(fila,58)->setText(col58);

  ui.externo_lineEdit->setText(externo);
  ui.concepto_sii_lineEdit->setText(concepto_sii);
}


void tabla_asientos::pasafechaasiento( QDate lafecha)
{
 ui.FechaApunte->setDate(lafecha);

 if (!ui.AsientolineEdit->text().isEmpty())
     guardaejercicio=ejerciciodelafecha(lafecha);
}

void tabla_asientos::pasanocerrar(bool noclose)
{
  nocerrar=noclose;
}

void tabla_asientos::pasadiario(QString qdiario)
{
 for (int veces=0; veces < ui.diariocomboBox->count(); veces++)
     {
       if (ui.diariocomboBox->itemText(veces)==qdiario) ui.diariocomboBox->setCurrentIndex(veces);
     }
}


void tabla_asientos::pasadatos1( int fila, QString col0, QString col1, QString col2,
                                 QString col3, QString col4, QString col16)
{

    for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila,veces,newItem);
             }

  ui.Tablaapuntes->item(fila,0)->setText(col0);
  ui.Tablaapuntes->item(fila,1)->setText(col1);

 QString cadnum;
  double num=convapunto(col2).toDouble();
  if (num<-0.001 || num>0.001)
      {
       cadnum=formateanumero(num,comadecimal,decimales);
       ui.Tablaapuntes->item(fila,2)->setText(cadnum);
      }
  num=convapunto(col3).toDouble();
  if (num<-0.001 || num>0.001)
      {
       cadnum=formateanumero(num,comadecimal,decimales);
       ui.Tablaapuntes->item(fila,3)->setText(cadnum);
      }
  ui.Tablaapuntes->item(fila,4)->setText(col4);

  ui.Tablaapuntes->item(fila,16)->setText(col16);

}


void tabla_asientos::pasadatos2(int fila, QString col5, QString col6, QString col7, QString col8,
                                QString col9, QString col10, QString col11, QString col12,
                                QString col13, QString col14, QString col15, QString col17,
                                QString col18, QString col19,
                                QString fechaop20, QString claveop21, QString bicosto22,
                                QString rectificada, QString nfacturas, QString finicial,
                                QString ffinal,
                                QString binversion, QString afecto, QString agrario,
                                QString nombre, QString cif,
                                QString importacion, QString exportacion,
                                QString exenta_nodeduc, QString isp_op_interiores,
                                QString caja_iva, bool no347, bool arrto_ret, bool arrto_sin_ret, QString dua,
                                QString info_donacion)
{
    for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila,veces,newItem);
             }

  ui.Tablaapuntes->item(fila,46)->setText(importacion);
  ui.Tablaapuntes->item(fila,47)->setText(exportacion);
  ui.Tablaapuntes->item(fila,48)->setText(exenta_nodeduc);
  ui.Tablaapuntes->item(fila,49)->setText(isp_op_interiores);
  ui.Tablaapuntes->item(fila,5)->setText(col5);

  // QMessageBox::warning( this, tr("tabla"),col6);

  double num=convapunto(col6).toDouble();
  QString cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,6)->setText(cadnum);

  ui.Tablaapuntes->item(fila,7)->setText(col7);

  num=convapunto(col8).toDouble();
  cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,8)->setText(cadnum);

  num=convapunto(col9).toDouble();
  cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,9)->setText(cadnum);

  num=convapunto(col10).toDouble();
  cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,10)->setText(cadnum);

  ui.Tablaapuntes->item(fila,11)->setText(col11);
  ui.Tablaapuntes->item(fila,12)->setText(col12);
  ui.Tablaapuntes->item(fila,13)->setText(col13);
  ui.Tablaapuntes->item(fila,14)->setText(col14);
  ui.Tablaapuntes->item(fila,15)->setText(col15);
  // hay que transformar el valor de la columna 17 (prorrata)
  if (col17.length()>0)
     {
      double val=convapunto(col17).toDouble()*100;
      if (val>0.00001) cadnum.setNum(val,'f',0);
         else cadnum="100";
     }
     else cadnum="100";
  ui.Tablaapuntes->item(fila,17)->setText(cadnum);

  ui.Tablaapuntes->item(fila,18)->setText(col18);
  ui.Tablaapuntes->item(fila,19)->setText(col19);

  ui.Tablaapuntes->item(fila,20)->setText(fechaop20);
  ui.Tablaapuntes->item(fila,21)->setText(claveop21);

  num=convapunto(bicosto22).toDouble();
  cadnum=formateanumero(num,comadecimal,decimales);
  ui.Tablaapuntes->item(fila,22)->setText(cadnum);
  ui.Tablaapuntes->item(fila,23)->setText(rectificada);
  ui.Tablaapuntes->item(fila,24)->setText(nfacturas);
  ui.Tablaapuntes->item(fila,25)->setText(finicial);
  ui.Tablaapuntes->item(fila,26)->setText(ffinal);

  ui.Tablaapuntes->item(fila,29)->setText(binversion);

  double valafecto=convapunto(afecto).toDouble()*100;
  afecto.setNum(valafecto,'f',2);
  if (afecto.endsWith(".00")) afecto= afecto.section('.',0,0);
    else if (comadecimal) afecto=convacoma(afecto);
  ui.Tablaapuntes->item(fila,31)->setText(afecto);
  ui.Tablaapuntes->item(fila,32)->setText(agrario);
  ui.Tablaapuntes->item(fila,33)->setText(nombre);
  ui.Tablaapuntes->item(fila,34)->setText(cif);
  ui.Tablaapuntes->item(fila,50)->setText(caja_iva);
  ui.Tablaapuntes->item(fila,51)->setText(no347 ? "1" : "");

  // bool arrto_ret, bool arrto_sin_ret, QString dua
  ui.Tablaapuntes->item(fila,54)->setText(arrto_ret ? "1" : "");
  ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret ? "1" : "");
  ui.Tablaapuntes->item(fila,56)->setText(dua);
  ui.Tablaapuntes->item(fila,59)->setText(info_donacion);
  qDebug() << info_donacion;
}


void tabla_asientos::pasadatos_ret(int fila, QString cta_retenido,
                                QString arrendamiento, QString clave, QString base_ret,
                                QString tipo_ret, QString retencion, QString ing_a_cta,
                                QString ing_a_cta_rep, QString nombre_ret,
                                QString cif_ret, QString provincia)
{
    for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila,veces,newItem);
             }

    ui.Tablaapuntes->item(fila,35)->setText(cta_retenido);
    ui.Tablaapuntes->item(fila,36)->setText(arrendamiento);
    ui.Tablaapuntes->item(fila,37)->setText(clave);
    ui.Tablaapuntes->item(fila,38)->setText(base_ret);
    ui.Tablaapuntes->item(fila,39)->setText(tipo_ret);
    ui.Tablaapuntes->item(fila,40)->setText(retencion);
    ui.Tablaapuntes->item(fila,41)->setText(ing_a_cta);
    ui.Tablaapuntes->item(fila,42)->setText(ing_a_cta_rep);
    ui.Tablaapuntes->item(fila,43)->setText(nombre_ret);
    ui.Tablaapuntes->item(fila,44)->setText(cif_ret);
    ui.Tablaapuntes->item(fila,45)->setText(provincia);

}

void tabla_asientos::activaparaedicion()
{
  ui.BotonIncorporar->setEnabled(true);
  ui.AsientoLabel->setEnabled(true);
  ui.AsientolineEdit->setEnabled(true);
  ui.numreceplineEdit->setReadOnly(false);
}


bool tabla_asientos::esparaedicion()
{
  return ui.BotonIncorporar->isEnabled();
}


void tabla_asientos::chequeatotales()
{

bool analiticaok=true;
bool ok;
double sumadebe,sumahaber,valor;
QString cadena;
int veces;
sumadebe=0;
 for (veces=0;veces<ultimafila();veces++)
     {
        valor=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble(&ok);
        if (ok) sumadebe+=valor;
        // chequeamos aquí analítica tabla
        if (basedatos::instancia()->analitica_tabla() &&
            !modoconsulta && (conanalitica() || conanalitica_parc()))
        {
           if ((conanalitica() && (escuentadegasto(ui.Tablaapuntes->item(veces,0)->text()) ||
                escuentadeingreso(ui.Tablaapuntes->item(veces,0)->text())))
                || (conanalitica_parc() &&
                codigo_en_analitica(ui.Tablaapuntes->item(veces,0)->text())))
               {
                if (inputaciones[veces]==0)
                   {
                    analiticaok=false;
                   }
                  else
                      {
                       if (!inputaciones[veces]->input_OK())
                         {
                           analiticaok=false;
                         }
                      }
               }
        }
     }

 int ndecimales=2;
 if (!decimales) ndecimales=0;
 cadena=QString( "%1" ).arg(
                               sumadebe, 0, 'f',ndecimales ) ;
 ui.LCDSumadebe->display(cadena);
   

 // Escribimos nmero en LCD de haber
sumahaber=0;
 for (veces=0;veces<ultimafila();veces++)
	    {
		valor=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble(&ok);
		if (ok) sumahaber+=valor;
		}
 cadena=QString( "%1" ).arg(
                               sumahaber, 0, 'f',ndecimales ) ;
 ui.LCDSumahaber->display(cadena);

 // Escribimos nmero en LCD de descuadre
 cadena=QString( "%1" ).arg(
                               sumadebe-sumahaber, 0, 'f',ndecimales ) ;
 
 if (sumadebe-sumahaber>-0.0001)
      descuadreazul();
 else
      descuadrerojo();
 
 ui.LCDDescuadre->display(cadena);
 
 if (sumadebe-sumahaber<0.001 && sumadebe-sumahaber>-0.001)
    // && (sumadebe>0.0001 || sumadebe<-0.0001)
    // && (sumahaber>0.0001 || sumahaber<-0.0001))
    {
     if (ui.Tablaapuntes->item(1,0)==NULL) ui.BotonIncorporar->setEnabled(false);
        else if (! ui.Tablaapuntes->item(1,0)->text().isEmpty())
               ui.BotonIncorporar->setEnabled(true);
     ui.BotonIncorporar->setFocus();
    }
    else ui.BotonIncorporar->setEnabled(false);

  if (fechadejerciciocerrado(ui.FechaApunte->date()) || !fechacorrespondeaejercicio(ui.FechaApunte->date()))
       ui.BotonIncorporar->setEnabled(false);
  if (!analiticaok) ui.BotonIncorporar->setEnabled(false);
  // if (!analiticaok) QMessageBox::warning( this, tr("tabla"),"error analitica");
}




void tabla_asientos::soportadoautonomo()
{
   ivasop_aut *qIVAsoportado = new ivasop_aut(comadecimal,decimales);
   // qIVAsoportado->hazbaseoblig();
   if (pasar_fecha_iva_aut)
      {
       qIVAsoportado->pasa_fecha(fecha_iva_aut);
      }
   int resultado=qIVAsoportado->exec();
   if (resultado==QDialog::Rejected) 
            {
            }
   if (resultado==QDialog::Accepted)
            {
	     QString qclaveiva,qtipoiva,qctafra,qctaiva,qcuentabase,qbase,qcuotaiva,qprorrata;
             QString qdocumento, qclaveop;
             QString qclaveiva2,qtipoiva2,qctaiva2,qcuentabase2,qbase2,qcuotaiva2;
             QString qclaveiva3,qtipoiva3,qctaiva3,qcuentabase3,qbase3,qcuotaiva3;
             QString rectificada, nfacturas, finicial, ffinal, binversion, afectacion;
             QDate qfechafra,qfechacontable, qfechaop;
             bool rectificativa, verifica;
             bool agrario;
             QString nombre, cif, import, cajaiva;
             QString cuenta_ret, tipo_ret, qretencion, clave_ret;
             bool ret_arrendamiento;
             bool iva_exento;
             QString cta_base_exento;
             QString base_imponible_exento;
             QString externo;
             /*
	     qIVAsoportado->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
					&qtipoiva,&qctaiva, &qcuota,&qctafra,
					&qfechafact,&soportado,&qprorrata);
             */

            qIVAsoportado->recuperadatos( 
                      &qcuentabase, &qbase, &qclaveiva, &qtipoiva,&qctaiva, &qcuotaiva,
                      &qcuentabase2, &qbase2, &qclaveiva2, &qtipoiva2,&qctaiva2, &qcuotaiva2,
                      &qcuentabase3, &qbase3, &qclaveiva3, &qtipoiva3,&qctaiva3, &qcuotaiva3,
                      &qctafra, &qprorrata, &qfechafra, &qfechacontable,
                      &qdocumento, &qfechaop, &qclaveop, &rectificativa,
                      &rectificada, &nfacturas, &finicial, &ffinal,
                      &verifica, &binversion, &afectacion, &agrario, &nombre, &cif, &import,
                      &cajaiva, &cuenta_ret, &tipo_ret, &qretencion, &clave_ret,&ret_arrendamiento,
                        &iva_exento, &cta_base_exento, &base_imponible_exento, &externo);
             hay_fecha_factura=true;
             fecha_factura=qfechafra;
             ui.fichdoclineEdit->setText(qIVAsoportado->fichdocumento());
             ui.externo_lineEdit->setText(externo);
	     QString concepto;
	     concepto=descripcioncuenta(qctafra);
         QString concepto_plantilla=qIVAsoportado->concepto();
         if (!concepto_plantilla.isEmpty()) concepto=concepto_plantilla;
             double prorrata=0;
             if (qprorrata.length()>0) prorrata=convapunto(qprorrata).toDouble()/100;
             double val_afectacion=convapunto(afectacion).toDouble()/100;
	     // empezamos por la primera línea = la base imponible primera
         int fila_actual=0;
         QString baseanalitica;
         if (qcuentabase.length()>0) {
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(0,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(0,veces,newItem);
                   }
             ui.Tablaapuntes->item(0,0)->setText(qcuentabase);
             ui.Tablaapuntes->item(0,1)->setText(concepto);
             baseanalitica=qbase;
             if (qprorrata.length()>0 || (!afectacion.isEmpty() && afectacion!="100"))
                {
                 double importe=convapunto(qbase).toDouble();
                 double corrector=1;
                 if (qprorrata.length()>0) corrector=prorrata;
                 if (!afectacion.isEmpty() && afectacion!="100")
                    corrector=corrector*val_afectacion;
                 importe+=convapunto(qcuotaiva).toDouble()*(1-corrector);
                 importe=CutAndRoundNumberToNDecimals(importe,2);
                 QString cadnum;
                 cadnum.setNum(importe,'f',2);
                 if (comadecimal) cadnum=convacoma(cadnum);
                 ui.Tablaapuntes->item(0,2)->setText(cadnum);
                 baseanalitica=cadnum;
                }
               else
	         ui.Tablaapuntes->item(0,2)->setText(qbase);
             if (es_cuenta_para_analitica(qcuentabase) &&
                 basedatos::instancia()->analitica_tabla())
                 realiza_asignainputdiario(0,baseanalitica);
             // segunda línea, la de la cuenta del primer IVA -- aquí nos quedamos
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(1,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(1,veces,newItem);
                   }
             ui.Tablaapuntes->item(1,0)->setText(qctaiva);
             ui.Tablaapuntes->item(1,1)->setText(concepto);
             if (qprorrata.length()==0 && (afectacion.isEmpty() || afectacion=="100"))
             ui.Tablaapuntes->item(1,2)->setText(qcuotaiva);
               else
                   {
                    double valor=convapunto(qcuotaiva).toDouble();
                    if (qprorrata.length()>0) valor =valor*prorrata;
                    if (!afectacion.isEmpty() && afectacion!="100")
                        valor=valor*val_afectacion;
                    valor=CutAndRoundNumberToNDecimals(valor,2);
                    QString cadnum;
                    cadnum.setNum(valor,'f',2);
                    if (comadecimal) cadnum=convacoma(cadnum);
                    ui.Tablaapuntes->item(1,2)->setText(cadnum);
                   }
             // actualizamos columnas 5 a 15
             ui.Tablaapuntes->item(1,5)->setText(qcuentabase);
             ui.Tablaapuntes->item(1,6)->setText(qbase);
             ui.Tablaapuntes->item(1,7)->setText(qclaveiva);
             ui.Tablaapuntes->item(1,8)->setText(qtipoiva);
             ui.Tablaapuntes->item(1,9)->setText("");
             ui.Tablaapuntes->item(1,10)->setText(qcuotaiva);
             ui.Tablaapuntes->item(1,11)->setText(qctafra);
             QString caddia; caddia.setNum(qfechafra.day());
             QString cadmes; cadmes.setNum(qfechafra.month());
             QString cadyear; cadyear.setNum(qfechafra.year());
             ui.Tablaapuntes->item(1,12)->setText(caddia);
             ui.Tablaapuntes->item(1,13)->setText(cadmes);
             ui.Tablaapuntes->item(1,14)->setText(cadyear);
             ui.Tablaapuntes->item(1,15)->setText("1");
             ui.Tablaapuntes->item(1,17)->setText(qprorrata);
             ui.Tablaapuntes->item(1,18)->setText(rectificativa ? "1" : "0");
             ui.Tablaapuntes->item(1,20)->setText(qfechaop.toString("yyyy-MM-dd"));
             ui.Tablaapuntes->item(1,21)->setText(qclaveop);
             ui.Tablaapuntes->item(1,23)->setText(rectificada);
             ui.Tablaapuntes->item(1,24)->setText(nfacturas);
             ui.Tablaapuntes->item(1,25)->setText(finicial);
             ui.Tablaapuntes->item(1,26)->setText(ffinal);
             ui.Tablaapuntes->item(1,29)->setText(binversion);
             ui.Tablaapuntes->item(1,31)->setText(afectacion);
             ui.Tablaapuntes->item(1,32)->setText(agrario ? "1" : "0");
             ui.Tablaapuntes->item(1,33)->setText(nombre);
             ui.Tablaapuntes->item(1,34)->setText(cif);
             ui.Tablaapuntes->item(1,46)->setText(import);
             ui.Tablaapuntes->item(1,50)->setText(cajaiva);
             fila_actual=1;
         }
             // seguimos por la segunda base imponible
	     // qcuentabase2
             if (qcuentabase2.length()>0)
               {
                if (fila_actual>0) fila_actual++;
                for (int veces=0;veces<=COLUMNAS;veces++)
                   if (ui.Tablaapuntes->item(fila_actual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(fila_actual,veces,newItem);
                   }
                ui.Tablaapuntes->item(fila_actual,0)->setText(qcuentabase2);
                ui.Tablaapuntes->item(fila_actual,1)->setText(concepto);
                baseanalitica=qbase2;
                if (qprorrata.length()>0 || (!afectacion.isEmpty() && afectacion!="100"))
                   { // **********************************
                    double importe=convapunto(qbase2).toDouble();
                    double corrector=1;
                    if (qprorrata.length()>0) corrector=prorrata;
                    if (!afectacion.isEmpty() && afectacion!="100")
                       corrector=corrector*val_afectacion;
                    importe+=convapunto(qcuotaiva2).toDouble()*(1-corrector);
                    QString cadnum;
                    importe=CutAndRoundNumberToNDecimals(importe,2);
                    cadnum.setNum(importe,'f',2);
                    if (comadecimal) cadnum=convacoma(cadnum);
                    ui.Tablaapuntes->item(fila_actual,2)->setText(cadnum);
                    baseanalitica=cadnum;
                   }
                 else
	            ui.Tablaapuntes->item(fila_actual,2)->setText(qbase2);
                 if (es_cuenta_para_analitica(qcuentabase2) &&
                     basedatos::instancia()->analitica_tabla())
                     realiza_asignainputdiario(fila_actual,baseanalitica);
                // siguiente línea, la de la cuenta del segundo IVA
                fila_actual++;
                for (int veces=0;veces<=COLUMNAS;veces++)
                   if (ui.Tablaapuntes->item(fila_actual,veces)==0)
                    {
                     QTableWidgetItem *newItem = new QTableWidgetItem("");
                     if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                     ui.Tablaapuntes->setItem(fila_actual,veces,newItem);
                    }
                ui.Tablaapuntes->item(fila_actual,0)->setText(qctaiva2);
                ui.Tablaapuntes->item(fila_actual,1)->setText(concepto);
                if (qprorrata.length()==0 && (afectacion.isEmpty() || afectacion=="100"))
                    // 222222222222222222222222222
                ui.Tablaapuntes->item(fila_actual,2)->setText(qcuotaiva2);
                  else
                   {
                      double valor=convapunto(qcuotaiva2).toDouble();
                      if (qprorrata.length()>0) valor =valor*prorrata;
                      if (!afectacion.isEmpty() && afectacion!="100")
                          valor=valor*val_afectacion;
                      valor=CutAndRoundNumberToNDecimals(valor,2);
                      QString cadnum;
                      cadnum.setNum(valor,'f',2);
                      if (comadecimal) cadnum=convacoma(cadnum);
                      ui.Tablaapuntes->item(fila_actual,2)->setText(cadnum);
                   }
                // actualizamos columnas 5 a 15 y 17
                ui.Tablaapuntes->item(fila_actual,5)->setText(qcuentabase2);
                ui.Tablaapuntes->item(fila_actual,6)->setText(qbase2);
                ui.Tablaapuntes->item(fila_actual,7)->setText(qclaveiva2);
                ui.Tablaapuntes->item(fila_actual,8)->setText(qtipoiva2);
                ui.Tablaapuntes->item(fila_actual,9)->setText("");
                ui.Tablaapuntes->item(fila_actual,10)->setText(qcuotaiva2);
                ui.Tablaapuntes->item(fila_actual,11)->setText(qctafra);
                QString caddia; caddia.setNum(qfechafra.day());
                QString cadmes; cadmes.setNum(qfechafra.month());
                QString cadyear; cadyear.setNum(qfechafra.year());
                ui.Tablaapuntes->item(fila_actual,12)->setText(caddia);
                ui.Tablaapuntes->item(fila_actual,13)->setText(cadmes);
                ui.Tablaapuntes->item(fila_actual,14)->setText(cadyear);
                ui.Tablaapuntes->item(fila_actual,15)->setText("1");
                ui.Tablaapuntes->item(fila_actual,17)->setText(qprorrata);
                ui.Tablaapuntes->item(fila_actual,18)->setText(rectificativa ? "1" : "0");
                ui.Tablaapuntes->item(fila_actual,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                ui.Tablaapuntes->item(fila_actual,21)->setText(qclaveop);
                ui.Tablaapuntes->item(fila_actual,23)->setText(rectificada);
                ui.Tablaapuntes->item(fila_actual,24)->setText(nfacturas);
                ui.Tablaapuntes->item(fila_actual,25)->setText(finicial);
                ui.Tablaapuntes->item(fila_actual,26)->setText(ffinal);
                ui.Tablaapuntes->item(fila_actual,29)->setText(binversion);
                ui.Tablaapuntes->item(fila_actual,31)->setText(afectacion);
                ui.Tablaapuntes->item(fila_actual,32)->setText(agrario ? "1" : "0");
                ui.Tablaapuntes->item(fila_actual,33)->setText(nombre);
                ui.Tablaapuntes->item(fila_actual,34)->setText(cif);
                ui.Tablaapuntes->item(fila_actual,46)->setText(import);
                ui.Tablaapuntes->item(fila_actual,50)->setText(cajaiva);
               }
             // seguimos por la tercera base imponible
             // qcuentabase3
             if (qcuentabase3.length()>0)
               {
                if (fila_actual>0) fila_actual++;
                for (int veces=0;veces<=COLUMNAS;veces++)
                   if (ui.Tablaapuntes->item(fila_actual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(fila_actual,veces,newItem);
                   }
                ui.Tablaapuntes->item(fila_actual,0)->setText(qcuentabase3);
                ui.Tablaapuntes->item(fila_actual,1)->setText(concepto);
                baseanalitica=qbase3;
                if (qprorrata.length()>0 || (!afectacion.isEmpty() && afectacion!="100"))
                   {
                    double importe=convapunto(qbase3).toDouble();
                    double corrector=1;
                    if (qprorrata.length()>0) corrector=prorrata;
                    if (!afectacion.isEmpty() && afectacion!="100")
                       corrector=corrector*val_afectacion;
                    importe+=convapunto(qcuotaiva3).toDouble()*(1-corrector);
                    importe=CutAndRoundNumberToNDecimals(importe,2);
                    QString cadnum;
                    cadnum.setNum(importe,'f',2);
                    if (comadecimal) cadnum=convacoma(cadnum);
                    ui.Tablaapuntes->item(fila_actual,2)->setText(cadnum);
                    baseanalitica=qbase3;
                   }
                 else
	            ui.Tablaapuntes->item(fila_actual,2)->setText(qbase3);
                 if (es_cuenta_para_analitica(qcuentabase3) &&
                     basedatos::instancia()->analitica_tabla())
                     realiza_asignainputdiario(fila_actual,baseanalitica);
                // siguiente línea, la de la cuenta del tercer IVA
                fila_actual++;
                for (int veces=0;veces<=COLUMNAS;veces++)
                   if (ui.Tablaapuntes->item(fila_actual,veces)==0)
                    {
                     QTableWidgetItem *newItem = new QTableWidgetItem("");
                     if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                     ui.Tablaapuntes->setItem(fila_actual,veces,newItem);
                    }
                 ui.Tablaapuntes->item(fila_actual,0)->setText(qctaiva3);
                 ui.Tablaapuntes->item(fila_actual,1)->setText(concepto);
                 if (qprorrata.length()==0 && (afectacion.isEmpty() || afectacion=="100"))
                    // 33333333333333333333333333333333333333
                   ui.Tablaapuntes->item(fila_actual,2)->setText(qcuotaiva3);
                  else
                   {
                      double valor=convapunto(qcuotaiva3).toDouble();
                      if (qprorrata.length()>0) valor =valor*prorrata;
                      if (!afectacion.isEmpty() && afectacion!="100")
                          valor=valor*val_afectacion;
                      valor=CutAndRoundNumberToNDecimals(valor,2);
                      QString cadnum;
                      cadnum.setNum(valor,'f',2);
                      if (comadecimal) cadnum=convacoma(cadnum);
                      ui.Tablaapuntes->item(fila_actual,2)->setText(cadnum);
                   }
                 // actualizamos columnas 5 a 15 y 17
                 ui.Tablaapuntes->item(fila_actual,5)->setText(qcuentabase3);
                 ui.Tablaapuntes->item(fila_actual,6)->setText(qbase3);
                 ui.Tablaapuntes->item(fila_actual,7)->setText(qclaveiva3);
                 ui.Tablaapuntes->item(fila_actual,8)->setText(qtipoiva3);
                 ui.Tablaapuntes->item(fila_actual,9)->setText("");
                 ui.Tablaapuntes->item(fila_actual,10)->setText(qcuotaiva3);
                 ui.Tablaapuntes->item(fila_actual,11)->setText(qctafra);
                 QString caddia; caddia.setNum(qfechafra.day());
                 QString cadmes; cadmes.setNum(qfechafra.month());
                 QString cadyear; cadyear.setNum(qfechafra.year());
                 ui.Tablaapuntes->item(fila_actual,12)->setText(caddia);
                 ui.Tablaapuntes->item(fila_actual,13)->setText(cadmes);
                 ui.Tablaapuntes->item(fila_actual,14)->setText(cadyear);
                 ui.Tablaapuntes->item(fila_actual,15)->setText("1");
                 ui.Tablaapuntes->item(fila_actual,17)->setText(qprorrata);
                 ui.Tablaapuntes->item(fila_actual,18)->setText(rectificativa ? "1" : "0");
                 ui.Tablaapuntes->item(fila_actual,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                 ui.Tablaapuntes->item(fila_actual,21)->setText(qclaveop);
                 ui.Tablaapuntes->item(fila_actual,23)->setText(rectificada);
                 ui.Tablaapuntes->item(fila_actual,24)->setText(nfacturas);
                 ui.Tablaapuntes->item(fila_actual,25)->setText(finicial);
                 ui.Tablaapuntes->item(fila_actual,26)->setText(ffinal);
                 ui.Tablaapuntes->item(fila_actual,29)->setText(binversion);
                 ui.Tablaapuntes->item(fila_actual,31)->setText(afectacion);
                 ui.Tablaapuntes->item(fila_actual,32)->setText(agrario ? "1" : "0");
                 ui.Tablaapuntes->item(fila_actual,33)->setText(nombre);
                 ui.Tablaapuntes->item(fila_actual,34)->setText(cif);
                 ui.Tablaapuntes->item(fila_actual,46)->setText(import);
                 ui.Tablaapuntes->item(fila_actual,50)->setText(cajaiva);
               }
             // -------------------------------------------------------------------------------------------------
             if (iva_exento)
                {
                  // metemos línea con base imponible (gasto)
                 if (fila_actual>0) fila_actual++;
                 for (int veces=0;veces<=COLUMNAS;veces++)
                    if (ui.Tablaapuntes->item(fila_actual,veces)==0)
                    {
                     QTableWidgetItem *newItem = new QTableWidgetItem("");
                     if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                     ui.Tablaapuntes->setItem(fila_actual,veces,newItem);
                    }
                 ui.Tablaapuntes->item(fila_actual,0)->setText(cta_base_exento);
                 ui.Tablaapuntes->item(fila_actual,1)->setText(concepto);
                 ui.Tablaapuntes->item(fila_actual,2)->setText(base_imponible_exento);
                 if (es_cuenta_para_analitica(cta_base_exento) &&
                      basedatos::instancia()->analitica_tabla())
                      realiza_asignainputdiario(fila_actual,base_imponible_exento);

                }
	     // actualizamos línea siguiente con ctafra
             fila_actual++;
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(fila_actual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(fila_actual,veces,newItem);
                   }
 	    ui.Tablaapuntes->item(fila_actual,0)->setText(qctafra);
	    ui.Tablaapuntes->item(fila_actual,1)->setText(concepto);
	    double vtotal;
	    vtotal=convapunto(qbase).toDouble()+convapunto(qcuotaiva).toDouble()+
                   convapunto(qbase2).toDouble()+convapunto(qcuotaiva2).toDouble()+
                   convapunto(qbase3).toDouble()+convapunto(qcuotaiva3).toDouble()+
                   convapunto(base_imponible_exento).toDouble();
        if (!cuenta_ret.isEmpty()) vtotal=vtotal-convapunto(qretencion).toDouble();
	    ui.Tablaapuntes->item(fila_actual,3)->setText(formateanumero(vtotal,comadecimal,decimales));
        // averiguamos si hay iva exento
        // iva_exento, &cta_base_exento, &base_imponible_exento
        if (iva_exento)
           {
            if (verifica) ui.exentacheckBox->setChecked(true);
            pasainfoexento(true,  cta_base_exento,
                           base_imponible_exento, qctafra, qfechafra,
                           qfechaop, qclaveop, rectificativa ? "1" : "0",
                           rectificada, nfacturas, finicial,
                           ffinal, binversion=="1", nombre, cif,
                           import=="1", false, false, false,false,
                           false,"",false,false,"",0, "");
            ui.Tablaapuntes->item(fila_actual,5)->setText(cta_base_exento);
            ui.Tablaapuntes->item(fila_actual,6)->setText(base_imponible_exento);
            ui.Tablaapuntes->item(fila_actual,7)->setText("");
            ui.Tablaapuntes->item(fila_actual,8)->setText("0");
            ui.Tablaapuntes->item(fila_actual,9)->setText("");
            ui.Tablaapuntes->item(fila_actual,10)->setText("0");
            ui.Tablaapuntes->item(fila_actual,11)->setText(qctafra);
            QString caddia,cadmes,cadyear;
            caddia.setNum(qfechafra.day());
            cadmes.setNum(qfechafra.month());
            cadyear.setNum(qfechafra.year());
            ui.Tablaapuntes->item(fila_actual,12)->setText(caddia);
            ui.Tablaapuntes->item(fila_actual,13)->setText(cadmes);
            ui.Tablaapuntes->item(fila_actual,14)->setText(cadyear);
            ui.Tablaapuntes->item(fila_actual,15)->setText("1"); // soportado

            ui.Tablaapuntes->item(fila_actual,20)->setText(qfechaop.toString("yyyy-MM-dd"));
            ui.Tablaapuntes->item(fila_actual,21)->setText(qclaveop);
            ui.Tablaapuntes->item(fila_actual,18)->setText(rectificativa ? "1" : "0");
            ui.Tablaapuntes->item(fila_actual,23)->setText(rectificada);
            ui.Tablaapuntes->item(fila_actual,24)->setText(nfacturas);
            ui.Tablaapuntes->item(fila_actual,25)->setText(finicial);
            ui.Tablaapuntes->item(fila_actual,26)->setText(ffinal);
            ui.Tablaapuntes->item(fila_actual,29)->setText(binversion);
            ui.Tablaapuntes->item(fila_actual,30)->setText("0");
            ui.Tablaapuntes->item(fila_actual,33)->setText(nombre);
            ui.Tablaapuntes->item(fila_actual,34)->setText(cif);
            ui.Tablaapuntes->item(fila_actual,46)->setText(import);
            ui.Tablaapuntes->item(fila_actual,47)->setText("0");
            ui.Tablaapuntes->item(fila_actual,48)->setText("");


           }
        // añadimos apuntes de retención
        if (!cuenta_ret.isEmpty())
           {
             fila_actual++;
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(fila_actual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(fila_actual,veces,newItem);
                   }
              ui.Tablaapuntes->item(fila_actual,0)->setText(cuenta_ret);
              ui.Tablaapuntes->item(fila_actual,1)->setText(concepto);
              ui.Tablaapuntes->item(fila_actual,3)->setText(qretencion);

              ui.Tablaapuntes->item(fila_actual,35)->setText(qctafra); //cuenta retenido
              ui.Tablaapuntes->item(fila_actual,36)->setText(ret_arrendamiento ? "1" : "0"); //retención de arrendamiento
              ui.Tablaapuntes->item(fila_actual,37)->setText(clave_ret); //clave arrendamiento
              QString base_percepciones;
              base_percepciones.setNum(convapunto(qbase).toDouble()+
                                        convapunto(qbase2).toDouble()+
                                        convapunto(qbase3).toDouble()+convapunto(base_imponible_exento).toDouble(),'f',2);
              ui.Tablaapuntes->item(fila_actual,38)->setText(base_percepciones); //base percepciones
              ui.Tablaapuntes->item(fila_actual,39)->setText(tipo_ret); //tipo retención
              ui.Tablaapuntes->item(fila_actual,40)->setText(qretencion); //retención
              ui.Tablaapuntes->item(fila_actual,41)->setText(""); //ingreso a cuenta
              ui.Tablaapuntes->item(fila_actual,42)->setText(""); //ingreso a cuenta repercutido
              ui.Tablaapuntes->item(fila_actual,43)->setText(nombre); //nombre retenido
              ui.Tablaapuntes->item(fila_actual,44)->setText(cif); //cif retenido
              ui.Tablaapuntes->item(fila_actual,45)->setText(""); //provincia
           }

	    if (qdocumento.length()>0)
	            {
		     for (int veces=0;veces<=fila_actual;veces++)
                       {
                        if (concepto_plantilla.isEmpty())
                            ui.Tablaapuntes->item(veces,1)->setText(concepto+" Fra. "+qdocumento);
     	                ui.Tablaapuntes->item(veces,4)->setText(qdocumento);
                       }
	            }
	    ui.FechaApunte->setDate(qfechacontable);
	    // si botón aceptar activado pasar, si no edición
	    // contenidocambiado(0,0);
	    chequeatotales();
	    if (verifica || !ui.BotonIncorporar->isEnabled()
		|| (fechadejerciciocerrado(ui.FechaApunte->date()))
		|| !fechacorrespondeaejercicio(ui.FechaApunte->date()) || conanalitica()
                || conanalitica_parc()) exec();
	        else incorporar();
	  }
    delete(qIVAsoportado);

}


void tabla_asientos::repercutidoautonomo()
{
    ivarep_aut *qIVArepercutido = new ivarep_aut(comadecimal,decimales);
    if (pasar_fecha_iva_aut)
       {
        qIVArepercutido->pasa_fecha(fecha_iva_aut);
       }
   int resultado=qIVArepercutido->exec();
   if (resultado==QDialog::Rejected) 
            {
            }

   if (resultado==QDialog::Accepted)
        {
          QString qcuentabase,qbase,qclaveiva,qtipoiva,qtipore,qcuota,qctafra,qctaiva,qdoc;
          QString qcuentabase2,qbase2,qclaveiva2,qtipoiva2,qtipore2,qcuota2,qctaiva2;
          QString qcuentabase3,qbase3,qclaveiva3,qtipoiva3,qtipore3,qcuota3,qctaiva3;
          QDate qfechafact,qfechacont;
          QDate qfechaop;
          QString qclaveop,rectificada,nfacturas,finicial,ffinal;
          bool rectificativa;
          bool verifica,cajaiva;
          QString nombre,cif,qexterno;
          qIVArepercutido->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
					&qtipoiva,&qtipore,&qctaiva, &qcuota,

                                        &qcuentabase2,&qbase2,&qclaveiva2,
					&qtipoiva2,&qtipore2,&qctaiva2, &qcuota2,

                                        &qcuentabase3,&qbase3,&qclaveiva3,
					&qtipoiva3,&qtipore3,&qctaiva3, &qcuota3,

					&qctafra,
                                        &qfechafact,&qfechacont,&qdoc,
                                        &qfechaop, &qclaveop, &rectificativa,
                                        &rectificada, &nfacturas, &finicial, &ffinal,
                                        &verifica, &nombre, &cif, &cajaiva, &qexterno);
         hay_fecha_factura=true;
         fecha_factura=qfechafact;
         ui.fichdoclineEdit->setText(qIVArepercutido->fichdocumento());
         ui.externo_lineEdit->setText(qexterno);
         QString concepto;
         QString concepto_plantilla=qIVArepercutido->concepto();
         concepto=descripcioncuenta(qctafra);
         delete(qIVArepercutido);
         if (!concepto_plantilla.isEmpty()) concepto=concepto_plantilla;
         // empezamos por la primera línea = la base imponible
         for (int veces=0;veces<=COLUMNAS;veces++)
             if (ui.Tablaapuntes->item(0,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(0,veces,newItem);
                   }
         ui.Tablaapuntes->item(0,0)->setText(qcuentabase);
         ui.Tablaapuntes->item(0,1)->setText(concepto);
         ui.Tablaapuntes->item(0,3)->setText(qbase);
         if (es_cuenta_para_analitica(qcuentabase) &&
             basedatos::instancia()->analitica_tabla())
             realiza_asignainputdiario(0,qbase);

         // segunda línea, la de la cuenta del primer IVA
         for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(1,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(1,veces,newItem);
                   }
         ui.Tablaapuntes->item(1,0)->setText(qctaiva);
         ui.Tablaapuntes->item(1,1)->setText(concepto);
         ui.Tablaapuntes->item(1,3)->setText(qcuota);
         // actualizamos columnas 5 a 15
         ui.Tablaapuntes->item(1,5)->setText(qcuentabase);
         ui.Tablaapuntes->item(1,6)->setText(qbase);
         ui.Tablaapuntes->item(1,7)->setText(qclaveiva);
         ui.Tablaapuntes->item(1,8)->setText(qtipoiva);
         ui.Tablaapuntes->item(1,9)->setText(qtipore);
         ui.Tablaapuntes->item(1,10)->setText(qcuota);
         ui.Tablaapuntes->item(1,11)->setText(qctafra);
         QString caddia; caddia.setNum(qfechafact.day());
         QString cadmes; cadmes.setNum(qfechafact.month());
         QString cadyear; cadyear.setNum(qfechafact.year());
         ui.Tablaapuntes->item(1,12)->setText(caddia);
         ui.Tablaapuntes->item(1,13)->setText(cadmes);
         ui.Tablaapuntes->item(1,14)->setText(cadyear);
         ui.Tablaapuntes->item(1,15)->setText("0");
         ui.Tablaapuntes->item(1,18)->setText(rectificativa ? "1" : "0");
         ui.Tablaapuntes->item(1,20)->setText(qfechaop.toString("yyyy-MM-dd"));
         ui.Tablaapuntes->item(1,21)->setText(qclaveop);
         ui.Tablaapuntes->item(1,23)->setText(rectificada);
         ui.Tablaapuntes->item(1,24)->setText(nfacturas);
         ui.Tablaapuntes->item(1,25)->setText(finicial);
         ui.Tablaapuntes->item(1,26)->setText(ffinal);
         ui.Tablaapuntes->item(1,33)->setText(nombre);
         ui.Tablaapuntes->item(1,34)->setText(cif);
         ui.Tablaapuntes->item(1,50)->setText(cajaiva ? "1" : "0");

         // -------------------------------------------------------------------------------------------------
         // segundo iva
         int lineaactual=1;
         if (qcuentabase2.length()>0)
            {
             lineaactual++;
             // empezamos por la primera línea = la base imponible
            for (int veces=0;veces<=COLUMNAS;veces++)
              if (ui.Tablaapuntes->item(lineaactual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(lineaactual,veces,newItem);
                   }
            ui.Tablaapuntes->item(lineaactual,0)->setText(qcuentabase2);
            ui.Tablaapuntes->item(lineaactual,1)->setText(concepto);
            ui.Tablaapuntes->item(lineaactual,3)->setText(qbase2);
            if (es_cuenta_para_analitica(qcuentabase2) &&
                basedatos::instancia()->analitica_tabla())
                realiza_asignainputdiario(lineaactual,qbase2);
            // segunda línea, la de la cuenta del IVA
            lineaactual++;
            for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(lineaactual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(lineaactual,veces,newItem);
                   }
            ui.Tablaapuntes->item(lineaactual,0)->setText(qctaiva2);
            ui.Tablaapuntes->item(lineaactual,1)->setText(concepto);
            ui.Tablaapuntes->item(lineaactual,3)->setText(qcuota2);
            // actualizamos columnas 5 a 15
            ui.Tablaapuntes->item(lineaactual,5)->setText(qcuentabase2);
            ui.Tablaapuntes->item(lineaactual,6)->setText(qbase2);
            ui.Tablaapuntes->item(lineaactual,7)->setText(qclaveiva2);
            ui.Tablaapuntes->item(lineaactual,8)->setText(qtipoiva2);
            ui.Tablaapuntes->item(lineaactual,9)->setText(qtipore2);
            ui.Tablaapuntes->item(lineaactual,10)->setText(qcuota2);
            ui.Tablaapuntes->item(lineaactual,11)->setText(qctafra);
            QString caddia; caddia.setNum(qfechafact.day());
            QString cadmes; cadmes.setNum(qfechafact.month());
            QString cadyear; cadyear.setNum(qfechafact.year());
            ui.Tablaapuntes->item(lineaactual,12)->setText(caddia);
            ui.Tablaapuntes->item(lineaactual,13)->setText(cadmes);
            ui.Tablaapuntes->item(lineaactual,14)->setText(cadyear);
            ui.Tablaapuntes->item(lineaactual,15)->setText("0");
            ui.Tablaapuntes->item(lineaactual,18)->setText(rectificativa ? "1" : "0");
            ui.Tablaapuntes->item(lineaactual,20)->setText(qfechaop.toString("yyyy-MM-dd"));
            ui.Tablaapuntes->item(lineaactual,21)->setText(qclaveop);
             ui.Tablaapuntes->item(lineaactual,23)->setText(rectificada);
             ui.Tablaapuntes->item(lineaactual,24)->setText(nfacturas);
             ui.Tablaapuntes->item(lineaactual,25)->setText(finicial);
             ui.Tablaapuntes->item(lineaactual,26)->setText(ffinal);
             ui.Tablaapuntes->item(lineaactual,33)->setText(nombre);
             ui.Tablaapuntes->item(lineaactual,34)->setText(cif);
             ui.Tablaapuntes->item(lineaactual,50)->setText(cajaiva ? "1" : "0");
            }
         // -----------------------------------------------------------------------------------------------
         // tercer iva
         if (qcuentabase3.length()>0)
            {
             lineaactual++;
             // empezamos por la primera línea = la base imponible
            for (int veces=0;veces<=COLUMNAS;veces++)
              if (ui.Tablaapuntes->item(lineaactual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(lineaactual,veces,newItem);
                   }
            ui.Tablaapuntes->item(lineaactual,0)->setText(qcuentabase3);
            ui.Tablaapuntes->item(lineaactual,1)->setText(concepto);
            ui.Tablaapuntes->item(lineaactual,3)->setText(qbase3);
            if (es_cuenta_para_analitica(qcuentabase3) &&
                basedatos::instancia()->analitica_tabla())
                realiza_asignainputdiario(lineaactual,qbase3);
            // segunda línea, la de la cuenta del primer IVA
            lineaactual++;
            for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(lineaactual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(lineaactual,veces,newItem);
                   }
            ui.Tablaapuntes->item(lineaactual,0)->setText(qctaiva3);
            ui.Tablaapuntes->item(lineaactual,1)->setText(concepto);
            ui.Tablaapuntes->item(lineaactual,3)->setText(qcuota3);
            // actualizamos columnas 5 a 15
            ui.Tablaapuntes->item(lineaactual,5)->setText(qcuentabase3);
            ui.Tablaapuntes->item(lineaactual,6)->setText(qbase3);
            ui.Tablaapuntes->item(lineaactual,7)->setText(qclaveiva3);
            ui.Tablaapuntes->item(lineaactual,8)->setText(qtipoiva3);
            ui.Tablaapuntes->item(lineaactual,9)->setText(qtipore3);
            ui.Tablaapuntes->item(lineaactual,10)->setText(qcuota3);
            ui.Tablaapuntes->item(lineaactual,11)->setText(qctafra);
            QString caddia; caddia.setNum(qfechafact.day());
            QString cadmes; cadmes.setNum(qfechafact.month());
            QString cadyear; cadyear.setNum(qfechafact.year());
            ui.Tablaapuntes->item(lineaactual,12)->setText(caddia);
            ui.Tablaapuntes->item(lineaactual,13)->setText(cadmes);
            ui.Tablaapuntes->item(lineaactual,14)->setText(cadyear);
            ui.Tablaapuntes->item(lineaactual,15)->setText("0");
            ui.Tablaapuntes->item(lineaactual,18)->setText(rectificativa ? "1" : "0");
            ui.Tablaapuntes->item(lineaactual,20)->setText(qfechaop.toString("yyyy-MM-dd"));
            ui.Tablaapuntes->item(lineaactual,21)->setText(qclaveop);
             ui.Tablaapuntes->item(lineaactual,23)->setText(rectificada);
             ui.Tablaapuntes->item(lineaactual,24)->setText(nfacturas);
             ui.Tablaapuntes->item(lineaactual,25)->setText(finicial);
             ui.Tablaapuntes->item(lineaactual,26)->setText(ffinal);
             ui.Tablaapuntes->item(lineaactual,33)->setText(nombre);
             ui.Tablaapuntes->item(lineaactual,34)->setText(cif);
             ui.Tablaapuntes->item(lineaactual,50)->setText(cajaiva ? "1" : "0");
            }
         // --------------------------------------------------------------------------------------------------
        // actualizamos línea siguiente con ctafra 
         lineaactual++;
         for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(lineaactual,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(lineaactual,veces,newItem);
                   }
        ui.Tablaapuntes->item(lineaactual,0)->setText(qctafra);
        ui.Tablaapuntes->item(lineaactual,1)->setText(concepto);
        double vtotal;
        vtotal=convapunto(qbase).toDouble()+convapunto(qcuota).toDouble();
        if (qcuota2.length()>0) vtotal+=convapunto(qbase2).toDouble()+convapunto(qcuota2).toDouble();
        if (qcuota3.length()>0) vtotal+=convapunto(qbase3).toDouble()+convapunto(qcuota3).toDouble();
        ui.Tablaapuntes->item(lineaactual,2)->setText(formateanumero(vtotal,comadecimal,decimales));
        if (qdoc.length()>0)
           {
	     for (int veces=0;veces<=lineaactual;veces++)
               {
                if (concepto_plantilla.isEmpty()) ui.Tablaapuntes->item(veces,1)->setText(concepto+" Fra. "+qdoc);
                ui.Tablaapuntes->item(veces,4)->setText(qdoc);
               }
           }
        ui.FechaApunte->setDate(qfechacont);
       // si botón aceptar activado pasar, si no edición
       chequeatotales();
       if (verifica || !ui.BotonIncorporar->isEnabled()
		|| (fechadejerciciocerrado(ui.FechaApunte->date()))
		|| !fechacorrespondeaejercicio(ui.FechaApunte->date()) || conanalitica()
                || conanalitica_parc()) exec();
         else incorporar();
     }
}

void tabla_asientos::activa_aibautonomo()
{
  esaibautonomo=true;
  esaib=true;
  ui.aibcheckBox->setChecked(true);
}

void tabla_asientos::quitavar_aibautonomo()
{
  esaibautonomo=false;
}

void tabla_asientos::quitavar_eibautonomo()
{
  eseibautonomo=false;
}

void tabla_asientos::activa_eibautonomo()
{
  eseibautonomo=true;
  eseib=true;
  ui.eibcheckBox->setChecked(true);
}

void tabla_asientos::make_linea_autofactura_no_ue(int linea)
{
  ui.Tablaapuntes->item(linea,28)->setText("1");
}


void tabla_asientos::make_linea_prest_servicio_ue(int linea)
{
  ui.Tablaapuntes->item(linea,27)->setText("1");
}


void tabla_asientos::aibautonomo()
{
  esaibautonomo=true;
  preparaaib();

   aib *a = new aib(comadecimal,decimales);

   ivadatosadic *datos = new (ivadatosadic);

   a->hazbaseoblig();

   QString qclaveiva,qtipoiva,qctafra,qctaivasop,
                      qcuentaivarep,qcuentabase,qbase,
                      qcuota,qprorrata,qautofactura,qbicoste,qautofacturanoue,
                      qbinversion, isp_op_interiores, oro_inversion;
   QDate qfechafact,qfechaop;
   QString concepto;


   while (true) {
     int resultado=a->exec();
     if (resultado==QDialog::Rejected)
            {
             delete(a);
             delete(datos);
             return;
            }

     a->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
                &qtipoiva,&qctaivasop, &qcuentaivarep,
                &qcuota,&qctafra,
                &qfechafact,&qprorrata,&qautofactura,&qfechaop,
                &qbicoste,&qautofacturanoue,&qbinversion,
                &isp_op_interiores,&oro_inversion);
     if (a->externo().isEmpty())
         concepto=descripcioncuenta(qctafra);
       else concepto=basedatos::instancia()->razon_externo(a->externo());
     QString pasa_concepto=concepto; // lleva la descripción de la cuenta del proveedor
     if (qautofactura=="1")
       pasa_concepto+=tr(" ISP ")+datos->documento();
     else
       {
        if (qautofacturanoue!="1") pasa_concepto+=tr(" AIB Fra. ")+datos->documento();
       }
     if (qautofacturanoue=="1")
       pasa_concepto+=tr(" ISP no UE ")+datos->documento();
     if (isp_op_interiores=="1")
       pasa_concepto+=tr(" ISP interior ")+datos->documento();
     datos->pasafecha(qfechafact);
     datos->pasaconcepto(pasa_concepto);
     resultado=datos->exec();
     if (resultado==QDialog::Rejected)
       {
        delete(a);
        delete(datos);
        return;
       }
     if (!datos->navegar_anterior()) break;
   }

   ui.externo_lineEdit->setText(a->externo());
   hay_fecha_factura=true;
   fecha_factura=qfechafact;
	     // empezamos por la primera línea = la base imponible
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(0,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(0,veces,newItem);
                   }
	     ui.Tablaapuntes->item(0,0)->setText(qcuentabase);
	     ui.Tablaapuntes->item(0,1)->setText(concepto);
	     ui.Tablaapuntes->item(0,2)->setText(qbase);
             if (es_cuenta_para_analitica(qcuentabase) &&
                 basedatos::instancia()->analitica_tabla())
                 realiza_asignainputdiario(0,qbase);
             // actualizamos línea siguiente con ctafra
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(1,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(1,veces,newItem);
                   }
 	    ui.Tablaapuntes->item(1,0)->setText(qctafra);
	    ui.Tablaapuntes->item(1,1)->setText(concepto);
	    ui.Tablaapuntes->item(1,3)->setText(qbase);
            // comprobar si se trata de operación exenta
            double valor; valor=convapunto(qcuota).toDouble();
            if (valor >-0.0001 and valor <0.0001)
               {
                // aib/autofactura exenta
                int fila=1;
                ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
                ui.Tablaapuntes->item(fila,6)->setText(qbase);
                ui.Tablaapuntes->item(fila,7)->setText("");
                ui.Tablaapuntes->item(fila,8)->setText("0");
                ui.Tablaapuntes->item(fila,9)->setText("");
                ui.Tablaapuntes->item(fila,10)->setText("0");
                ui.Tablaapuntes->item(fila,11)->setText(qctafra);
                QString caddia,cadmes,cadyear;
                caddia.setNum(qfechafact.day());
                cadmes.setNum(qfechafact.month());
                cadyear.setNum(qfechafact.year());
                ui.Tablaapuntes->item(fila,12)->setText(caddia);
                ui.Tablaapuntes->item(fila,13)->setText(cadmes);
                ui.Tablaapuntes->item(fila,14)->setText(cadyear);
                ui.Tablaapuntes->item(fila,15)->setText("AIB0"); // ni 1 ni 0 AIB exenta
                ui.Tablaapuntes->item(fila,19)->setText(qautofactura);
                ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                ui.Tablaapuntes->item(fila,21)->setText(qautofactura=="1"
                                                        || qautofacturanoue=="1"
                                                        || isp_op_interiores=="1" ? "I" : "P");
                ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
                ui.Tablaapuntes->item(fila,28)->setText(qautofacturanoue);
                ui.Tablaapuntes->item(fila,29)->setText(qbinversion);
                ui.Tablaapuntes->item(fila,49)->setText(isp_op_interiores);
                ui.Tablaapuntes->item(fila,53)->setText(oro_inversion);
               }
              else // operación no exenta
             {
             // siguiente línea, la de la cuenta del IVA soportado
            for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(2,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(2,veces,newItem);
                   }
             double valprorrata=1;
             if (qprorrata.length()>0) valprorrata=convapunto(qprorrata).toDouble()/100;
             if (valprorrata>-0.001 && valprorrata<0.001) valprorrata=1;
             ui.Tablaapuntes->item(2,0)->setText(qctaivasop);
             ui.Tablaapuntes->item(2,1)->setText(concepto);
             if (qcuota.contains('-')>0)
               {
                double valcuota=convapunto(qcuota).toDouble()*-1*valprorrata;
                QString cadnum; cadnum.setNum(valcuota,'f',2);
                ui.Tablaapuntes->item(2,3)->setText(cadnum);
               }
              else 
                  {
                   double valcuota=convapunto(qcuota).toDouble()*valprorrata;
                   QString cadnum; cadnum.setNum(valcuota,'f',2);
                   ui.Tablaapuntes->item(2,2)->setText(cadnum);
                  }
	     // actualizamos columnas 5 a 15 y 17
	     ui.Tablaapuntes->item(2,5)->setText(qcuentabase);
	     ui.Tablaapuntes->item(2,6)->setText(qbase);
	     ui.Tablaapuntes->item(2,7)->setText(qclaveiva);
	     ui.Tablaapuntes->item(2,8)->setText(qtipoiva);
	     ui.Tablaapuntes->item(2,9)->setText("");
	     ui.Tablaapuntes->item(2,10)->setText(qcuota);
	     ui.Tablaapuntes->item(2,11)->setText(qctafra);
	     QString caddia; caddia.setNum(qfechafact.day());
	     QString cadmes; cadmes.setNum(qfechafact.month());
	     QString cadyear; cadyear.setNum(qfechafact.year());
	     ui.Tablaapuntes->item(2,12)->setText(caddia);
	     ui.Tablaapuntes->item(2,13)->setText(cadmes);
	     ui.Tablaapuntes->item(2,14)->setText(cadyear);
	     ui.Tablaapuntes->item(2,15)->setText("1");
             ui.Tablaapuntes->item(2,17)->setText(qprorrata);
             ui.Tablaapuntes->item(2,18)->setText("0"); // rectificativa
             ui.Tablaapuntes->item(2,19)->setText(qautofactura);
             ui.Tablaapuntes->item(2,20)->setText(qfechaop.toString("yyyy-MM-dd"));
             ui.Tablaapuntes->item(2,21)->setText(
                     (qautofactura=="1" || qautofacturanoue=="1"
                      || isp_op_interiores=="1") ? "I" : "P");
             ui.Tablaapuntes->item(2,22)->setText(qbicoste);
             ui.Tablaapuntes->item(2,28)->setText(qautofacturanoue);
             ui.Tablaapuntes->item(2,29)->setText(qbinversion);
             ui.Tablaapuntes->item(2,49)->setText(isp_op_interiores);
             ui.Tablaapuntes->item(2,53)->setText(oro_inversion);
             // --------------------------------------------------------------------------------------------------
	     // siguiente línea, la de la cuenta del IVA repercutido
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(3,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(3,veces,newItem);
                   }
	     ui.Tablaapuntes->item(3,0)->setText(qcuentaivarep);
	     ui.Tablaapuntes->item(3,1)->setText(concepto);
         if (qcuota.contains('-'))
            {
             QString valcuota=qcuota;
             ui.Tablaapuntes->item(3,2)->setText(valcuota.remove('-'));
            }
             else
               ui.Tablaapuntes->item(3,3)->setText(qcuota);
	     // actualizamos columnas 5 a 15
	     ui.Tablaapuntes->item(3,5)->setText(qcuentabase);
	     ui.Tablaapuntes->item(3,6)->setText(qbase);
	     ui.Tablaapuntes->item(3,7)->setText(qclaveiva);
	     ui.Tablaapuntes->item(3,8)->setText(qtipoiva);
	     ui.Tablaapuntes->item(3,9)->setText("");
	     ui.Tablaapuntes->item(3,10)->setText(qcuota);
	     ui.Tablaapuntes->item(3,11)->setText(qctafra);
	     caddia.setNum(qfechafact.day());
	     cadmes.setNum(qfechafact.month());
	     cadyear.setNum(qfechafact.year());
	     ui.Tablaapuntes->item(3,12)->setText(caddia);
	     ui.Tablaapuntes->item(3,13)->setText(cadmes);
	     ui.Tablaapuntes->item(3,14)->setText(cadyear);
	     ui.Tablaapuntes->item(3,15)->setText("0");
	     ui.Tablaapuntes->item(3,19)->setText(qautofactura);
             ui.Tablaapuntes->item(3,20)->setText(qfechaop.toString("yyyy-MM-dd"));
             ui.Tablaapuntes->item(3,21)->setText(qautofactura=="1"
                                                  || qautofacturanoue== "1"
                                                  || isp_op_interiores== "1" ? "I" : "P");
             ui.Tablaapuntes->item(3,22)->setText(qbicoste);
             ui.Tablaapuntes->item(3,28)->setText(qautofacturanoue);
             ui.Tablaapuntes->item(3,29)->setText(qbinversion);
             ui.Tablaapuntes->item(3,49)->setText(isp_op_interiores);
             // -----------------------------------------------------------------------------------------------
             // queda ver si hace falta poner la base imponible por la diferencia
             // queda comprobar descuadre y asignar línea con cuenta de gasto/compra si procede
             if (sumadebe()-sumahaber()>0.001 || sumadebe()-sumahaber()<-0.001)
                {
                 int fila=4;
                 for (int veces=0;veces<=COLUMNAS;veces++)
                 if (ui.Tablaapuntes->item(fila,veces)==0)
                    {
                     QTableWidgetItem *newItem = new QTableWidgetItem("");
                     if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                     ui.Tablaapuntes->setItem(fila,veces,newItem);
                    }
                 ui.Tablaapuntes->item(fila,0)->setText(qcuentabase);
                 ui.Tablaapuntes->item(fila,1)->setText(concepto);
                 double diferencia=sumahaber()-sumadebe();
                 QString cadnum; cadnum.setNum(diferencia,'f',2);
                 if (comadecimal) cadnum=convacoma(cadnum);
                 ui.Tablaapuntes->item(fila,2)->setText(cadnum);
                 if (es_cuenta_para_analitica(qcuentabase) &&
                     basedatos::instancia()->analitica_tabla())
                     realiza_asignainputdiario(fila,cadnum);
                }
              } // fin operación no exenta
	     // --------------------------------------------------------------------------------------------------

            //QMessageBox::warning( this, tr("INTERRUPCIÓN"),
            //tr("PUNTO DE CORTE") );

            concepto=datos->recuperaconcepto();
            ui.fichdoclineEdit->setText(datos->fichdocumento());
            // if (datos->documento().length()>0)
	            {
		     for (int veces=0;veces<4;veces++)
		      {
                       if (ui.Tablaapuntes->item(veces,1)!=0
                            && ui.Tablaapuntes->item(veces,4)!=0)
                         {
                          ui.Tablaapuntes->item(veces,1)->setText(concepto);
                          ui.Tablaapuntes->item(veces,4)->setText(datos->documento());
                         }
		      }
	            }
	    ui.FechaApunte->setDate(datos->fechaasiento());
	    // si botón aceptar activado pasar, si no edición
	    chequeatotales();

        bool verificar=datos->esverificar();

        delete(a);
        delete (datos);


        if (verificar || !ui.BotonIncorporar->isEnabled()
		|| (fechadejerciciocerrado(ui.FechaApunte->date()))
                || !fechacorrespondeaejercicio(ui.FechaApunte->date()) || ( (conanalitica()
                || conanalitica_parc()) && !basedatos::instancia()->analitica_tabla())  )
		{
		   // quitar líneas de iva y hacer esaibautonomo falso
		   esaibautonomo=false;
		   for (int veces=0; veces<17; veces++)
		       {
		          ui.Tablaapuntes->item(2,veces)->setText("");
		          ui.Tablaapuntes->item(3,veces)->setText("");
                          if (ui.Tablaapuntes->item(4,veces)!=0)
		             ui.Tablaapuntes->item(4,veces)->setText("");
		       }
		   exec();
	         }
	        else incorporar();



}



void tabla_asientos::eibautonomo()
{
  eseibautonomo=true;
  preparaeib();
  
   eib *e = new eib(comadecimal,decimales);
   e->hazbaseoblig();
   ivadatosadic *datos = new (ivadatosadic);
   QString qcuentabase,qbase,qctafra,qclave,qpreservicios;
   QDate qfechafact,qfechaop;
   QString concepto;
   QString ad;

   while (true) {
    int resultado=e->exec();
    if (resultado==QDialog::Rejected)
            {
              delete e;
              delete datos;
              return;
            }
    e->recuperadatos(&qcuentabase,&qbase,
					&qctafra,
                    &qfechafact,&qfechaop,
                    &qclave,&qpreservicios);
    hay_fecha_factura=true;
    fecha_factura=qfechafact;
    if (e->externo().isEmpty())
        concepto=descripcioncuenta(qctafra);
         else
           concepto=basedatos::instancia()->razon_externo(e->externo());
    ad= tr(" EIB Fra. ");
    if (qpreservicios=="1") ad= tr(" Prest.serv.UE ");
    ad=concepto+ad;

    datos->pasaconcepto(ad);
    datos->pasafecha(qfechafact);
    resultado=datos->exec();
    if (resultado==QDialog::Rejected)
            {
              delete e;
              delete datos;
              return;
            }
    if (!datos->navegar_anterior()) break;

   }

         ui.externo_lineEdit->setText(e->externo());
             // empezamos línea con ctafra
         for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(0,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(0,veces,newItem);
                   }
 	     ui.Tablaapuntes->item(0,0)->setText(qctafra);
	     ui.Tablaapuntes->item(0,1)->setText(concepto);
	     ui.Tablaapuntes->item(0,2)->setText(qbase);
	     // siguiente línea = la base imponible
             for (int veces=0;veces<=COLUMNAS;veces++)
                if (ui.Tablaapuntes->item(1,veces)==0)
                   {
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.Tablaapuntes->setItem(1,veces,newItem);
                   }
	     ui.Tablaapuntes->item(1,0)->setText(qcuentabase);
	     ui.Tablaapuntes->item(1,1)->setText(concepto);
	     ui.Tablaapuntes->item(1,3)->setText(qbase);
             if (es_cuenta_para_analitica(qcuentabase) &&
                 basedatos::instancia()->analitica_tabla())
                 realiza_asignainputdiario(1,qbase);
             // actualizamos columnas 5 a 15
	     ui.Tablaapuntes->item(0,5)->setText(qcuentabase);
	     ui.Tablaapuntes->item(0,6)->setText(qbase);
	     ui.Tablaapuntes->item(0,7)->setText("");
	     ui.Tablaapuntes->item(0,8)->setText("0");
	     ui.Tablaapuntes->item(0,9)->setText("");
	     ui.Tablaapuntes->item(0,10)->setText("0");
	     ui.Tablaapuntes->item(0,11)->setText(qctafra);
	     QString caddia; caddia.setNum(qfechafact.day());
	     QString cadmes; cadmes.setNum(qfechafact.month());
	     QString cadyear; cadyear.setNum(qfechafact.year());
	     ui.Tablaapuntes->item(0,12)->setText(caddia);
	     ui.Tablaapuntes->item(0,13)->setText(cadmes);
	     ui.Tablaapuntes->item(0,14)->setText(cadyear);
	     ui.Tablaapuntes->item(0,15)->setText("0");
             ui.Tablaapuntes->item(0,20)->setText(qfechaop.toString("yyyy-MM-dd"));
             ui.Tablaapuntes->item(0,21)->setText(qclave);
             ui.Tablaapuntes->item(0,27)->setText(qpreservicios);
             // --------------------------------------------------------------------------------------------------
        concepto=datos->recuperaconcepto();
            ui.fichdoclineEdit->setText(datos->fichdocumento());
	    if (datos->documento().length()>0)
	            {
                 for (int veces=0;veces<2;veces++)
                    {
                       ui.Tablaapuntes->item(veces,1)->setText(concepto);
     	               ui.Tablaapuntes->item(veces,4)->setText(datos->documento());
                    }
	            }
	    ui.FechaApunte->setDate(datos->fechaasiento());
	    // si botón aceptar activado pasar, si no edición
	    chequeatotales();
	    if (datos->esverificar() || !ui.BotonIncorporar->isEnabled()
		|| (fechadejerciciocerrado(ui.FechaApunte->date()))
                || !fechacorrespondeaejercicio(ui.FechaApunte->date()) || ((conanalitica()
                || conanalitica_parc()) && !basedatos::instancia()->analitica_tabla()))
           {
            // quitar campos de iva y hacer eseibautonomo falso
            eseibautonomo=false;
            for (int veces=5; veces<16; veces++)  ui.Tablaapuntes->item(0,veces)->setText("");
            exec();
           }
	        else incorporar();

    delete(datos);
    delete(e);

}


void tabla_asientos::actualizaestado()
{
// Escribimos número en LCD de debe
double sumadebe=0;
double valor=0;
bool ok=false;
 for (int veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
	    {
                if (ui.Tablaapuntes->item(veces,0)==0) break;
                valor=0;
                if (ui.Tablaapuntes->item(veces,2)->text().length()>0)
		   valor=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble(&ok);
		if (ok) sumadebe+=valor;
	    }
 int ndecimales=2;
 if (!decimales) ndecimales=0;
 QString cadena=QString( "%1" ).arg(
                               sumadebe, 0, 'f',ndecimales ) ;
 ui.LCDSumadebe->display(cadena);
   

 // Escribimos número en LCD de haber
double sumahaber=0;
 for (int veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
	    {
                if (ui.Tablaapuntes->item(veces,0)==0) break;
                valor=0;
                if (ui.Tablaapuntes->item(veces,3)->text().length()>0)
		   valor=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble(&ok);
		if (ok) sumahaber+=valor;
	    }
 cadena=QString( "%1" ).arg(
                               sumahaber, 0, 'f',ndecimales ) ;
 ui.LCDSumahaber->display(cadena);

 // Escribimos número en LCD de descuadre
 cadena=QString( "%1" ).arg(
                               sumadebe-sumahaber, 0, 'f',ndecimales ) ;

 if (sumadebe-sumahaber>-0.0001)
      descuadreazul();
 else
      descuadrerojo();;

 ui.LCDDescuadre->display(cadena);
 
 if (sumadebe-sumahaber<0.0001 && sumadebe-sumahaber>-0.0001)
   //  && (sumadebe>0.0001 || sumadebe<-0.0001)
   //  && (sumahaber>0.0001 || sumahaber<-0.0001))
    {
     if (ui.Tablaapuntes->item(1,0)==0)
        {
         ui.BotonIncorporar->setEnabled(false);
         return;
        }
        else
            if (ui.Tablaapuntes->item(1,0)->text().length()==0)
               {
                ui.BotonIncorporar->setEnabled(false);
                return;
               }
     for (int veces=0;veces<ui.Tablaapuntes->rowCount();veces++)
         {
           if (ui.Tablaapuntes->item(veces,0)==0) break;
              else
                  if (ui.Tablaapuntes->item(veces,0)->text().length()==0) break;
           if ((ui.Tablaapuntes->item(veces,2)==0) || (ui.Tablaapuntes->item(veces,3)==0))
              {
               ui.BotonIncorporar->setEnabled(false);
               return;
              }
           double valordebe=0;
           if (ui.Tablaapuntes->item(veces,2)->text().length()>0)
           valordebe=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble();
           double valorhaber=0;
           if (ui.Tablaapuntes->item(veces,3)->text().length()>0)
              valorhaber=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble();
           if (valordebe==0 && valorhaber==0 && ui.Tablaapuntes->rowCount()==1)
              {
               ui.BotonIncorporar->setEnabled(false);
               return;
              }
         }
     ui.BotonIncorporar->setEnabled(true);
     // ui.BotonIncorporar->setFocus();
     return;
    }
     else ui.BotonIncorporar->setEnabled(false);
}


bool tabla_asientos::botonincorporaractivado()
{
  if (ui.BotonIncorporar->isEnabled()) return true;
    else return false;
}


void tabla_asientos::fechacambiada()
{
 // chequeatotales();

bool ok;
double sumadebe,sumahaber,valor;
QString cadena;
int veces;
sumadebe=0;
 for (veces=0;veces<ultimafila();veces++)
	    {
		valor=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble(&ok);
		if (ok) sumadebe+=valor;
		}
   

 sumahaber=0;
 for (veces=0;veces<ultimafila();veces++)
	    {
		valor=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble(&ok);
		if (ok) sumahaber+=valor;
		}
 
 
 if (sumadebe-sumahaber<0.0001 && sumadebe-sumahaber>-0.0001
     && (sumadebe>0.0001 || sumadebe<-0.0001)
     && (sumahaber>0.0001 || sumahaber<-0.0001))
    {
     ui.BotonIncorporar->setEnabled(true);
    }
    else ui.BotonIncorporar->setEnabled(false);

  if (fechadejerciciocerrado(ui.FechaApunte->date()) || !fechacorrespondeaejercicio(ui.FechaApunte->date()))
       ui.BotonIncorporar->setEnabled(false);
}

void tabla_asientos::cancelar()
{
    if (modoconsulta) { reject(); return; }

    if (ui.Tablaapuntes->item(0,0)==NULL) {reject(); return;}
    else if (ui.Tablaapuntes->item(0,0)->text().isEmpty()) {reject(); return;}

   if (QMessageBox::question(
            this,
            tr("Tabla de asientos"),
            tr("¿ Desea salir y cancelar la edición ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==0 )
                          reject();
}


double tabla_asientos::sumadebe()
{
 double qsumadebe=0;
 double valor=0;
 for (int veces=0;veces<ultimafila();veces++)
   {
     bool ok;
     valor=convapunto(ui.Tablaapuntes->item(veces,2)->text()).toDouble(&ok);
     if (ok) qsumadebe+=valor;
   }
 return qsumadebe;
}

double tabla_asientos::sumahaber()
{
 double qsumahaber=0;
 double valor=0;
 for (int veces=0;veces<ultimafila();veces++)
    {
      bool ok;
      valor=convapunto(ui.Tablaapuntes->item(veces,3)->text()).toDouble(&ok);
      if (ok) qsumahaber+=valor;
    }
 return qsumahaber;
}

void tabla_asientos::pasacadenaprorrata(QString cad)
{
  gprorrata=cad;
}


bool tabla_asientos::procesa_op_exenta()
{
    // procesado para operaciones exentas en libros de facturas IVA
       exento *e = new exento(comadecimal,decimales);
       // --------------------------------------------------------------------------
       // pasamos datos a plantilla
      int fila=0;
      fila=ultimafila()-1;
      QString concepto;
      if (baseimponibleexento.isEmpty())
       {
        if (fila>0)
          {
            QString qcuentabase,qctafra;
            if (escuentadeingreso(ui.Tablaapuntes->item(fila,0)->text()))
               {
                qcuentabase=ui.Tablaapuntes->item(fila,0)->text();
                qctafra=ui.Tablaapuntes->item(fila-1,0)->text();
                // podríamos forzar aquí libro facturas emitidas
                e->fuerzaemitidas();
               }
               else
                   {
                    qctafra=ui.Tablaapuntes->item(fila,0)->text();
                    qcuentabase=ui.Tablaapuntes->item(fila-1,0)->text();
                   }
            QString qbaseimponible=ui.Tablaapuntes->item(fila,3)->text();
            if (qbaseimponible.length()==0) qbaseimponible="-"+ui.Tablaapuntes->item(fila,2)->text();
            concepto=ui.Tablaapuntes->item(fila,1)->text();
            /* void pasadatos( QString cuentabase, QString baseimponible, QString ctafra,
                                QDate qfechafra, QDate fechaop,
                                    QString claveop, QString rectificativa,QString frectif, QString numfact,
                               QString facini, QString facfinal ); */

            e->pasadatos(qcuentabase,
                        qbaseimponible,
                        qctafra,
                        ui.FechaApunte->date(),ui.FechaApunte->date(),"",
                        "","",
                        "","",
                        "",binversionexento ? "1" : "", opnosujeta ? "1" : "","","",
                        ui.Tablaapuntes->item(fila,46)->text(), // importaciones
                        ui.Tablaapuntes->item(fila,47)->text(), // exportación
                        ui.Tablaapuntes->item(fila,48)->text(), // exenta sin dcho deducción
                         ui.Tablaapuntes->item(fila,52)->text(), // ventas nacionales fuera ue
                         ui.Tablaapuntes->item(fila,49)->text(), // isp op interiores en este caso para emitidas isp
                         registro_donacion, ui.Tablaapuntes->item(fila,56)->text() // DUA
                        );
          }
       }
        else
           {
            e->pasadatos(ctabaseexento,
                        formateanumero(convapunto(baseimponibleexento).toDouble(),comadecimal,decimales),
                        cuentafraexento,
                        fechaexento, fechaopexento, claveopexento,frectificativaexento,
                        frectificadaexento,nfacturasexento,inicialexento, finalexento,
                        binversionexento ? "1" : "", opnosujeta ? "1" : "",nombreexento,cifexento,
                        importacionexento ? "1": "", // importaciones
                        exportacionexento ? "1" : "", // exportación
                        nodeduccionexento ? "1" : "", // exenta no dcho deducción
                         ventas_nacionales_fuera_tac ? "1" : "", // ventas nacionales fuera tac
                         emitidas_int_isp ? "1" : "", // operaciones isp interiores emitidas
                         registro_donacion, dua_ex // DUA
                        );
            if (!ivaexentorecibidas) e->fuerzaemitidas();
           }
      // activar modoconsulta si procede
      if (modoconsulta) e->modoconsulta();
      int resultado=e->exec();
      if (resultado==QDialog::Rejected) {delete(e); return false; }
      if (modoconsulta) {delete(e); return true;}
      // asignamos campos de IVA
      QString qcuentabase,qbase,qctafra, qclaveop;
      QDate qfechafact, qfechaop;
      QString qrectificativa, qfrectif, qnumfact, qfacini,
             qfacfinal, qbinversion, qopnosujeta, nombre, cif;
      QString importacion, exportacion, exenta_no_deduc, venta_fuera_tac, qemitidas_int_sip;
      QString datos_donacion;
      QString dua;
      e->recuperadatos(&qcuentabase,&qbase,
                        &qctafra,&qfechafact, &qfechaop, &qclaveop, &qrectificativa,
                        &qfrectif, &qnumfact, &qfacini, &qfacfinal,
                        &qbinversion, &qopnosujeta, &nombre, &cif,
                        &importacion, &exportacion, &exenta_no_deduc, &venta_fuera_tac,
                         &qemitidas_int_sip, &datos_donacion, &dua);
      hay_fecha_factura=true;
      fecha_factura=qfechafact;
     // hay que encontrar la cuenta de ctafra en la tabla y asignarles los campos del IVA
      int veces=0;
      while (veces<ultimafila())
           { 
            if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) break;
            veces++;
           } 
       if (veces==ultimafila())
           {
             QMessageBox::warning( this, tr("Tabla de apuntes"),
             tr("ERROR: El código de cuenta factura suministrado no existe en la tabla de asientos"));
             delete(e);
             return false;
           }
      fila=veces;
     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
     ui.Tablaapuntes->item(fila,6)->setText(qbase);
     ui.Tablaapuntes->item(fila,7)->setText("");
     ui.Tablaapuntes->item(fila,8)->setText("0");
     ui.Tablaapuntes->item(fila,9)->setText("");
     ui.Tablaapuntes->item(fila,10)->setText("0");
     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
     QString caddia,cadmes,cadyear;
     caddia.setNum(qfechafact.day());
     cadmes.setNum(qfechafact.month());
     cadyear.setNum(qfechafact.year());
     ui.Tablaapuntes->item(fila,12)->setText(caddia);
     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
     if (e->eslibroemitidas()) ui.Tablaapuntes->item(fila,15)->setText("0"); //repercutido
        else ui.Tablaapuntes->item(fila,15)->setText("1"); // soportado
     delete(e);
     ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
     ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
     ui.Tablaapuntes->item(fila,18)->setText(qrectificativa);
     ui.Tablaapuntes->item(fila,23)->setText(qfrectif);
     ui.Tablaapuntes->item(fila,24)->setText(qnumfact);
     ui.Tablaapuntes->item(fila,25)->setText(qfacini);
     ui.Tablaapuntes->item(fila,26)->setText(qfacfinal);
     ui.Tablaapuntes->item(fila,29)->setText(qbinversion);
     ui.Tablaapuntes->item(fila,30)->setText(qopnosujeta);
     ui.Tablaapuntes->item(fila,33)->setText(nombre);
     ui.Tablaapuntes->item(fila,34)->setText(cif);
     ui.Tablaapuntes->item(fila,46)->setText(importacion);
     ui.Tablaapuntes->item(fila,47)->setText(exportacion);
     ui.Tablaapuntes->item(fila,48)->setText(exenta_no_deduc);
     ui.Tablaapuntes->item(fila,49)->setText(qemitidas_int_sip);
     ui.Tablaapuntes->item(fila,52)->setText(venta_fuera_tac);
     ui.Tablaapuntes->item(fila,56)->setText(dua);
     ui.Tablaapuntes->item(fila,59)->setText(datos_donacion);
     // QMessageBox::warning( this, tr("Tabla de apuntes"),
     //   tr("no sujeta ")+qopnosujeta);
     return true;
}

void tabla_asientos::activa_opexenta()
{
 ui.exentacheckBox->setChecked(true);
}


void tabla_asientos::borra_valores_iva(int fila)
{
 for (int veces=5; veces<=15; veces++)
     {
      ui.Tablaapuntes->item(fila,veces)->setText("");
     }

 for (int veces=17; veces<=COLUMNAS; veces++)
     {
      ui.Tablaapuntes->item(fila,veces)->setText("");
     }
}


void tabla_asientos::procesaivaasociado(int fila, int columna,QString cuentabase,
                                        QString cuentaiva, QString tipoiva, QString base)
{
   // si es rectificativa, autofactura, aib, eib habría que salir
   if (ui.aibcheckBox->isChecked()) return;
   if (ui.eibcheckBox->isChecked()) return;
   if (ui.exentacheckBox->isChecked()) return;
   // QMessageBox::warning( this, tr("tabla"), "procesa iva asociado");
   // if (esparaedicion()) return;
   // QMessageBox::warning( this, tr("tabla"), "procesa iva asociado");
   if (escuentadeivasoportado(cuentabase) || escuentadeivarepercutido(cuentabase))
       return;
   // comprobamos que no estamos en la antepenúltima línea y que la próxima línea esté vacía
   if (fila+3>=ui.Tablaapuntes->rowCount()) return;
   if (prorrata_iva()>0.001 && prorrata_iva()<100 && fila+4>=ui.Tablaapuntes->rowCount()) return;
   if (ui.Tablaapuntes->item(fila+1,0)!=NULL)
       if (!ui.Tablaapuntes->item(fila+1,0)->text().isEmpty()) return;

   // cumplimentamos fila+1 y abrimos diálogo IVA
    for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila+1,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila+1,veces,newItem);
             }
   ui.Tablaapuntes->item(fila+1,0)->setText(cuentaiva);

   if (escuentadeivasoportado(cuentaiva))
      {   
       QString cuentaret, ret, tipo_oper_ret;
       bool arrend;
       bool conret=basedatos::instancia()->hayretasociada(cuentabase,&cuentaret,&ret,&arrend,
                                                          &tipo_oper_ret);
       if (!conret)
           conret=basedatos::instancia()->hayretasociada_cta(cuentabase,&cuentaret,&ret,&arrend,
                                                                 &tipo_oper_ret);
       // double tiporet=ret.toDouble()/100;
       //if (!conret) tiporet=0;
        ivasoportado *qivasoportado = new ivasoportado(comadecimal,decimales);
        QString qcuota;
        QDate qfechafact;
        QString qbase;
          if (columna==2) qbase=base;
             else
             {
                if (columna==3 &&
                     base.left(1)!="-")
                  {
                    qbase="-";
                    qbase+=base;
                  } else qbase=base.right(base.length()-1);
            }

       qfechafact=ui.FechaApunte->date();
       // qivasoportado->pasadatos_fin(cuentafin,qbase,cuentaiva,qfechafact,ret);
       qivasoportado->pasatipoiva(tipoiva);
       qivasoportado->pasadatos_base(cuentabase, qbase, cuentaiva,
                                         qfechafact, ret );

       // qivasoportado->pasadatos(cuentabase,qbase,cuentaiva,qcuota,"",qfechafact);
       qivasoportado->fococtafra();
       int resultado=qivasoportado->exec();
       if (resultado==QDialog::Rejected)
          {
            ui.Tablaapuntes->item(fila+1,0)->setText("");
          }
       if (resultado==QDialog::Accepted)
               {
                 // ----
                 QString qclaveiva,qtipoiva,qctafra,soportado,qctaiva,qprorrata,qrectificativa,qclaveop,qbicoste;
                 QDate qfechafact, qfechaop;
                 QString frectif,numfact,facini,facfinal,binversion,afectacion;
                 QString agrario, nombre, cif, import, cajaiva;
                 bool no347;
                 bool arrto_ret, arrto_sin_ret;
                 QString dua;

                 qivasoportado->recuperadatos(&cuentabase,&qbase,&qclaveiva,
                                                &qtipoiva,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qprorrata,
                               &qrectificativa, &qfechaop, &qclaveop,
                               &qbicoste,&frectif,&numfact,
                               &facini, &facfinal, &binversion,&afectacion,
                               &agrario, &nombre, &cif, &import, &cajaiva, &no347,
                                              &arrto_ret, &arrto_sin_ret, &dua);
                 fecha_factura=qfechafact;
                 hay_fecha_factura=true;
                 double valprorrata=0;
                 double valafectacion=0;
                 if (qprorrata.length()>0) valprorrata=convapunto(qprorrata).toDouble()/100;
                 if (afectacion.length()>0) valafectacion=convapunto(afectacion).toDouble()/100;
                 if (valprorrata<0.00001) valprorrata=1;
                 if (valafectacion<0) valafectacion=1;
                 ui.Tablaapuntes->item(fila+1,0)->setText(qctaiva);
                 ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                  // actualizamos cuota en DEBE
                     double valor=convapunto(qcuota).toDouble()*valprorrata*valafectacion;
                     ui.Tablaapuntes->item(fila+1,2)->setText(formateanumero(valor,comadecimal,decimales));
                 // actualizamos columnas 5 a 15 y 17
                     ui.Tablaapuntes->item(fila+1,5)->setText(cuentabase);
                     ui.Tablaapuntes->item(fila+1,6)->setText(qbase);
                     ui.Tablaapuntes->item(fila+1,7)->setText(qclaveiva);
                     ui.Tablaapuntes->item(fila+1,8)->setText(qtipoiva);
                     ui.Tablaapuntes->item(fila+1,9)->setText("");
                     ui.Tablaapuntes->item(fila+1,10)->setText(qcuota);
                     ui.Tablaapuntes->item(fila+1,11)->setText(qctafra);
                     QString caddia; caddia.setNum(qfechafact.day());
                     QString cadmes; cadmes.setNum(qfechafact.month());
                     QString cadyear; cadyear.setNum(qfechafact.year());
                     ui.Tablaapuntes->item(fila+1,12)->setText(caddia);
                     ui.Tablaapuntes->item(fila+1,13)->setText(cadmes);
                     ui.Tablaapuntes->item(fila+1,14)->setText(cadyear);
                     ui.Tablaapuntes->item(fila+1,15)->setText(soportado);
                     ui.Tablaapuntes->item(fila+1,17)->setText(qprorrata);
                     ui.Tablaapuntes->item(fila+1,18)->setText(qrectificativa);
                     ui.Tablaapuntes->item(fila+1,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                     ui.Tablaapuntes->item(fila+1,21)->setText(qclaveop);
                     ui.Tablaapuntes->item(fila+1,22)->setText(qbicoste);
                     ui.Tablaapuntes->item(fila+1,23)->setText(frectif);
                     ui.Tablaapuntes->item(fila+1,24)->setText(numfact);
                     ui.Tablaapuntes->item(fila+1,25)->setText(facini);
                     ui.Tablaapuntes->item(fila+1,26)->setText(facfinal);
                     ui.Tablaapuntes->item(fila+1,29)->setText(binversion);
                     ui.Tablaapuntes->item(fila+1,30)->setText("");
                     ui.Tablaapuntes->item(fila+1,31)->setText(afectacion);
                     ui.Tablaapuntes->item(fila+1,32)->setText(agrario);
                     ui.Tablaapuntes->item(fila+1,33)->setText(nombre);
                     ui.Tablaapuntes->item(fila+1,34)->setText(cif);
                     ui.Tablaapuntes->item(fila+1,46)->setText(import);
                     ui.Tablaapuntes->item(fila+1,50)->setText(cajaiva);
                     ui.Tablaapuntes->item(fila+1,51)->setText(no347 ? "1" : "");
                     ui.Tablaapuntes->item(fila,54)->setText(arrto_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,56)->setText(dua);
                     int ulpos=0;
                     // actualizamos línea siguiente con ctafra si procede
                     int actualizar=1; // para comprobar si existe la cuenta de factura
                     // ****************************************************************************
                     for (int veces=0;veces<fila;veces++)
                         if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) actualizar=0;
                     if (actualizar && ui.Tablaapuntes->item(fila+2,0)==0)
                         {
                           // *************************************************************
                           for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(fila+2,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+2,veces,newItem);
                              }
                         }
                     if (actualizar && ui.Tablaapuntes->item(fila+2,0)->text().length()==0)
                            {
                             ulpos=fila+2;
                             ui.Tablaapuntes->item(fila+2,0)->setText(qctafra);
                             ui.Tablaapuntes->item(fila+2,1)->setText(ui.Tablaapuntes->item(fila+1,1)->text());
                             // falta el importe total no ?
                             double importe=convapunto(qbase).toDouble()+convapunto(qcuota).toDouble()
                                            -convapunto(qivasoportado->valor_retencion()).toDouble();
                             ui.Tablaapuntes->item(fila+2,3)->setText(
                                                       formateanumero(importe,comadecimal,decimales));
                            }
                     if (actualizar && prorrata_iva()>0.001 && prorrata_iva()<100)
                         {
                           // actualizamos diferencia prorrata para cuenta de gasto
                           for (int veces=0;veces<=COLUMNAS;veces++)
                             if (ui.Tablaapuntes->item(fila+3,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+3,veces,newItem);
                              }
                           ulpos=fila+3;
                           ui.Tablaapuntes->item(fila+3,0)->setText(cuentabase);
                           ui.Tablaapuntes->item(fila+3,1)->setText(ui.Tablaapuntes->item(fila+2,1)->text());
                           double valor=convapunto(qcuota).toDouble()-convapunto(qcuota).toDouble()*valprorrata*valafectacion;
                           ui.Tablaapuntes->item(fila+3,2)->setText(formateanumero(valor,comadecimal,decimales));
                         }
                     if (conret) // generamos apunte de retención
                       {
                         ulpos++;
                         for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(ulpos,veces)==0)
                            {
                             QTableWidgetItem *newItem = new QTableWidgetItem("");
                             if (veces==2 || veces==3)
                                newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                             ui.Tablaapuntes->setItem(ulpos,veces,newItem);
                            }
                         QString valret=qivasoportado->valor_retencion();
                         ui.Tablaapuntes->item(ulpos,0)->setText(cuentaret);
                         ui.Tablaapuntes->item(ulpos,1)->setText(ui.Tablaapuntes->item(fila+2,1)->text());
                         double valor=convapunto(valret).toDouble();
                         ui.Tablaapuntes->item(ulpos,3)->setText(formateanumero(valor,comadecimal,decimales));
                         //cuentafin,&cuentaret,&ret,&arrend,
                         //&tipo_oper_ret
                         ui.Tablaapuntes->item(ulpos,35)->setText(qctafra); //cuenta retenido
                         ui.Tablaapuntes->item(ulpos,36)->setText(arrend ? "1" : "0"); //retención de arrendamiento
                         ui.Tablaapuntes->item(ulpos,37)->setText(tipo_oper_ret.left(1)); //clave arrendamiento
                         ui.Tablaapuntes->item(ulpos,38)->setText(qbase); //base percepciones
                         ui.Tablaapuntes->item(ulpos,39)->setText(ret); //tipo retención
                         ui.Tablaapuntes->item(ulpos,40)->setText(valret); //retención
                         ui.Tablaapuntes->item(ulpos,41)->setText(""); //ingreso a cuenta
                         ui.Tablaapuntes->item(ulpos,42)->setText(""); //ingreso a cuenta repercutido
                         ui.Tablaapuntes->item(ulpos,43)->setText(""); //nombre retenido
                         ui.Tablaapuntes->item(ulpos,44)->setText(""); //cif retenido
                         ui.Tablaapuntes->item(ulpos,45)->setText(""); //provincia

                       }
                  }
         delete(qivasoportado);
        }
   if (escuentadeivarepercutido(cuentaiva))
      {
        // estamos en la línea de la base
        ivarepercutido *qivarepercutido = new ivarepercutido(comadecimal,decimales);
        QString qbase;
        QString qcuota;
        QDate qfechafact;
          if (columna==3) qbase=base;
             else
             {
                if (columna==2 &&
                     qbase.left(1)!="-")
                  {
                    qbase="-";
                    qbase+=base;
                  } else qbase=base.right(base.length()-1);
            }
         qfechafact=ui.FechaApunte->date();
       QString qrectificativa=ui.Tablaapuntes->item(fila,18)->text();
       qivarepercutido->pasadatos(cuentabase,qbase,cuentaiva,qcuota,"",
                                  qfechafact,qrectificativa);
       qivarepercutido->pasatipoiva(tipoiva);
       qivarepercutido->fococtafra();
       int resultado=qivarepercutido->exec();
       if (resultado==QDialog::Rejected)
          {
           ui.Tablaapuntes->item(fila+1,columna)->setText("");
          }

       if (resultado==QDialog::Accepted)
               {
                 QString qclaveiva,qtipoiva,qtipore,qctafra,soportado,qctaiva,qrectif,qclaveop,qbicoste;
                 QString frectif,numfact,facini,facfinal, nombre, cif, cajaiva, arrto_ret, arrto_sin_ret;
                 QDate qfechafact,qfechaop;
                 qivarepercutido->recuperadatos(&cuentabase,&qbase,&qclaveiva,
                                                &qtipoiva,&qtipore,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qrectif, &qfechaop, &qclaveop,
                                                &qbicoste,&frectif,&numfact,
                                                &facini, &facfinal, &nombre, &cif, &cajaiva, &arrto_ret, &arrto_sin_ret);
                 fecha_factura=qfechafact;
                 hay_fecha_factura=true;
                 ui.Tablaapuntes->item(fila+1,0)->setText(qctaiva);
                 ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                  // actualizamos cuota en HABER
                  double valor=convapunto(qcuota).toDouble();
                  ui.Tablaapuntes->item(fila+1,3)->setText(formateanumero(valor,comadecimal,decimales));
                  // ***********************************************
                 // actualizamos columnas 5 a 13

                     ui.Tablaapuntes->item(fila+1,5)->setText(cuentabase);
                     ui.Tablaapuntes->item(fila+1,6)->setText(qbase);
                     ui.Tablaapuntes->item(fila+1,7)->setText(qclaveiva);
                     ui.Tablaapuntes->item(fila+1,8)->setText(qtipoiva);
                     ui.Tablaapuntes->item(fila+1,9)->setText(qtipore);
                     ui.Tablaapuntes->item(fila+1,10)->setText(qcuota);
                     ui.Tablaapuntes->item(fila+1,11)->setText(qctafra);
                     QString caddia; caddia.setNum(qfechafact.day());
                     QString cadmes; cadmes.setNum(qfechafact.month());
                     QString cadyear; cadyear.setNum(qfechafact.year());
                     ui.Tablaapuntes->item(fila+1,12)->setText(caddia);
                     ui.Tablaapuntes->item(fila+1,13)->setText(cadmes);
                     ui.Tablaapuntes->item(fila+1,14)->setText(cadyear);
                     ui.Tablaapuntes->item(fila+1,15)->setText(soportado);
                     ui.Tablaapuntes->item(fila+1,18)->setText(qrectif);
                     ui.Tablaapuntes->item(fila+1,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                     ui.Tablaapuntes->item(fila+1,21)->setText(qclaveop);
                     ui.Tablaapuntes->item(fila+1,22)->setText(qbicoste);
                     ui.Tablaapuntes->item(fila,23)->setText(frectif);
                     ui.Tablaapuntes->item(fila,24)->setText(numfact);
                     ui.Tablaapuntes->item(fila,25)->setText(facini);
                     ui.Tablaapuntes->item(fila,26)->setText(facfinal);
                     ui.Tablaapuntes->item(fila,33)->setText(nombre);
                     ui.Tablaapuntes->item(fila,34)->setText(cif);
                     ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
                     ui.Tablaapuntes->item(fila,54)->setText(arrto_ret);
                     ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret);
                     // actualizamos línea siguiente con qctafra si procede
                     int actualizar=1; // para comprobar si existe la cuenta de factura
                     // ****************************************************************************
                     for (int veces=0;veces<fila;veces++)
                         if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) actualizar=0;
                     if (actualizar && ui.Tablaapuntes->item(fila+3,0)==0)
                         {
                           // *************************************************************
                           for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(fila+2,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+2,veces,newItem);
                              }
                         }
                     if (actualizar && ui.Tablaapuntes->item(fila+2,0)->text().length()==0)
                            {
                             ui.Tablaapuntes->item(fila+2,0)->setText(qctafra);
                             ui.Tablaapuntes->item(fila+2,1)->setText(ui.Tablaapuntes->item(fila+1,1)->text());
                             // falta el importe total no ?
                             double importe=convapunto(qbase).toDouble()+convapunto(qcuota).toDouble();
                             ui.Tablaapuntes->item(fila+2,2)->setText(
                                                       formateanumero(importe,comadecimal,decimales));
                            }

                  }
         delete(qivarepercutido);
        }

}

// -------------------------------------------------------------------------------
void tabla_asientos::procesaivaasociado_financ(int fila, int columna,QString cuentafin,
                                        QString cuentaiva, QString tipoiva, QString total,
                                               QString cta_base_iva)
{
   // Procesa iva asociado cuando la cuenta que lo genera no es de gasto/ingreso
   // si es rectificativa, autofactura, aib, eib habría que salir
   if (ui.aibcheckBox->isChecked()) return;
   if (ui.eibcheckBox->isChecked()) return;
   if (ui.exentacheckBox->isChecked()) return;
   // QMessageBox::warning( this, tr("tabla"), "procesa iva asociado");
   // if (esparaedicion()) return;
   // QMessageBox::warning( this, tr("tabla"), "procesa iva asociado");
   if (escuentadeivasoportado(cuentafin) || escuentadeivarepercutido(cuentafin))
       return;
   // comprobamos que no estamos en la antepenúltima línea y que la próxima línea esté vacía
   if (fila+4>=ui.Tablaapuntes->rowCount()) return;
   if (prorrata_iva()>0.001 && prorrata_iva()<100 && fila+4>=ui.Tablaapuntes->rowCount()) return;
   if (ui.Tablaapuntes->item(fila+1,0)!=NULL)
       if (!ui.Tablaapuntes->item(fila+1,0)->text().isEmpty()) return;

   // hay retención asociada ?

   // bool hayretasociada(QString cuenta, QString &cuentaret, QString &ret, bool &arrend,
   //                               QString &tipo_oper_ret);
   QString cuentaret, ret, tipo_oper_ret;
   bool arrend;
   bool conret=false;
   if (!ui.externo_lineEdit->text().isEmpty())
      conret=basedatos::instancia()->hayretasociada_externo(ui.externo_lineEdit->text(),&cuentaret,&ret,&arrend,
                                                      &tipo_oper_ret);

   if (!conret)
      conret=basedatos::instancia()->hayretasociada(cuentafin,&cuentaret,&ret,&arrend,
                                                         &tipo_oper_ret);
   if (!conret)
       conret=basedatos::instancia()->hayretasociada_cta(cuentafin,&cuentaret,&ret,&arrend,
                                                             &tipo_oper_ret);
   // cumplimentamos fila+1 y abrimos diálogo IVA
    for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila+1,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila+1,veces,newItem);
             }
   ui.Tablaapuntes->item(fila+1,0)->setText(cuentaiva);

   if (escuentadeivasoportado(cuentaiva))
      {
        ivasoportado *qivasoportado = new ivasoportado(comadecimal,decimales);
        QString qcuota;
        QDate qfechafact;
        QString qtotal;
        double labase;
          if (columna==3) qtotal=total;
             else
             {
                if (columna==2 &&
                     total.left(1)!="-")
                  {
                    qtotal="-";
                    qtotal+=total;
                  } else qtotal=total.right(total.length()-1);
            }
        double tiporet=ret.toDouble()/100;
        if (!conret) tiporet=0;
        // labase=(1+tiporet)*qtotal.toDouble();
        QString tipo,re;
        if (!basedatos::instancia()->selectTiporetiposivaclave (tipoiva,&tipo,&re)) tipo="0";
        labase=convapunto(qtotal).toDouble()/(1+convapunto(tipo).toDouble()/100-tiporet);
        QString qbase=formateanumero(labase,comadecimal,decimales);
       qfechafact=ui.FechaApunte->date();
       //void ivasoportado::pasadatos_fin( QString ctafra, QString baseimponible,
       //                                  QString qcuentaiva,
       //                                  QDate qfechafra, QString porc_ret );

       qivasoportado->pasatipoiva(tipoiva);

       qivasoportado->pasadatos_fin(cuentafin,qbase,cuentaiva,qfechafact,ret,cta_base_iva);
       qivasoportado->fococtabi();
       int resultado=qivasoportado->exec();
       if (resultado==QDialog::Rejected)
          {
            ui.Tablaapuntes->item(fila+1,0)->setText("");
          }
       if (resultado==QDialog::Accepted)
               {
                 // ----
                 QString qclaveiva,qtipoiva,qctafra,soportado,qctaiva,qprorrata,qrectificativa,qclaveop,qbicoste;
                 QDate qfechafact, qfechaop;
                 QString frectif,numfact,facini,facfinal,binversion,afectacion;
                 QString agrario, nombre, cif;
                 QString cuentabase; QString import, cajaiva;
                 bool no347;
                 bool arrto_ret, arrto_sin_ret;
                 QString dua;
                 qivasoportado->recuperadatos(&cuentabase,&qbase,&qclaveiva,
                                                &qtipoiva,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qprorrata,
                               &qrectificativa, &qfechaop, &qclaveop,
                               &qbicoste,&frectif,&numfact,
                               &facini, &facfinal, &binversion,&afectacion,
                               &agrario, &nombre, &cif, &import, &cajaiva, &no347,
                               &arrto_ret, &arrto_sin_ret, &dua);
                 double valprorrata=0;
                 double valafectacion=0;
                 fecha_factura=qfechafact;
                 hay_fecha_factura=true;
                 if (qprorrata.length()>0) valprorrata=convapunto(qprorrata).toDouble()/100;
                 if (afectacion.length()>0) valafectacion=convapunto(afectacion).toDouble()/100;
                 if (valprorrata<0.00001) valprorrata=1;
                 if (valafectacion<0) valafectacion=1;
                 ui.Tablaapuntes->item(fila+1,0)->setText(qctaiva);
                 ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                  // actualizamos cuota en DEBE
                     double valor=convapunto(qcuota).toDouble()*valprorrata*valafectacion;
                     ui.Tablaapuntes->item(fila+1,2)->setText(formateanumero(valor,comadecimal,decimales));
                 // actualizamos columnas 5 a 15 y 17
                     ui.Tablaapuntes->item(fila+1,5)->setText(cuentabase);
                     ui.Tablaapuntes->item(fila+1,6)->setText(qbase);
                     ui.Tablaapuntes->item(fila+1,7)->setText(qclaveiva);
                     ui.Tablaapuntes->item(fila+1,8)->setText(qtipoiva);
                     ui.Tablaapuntes->item(fila+1,9)->setText("");
                     ui.Tablaapuntes->item(fila+1,10)->setText(qcuota);
                     ui.Tablaapuntes->item(fila+1,11)->setText(qctafra);
                     QString caddia; caddia.setNum(qfechafact.day());
                     QString cadmes; cadmes.setNum(qfechafact.month());
                     QString cadyear; cadyear.setNum(qfechafact.year());
                     ui.Tablaapuntes->item(fila+1,12)->setText(caddia);
                     ui.Tablaapuntes->item(fila+1,13)->setText(cadmes);
                     ui.Tablaapuntes->item(fila+1,14)->setText(cadyear);
                     ui.Tablaapuntes->item(fila+1,15)->setText(soportado);
                     ui.Tablaapuntes->item(fila+1,17)->setText(qprorrata);
                     ui.Tablaapuntes->item(fila+1,18)->setText(qrectificativa);
                     ui.Tablaapuntes->item(fila+1,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                     ui.Tablaapuntes->item(fila+1,21)->setText(qclaveop);
                     ui.Tablaapuntes->item(fila+1,22)->setText(qbicoste);
                     ui.Tablaapuntes->item(fila+1,23)->setText(frectif);
                     ui.Tablaapuntes->item(fila+1,24)->setText(numfact);
                     ui.Tablaapuntes->item(fila+1,25)->setText(facini);
                     ui.Tablaapuntes->item(fila+1,26)->setText(facfinal);
                     ui.Tablaapuntes->item(fila+1,29)->setText(binversion);
                     ui.Tablaapuntes->item(fila+1,30)->setText("");
                     ui.Tablaapuntes->item(fila+1,31)->setText(afectacion);
                     ui.Tablaapuntes->item(fila+1,32)->setText(agrario);
                     ui.Tablaapuntes->item(fila+1,33)->setText(nombre);
                     ui.Tablaapuntes->item(fila+1,34)->setText(cif);
                     ui.Tablaapuntes->item(fila+1,46)->setText(cif);
                     ui.Tablaapuntes->item(fila+1,50)->setText(cajaiva);
                     ui.Tablaapuntes->item(fila+1,51)->setText(no347 ? "1" : "");
                     ui.Tablaapuntes->item(fila,54)->setText(arrto_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,56)->setText(dua);

                     // actualizamos línea siguiente con cta gasto
                     // ****************************************************************************
                     if (ui.Tablaapuntes->item(fila+2,0)==0)
                         {
                           // *************************************************************
                           for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(fila+2,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+2,veces,newItem);
                              }
                         }
                     if (ui.Tablaapuntes->item(fila+2,0)->text().length()==0)
                            {
                             ui.Tablaapuntes->item(fila+2,0)->setText(cuentabase);
                             ui.Tablaapuntes->item(fila+2,1)->setText(ui.Tablaapuntes->item(fila+1,1)->text());
                             // falta el importe total no ?
                             double importe=convapunto(qbase).toDouble();
                             ui.Tablaapuntes->item(fila+2,2)->setText(
                                                       formateanumero(importe,comadecimal,decimales));
                             procesa_celda_analitica(fila+2,2);
                            }
                     int ulpos=fila+2;
                     if (prorrata_iva()>0.001 && prorrata_iva()<100)
                         {
                           // actualizamos diferencia prorrata para cuenta de gasto
                           for (int veces=0;veces<=COLUMNAS;veces++)
                             if (ui.Tablaapuntes->item(fila+3,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+3,veces,newItem);
                              }

                           ui.Tablaapuntes->item(fila+3,0)->setText(cuentabase);
                           ui.Tablaapuntes->item(fila+3,1)->setText(ui.Tablaapuntes->item(fila+2,1)->text());
                           double valor=convapunto(qcuota).toDouble()-convapunto(qcuota).toDouble()*valprorrata*valafectacion;
                           ui.Tablaapuntes->item(fila+3,2)->setText(formateanumero(valor,comadecimal,decimales));
                           procesa_celda_analitica(fila+3,2);
                           ulpos=fila+3;
                         }
                     if (conret) // generamos apunte de retención
                       {
                         ulpos++;
                         for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(ulpos,veces)==0)
                            {
                             QTableWidgetItem *newItem = new QTableWidgetItem("");
                             if (veces==2 || veces==3)
                                newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                             ui.Tablaapuntes->setItem(ulpos,veces,newItem);
                            }
                         QString valret=qivasoportado->valor_retencion();
                         ui.Tablaapuntes->item(ulpos,0)->setText(cuentaret);
                         ui.Tablaapuntes->item(ulpos,1)->setText(ui.Tablaapuntes->item(fila+2,1)->text());
                         double valor=convapunto(valret).toDouble();
                         ui.Tablaapuntes->item(ulpos,3)->setText(formateanumero(valor,comadecimal,decimales));
                         //cuentafin,&cuentaret,&ret,&arrend,
                         //&tipo_oper_ret
                         ui.Tablaapuntes->item(ulpos,35)->setText(cuentafin); //cuenta retenido
                         ui.Tablaapuntes->item(ulpos,36)->setText(arrend ? "1" : "0"); //retención de arrendamiento
                         ui.Tablaapuntes->item(ulpos,37)->setText(tipo_oper_ret.left(1)); //clave arrendamiento
                         ui.Tablaapuntes->item(ulpos,38)->setText(qbase); //base percepciones
                         ui.Tablaapuntes->item(ulpos,39)->setText(ret); //tipo retención
                         ui.Tablaapuntes->item(ulpos,40)->setText(valret); //retención
                         ui.Tablaapuntes->item(ulpos,41)->setText(""); //ingreso a cuenta
                         ui.Tablaapuntes->item(ulpos,42)->setText(""); //ingreso a cuenta repercutido
                         ui.Tablaapuntes->item(ulpos,43)->setText(""); //nombre retenido
                         ui.Tablaapuntes->item(ulpos,44)->setText(""); //cif retenido
                         ui.Tablaapuntes->item(ulpos,45)->setText(""); //provincia

                       }
                  }
         delete(qivasoportado);
        }

   if (escuentadeivarepercutido(cuentaiva))
      {
        // estamos en la línea de la base
        ivarepercutido *qivarepercutido = new ivarepercutido(comadecimal,decimales);
        QString qtotal;
        QString qcuota;
        QDate qfechafact;
          if (columna==2) qtotal=total;
             else
             {
                if (columna==3 &&
                     total.left(1)!="-")
                  {
                    qtotal="-";
                    qtotal+=total;
                  } else qtotal=total.right(total.length()-1);
            }
        QString tipo, re;
        if (!basedatos::instancia()->selectTiporetiposivaclave(tipoiva,&tipo,&re)) tipo="0";
        double labase;
        labase=convapunto(qtotal).toDouble()/(1+(convapunto(tipo).toDouble()+convapunto(re).toDouble())/100);
        QString qbase=formateanumero(labase, comadecimal, decimales);
         qfechafact=ui.FechaApunte->date();
       //QString qrectificativa=ui.Tablaapuntes->item(fila,18)->text();
         qivarepercutido->pasatipoiva(tipoiva);
       qivarepercutido->pasadatos_fin( cuentafin, qbase, cuentaiva,
                                      qfechafact, cta_base_iva);
       qivarepercutido->fococtabi();
       int resultado=qivarepercutido->exec();
       if (resultado==QDialog::Rejected)
          {
           ui.Tablaapuntes->item(fila+1,columna)->setText("");
          }

       if (resultado==QDialog::Accepted)
               {
                 QString qclaveiva,qtipoiva,qtipore,qctafra,soportado,qctaiva,qrectif,qclaveop,qbicoste;
                 QString frectif,numfact,facini,facfinal, nombre, cif, cajaiva, arrto_ret, arrto_sin_ret;
                 QString cuentabase;
                 QDate qfechafact,qfechaop;
                 qivarepercutido->recuperadatos(&cuentabase,&qbase,&qclaveiva,
                                                &qtipoiva,&qtipore,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qrectif, &qfechaop, &qclaveop,
                                                &qbicoste,&frectif,&numfact,
                                                &facini, &facfinal, &nombre, &cif, &cajaiva, &arrto_ret,
                                                &arrto_sin_ret);
                 fecha_factura=qfechafact;
                 hay_fecha_factura=true;
                 ui.Tablaapuntes->item(fila+1,0)->setText(qctaiva);
                 ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                  // actualizamos cuota en HABER
                  double valor=convapunto(qcuota).toDouble();
                  ui.Tablaapuntes->item(fila+1,3)->setText(formateanumero(valor,comadecimal,decimales));
                  // ***********************************************
                 // actualizamos columnas 5 a 13

                     ui.Tablaapuntes->item(fila+1,5)->setText(cuentabase);
                     ui.Tablaapuntes->item(fila+1,6)->setText(qbase);
                     ui.Tablaapuntes->item(fila+1,7)->setText(qclaveiva);
                     ui.Tablaapuntes->item(fila+1,8)->setText(qtipoiva);
                     ui.Tablaapuntes->item(fila+1,9)->setText(qtipore);
                     ui.Tablaapuntes->item(fila+1,10)->setText(qcuota);
                     ui.Tablaapuntes->item(fila+1,11)->setText(qctafra);
                     QString caddia; caddia.setNum(qfechafact.day());
                     QString cadmes; cadmes.setNum(qfechafact.month());
                     QString cadyear; cadyear.setNum(qfechafact.year());
                     ui.Tablaapuntes->item(fila+1,12)->setText(caddia);
                     ui.Tablaapuntes->item(fila+1,13)->setText(cadmes);
                     ui.Tablaapuntes->item(fila+1,14)->setText(cadyear);
                     ui.Tablaapuntes->item(fila+1,15)->setText(soportado);
                     ui.Tablaapuntes->item(fila+1,18)->setText(qrectif);
                     ui.Tablaapuntes->item(fila+1,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                     ui.Tablaapuntes->item(fila+1,21)->setText(qclaveop);
                     ui.Tablaapuntes->item(fila+1,22)->setText(qbicoste);
                     ui.Tablaapuntes->item(fila,23)->setText(frectif);
                     ui.Tablaapuntes->item(fila,24)->setText(numfact);
                     ui.Tablaapuntes->item(fila,25)->setText(facini);
                     ui.Tablaapuntes->item(fila,26)->setText(facfinal);
                     ui.Tablaapuntes->item(fila,33)->setText(nombre);
                     ui.Tablaapuntes->item(fila,34)->setText(cif);
                     ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
                     ui.Tablaapuntes->item(fila,54)->setText(arrto_ret);
                     ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret);
                     // actualizamos línea siguiente con cta ingreso
                     // ****************************************************************************
                     if (ui.Tablaapuntes->item(fila+3,0)==0)
                         {
                           // *************************************************************
                           for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(fila+2,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+2,veces,newItem);
                              }
                         }
                     if (ui.Tablaapuntes->item(fila+2,0)->text().length()==0)
                            {
                             ui.Tablaapuntes->item(fila+2,0)->setText(cuentabase);
                             ui.Tablaapuntes->item(fila+2,1)->setText(ui.Tablaapuntes->item(fila+1,1)->text());
                             // falta el importe de la base
                             double importe=convapunto(qbase).toDouble();
                             ui.Tablaapuntes->item(fila+2,3)->setText(
                                                       formateanumero(importe,comadecimal,decimales));
                             procesa_celda_analitica(fila+2,3);
                            }

                  }
         delete(qivarepercutido);
        }

}


// -------------------------------------------------------------------------------




void tabla_asientos::pasainfoexento(bool qivaexentorecibidas,  QString qctabaseexento,
                    QString qbaseimponibleexento, QString qcuentafraexento, QDate qfechaexento,
                    QDate qfechaopexento, QString qclaveopexento, QString qfrectificativaexento,
                    QString qfrectificadaexento, QString qnfacturasexento, QString qinicialexento,
                    QString qfinalexento, bool  binversion, QString qnombre, QString qcif,
                    bool importacion, bool exportacion, bool nodeduccion, bool ventas_fuera_tac,
                                    bool qemitidas_int_isp, bool donacion, QString clave_donacion,
                                    bool donacion_especie, bool donacion_2ejer, QString comunidad_autonoma,
                                    double porcent_deduc_autonomia, QString dua)
{
 ivaexentorecibidas=qivaexentorecibidas;
 ctabaseexento=qctabaseexento;
 baseimponibleexento=qbaseimponibleexento;
 cuentafraexento=qcuentafraexento;
 fechaexento=qfechaexento;
 fechaopexento=qfechaopexento;
 claveopexento=qclaveopexento;
 frectificativaexento=qfrectificativaexento;
 frectificadaexento=qfrectificadaexento;
 nfacturasexento=qnfacturasexento;
 inicialexento=qinicialexento;
 finalexento=qfinalexento;
 binversionexento=binversion;
 nombreexento=qnombre;
 cifexento=qcif;
 importacionexento=importacion;
 exportacionexento=exportacion;
 nodeduccionexento=nodeduccion;
 ventas_nacionales_fuera_tac=ventas_fuera_tac;
 emitidas_int_isp=qemitidas_int_isp;
 dua_ex=dua;

 if (donacion) {
    QJsonObject reg_donacion;
    reg_donacion.insert("clave",clave_donacion);
    reg_donacion.insert("especie",donacion_especie);
    reg_donacion.insert("recurrente",donacion_2ejer);
    reg_donacion.insert("ca",comunidad_autonoma);
    reg_donacion.insert("deduccion_ca",porcent_deduc_autonomia);
    QJsonDocument doc(reg_donacion);
    registro_donacion=doc.toJson();
    // qDebug() << registro_donacion;
 }

}

void tabla_asientos::activa_exportacionexento()
{
    exportacionexento=true;
}

void tabla_asientos::activa_nodeduccionexento()
{
    nodeduccionexento=true;
}


bool tabla_asientos::procesa_aib(QString documento)
{
      aib *a = new aib(comadecimal,decimales);
      a->esconde_externo();
      if (esautofactura) a->selec_autofactura();
      if (esautofactura_no_ue) a->selec_autofactura_no_ue();
      if (es_isp_op_interiores) a->selec_isp_op_interiores();
      // pasamos datos a plantilla de iva AIB
      int fila=ultimafila()-1;
      QString concepto;
      if (fila>0) concepto=ui.Tablaapuntes->item(fila,1)->text();
      if (fila>0 && aib_autof_cbase.length()==0)
         {
            QString qcuentabase=ui.Tablaapuntes->item(fila-1,0)->text();
            QString qctafra=ui.Tablaapuntes->item(fila,0)->text();
            QString qbaseimponible=ui.Tablaapuntes->item(fila,3)->text();
            QDate qfecha=ui.FechaApunte->date();

            a->pasadatos(qcuentabase,
                        qbaseimponible,
                        cuentadeivasoportado(),
                        cuentadeivarepercutido(),
                        "",
                        qctafra,
                        qfecha,
                        gprorrata);
         }

      if (aib_autof_cbase.length()>0)
               {
                 QString qclaveiva;
                 QString qivasoportado=cuentadeivasoportado();
                 QString qivarepercutido=cuentadeivarepercutido();
                 QString qcuentabase=aib_autof_cbase;
                 QString qbaseimponible=formateanumero(convapunto(aib_autof_bi).toDouble(),comadecimal,decimales);
                 qclaveiva=aib_autof_clave_iva;
                 qivarepercutido=aib_autof_ctaivarep;
                 qivasoportado=aib_autof_ctaivasop;
                 QString qctafra=aib_autof_ctafra;
                 QDate qfecha=aib_autof_fecha_fra;
                 a->pasadatos2( qcuentabase, qbaseimponible, qclaveiva,
                       "", qivasoportado,qivarepercutido, "", qctafra, qfecha,
                       aib_fecha_op, aib_bicoste, aib_binversion, isp_oroinversion);
                }

      int veces=0;
      while (veces<ultimafila())
         {
          if (escuentadeivasoportado(ui.Tablaapuntes->item(veces,0)->text()) ||
              escuentadeivarepercutido(ui.Tablaapuntes->item(veces,0)->text()))
              {
               QMessageBox::warning( this, tr("Tabla de apuntes"),
                              tr("ERROR: No pueden haber cuentas de IVA,"
                                 " éstas se van a generar automáticamente"));
               delete(a);
               return false;
              }
           veces++;
          }
      if (modoconsulta) a->modoconsulta();
      int resultado=a->exec();
      if (resultado==QDialog::Rejected || modoconsulta) { delete(a); return false; }
      // pasamos datos de la plantilla a filas en la tabla
      fila=veces;
      if (fila>ui.Tablaapuntes->rowCount()-3)
              {
               QMessageBox::warning( this, tr("Tabla de apuntes"),
                              tr("ERROR: La tabla de apuntes está demasiado llena"));
               delete(a);
               return false;
              }
      QString qcuentabase,qbase,qclaveiva,qtipoiva,qctaivasop,qctaivarep,qcuota,qctafra,qdoc,qprorrata;
      QString qautofactura, qbicoste,qautofacturanoue, qbieninversion, isp_op_interiores, oro_inversion;
      QDate qfechafact, qfechaop;
      a->recuperadatos(&qcuentabase,&qbase,&qclaveiva,
                        &qtipoiva,&qctaivasop, &qctaivarep,&qcuota,&qctafra,
                        &qfechafact,&qprorrata,&qautofactura,&qfechaop,&qbicoste,
                        &qautofacturanoue, &qbieninversion,
                        &isp_op_interiores, &oro_inversion);
      // comprobar si se trata de operación exenta
      hay_fecha_factura=true;
      fecha_factura=qfechafact;
      double valor; valor=convapunto(qcuota).toDouble();
      if (valor >-0.0001 and valor <0.0001)
         {
          // aib/autofactura exenta
          // hay que encontrar la cuenta de ctafra en la tabla y asignarles los campos del IVA
          //
          int veces=0;
          while (veces<ultimafila())
           {
            if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) break;
            veces++;
           }
          if (veces==ultimafila())
           {
            QMessageBox::warning( this, tr("Tabla de apuntes"),
             tr("ERROR: El código de cuenta factura suministrado no existe en la tabla de asientos"));
            return false;
           }
          fila=veces;
          ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
          ui.Tablaapuntes->item(fila,6)->setText(qbase);
          ui.Tablaapuntes->item(fila,7)->setText("");
          ui.Tablaapuntes->item(fila,8)->setText("0");
          ui.Tablaapuntes->item(fila,9)->setText("");
          ui.Tablaapuntes->item(fila,10)->setText("0");
          ui.Tablaapuntes->item(fila,11)->setText(qctafra);
          QString caddia,cadmes,cadyear;
          caddia.setNum(qfechafact.day());
          cadmes.setNum(qfechafact.month());
          cadyear.setNum(qfechafact.year());
          ui.Tablaapuntes->item(fila,12)->setText(caddia);
          ui.Tablaapuntes->item(fila,13)->setText(cadmes);
          ui.Tablaapuntes->item(fila,14)->setText(cadyear);
          ui.Tablaapuntes->item(fila,15)->setText("AIB0"); // ni 1 ni 0 AIB exenta
          ui.Tablaapuntes->item(fila,19)->setText(qautofactura);
          ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
          ui.Tablaapuntes->item(fila,21)->setText(qautofactura=="1"
                                                  || qautofacturanoue=="1"
                                                  || isp_op_interiores=="1" ? "I" : "P");
          ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
          ui.Tablaapuntes->item(fila,28)->setText(qautofacturanoue);
          ui.Tablaapuntes->item(fila,29)->setText(qbieninversion);
          ui.Tablaapuntes->item(fila,49)->setText(isp_op_interiores);
          ui.Tablaapuntes->item(fila,53)->setText(oro_inversion);
          return true;
         }
      // lo que sigue para operación no exenta
      for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila,veces,newItem);
             }

      ui.Tablaapuntes->item(fila,0)->setText(qctaivasop);
      // actualizamos cuota en DEBE ó HABER
      double prorrata;
      if (qprorrata.length()==0) prorrata=1;
         else prorrata=convapunto(qprorrata).toDouble()/100;
      if (qcuota.contains('-')>0)
         {
          double valor=convapunto(qcuota).toDouble()*prorrata*-1;
          QString cadnum; cadnum.setNum(valor,'f',2);
          if (comadecimal) cadnum=convacoma(cadnum);
          ui.Tablaapuntes->item(fila,3)->setText(cadnum);
         }
     else
         {
          double valor=convapunto(qcuota).toDouble()*prorrata;
          QString cadnum; cadnum.setNum(valor,'f',2);
          if (comadecimal) cadnum=convacoma(cadnum);
          ui.Tablaapuntes->item(fila,2)->setText(cadnum);
         }
     ui.Tablaapuntes->item(fila,4)->setText(documento);
     // actualizamos columnas 5 a 15 y 17
     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
     ui.Tablaapuntes->item(fila,6)->setText(qbase);
     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
     ui.Tablaapuntes->item(fila,9)->setText("");
     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
     QString caddia; caddia.setNum(qfechafact.day());
     QString cadmes; cadmes.setNum(qfechafact.month());
     QString cadyear; cadyear.setNum(qfechafact.year());
     ui.Tablaapuntes->item(fila,12)->setText(caddia);
     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
     ui.Tablaapuntes->item(fila,15)->setText("1"); //soportado
     ui.Tablaapuntes->item(fila,17)->setText(qprorrata);
     ui.Tablaapuntes->item(fila,1)->setText(concepto);
     ui.Tablaapuntes->item(fila,19)->setText(qautofactura);
     ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
     ui.Tablaapuntes->item(fila,21)->setText(qautofactura=="1"
                                             || qautofacturanoue=="1"
                                             || isp_op_interiores=="1" ? "I" : "P");
     ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
     ui.Tablaapuntes->item(fila,28)->setText(qautofacturanoue);
     ui.Tablaapuntes->item(fila,29)->setText(qbieninversion);
     ui.Tablaapuntes->item(fila,49)->setText(isp_op_interiores);
     ui.Tablaapuntes->item(fila,53)->setText(oro_inversion);

     fila++;
      for (int veces=0;veces<=COLUMNAS;veces++)
         if (ui.Tablaapuntes->item(fila,veces)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
              ui.Tablaapuntes->setItem(fila,veces,newItem);
             }

     ui.Tablaapuntes->item(fila,0)->setText(qctaivarep);
      // actualizamos cuota en DEBE ó HABER
     QString colocar=qcuota;
      if (colocar.contains('-')>0)
         ui.Tablaapuntes->item(fila,2)->setText(colocar.remove('-'));
     else
         ui.Tablaapuntes->item(fila,3)->setText(colocar);
     ui.Tablaapuntes->item(fila,4)->setText(documento);
     // actualizamos columnas 5 a 13
     ui.Tablaapuntes->item(fila,5)->setText(qcuentabase);
     ui.Tablaapuntes->item(fila,6)->setText(qbase);
     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
     ui.Tablaapuntes->item(fila,9)->setText("");
     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
     caddia.setNum(qfechafact.day());
     cadmes.setNum(qfechafact.month());
     cadyear.setNum(qfechafact.year());
     ui.Tablaapuntes->item(fila,12)->setText(caddia);
     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
     ui.Tablaapuntes->item(fila,15)->setText("0"); //repercutido
     ui.Tablaapuntes->item(fila,1)->setText(concepto);
     ui.Tablaapuntes->item(fila,19)->setText(qautofactura);
     ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
     ui.Tablaapuntes->item(fila,21)->setText(qautofactura=="1"
                                             || qautofacturanoue=="1"
                                             || isp_op_interiores=="1" ? "I" : "P");
     ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
     ui.Tablaapuntes->item(fila,28)->setText(qautofacturanoue);
     ui.Tablaapuntes->item(fila,29)->setText(qbieninversion);
     ui.Tablaapuntes->item(fila,49)->setText(isp_op_interiores);
     ui.Tablaapuntes->item(fila,53)->setText(oro_inversion);

      delete(a);

     // queda comprobar descuadre y asignar línea con cuenta de gasto/compra si procede
      if (sumadebe()-sumahaber()>0.001 || sumadebe()-sumahaber()<-0.001)
         {
          fila++;
          for (int veces=0;veces<=COLUMNAS;veces++)
            if (ui.Tablaapuntes->item(fila,veces)==0)
              {
               QTableWidgetItem *newItem = new QTableWidgetItem("");
               if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
               ui.Tablaapuntes->setItem(fila,veces,newItem);
              }
           ui.Tablaapuntes->item(fila,0)->setText(qcuentabase);
           ui.Tablaapuntes->item(fila,1)->setText(concepto);
           double diferencia=sumahaber()-sumadebe();
           QString cadnum; cadnum.setNum(diferencia,'f',2);
           if (comadecimal) cadnum=convacoma(cadnum);
           ui.Tablaapuntes->item(fila,2)->setText(cadnum);
           ui.Tablaapuntes->item(fila,4)->setText(documento);
           if (es_cuenta_para_analitica(qcuentabase) &&
               basedatos::instancia()->analitica_tabla())
               realiza_asignainputdiario(fila,cadnum);
           if (!basedatos::instancia()->analitica_tabla() &&
                   (conanalitica() || (conanalitica_parc() && codigo_en_analitica(qcuentabase))) )
              {
                QString cadci;
                for (int veces=0; veces<fila; veces++)
                    {
                      if (!ui.Tablaapuntes->item(veces,16)->text().isEmpty())
                          cadci=ui.Tablaapuntes->item(veces,16)->text();

                    }
                ui.Tablaapuntes->item(fila,16)->setText(cadci);
              }
         }
    return true;
}


void tabla_asientos::pasainfoaib1(QString cbase, QString bi, QString clave_iva,
                                  QString ctaivasop, QString ctafra, QDate fecha_fra,
                                  QDate fecha_op, QString bicoste, bool binversion, bool oro_inversion)
{
 aib_autof_cbase=cbase;
 aib_autof_bi=bi;
 aib_autof_clave_iva=clave_iva;
 aib_autof_ctaivasop=ctaivasop;
 aib_autof_ctafra=ctafra;
 aib_autof_fecha_fra=fecha_fra;
 aib_fecha_op=fecha_op;
 aib_bicoste=bicoste;
 aib_binversion=binversion;
 isp_oroinversion=oro_inversion;
}

void tabla_asientos::pasainfoaib2(QString ctaivarep)
{
 aib_autof_ctaivarep=ctaivarep;
}


void tabla_asientos::pasainfoeib(QString cbase, QString bi,QString cuenta_fra, QDate fecha_fra,
                                 QDate fechaop, QString claveop)
{
  eib_cbase=cbase;
  eib_bi=formateanumero(convapunto(bi).toDouble(),comadecimal,decimales);
  eib_cuenta_fra=cuenta_fra;
  eib_fecha_fra=fecha_fra;
  eib_fechaop=fechaop;
  eib_claveop=claveop;
}


void tabla_asientos::muevearriba()
{
  if (ui.Tablaapuntes->currentRow()==0) return;

  ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
  ui.Tablaapuntes->disconnect(SIGNAL(  currentCellChanged ( int , int, int, int )));
  // guardamos contenidos de la fila superior
  QString cols[COLUMNAS+1];
  int pos=ui.Tablaapuntes->currentRow();
  if (ui.Tablaapuntes->item(pos,0)==0) return;


  for (int veces=0;veces<=COLUMNAS;veces++)
  {
    // QMessageBox::warning( this, tr("tabla"),ui.Tablaapuntes->item(pos-1,veces)->text());
    cols[veces]=ui.Tablaapuntes->item(pos-1,veces)->text();
  }

  // guardamos imputaciones de analitica
  asignainputdiario *guarda_inputacion;
  guarda_inputacion=inputaciones.at(pos-1);

  // asignamos valores a la línea previa
  for (int veces=0;veces<=COLUMNAS;veces++)
     ui.Tablaapuntes->item(pos-1,veces)->setText(
                ui.Tablaapuntes->item(pos,veces)->text());

  bool refrescarinput=inputaciones.at(pos-1)!=NULL || inputaciones.at(pos)!=NULL;
  inputaciones[pos-1]=inputaciones.at(pos);
  if (refrescarinput) marca_ci(pos-1);

  // asignamos valores a la línea siguiente
  for (int veces=0;veces<=COLUMNAS;veces++)
     ui.Tablaapuntes->item(pos,veces)->setText(cols[veces]);
  inputaciones[pos]=guarda_inputacion;
  if (refrescarinput) marca_ci(pos);


  connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
     SLOT(contenidocambiado(int,int )));
  connect(ui.Tablaapuntes,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posceldacambiada ( int, int, int, int  )));

  int fila=pos-1;
  ui.Tablaapuntes->setCurrentCell(fila,0);
}



// *************************************************************************************
void tabla_asientos::mueveabajo()
{
    if (ui.Tablaapuntes->currentRow()>=ui.Tablaapuntes->rowCount()-1) return;
    if (ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow()+1,0)==0) return;
        else if (ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow()+1,0)->text().length()==0) return;
    if (ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow(),0)==0) return;

  // guardamos contenidos de la fila inferior
  QString cols[COLUMNAS+1];

  for (int veces=0;veces<=COLUMNAS;veces++)
    cols[veces]=ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow()+1,veces)->text();
  int pos=ui.Tablaapuntes->currentRow();
  // guardamos imputaciones de analitica
  asignainputdiario *guarda_inputacion;
  guarda_inputacion=inputaciones.at(pos+1);

  ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));
  ui.Tablaapuntes->disconnect(SIGNAL(  currentCellChanged ( int , int, int, int )));

  // asignamos valores a la fila inferior
  for (int veces=0;veces<=COLUMNAS;veces++)
    ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow()+1,veces)->setText(
                ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow(),veces)->text());

  bool refrescarinput=inputaciones.at(pos+1)!=NULL || inputaciones.at(pos)!=NULL;
  inputaciones[pos+1]=inputaciones.at(pos);
  if (refrescarinput) marca_ci(pos+1);


  // asignamos valores (guardados) a la fila en curso
  for (int veces=0;veces<=COLUMNAS;veces++)
    ui.Tablaapuntes->item(ui.Tablaapuntes->currentRow(),veces)->setText(cols[veces]);
  inputaciones[pos]=guarda_inputacion;
  if (refrescarinput) marca_ci(pos);

  connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
     SLOT(contenidocambiado(int,int )));
  connect(ui.Tablaapuntes,SIGNAL(  currentCellChanged ( int , int, int, int )),this,
           SLOT(posceldacambiada ( int, int, int, int  )));

  int fila=ui.Tablaapuntes->currentRow()+1;
  ui.Tablaapuntes->setCurrentCell(fila,0);

}


void tabla_asientos::buscacopiadoc()
{
#ifdef NOMACHINE
    QString rutacarga=rutacargadocs().isEmpty() ? adapta(dirtrabajo()):adapta(rutacargadocs());
    QDir direc(rutacarga);
    if (!direc.exists(rutacarga))
       {
         QMessageBox::warning( this, tr("Cargar documento"),
                               tr("ERROR: la ruta para cargar documentos no es correcta") );
         return;

       }

  directorio *dir = new directorio();
  dir->pasa_directorio(rutacarga);
  if (dir->exec() == QDialog::Accepted)
    {
      ui.fichdoclineEdit->setText(dir->ruta_actual());
    }
  delete(dir);
#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Todos los archivos (*)");
  filtros << tr("Archivos jpg (*.jpg)");
  filtros << tr("Archivos jpg (*.jpeg)");
  filtros << tr("Archivos png (*.png)");
  filtros << tr("Archivos pdf (*.pdf)");
  dialogofich.setNameFilters(filtros);
  if (rutacargadocs().isEmpty())
     dialogofich.setDirectory(adapta(dirtrabajo()));
    else
      dialogofich.setDirectory(adapta(rutacargadocs()));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE COPIA DE DOCUMENTO"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
          ui.fichdoclineEdit->setText(fileNames.at(0));          
          }
     }
#endif
}


void tabla_asientos::visdoc()
{
   if (ui.fichdoclineEdit->text().isEmpty()) return;
   QString url=ui.fichdoclineEdit->text();

   QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

   /*if (!ejecuta(aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())),ui.fichdoclineEdit->text()))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+ui.fichdoclineEdit->text()+tr(" con ")+
                             aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())));*/

}

void tabla_asientos::pasafichdoc(QString cadena)
{
    ui.docasignadolineEdit->setText(cadena);
}


void tabla_asientos::visdocdef()
{

  QString fichdoc=ui.docasignadolineEdit->text();

   if (fichdoc.isEmpty()) return;

   if (basedatos::instancia()->gestor_doc_bd()) {
       QString cadnum;
       if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
           else cadnum=fichdoc.section('-',0,0);
       qlonglong num=cadnum.toLongLong();
       QByteArray outByteArray;
       outByteArray=basedatos::instancia()->documento_bd(num);
       QString url=QStandardPaths::writableLocation(
                   QStandardPaths::TempLocation).append(QDir::separator()).append(fichdoc);
       QFile file(url);
       file.open(QIODevice::WriteOnly);

       //EL BLOB OBTENIDO DE LA CONSULTA LO PASAMOS AL ARCHIVO
       file.write(outByteArray);
       file.close();
       QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
       return;
   }


  fichdoc=expanderutadocfich(fichdoc);
  QDesktopServices::openUrl(QUrl(fichdoc.prepend("file:"),QUrl::TolerantMode));
   /*if (!ejecuta(aplicacionabrirfich(extensionfich(fichdoc)),fichdoc))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+fichdoc+tr(" con ")+
                             aplicacionabrirfich(extensionfich(fichdoc)));*/


}


void tabla_asientos::impridocdef()
{
  QString rutapdf=rutacopiapdf();

  if (rutapdf.isEmpty())
   {
    QString fichdoc=ui.docasignadolineEdit->text();

    if (fichdoc.isEmpty()) return;

    if (basedatos::instancia()->gestor_doc_bd()) {
        QString cadnum;
        if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
            else cadnum=fichdoc.section('-',0,0);
        qlonglong num=cadnum.toLongLong();
        QByteArray outByteArray;
        outByteArray=basedatos::instancia()->documento_bd(num);
        QString url=QStandardPaths::writableLocation(
                    QStandardPaths::TempLocation).append(QDir::separator()).append(fichdoc);
        QFile file(url);
        file.open(QIODevice::WriteOnly);

        //EL BLOB OBTENIDO DE LA CONSULTA LO PASAMOS AL ARCHIVO
        file.write(outByteArray);
        file.close();
        QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
        return;
    }

    fichdoc=expanderutadocfich(fichdoc);

    QDesktopServices::openUrl(QUrl(fichdoc.prepend("file:"),QUrl::TolerantMode));

    /*if (!ejecuta(aplicacionabrirfich(extensionfich(fichdoc)),fichdoc))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+fichdoc+tr(" con ")+
                             aplicacionabrirfich(extensionfich(fichdoc)));*/
    return;
   }

  QString destino=rutapdf;
  destino.append(QDir::separator());
  destino.append(ui.docasignadolineEdit->text());
  // QMessageBox::warning( this, tr("TABLA DE ASIENTOS"), destino);
  // QMessageBox::warning( this, tr("TABLA DE ASIENTOS"), destino);
  QFile fich(adapta(expanderutadocfich(ui.docasignadolineEdit->text())));
  //if (eswindows()) fich.copy(QFile::encodeName(destino));
  //   else fich.copy(destino);
  fich.copy(destino);
}



void tabla_asientos::suprimedocdef()
{
    QString fichdoc=ui.docasignadolineEdit->text();

   if (fichdoc.isEmpty()) return;

   bool gestor_bd=basedatos::instancia()->gestor_doc_bd();

   if (gestor_bd) {
       QString cadnum;
       if (!fichdoc.contains('-')) cadnum=fichdoc.section('.',0,0);
           else cadnum=fichdoc.section('-',0,0);
       qlonglong num=cadnum.toLongLong();
       basedatos::instancia()->borra_doc_gestor(num);
       ui.docasignadolineEdit->clear();
       basedatos::instancia()->borra_docdiario (ui.AsientolineEdit->text(),ejerciciodelafecha(ui.FechaApunte->date()));
       return;
   }

  fichdoc=adapta(expanderutadocfich(fichdoc));
  QFile file(fichdoc);

     if (QMessageBox::question(
            this,
            tr("Tabla de asientos"),
            tr("¿ Desea borrar el fichero asignado del documento ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==1 )
                          return;


   if (!file.remove())
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se ha podido borrar ")+ui.docasignadolineEdit->text());
    else ui.docasignadolineEdit->clear();

 // falta eliminar físicamente del campo del diario
 basedatos::instancia()->borra_docdiario (ui.AsientolineEdit->text(),ejerciciodelafecha(ui.FechaApunte->date()));
}


void tabla_asientos::borraasignaciondoc()
{
    ui.fichdoclineEdit->clear();
}

void tabla_asientos::pasafichprov(QString cadena)
{
  ui.fichdoclineEdit->setText(cadena);
}

void tabla_asientos::selec_nosujeta()
{
    opnosujeta=true;
}

void tabla_asientos::activa_modoconsulta()
{
    modoconsulta=true;
    setWindowTitle(tr("TABLA DE ASIENTOS: MODO CONSULTA"));
    ui.Tablaapuntes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.BotonBorraLinea->setEnabled(false);
    ui.arribapushButton->setEnabled(false);
    ui.abajopushButton->setEnabled(false);
    ui.aibcheckBox->setEnabled(false);
    ui.eibcheckBox->setEnabled(false);
    ui.exentacheckBox->setEnabled(false);
    ui.copiadoccheckBox->setEnabled(false);

    ui.FechaApunte->setReadOnly(true);
    ui.diariocomboBox->setEnabled(false);
    ui.borravisdocpushButton->setEnabled(false);

    ui.fichdocpushButton->setEnabled(false);
    ui.borraasdocpushButton->setEnabled(false);
    ui.fichdoclineEdit->setReadOnly(true);

    ui.BotonBorrar->setEnabled(false);
    ui.pegarpushButton->setEnabled(false);

    ui.IVApushButton->setText(tr("Consulta &IVA"));

    ui.BotonIncorporar->setText(tr("&Anotaciones"));
    ui.editarcipushButton->setText("C&onsulta CI");
    if (ui.aibcheckBox->isChecked() || ui.eibcheckBox->isChecked()
        || ui.exentacheckBox->isChecked())
        ui.BotonIncorporar->setEnabled(true);
     else ui.BotonIncorporar->setEnabled(false);

    ui.busca_externo_pushButton->setEnabled(false);
    ui.externo_lineEdit->setReadOnly(true);
    ui.quita_externo_pushButton->setEnabled(false);
    ui.concepto_sii_lineEdit->setReadOnly(true);
}


tabla_asientos::~tabla_asientos()
{
    int filas=inputaciones.count();
    for (int veces=0; veces<filas; veces++)
      {
       if (inputaciones[veces] != NULL)
        {
           delete (inputaciones[veces]);
           inputaciones[veces]=NULL;
        }
      }
}


void tabla_asientos::marca_ci(int fila)
{
    QLabel  *etiqueta = new QLabel();
    etiqueta->setAlignment(Qt::AlignHCenter);

  if (inputaciones[fila]==NULL)
    {
      etiqueta->setPixmap(QPixmap());
      ui.Tablaapuntes->setCellWidget(fila,16,etiqueta);
      return;
    }

  if (inputaciones[fila]->input_OK())
    {
    // marcar OK en columna 16
    etiqueta->setPixmap(QPixmap(":/iconos/graf/check2.png"));
    ui.Tablaapuntes->setCellWidget(fila,16,etiqueta);
    }
    else
      {
        // marcar cruz en columna 16
        etiqueta->setPixmap(QPixmap(":/iconos/graf/cruz.png"));
        ui.Tablaapuntes->setCellWidget(fila,16,etiqueta);
      }
}

bool tabla_asientos::cuenta_para_analitica(QString codigo)
{
  bool procesar_analitica=false;
  if (conanalitica() && (escuentadegasto(codigo) || escuentadeingreso(codigo)))
     procesar_analitica=true;

  if (conanalitica_parc() && codigo_en_analitica(codigo))
     procesar_analitica=true;
  return procesar_analitica;
}

void tabla_asientos::crea_tabla_ci(int fila)
{
if (basedatos::instancia()->analitica_tabla() &&
    inputaciones[fila]==0)
   {
    asignainputdiario *a = new asignainputdiario(comadecimal,decimales);
    inputaciones[fila]=a;
    inputaciones[fila]->pasacta(ui.Tablaapuntes->item(fila,0)->text());

   }
}

void tabla_asientos::pasainputacion(int fila, QString ci, double total, double porcentaje)
{
// QString CI, QString total, double porcentaje
// OJO !! porcentaje se suministra en tanto por uno
QString cadtotal;
cadtotal.setNum(total,'f',2);
inputaciones[fila]->pasainputacion(ci,
                                   cadtotal,
                                   porcentaje);
}


void tabla_asientos::actualiza_inputacion(int fila, double total, bool modoconsulta)
{
        inputaciones[fila]->actualiza_total();
        inputaciones[fila]->pasatotal(formateanumero(total,comadecimal,decimales));
        if (modoconsulta) inputaciones[fila]->activa_modoconsulta();
}



void tabla_asientos::realiza_asignainputdiario(int fila, QString importe)
{
 // primero investigamos si imputaciones no está vacía
    if (basedatos::instancia()->analitica_tabla() &&
        inputaciones[fila]==0)
       {
        asignainputdiario *a = new asignainputdiario(comadecimal,decimales);
        inputaciones[fila]=a;
       }
  // cargamos  CI y porcentaje correspondiente a la cuenta de la columna 0

  QSqlQuery query = basedatos::instancia()->ci_input(ui.Tablaapuntes->item(fila,0)->text());
  if (query.isActive())
  while (query.next())
     {
       inputaciones[fila]->pasainputacion(query.value(0).toString(),
                                          convapunto(importe),
                                          query.value(1).toDouble());
     }
  inputaciones[fila]->actualiza_total();
  inputaciones[fila]->pasatotal(importe);
  marca_ci(fila);
}


void tabla_asientos::pasalineafactura(int linea)
{
  lineafactura=linea;
}

qlonglong tabla_asientos::qpasefactura()
{
  return pasefactura;
}



void tabla_asientos::boton_retencion_pulsado()
{

   int fila=ui.Tablaapuntes->currentRow();

   if (ui.Tablaapuntes->item(fila,0)!=0)
     {
      if (ui.Tablaapuntes->item(fila,35)->text().isEmpty())
          return;
     }
     else return;

   ui.Tablaapuntes->disconnect(SIGNAL(cellChanged(int,int)));


   retencion *qretencion = new retencion(comadecimal,decimales);

QString qcuentaretencion, cta_retenido;
bool ret_arrendamiento=false;
QString clave, base_percepciones, tipo_ret, retencion, ing_cta, ing_cta_repercutido;
QString nombre_ret, cif_ret, provincia;


   qcuentaretencion=ui.Tablaapuntes->item(fila,0)->text();
   // QMessageBox::warning( this, tr("Tabla de apuntes"),
   //   qcuentaretencion);
   cta_retenido=ui.Tablaapuntes->item(fila,35)->text(); //cuenta retenido
   if (ui.Tablaapuntes->item(fila,36)->text()=="1") ret_arrendamiento=true; //retención de arrendamiento
   clave=ui.Tablaapuntes->item(fila,37)->text(); //clave retención
   base_percepciones=ui.Tablaapuntes->item(fila,38)->text(); //base percepciones
   tipo_ret=ui.Tablaapuntes->item(fila,39)->text(); //tipo retención
   retencion=ui.Tablaapuntes->item(fila,40)->text(); //retención
   ing_cta=ui.Tablaapuntes->item(fila,41)->text(); //ingreso a cuenta
   ing_cta_repercutido=ui.Tablaapuntes->item(fila,42)->text(); //ingreso a cuenta repercutido
   nombre_ret=ui.Tablaapuntes->item(fila,43)->text(); //nombre retenido
   cif_ret=ui.Tablaapuntes->item(fila,44)->text(); //cif retenido
   provincia=ui.Tablaapuntes->item(fila,45)->text(); //provincia

  qretencion->pasadatos_all(qcuentaretencion,
                            cta_retenido, ret_arrendamiento,
                            clave, base_percepciones,
                            tipo_ret, retencion,
                            ing_cta, ing_cta_repercutido,
                            nombre_ret, cif_ret, provincia);

   // activar modo consulta
   if (modoconsulta) qretencion->modoconsulta();
   int resultado=qretencion->exec();
   if (resultado==QDialog::Rejected)
            {
            }


   if (resultado==QDialog::Accepted && !modoconsulta)
           {
             QString cta_retenido, clave, base_percepciones, tipo_ret, retencion, ing_cta;
             QString ing_cta_repercutido, nombre_ret, cif_ret, provincia;
             bool ret_arrendamiento;
             qretencion->recuperadatos(&cta_retenido, &ret_arrendamiento, &clave,
                                            &base_percepciones, &tipo_ret, &retencion,
                                            &ing_cta, &ing_cta_repercutido, &nombre_ret,
                                            &cif_ret, &provincia);                          

             //ui.Tablaapuntes->item(fila,0)->setText(----); no cambia la cuenta de retención
              // actualizamos retención en DEBE ó HABER
             QString numretencion=ing_cta.isEmpty() ? retencion : ing_cta;
             if (numretencion.contains("-"))
               {
                  if (ui.Tablaapuntes->item(fila,2)->text().length() >0)
                    {
                     double valor;
                     bool ok;
                     valor=convapunto(numretencion.remove("-")).toDouble(&ok);
                     if (ok)
                         ui.Tablaapuntes->item(fila,2)->setText(formateanumero(valor,comadecimal,decimales));
                      else ui.Tablaapuntes->item(fila,2)->setText("");
                    }
                 else ui.Tablaapuntes->item(fila,3)->setText(numretencion);
               }
                 else
                      {
                        if (ui.Tablaapuntes->item(fila,3)->text().length()>0)
                            ui.Tablaapuntes->item(fila,3)->setText(numretencion);
                        else { QString pasar="-"; pasar+=numretencion;
                            ui.Tablaapuntes->item(fila,2)->setText(pasar); }
                      }
             // Actualizamos columnas en tabla para retenciones

             ui.Tablaapuntes->item(fila,35)->setText(cta_retenido); //cuenta retenido
             ui.Tablaapuntes->item(fila,36)->setText(ret_arrendamiento ? "1" : "0"); //retención de arrendamiento
             ui.Tablaapuntes->item(fila,37)->setText(clave); //clave arrendamiento
             ui.Tablaapuntes->item(fila,38)->setText(base_percepciones); //base percepciones
             ui.Tablaapuntes->item(fila,39)->setText(tipo_ret); //tipo retención
             ui.Tablaapuntes->item(fila,40)->setText(retencion); //retención
             ui.Tablaapuntes->item(fila,41)->setText(ing_cta); //ingreso a cuenta
             ui.Tablaapuntes->item(fila,42)->setText(ing_cta_repercutido); //ingreso a cuenta repercutido
             ui.Tablaapuntes->item(fila,43)->setText(nombre_ret); //nombre retenido
             ui.Tablaapuntes->item(fila,44)->setText(cif_ret); //cif retenido
             ui.Tablaapuntes->item(fila,45)->setText(provincia); //provincia



            }

   delete(qretencion);

   connect(ui.Tablaapuntes,SIGNAL( cellChanged ( int , int )),this,
      SLOT(contenidocambiado(int,int )));
   actualizaestado();
}


void tabla_asientos::pasa_datos_ref(int fila, QString cta_retenido, bool ret_arrendamiento,
                                      QString clave, QString base_percepciones,
                                      QString tipo_ret, QString retencion,
                                      QString ing_cta, QString ing_cta_repercutido,
                                      QString nombre_ret, QString cif_ret, QString provincia)
{
    ui.Tablaapuntes->item(fila,35)->setText(cta_retenido); //cuenta retenido
    ui.Tablaapuntes->item(fila,36)->setText(ret_arrendamiento ? "1" : "0"); //retención de arrendamiento
    ui.Tablaapuntes->item(fila,37)->setText(clave); //clave arrendamiento
    ui.Tablaapuntes->item(fila,38)->setText(base_percepciones); //base percepciones
    ui.Tablaapuntes->item(fila,39)->setText(tipo_ret); //tipo retención
    ui.Tablaapuntes->item(fila,40)->setText(retencion); //retención
    ui.Tablaapuntes->item(fila,41)->setText(ing_cta); //ingreso a cuenta
    ui.Tablaapuntes->item(fila,42)->setText(ing_cta_repercutido); //ingreso a cuenta repercutido
    ui.Tablaapuntes->item(fila,43)->setText(nombre_ret); //nombre retenido
    ui.Tablaapuntes->item(fila,44)->setText(cif_ret); //cif retenido
    ui.Tablaapuntes->item(fila,45)->setText(provincia); //provincia
}

void tabla_asientos::procesa_ret_apunte(int fila,
                                        int columna,
                                        QString qcuentaretencion,
                                        QString qbase_percepciones,
                                        double tipo_ret,
                                        QString tipo_operacion_ret,
                                        bool ret_arrendamiento)
{

    retencion *qretencion = new retencion(comadecimal,decimales);
    // pasamos cuenta retención, base y retención
   QString qtipo_ret; qtipo_ret.setNum(tipo_ret,'f',2);
   if (comadecimal) qtipo_ret=convacoma(qtipo_ret);
   qretencion->pasadatos2(qcuentaretencion, ret_arrendamiento,
                                 tipo_operacion_ret, qbase_percepciones,
                                 qtipo_ret);
   int resultado=qretencion->exec();
   if (resultado==QDialog::Rejected)
      {
       ui.Tablaapuntes->item(fila,columna)->setText("");
       for (int qveces=5;qveces<=15;qveces++) ui.Tablaapuntes->item(fila,qveces)->setText("");
       for (int qveces=17;qveces<=COLUMNAS;qveces++)
           ui.Tablaapuntes->item(fila,qveces)->setText("");
      }

   if (resultado==QDialog::Accepted)
           {
             ret_procesada=true;
             QString cta_retenido, clave, base_percepciones, tipo_ret, retencion, ing_cta;
             QString ing_cta_repercutido, nombre_ret, cif_ret, provincia;
             bool ret_arrendamiento;
             qretencion->recuperadatos(&cta_retenido, &ret_arrendamiento, &clave,
                                            &base_percepciones, &tipo_ret, &retencion,
                                            &ing_cta, &ing_cta_repercutido, &nombre_ret,
                                            &cif_ret, &provincia);
             //ui.Tablaapuntes->item(fila,0)->setText(----); no cambia la cuenta de retención
              // actualizamos retención en DEBE ó HABER
             QString numretencion=ing_cta.isEmpty() ? retencion : ing_cta;
             ui.Tablaapuntes->item(fila,3)->setText(numretencion);
             // Actualizamos columnas en tabla para retenciones

             ui.Tablaapuntes->item(fila,35)->setText(cta_retenido); //cuenta retenido
             ui.Tablaapuntes->item(fila,36)->setText(ret_arrendamiento ? "1" : "0"); //retención de arrendamiento
             ui.Tablaapuntes->item(fila,37)->setText(clave); //clave arrendamiento
             ui.Tablaapuntes->item(fila,38)->setText(base_percepciones); //base percepciones
             ui.Tablaapuntes->item(fila,39)->setText(tipo_ret); //tipo retención
             ui.Tablaapuntes->item(fila,40)->setText(retencion); //retención
             ui.Tablaapuntes->item(fila,41)->setText(ing_cta); //ingreso a cuenta
             ui.Tablaapuntes->item(fila,42)->setText(ing_cta_repercutido); //ingreso a cuenta repercutido
             ui.Tablaapuntes->item(fila,43)->setText(nombre_ret); //nombre retenido
             ui.Tablaapuntes->item(fila,44)->setText(cif_ret); //cif retenido
             ui.Tablaapuntes->item(fila,45)->setText(provincia); //provincia

             int actualizar=1;
             for (int veces=0;veces<fila;veces++)
                 if (ui.Tablaapuntes->item(veces,0)->text()==cta_retenido) actualizar=0;
             if (actualizar && ui.Tablaapuntes->item(fila+1,0)==0)
                 {
                   // *************************************************************
                   for (int veces=0;veces<=COLUMNAS;veces++)
                   if (ui.Tablaapuntes->item(fila+1,veces)==0)
                      {
                       QTableWidgetItem *newItem = new QTableWidgetItem("");
                       if (veces==2 || veces==3)
                          newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                       ui.Tablaapuntes->setItem(fila+1,veces,newItem);
                      }
                 }
             if (actualizar && ui.Tablaapuntes->item(fila+1,0)->text().length()==0)
                    {
                     ui.Tablaapuntes->item(fila+1,0)->setText(cta_retenido);
                     ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                    }

            }

    delete(qretencion);

}


void tabla_asientos::procesaiva_cta(int fila, int columna,
                                    QString tipoiva)
{
   // si es rectificativa, autofactura, aib, eib habría que salir
   if (ui.aibcheckBox->isChecked()) return;
   if (ui.eibcheckBox->isChecked()) return;
   if (ui.exentacheckBox->isChecked()) return;
   if (ui.regIVAcheckBox->isChecked()) return;
   if (esparaedicion()) return;

   // comprobamos que no estamos en la antepenúltima línea y que la próxima línea esté vacía
   if (fila+3>=ui.Tablaapuntes->rowCount()) return;
   if (prorrata_iva()>0.001 && prorrata_iva()<100 && fila+4>=ui.Tablaapuntes->rowCount()) return;
   if (ui.Tablaapuntes->item(fila+1,0)!=NULL)
       if (!ui.Tablaapuntes->item(fila+1,0)->text().isEmpty()) return;
   // comprobamos que no estemos en la primera línea
   if (fila==0) return;

   QString cuentaiva=ui.Tablaapuntes->item(fila,0)->text();
   QString base;
   QString cuentabase;
   cuentabase=ui.Tablaapuntes->item(fila-1,0)->text();
   if (ui.Tablaapuntes->item(fila-1,2)->text().isEmpty())
      {
       base= ui.Tablaapuntes->item(fila-1,3)->text();
       if (escuentadeivasoportado(cuentaiva))
         {
          // hay que cambiar el signo de la base imponible
          if (base.contains('-')) base.remove('-');
             else base="-"+base.remove(' ');
         }
      }
     else
       {
         base= ui.Tablaapuntes->item(fila-1,2)->text();
         if (escuentadeivarepercutido(cuentaiva))
           {
            // hay que cambiar el signo de la base imponible
            if (base.contains('-')) base.remove('-');
               else base="-"+base.remove(' ');
           }

       }

   // abrimos diálogo IVA ********************************

   if (escuentadeivasoportado(cuentaiva))
      {
        ivasoportado *qivasoportado = new ivasoportado(comadecimal,decimales);
        QString qcuota;
        QDate qfechafact;
        QString qbase=base;
       qfechafact=ui.FechaApunte->date();
       qivasoportado->pasadatos(cuentabase,qbase,cuentaiva,qcuota,"",qfechafact);
       qivasoportado->pasatipoiva(tipoiva);
       qivasoportado->fococtafra();
       int resultado=qivasoportado->exec();
       if (resultado==QDialog::Rejected)
          {
            ui.Tablaapuntes->item(fila,0)->setText("");
          }
       if (resultado==QDialog::Accepted)
               {
                 // ----
                 QString qclaveiva,qtipoiva,qctafra,soportado,qctaiva,qprorrata,qrectificativa,qclaveop,qbicoste;
                 QDate qfechafact, qfechaop;
                 QString frectif,numfact,facini,facfinal,binversion,afectacion;
                 QString agrario, nombre, cif, import, cajaiva;
                 bool no347;
                 bool arrto_ret, arrto_sin_ret;
                 QString dua;

                 qivasoportado->recuperadatos(&cuentabase,&qbase,&qclaveiva,
                                                &qtipoiva,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qprorrata,
                               &qrectificativa, &qfechaop, &qclaveop,
                               &qbicoste,&frectif,&numfact,
                               &facini, &facfinal, &binversion,&afectacion,
                               &agrario, &nombre, &cif, &import, &cajaiva, &no347,
                                              &arrto_ret, &arrto_sin_ret, &dua);
                 double valprorrata=0;
                 double valafectacion=0;
                 if (qprorrata.length()>0) valprorrata=convapunto(qprorrata).toDouble()/100;
                 if (afectacion.length()>0) valafectacion=convapunto(afectacion).toDouble()/100;
                 if (valprorrata<0.00001) valprorrata=1;
                 if (valafectacion<0) valafectacion=1;
                 ui.Tablaapuntes->item(fila,0)->setText(qctaiva);
                 ui.Tablaapuntes->item(fila,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                  // actualizamos cuota en DEBE
                     double valor=convapunto(qcuota).toDouble()*valprorrata*valafectacion;
                     ui.Tablaapuntes->item(fila,2)->setText(formateanumero(valor,comadecimal,decimales));
                 // actualizamos columnas 5 a 15 y 17
                     ui.Tablaapuntes->item(fila,5)->setText(cuentabase);
                     ui.Tablaapuntes->item(fila,6)->setText(qbase);
                     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
                     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
                     ui.Tablaapuntes->item(fila,9)->setText("");
                     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
                     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
                     QString caddia; caddia.setNum(qfechafact.day());
                     QString cadmes; cadmes.setNum(qfechafact.month());
                     QString cadyear; cadyear.setNum(qfechafact.year());
                     ui.Tablaapuntes->item(fila,12)->setText(caddia);
                     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
                     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
                     ui.Tablaapuntes->item(fila,15)->setText(soportado);
                     ui.Tablaapuntes->item(fila,17)->setText(qprorrata);
                     ui.Tablaapuntes->item(fila,18)->setText(qrectificativa);
                     ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                     ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
                     ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
                     ui.Tablaapuntes->item(fila,23)->setText(frectif);
                     ui.Tablaapuntes->item(fila,24)->setText(numfact);
                     ui.Tablaapuntes->item(fila,25)->setText(facini);
                     ui.Tablaapuntes->item(fila,26)->setText(facfinal);
                     ui.Tablaapuntes->item(fila,29)->setText(binversion);
                     ui.Tablaapuntes->item(fila,30)->setText("");
                     ui.Tablaapuntes->item(fila,31)->setText(afectacion);
                     ui.Tablaapuntes->item(fila,32)->setText(agrario);
                     ui.Tablaapuntes->item(fila,33)->setText(nombre);
                     ui.Tablaapuntes->item(fila,34)->setText(cif);
                     ui.Tablaapuntes->item(fila,49)->setText(import);
                     ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
                     ui.Tablaapuntes->item(fila,51)->setText(no347 ? "1" : "");
                     ui.Tablaapuntes->item(fila,54)->setText(arrto_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret ? "1" : "");
                     ui.Tablaapuntes->item(fila,56)->setText(dua);

                     // actualizamos línea siguiente con ctafra si procede
                     int actualizar=1; // para comprobar si existe la cuenta de factura
                     // ****************************************************************************
                     for (int veces=0;veces<fila;veces++)
                         if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) actualizar=0;
                     if (actualizar && ui.Tablaapuntes->item(fila+1,0)==0)
                         {
                           // *************************************************************
                           for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(fila+1,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+1,veces,newItem);
                              }
                         }
                     if (actualizar && ui.Tablaapuntes->item(fila+1,0)->text().length()==0)
                            {
                             ui.Tablaapuntes->item(fila+1,0)->setText(qctafra);
                             ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                             // falta el importe total no ?
                             double importe=convapunto(qbase).toDouble()+convapunto(qcuota).toDouble();
                             ui.Tablaapuntes->item(fila+1,3)->setText(
                                                       formateanumero(importe,comadecimal,decimales));
                            }
                     if (actualizar && prorrata_iva()>0.001 && prorrata_iva()<100)
                         {
                           // actualizamos diferencia prorrata para cuenta de gasto
                           for (int veces=0;veces<=COLUMNAS;veces++)
                             if (ui.Tablaapuntes->item(fila+2,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+2,veces,newItem);
                              }

                           ui.Tablaapuntes->item(fila+2,0)->setText(cuentabase);
                           ui.Tablaapuntes->item(fila+2,1)->setText(ui.Tablaapuntes->item(fila+2,1)->text());
                           double valor=convapunto(qcuota).toDouble()-convapunto(qcuota).toDouble()*valprorrata;
                           ui.Tablaapuntes->item(fila+2,2)->setText(formateanumero(valor,comadecimal,decimales));
                         }
                  }
         delete(qivasoportado);
        }
   if (escuentadeivarepercutido(cuentaiva))
      {
        // estamos en la línea de la base
        ivarepercutido *qivarepercutido = new ivarepercutido(comadecimal,decimales);
        QString qbase=base;
        QString qcuota;
        QDate qfechafact;
         qfechafact=ui.FechaApunte->date();
       QString qrectificativa=ui.Tablaapuntes->item(fila,18)->text();
       qivarepercutido->pasadatos(cuentabase,qbase,cuentaiva,qcuota,"",qfechafact,qrectificativa);
       qivarepercutido->pasatipoiva(tipoiva);
       qivarepercutido->fococtafra();
       int resultado=qivarepercutido->exec();
       if (resultado==QDialog::Rejected)
          {
           ui.Tablaapuntes->item(fila,columna)->setText("");
          }

       if (resultado==QDialog::Accepted)
               {
                 QString qclaveiva,qtipoiva,qtipore,qctafra,soportado,qctaiva,qrectif,qclaveop,qbicoste;
                 QString frectif,numfact,facini,facfinal, nombre, cif, cajaiva, arrto_ret, arrto_sin_ret;
                 QDate qfechafact,qfechaop;
                 qivarepercutido->recuperadatos(&cuentabase,&qbase,&qclaveiva,
                                                &qtipoiva,&qtipore,&qctaiva, &qcuota,&qctafra,
                                                &qfechafact,&soportado,&qrectif, &qfechaop, &qclaveop,
                                                &qbicoste,&frectif,&numfact,
                                                &facini, &facfinal, &nombre, &cif, &cajaiva,
                                                &arrto_ret, &arrto_sin_ret);
                 ui.Tablaapuntes->item(fila,0)->setText(qctaiva);
                 ui.Tablaapuntes->item(fila,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                  // actualizamos cuota en HABER
                  double valor=convapunto(qcuota).toDouble();
                  ui.Tablaapuntes->item(fila,3)->setText(formateanumero(valor,comadecimal,decimales));
                  // ***********************************************
                 // actualizamos columnas 5 a 13

                     ui.Tablaapuntes->item(fila,5)->setText(cuentabase);
                     ui.Tablaapuntes->item(fila,6)->setText(qbase);
                     ui.Tablaapuntes->item(fila,7)->setText(qclaveiva);
                     ui.Tablaapuntes->item(fila,8)->setText(qtipoiva);
                     ui.Tablaapuntes->item(fila,9)->setText(qtipore);
                     ui.Tablaapuntes->item(fila,10)->setText(qcuota);
                     ui.Tablaapuntes->item(fila,11)->setText(qctafra);
                     QString caddia; caddia.setNum(qfechafact.day());
                     QString cadmes; cadmes.setNum(qfechafact.month());
                     QString cadyear; cadyear.setNum(qfechafact.year());
                     ui.Tablaapuntes->item(fila,12)->setText(caddia);
                     ui.Tablaapuntes->item(fila,13)->setText(cadmes);
                     ui.Tablaapuntes->item(fila,14)->setText(cadyear);
                     ui.Tablaapuntes->item(fila,15)->setText(soportado);
                     ui.Tablaapuntes->item(fila,18)->setText(qrectif);
                     ui.Tablaapuntes->item(fila,20)->setText(qfechaop.toString("yyyy-MM-dd"));
                     ui.Tablaapuntes->item(fila,21)->setText(qclaveop);
                     ui.Tablaapuntes->item(fila,22)->setText(qbicoste);
                     ui.Tablaapuntes->item(fila,23)->setText(frectif);
                     ui.Tablaapuntes->item(fila,24)->setText(numfact);
                     ui.Tablaapuntes->item(fila,25)->setText(facini);
                     ui.Tablaapuntes->item(fila,26)->setText(facfinal);
                     ui.Tablaapuntes->item(fila,33)->setText(nombre);
                     ui.Tablaapuntes->item(fila,34)->setText(cif);
                     ui.Tablaapuntes->item(fila,50)->setText(cajaiva);
                     ui.Tablaapuntes->item(fila,54)->setText(arrto_ret);
                     ui.Tablaapuntes->item(fila,55)->setText(arrto_sin_ret);
                     // actualizamos línea siguiente con qctafra si procede
                     int actualizar=1; // para comprobar si existe la cuenta de factura
                     // ****************************************************************************
                     for (int veces=0;veces<fila;veces++)
                         if (ui.Tablaapuntes->item(veces,0)->text()==qctafra) actualizar=0;
                     if (actualizar && ui.Tablaapuntes->item(fila+1,0)==0)
                         {
                           // *************************************************************
                           for (int veces=0;veces<=COLUMNAS;veces++)
                           if (ui.Tablaapuntes->item(fila+1,veces)==0)
                              {
                               QTableWidgetItem *newItem = new QTableWidgetItem("");
                               if (veces==2 || veces==3)
                                  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                               ui.Tablaapuntes->setItem(fila+1,veces,newItem);
                              }
                         }
                     if (actualizar && ui.Tablaapuntes->item(fila+1,0)->text().length()==0)
                            {
                             ui.Tablaapuntes->item(fila+1,0)->setText(qctafra);
                             ui.Tablaapuntes->item(fila+1,1)->setText(ui.Tablaapuntes->item(fila,1)->text());
                             // falta el importe total no ?
                             double importe=convapunto(qbase).toDouble()+convapunto(qcuota).toDouble();
                             ui.Tablaapuntes->item(fila+1,2)->setText(
                                                       formateanumero(importe,comadecimal,decimales));
                            }

                  }
         delete(qivarepercutido);
        }

}

void tabla_asientos::fuerzacontado()
{
  forzarcontado=true;
}

void tabla_asientos::fuerza_sin_vencimientos()
{
  sinvencimientos=true;
}

void tabla_asientos::marca_regul_iva()
{
    ui.regIVAcheckBox->setChecked(true);
}


QString tabla_asientos::ruta_docs()
{
  return directorio_doc;
}


void tabla_asientos::evita_pregunta_venci()
{
  evitar_pregunta_venci=true;
}


void tabla_asientos::pasa_doc(QString doc)
{
  docini=doc;
}

void tabla_asientos::limpiaiva(int fila)
{
    ui.Tablaapuntes->item(fila,5)->setText(QString());
    ui.Tablaapuntes->item(fila,6)->setText(QString());
    ui.Tablaapuntes->item(fila,7)->setText(QString());
    ui.Tablaapuntes->item(fila,8)->setText(QString());
    ui.Tablaapuntes->item(fila,9)->setText("");
    ui.Tablaapuntes->item(fila,10)->setText(QString());
    ui.Tablaapuntes->item(fila,11)->setText(QString());
    ui.Tablaapuntes->item(fila,12)->setText(QString());
    ui.Tablaapuntes->item(fila,13)->setText(QString());
    ui.Tablaapuntes->item(fila,14)->setText(QString());
    ui.Tablaapuntes->item(fila,15)->setText(QString());
    ui.Tablaapuntes->item(fila,17)->setText(QString());
    ui.Tablaapuntes->item(fila,18)->setText(QString());
    ui.Tablaapuntes->item(fila,20)->setText(QString());
    ui.Tablaapuntes->item(fila,21)->setText(QString());
    ui.Tablaapuntes->item(fila,22)->setText(QString());
    ui.Tablaapuntes->item(fila,23)->setText(QString());
    ui.Tablaapuntes->item(fila,24)->setText(QString());
    ui.Tablaapuntes->item(fila,25)->setText(QString());
    ui.Tablaapuntes->item(fila,26)->setText(QString());
    ui.Tablaapuntes->item(fila,27)->setText(QString());
    ui.Tablaapuntes->item(fila,28)->setText(QString());
    ui.Tablaapuntes->item(fila,29)->setText(QString());
    ui.Tablaapuntes->item(fila,30)->setText("");
    ui.Tablaapuntes->item(fila,31)->setText(QString());
    ui.Tablaapuntes->item(fila,32)->setText(QString());
    ui.Tablaapuntes->item(fila,33)->setText(QString());
    ui.Tablaapuntes->item(fila,34)->setText(QString());
    ui.Tablaapuntes->item(fila,46)->setText(QString());
    ui.Tablaapuntes->item(fila,50)->setText(QString());

}


void tabla_asientos::limpiaret(int fila)
{
    ui.Tablaapuntes->item(fila,35)->setText(QString()); //cuenta retenido
    ui.Tablaapuntes->item(fila,36)->setText(QString()); //retención de arrendamiento
    ui.Tablaapuntes->item(fila,37)->setText(QString()); //clave arrendamiento
    ui.Tablaapuntes->item(fila,38)->setText(QString()); //base percepciones
    ui.Tablaapuntes->item(fila,39)->setText(QString()); //tipo retención
    ui.Tablaapuntes->item(fila,40)->setText(QString()); //retención
    ui.Tablaapuntes->item(fila,41)->setText(QString()); //ingreso a cuenta
    ui.Tablaapuntes->item(fila,42)->setText(QString()); //ingreso a cuenta repercutido
    ui.Tablaapuntes->item(fila,43)->setText(QString()); //nombre retenido
    ui.Tablaapuntes->item(fila,44)->setText(QString()); //cif retenido
    ui.Tablaapuntes->item(fila,45)->setText(QString()); //provincia

}

void tabla_asientos::desactiva_avisar_cta_libro_fact_no_diario()
{
 avisar_cta_libro_fact_no_diario=false;
}

void tabla_asientos::buscar_externo()
{
   busca_externo *b = new busca_externo();
   int rdo=b->exec();
   if (rdo==QDialog::Accepted)
     {
       ui.externo_lineEdit->setText(b->codigo_elec());
     }
     else ui.externo_lineEdit->clear();
   delete(b);
}

void tabla_asientos::campo_externo_cambiado()
{
 QString codigo=ui.externo_lineEdit->text();
 if (basedatos::instancia()->existe_externo(codigo))
   {
     ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(codigo));
   }
   else ui.descrip_externo_lineEdit->clear();
}

void tabla_asientos::borra_externo()
{
  ui.externo_lineEdit->clear();
}

void tabla_asientos::ver_externo()
{
    externos *e = new externos();
    e->pasa_codigo(ui.externo_lineEdit->text());
    e->exec();
    delete(e);
}

void tabla_asientos::pasa_externo(QString codigo)
{
  ui.externo_lineEdit->setText(codigo);
}

void tabla_asientos::pasa_concepto_sii(QString codigo)
{
   ui.concepto_sii_lineEdit->setText(codigo);
}

void tabla_asientos::predefinido()
{
    espredefinido=true;
}


void tabla_asientos::imputctas()
{
    buscasubcuenta_input *b = new buscasubcuenta_input();
    b->exec();
    delete(b);
}


void tabla_asientos::fichero_soltado() {
    ui.fichdoclineEdit->setText(etiqueta->nombre_fich());
}
