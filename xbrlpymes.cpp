#include "xbrlpymes.h"
#include "ui_xbrlpymes.h"
#include "basedatos.h"
#include "funciones.h"
#include "calcestado.h"
#include "patrimonio.h"
#include "directorio.h"
#include <QMessageBox>
#include <QFileDialog>

xbrlpymes::xbrlpymes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xbrlpymes)
{
    ui->setupUi(this);

    // tenemos que cargar los combos de los ejercicios
    QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
    QStringList ej1,ej2;
    ej2 << noejercicio();
    if ( query.isActive() ) {
            while ( query.next() )
                {
                  ej1 << query.value(0).toString();
                  ej2 << query.value(0).toString();
                }
            }
    ui->actualcomboBox->addItems(ej1);
    ui->anteriorcomboBox->addItems(ej2);

    QString ejercicio;
    ejercicio=ejerciciodelafecha(QDate::currentDate());
    QDate inicio=inicioejercicio(ejercicio);
    inicio=inicio.addDays(-1);
    ejercicio=ejerciciodelafecha(inicio);
    inicio=inicioejercicio(ejercicio);
    inicio=inicio.addDays(-1);
    QString ejercicioant=ejerciciodelafecha(inicio);

    if (!ejercicio.isEmpty())
       {
        int indice=0;
        while (indice<ui->actualcomboBox->count())
           {
             if (ui->actualcomboBox->itemText(indice)==ejercicio) break;
             indice++;
           }
        if (indice>ui->actualcomboBox->count()) indice--;
        ui->actualcomboBox->setCurrentIndex(indice);
       }

    if (!ejercicioant.isEmpty())
       {
        int indice=0;
        while (indice<ui->anteriorcomboBox->count())
           {
             if (ui->anteriorcomboBox->itemText(indice)==ejercicioant) break;
             indice++;
           }
        if (indice>ui->anteriorcomboBox->count()) indice--;
        ui->anteriorcomboBox->setCurrentIndex(indice);
       }

    connect(ui->procesarpushButton,SIGNAL(clicked()),this,SLOT(gen_archivo()));
    bal_pymes_oficial=tr("BALANCE PYMES OFICIAL");
    pyg_pymes_oficial=tr("CUENTA PYG PYMES OFICIAL");
    ui->procesolabel->setText("");
    ui->progressBar->reset();
    ui->ecpncheckBox->hide();
}

xbrlpymes::~xbrlpymes()
{
    delete ui;
}


QString xbrlpymes::noejercicio()
{
  return tr("- -");
}


QString xbrlpymes::generar_contenido()
{

    // -----------------------------------------------------------------------------------------
     // averiguamos si existen los estados para el depósito
     // -------------------------------------------------------------

     if (!basedatos::instancia()->existe_estado(bal_pymes_oficial))
       {
         QMessageBox::warning( this, tr("Estados Contables"),tr("ERROR, NO EXISTE: ")+
                               bal_pymes_oficial);
         return QString() ;
       }

     if (!basedatos::instancia()->existe_estado(pyg_pymes_oficial))
       {
         QMessageBox::warning( this, tr("Estados Contables"),tr("ERROR, NO EXISTE: ")+
                               pyg_pymes_oficial);
         return QString();
       }

   ui->progressBar->setMaximum(10);

   ui->procesolabel->setText(tr("Generando balance de situación ...."));
   ui->progressBar->setValue(2);
   QApplication::processEvents();

   QString ejactual=ui->actualcomboBox->currentText();
   QString ejanterior=ui->anteriorcomboBox->currentText();

   QDate cierre_actual=finejercicio(ejactual);
   QDate cierre_anterior=finejercicio(ejanterior);
   QDate apertura_actual=inicioejercicio(ejactual);
   QDate apertura_anterior=inicioejercicio(ejanterior);
   QString anyo_cierre_actual; anyo_cierre_actual.setNum(cierre_actual.year());
   QString anyo_cierre_anterior;
   if (ejanterior!=noejercicio()) anyo_cierre_anterior.setNum(cierre_anterior.year());

   QString cad_xml;
   cad_xml="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
/*   "<xbrli:xbrl xmlns:pgc-07-rol=\"http://www.icac.meh.es/es/fr/gaap/pgc07/2010-01-01/roles\""
   " xmlns:pgc-07-ref=\"http://www.icac.meh.es/es/fr/gaap/pgc07/referenceParts/2009-01-01\""
   " xmlns:pgc07cbs-dvs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/comunbase/EstadoTotalCambiosPatrimonioNeto/VariacionSaldo/Dimension/2009-01-01\""
   " xmlns:xbrldt=\"http://xbrl.org/2005/xbrldt\""
   " xmlns:pgc-07-c-ap=\"http://www.icac.meh.es/es/fr/gaap/pgc07/comun-abreviadopymes/2009-01-01\""
   " xmlns:pgc-07-a-pyg=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/PerdidasGanancias/2009-01-01\""
   " xmlns:pgc07p-etcpn=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/patrimonioNeto/B/EstadoTotalCambiosPatrimonioNeto/2009-01-01\""
   " xmlns:pgc07p-d-pn=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/EstadoTotalCambiosPatrimonioNeto/PatrimonioNeto/Dimension/2009-01-01\""
   " xmlns:pgc07cbs-dpn=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/comunbase/EstadoTotalCambiosPatrimonioNeto/PatrimonioNeto/Dimension/2009-01-01\""
   " xmlns:link=\"http://www.xbrl.org/2003/linkbase\""
   " xmlns:pgc07p-d-vs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/EstadoTotalCambiosPatrimonioNeto/VariacionSaldo/Dimension/2009-01-01\""
   " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
   " xmlns:pgc-07-p-bal=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/balance/2009-01-01\""
   " xmlns:iso4217=\"http://www.xbrl.org/2003/iso4217\""
   " xmlns:pgc-07-c-bs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/comun-base/2009-01-01\""
   " xmlns:pgc-07-p=\"http://www.icac.meh.es/es/fr/gaap/pgc07/pymes/2009-01-01\""
   " xmlns:xbrldi=\"http://xbrl.org/2006/xbrldi\""
   " xmlns:xbrli=\"http://www.xbrl.org/2003/instance\""
   " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
 */
 /*  "<xbrli:xbrl xmlns:pgc07pymes=\"http://www.icac.meh.es/es/fr/gaap/pgc07/modelo-pymes/2009-01-01\" xmlns:xbrldi=\"http://xbrl.org/2006/xbrldi\" xmlns:dgi-gen-bas=\"http://www.xbrl.org.es/es/2008/dgi/gp/gen-bas/2011-01-30\" xmlns:pgc07m-roles=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/roles/2009-01-01\" xmlns:pgc07mp-d-sf=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/SituacionFiscal/Dimension/2009-01-01\" xmlns:pgc-07-c-bs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/comun-base/2009-01-01\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:pgc07mp-apdo12=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado12/OperacionesPartesVinculadas/2009-01-01\" xmlns:pgc07mp-apdo3=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado3/AplicacionResultados/2009-01-01\" xmlns:pgc07cbs-dvs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/comunbase/EstadoTotalCambiosPatrimonioNeto/VariacionSaldo/Dimension/2009-01-01\" xmlns:pgc07mp-apdo2=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado2/BasesPresentacionCuentasAnuales/2009-01-01\" xmlns:pgc07mp-apdo4=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado4/NormasRegistroValoracion/2009-01-01\" xmlns:iso4217=\"http://www.xbrl.org/2003/iso4217\" xmlns:pgc-07-ref=\"http://www.icac.meh.es/es/fr/gaap/pgc07/referenceParts/2009-01-01\" xmlns:pgc07cbs-dpn=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/comunbase/EstadoTotalCambiosPatrimonioNeto/PatrimonioNeto/Dimension/2009-01-01\" xmlns:dgi-gen-ex=\"http://www.xbrl.org.es/es/2008/dgi/gp/gen-ex/2011-01-30\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:dgi-rel=\"http://www.xbrl.org.es/es/2008/dgi/gp/rel/2011-01-30\" xmlns:pgc07mp-apdo13=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado13/OtraInformacion/2009-01-01\" xmlns:pgc07mc-apdo0=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/apartado0/IdentificacionGeneral/2009-01-01\" xmlns:pgc07mp-apdo7=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado7/PasivosFinancieros/2009-01-01\" xmlns:dgi-lc-int=\"http://www.xbrl.org.es/es/2008/dgi/gp/lc-int/2011-01-30\" xmlns:pgc07p-etcpn=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/patrimonioNeto/B/EstadoTotalCambiosPatrimonioNeto/2009-01-01\" xmlns:pgc07mp-apdo11=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado11/SubvencionesDonancionesLegados/2009-01-01\" xmlns:pgc07mp-apdo9=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado9/SituacionFiscal/2009-01-01\" xmlns:link=\"http://www.xbrl.org/2003/linkbase\" xmlns:dgi-eco-bas=\"http://www.xbrl.org.es/es/2008/dgi/gp/eco-bas/2011-01-30\" xmlns:dgi-dat-inf=\"http://www.xbrl.org.es/es/2008/dgi/gp/dat-inf/2011-01-30\" xmlns:pgc07d-aplaz=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/AplazamientosPagoPagosRealizadosPendientes/Dimension/2009-01-01\" xmlns:pgc07d-reclsf=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/ReclasificacionInstrumentosFinancieros/Dimension/2009-01-01\" xmlns:pgc07mp-rsm=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/resumen/2009-01-01\" xmlns:pgc07m-d-pv=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/PartesVinculadas/Dimension/2009-01-01\" xmlns:pgc-07-p-pyg=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/PerdidasGanancias/2009-01-01\" xmlns:pgc07mc-bs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/base/2009-01-01\" xmlns:pgc07ma-apdo15=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado15/AplazamientosPago/2009-01-01\" xmlns:pgc07d-plzven=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/PlazoVencimientoInstrumentosFinancieros/Dimension/2009-01-01\" xmlns:pgc07pm=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/ModeloPymes/2009-01-01\" xmlns:pgc07p=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/2009-01-01\" xmlns:pgc-07-p=\"http://www.icac.meh.es/es/fr/gaap/pgc07/pymes/2009-01-01\" xmlns:pgc07mp-d-inm=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/InmovilizadoMaterialIntangibleInversionesInmobiliarias/Dimension/2009-01-01\" xmlns:pgc-07-types=\"http://www.icac.meh.es/es/fr/gaap/pgc07/types/2009-01-01\" xmlns:xbrli=\"http://www.xbrl.org/2003/instance\" xmlns:pgc07mp-apdo5=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado5/InmovilizadoMaterialIntangibleInversionesInmobiliarias/2009-01-01\" xmlns:dgi-est-gen=\"http://www.xbrl.org.es/es/2008/dgi/gp/est-gen/2011-01-30\" xmlns:pgc07mp-apdo0=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado0/IdentificacionGeneral/2009-01-01\" xmlns:pgc07mc-ap=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/abreviadoPymes/2009-01-01\" xmlns:pgc07d-clase=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/ClasesInstrumentosFinancieros/Dimension/2009-01-01\" xmlns:pgc07m-d-cp=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/CategoriasProfesionales/Dimension/2009-01-01\" xmlns:pgc07mp-apdo8=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado8/FondosPropios/2009-01-01\" xmlns:pgc-07-roles=\"http://www.icac.meh.es/es/fr/gaap/pgc07/roles/2009-01-01\" xmlns:pgc07p-d-vs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/EstadoTotalCambiosPatrimonioNeto/VariacionSaldo/Dimension/2009-01-01\" xmlns:pgc07p-d-pn=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/EstadoTotalCambiosPatrimonioNeto/PatrimonioNeto/Dimension/2009-01-01\" xmlns:dgi-lc-es=\"http://www.xbrl.org.es/es/2008/dgi/gp/lc-es/2011-01-30\" xmlns:xbrldt=\"http://xbrl.org/2005/xbrldt\" xmlns:dgi-types=\"http://www.xbrl.org.es/es/2008/dgi/gp/types/2011-01-30\" xmlns:pgc07mc-na=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/normalAbreviado/2009-01-01\" xmlns:pgc07mp-apdo14=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado14/InformacionMedioAmbiente/2009-01-01\" xmlns:pgc-07-p-bal=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/balance/2009-01-01\" xmlns:dgi-cnae-09=\"http://www.xbrl.org.es/es/2008/dgi/gp/lc-cnae-2009/2011-01-30\" xmlns:ref=\"http://www.xbrl.org/2004/ref\" xmlns:pgc-07-c-ap=\"http://www.icac.meh.es/es/fr/gaap/pgc07/comun-abreviadopymes/2009-01-01\" xmlns:pgc07mp-apdo10=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado10/IngresosGastos/2009-01-01\" xmlns:pgc07mp-apdo6=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado6/ActivosFinancieros/2009-01-01\" xmlns:pgc07mp=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/2009-01-01\">\n"
   " <link:schemaRef xlink:type=\"simple\" xlink:href=\"http://www.icac.meh.es/taxonomia/pgc-2010-01-01/pgc07-pymes-completo.xsd\"/>\n";
*/
   "<xbrli:xbrl xmlns:pgc07p=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/2018-01-01\" xmlns:pgc07mp=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/2018-01-01\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:pgc07m-roles=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/roles/2018-01-01\" xmlns:pgc-07-c-bs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/comun-base/2018-01-01\" xmlns:iso4217=\"http://www.xbrl.org/2003/iso4217\" xmlns:dgi-gen-ex=\"http://www.xbrl.org.es/es/2008/dgi/gp/gen-ex/2016-01-01\" xmlns:pgc-07-ref=\"http://www.icac.meh.es/es/fr/gaap/pgc07/referenceParts/2018-01-01\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:dgi-rel=\"http://www.xbrl.org.es/es/2008/dgi/gp/rel/2016-01-01\" xmlns:dgi-lc-int=\"http://www.xbrl.org.es/es/2008/dgi/gp/lc-int/2016-01-01\" xmlns:pgc07mc-apdo0=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/apartado0/IdentificacionGeneral/2018-01-01\" xmlns:link=\"http://www.xbrl.org/2003/linkbase\" xmlns:dgi-eco-bas=\"http://www.xbrl.org.es/es/2008/dgi/gp/eco-bas/2016-01-01\" xmlns:dgi-dat-inf=\"http://www.xbrl.org.es/es/2008/dgi/gp/dat-inf/2016-01-01\" xmlns:pgc-07-p-pyg=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/PerdidasGanancias/2018-01-01\" xmlns:pgc07d-reclsf=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/ReclasificacionInstrumentosFinancieros/Dimension/2018-01-01\" xmlns:pgc07mc-bs=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/base/2018-01-01\" xmlns:pgc07m-d-pv=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/PartesVinculadas/Dimension/2018-01-01\" xmlns:dgi-est-gen=\"http://www.xbrl.org.es/es/2008/dgi/gp/est-gen/2016-01-01\" xmlns:xbrli=\"http://www.xbrl.org/2003/instance\" xmlns:pgc07d-plzven=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/PlazoVencimientoInstrumentosFinancieros/Dimension/2018-01-01\" xmlns:pgc-07-p=\"http://www.icac.meh.es/es/fr/gaap/pgc07/pymes/2018-01-01\" xmlns:dgi-lc-es=\"http://www.xbrl.org.es/es/2008/dgi/gp/lc-es/2016-01-01\" xmlns:pgc07mp-apdo0=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/pymes/apartado0/IdentificacionGeneral/2018-01-01\" xmlns:pgc07mc-ap=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/abreviadoPymes/2018-01-01\" xmlns:pgc-07-types=\"http://www.icac.meh.es/es/fr/gaap/pgc07/types/2018-01-01\" xmlns:pgc07d-clase=\"http://www.icac.meh.es/es/fr/gaap/pgc07/memoria/comun/ClasesInstrumentosFinancieros/Dimension/2018-01-01\" xmlns:dgi-types=\"http://www.xbrl.org.es/es/2008/dgi/gp/types/2016-01-01\" xmlns:xbrldt=\"http://xbrl.org/2005/xbrldt\" xmlns:pgc-07-roles=\"http://www.icac.meh.es/es/fr/gaap/pgc07/roles/2018-01-01\" xmlns:dgi-cnae-09=\"http://www.xbrl.org.es/es/2008/dgi/gp/lc-cnae-2009/2016-01-01\" xmlns:ref=\"http://www.xbrl.org/2004/ref\" xmlns:pgc-07-p-bal=\"http://www.icac.meh.es/es/fr/gaap/pgc07/cuentas/pymes/balance/2018-01-01\" xmlns:dgi-gen-bas=\"http://www.xbrl.org.es/es/2008/dgi/gp/gen-bas/2016-01-01\" xmlns:pgc-07-c-ap=\"http://www.icac.meh.es/es/fr/gaap/pgc07/comun-abreviadopymes/2018-01-01\">"
   "<link:schemaRef xlink:type=\"simple\" xlink:href=\"http://www.icac.meh.es/taxonomia/pgc-2018-01-01/pgc07-pymes.xsd\" />";

   QString entity="  <xbrli:entity>\n";
   entity+=       "    <xbrli:identifier scheme=\"http://www.icac.meh.es/xbrl\">";
   entity+=basedatos::instancia()->cif();
   entity+="</xbrli:identifier>\n";
   entity+=       "  </xbrli:entity>\n";

   QString abrecontex="<xbrli:context id=\"%1\">\n";
   QString cierracontex="</xbrli:context>\n";

   QString period="  <xbrli:period>\n"
                  "    <xbrli:instant>%1</xbrli:instant>\n"
                  "  </xbrli:period>\n";


   cad_xml+=abrecontex.arg("Y1_"+anyo_cierre_actual+"_Balance");
   cad_xml+=entity;
   cad_xml+=period.arg(cierre_actual.toString("yyyy-MM-dd"));
   cad_xml+=cierracontex;

   if (!anyo_cierre_anterior.isEmpty())
     {
      cad_xml+=abrecontex.arg("Y1_"+anyo_cierre_anterior+"_Balance");
      cad_xml+=entity;
      cad_xml+=period.arg(cierre_anterior.toString("yyyy-MM-dd"));
      cad_xml+=cierracontex;
     }

   QString period2="  <xbrli:period>\n"
                   "    <xbrli:startDate>%1</xbrli:startDate>\n"
                   "    <xbrli:endDate>%2</xbrli:endDate>\n"
                   "  </xbrli:period>\n";

   cad_xml+=abrecontex.arg("Y1_"+anyo_cierre_actual+"_PYG");
   cad_xml+=entity;
   cad_xml+=period2.arg(apertura_actual.toString("yyyy-MM-dd"),cierre_actual.toString("yyyy-MM-dd"));
   cad_xml+=cierracontex;

 if (!anyo_cierre_anterior.isEmpty())
   {
    cad_xml+=abrecontex.arg("Y1_"+anyo_cierre_anterior+"_PYG");
    cad_xml+=entity;
    cad_xml+=period2.arg(apertura_anterior.toString("yyyy-MM-dd"),cierre_anterior.toString("yyyy-MM-dd"));
    cad_xml+=cierracontex;
   }

/*
  <xbrli:context id="Y1_2010_etcpn_Duration_CambiosPatrimonioNetoPrimaEmision">
    <xbrli:entity>
      <xbrli:identifier scheme="http://www.bde.es/xbrl/test">Comercial Textil Óleo, S.A.</xbrli:identifier>
    </xbrli:entity>
    <xbrli:period>
      <xbrli:startDate>2010-01-01</xbrli:startDate>
      <xbrli:endDate>2010-12-31</xbrli:endDate>
    </xbrli:period>
    <xbrli:scenario>
      <xbrldi:explicitMember dimension="pgc07cbs-dpn:CambiosPatrimonioNetoDimension">pgc07cbs-dpn:CambiosPatrimonioNetoPrimaEmision</xbrldi:explicitMember>
    </xbrli:scenario>
  </xbrli:context>

*/
  if (ui->ecpncheckBox->isChecked())
   cad_xml+=dimensiones_ecpn(anyo_cierre_actual, anyo_cierre_anterior,
                             apertura_actual, cierre_actual,
                             apertura_anterior, cierre_anterior);

  cad_xml+="<xbrli:unit id=\"euro\">\n";
  cad_xml+="  <xbrli:measure>iso4217:EUR</xbrli:measure>\n";
  cad_xml+="</xbrli:unit>\n";

  // ---------------------------------------------------------------------------------------
  cad_xml+="<pgc-07-c-bs:BalanceSituacionVariable decimals=\"0\" contextRef=\"Y1_";
  cad_xml+=anyo_cierre_actual;
  cad_xml+="_Balance\" unitRef=\"euro\">0</pgc-07-c-bs:BalanceSituacionVariable>\n";

  if (!anyo_cierre_anterior.isEmpty())
  {
   cad_xml+="<pgc-07-c-bs:BalanceSituacionVariable decimals=\"0\" contextRef=\"Y1_";
   cad_xml+=anyo_cierre_anterior;
   cad_xml+="_Balance\" unitRef=\"euro\">0</pgc-07-c-bs:BalanceSituacionVariable>\n";
  }

  // ui->procesolabel->setText(tr("Generando balance de situación ...."));
  ui->progressBar->setValue(3);
  QApplication::processEvents();


  calcestado *c = new calcestado;
  c->cargaestado(bal_pymes_oficial);
  c->pasaejercicio1(ui->actualcomboBox->currentText());
  c->pasaejercicio2(ui->anteriorcomboBox->currentText());
  c->no_msj_calculado();
  c->calculaestado();
  // c->exec();

  delete(c);

  cad_xml+=balance_situacion(anyo_cierre_actual, 2);

  if (!anyo_cierre_anterior.isEmpty())
    cad_xml+=balance_situacion(anyo_cierre_anterior, 3);

  calcestado *cc = new calcestado;
  cc->cargaestado(pyg_pymes_oficial);
  cc->pasaejercicio1(ui->actualcomboBox->currentText());
  cc->pasaejercicio2(ui->anteriorcomboBox->currentText());
  cc->no_msj_calculado();
  cc->calculaestado();
  // c->exec();

  delete(cc);

  ui->procesolabel->setText(tr("Generando pérdidas y ganancias ...."));
  ui->progressBar->setValue(6);
  QApplication::processEvents();

  cad_xml+=pyg(anyo_cierre_actual, 2);

  if (!anyo_cierre_anterior.isEmpty())
     cad_xml+=pyg(anyo_cierre_anterior, 3);


  ui->procesolabel->setText(tr("Generando estado cambios patrimonio ...."));
  ui->progressBar->setValue(8);
  QApplication::processEvents();


  /*if (ui->ecpncheckBox->isChecked())
   cad_xml+=ecpn(anyo_cierre_actual, anyo_cierre_anterior);*/


  cad_xml+="</xbrli:xbrl>\n";

  ui->procesolabel->setText("");
  ui->progressBar->reset();
  QApplication::processEvents();

  return cad_xml;
}


void xbrlpymes::gen_archivo()
{

  QString nombre;

  QString xml = generar_contenido();

  if (xml.isEmpty()) return;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajo());
  dir->filtrar("*.xml");
  dir->activa_pide_archivo("");
  if (dir->exec() == QDialog::Accepted)
    {
      nombre=dir->nuevo_conruta();
    }
   delete(dir);

#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::AnyFile);
  dialogofich.setOption(QFileDialog::DontConfirmOverwrite,true);
  dialogofich.setAcceptMode (QFileDialog::AcceptSave );
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Archivos XML (*.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajobd()));
  dialogofich.setWindowTitle(tr("ARCHIVO PARA EXPORTAR CUENTAS ANUALES"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           // QString nombre=nombre.fromLocal8bit(fileNames.at(0));
           nombre=fileNames.at(0);
          }
     }

#endif

  if (nombre.isEmpty()) return;

  if (nombre.right(4)!=".xml") nombre=nombre+".xml";
   QFile estados( adapta(nombre)  );
   if (estados.exists() && QMessageBox::question(this,
       tr("¿ Sobreescribir ? -- Exportar cuentas"),
       tr("'%1' ya existe."
          "¿ Desea sobreescribirlo ?")
         .arg( nombre ),
       tr("&Sí"), tr("&No"),
       QString(), 0, 1 ) )
     return ;

   // -----------------------------------------------------------------------------------

   QFile fichero( adapta(nombre)  );

   if ( !fichero.open( QIODevice::WriteOnly ) )
       QMessageBox::warning( this, tr("EXPORTAR CUENTAS ANUALES"),
                    tr("ERROR al generar el archivo"));


   QTextStream stream( &fichero );
   stream.setEncoding(QStringConverter::Utf8);

   // ------------------------------------------------------------------------------------
   stream << xml;
   fichero.close();

   QMessageBox::information( this, tr("EXPORTAR CUENTAS ANUALES"),
                 tr("Proceso terminado"));

}


QString xbrlpymes::balance_situacion(QString anyo_cierre, int pos)
{
QString cad_xml;
QSqlQuery q = basedatos::instancia()->select5Estadostituloparte1ordernodo(
                      bal_pymes_oficial , true ); // parte I
QString linea_bs, linea_ap;
linea_bs="<pgc-07-c-bs:%1 decimals=\"0\" contextRef=\"Y1_%2_Balance\" unitRef=\"euro\">%3</pgc-07-c-bs:%1>\n";
linea_ap="<pgc-07-c-ap:%1 decimals=\"0\" contextRef=\"Y1_%2_Balance\" unitRef=\"euro\">%3</pgc-07-c-ap:%1>\n";
if ( q.isActive() )
   {
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
                   cad_xml+=linea_bs.arg("ActivoNoCorriente",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoNoCorrienteInmovilizadoIntangible",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoNoCorrienteInmovilizadoMaterial",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoNoCorrienteInversionesInmobiliarias",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoNoCorrienteInversionesEmpresasGrupoEmpresasAsociadasLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoNoCorrienteInversionesFinancierasLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoNoCorrienteActivosImpuestoDiferido",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoNoCorrienteDeudasComercialesNoCorriente",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorriente",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteExistencias",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteDeudoresComercialesOtrasCuentasCobrar",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteDeudoresComercialesOtrasCuentasCobrarClientesVentasPrestacionesServicios",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteDeudoresComercialesOtrasCuentasCobrarClientesVentasPrestacionesServiciosLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteDeudoresComercialesOtrasCuentasCobrarClientesVentasPrestacionesServiciosCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteDeudoresComercialesOtrasCuentasCobrarAccionistasDesembolsosExigidos",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_ap.arg("ActivoCorrienteDeudoresComercialesOtrasCuentasCobrarOtrosDeudores",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_ap.arg("ActivoCorrienteInversionesEmpresasGrupoEmpresasAsociadasCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteInversionesFinancierasCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrientePeriodificacionesCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("ActivoCorrienteEfectivoOtrosActivosLiquidosEquivalentes",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("TotalActivo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));

   }

q = basedatos::instancia()->select5Estadostituloparte1ordernodo(
                        bal_pymes_oficial , false ); // parte II

if ( q.isActive() )
   {
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNeto",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropios",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosCapital",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosCapitalEscriturado",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosCapitalNoExigido",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosPrimaEmision",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001)) {

         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosReservas",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosReservasOtrasReservas",
                                      anyo_cierre,
                                      formatea_redondeado(q.value(pos).toDouble(),false,true));
     }
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosAccionesParticipacionesPatrimonioPropias",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosResultadosEjerciciosAnteriores",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosOtrasAportacionesSocios",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosResultadoEjercicio",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoFondosPropiosDividendoCuenta",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoAjustesCambioValor",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoSubvencionesDonacionesLegadosRecibidos",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorriente",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrienteProvisionesLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrienteDeudasLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrienteDeudasLargoPlazoDeudasEntidadesCredito",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrienteDeudasLargoPlazoAcreedoresArrendamientoFinanciero",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_ap.arg("PasivoNoCorrienteDeudasLargoPlazoOtrasDeudas",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrienteDeudasEmpresasGrupoEmpresasAsociadasLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrientePasivosImpuestoDiferido",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrientePeriodificacionesLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrienteAcreedoresComercialesNoCorrientes",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoNoCorrienteDeudaCaracteristicasEspecialesLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorriente",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteProvisionesCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteDeudasCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteDeudasCortoPlazoDeudasEntidadesCredito",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteDeudasCortoPlazoAcreedoresArrendamientoFinanciero",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_ap.arg("PasivoCorrienteDeudasCortoPlazoOtrasDeudas",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteDeudasEmpresasGrupoEmpresasAsociadas",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteAcreedoresComercialesOtrasCuentasPagar",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteAcreedoresComercialesOtrasCuentasPagarProveedores",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteAcreedoresComercialesOtrasCuentasPagarProveedoresLargoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteAcreedoresComercialesOtrasCuentasPagarProveedoresCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_ap.arg("PasivoCorrienteAcreedoresComercialesOtrasCuentasPagarOtrosAcreedores",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrientePeriodificacionesCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PasivoCorrienteDeudasCaracteristicasEspecialesCortoPlazo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PatrimonioNetoPasivoTotal",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
   }
  return cad_xml;
}


// --------------------------------------------------------------------------------------------

QString xbrlpymes::pyg(QString anyo_cierre, int pos)
{
QString cad_xml;
QSqlQuery q = basedatos::instancia()->select5Estadostituloparte1ordernodo(
                      pyg_pymes_oficial , true ); // parte I
QString linea_bs, linea_ap;
linea_bs="<pgc-07-c-bs:%1 decimals=\"0\" contextRef=\"Y1_%2_PYG\" unitRef=\"euro\">%3</pgc-07-c-bs:%1>\n";
linea_ap="<pgc-07-c-ap:%1 decimals=\"0\" contextRef=\"Y1_%2_PYG\" unitRef=\"euro\">%3</pgc-07-c-ap:%1>\n";
// formatea_redondeado(q.value(2).toDouble(),false,true)
if ( q.isActive() )
   {
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
                   cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasImporteNetoCifraNegocios",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasVariacionExistenciasProductosTerminadosProductosCursoFabricacion",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasTrabajosRealizadosEmpresaActivo",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasAprovisionamientos",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasOtrosIngresosExplotacion",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasGestionPersonal",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasOtrosGastosExplotacion",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasAmortizacionInmovilizado",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasImputacionSubvencionesInmovilizadoNoFinancieroOtras",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasExcesosProvisiones",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasDeterioroResultadoEnajenacionesInmovilizado",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOtrosResultados",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasResultadoExplotacion",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasIngresosFinancieros",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasIngresosFinancierosImputacionSubvencionesDonacionesLegadosCaracterFinanciero",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_ap.arg("PerdidasGananciasOperacionesContinuadasIngresosFinancierosOtrosIngresosFinancieros",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasGastosFinancieros",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasVariacionValorRazonableInstrumentosFinancieros",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasDiferenciasCambio",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasDeterioroResultadoEnajenacionesInstrumentosFinancieros",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasResultadoFinanciero",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasResultadoAntesImpuestos",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasOperacionesContinuadasImpuestosSobreBeneficios",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));
     if  (q.next() && (q.value(pos).toDouble()>0.001 || q.value(pos).toDouble()<-0.001))
         cad_xml+=linea_bs.arg("PerdidasGananciasResultadoEjercicio",
                                          anyo_cierre,
                                          formatea_redondeado(q.value(pos).toDouble(),false,true));

   }

  return cad_xml;
}


QString xbrlpymes::ecpn(QString anyo_actual, QString anyo_anterior)
{
    if (anyo_anterior.isEmpty())
       {
         int actual=anyo_actual.toInt() ;
         actual--;
         anyo_anterior.setNum(actual);
       }
    patrimonio *p = new patrimonio();
    p->pasa_ejercicio(ui->actualcomboBox->currentText());
    p->modo_deposito();
    p->exec();

    // navegamos por la plantilla en busca de las etiquetas
/*
"pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicio"
"pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesCambiosCriterio"
"pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesErroresEjercicio"

"pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicio"

"pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioResultadoPyG"

"pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioTotalIngresosGastosReconocidos"

"pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioIngresosGastosReconocidosIngresosFiscalesDistribuirVariosEjercicios"

"pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioIngresosGastosReconocidosOtrosIngresosGastos"

"pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietarios"

"pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosAumentosCapital"

"pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosReduccionesCapital"

"pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosOtrasOperacionesSociosOperacionesPropietarios"

"pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOtrasVariacionesPatrimonioNeto"

"pgc07cbs-dvs:VariacionSaldoSaldoFinalEjercicio"

"pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesCambiosCriterio"
"pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesErroresEjercicio"
*/

 QString cad_xml;
 cad_xml+=varpat_instante(0,
                          "pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicio",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(1,
                          "pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesCambiosCriterio",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(2,
                          "pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesErroresEjercicio",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(3,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicio",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(4,
                          "pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioResultadoPyG",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(5,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioTotalIngresosGastosReconocidos",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(6,
                          "pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioIngresosGastosReconocidosIngresosFiscalesDistribuirVariosEjercicios",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(7,
                          "pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioIngresosGastosReconocidosOtrosIngresosGastos",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(8,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietarios",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(9,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosAumentosCapital",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(10,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosReduccionesCapital",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(11,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosOtrasOperacionesSociosOperacionesPropietarios",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(12,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOtrasVariacionesPatrimonioNeto",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(13,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOtrasVariacionesPatrimonioNetoReservaRevalorizacion",
                          anyo_anterior,
                          p);

 cad_xml+=varpat_instante(14,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOtrasVariacionesPatrimonioNetoOtrasVariaciones",
                          anyo_anterior,
                          p);

 /*
 cad_xml+=varpat_instante(13,
                          "pgc07cbs-dvs:VariacionSaldoSaldoFinalEjercicio",
                          anyo_anterior,
                          p);
                          */
 cad_xml+=varpat_instante(15,
                          "pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicio",
                          anyo_actual,
                          p);
// ----------------------------------------------------------------------------------------
 cad_xml+=varpat_instante(16,
                          "pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesCambiosCriterio",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(17,
                          "pgc07cbs-dvs:VariacionSaldoSaldoInicioEjercicioAjustesErroresEjercicio",
                          anyo_actual,
                          p);


 cad_xml+=varpat_instante(18,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicio",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(19,
                          "pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioResultadoPyG",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(20,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioTotalIngresosGastosReconocidos",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(21,
                          "pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioIngresosGastosReconocidosIngresosFiscalesDistribuirVariosEjercicios",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(22,
                          "pgc07p-d-vs:VariacionSaldoSaldoAjustadoInicioEjercicioIngresosGastosReconocidosOtrosIngresosGastos",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(23,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietarios",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(24,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosAumentosCapital",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(25,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosReduccionesCapital",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(26,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOperacionesSociosOperacionesPropietariosOtrasOperacionesSociosOperacionesPropietarios",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(27,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOtrasVariacionesPatrimonioNeto",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(28,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOtrasVariacionesPatrimonioNetoMovimientoReservaRevalorizacion",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(29,
                          "pgc07cbs-dvs:VariacionSaldoSaldoAjustadoInicioEjercicioOtrasVariacionesPatrimonioNetoOtrasVariaciones",
                          anyo_actual,
                          p);

 cad_xml+=varpat_instante(30,
                          "pgc07cbs-dvs:VariacionSaldoSaldoFinalEjercicio",
                          anyo_actual,
                          p);

 delete(p);
 return cad_xml;
}


QString xbrlpymes::varpat_instante(int fila, QString etiqueta, QString periodo, patrimonio *p)
{
  // %1 --- Etiqueta
  // %2 --- Período (ejemplo 2009)
  // %3 --- Instante
  // %4 --- Importe
  QString linea="<%1 decimals=\"0\" contextRef=\"Y1_%2_%3\" unitRef=\"euro\">%4</%1>\n";
  QString cad_xml;

  if (!p->contenido(fila,1).isEmpty())
     cad_xml+=linea.arg(etiqueta,
                     periodo,
                     "etcpn_Instant_CambiosPatrimonioNetoCapitalEscriturado",
                      p->contenido(fila,1));

 if (!p->contenido(fila,2).isEmpty())
   cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoCapitalNoExigido",
                    p->contenido(fila,2));

 if (!p->contenido(fila,3).isEmpty())
   cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoPrimaEmision",
                    p->contenido(fila,3));

 if (!p->contenido(fila,4).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoReservas",
                    p->contenido(fila,4));

 if (!p->contenido(fila,5).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias",
                    p->contenido(fila,5));

 if (!p->contenido(fila,6).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoResultadoEjerciciosAnteriores",
                    p->contenido(fila,6));


 if (!p->contenido(fila,7).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoOtrasAportacionesSocios",
                    p->contenido(fila,7));

 if (!p->contenido(fila,8).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoResultadoEjercicio",
                    p->contenido(fila,8));

 if (!p->contenido(fila,9).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoDividendoCuenta",
                    p->contenido(fila,9));

 if (!p->contenido(fila,10).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto",
                    p->contenido(fila,10));

 if (!p->contenido(fila,11).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos",
                    p->contenido(fila,11));

 if (!p->contenido(fila,12).isEmpty())
 cad_xml+=linea.arg(etiqueta,
                    periodo,
                    "etcpn_Instant_CambiosPatrimonioNetoTotalCambios",
                    p->contenido(fila,12));

 return cad_xml;
}



QString xbrlpymes::dimensiones_ecpn(QString anyo_cierre_actual, QString anyo_cierre_anterior,
                                    QDate apertura_actual, QDate cierre_actual,
                                    QDate apertura_anterior, QDate cierre_anterior)
{
    QString cad_xml;
    QString abrecontex="<xbrli:context id=\"%1\">\n";
    QString cierracontex="</xbrli:context>\n";

    QString entity="  <xbrli:entity>\n";
    entity+=       "    <xbrli:identifier scheme=\"http://www.icac.meh.es/xbrl\">";
    entity+=basedatos::instancia()->cif();
    entity+="</xbrli:identifier>\n";
    entity+=       "  </xbrli:entity>\n";

    QString period="  <xbrli:period>\n"
                   "    <xbrli:instant>%1</xbrli:instant>\n"
                   "  </xbrli:period>\n";

    QString period2="  <xbrli:period>\n"
                    "    <xbrli:startDate>%1</xbrli:startDate>\n"
                    "    <xbrli:endDate>%2</xbrli:endDate>\n"
                    "  </xbrli:period>\n";


    QString ecpn=abrecontex.arg("Y1_"+anyo_cierre_actual+"%1");
    ecpn+=entity;
    ecpn+=period2.arg(apertura_actual.toString("yyyy-MM-dd"),cierre_actual.toString("yyyy-MM-dd"));
    ecpn+="  <xbrli:scenario>\n";
    ecpn+="    <xbrldi:explicitMember dimension=\"pgc07cbs-dpn:CambiosPatrimonioNetoDimension\">%2</xbrldi:explicitMember>\n";
    ecpn+="  </xbrli:scenario>\n";
    ecpn+=cierracontex;

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoPrimaEmision",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoPrimaEmision");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoReservas",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoReservas");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoResultadoEjerciciosAnteriores",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjerciciosAnteriores");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoOtrasAportacionesSocios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrasAportacionesSocios");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoResultadoEjercicio",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjercicio");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoDividendoCuenta",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoDividendoCuenta");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoTotalCambios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoTotalCambios");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoCapitalEscriturado",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalEscriturado");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoCapitalNoExigido",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalNoExigido");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto");
    /*
    <xbrli:context id="Y1_2010_etcpn_Instant_CambiosPatrimonioNetoPrimaEmision">
      <xbrli:entity>
        <xbrli:identifier scheme="http://www.bde.es/xbrl/test">Comercial Textil Óleo, S.A.</xbrli:identifier>
      </xbrli:entity>
      <xbrli:period>
        <xbrli:instant>2010-12-31</xbrli:instant>
      </xbrli:period>
      <xbrli:scenario>
        <xbrldi:explicitMember dimension="pgc07cbs-dpn:CambiosPatrimonioNetoDimension">pgc07cbs-dpn:CambiosPatrimonioNetoPrimaEmision</xbrldi:explicitMember>
      </xbrli:scenario>
    </xbrli:context>
     */

    QString ecpn2=abrecontex.arg("Y1_"+anyo_cierre_actual+"%1");
    ecpn2+=entity;
    ecpn2+=period.arg(cierre_actual.toString("yyyy-MM-dd"));
    ecpn2+="  <xbrli:scenario>\n";
    ecpn2+="    <xbrldi:explicitMember dimension=\"pgc07cbs-dpn:CambiosPatrimonioNetoDimension\">%2</xbrldi:explicitMember>\n";
    ecpn2+="  </xbrli:scenario>\n";
    ecpn2+=cierracontex;

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoPrimaEmision",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoPrimaEmision");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoReservas",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoReservas");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoResultadoEjerciciosAnteriores",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjerciciosAnteriores");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoOtrasAportacionesSocios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrasAportacionesSocios");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoResultadoEjercicio",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjercicio");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoDividendoCuenta",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoDividendoCuenta");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoTotalCambios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoTotalCambios");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoCapitalEscriturado",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalEscriturado");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoCapitalNoExigido",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalNoExigido");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto");

    // ---------------------------------------------------------------------------------------------
  if (!anyo_cierre_anterior.isEmpty())
    {
    ecpn=abrecontex.arg("Y1_"+anyo_cierre_anterior+"%1");
    ecpn+=entity;
    ecpn+=period2.arg(apertura_anterior.toString("yyyy-MM-dd"),cierre_anterior.toString("yyyy-MM-dd"));
    ecpn+="  <xbrli:scenario>\n";
    ecpn+="    <xbrldi:explicitMember dimension=\"pgc07cbs-dpn:CambiosPatrimonioNetoDimension\">%2</xbrldi:explicitMember>\n";
    ecpn+="  </xbrli:scenario>\n";
    ecpn+=cierracontex;

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoPrimaEmision",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoPrimaEmision");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoReservas",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoReservas");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoResultadoEjerciciosAnteriores",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjerciciosAnteriores");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoOtrasAportacionesSocios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrasAportacionesSocios");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoResultadoEjercicio",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjercicio");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoDividendoCuenta",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoDividendoCuenta");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoTotalCambios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoTotalCambios");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoCapitalEscriturado",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalEscriturado");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoCapitalNoExigido",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalNoExigido");

    cad_xml+=ecpn.arg("_etcpn_Duration_CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto");

    // ------------------------------------------------------------------------------------------
    ecpn2=abrecontex.arg("Y1_"+anyo_cierre_anterior+"%1");
    ecpn2+=entity;
    ecpn2+=period.arg(cierre_anterior.toString("yyyy-MM-dd"));
    ecpn2+="  <xbrli:scenario>\n";
    ecpn2+="    <xbrldi:explicitMember dimension=\"pgc07cbs-dpn:CambiosPatrimonioNetoDimension\">%2</xbrldi:explicitMember>\n";
    ecpn2+="  </xbrli:scenario>\n";
    ecpn2+=cierracontex;

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoPrimaEmision",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoPrimaEmision");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoReservas",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoReservas");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoAccionesParticipacionesPatrimonioPropias");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoResultadoEjerciciosAnteriores",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjerciciosAnteriores");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoOtrasAportacionesSocios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrasAportacionesSocios");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoResultadoEjercicio",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoResultadoEjercicio");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoDividendoCuenta",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoDividendoCuenta");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoSubvencionesDonacionesLegadosRecibidos");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoTotalCambios",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoTotalCambios");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoCapitalEscriturado",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalEscriturado");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoCapitalNoExigido",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoCapitalNoExigido");

    cad_xml+=ecpn2.arg("_etcpn_Instant_CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto",
                      "pgc07cbs-dpn:CambiosPatrimonioNetoOtrosInstrumentosPatrimonioNeto");
    } // año anterior
    // ------------------------------------------------------------------------------------------

    return cad_xml;
}



xml_is_pymes::xml_is_pymes() : xbrlpymes()
{
  setWindowTitle(tr("Exportación para Impuesto Sociedades"));
  bal_pymes_is=tr("BALANCE PYMES IS");
  pyg_pymes_is=tr("CUENTA PYG PYMES IS");

}


QString xml_is_pymes::generar_contenido()
{
    QString ejactual=ui->actualcomboBox->currentText();
    QString ejanterior=ui->anteriorcomboBox->currentText();

    QDate cierre_actual=finejercicio(ejactual);
    QDate cierre_anterior=finejercicio(ejanterior);
    QDate apertura_actual=inicioejercicio(ejactual);
    //QDate apertura_anterior=inicioejercicio(ejanterior);
    QString anyo_cierre_actual; anyo_cierre_actual.setNum(cierre_actual.year());
    QString anyo_cierre_anterior;
    QString anyo_apertura_actual; anyo_apertura_actual.setNum(apertura_actual.year());
    if (ejanterior!=noejercicio()) anyo_cierre_anterior.setNum(cierre_anterior.year());

    bal_pymes_is+=anyo_apertura_actual;
    pyg_pymes_is+=anyo_apertura_actual;


    // -----------------------------------------------------------------------------------------
     // averiguamos si existen los estados para el depósito
     // -------------------------------------------------------------

     if (!basedatos::instancia()->existe_estado(bal_pymes_is))
       {
         QMessageBox::warning( this, tr("Estados Contables"),tr("ERROR, NO EXISTE: ")+
                               bal_pymes_is);
         return QString() ;
       }

     if (!basedatos::instancia()->existe_estado(pyg_pymes_is))
       {
         QMessageBox::warning( this, tr("Estados Contables"),tr("ERROR, NO EXISTE: ")+
                               pyg_pymes_is);
         return QString();
       }

   ui->progressBar->setMaximum(10);

   ui->procesolabel->setText(tr("Generando balance de situación ...."));
   ui->progressBar->setValue(2);
   QApplication::processEvents();



    QString namexml="MOD200";
    namexml+=anyo_apertura_actual;
    QDomDocument doc(namexml);
    // QDomDocument doc();
    QDomElement root = doc.createElement(namexml);
    doc.appendChild(root);

    QDomElement tag = doc.createElement("Normal");
    root.appendChild(tag);

    QDomElement tag_balance = doc.createElement("Balance");
    tag.appendChild(tag_balance);

    QDomElement tag_cuenta_pyg = doc.createElement("CuentaPyG");
    tag.appendChild(tag_cuenta_pyg);

    QDomElement tag_cambiospn = doc.createElement("CambiosPN");
    tag.appendChild(tag_cambiospn);

    // metemos páginas en cada estado

    // BALANCE
    QDomElement pagina03 = doc.createElement("Pagina03"); tag_balance.appendChild(pagina03);
    QDomElement pagina04 = doc.createElement("Pagina04"); tag_balance.appendChild(pagina04);
    QDomElement pagina05 = doc.createElement("Pagina05"); tag_balance.appendChild(pagina05);
    QDomElement pagina06 = doc.createElement("Pagina06"); tag_balance.appendChild(pagina06);


    // Pérdidas y ganancias
    QDomElement pagina07 = doc.createElement("Pagina07"); tag_cuenta_pyg.appendChild(pagina07);
    QDomElement pagina08 = doc.createElement("Pagina08"); tag_cuenta_pyg.appendChild(pagina08);

    // Cambios patrimonio neto
    // QDomElement pagina09 = doc.createElement("Pagina09"); tag_cambiospn.appendChild(pagina09);
    QDomElement pagina10 = doc.createElement("Pagina10"); tag_cambiospn.appendChild(pagina10);
    QDomElement pagina11 = doc.createElement("Pagina11"); tag_cambiospn.appendChild(pagina11);


    // Páginas 03 a 06 == balance

    calcestado *c = new calcestado;
    c->cargaestado(bal_pymes_is);
    c->pasaejercicio1(ui->actualcomboBox->currentText());
    c->pasaejercicio2(ui->anteriorcomboBox->currentText());
    c->no_msj_calculado();
    c->calculaestado();
    // c->exec();

    delete(c);

    QString xad;
    if (anyo_apertura_actual>"2014") xad="00";

    QSqlQuery q = basedatos::instancia()->select5Estadostituloparte1ordernodo(
                          bal_pymes_is , true ); // parte I
    if ( q.isActive() )
       while  (q.next())
        {
          // Página 3 // formatea_redondeado(double numero,bool comadecimal, bool decimales)
          if (q.value(2).toDouble()>-0.001 && q.value(2).toDouble()<0.001) continue;
          if (q.value(1).toString().contains("{106}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"106",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{110}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"110",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{111}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"111",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{115}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"115",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{119}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"119",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{125}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"125",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{127}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"127",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{133}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"133",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{134}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"134",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{135}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"135",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{138}"))
              addElementoTextoDom(doc,pagina03,"T"+xad+"138",formatea_redondeado(q.value(2).toDouble(),false,true));
          // Página 4
          //if (q.value(1).toString().contains("{150}"))
          //    addElementoTextoDom(doc,pagina04,"T"+xad+"150",q.value(2).toString());
          if (q.value(1).toString().contains("{151}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"151",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{152}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"152",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{158}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"158",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{159}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"159",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{161}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"161",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{167}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"167",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{169}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"169",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{175}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"175",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{176}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"176",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{177}"))
              addElementoTextoDom(doc,pagina04,"T"+xad+"177",formatea_redondeado(q.value(2).toDouble(),false,true));
          //if (q.value(1).toString().contains("{180}"))
          //    addElementoTextoDom(doc,pagina04,"T"+xad+"180",q.value(2).toString());
        }
    q = basedatos::instancia()->select5Estadostituloparte1ordernodo(
                              bal_pymes_is , false ); // parte I
    if ( q.isActive() )
        while  (q.next())
         {
          // Página 5
          if (q.value(2).toDouble()>-0.001 && q.value(2).toDouble()<0.001) continue;
          if (q.value(1).toString().contains("{188}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"188",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{189}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"189",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{190}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"190",formatea_redondeado(q.value(2).toDouble(),false,true));

          if (q.value(1).toString().contains("{191}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"191",formatea_redondeado(q.value(2).toDouble(),false,true));

          if (q.value(1).toString().contains("{193}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"193",formatea_redondeado(q.value(2).toDouble(),false,true));

          if (q.value(1).toString().contains("{194}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"194",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{195}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"195",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{198}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"198",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{199}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"199",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{200}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"200",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{208}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"208",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{209}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"209",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{211}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"211",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{218}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"218",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{219}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"219",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{222}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"222",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{223}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"223",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{224}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"224",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{225}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"225",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{226}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"226",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{227}"))
              addElementoTextoDom(doc,pagina05,"T"+xad+"227",formatea_redondeado(q.value(2).toDouble(),false,true));

          // Página 6
          if (q.value(1).toString().contains("{230}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"230",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{233}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"233",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{234}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"234",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{237}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"237",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{238}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"238",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{241}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"241",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{242}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"242",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{249}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"249",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{250}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"250",formatea_redondeado(q.value(2).toDouble(),false,true));
          if (q.value(1).toString().contains("{251}"))
              addElementoTextoDom(doc,pagina06,"T"+xad+"251",formatea_redondeado(q.value(2).toDouble(),false,true));

        }


    ui->procesolabel->setText(tr("Generando pérdidas y ganancias ...."));
    ui->progressBar->setValue(6);
    QApplication::processEvents();

    // Página 7, 8: Cuenta de pérdidas y ganancias
    calcestado *cc = new calcestado;
    cc->cargaestado(pyg_pymes_is);
    cc->pasaejercicio1(ui->actualcomboBox->currentText());
    cc->pasaejercicio2(ui->anteriorcomboBox->currentText());
    cc->no_msj_calculado();
    cc->calculaestado();
    delete(cc);

    q = basedatos::instancia()->select5Estadostituloparte1ordernodo(
                              pyg_pymes_is , true ); // parte I
        if ( q.isActive() )
           while  (q.next())
            {
              // Página 7
              if (q.value(2).toDouble()>-0.001 && q.value(2).toDouble()<0.001) continue;
              if (q.value(1).toString().contains("{255}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"255",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{258}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"258",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{259}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"259",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{260}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"260",formatea_redondeado(q.value(2).toDouble(),false,true));
              //if (q.value(1).toString().contains("{266}"))
              //    addElementoTextoDom(doc,pagina07,"T"+xad+"266",q.value(2).toString());
              if (q.value(1).toString().contains("{267}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"267",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{268}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"268",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{269}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"269",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{271}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"271",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{272}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"272",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{273}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"273",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{274}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"274",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{275}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"275",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{276}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"226",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{277}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"277",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{253}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"253",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{254}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"254",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{284}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"284",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{285}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"285",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{286}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"286",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{288}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"288",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{289}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"289",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{290}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"290",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{291}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"291",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{292}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"292",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{293}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"293",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{295}"))
                  addElementoTextoDom(doc,pagina07,"T"+xad+"295",formatea_redondeado(q.value(2).toDouble(),false,true));
              // if (q.value(1).toString().contains("{298}"))
              //    addElementoTextoDom(doc,pagina07,"T"+xad+"298",q.value(2).toString());
              // Página 8
              if (q.value(1).toString().contains("{299}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"299",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{300}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"300",formatea_redondeado(q.value(2).toDouble(),false,true));
              //if (q.value(1).toString().contains("{301}"))
              //    addElementoTextoDom(doc,pagina07,"T"+xad+"301",q.value(2).toString());
              if (q.value(1).toString().contains("{302}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"302",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{303}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"303",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{304}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"304",formatea_redondeado(q.value(2).toDouble(),false,true));
              // Página 8
              if (q.value(1).toString().contains("{306}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"306",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{307}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"307",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{308}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"308",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{309}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"309",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{312}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"312",formatea_redondeado(q.value(2).toDouble(),false,true));
              //if (q.value(1).toString().contains("{314}"))
              //    addElementoTextoDom(doc,pagina08,"T"+xad+"314",q.value(2).toString());
              if (q.value(1).toString().contains("{315}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"315",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{316}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"316",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{317}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"317",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{318}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"318",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{319}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"319",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{320}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"320",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{321}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"321",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{322}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"322",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{323}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"323",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{326}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"326",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{330}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"330",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{331}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"331",formatea_redondeado(q.value(2).toDouble(),false,true));
              if (q.value(1).toString().contains("{332}"))
                  addElementoTextoDom(doc,pagina08,"T"+xad+"332",formatea_redondeado(q.value(2).toDouble(),false,true));

            }
  // return QString();


  // procedemos ahora con ECPN
  if (ui->ecpncheckBox->isChecked()) {
  patrimonio *p = new patrimonio();
  p->pasa_ejercicio(ui->actualcomboBox->currentText());
  p->modo_deposito();
  p->exec();

  ui->procesolabel->setText(tr("Generando estado cambios patrimonio ...."));
  ui->progressBar->setValue(8);
  QApplication::processEvents();


  // página 10
    // línea 13 --> 15
  for (int veces=0; veces<7; veces++)
      {
       int casilla=380+veces;
       QString cadcasilla; cadcasilla.setNum(casilla);
       cadcasilla="T"+xad+""+cadcasilla;
       QString cad=convapunto(p->contenido(15,veces+1));
       if (!cad.isEmpty())
          addElementoTextoDom(doc,pagina10,cadcasilla,cad);
      }
    // línea 14 --> 16
  for (int veces=0; veces<7; veces++)
     {
      int casilla=394+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(16,veces+1));
      if (!cad.isEmpty())
         addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 15 --> 17
  for (int veces=0; veces<7; veces++)
     {
      int casilla=408+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(17,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 17 --> 19
  for (int veces=0; veces<7; veces++)
     {
      int casilla=450+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(19,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 19 --> 21
  for (int veces=0; veces<7; veces++)
     {
      int casilla=478+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(21,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 20 --> 22
  for (int veces=0; veces<7; veces++)
     {
      int casilla=492+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(22,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 22 --> 24
  for (int veces=0; veces<7; veces++)
     {
      int casilla=520+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(24,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 23 --> 25
  for (int veces=0; veces<7; veces++)
     {
      int casilla=534+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(25,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 24 -- 26
  for (int veces=0; veces<7; veces++)
     {
      int casilla=604+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(26,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }
  // línea 25 --> 27 - es del total, ahora procede 28 y 29
  for (int veces=0; veces<7; veces++)
     {
      int casilla=715+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(28,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }

  for (int veces=0; veces<7; veces++)
     {
      int casilla=729+veces;
      QString cadcasilla; cadcasilla.setNum(casilla);
      cadcasilla="T"+xad+""+cadcasilla;
      QString cad=convapunto(p->contenido(29,veces+1));
      if (!cad.isEmpty())
        addElementoTextoDom(doc,pagina10,cadcasilla,cad);
     }

  // página 11 -----------------------------------------
  // línea 13 --> 15
  QString cad;
  cad=convapunto(p->contenido(15,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"387",cad);
  cad=convapunto(p->contenido(15,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"388",cad);
  cad=convapunto(p->contenido(15,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"391",cad);
  cad=convapunto(p->contenido(15,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"392",cad);
  // línea 14
  cad=convapunto(p->contenido(16,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"401",cad);
  cad=convapunto(p->contenido(16,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"402",cad);
  cad=convapunto(p->contenido(16,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"405",cad);
  cad=convapunto(p->contenido(16,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"406",cad);
  // línea 15
  cad=convapunto(p->contenido(17,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"415",cad);
  cad=convapunto(p->contenido(17,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"416",cad);
  cad=convapunto(p->contenido(17,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"419",cad);
  cad=convapunto(p->contenido(17,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"420",cad);
  // línea 17
  cad=convapunto(p->contenido(19,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"457",cad);
  cad=convapunto(p->contenido(19,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"458",cad);
  cad=convapunto(p->contenido(19,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"461",cad);
  cad=convapunto(convapunto(p->contenido(19,11)));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"462",cad);
  // línea 19
  cad=convapunto(p->contenido(21,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"485",cad);
  cad=convapunto(p->contenido(21,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"486",cad);
  cad=convapunto(p->contenido(21,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"489",cad);
  cad=convapunto(p->contenido(21,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"490",cad);
  // línea 20
  cad=convapunto(p->contenido(22,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"499",cad);
  cad=convapunto(p->contenido(22,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"502",cad);
  cad=convapunto(p->contenido(22,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"503",cad);
  cad=convapunto(p->contenido(22,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"504",cad);
  // línea 22
  cad=convapunto(p->contenido(24,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"527",cad);
  cad=convapunto(p->contenido(24,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"528",cad);
  cad=convapunto(p->contenido(24,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"531",cad);
  cad=convapunto(p->contenido(24,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"532",cad);
  // línea 23
  cad=convapunto(p->contenido(25,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"541",cad);
  cad=convapunto(p->contenido(25,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"542",cad);
  cad=convapunto(p->contenido(25,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"545",cad);
  cad=convapunto(p->contenido(25,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"546",cad);
  // línea 24
  cad=convapunto(p->contenido(26,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"611",cad);
  cad=convapunto(p->contenido(26,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"612",cad);
  cad=convapunto(p->contenido(26,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"615",cad);
  cad=convapunto(p->contenido(26,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"616",cad);
  // línea 25 --- ya no, pasamos 28 y 29
  cad=convapunto(p->contenido(28,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"722",cad);
  cad=convapunto(p->contenido(28,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"723",cad);
  cad=convapunto(p->contenido(28,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"726",cad);
  cad=convapunto(p->contenido(28,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"727",cad);

  cad=convapunto(p->contenido(29,8));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"736",cad);
  cad=convapunto(p->contenido(29,9));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"737",cad);
  cad=convapunto(p->contenido(29,10));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"740",cad);
  cad=convapunto(p->contenido(29,11));
  if (!cad.isEmpty())
      addElementoTextoDom(doc,pagina11,"T"+xad+"741",cad);

  delete(p);

  } // ECPN

  QString xml = doc.toString();
  xml.remove("<!DOCTYPE MOD200"+anyo_apertura_actual+">");
  xml.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  ui->procesolabel->setText("");
  ui->progressBar->reset();
  QApplication::processEvents();

  return xml;
}
