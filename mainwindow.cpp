#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "conexion.h"
#include "preferencias.h"
#include "basedatos.h"
#include "funciones.h"
#include "login.h"
#include "acercade.h"
#include <QMessageBox>
#include <QDockWidget>
#include "editconfig.h"
#include "ejercicios.h"
#include "creaempresa.h"
#include "borraactual.h"
#include "notas.h"
#include "pideejercicio.h"
#include "bloqueaper.h"
#include "usuarios.h"
#include "clave_usuario.h"
#include "import_usuarios.h"
#include "edtabla.h"
#include "carta347.h"
#include "editaplan.h"
#include "buscasubcuenta.h"
#include "datos_accesorios.h"
#include "iva_ret_ctas.h"
#include "bloqueacta.h"
#include <QFileDialog>
#include "introfecha.h"
#include "tabla_asientos.h"
#include "selecasmod.h"
#include "editarasiento.h"
#include "pidenombre.h"
#include "asigna_fichdoc.h"
#include "filtrodiario.h"
#include "cargaespera.h"
#include "imprimediario.h"
#include "conceptos.h"

#include "regularizacion.h"
#include "cierre.h"
#include "suprimecierre.h"
#include "apertura.h"
#include <QProgressDialog>
#include "diarios.h"
#include "regularizaciones.h"
#include "exec_regul.h"
#include "consmayor.h"
#include "cuadimprimayor.h"
#include "acumuladosmes.h"
#include "consplan.h"
#include "diarioresumen.h"
#include "relop.h"
#include "pidecuentafechas.h"
#include "concilia.h"
#include "dif_conc.h"
#include "pidecuentafecha.h"
#include "procesavencimiento.h"
#include "ret_irpf.h"
#include "consret.h"
#include "consultavencipase.h"
#include "listavenci.h"
#include "venci_ctas.h"
#include "tiposvenci.h"
#include "vtosanulados.h"
#include "tiposiva.h"
#include "tiposivacta.h"
#include "cheq_libroiva.h"
#include "liquidacioniva.h"
#include "rellibiva.h"
#include "libro_emitidas.h"
#include "libro_recibidas.h"
#include "modelo340.h"
#include "conta347.h"
#include "ci.h"
#include "consinput.h"
#include "buscasubcuenta_input.h"
#include "cambiaci.h"
#include "reasigna_input_diario.h"
#include "selecestado.h"
#include "patrimonio.h"
#include "concepto_evpn.h"
#include "columnas_ecpn_pymes.h"
#include "xbrlpymes.h"
#include "procesamemoria.h"
#include "presupuesto_ed.h"
#include "desviaciones.h"
#include "planamortizaciones.h"
#include "genamort.h"
#include "amortbi.h"
#include "hcoamort.h"
#include "inmovneto.h"
#include "import_cplus.h"
#include "marcacerrado.h"
#include "asignadiario.h"
#include "libros.h"
#include "copiaseg.h"
#include "consolidacion_sinc.h"
#include "renumerar.h"
#include "intercambianum.h"
#include "borraintasientos.h"
#include "cambiacuenta.h"
#include "cambiacuentapase.h"
#include "cambiacipase.h"
#include "editafechaasiento.h"
#include "edit_conc_doc.h"
//#include "editanuevoplan.h"
//#include "equivalencias.h"
//#include "sustcuenta.h"
#include "infobd.h"
#include "acercade.h"
#include "licencia.h"
#include "domiciliaciones.h"
#include "privilegios.h"
#include "execasientomodelo.h"
#include "calcestado.h"
#include "modelo349.h"
#include "pidecuenta.h"
#include "infovenci.h"
#include "ctas_iva_inversion.h"
#include "formas_pago.h"
#include "papelera.h"
#include "externos.h"
#include "tablasii.h"
#include "impor_externos_csv.h"
#include "direc_sii.h"
#include "busca_externo.h"
#include "inv_inmovilizado.h"
#include "modelo_182.h"
#include "tipos_cambio.h"

#include <QSplashScreen>
#include <QPdfDocument>

#define APLICACION "KEME-Contabilidad - "


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //QSplashScreen *splash=NULL;
      QPixmap pixmap( ":/splash/graf/splash.jpg" );
    //  if (!es_os_x())
    //    {
    splash = new QSplashScreen( pixmap );
    splash->show();
    //    }
QApplication::processEvents();
     ui->setupUi(this);
QApplication::processEvents();
    punterodiario=NULL;
    treeWidget=NULL;
    estadosList=NULL;
    dockAutomaticos=NULL;
    dockEstados=NULL;

    QString directorio;
    QString nombredir;
    directorio=dirtrabajodefecto();
    // QDir d(directorio);
    // if (!d.exists()) d.mkdir(directorio);
    dirtrabajo=directorio;

    dirconfig=trayconfig();

    nombredir="/keme.cfg";
    nombredir[0]=QDir::separator();
    fichconfig=dirconfig+nombredir;

    fichconexiones=fichconex();

    visordvi=visordvidefecto();
    aplicacionimprimir=programa_imprimir_defecto();
    navegador="rekonq";
    if (eswindows())
      {
       navegador=windirprogram();
       navegador+="\\Internet Explorer\\iexplore";
      }
    filtrarpor=3;
    ordendiario=FECHA;
    estilonumerico=COMADECIMAL;
    sindecimales=false;
    // numeracion=NUMERACIONORIGINAL;
    filtropersonalizado="";
    filtrartex=false;
    idioma=0;
    // if (eswindows()) filtrartex=true;
    anchocol[0]=130;
    anchocol[1]=120;
    anchocol[2]=120;
    anchocol[3]=300;
    anchocol[4]=130;
    anchocol[5]=130;
    anchocol[6]=130;
    anchocol[7]=130;
    anchocol[8]=130;
    anchocol[9]=130;
    anchocol[10]=130;
    anchocol[11]=50;
    anchocol[12]=130;
    anchocol[13]=130;
    anchocol[14]=100;
    anchocol[15]=100;

    noguardarpref=false;
    haydockautomaticos=true;
    haydockestados=true;

    haygestionusuarios=false;
    guardausuario=false;
    guardaconexion=true;

    cplus="1";
    cdomiciliaciones="1";

    rutadocs=dirdocsdefecto();

    s = NULL; // diálogo de cuentas auxiliares
    c = NULL; // ventana consulta de mayor permanente
    b = NULL; // Ventana bss

    activa_msj_tabla=false;

    // cargamos tipos_rectificativas

    tipos_rectificativas << "R1 - Error fundado en derecho y Art.80.1, 2, 6 LIVA";
    tipos_rectificativas << "R2 - Art. 80.3 LIVA";
    tipos_rectificativas << "R3 - Art. 80.4 LIVA";
    tipos_rectificativas << "R4 - Factura rectificativa (Resto)";
    tipos_rectificativas << "R5 - Factura Rectificativa en facturas simplificadas";

    fila_presupuesto_tabla=0;

    QNetworkAccessManager *manager=new QNetworkAccessManager();
    mNetMan=manager;

    connect(ui->actionConexion,SIGNAL(triggered()),SLOT(conex()));
    connect(ui->actionOrden_numero, SIGNAL(toggled(bool)), this,
      SLOT (qordendiario(bool)));
    connect(ui->actionPreferencias, SIGNAL(triggered()),this,SLOT(qpreferencias()));
    connect(ui->actionAcerca_de,SIGNAL(triggered()),this,SLOT(met_acercade()));
    connect(ui->action_Salir,SIGNAL(triggered()),this,SLOT(salir()));
    connect(ui->actionConfiguraci_n,SIGNAL(triggered()),this,SLOT(configuracion()));
    connect(ui->actionEjercicios,SIGNAL(triggered()),this,SLOT(ed_ejercicios()));
    connect(ui->actionCrear_Empresa, SIGNAL(triggered()), this, SLOT(nuevaempresa()));

    connect(ui->actionBorrar_Empresa, SIGNAL(triggered()), this, SLOT(eliminaempresaactual()));

    connect(ui->actionCopiar_Empresa, SIGNAL(triggered()), this, SLOT(copiaempresa()));

    connect(ui->actionConsultar_Notas, SIGNAL(triggered()), this, SLOT(consultanotas()));
    connect(ui->actionEditar_Notas, SIGNAL(triggered()), this, SLOT(editanotas()));

    connect(ui->actionHabilitar_Deshabilitar,SIGNAL(triggered()), this, SLOT(deshab_periodos()));

    connect(ui->actionCambio_usuario, SIGNAL(triggered()), this, SLOT(cambiousuario()));

    connect(ui->actionGest_usuarios, SIGNAL(triggered()), this, SLOT(gestionusuarios()));

    connect(ui->actionCambiar_Clave, SIGNAL(triggered()), this, SLOT(cambiaclave()));

    connect(ui->actionImportar_usuarios, SIGNAL(triggered()), this, SLOT(iusuarios()));

    connect(ui->actionCodigo_pais, SIGNAL(triggered()), this, SLOT(tabla_codigo_pais()));
    connect(ui->actionClave_identific_fiscal, SIGNAL(triggered()), this, SLOT(tabla_id_fiscal()));
    connect(ui->actionClave_operaciones_expedidas, SIGNAL(triggered()), this, SLOT(tabla_op_expedidas()));
    connect(ui->actionClave_operaciones_recibidas, SIGNAL(triggered()), this, SLOT(tabla_op_recibidas()));
    connect(ui->actionClave_retenciones_irpf, SIGNAL(triggered()),this,SLOT(tabla_clave_retenciones()));
    connect(ui->actionConceptos_EVPN,SIGNAL(triggered()), this, SLOT(tabla_concepto_evpn_pymes()));
    connect(ui->actionProvincias, SIGNAL(triggered()),this,SLOT(tabla_provincias()));
    connect(ui->actionContenido_comunicacion_347, SIGNAL(triggered()), this, SLOT(contenido347()));
    connect(ui->actionCuentas,SIGNAL(triggered()),SLOT(edplan()));

    connect(ui->actionCuentas_auxiliares, SIGNAL(triggered()),SLOT(edsubcuentas()));
    connect(ui->actionDatos_accesorios_cuentas, SIGNAL(triggered()), SLOT(datos_acc()));
    connect(ui->actionAsignar_IVA_retenci_n_a_cuentas, SIGNAL(triggered()),SLOT(ctas_iva_ret()));
    connect(ui->actionBloquear_cuentas, SIGNAL(triggered()),SLOT(bloqueactas()));

    connect(ui->actionExportar_plan, SIGNAL(triggered()),SLOT(exportplan()));
    connect(ui->actionExportarplan_cuentas_auxiliares,SIGNAL(triggered()),SLOT(exportplansubcuentas()));
    connect(ui->actionImportar_plan, SIGNAL(triggered()),SLOT(actualizaplan()));

    connect(ui->actionNuevo_Asiento,SIGNAL(triggered()),SLOT(nuevoasiento()));

    connect(ui->actionEditar_Asiento,SIGNAL(triggered()),SLOT(editaasiento()));

    connect(ui->actionEditar_Asiento_N_mero,SIGNAL(triggered()),SLOT(editanumasiento()));

    connect(ui->actionConsulta_Asiento,SIGNAL(triggered()),SLOT(consultaasiento()));
    connect(ui->actionConsulta_Asiento_N_mero,SIGNAL(triggered()),SLOT(consultanumasiento()));

    connect(ui->actionChequear_Revisado,SIGNAL(triggered()),SLOT(activa_revisado()));

    connect(ui->actionAsigna_Fichero_a_Asiento,SIGNAL(triggered()),SLOT(asigna_ficherodocumento()));

    connect(ui->actionBorrar_Asiento, SIGNAL(triggered()),SLOT(borraasiento()));

    connect(ui->actionEditar_Filtro, SIGNAL(triggered()),SLOT(filtrardiario()));

    connect(ui->actionFiltro_inicial, SIGNAL(triggered()),SLOT(filtrar_inicio()));

    connect(ui->actionVisualizar_Documento, SIGNAL(triggered()),SLOT(visdoc()));

    connect(ui->actionCopiar, SIGNAL(triggered()),SLOT(copiar()));
    connect(ui->actionCortar, SIGNAL(triggered()), SLOT(cortar()));
    connect(ui->actionPegar, SIGNAL(triggered()), SLOT(pegar()));

    connect(ui->actionAsientos_en_Espera, SIGNAL(triggered()),SLOT(asientosespera()));
    connect(ui->actionRefrescar,SIGNAL(triggered()),SLOT(refrescardiario()));

    connect(ui->actionImprimir_Diario, SIGNAL(triggered()), SLOT(impridiario()));

    connect(ui->actionConceptos, SIGNAL(triggered()),SLOT(edicionconceptos()));

    connect(ui->actionAsientos_Predefinidos, SIGNAL(triggered()), SLOT(gestautomaticos()));

    connect(ui->actionRegularizacin, SIGNAL(triggered()),SLOT(regulariza()));
    connect(ui->actionCierre_ejercicio, SIGNAL(triggered()), SLOT(cierra()));
    connect(ui->actionRegul_cierre_apertura, SIGNAL(triggered()),SLOT(reg_cierre_apertura()));
    connect(ui->actionDeshacer_Cierre,SIGNAL(triggered()),SLOT(suprimircierre()));
    connect(ui->actionAsiento_de_Apertura,SIGNAL(triggered()),SLOT(apert()));

    connect(ui->actionOrdenar_por_campo,SIGNAL(triggered()),SLOT(ordenarpor_campo()));
    connect(ui->actionTipos_de_Diario,SIGNAL(triggered()),SLOT(tiposdiario()));
    connect(ui->actionEdicin_regul,SIGNAL(triggered()),SLOT(regulparc()));
    connect(ui->actionEjecutar_regul,SIGNAL(triggered()),SLOT(ejecuta_regul()));
    connect(ui->actionImportar_regul,SIGNAL(triggered()),SLOT(importa_reg()));
    connect(ui->actionMayor_a_partir_de_apunte,SIGNAL(triggered()),SLOT(consultamayor()));
    connect(ui->actionMayor_ventana_permanente,SIGNAL(triggered()),SLOT(consultamayor_permanente()));
    connect(ui->actionImprime_mayor,SIGNAL(triggered()),SLOT(imprimemayor()));

    connect(ui->actionBalance_de_Sumas_y_Saldos,SIGNAL(triggered()),SLOT(genbss()));
    connect(ui->actionAcumulados_mes,SIGNAL(triggered()),SLOT(acummes()));

    connect(ui->actionConsulta_Plan,SIGNAL(triggered()),SLOT(consultaplan()));
    connect(ui->actionDiario_Resumen,SIGNAL(triggered()),SLOT(dresumen()));
    connect(ui->actionVolumen_de_operaciones_con_terceras_personas,SIGNAL(triggered()),SLOT(roper()));

    connect(ui->actionConciliacion_de_cuentas,SIGNAL(triggered()),SLOT(conciliacion()));
    connect(ui->actionConceptos_de_diferencias,SIGNAL(triggered()),SLOT(conc_diferencias()));
    connect(ui->actionImportar_cuaderno, SIGNAL(triggered()),SLOT(importac43()));
    connect(ui->actionImporta_C_SV, SIGNAL(triggered()),SLOT(importacsv()));
    connect(ui->actionBorrar_importado,SIGNAL(triggered()),SLOT(borraimportac43()));
    connect(ui->actionConcilia_cuenta_importada,SIGNAL(triggered()),SLOT(conciliacion_imp()));
    connect(ui->actionProcesa_vencimientos_c43,SIGNAL(triggered()),SLOT(contabiliza_venci_c43()));

    connect(ui->actionRetenciones_irpf,SIGNAL(triggered()),SLOT(consulta_retenciones()));
    connect(ui->actionModelo_182,SIGNAL(triggered()),SLOT(roper182()));

    connect(ui->actionVencimientos_a_partir_diario,SIGNAL(triggered()),SLOT(consvencipase()));
    connect(ui->actionProcesar_vencimientos,SIGNAL(triggered()),SLOT(procvencimientos()));
    connect(ui->actionAsignar_tipos_de_vencimientos_a_cuentas,SIGNAL(triggered()),SLOT(tipos_venci_a_cuentas()));
    connect(ui->actionEditar_tipos_de_vencimientos,SIGNAL(triggered()),SLOT(ejecuta_tiposvenci()));
    connect(ui->actionVencimientos_anulados,SIGNAL(triggered()),SLOT(vanul()));
    connect(ui->actionDomiciliaciones,SIGNAL(triggered()),SLOT(domicilia()));

    connect(ui->actionNuevo_Registro_IVA_soportado,SIGNAL(triggered()),SLOT(apunteivasoportado()));
    connect(ui->actionNuevo_Registro_IVA_repercutido,SIGNAL(triggered()),SLOT(apunteivarepercutido()));
    connect(ui->actionAIB_Inversi_n_sujeto_pasivo,SIGNAL(triggered()),SLOT(apunteaib()));
    connect(ui->actionEIB_Prestaci_n_servicos_UE,SIGNAL(triggered()),SLOT(apunteeib()));
    connect(ui->actionTipos_de_IVA,SIGNAL(triggered()),SLOT(qtiposiva()));
    connect(ui->actionCuentas_de_IVA_Soportado,SIGNAL(triggered()),SLOT(cuentasiva_soportado()));
    connect(ui->actionCuentas_de_IVA_Repercutido, SIGNAL(triggered()),SLOT(cuentasiva_repercutido()));
    connect(ui->actionChequea_Libro_Facturas,SIGNAL(triggered()),SLOT(chequea_libro_facturas()));
    connect(ui->actionApuntes_sin_Representacin_en_Libros_de_IVA,SIGNAL(triggered()),SLOT(relacion_apuntes_no_libroiva()));
    connect(ui->actionComprobar_Cuadre_de_Cuotas_con_Diario,SIGNAL(triggered()),SLOT(relacion_cuotas_descuadre()));
    connect(ui->actionLiqudaci_n_de_IVA,SIGNAL(triggered()),SLOT(liqiva()));

    connect(ui->actionRelaci_n_de_Operaciones_seg_n_Libros_de_IVA,SIGNAL(triggered()),SLOT(relopiva()));

    connect(ui->actionLibros_de_Facturas_Recibidas,SIGNAL(triggered()),SLOT(librorecibidas()));
    connect(ui->actionLibro_de_Facturas_Emitidas,SIGNAL(triggered()),SLOT(libroemitidas()));
    connect(ui->actionModelo_340,SIGNAL(triggered()),SLOT(m340()));
    connect(ui->actionModelo_347_F50_Navarra, SIGNAL(triggered()),SLOT(modelo347()));

    connect(ui->actionPrimer_Nivel,SIGNAL(triggered()),SLOT(ci1()));
    connect(ui->actionSegundo_Nivel,SIGNAL(triggered()),SLOT(ci2()));
    connect(ui->actionTercer_Nivel,SIGNAL(triggered()),SLOT(ci3()));
    connect(ui->actionConsulta_Imputaciones,SIGNAL(triggered()),SLOT(cinput()));

    connect(ui->actionCoeficientes_Imputaci_n_Cuenta, SIGNAL(triggered()),SLOT(imputctas()));
    connect(ui->actionVer_Imputaciones,SIGNAL(triggered()),SLOT(verinput_apunte()));
    connect(ui->actionIntercambiar_CI_en_Diario, SIGNAL(triggered()), SLOT(cambiacidiario()));
    connect(ui->actionReasignar_Imputaciones, SIGNAL(triggered()),SLOT(reasig_input()));
    connect(ui->actionCambiar_Imputaciones_a_Cuenta_en_Diario,SIGNAL(triggered()),SLOT(cambia_input_cta_diario()));

    connect(ui->actionGesti_n_Estados_Contables,SIGNAL(triggered()),SLOT(gestestados()));
    connect(ui->actionLibera_C_lculo_Saldos_Medios,SIGNAL(triggered()),SLOT(liberacalcmedios()));
    connect(ui->actionECPN_PYMES,SIGNAL(triggered()),SLOT(ecpn_pymes()));
    connect(ui->actionAsignar_Concepto_a_apuntes, SIGNAL(triggered()),SLOT(concepto_ecpn_pymes()));
    connect(ui->actionColumnas_EVPN, SIGNAL(triggered()),SLOT(qcolumnas_ecpn_pymes()));

    connect(ui->actionXML_Cuentas_PYMES,SIGNAL(triggered()),SLOT(genxbrl()));
    connect(ui->actionXML_Impuesto_Sociedades_PYMES,SIGNAL(triggered()),SLOT(genis()));
    connect(ui->actionProcesar_Memoria,SIGNAL(triggered()),SLOT(procmemoria()));
    connect(ui->actionEditar_Presupuesto_Cuentas_Auxiliares,SIGNAL(triggered()),SLOT(presupuestos()));
    connect(ui->actionEditar_Presupuesto_Cuentas,SIGNAL(triggered()),SLOT(presupuestoscta()));
    connect(ui->actionPrevisiones_Realizaciones_Cuentas_Auxiliares,SIGNAL(triggered()),SLOT(desv()));
    connect(ui->actionPrevisiones_Realizaciones_Cuentas,SIGNAL(triggered()),SLOT(desvctas()));

    connect(ui->actionPlan_de_amortizaciones,SIGNAL(triggered()),SLOT(plamort()));
    connect(ui->actionAsiento_de_amortizaciones,SIGNAL(triggered()),SLOT(asamort()));
    connect(ui->actionDiferencia_en_base_imponible_I_S,SIGNAL(triggered()),SLOT(difbi()));
    connect(ui->actionHist_rico_de_diferencias_en_amortizaciones,SIGNAL(triggered()),SLOT(hamort()));
    connect(ui->actionInmovilizado_neto,SIGNAL(triggered()),SLOT(ineto()));

    connect(ui->actionImportar_Asientos,SIGNAL(triggered()),SLOT(importasientos()));
    connect(ui->actionImportar_Contapls,SIGNAL(triggered()),SLOT(imp_cplus()));
    connect(ui->actionMarcar_como_cerrado_ejercicio,SIGNAL(triggered()),SLOT(marcarcerrado()));
    connect(ui->actionProcesar_fichero_importacin,SIGNAL(triggered()),SLOT(importafich()));

    connect(ui->actionAsignar_Diario,SIGNAL(triggered()),SLOT(asignardiario()));
    connect(ui->actionAgrupar_ficheros_TEX_en_libro,SIGNAL(triggered()),SLOT(librosagr()));
    connect(ui->actionConsultar_ficheros_TEX,SIGNAL(triggered()),SLOT(consultatex()));
    connect(ui->actionGenerar_PDF,SIGNAL(triggered()),SLOT(generarpdf()));
    connect(ui->actionCopia_Seguridad,SIGNAL(triggered()),SLOT(seguridad()));
    connect(ui->actionConsolidaci_n_Contable,SIGNAL(triggered()),SLOT(consolid_sinc()));
    connect(ui->actionRenumerar_Asientos,SIGNAL(triggered()),SLOT(renumera()));
    connect(ui->actionIntercambiar_nmeros_de_asiento,SIGNAL(triggered()),SLOT(intercambiaas()));
    connect(ui->actionBorrar_Intervalo_de_asientos, SIGNAL(triggered()),SLOT(borraint()));
    connect(ui->actionIntercambiar_cuenta_en_diario, SIGNAL(triggered()),SLOT(cambiacuen()));
    connect(ui->actionCambiar_cuenta_a_apunte,SIGNAL(triggered()),SLOT(cambiacuenpase()));
    connect(ui->actionEditar_CI_en_apunte,SIGNAL(triggered()),SLOT(cambiaciapunte()));
    connect(ui->actionEdita_fecha_a_asiento,SIGNAL(triggered()),SLOT(editafechaasien()));
    connect(ui->actionEdita_concepto_y_documento_en_pase,SIGNAL(triggered()),SLOT(edcondoc()));
    //connect(ui->actionCarga_plan_nuevo,SIGNAL(triggered()),SLOT(carganuevoplan()));
    //connect(ui->actionEditar_plan_nuevo,SIGNAL(triggered()),SLOT(ednuevoplan()));
    //connect(ui->actionEditar_equivalencias_nuevo_antiguo, SIGNAL(triggered()),SLOT(equivalenciasplan()));
    //connect(ui->actionSustituir_subcuentas,SIGNAL(triggered()),SLOT(susticuenta()));
    //connect(ui->actionBorrar_plan_nuevo,SIGNAL(triggered()),SLOT(borranuevoplan()));
    connect(ui->actionVerificar_base_de_datos,SIGNAL(triggered()),SLOT(verificaBD()));
    connect(ui->actionRectificar_saldos,SIGNAL(triggered()),SLOT(actusaldos()));

    connect(ui->actionInformaci_n_de_la_conexi_n,SIGNAL(triggered()),SLOT(infoconex()));
    connect(ui->actionAcuerdo_de_licencia,SIGNAL(triggered()),SLOT(acuerdolicencia()));
    connect(ui->actionAcuerdo_de_licencia_en_espa_ol,SIGNAL(triggered()),SLOT(acuerdolicenciaesp()));
    connect(ui->actionModelo_349,SIGNAL(triggered()),SLOT(m349()));
    connect(ui->actionInfo_venci,SIGNAL(triggered()), SLOT(informacion_venci()));
    connect(ui->actionCuentas_inversion_SP,SIGNAL(triggered()),SLOT(ctas_inversion_sp()));

    connect(ui->actionFormas_de_pago,SIGNAL(triggered()),SLOT(edformas_pago()));

    connect(ui->actionImportar_Datos_Cuentas,SIGNAL(triggered(bool)),SLOT(import_ctas_datos()));

    connect(ui->actionRegenera_Instalacion_Archivos,SIGNAL(triggered(bool)),SLOT(regenera_arch_config()));
    connect(ui->actionPapelera,SIGNAL(triggered(bool)),SLOT(edpapelera()));

    connect(ui->actionGestion_Externos,SIGNAL(triggered(bool)),SLOT(gest_externos()));

    connect(ui->actionSII_Facturas_Recibidas,SIGNAL(triggered(bool)),SLOT(gen_sii_recibidas()));
    connect(ui->actionSII_Facturas_Emitidas,SIGNAL(triggered(bool)),SLOT(gen_sii_emitidas()));

    connect(ui->actionImportar_Externos,SIGNAL(triggered(bool)),SLOT(import_externos()));

    connect(ui->actionDirecciones_Servidor_SOAP,SIGNAL(triggered(bool)),SLOT(direc_soap_sii()));

    connect(ui->actionAsignar_E_xterno,SIGNAL(triggered()),SLOT(asigna_externo_apunte()));
    connect(ui->actionCarga_Documentos_en_Base_de_Datos,SIGNAL(triggered(bool)),SLOT(util_carga_docs_bd()));
    connect(ui->actionInventario_inmovilizado,SIGNAL(triggered(bool)),SLOT(inv_inmov()));
    //ui->actionGestion_Externos->setVisible(false);

    connect(ui->actionTipos_de_proveedor,SIGNAL(triggered(bool)),SLOT(tabla_tipo_proveedor()));
    connect(ui->actionHomologacin,SIGNAL(triggered(bool)),SLOT(tabla_homologacion()));
    connect(ui->actionActividades,SIGNAL(triggered(bool)),SLOT(tabla_actividades()));

QApplication::processEvents();
    procesar_inicio();  // carga preferencias y usuario
    check_dir_trabajo();
    if (!(splash==NULL))
     {
      splash->finish( this );
      delete splash;
      splash=NULL;
     }

}

MainWindow::~MainWindow()
{
    if (punterodiario!=NULL)
       {
        if (!noguardarpref)  grabapreferencias();
        delete(punterodiario);
       }
    if (treeWidget!=NULL) delete(treeWidget);
    if (estadosList!=NULL) delete(estadosList);
    if (dockAutomaticos!=NULL) delete(dockAutomaticos);
    if (dockEstados!=NULL) delete(dockEstados);

    delete ui;
}

void MainWindow::icono_papelera()
{
    if (basedatos::instancia()->papelera_vacia())
       {
        QIcon icono(":/iconos/graf/trash-icon.png");
        ui->actionPapelera->setIcon(icono);
       }
       else
           {
            QIcon icono(":/iconos/graf/Recycle-Bin-full-icon.png");
            ui->actionPapelera->setIcon(icono);
           }

}

void MainWindow::conex()
{

// primero nos quedamos con los parámetros de la anterior conexión
// este es un slot para cambiar conexión no para partir desde cero
// pero podemos partir desde 0

    if (s!=NULL) s->close();
    if (c!=NULL) c->close();
    if (b!=NULL) b->close();
    if (s!=NULL) delete(s);
    if (c!=NULL) delete(c);
    if (b!=NULL) delete(b);

    s = NULL; // diálogo de cuentas auxiliares
    c = NULL; // ventana consulta de mayor permanente
    b = NULL; // Ventana bss



 QString anombredb;
 QString anombreusuario;
 QString aclave;
 QString ahost;
 QString acontrolador;
 QString apuerto;

 if (punterodiario!=NULL)
 {
  delete(punterodiario);
  delete(treeWidget);
  // modeloarbol.borraarbol();
  // modeloarbol
  delete(estadosList);
  delete(dockAutomaticos);
  delete(dockEstados);
  punterodiario=NULL;
  treeWidget=NULL;
  estadosList=NULL;
  dockAutomaticos=NULL;
  {
   anombredb = basedatos::instancia()->nombre();
   anombreusuario = basedatos::instancia()->usuario();
   aclave = basedatos::instancia()->clave();
   ahost = basedatos::instancia()->host();
   acontrolador = basedatos::instancia()->controlador();
   apuerto.setNum( basedatos::instancia()->puerto() );
   basedatos::instancia()->cerrar(); //linea nueva
   // bd.removeDatabase(anombredb);
  }
   QSqlDatabase::removeDatabase("qt_sql_default_connection");
 }


// destruimos diario antiguo
// abría que quitarlo primero de centralwidget ??
// setCentralWidget(NULL);
// dialogo de conexión más conexión
// QSqlDatabase::removeDatabase(anombredb);

conexion *laconex = new conexion;
laconex->pasaarchconex(fichconexiones);

int resultado=laconex->exec();
if (resultado==QDialog::Accepted)
  {
    QString nbd;
    if (laconex->controlador().contains("SQLITE"))
     {
      for (int veces=0; veces<conexion_rutas.count(); veces++)
          {
            if (conexion_rutas.at(veces)==laconex->basedatos().toLower())
              {
                nbd=rutas_trabajo.at(veces);
                break;
              }
          }

      if (nbd.isEmpty()) nbd=dirtrabajobd();
      nbd+=QDir::separator();
     }
   nbd+=laconex->basedatos().toLower();
   //QMessageBox::warning(this, tr("KEME-Contabilidad"),nbd);
  // if (laconex->controlador().contains("SQLITE")) nbd=adapta(nbd);

   if (laconex->controlador().contains("SQLITE"))
      {
       if (!QFile::exists(nbd))
          {
           QMessageBox::warning(this, tr("KEME-Contabilidad"),
                 tr("No existe la base de datos especificada"));
           if (anombredb.isEmpty()) return;
           abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);

           // falta cargar el diario, etc.
           actudatosprincipal();
           usuario.clear();
           laconex->actulistaconex();
           laconex->actufichconex();
           delete(laconex);
           if (!cargausuario())
                {
                  QMessageBox::warning(this, tr("KEME-Contabilidad"),
                     tr("No se ha podido entrar con el usuario especificado"));
                  if (anombredb.isEmpty()) return;
                  abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
                }
                else
                    haygestionusuarios=basedatos::instancia()->gestiondeusuarios();
           if (anombredb.isEmpty()) activaTodo();
           if (!haygestionusuarios)
             {
               usuario.clear();
               resetprivilegios();
               actuprivilegios();
             }
           icono_papelera();
           return;
          }
      }

   if (!abrebasedatos(laconex->host(),
                   nbd,
                   laconex->usuario().toLower(),
                   laconex->clave(),
                   laconex->controlador(),
                   laconex->puerto()))
       {
        // Mensaje no se ha podido abrir; abrimos la antigua
        QMessageBox::warning(this, tr("KEME-Contabilidad"),
                 tr("No se ha podido abrir la base de datos especificada"));
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
        if (anombredb.isEmpty()) return;
        abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
       }
        else
              {
               usuario.clear();
               laconex->actulistaconex();
               laconex->actufichconex();

               if (!cargausuario())
                {
                  QMessageBox::warning(this, tr("KEME-Contabilidad"),
                     tr("No se ha podido entrar con el usuario especificado"));
                  if (anombredb.isEmpty()) return;
                  abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
                }
                 else haygestionusuarios=basedatos::instancia()->gestiondeusuarios();
               if (anombredb.isEmpty()) activaTodo();
               if (!haygestionusuarios)
                  {
                   usuario.clear();
                   resetprivilegios();
                   actuprivilegios();
                  }
               icono_papelera();
               if (!basedatos::instancia()->hay_externos())
                 {
                  ui->actionGestion_Externos->setVisible(false);
                  ui->actionAsignar_E_xterno->setVisible(false);
                 }
                 else
                     {
                      ui->actionGestion_Externos->setVisible(true);
                      ui->actionAsignar_E_xterno->setVisible(true);
                     }
              }
   }
    else if (!anombredb.isEmpty()) abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
delete(laconex);
actudatosprincipal();

}

void MainWindow::actudatosprincipal()
{
  QString msj_titulo=tr(APLICACION);
  if (!titulo.isEmpty()) msj_titulo=titulo+" - ";
if (basedatos::instancia()->gestiondeusuarios())
  setWindowTitle(msj_titulo+nombreempresa()+" - Usuario: "+usuario);
 else
     setWindowTitle(msj_titulo+nombreempresa());
// calculaprimerosasientos();
 setUnifiedTitleAndToolBarOnMac(true);

diario *eldiario = new diario;
punterodiario=eldiario;
setCentralWidget(eldiario);
QString cadena;
filtroactivo=condicionesfiltrodefecto();
eldiario->pasafiltroedlin(filtro_a_lingu(filtroactivo));
if (filtroactivo.length()>0) cadena=" WHERE "+filtroactivo+" ";
if (filtroactivo.isEmpty()) cadena="where contabilizado ";
  else cadena+=" and contabilizado ";
cadena+=ordenarpor();
eldiario->activaconfiltro(cadena,estilonumerico,sindecimales,usuario);
eldiario->activaconfiltrob(filtroactivob,estilonumerico,sindecimales);
eldiario->pasaanchos(anchocol);
eldiario->muestratabla();
punterodiario->cabecera_campo_orden(n_campo_orden());
on_diario_tab_cambiado(0);
connect(eldiario, SIGNAL(cambio_tab(int)),this, SLOT(on_diario_tab_cambiado(int)));
connect(eldiario, SIGNAL(filedroped()),this, SLOT(fichero_soltado_en_diario()));
imgdiario();
creaDockWindows();
}






void MainWindow::procesar_inicio()
{
    // intentamos cargar preferencias en configuración
    // apertura de base de datos
    // fijar acciones activadas según apertura de base de datos y privilegios de usuario
    bool bdabierta=false;
    bool hayfichpref=false;
    if (!cargapreferencias(&bdabierta, &hayfichpref) || !bdabierta )
       {
        // desactivamos todas las acciones excepto conexión, acerca de, etc.
        activa_sinconexion();
        return;
       }
    // bd está abierta
    activaTodo();
    if (basedatos::instancia()->papelera_vacia())
       {
        QIcon icono(":/iconos/graf/trash-icon.png");
        ui->actionPapelera->setIcon(icono);
       }
       else
           {
            QIcon icono(":/iconos/graf/Recycle-Bin-full-icon.png");
            ui->actionPapelera->setIcon(icono);
           }
    bool gestusuarios=basedatos::instancia()->gestiondeusuarios();
    if (!gestusuarios)
       {
        resetprivilegios();
        actuprivilegios();
       }
    if (guardausuario && gestusuarios)
       {
        // QMessageBox::warning( this, tr("usuarios"), usuario + " " + claveusuario);
        if (basedatos::instancia()->claveusuariook(usuario, claveusuario))
           {
                // cargamos privilegios ... privilegios[62];
                QString privileg=basedatos::instancia()->privilegios(usuario);
                for (int veces=0; veces<NUMPRIVILEGIOS; veces++)
                  {
                    privilegios[veces]= privileg[veces]=='1' ? true : false;
                    // privilegios_global[veces]=privilegios[veces];
                  }
                // QMessageBox::warning( this, tr("TABLA DE ASIENTOS"), privileg);
                // actualizar con método menús
                actuprivilegios();
           }
         else {
               // dejar solo activado conexión y cambio de usuario
               activa_conexion_no_usuario();
               return;
              }
        }
    if (!guardausuario && gestusuarios)
     if (!cargausuario())
      {
         // dejar solo activado conexión y cambio de usuario
         activa_conexion_no_usuario();
         ui->actionConexion->setEnabled(false);
        return;
      }
    // conexión y usuario OK
    // llamamos a función aquí para cargar el diario y docks
    if (!basedatos::instancia()->hay_externos())
      {
       ui->actionGestion_Externos->setVisible(false);
       ui->actionAsignar_E_xterno->setVisible(false);
      }
    activaWidgets();
}


void MainWindow::activaWidgets()
{
    if (punterodiario!=NULL)
       {
        delete(punterodiario);
       }
    if (treeWidget!=NULL) delete(treeWidget);
    if (estadosList!=NULL) delete(estadosList);
    if (dockAutomaticos!=NULL) delete(dockAutomaticos);
    if (dockEstados!=NULL) delete(dockEstados);

    diario *eldiario = new diario;
    punterodiario=eldiario;
    setCentralWidget(eldiario);
    QString cadena;
    filtroactivo=condicionesfiltrodefecto();
    eldiario->pasafiltroedlin(filtro_a_lingu(filtroactivo));
    if (filtroactivo.length()>0) cadena=" WHERE "+filtroactivo+" ";
    if (filtroactivo.isEmpty()) cadena="where contabilizado ";
      else cadena+=" and contabilizado ";

    cadena+=ordenarpor();
    if (!basedatos::instancia()->gestiondeusuarios()) usuario.clear();
    eldiario->activaconfiltro(cadena,estilonumerico,sindecimales,usuario);
    eldiario->activaconfiltrob("",estilonumerico,sindecimales);
    eldiario->pasaanchos(anchocol);
    punterodiario->cabecera_campo_orden(n_campo_orden());
    eldiario->muestratabla();
    creaDockWindows();
    if (!basedatos::instancia()->con_amoinv()) ui->actionInventario_inmovilizado->setVisible(false);
/*
    if (hayfich)
       {
        splash->finish( this );
        delete splash;
     }
*/
    imgdiario();
    on_diario_tab_cambiado(0);
    connect(eldiario, SIGNAL(cambio_tab(int )),this, SLOT(on_diario_tab_cambiado(int )));
    connect(eldiario, SIGNAL(filedroped()),this, SLOT(fichero_soltado_en_diario()));

    QString msj_titulo=tr(APLICACION);
    if (!titulo.isEmpty()) msj_titulo=titulo+" - ";
    if (basedatos::instancia()->gestiondeusuarios())
      setWindowTitle(msj_titulo+nombreempresa()+" - Usuario: "+usuario);
     else
      setWindowTitle(msj_titulo+nombreempresa());
    // if (cplus!="1") ui.actionImportar_Contaplus->setVisible(false);
    // if (cdomiciliaciones!="1") ui.actionDomiciliaciones->setVisible(false);
    setUnifiedTitleAndToolBarOnMac(true);


}


QString MainWindow::condicionesfiltrodefecto(void)
{
 QString cadfiltro;
 QDate fechaactual=QDate::currentDate();
 QDate fecha;
    if (filtrarpor==1)
      {
         QString ejercicio=ejerciciodelafecha(fechaactual);
         if (ejercicio.length()>0)
            {
               cadfiltro="fecha>='";
               cadfiltro+=inicioejercicio(ejercicio).toString("yyyy-MM-dd");
               cadfiltro+="' and fecha<='";
               cadfiltro+=finejercicio(ejercicio).toString("yyyy-MM-dd");
               cadfiltro+="'";
            }
      }
   if (filtrarpor==2)
      {
        fecha=fechaactual.addDays(-30);
        cadfiltro="fecha>='";
        cadfiltro+=fecha.toString("yyyy-MM-dd");
        cadfiltro+="'";
     }
   if (filtrarpor==3)
     {
       // primero encontramos máxima fecha del diario
       QDate maxfecha=basedatos::instancia()->maxfecha_diario();
       QString ejercicio=ejerciciodelafecha(maxfecha);
       if (!ejercicio.isEmpty())
          {
           qlonglong vnum = basedatos::instancia()->proximoasiento(ejercicio);
           QString cadnum; cadnum.setNum(vnum-1);
           cadfiltro="asiento>";
           cadfiltro+=cadnum;
           cadfiltro+="-30";
           cadfiltro+=" and ejercicio='";
           cadfiltro+=ejercicio.left(-1).replace("'","''");
           cadfiltro+="'";
          }
     }
   if (filtropersonalizado.length()>0)
    {
        cadfiltro=filtropersonalizado;
    }

  return cadfiltro;
}


void MainWindow::imgdiario()
{
   if (basedatos::instancia()->gestiondeusuarios())
    {
                 QString imagen=basedatos::instancia()->imagenusuario(usuario);
                 QPixmap foto;
                 if (imagen.length()>0)
                    {
                     QByteArray byteshexa;
                     byteshexa.append ( imagen.toUtf8() );
                     QByteArray bytes;
                     bytes=bytes.fromHex ( byteshexa );
                     QPixmap carga;
                     carga.loadFromData ( bytes, "PNG");
                     foto=carga.scaledToHeight(30);
                    } else punterodiario->resetimagen();
                 punterodiario->asignaimagen(foto);
    }
    else
         {
           QPixmap foto;
           punterodiario->asignaimagen(foto);
           punterodiario->resetimagen();
         }
}


void MainWindow::creaDockWindows()
{

 dockAutomaticos = new QDockWidget(tr("Asientos automáticos"),this);
 dockAutomaticos->setAllowedAreas(Qt::RightDockWidgetArea);
 // automaticosList = new QListWidget(dockAutomaticos);
 treeWidget = new QTreeWidget(dockAutomaticos);
 QStringList columnas;
 columnas << tr("CÓDIGO");
 treeWidget->setHeaderLabels(columnas);

 cargaarbol();
 //treeView->setModel(&modeloarbol);
 //treeView->expandAll();
 treeWidget->resizeColumnToContents(0);
 // treeView->resizeColumnToContents(0);
 treeWidget->setAlternatingRowColors(true);
 // treeView->setEditTriggers ( QAbstractItemView::NoEditTriggers );

 dockAutomaticos->setWidget(treeWidget);
 if (haydockautomaticos) addDockWidget(Qt::RightDockWidgetArea, dockAutomaticos);
    else
        {
         // modeloarbol.borraarbol();
         dockAutomaticos->close();
        }
 // ui.menuVer->addAction(dockAutomaticos->toggleViewAction());

 // ---------------------------------------------------------------------------------------

 dockEstados = new QDockWidget(tr("Estados contables"),this);
 dockEstados->setAllowedAreas(Qt::RightDockWidgetArea);

 QSqlQuery query = basedatos::instancia()->selectTitulocabeceraestadosordertitulo();
 estadosList = new QListWidget(dockEstados);

 if ( query.isActive() ) {
          while ( query.next() )
                   estadosList->addItem (query.value(0).toString()) ;
      }
 estadosList->setSizePolicy ( QSizePolicy::Preferred, QSizePolicy::Preferred );
 dockEstados->setWidget(estadosList);
 if (haydockestados) addDockWidget(Qt::RightDockWidgetArea, dockEstados);
    else dockEstados->close();

 if (estadosList->count()>0) estadosList->setCurrentRow(0);

 connect(treeWidget, SIGNAL(clicked(QModelIndex)) , this,
         SLOT(ejecuta_automatico()));
 connect(estadosList, SIGNAL(itemActivated ( QListWidgetItem * ) ), this,
         SLOT(cal_estado(QListWidgetItem *)));


}


void MainWindow::cargaarbol()
{
    QStringList lista_codigo;
    QList<QTreeWidgetItem*> lista_puntero_item;

    QStringList lista= basedatos::instancia()->tipos_automaticos();
    for (int veces=0; veces<lista.count(); veces++)
       {
        QStringList datos;
        datos << lista.at(veces);
        lista_codigo << lista.at(veces);
        QTreeWidgetItem *elemento = new QTreeWidgetItem(treeWidget,datos);
        elemento->setExpanded(true);
        lista_puntero_item << elemento;
       }

  //  TreeItem *elemento;
    QSqlQuery query = basedatos::instancia()->selectAsientomodelocab_as_modeloorderasientomodelo();
    if ( query.isActive() ) {
             while ( query.next() )
                {
                 for (int veces=0; veces<lista_codigo.count(); veces++)
                     {
                      if (lista_codigo.at(veces)==query.value(1).toString())
                         {
                          QStringList datos;
                          datos << query.value(0).toString();
                          new QTreeWidgetItem(lista_puntero_item.at(veces),datos);
                          break;
                         }
                     }
                 }
             }

}

void MainWindow::activaTodo()
{
    ui->menuEditar_tablas_AEAT_Mercantiles->setEnabled(true);
    ui->menu_tiles->setEnabled(true);

    ui->actionOrden_numero->setEnabled(true);
    ui->actionPreferencias->setEnabled(true);
    ui->actionConfiguraci_n->setEnabled(true);
    ui->actionEjercicios->setEnabled(true);
    ui->actionCrear_Empresa->setEnabled(true);
    ui->actionBorrar_Empresa->setEnabled(true);
    ui->actionCopiar_Empresa->setEnabled(true);
    ui->actionConsultar_Notas->setEnabled(true);
    ui->actionEditar_Notas->setEnabled(true);
    ui->actionHabilitar_Deshabilitar->setEnabled(true);
    ui->actionCambio_usuario->setEnabled(true);
    ui->actionGest_usuarios->setEnabled(true);
    ui->actionCambiar_Clave->setEnabled(true);
    ui->actionImportar_usuarios->setEnabled(true);
    ui->actionCuentas->setEnabled(true);
    ui->actionCuentas_auxiliares->setEnabled(true);
    ui->actionDatos_accesorios_cuentas->setEnabled(true);
    ui->actionAsignar_IVA_retenci_n_a_cuentas->setEnabled(true);
    ui->actionBloquear_cuentas->setEnabled(true);
    ui->actionExportar_plan->setEnabled(true);
    ui->actionExportarplan_cuentas_auxiliares->setEnabled(true);
    ui->actionImportar_plan->setEnabled(true);
    ui->actionNuevo_Asiento->setEnabled(true);
    ui->actionEditar_Asiento->setEnabled(true);
    ui->actionEditar_Asiento_N_mero->setEnabled(true);
    ui->actionConsulta_Asiento->setEnabled(true);
    ui->actionConsulta_Asiento_N_mero->setEnabled(true);
    ui->actionChequear_Revisado->setEnabled(true);
    ui->actionAsigna_Fichero_a_Asiento->setEnabled(true);
    ui->actionBorrar_Asiento->setEnabled(true);
    ui->actionEditar_Filtro->setEnabled(true);
    ui->actionFiltro_inicial->setEnabled(true);
    ui->actionVisualizar_Documento->setEnabled(true);
    ui->actionCopiar->setEnabled(true);
    ui->actionCortar->setEnabled(true);
    ui->actionPegar->setEnabled(true);
    ui->actionAsientos_en_Espera->setEnabled(true);
    ui->actionRefrescar->setEnabled(true);
    ui->actionImprimir_Diario->setEnabled(true);
    ui->actionConceptos->setEnabled(true);
    ui->actionAsientos_Predefinidos->setEnabled(true);
    ui->actionRegularizacin->setEnabled(true);
    ui->actionCierre_ejercicio->setEnabled(true);
    ui->actionRegul_cierre_apertura->setEnabled(true);
    ui->actionDeshacer_Cierre->setEnabled(true);
    ui->actionAsiento_de_Apertura->setEnabled(true);
    ui->actionOrdenar_por_campo->setEnabled(true);
    ui->actionTipos_de_Diario->setEnabled(true);
    ui->actionEdicin_regul->setEnabled(true);
    ui->actionEjecutar_regul->setEnabled(true);
    ui->actionImportar_regul->setEnabled(true);
    ui->actionMayor_a_partir_de_apunte->setEnabled(true);
    ui->actionMayor_ventana_permanente->setEnabled(true);
    ui->actionImprime_mayor->setEnabled(true);
    ui->actionBalance_de_Sumas_y_Saldos->setEnabled(true);
    ui->actionAcumulados_mes->setEnabled(true);
    ui->actionConsulta_Plan->setEnabled(true);
    ui->actionDiario_Resumen->setEnabled(true);
    ui->actionVolumen_de_operaciones_con_terceras_personas->setEnabled(true);
    ui->actionConciliacion_de_cuentas->setEnabled(true);
    ui->actionConceptos_de_diferencias->setEnabled(true);
    ui->actionImportar_cuaderno->setEnabled(true);
    ui->actionBorrar_importado->setEnabled(true);
    ui->actionConcilia_cuenta_importada->setEnabled(true);
    ui->actionProcesa_vencimientos_c43->setEnabled(true);
    ui->actionRetenciones_irpf->setEnabled(true);
    ui->actionModelo_182->setEnabled(true);
    ui->actionVencimientos_a_partir_diario->setEnabled(true);
    ui->actionProcesar_vencimientos->setEnabled(true);
    ui->actionAsignar_tipos_de_vencimientos_a_cuentas->setEnabled(true);
    ui->actionEditar_tipos_de_vencimientos->setEnabled(true);
    ui->actionDomiciliaciones->setEnabled(true);
    ui->actionVencimientos_anulados->setEnabled(true);
    ui->actionNuevo_Registro_IVA_soportado->setEnabled(true);
    ui->actionNuevo_Registro_IVA_repercutido->setEnabled(true);
    ui->actionAIB_Inversi_n_sujeto_pasivo->setEnabled(true);
    ui->actionEIB_Prestaci_n_servicos_UE->setEnabled(true);
    ui->actionTipos_de_IVA->setEnabled(true);
    ui->actionCuentas_de_IVA_Soportado->setEnabled(true);
    ui->actionCuentas_de_IVA_Repercutido->setEnabled(true);
    ui->actionChequea_Libro_Facturas->setEnabled(true);
    ui->actionApuntes_sin_Representacin_en_Libros_de_IVA->setEnabled(true);
    ui->actionComprobar_Cuadre_de_Cuotas_con_Diario->setEnabled(true);
    ui->actionLiqudaci_n_de_IVA->setEnabled(true);
    ui->actionRelaci_n_de_Operaciones_seg_n_Libros_de_IVA->setEnabled(true);
    ui->actionLibros_de_Facturas_Recibidas->setEnabled(true);
    ui->actionLibro_de_Facturas_Emitidas->setEnabled(true);
    ui->actionModelo_340->setEnabled(true);
    ui->actionModelo_347_F50_Navarra->setEnabled(true);
    ui->actionPrimer_Nivel->setEnabled(true);
    ui->actionSegundo_Nivel->setEnabled(true);
    ui->actionTercer_Nivel->setEnabled(true);
    ui->actionConsulta_Imputaciones->setEnabled(true);
    ui->actionCoeficientes_Imputaci_n_Cuenta->setEnabled(true);
    ui->actionVer_Imputaciones->setEnabled(true);
    ui->actionIntercambiar_CI_en_Diario->setEnabled(true);
    ui->actionReasignar_Imputaciones->setEnabled(true);
    ui->actionCambiar_Imputaciones_a_Cuenta_en_Diario->setEnabled(true);
    ui->actionGesti_n_Estados_Contables->setEnabled(true);
    ui->actionLibera_C_lculo_Saldos_Medios->setEnabled(true);
    ui->actionECPN_PYMES->setEnabled(true);
    ui->actionAsignar_Concepto_a_apuntes->setEnabled(true);
    ui->actionXML_Cuentas_PYMES->setEnabled(true);
    ui->actionXML_Impuesto_Sociedades_PYMES->setEnabled(true);
    ui->actionProcesar_Memoria->setEnabled(true);
    ui->actionEditar_Presupuesto_Cuentas_Auxiliares->setEnabled(true);
    ui->actionEditar_Presupuesto_Cuentas->setEnabled(true);
    ui->actionPrevisiones_Realizaciones_Cuentas_Auxiliares->setEnabled(true);
    ui->actionPrevisiones_Realizaciones_Cuentas->setEnabled(true);
    ui->actionPlan_de_amortizaciones->setEnabled(true);
    ui->actionAsiento_de_amortizaciones->setEnabled(true);
    ui->actionDiferencia_en_base_imponible_I_S->setEnabled(true);
    ui->actionHist_rico_de_diferencias_en_amortizaciones->setEnabled(true);
    ui->actionInmovilizado_neto->setEnabled(true);
    ui->actionImportar_Asientos->setEnabled(true);
    ui->actionImportar_Contapls->setEnabled(true);
    ui->actionMarcar_como_cerrado_ejercicio->setEnabled(true);
    ui->actionProcesar_fichero_importacin->setEnabled(true);
    ui->actionAsignar_Diario->setEnabled(true);
    ui->actionAgrupar_ficheros_TEX_en_libro->setEnabled(true);
    ui->actionConsultar_ficheros_TEX->setEnabled(true);
    ui->actionGenerar_PDF->setEnabled(true);
    ui->actionCopia_Seguridad->setEnabled(true);
    ui->actionConsolidaci_n_Contable->setEnabled(true);
    ui->actionInformaci_n_de_la_conexi_n->setEnabled(true);
    ui->actionFormas_de_pago->setEnabled(true);

    ui->actionImporta_C_SV->setEnabled(true);
    ui->actionPapelera->setEnabled(true);
    ui->actionInfo_venci->setEnabled(true);
    ui->actionCuentas_inversion_SP->setEnabled(true);
    ui->actionModelo_349->setEnabled(true);
    ui->actionAsignar_E_xterno->setEnabled(true);

}

void MainWindow::activa_sinconexion()
{
 ui->actionConexion->setEnabled(true);
 ui->actionAcerca_de->setEnabled(true);
 ui->action_Salir->setEnabled(true);
 ui->actionAcuerdo_de_licencia->setEnabled(true);
 ui->actionAcuerdo_de_licencia_en_espa_ol->setEnabled(true);
 ui->actionCrear_Empresa->setEnabled(true);

 ui->menuEditar_tablas_AEAT_Mercantiles->setEnabled(false);
 ui->menu_tiles->setEnabled(false);

 ui->actionOrden_numero->setEnabled(false);
 ui->actionPreferencias->setEnabled(false);
 ui->actionConfiguraci_n->setEnabled(false);
 ui->actionEjercicios->setEnabled(false);
 //ui->actionCrear_Empresa->setEnabled(false);
 ui->actionBorrar_Empresa->setEnabled(false);
 ui->actionCopiar_Empresa->setEnabled(false);
 ui->actionConsultar_Notas->setEnabled(false);
 ui->actionEditar_Notas->setEnabled(false);
 ui->actionHabilitar_Deshabilitar->setEnabled(false);
 ui->actionCambio_usuario->setEnabled(false);
 ui->actionGest_usuarios->setEnabled(false);
 ui->actionCambiar_Clave->setEnabled(false);
 ui->actionImportar_usuarios->setEnabled(false);
 ui->actionCuentas->setEnabled(false);
 ui->actionCuentas_auxiliares->setEnabled(false);
 ui->actionGestion_Externos->setEnabled(false);
 ui->actionDatos_accesorios_cuentas->setEnabled(false);
 ui->actionAsignar_IVA_retenci_n_a_cuentas->setEnabled(false);
 ui->actionBloquear_cuentas->setEnabled(false);
 ui->actionExportar_plan->setEnabled(false);
 ui->actionExportarplan_cuentas_auxiliares->setEnabled(false);
 ui->actionImportar_plan->setEnabled(false);
 ui->actionNuevo_Asiento->setEnabled(false);
 ui->actionEditar_Asiento->setEnabled(false);
 ui->actionEditar_Asiento_N_mero->setEnabled(false);
 ui->actionConsulta_Asiento->setEnabled(false);
 ui->actionConsulta_Asiento_N_mero->setEnabled(false);
 ui->actionChequear_Revisado->setEnabled(false);
 ui->actionAsigna_Fichero_a_Asiento->setEnabled(false);
 ui->actionBorrar_Asiento->setEnabled(false);
 ui->actionEditar_Filtro->setEnabled(false);
 ui->actionFiltro_inicial->setEnabled(false);
 ui->actionVisualizar_Documento->setEnabled(false);
 ui->actionCopiar->setEnabled(false);
 ui->actionCortar->setEnabled(false);
 ui->actionPegar->setEnabled(false);
 ui->actionAsientos_en_Espera->setEnabled(false);
 ui->actionRefrescar->setEnabled(false);
 ui->actionImprimir_Diario->setEnabled(false);
 ui->actionConceptos->setEnabled(false);
 ui->actionAsientos_Predefinidos->setEnabled(false);
 ui->actionRegularizacin->setEnabled(false);
 ui->actionCierre_ejercicio->setEnabled(false);
 ui->actionRegul_cierre_apertura->setEnabled(false);
 ui->actionDeshacer_Cierre->setEnabled(false);
 ui->actionAsiento_de_Apertura->setEnabled(false);
 ui->actionOrdenar_por_campo->setEnabled(false);
 ui->actionTipos_de_Diario->setEnabled(false);
 ui->actionEdicin_regul->setEnabled(false);
 ui->actionEjecutar_regul->setEnabled(false);
 ui->actionImportar_regul->setEnabled(false);
 ui->actionMayor_a_partir_de_apunte->setEnabled(false);
 ui->actionMayor_ventana_permanente->setEnabled(false);
 ui->actionImprime_mayor->setEnabled(false);
 ui->actionBalance_de_Sumas_y_Saldos->setEnabled(false);
 ui->actionAcumulados_mes->setEnabled(false);
 ui->actionConsulta_Plan->setEnabled(false);
 ui->actionDiario_Resumen->setEnabled(false);
 ui->actionVolumen_de_operaciones_con_terceras_personas->setEnabled(false);
 ui->actionConciliacion_de_cuentas->setEnabled(false);
 ui->actionConceptos_de_diferencias->setEnabled(false);
 ui->actionImportar_cuaderno->setEnabled(false);
 ui->actionBorrar_importado->setEnabled(false);
 ui->actionConcilia_cuenta_importada->setEnabled(false);
 ui->actionProcesa_vencimientos_c43->setEnabled(false);
 ui->actionRetenciones_irpf->setEnabled(false);
 ui->actionModelo_182->setEnabled(false);
 ui->actionModelo_182_registro_donaciones->setEnabled(false);
 ui->actionVencimientos_a_partir_diario->setEnabled(false);
 ui->actionProcesar_vencimientos->setEnabled(false);
 ui->actionAsignar_tipos_de_vencimientos_a_cuentas->setEnabled(false);
 ui->actionEditar_tipos_de_vencimientos->setEnabled(false);
 ui->actionDomiciliaciones->setEnabled(false);
 ui->actionFicheros_de_transferencias->setEnabled(false);
 ui->actionVencimientos_anulados->setEnabled(false);
 ui->actionNuevo_Registro_IVA_soportado->setEnabled(false);
 ui->actionNuevo_Registro_IVA_repercutido->setEnabled(false);
 ui->actionAIB_Inversi_n_sujeto_pasivo->setEnabled(false);
 ui->actionEIB_Prestaci_n_servicos_UE->setEnabled(false);
 ui->actionTipos_de_IVA->setEnabled(false);
 ui->actionCuentas_de_IVA_Soportado->setEnabled(false);
 ui->actionCuentas_de_IVA_Repercutido->setEnabled(false);
 ui->actionChequea_Libro_Facturas->setEnabled(false);
 ui->actionApuntes_sin_Representacin_en_Libros_de_IVA->setEnabled(false);
 ui->actionComprobar_Cuadre_de_Cuotas_con_Diario->setEnabled(false);
 ui->actionLiqudaci_n_de_IVA->setEnabled(false);
 ui->actionRelaci_n_de_Operaciones_seg_n_Libros_de_IVA->setEnabled(false);
 ui->actionLibros_de_Facturas_Recibidas->setEnabled(false);
 ui->actionLibro_de_Facturas_Emitidas->setEnabled(false);
 ui->actionModelo_340->setEnabled(false);
 ui->actionModelo_347_F50_Navarra->setEnabled(false);
 ui->actionPrimer_Nivel->setEnabled(false);
 ui->actionSegundo_Nivel->setEnabled(false);
 ui->actionTercer_Nivel->setEnabled(false);
 ui->actionConsulta_Imputaciones->setEnabled(false);
 ui->actionCoeficientes_Imputaci_n_Cuenta->setEnabled(false);
 ui->actionVer_Imputaciones->setEnabled(false);
 ui->actionIntercambiar_CI_en_Diario->setEnabled(false);
 ui->actionReasignar_Imputaciones->setEnabled(false);
 ui->actionCambiar_Imputaciones_a_Cuenta_en_Diario->setEnabled(false);
 ui->actionGesti_n_Estados_Contables->setEnabled(false);
 ui->actionLibera_C_lculo_Saldos_Medios->setEnabled(false);
 ui->actionECPN_PYMES->setEnabled(false);
 ui->actionAsignar_Concepto_a_apuntes->setEnabled(false);
 ui->actionXML_Cuentas_PYMES->setEnabled(false);
 ui->actionXML_Impuesto_Sociedades_PYMES->setEnabled(false);
 ui->actionProcesar_Memoria->setEnabled(false);
 ui->actionEditar_Presupuesto_Cuentas_Auxiliares->setEnabled(false);
 ui->actionEditar_Presupuesto_Cuentas->setEnabled(false);
 ui->actionPrevisiones_Realizaciones_Cuentas_Auxiliares->setEnabled(false);
 ui->actionPrevisiones_Realizaciones_Cuentas->setEnabled(false);
 ui->actionPlan_de_amortizaciones->setEnabled(false);
 ui->actionAsiento_de_amortizaciones->setEnabled(false);
 ui->actionDiferencia_en_base_imponible_I_S->setEnabled(false);
 ui->actionHist_rico_de_diferencias_en_amortizaciones->setEnabled(false);
 ui->actionInmovilizado_neto->setEnabled(false);
 ui->actionImportar_Asientos->setEnabled(false);
 ui->actionImportar_Contapls->setEnabled(false);
 ui->actionMarcar_como_cerrado_ejercicio->setEnabled(false);
 ui->actionProcesar_fichero_importacin->setEnabled(false);
 ui->actionAsignar_Diario->setEnabled(false);
 ui->actionAgrupar_ficheros_TEX_en_libro->setEnabled(false);
 ui->actionConsultar_ficheros_TEX->setEnabled(false);
 ui->actionGenerar_PDF->setEnabled(false);
 ui->actionCopia_Seguridad->setEnabled(false);
 ui->actionConsolidaci_n_Contable->setEnabled(false);
 ui->actionInformaci_n_de_la_conexi_n->setEnabled(false);
 ui->actionFormas_de_pago->setEnabled(false);

 ui->actionImporta_C_SV->setEnabled(false);
 ui->actionPapelera->setEnabled(false);
 ui->actionInfo_venci->setEnabled(false);
 ui->actionCuentas_inversion_SP->setEnabled(false);
 ui->actionModelo_349->setEnabled(false);
 ui->actionAsignar_E_xterno->setEnabled(false);

}

void MainWindow::activa_conexion_no_usuario()
{
    activa_sinconexion();
    ui->actionCrear_Empresa->setEnabled(false);
    // falta activar cambio usuario
    ui->actionCambio_usuario->setEnabled(true);

}


bool MainWindow::cargausuario()
{
     if (basedatos::instancia()->gestiondeusuarios())
        {
         // repetir dos veces petición de clave hasta correcta
         if (!(splash==NULL))
          {
           splash->finish( this );
           delete splash;
           splash=NULL;
          }

         login *l = new login();
         l->setWindowTitle(nombreempresa());
         int veces=1;
         bool valido=false;
         while (veces<3)
           {
             int resultado=l->exec();
             if (resultado != QDialog::Accepted) return false;
             if (basedatos::instancia()->claveusuariook(l->usuario(), l->clave()))
                {
                 valido=true;
                 usuario=l->usuario();
                 claveusuario=l->clave();
                 // cargamos privilegios ... privilegios[62];
                 QString privileg=basedatos::instancia()->privilegios(l->usuario());
                 for (int veces=0; veces<NUMPRIVILEGIOS; veces++)
                    {
                     privilegios[veces]= privileg[veces]=='1' ? true : false;
                     // privilegios_global[veces]=privilegios[veces];
                    }
                 // QMessageBox::warning( this, tr("TABLA DE ASIENTOS"), privileg);
                 // actualizar con método menús
                 actuprivilegios();
                 // QMessageBox::warning( this, tr("TABLA DE ASIENTOS"), privileg);
                 // asignamos imagen
                 break;
                }
             veces++;
           }
         delete (l);
         if (!valido)
            {
              QMessageBox::warning( this, tr("CONTROL DE ACCESO"),
               tr("ERROR: CÓDIGO/CLAVE ERRÓNEOS"));
              return false;
            }
     } else { usuario.clear(); resetprivilegios(); }
     return true;
 }


void MainWindow::actuprivilegios()
{
  /*   enum  privilegios {
             p_configuracion,      ejercicios_contables,   gestion_periodos,      edita_notas,
             consulta_notas,       cambiar_conexion_bd,    gestusuarios,          crea_borra_bd,
             copiar_empresa,       tablas_aeat,            plan,                  cuentas_auxiliares,
             datos_accesorios_cta, asigna_iva_ret_cta,     bloquear_cuentas,      exportar_plan,
             importar_plan,        nuev_asiento,           edi_asiento,           marcar_revisado,
             asigna_fichero,       borrar_asiento,         filtrar_diario,        portapapeles,
             cargar_espera,        imprime_diario,         pconceptos,            edicion_automaticos,
             ejecutar_automaticos, exportar_automaticos,   fin_ejercicio,         tipos_diario,
             reg_ejecutar,         reg_edicion,            reg_importar,          balance_ss,
             informativas,         concilia_cuentas,       importar_c43,          conceptos_conciliacion,
             cons_ret,             vencimientos_asignar,   vencimientos_procesar, mdomiciliaciones,
             asig_venci_cta,       tipos_de_venci,         vencim_anulados,       iva_sop,
             iva_rep,              iva_aib,                iva_eib,               tipos_iva,
             asigna_cta_tipo_iva,  chequea_libros_iva,     libro_recibid,         libro_emitid,
             edicion_ci,           inputaciones_cuenta,    ver_inputaciones,      intercambia_ci_diario,
             reasigna_imputaciones,cambio_imputac_cuenta,  ejecutar_estados,      edicion_estados,
             ecpn_pyme,            xml_ctas,               xml_is,                procesar_memoria,
             presupuestos_edicion, previs_realiz,          asiento_amortizaciones,plan_amortizaciones,
             ed_preferencias,      importar_asientos,      import_contaplus,      importfich,
             marcar_cerrado_ej,    asignar_diario,         agrupar_tex_libro,     pconsolidacion,
             copia_segur,          utiles,                 activa_visor_local     activa_sii
             }; */


    ui->actionConexion->setEnabled(privilegios[cambiar_conexion_bd]);
     // ui->actionOrden_numero
    ui->actionPreferencias->setEnabled(privilegios[ed_preferencias]);
    // ui->actionAcerca_de
    // ui->action_Salir
    ui->actionConfiguraci_n->setEnabled(privilegios[p_configuracion]);
    ui->actionEjercicios->setEnabled(privilegios[ejercicios_contables]);
    ui->actionCrear_Empresa->setEnabled(privilegios[crea_borra_bd]);

    ui->actionBorrar_Empresa->setEnabled(privilegios[crea_borra_bd]);
    ui->actionCopiar_Empresa->setEnabled(privilegios[copiar_empresa]);
    ui->actionConsultar_Notas->setEnabled(privilegios[consulta_notas]);
    ui->actionEditar_Notas->setEnabled(privilegios[edita_notas]);
    ui->actionHabilitar_Deshabilitar->setEnabled(privilegios[gestion_periodos]);
    // ui->actionCambio_usuario
    ui->actionGest_usuarios->setEnabled(privilegios[gestusuarios]);
    // ui->actionCambiar_Clave
    ui->actionImportar_usuarios->setEnabled(privilegios[gestusuarios]);

    ui->menuEditar_tablas_AEAT_Mercantiles->setEnabled(privilegios[tablas_aeat]);

    // ui->actionCodigo_pais
    // ui->actionClave_identific_fiscal
    // ui->actionClave_operaciones_expedidas
    // ui->actionClave_operaciones_recibidas
    // ui->actionClave_retenciones_irpf
    // ui->actionConceptos_EVPN
    // ui->actionProvincias
    // ui->actionContenido_comunicacion_347
    // ui->actionColumnas_EVPN


    ui->actionCuentas->setEnabled(privilegios[plan]);
    ui->actionCuentas_auxiliares->setEnabled(privilegios[cuentas_auxiliares]);
    ui->actionGestion_Externos->setEnabled(privilegios[cuentas_auxiliares]);
    ui->actionAsignar_E_xterno->setEnabled(privilegios[cuentas_auxiliares]);
    ui->actionDatos_accesorios_cuentas->setEnabled(privilegios[datos_accesorios_cta]);
    ui->actionAsignar_IVA_retenci_n_a_cuentas->setEnabled(privilegios[asigna_iva_ret_cta]);
    ui->actionBloquear_cuentas->setEnabled(privilegios[bloquear_cuentas]);
    ui->actionExportar_plan->setEnabled(privilegios[exportar_plan]);
    ui->actionExportarplan_cuentas_auxiliares->setEnabled(privilegios[exportar_plan]);
    ui->actionImportar_plan->setEnabled(privilegios[importar_plan]);
    ui->actionNuevo_Asiento->setEnabled(privilegios[nuev_asiento]);
    ui->actionEditar_Asiento->setEnabled(privilegios[edi_asiento]);
    ui->actionEditar_Asiento_N_mero->setEnabled(privilegios[edi_asiento]);
    ui->actionPapelera->setEnabled(privilegios[edi_asiento]);
    // ui->actionConsulta_Asiento
    // ui->actionConsulta_Asiento_N_mero
    ui->actionChequear_Revisado->setEnabled(privilegios[marcar_revisado]);
    ui->actionAsigna_Fichero_a_Asiento->setEnabled(privilegios[asigna_fichero]);
    ui->actionBorrar_Asiento->setEnabled(privilegios[borrar_asiento]);
    ui->actionEditar_Filtro->setEnabled(privilegios[filtrar_diario]);
    ui->actionFiltro_inicial->setEnabled(privilegios[filtrar_diario]);

    // ui->actionVisualizar_Documento
    ui->actionCopiar->setEnabled(privilegios[portapapeles]);
    ui->actionCortar->setEnabled(privilegios[portapapeles]);
    ui->actionPegar->setEnabled(privilegios[portapapeles]);

    ui->actionAsientos_en_Espera->setEnabled(privilegios[cargar_espera]);
    // ui->actionRefrescar
    ui->actionImprimir_Diario->setEnabled(privilegios[imprime_diario]);
    ui->actionConceptos->setEnabled(privilegios[pconceptos]);

    // ui->actionAsientos_Predefinidos

    ui->actionRegularizacin->setEnabled(privilegios[fin_ejercicio]);
    ui->actionCierre_ejercicio->setEnabled(privilegios[fin_ejercicio]);
    ui->actionRegul_cierre_apertura->setEnabled(privilegios[fin_ejercicio]);
    ui->actionDeshacer_Cierre->setEnabled(privilegios[fin_ejercicio]);
    ui->actionAsiento_de_Apertura->setEnabled(privilegios[fin_ejercicio]);

    // ui->actionOrdenar_por_campo
    ui->actionTipos_de_Diario->setEnabled(privilegios[tipos_diario]);

    ui->actionEdicin_regul->setEnabled(privilegios[reg_edicion]);
    ui->actionEjecutar_regul->setEnabled(privilegios[reg_ejecutar]);
    ui->actionImportar_regul->setEnabled(privilegios[reg_importar]);

    // ui->actionMayor_a_partir_de_apunte
    // ui->actionMayor_ventana_permanente
    // ui->actionImprime_mayor

    ui->actionBalance_de_Sumas_y_Saldos->setEnabled(privilegios[balance_ss]);

    // ui->actionAcumulados_mes
    // ui->actionConsulta_Plan
    // ui->actionDiario_Resumen

    ui->actionVolumen_de_operaciones_con_terceras_personas->setEnabled(privilegios[informativas]);
    ui->actionConciliacion_de_cuentas->setEnabled(privilegios[concilia_cuentas]);
    ui->actionConceptos_de_diferencias->setEnabled(privilegios[conceptos_conciliacion]);
    ui->actionImportar_cuaderno->setEnabled(privilegios[importar_c43]);
    ui->actionImporta_C_SV->setEnabled(privilegios[importar_c43]);
    ui->actionBorrar_importado->setEnabled(privilegios[concilia_cuentas]);
    ui->actionConcilia_cuenta_importada->setEnabled(privilegios[concilia_cuentas]);
    ui->actionProcesa_vencimientos_c43->setEnabled(privilegios[concilia_cuentas]);

    ui->actionRetenciones_irpf->setEnabled(privilegios[cons_ret]);
    ui->actionModelo_182->setEnabled(privilegios[informativas]);
    ui->actionModelo_182_registro_donaciones->setEnabled(privilegios[informativas]);
    ui->actionVencimientos_a_partir_diario->setEnabled(privilegios[vencimientos_asignar]);
    ui->actionProcesar_vencimientos->setEnabled(privilegios[vencimientos_procesar]);
    ui->actionAsignar_tipos_de_vencimientos_a_cuentas->setEnabled(privilegios[asig_venci_cta]);
    ui->actionDomiciliaciones->setEnabled(privilegios[mdomiciliaciones]);
    ui->actionFicheros_de_transferencias->setEnabled(privilegios[mdomiciliaciones]);

    ui->actionEditar_tipos_de_vencimientos->setEnabled(privilegios[tipos_de_venci]);
    ui->actionInfo_venci->setEnabled(privilegios[tipos_de_venci]);
    ui->actionFormas_de_pago->setEnabled(privilegios[tipos_de_venci]);

    ui->actionVencimientos_anulados->setEnabled(privilegios[vencim_anulados]);
    ui->actionNuevo_Registro_IVA_soportado->setEnabled(privilegios[iva_sop]);
    ui->actionNuevo_Registro_IVA_repercutido->setEnabled(privilegios[iva_rep]);
    ui->actionAIB_Inversi_n_sujeto_pasivo->setEnabled(privilegios[iva_aib]);
    ui->actionEIB_Prestaci_n_servicos_UE->setEnabled(privilegios[iva_eib]);
    ui->actionTipos_de_IVA->setEnabled(privilegios[tipos_iva]);


    ui->actionCuentas_de_IVA_Soportado->setEnabled(privilegios[asigna_cta_tipo_iva]);
    ui->actionCuentas_de_IVA_Repercutido->setEnabled(privilegios[asigna_cta_tipo_iva]);
    ui->actionCuentas_inversion_SP->setEnabled(privilegios[asigna_cta_tipo_iva]);
    ui->actionChequea_Libro_Facturas->setEnabled(privilegios[chequea_libros_iva]);
    ui->actionApuntes_sin_Representacin_en_Libros_de_IVA->setEnabled(privilegios[chequea_libros_iva]);
    ui->actionComprobar_Cuadre_de_Cuotas_con_Diario->setEnabled(privilegios[chequea_libros_iva]);

    // ui->actionLiqudaci_n_de_IVA

    ui->actionRelaci_n_de_Operaciones_seg_n_Libros_de_IVA->setEnabled(privilegios[informativas]);
    ui->actionLibros_de_Facturas_Recibidas->setEnabled(privilegios[libro_recibid]);
    ui->actionLibro_de_Facturas_Emitidas->setEnabled(privilegios[libro_emitid]);
    ui->actionModelo_340->setEnabled(privilegios[informativas]);
    ui->actionModelo_347_F50_Navarra->setEnabled(privilegios[informativas]);
    ui->actionModelo_349->setEnabled(privilegios[informativas]);
    ui->actionPrimer_Nivel->setEnabled(privilegios[edicion_ci]);
    ui->actionSegundo_Nivel->setEnabled(privilegios[edicion_ci]);
    ui->actionTercer_Nivel->setEnabled(privilegios[edicion_ci]);
    // ui->actionConsulta_Imputaciones
    ui->actionCoeficientes_Imputaci_n_Cuenta->setEnabled(privilegios[inputaciones_cuenta]);
    ui->actionVer_Imputaciones->setEnabled(privilegios[ver_inputaciones]);
    ui->actionIntercambiar_CI_en_Diario->setEnabled(privilegios[intercambia_ci_diario]);
    ui->actionReasignar_Imputaciones->setEnabled(privilegios[reasigna_imputaciones]);
    ui->actionCambiar_Imputaciones_a_Cuenta_en_Diario->setEnabled(privilegios[cambio_imputac_cuenta]);
    // ui->actionGesti_n_Estados_Contables
    // ui->actionLibera_C_lculo_Saldos_Medios
    ui->actionECPN_PYMES->setEnabled(privilegios[ecpn_pyme]);
    ui->actionAsignar_Concepto_a_apuntes->setEnabled(privilegios[ecpn_pyme]);
    ui->actionXML_Cuentas_PYMES->setEnabled(privilegios[xml_ctas]);
    ui->actionXML_Impuesto_Sociedades_PYMES->setEnabled(privilegios[xml_is]);
    ui->actionProcesar_Memoria->setEnabled(privilegios[procesar_memoria]);
    ui->actionEditar_Presupuesto_Cuentas_Auxiliares->setEnabled(privilegios[presupuestos_edicion]);
    ui->actionEditar_Presupuesto_Cuentas->setEnabled(privilegios[presupuestos_edicion]);
    ui->actionPrevisiones_Realizaciones_Cuentas_Auxiliares->setEnabled(privilegios[previs_realiz]);
    ui->actionPrevisiones_Realizaciones_Cuentas->setEnabled(privilegios[previs_realiz]);

    ui->actionPlan_de_amortizaciones->setEnabled(privilegios[plan_amortizaciones]);
    ui->actionAsiento_de_amortizaciones->setEnabled(privilegios[asiento_amortizaciones]);
    // ui->actionDiferencia_en_base_imponible_I_S
    // ui->actionHist_rico_de_diferencias_en_amortizaciones
    // ui->actionInmovilizado_neto
    ui->actionImportar_Asientos->setEnabled(privilegios[importar_asientos]);
    ui->actionImportar_Contapls->setEnabled(privilegios[import_contaplus]);
    ui->actionMarcar_como_cerrado_ejercicio->setEnabled(privilegios[marcar_cerrado_ej]);
    ui->actionProcesar_fichero_importacin->setEnabled(privilegios[importfich]);
    ui->actionAsignar_Diario->setEnabled(privilegios[asignar_diario]);
    ui->actionAgrupar_ficheros_TEX_en_libro->setEnabled(privilegios[agrupar_tex_libro]);
    // ui->actionConsultar_ficheros_TEX
    // ui->actionGenerar_PDF
    ui->actionCopia_Seguridad->setEnabled(privilegios[copia_segur]);
    ui->actionConsolidaci_n_Contable->setEnabled(privilegios[pconsolidacion]);

    ui->menuUtilidades->setEnabled(privilegios[utiles]);
    ui->menuSII->setEnabled(privilegios[activa_sii]);

    // ui->actionRenumerar_Asientos
    // ui->actionIntercambiar_nmeros_de_asiento
    // ui->actionBorrar_Intervalo_de_asientos
    // ui->actionIntercambiar_cuenta_en_diario
    // ui->actionCambiar_cuenta_a_apunte
    // ui->actionEditar_CI_en_apunte
    // ui->actionEdita_fecha_a_asiento
    // ui->actionEdita_concepto_y_documento_en_pase
    // ui->actionCarga_plan_nuevo
    // ui->actionEditar_plan_nuevo
    // ui->actionEditar_equivalencias_nuevo_antiguo
    // ui->actionSustituir_subcuentas
    // ui->actionBorrar_plan_nuevo
    // ui->actionVerificar_base_de_datos
    // ui->actionRectificar_saldos

    // ui->actionInformaci_n_de_la_conexi_n
    // ui->actionAcuerdo_de_licencia
    // ui->actionAcuerdo_de_licencia_en_espa_ol


/*
     ui.actionBorrar_empresa->setEnabled(privilegios[cambiar_conexion_bd]);
     ui.actionNuevaempresa->setEnabled(privilegios[cambiar_conexion_bd]);
     ui.actionEjercicios_contables->setEnabled(privilegios[ejercicios_contables]);
     ui.actionHabilitar_periodos->setEnabled(privilegios[gestion_periodos]);
     ui.actionConfiguraci_n->setEnabled(privilegios[p_configuracion]);

     ui.actionCodigo_pais->setEnabled(privilegios[tablas_aeat]);
     ui.actionClave_identificacion_fiscal->setEnabled(privilegios[tablas_aeat]);
     ui.actionClave_operaciones_expedidas->setEnabled(privilegios[tablas_aeat]);
     ui.actionClave_operaciones_recibidas->setEnabled(privilegios[tablas_aeat]);
     ui.actionClave_retenciones_irpf->setEnabled(privilegios[tablas_aeat]);
     ui.actionProvincias->setEnabled(privilegios[tablas_aeat]);
     ui.actionConceptos_EVPN->setEnabled(privilegios[tablas_aeat]);

     ui.actionCuentas->setEnabled(privilegios[plan]);
     ui.actionSubcuentas->setEnabled(privilegios[cuentas_auxiliares]);
     ui.actionEditar_venci->setEnabled(privilegios[tipos_vencimientos]);


     ui.actionBloquear_cuentas->setEnabled(privilegios[bloquear_cuentas]);
     ui.actionExportar_plan->setEnabled(privilegios[exportar_plan]);
     ui.actionExportar_plan_con_subcuentas->setEnabled(privilegios[exportar_plan]);
     ui.actionImportar_plan->setEnabled(privilegios[importar_plan]);
     // ui.actionNuevo_asiento->setEnabled(privilegios[asientos]);
     ui.actionEditar_asiento->setEnabled(privilegios[asientos]);
     ui.actionEditar_asiento_num->setEnabled(privilegios[asientos]);

     ui.actionCopiar->setEnabled(privilegios[portapapeles]);
     ui.actionPegar->setEnabled(privilegios[portapapeles]);
     ui.actionCortar->setEnabled(privilegios[portapapeles]);
     ui.actionConceptos->setEnabled(privilegios[pconceptos]);

     // edicion_automaticos
     // ejecutar_automaticos
     // exportar_automaticos

     ui.actionRegularizacion->setEnabled(privilegios[fin_ejercicio]);
     ui.actionCierre_de_ejercicio->setEnabled(privilegios[fin_ejercicio]);
     ui.actionDeshacer_cierre->setEnabled(privilegios[fin_ejercicio]);
     ui.actionAsiento_de_apertura->setEnabled(privilegios[fin_ejercicio]);

     ui.actionTipos_diario->setEnabled(privilegios[tipos_diario]);

     ui.actionEjecutar_reg->setEnabled(privilegios[reg_ejecutar]);
     ui.actionImportar_reg->setEnabled(privilegios[reg_importar]);
     ui.actionEdicion_regul->setEnabled(privilegios[reg_edicion]);

     ui.actionConciliacion_cuentas->setEnabled(privilegios[concilia_cuentas]);
     ui.actionConcilia_importada->setEnabled(privilegios[concilia_cuentas]);
     ui.actionConceptos_diferencias->setEnabled(privilegios[conceptos_conciliacion]);
     ui.actionImportar_cuaderno->setEnabled(privilegios[importar_c43]);
     ui.actionBorrar_importado->setEnabled(privilegios[importar_c43]);

     ui.actionVencimientos_a_partir_diario->setEnabled(privilegios[vencimientos_asignar]);
     ui.actionAsignar_vencimientos_cuentas_a_cobrar->setEnabled(privilegios[vencimientos_asignar]);
     ui.actionAsignar_vencimientos_cuentas_a_pagar->setEnabled(privilegios[vencimientos_asignar]);
     ui.actionVencimientos_anulados->setEnabled(privilegios[vencimientos_asignar]);

     ui.actionProcesar_vencimientos->setEnabled(privilegios[vencimientos_procesar]);

     ui.actionNuevo_registro_de_IVA_soportado->setEnabled(privilegios[iva_sop]);
     ui.actionNuevo_registro_de_IVA_repercutido->setEnabled(privilegios[iva_rep]);
     ui.actionAdquisici_n_intracomunitaria_de_bienes->setEnabled(privilegios[iva_aib]);
     ui.actionEntrega_intracomunitaria_de_bienes->setEnabled(privilegios[iva_eib]);

     ui.actionTipos_de_IVA->setEnabled(privilegios[tipos_iva]);

     ui.actionC_digos_imputaci_n_primer_nivel->setEnabled(privilegios[edicion_ci]);
     ui.actionC_digos_imputaci_n_segundo_nivel->setEnabled(privilegios[edicion_ci]);
     ui.actionC_digos_imputaci_n_tercer_nivel->setEnabled(privilegios[edicion_ci]);

     ui.actionNuevo_estado_contable->setEnabled(privilegios[edicion_estados]);
     ui.actionEditar_estado_contable->setEnabled(privilegios[edicion_estados]);

     ui.actionProcesar_memoria->setEnabled(privilegios[procesar_memoria]);

     ui.actionEditar_presupuesto->setEnabled(privilegios[presupuestos_edicion]);
     ui.actionEditar_pres_cuentas->setEnabled(privilegios[presupuestos_edicion]);

     ui.actionPlan_de_amortizaciones->setEnabled(privilegios[plan_amortizaciones]);
     ui.actionAsiento_de_amortizaciones->setEnabled(privilegios[asiento_amortizaciones]);
     ui.actionPreferencias->setEnabled(privilegios[ed_preferencias]);

     ui.actionImportar_asientos->setEnabled(privilegios[importar_asientos]);
     ui.actionMarcar_como_cerrado_ejercicio->setEnabled(privilegios[marcar_cerrado_ej]);

     ui.actionAsignar_diario->setEnabled(privilegios[asignar_diario]);

     ui.actionRenumerar_asientos->setEnabled(privilegios[renumerar_asientos]);

     ui.actionIntercambiar_numeros->setEnabled(privilegios[intercambiar_num_asientos]);

     ui.actionAgrupar_TEX->setEnabled(privilegios[agrupar_tex_libro]);

     ui.actionBorrar_intervalo->setEnabled(privilegios[borrar_int_asientos]);

     ui.actionIntercambiar_cuenta->setEnabled(privilegios[interc_ctas_diario]);

     ui.actionCambiar_cuenta->setEnabled(privilegios[cambia_cuenta_apunte]);

     ui.actionEditar_CI->setEnabled(privilegios[editar_ci_apunte]);

     ui.actionIntercambiar_CI_en_diario->setEnabled(privilegios[intercambia_ci_diario]);

     ui.actionEdita_fecha_asiento->setEnabled(privilegios[edit_fecha_asiento]);

     ui.actionEditar_concepto_y_documento->setEnabled(privilegios[ed_conc_doc_pase]);

     ui.actionCargar_plan_nuevo->setEnabled(privilegios[conversion_plan]);

     ui.actionBorrar_plan_nuevo->setEnabled(privilegios[conversion_plan]);

     ui.actionEditar_plan_nuevo->setEnabled(privilegios[conversion_plan]);

     ui.actionEditar_equivalencias->setEnabled(privilegios[conversion_plan]);

     ui.actionMover_subcuentas_en_plan_nuevo->setEnabled(privilegios[conversion_plan]);

     ui.actionConsolidacion_contable->setEnabled(privilegios[pconsolidacion]);

     ui.actionGestion_usuarios->setEnabled(privilegios[gestusuarios]);

     ui.actionImportar_usuarios->setEnabled(privilegios[gestusuarios]);


     //verificadb,
     //consulta_asiento,     consulta_num_asiento,   edita_notas,           consulta_notas,
     //asigna_fichero,       inputaciones_cuenta,    ver_inputaciones

     ui.actionVerifica_DB->setEnabled(privilegios[verificadb]);
     ui.actionConsultar_asiento->setEnabled(privilegios[consulta_asiento]);
     ui.actionConsulta_numero->setEnabled(privilegios[consulta_num_asiento]);
     ui.actionEdita_notas->setEnabled(privilegios[edita_notas]);
     ui.actionConsulta_notas->setEnabled(privilegios[consulta_notas]);
     ui.actionAsigna_fichero->setEnabled(privilegios[asigna_fichero]);
     ui.actionImputaciones_cuenta->setEnabled(privilegios[inputaciones_cuenta]);
     ui.actionVer_inputaciones->setEnabled(privilegios[ver_inputaciones]);

     ui.actionNuevo_asiento_2->setEnabled(privilegios[nuev_asiento]);
     ui.actionBorra_asiento->setEnabled(privilegios[borrar_asiento]);

     ui.actionCopiar_empresa->setEnabled(privilegios[copiar_empresa]);

     ui.actionCargar_asiento_en_espera->setEnabled(privilegios[cargar_espera]);

     ui.actionDatos_accesorios->setEnabled(privilegios[datos_accesorios_cta]);

// #ifndef ENTERPRISE
//     ui.menuFacturaci_n->setEnabled(privilegios[facturacion]);

//     ui.menu_Registros->setEnabled(privilegios[registros_facturacion]);
// #endif

     ui.action_Retenciones_IRPF->setEnabled(privilegios[cons_ret]);

     ui.actionModelo_347->setEnabled(privilegios[prmodelo347]);
     ui.actionModelo_340->setEnabled(privilegios[prmodelo347]);

     ui.actionProcesar_fichero->setEnabled(privilegios[importfich]);
// #ifdef ENTERPRISE
     ui.actionTipos_a_cuentas->setEnabled(privilegios[asig_venci_cta]);

// #endif
     ui.actionEstado_cpn->setEnabled(privilegios[ecpn_pyme]);
     ui.actionAsignar_concepto_para_ECPN_PYMES->setEnabled(privilegios[ecpn_pyme]);
     ui.actionConceptos_EVPN->setEnabled(privilegios[ecpn_pyme]);
     ui.actionContenido_columnas_pymes->setEnabled(privilegios[ecpn_pyme]);

     ui.actionImportar_Contaplus->setEnabled(privilegios[import_contaplus]);
     ui.actionMarcar_revisado->setEnabled(privilegios[marcar_revisado]);
     ui.actionDomiciliaciones->setEnabled(privilegios[mdomiciliaciones]);
     ui.actionXML_Cuentas->setEnabled(privilegios[xml_ctas]);
     ui.actionXML_IS->setEnabled(privilegios[xml_is]);
     */
 }

void  MainWindow::resetprivilegios()
{
    for(int veces=0; veces<NUMPRIVILEGIOS ;veces++)
    {
        privilegios[veces]=true;
        // privilegios_global[veces]=true;
    }
    // QPixmap foto;
    // punterodiario->asignaimagen(foto);
    // punterodiario->resetimagen();
}



bool MainWindow::abrebasedatos(QString vpghost,QString vpgdbname,QString vpguser,
                            QString vpgpassword,QString vcontrolador,QString vpuerto)
{
        if (! basedatos::instancia()->abrirBase( vcontrolador, vpgdbname, vpguser,
            vpgpassword, vpghost, vpuerto) )
          return false;
    return true;
}


bool MainWindow::cargapreferencias(bool *bdabierta, bool *hayfichpref)
{
    QString nombrefichcompleto=fichconfig;
    QString vpghost,vpgdbname,vpguser,vpgpassword,vcontrolador,vpuerto;
    QFile fichero(nombrefichcompleto);
    if (!fichero.exists())
       {
        *bdabierta=false;
        return false;
       }

    bool guarda_geom=false;
    int x=0,y=0, ancho=0, alto=0;
     if ( fichero.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea,variable,contenido;
          *hayfichpref=true;
          while ( !stream.atEnd() ) {
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              // analizamos la linea de texto
              if (linea.contains("NO_COPIAR_DOC")) copiardoc=false;
              if (linea.contains('=')) {
                 variable=linea.section('=',0,0);
                 contenido=linea.section('=',1,1);
                 if (variable==QString("FILTRO"))
                    {
                     contenido=linea.remove(QString("FILTRO="));
                     filtropersonalizado=contenido;
                     filtroactivo=contenido;
                    }
                 if (variable.compare("PGHOST")==0) vpghost=contenido;
                 if (variable.compare("DBNAME")==0) vpgdbname=contenido;
                 if (variable.compare("PGUSER")==0) vpguser=contenido;
                 if (variable.compare("PASSWORD")==0) vpgpassword=contenido;
                 if (variable.compare("CONTROLADOR")==0) vcontrolador=contenido;
                 if (variable.compare("PORT")==0) vpuerto=contenido;

                 if (variable.compare("COL0")==0) if (contenido.toInt()>10) anchocol[0]=contenido.toInt();
                 if (variable.compare("COL1")==0) if (contenido.toInt()>10) anchocol[1]=contenido.toInt();
                 if (variable.compare("COL2")==0) if (contenido.toInt()>10) anchocol[2]=contenido.toInt();
                 if (variable.compare("COL3")==0) if (contenido.toInt()>10) anchocol[3]=contenido.toInt();
                 if (variable.compare("COL4")==0) if (contenido.toInt()>10) anchocol[4]=contenido.toInt();
                 if (variable.compare("COL5")==0) if (contenido.toInt()>10) anchocol[5]=contenido.toInt();
                 if (variable.compare("COL6")==0) if (contenido.toInt()>10) anchocol[6]=contenido.toInt();
                 if (variable.compare("COL7")==0) if (contenido.toInt()>10) anchocol[7]=contenido.toInt();
                 if (variable.compare("COL8")==0) if (contenido.toInt()>10) anchocol[8]=contenido.toInt();
                 if (variable.compare("COL9")==0) if (contenido.toInt()>10) anchocol[9]=contenido.toInt();
                 if (variable.compare("COL10")==0) if (contenido.toInt()>10) anchocol[10]=contenido.toInt();
                 if (variable.compare("COL11")==0) if (contenido.toInt()>10) anchocol[11]=contenido.toInt();
                 if (variable.compare("COL12")==0) if (contenido.toInt()>10) anchocol[12]=contenido.toInt();
                 if (variable.compare("COL13")==0) if (contenido.toInt()>10) anchocol[13]=contenido.toInt();
                 if (variable.compare("COL14")==0) if (contenido.toInt()>10) anchocol[14]=contenido.toInt();
                 if (variable.compare("COL15")==0) if (contenido.toInt()>10) anchocol[15]=contenido.toInt();
                 // ---------------------------------------------------------------------------------------
                 if (variable.compare("DIRECTORIO")==0) dirtrabajo=contenido;
                 if (variable.compare("VISORDVI")==0) visordvi=contenido;
                 if (variable.compare("IMPRIMIR")==0) aplicacionimprimir=contenido;
                 if (variable.compare("NAVEGADOR")==0) navegador=contenido;
                 if (variable.compare("FILTROPOR")==0) filtrarpor=contenido.toInt();
                 if (variable.compare("ORDENARPOR")==0)
                    {
                     ordendiario=contenido.toInt();
                     ui->actionOrden_numero->disconnect(SIGNAL(toggled(bool)));
                     if (ordendiario!=FECHA) ui->actionOrden_numero->setChecked(true);
                           else ui->actionOrden_numero->setChecked(false);
                     connect(ui->actionOrden_numero, SIGNAL(toggled(bool)), this,
                       SLOT (qordendiario(bool)));
                    }
                 if (variable.compare("SINDECIMALES")==0) sindecimales=contenido.toInt();
                 if (variable.compare("FILTRO")==0) filtropersonalizado=contenido;
                 if (variable.compare("ESTILONUMERICO")==0) estilonumerico=contenido.toInt();
                 if (variable.compare("FILTRARTEX")==0) filtrartex=contenido.toInt();
                 if (variable.compare("IDIOMA")==0) idioma=contenido.toInt();
                 if (variable.compare("DOCKAUTOMATICOS")==0) haydockautomaticos=contenido.toInt();
                 if (variable.compare("DOCKESTADOS")==0) haydockestados=contenido.toInt();
                 if (variable.compare("RUTADOCS")==0) rutadocs=contenido;
                 if (variable.compare("RUTACARGADOCS")==0) rutacargadocs=contenido;
                 if (variable.compare("RUTACOPIAPDF")==0) rutacopiapdf=contenido;
                 if (variable.compare("RUTACURL")==0) ruta_curl=contenido;
                 global_ruta_curl=ruta_curl;
                 if (variable.compare("EXT1")==0) ext1=contenido;
                 if (variable.compare("VIS1")==0) vis1=contenido;
                 if (variable.compare("EXT2")==0) ext2=contenido;
                 if (variable.compare("VIS2")==0) vis2=contenido;
                 if (variable.compare("EXT3")==0) ext3=contenido;
                 if (variable.compare("VIS3")==0) vis3=contenido;
                 if (variable.compare("EXT4")==0) ext4=contenido;
                 if (variable.compare("VIS4")==0) vis4=contenido;

                 if (variable.compare("GUARDACONEXION")==0) guardaconexion=contenido.toInt();
                 if (variable.compare("GUARDAUSUARIO")==0) guardausuario=contenido.toInt();

                 if (variable.compare("USUARIO")==0) usuario=contenido;
                 if (variable.compare("CLAVEUSUARIO")==0) claveusuario=contenido;
                 if (variable.compare("EDITORLATEX")==0) editorlatex=contenido;
                 if (variable.compare("RUTASEG")==0) ruta_seg=contenido;
                 if (variable.compare("RUTALIBROS")==0) ruta_libros=contenido;
                 if (variable.compare("URL_ACTU")==0) {url_actu=contenido; global_url_actu=contenido;}
                 if (variable.compare("MSJ_URL_ACTU")==0) { msj_url_actu=contenido; global_msj_url_actu=contenido;}

                 if (variable.compare("ICONO")==0) arch_icono=contenido;
                 if (variable.compare("TITULO")==0) titulo=contenido;
                 if (variable.compare("SPLASH")==0) arch_splash=contenido;
                 if (variable.compare("CPLUS")==0) cplus=contenido;
                 if (variable.compare("CDOMICILIACIONES")==0) cdomiciliaciones=contenido;
                 if (variable.compare("SCRIPT_COPIAS")==0) script_copias=contenido;
                 if (variable.compare("FORZAR_VER_PDF")==0) forzar_ver_pdf=contenido;

                 if (variable.compare("CONTROL_DOCS_REPETIDOS")==0)
                     control_docs_repetidos=contenido.trimmed()=="1";

                 if (variable.compare("VISUALIZADOR_EXTENDIDO")==0) {
                     visualizador_extendido=contenido.trimmed()=="1";
                     hay_visualizador_extendido=contenido.trimmed()=="1";
                 }

                 if (variable.compare("IMPORT_REMOTA")==0) {
                     import_remota=contenido.trimmed()=="1";

                 }

                 if (variable.compare("RUTA")==0)
                    {
                      QString linea=contenido;
                      conexion_rutas << linea.section(';',0,0);
                      rutas_trabajo << linea.section(';',1,1);
                      rutas_docs << linea.section(';',2,2);
                    }

                 if (!arch_icono.isEmpty())
                    {
                     if (QFile::exists(arch_icono))
                        {
                         QIcon icono(arch_icono);
                         setWindowIcon(icono);
                         ui->actionAcerca_de->setIcon(icono);
                        }
                     if (!titulo.isEmpty())
                       {
                        setWindowTitle(titulo);
                       }
                    }

                 if (variable.compare("POSX")==0)
                    {
                     guarda_geom=true;
                     x=contenido.toInt();
                    }
                 if (variable.compare("POSY")==0) y=contenido.toInt();
                 if (variable.compare("ANCHO")==0) ancho=contenido.toInt();
                 if (variable.compare("ALTO")==0) alto=contenido.toInt();

              }
          }
          #ifdef SMART
          guarda_geom=false;
          #endif
          if (guarda_geom)
               {
                 resize(ancho,alto);
                 move(x,y);
               }
          fichero.close();
        }

     if (!guardaconexion)
       {
         *bdabierta=false;
         return true;
       }

     if (vpgdbname=="template1")
        {
         *bdabierta=false;
         return true;
        }

     if (vcontrolador.contains("SQLITE") && !QFile::exists(vpgdbname))
        {
         *bdabierta=false;
         return true;
        }
     if (abrebasedatos(vpghost,vpgdbname,vpguser,
                        vpgpassword,vcontrolador,vpuerto))
         *bdabierta=true;
        else
               *bdabierta=false;

   return true;

}


void MainWindow::qordendiario(bool numero)
{
     if (numero) ordendiario=ASIENTO;
        else ordendiario=FECHA;
     QString cadena;
     // filtroactivo=condicionesfiltrodefecto();
     punterodiario->pasafiltroedlin(filtroactivo);
     if (filtroactivo.length()>0) cadena=" WHERE "+filtroactivo+" ";
     //  falta pasar cadena a lineEdit del filtro
     cadena+=ordenarpor();
     punterodiario->pasafiltro(cadena,estilonumerico,sindecimales);
     // punterodiario->pasaanchos(anchocol);
     if (basedatos::instancia()->essqlite()) punterodiario->refresca(); // nuevo para SQLITE
     punterodiario->cabecera_campo_orden(n_campo_orden());

     punterodiario->muestratabla();
}


int MainWindow::n_campo_orden()
{
    int campo_orden;
    if (orden_campo_diario.isEmpty())
       {
        if (ordendiario==ASIENTO)  {campo_orden=2;}
          else {campo_orden=1;}
       }
      else
          {
           if (orden_campo_diario=="cuenta") campo_orden=0;
           if (orden_campo_diario=="concepto") campo_orden=3;
           if (orden_campo_diario=="debe") campo_orden=4;
           if (orden_campo_diario=="haber") campo_orden=5;
           if (orden_campo_diario=="documento") campo_orden=6;
           if (orden_campo_diario=="diario") campo_orden=7;
           if (orden_campo_diario=="usuario") campo_orden=8;
           if (orden_campo_diario=="pase") campo_orden=9;
           if (orden_campo_diario=="ci") campo_orden=10;
           if (orden_campo_diario=="clave_ci") campo_orden=11;
           if (orden_campo_diario=="copia_doc") campo_orden=12;
           if (orden_campo_diario=="ejercicio") campo_orden=13;
           if (orden_campo_diario=="nrecepcion") campo_orden=14;
           if (orden_campo_diario=="revisado") campo_orden=15;
          }

    return campo_orden;

}

QString MainWindow::ordenarpor(void)
{
  // cuenta, fecha, asiento, concepto, debe,
  // haber, documento, diario, usuario, pase,ci,
  // clave_ci, copia_doc, ejercicio

  QString cadena=" ORDER BY ";

  if (orden_campo_diario.isEmpty())
     {
      if (ordendiario==ASIENTO)  {cadena+="ejercicio,asiento desc,pase";}
        else {cadena+="fecha desc,pase";}
     }
    else
        {
         cadena+=orden_campo_diario;
         if (orden_campo_diario!="pase") cadena+=",pase";
        }

  return cadena;
}


void MainWindow::qpreferencias()
  {
      int resultado;
      preferencias *prefer;
      preferencias *laprefer = new preferencias;
      prefer=laprefer;
      // pasamos variables al diálogo
      prefer->pasadirtrabajo(dirtrabajo);
      prefer->pasavisordvi(visordvi);
      prefer->pasaaplicimprimir(aplicacionimprimir);
      prefer->pasanavegador(navegador);
      prefer->pasafiltropor(filtrarpor);
      prefer->pasaordenarpor(ordendiario);
      prefer->pasaestilonumerico(estilonumerico);
      prefer->pasasindecimales(sindecimales);
      // prefer->pasanumeracion(numeracion);
      prefer->pasafiltrodiariopersonalizado(filtropersonalizado);
      prefer->pasafiltrartex(filtrartex);
      prefer->pasaidioma(idioma);
      prefer->pasahaydockautomaticos(haydockautomaticos);
      prefer->pasahaydockestados(haydockestados);
      bool guardadockautomaticos=haydockautomaticos;
      bool guardadockestados=haydockestados;
      prefer->pasarutadocs(rutadocs);
      prefer->pasarutacopiapdf(rutacopiapdf);
      prefer->pasa_ruta_curl(ruta_curl);
      prefer->pasaext1(ext1);
      prefer->pasavis1(vis1);
      prefer->pasaext2(ext2);
      prefer->pasavis2(vis2);
      prefer->pasaext3(ext3);
      prefer->pasavis3(vis3);
      prefer->pasaext4(ext4);
      prefer->pasavis4(vis4);
      prefer->pasarutacargadocs(rutacargadocs);
      prefer->pasaguardausuario(guardausuario);
      prefer->pasaguardaconexion(guardaconexion);
      prefer->pasaeditorlatex(editorlatex);
      prefer->pasa_ruta_seg(ruta_seg);
      prefer->pasarutalibros(ruta_libros);
      prefer->pasa_visualizador_extendido(visualizador_extendido);
      prefer->pasa_repetidos(control_docs_repetidos);
      prefer->pasa_import_remota(import_remota);
      prefer->pasa_listas_info(conexion_rutas, rutas_trabajo, rutas_docs);
      prefer->pasa_url_actu(url_actu);
      prefer->pasa_msj_url_actu(msj_url_actu);
      resultado=prefer->exec();
      if (resultado==QDialog::Accepted)
         {
         // pasar contenido a variables
             dirtrabajo=prefer->dirtrabajo();
             visordvi=prefer->visordvi();
             aplicacionimprimir=prefer->aplicimprimir();
             navegador=prefer->navegador();
             filtrarpor=prefer->filtropor();
             ordendiario=prefer->ordenarpor();
             ui->actionOrden_numero->disconnect(SIGNAL(toggled(bool)));
             if (ordendiario!=FECHA) ui->actionOrden_numero->setChecked(true);
                else ui->actionOrden_numero->setChecked(false);
             connect(ui->actionOrden_numero, SIGNAL(toggled(bool)), this,
                   SLOT (qordendiario(bool)));
             estilonumerico=prefer->estilonumerico();
             sindecimales=prefer->sindecimales();
             // numeracion=prefer->numeracion();
             filtropersonalizado=prefer->filtrodiariopersonalizado();
             filtrartex=prefer->esfiltrartex();
             idioma=prefer->qidioma();
             haydockautomaticos=prefer->sihaydockautomaticos();
             haydockestados=prefer->sihaydockestados();
             if (haydockautomaticos!=guardadockautomaticos || haydockestados!=guardadockestados)
                {
                 // actualizamos de nuevo los dockwidgets
                 delete(treeWidget);
                 // modeloarbol.borraarbol();
                 delete(estadosList);
                 delete(dockAutomaticos);
                 delete(dockEstados);
                 creaDockWindows();
                }
             rutadocs=prefer->rutadocs();
             rutacargadocs=prefer->rutacargadocs();
             rutacopiapdf=prefer->rutacopiapdf();
             ruta_seg=prefer->ruta_seg();
             ruta_libros=prefer->rutalibros();
             ext1=prefer->ext1();
             vis1=prefer->vis1();
             ext2=prefer->ext2();
             vis2=prefer->vis2();
             ext3=prefer->ext3();
             vis3=prefer->vis3();
             ext4=prefer->ext4();
             vis4=prefer->vis4();
             guardaconexion=prefer->seguardaempresa();
             guardausuario=prefer->seguardausuario();
             editorlatex=prefer->editorlatex();
             visualizador_extendido=prefer->visualizador_extendido();
             control_docs_repetidos=prefer->hay_control_docs_repetidos();
             hay_visualizador_extendido=prefer->visualizador_extendido();
             QStringList qconex_rutas, qrutas_trabajo, qrutas_docs;
             prefer->recupera_listas_info(&qconex_rutas, &qrutas_trabajo, &qrutas_docs);
             conexion_rutas=qconex_rutas;
             rutas_trabajo=qrutas_trabajo;
             rutas_docs=qrutas_docs;
             ruta_curl=prefer->ruta_curl();
             global_ruta_curl=ruta_curl;
             import_remota=prefer->import_remota();
             url_actu=prefer->url_actu();
             msj_url_actu=prefer->msj_url_actu();
             global_url_actu=url_actu;
             global_msj_url_actu=msj_url_actu;
         }
      delete(prefer);
      grabapreferencias();

     QString cadena;
     filtroactivo=condicionesfiltrodefecto();
     punterodiario->pasafiltroedlin(filtroactivo);
     if (filtroactivo.length()>0) cadena=" WHERE "+filtroactivo+" ";
     //  falta pasar cadena a lineEdit del filtro
     cadena+=ordenarpor();
     punterodiario->pasafiltro(cadena,estilonumerico,sindecimales);
     // punterodiario->pasaanchos(anchocol);
     punterodiario->muestratabla();

  }

void MainWindow::grabapreferencias()
{
  QString qfichero=fichconfig;
  if (! basedatos::instancia()->abierta() ) return;
  QString anombredb = basedatos::instancia()->nombre();
  QString anombreusuario = basedatos::instancia()->usuario();
  QString aclave = basedatos::instancia()->clave();
  QString ahost = basedatos::instancia()->host();
  QString cadvalor;
  QString acontrolador;
  acontrolador = basedatos::instancia()->controlador();
  QString apuerto;
  apuerto.setNum(basedatos::instancia()->puerto());

  QFile fichero(qfichero);
  if (! fichero.open( QIODevice::WriteOnly ) ) return;
  QTextStream stream( &fichero );
  stream.setEncoding(QStringConverter::Utf8);

  stream << "DBNAME=" << filtracadxml(anombredb) << "\n";
  stream << "PGUSER=" << filtracadxml(anombreusuario) << "\n";
  stream << "PASSWORD=" << filtracadxml(aclave) << "\n";
  stream << "PGHOST=" << filtracadxml(ahost) << "\n";
  stream << "CONTROLADOR=" << filtracadxml(acontrolador) << "\n";
  stream << "PORT=" << filtracadxml(apuerto) << "\n";
  if (punterodiario!=NULL)
     {
        stream << "COL0=";
    cadvalor.setNum(punterodiario->anchocolumna(0));
    stream << cadvalor << "\n";
    stream << "COL1=";
    cadvalor.setNum(punterodiario->anchocolumna(1));
    stream << cadvalor << "\n";
    stream << "COL2=";
    cadvalor.setNum(punterodiario->anchocolumna(2));
    stream << cadvalor << "\n";
    stream << "COL3=";
    cadvalor.setNum(punterodiario->anchocolumna(3));
    stream << cadvalor << "\n";
    stream << "COL4=";
    cadvalor.setNum(punterodiario->anchocolumna(4));
    stream << cadvalor << "\n";
    stream << "COL5=";
    cadvalor.setNum(punterodiario->anchocolumna(5));
    stream << cadvalor << "\n";
    stream << "COL6=";
    cadvalor.setNum(punterodiario->anchocolumna(6));
    stream << cadvalor << "\n";
    stream << "COL7=";
    cadvalor.setNum(punterodiario->anchocolumna(7));
    stream << cadvalor << "\n";
    stream << "COL8=";
    cadvalor.setNum(punterodiario->anchocolumna(8));
    stream << cadvalor << "\n";
    stream << "COL9=";
    cadvalor.setNum(punterodiario->anchocolumna(9));
    stream << cadvalor << "\n";
    stream << "COL10=";
    cadvalor.setNum(punterodiario->anchocolumna(10));
        stream << cadvalor << "\n";
        stream << "COL11=";
        cadvalor.setNum(punterodiario->anchocolumna(11));
        stream << cadvalor << "\n";
        stream << "COL12=";
        cadvalor.setNum(punterodiario->anchocolumna(12));
        stream << cadvalor << "\n";
        stream << "COL13=";
        cadvalor.setNum(punterodiario->anchocolumna(13));
        stream << cadvalor << "\n";
        stream << "COL14=";
        cadvalor.setNum(punterodiario->anchocolumna(14));
        stream << cadvalor << "\n";
        stream << "COL15=";
        cadvalor.setNum(punterodiario->anchocolumna(15));
        stream << cadvalor << "\n";
        stream << "DIRECTORIO=" << filtracadxml(dirtrabajo) << "\n";
        stream << "VISORDVI="  << filtracadxml(visordvi) << "\n";
        stream << "IMPRIMIR="  << filtracadxml(aplicacionimprimir) << "\n";
        stream << "NAVEGADOR=" << filtracadxml(navegador) << "\n";
        if (filtrarpor>0)
          {
           stream << "FILTROPOR=";
           cadvalor.setNum(filtrarpor);
           stream << cadvalor << "\n";
          }
        stream << "ORDENARPOR=";
        cadvalor.setNum(ordendiario);
        stream << cadvalor << "\n";
        stream << "ESTILONUMERICO=";
        cadvalor.setNum(estilonumerico);
        stream << cadvalor << "\n";
        stream << "SINDECIMALES=";
        cadvalor.setNum(sindecimales);
        stream << cadvalor << "\n";
        stream << "FILTRARTEX=";
        cadvalor.setNum(filtrartex);
        stream << cadvalor << "\n";
        stream << "IDIOMA=";
        cadvalor.setNum(idioma);
        stream << cadvalor << "\n";
        stream << "DOCKAUTOMATICOS=";
        cadvalor.setNum(haydockautomaticos);
        stream << cadvalor << "\n";
        stream << "DOCKESTADOS=";
        cadvalor.setNum(haydockestados);
        stream << cadvalor << "\n";

        stream << "RUTADOCS=";
        stream << rutadocs << "\n";
        stream << "RUTACARGADOCS=";
        stream << rutacargadocs << "\n";

        stream << "RUTACOPIAPDF=";
        stream << rutacopiapdf << "\n";

        stream << "RUTACURL=";
        stream << ruta_curl << "\n";

        stream << "EXT1=";
        stream << ext1 << "\n";
        stream << "VIS1=";
        stream << vis1 << "\n";
        stream << "EXT2=";
        stream << ext2 << "\n";
        stream << "VIS2=";
        stream << vis2 << "\n";
        stream << "EXT3=";
        stream << ext3 << "\n";
        stream << "VIS3=";
        stream << vis3 << "\n";
        stream << "EXT4=";
        stream << ext4 << "\n";
        stream << "VIS4=";
        stream << vis4 << "\n";

        stream << "EDITORLATEX=";
        stream << editorlatex << "\n";

        stream << "RUTASEG=";
        stream << ruta_seg << "\n";

        stream << "RUTALIBROS=";
        stream << ruta_libros << "\n";

        stream << "URL_ACTU=";
        stream << url_actu << "\n";

        stream << "MSJ_URL_ACTU=";
        stream << msj_url_actu << "\n";


        stream << "GUARDACONEXION=";
        cadvalor.setNum(guardaconexion);
        stream << cadvalor << "\n";

        stream << "GUARDAUSUARIO=";
        cadvalor.setNum(guardausuario);
        stream << cadvalor << "\n";

        stream << "ICONO=";
        stream << arch_icono << "\n";
        stream << "TITULO=";
        stream << titulo << "\n";
        stream << "SPLASH=";
        stream << arch_splash << "\n";
        stream << "CPLUS=";
        stream << cplus << "\n";
        stream << "CDOMICILIACIONES=";
        stream << cdomiciliaciones << "\n";
        stream << "SCRIPT_COPIAS=";
        stream << script_copias << "\n";
        stream << "FORZAR_VER_PDF=";
        stream << forzar_ver_pdf << "\n";

        stream << "IMPORT_REMOTA=";
        if (import_remota)
           stream <<  "1" << "\n";
         else stream << "0" << "\n";

        stream << "VISUALIZADOR_EXTENDIDO=";
        cadvalor.setNum(visualizador_extendido);
        stream << cadvalor << "\n";
        stream << "CONTROL_DOCS_REPETIDOS=";
        if (control_docs_repetidos) stream << "1";
           else stream <<"0";
        stream << "\n";

        if (!copiardoc)
          {
           stream << "NO_COPIAR_DOC\n";
          }
        QSize windowSize;
        windowSize = size(); // tamaño de nuestra ventana
        int width = windowSize.width();
        int height = windowSize.height();
        QString ancho,alto;
        ancho.setNum(width);
        alto.setNum(height);

        stream << "ANCHO=" << ancho << "\n";
        stream << "ALTO=" << alto << "\n";

        QPoint punto;
        punto=pos();
        int posx=punto.x();
        int posy=punto.y();
        QString px,py;
        px.setNum(posx);
        py.setNum(posy);

        stream << "POSX=" << px << "\n";
        stream << "POSY=" << py << "\n";

        if (guardausuario)
          {
           stream << "USUARIO=";
           stream << usuario << "\n";
           stream << "CLAVEUSUARIO=";
           stream << claveusuario << "\n";
          }

        if (filtropersonalizado.length()>0) stream << "FILTRO=" << filtropersonalizado << "\n";

        for (int veces=0;veces<conexion_rutas.count();veces++)
           {
             stream << "RUTA=" << conexion_rutas.at(veces);
             stream << ";" << rutas_trabajo.at(veces);
             stream << ";" << rutas_docs.at(veces);
             stream << "\n";
           }
    }

  fichero.close();
}


void MainWindow::met_acercade()
{
  acercade *a = new acercade(titulo);
  a->exec();
  delete(a);
}


void MainWindow::salir()
  {
    close();
  }

void MainWindow::configuracion()
{
  editconfig *e = new editconfig();
  e->exec();
  delete(e);
}

void MainWindow::ed_ejercicios()
{
  ejercicios *e = new ejercicios();
  e->exec();
  delete(e);
}


void MainWindow::nuevaempresa()
{
  // primero nos quedamos con los parámetros de la anterior conexión

   QString anombredb;
   QString anombreusuario;
   QString aclave;
   QString ahost;
   QString acontrolador;
   QString apuerto;

  delete(punterodiario);
  delete(treeWidget);
  // modeloarbol.borraarbol();
  delete(estadosList);
  delete(dockAutomaticos);
  delete(dockEstados);

  {
   anombredb = basedatos::instancia()->nombre();
   anombreusuario = basedatos::instancia()->usuario();
   aclave = basedatos::instancia()->clave();
   ahost = basedatos::instancia()->host();
   acontrolador = basedatos::instancia()->controlador();
   apuerto.setNum( basedatos::instancia()->puerto() );
   basedatos::instancia()->cerrar(); //linea nueva
   // bd.removeDatabase(anombredb);
  }

  QSqlDatabase::removeDatabase("qt_sql_default_connection");
  creaempresa *c = new creaempresa;

  int resultado, resultadocreabd=0;
  // hasta que no se le dé al botón cancelar o se acepte la configuración perdirá los datos y los comprobará
  do {
    resultado = c-> exec();
    if (resultado == QDialog::Accepted) {
      // crea tablas y datos para la nueva empresa
      resultadocreabd = c->creabd();
    }
    // si los datos introducidos son correctos, guarda los datos en el archivo de configuración
    if (resultadocreabd)
    {
      c->actufichconex(fichconexiones);
      editconfig *ed = new editconfig;
      ed->exec();
      delete ed;
    }
  } while ( (resultado == QDialog::Accepted) && (resultadocreabd==0) );

  // si se cancela el comando volver a abrir la base de datos anterior
  if (resultado == QDialog::Rejected) {
    if (anombredb.isEmpty()) return;
    abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
  }
  delete c;



  setWindowTitle(tr(APLICACION)+nombreempresa());
  setUnifiedTitleAndToolBarOnMac(true);
  // calculaprimerosasientos();

  diario *eldiario = new diario;
  punterodiario=eldiario;
  setCentralWidget(eldiario);
  QString cadena;
  filtroactivo=condicionesfiltrodefecto();
  eldiario->pasafiltroedlin(filtro_a_lingu(filtroactivo));
  if (filtroactivo.length()>0) cadena=" WHERE "+filtroactivo+" ";
  cadena+=ordenarpor();
  testigic();
  eldiario->activaconfiltro(cadena,estilonumerico,sindecimales, usuario);
  eldiario->pasaanchos(anchocol);
  eldiario->muestratabla();
  creaDockWindows();
  on_diario_tab_cambiado(0);
  connect(eldiario, SIGNAL(cambio_tab(int )),this, SLOT(on_diario_tab_cambiado(int )));
  connect(eldiario, SIGNAL(filedroped()),this, SLOT(fichero_soltado_en_diario()));
  activaTodo();
}


void MainWindow::testigic()
{
     if (conigic())
        {
         /*
         ui.menuIva->setTitle(tr("&IGIC"));
         ui.actionNuevo_registro_de_IVA_soportado->setText(tr("Nuevo registro de IGIC &soportado"));
         ui.actionNuevo_registro_de_IVA_repercutido->setText(tr("Nuevo registro de IGIC &repercutido"));
         ui.actionTipos_de_IVA->setText(tr("&Tipos de IGIC"));
         ui.actionLiquidaci_n_de_IVA->setText(tr("Li&quidación de IGIC"));
         ui.actionRelopiva->setText(tr("Rela&ción de operaciones según libros de IGIC"));
         ui.actionLibro_IVA_soportado_por_AIB->setEnabled(false);
         ui.actionLibro_IVA_repercutido_por_AIB->setEnabled(false);
         ui.actionAdquisici_n_intracomunitaria_de_bienes->setEnabled(false);
         ui.actionEntrega_intracomunitaria_de_bienes->setEnabled(false); */
        }
        else
              {
               /*
               ui.menuIva->setTitle("&IVA");
               ui.actionNuevo_registro_de_IVA_soportado->setText(tr("Nuevo registro de IVA &soportado"));
               ui.actionNuevo_registro_de_IVA_repercutido->setText(tr("Nuevo registro de IVA &repercutido"));
               ui.actionTipos_de_IVA->setText(tr("&Tipos de IVA"));
               ui.actionLiquidaci_n_de_IVA->setText(tr("Li&quidación de IVA"));
               ui.actionRelopiva->setText(tr("Rela&ción de operaciones según libros de IVA"));
               ui.actionAdquisici_n_intracomunitaria_de_bienes->setEnabled(true);
               ui.actionLibro_IVA_soportado_por_AIB->setEnabled(true);
               ui.actionLibro_IVA_repercutido_por_AIB->setEnabled(true);
               ui.actionEntrega_intracomunitaria_de_bienes->setEnabled(true); */
              }

}


void MainWindow::eliminaempresaactual()
{
    borraactual *b=new borraactual();
    int resultado=b->exec();
    if (resultado == QDialog::Accepted && b->afirmativo())
       {
        QString anombredb;
        QString anombreusuario;
        QString aclave;
        QString ahost;
        QString acontrolador;
        int apuerto;
        delete(punterodiario);
        delete(treeWidget);
        delete(estadosList);
        delete(dockAutomaticos);
        delete(dockEstados);
        punterodiario=NULL;
        treeWidget=NULL;
        estadosList=NULL;
        dockAutomaticos=NULL;
        dockEstados=NULL;
        setWindowTitle(tr(APLICACION));
        {
         anombredb = basedatos::instancia()->nombre();
         anombreusuario = basedatos::instancia()->usuario();
         aclave = basedatos::instancia()->clave();
         ahost = basedatos::instancia()->host();
         acontrolador = basedatos::instancia()->controlador();
         apuerto = basedatos::instancia()->puerto();
         basedatos::instancia()->cerrar();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
        // comprobamos si es sqlite
        if (acontrolador.contains("SQLITE"))
           {
            QFile::remove(anombredb);
           }
         else
           {
            // accedemos a template1 o mysql
            // borramos la base de datos
            // llamamos a oknosalir; si salida, cerrar y return
            if (basedatos::instancia()->codigoControlador(acontrolador) == basedatos::POSTGRES) {
                if ( !basedatos::instancia()->abrirBase_s(acontrolador, "template1",
                    anombreusuario, aclave, ahost, QString::number(apuerto)) ) {
                    QMessageBox::warning( this, tr("Apertura de base de datos"),
                        tr("IMPOSIBLE ABRIR BASE DE DATOS ")+ basedatos::instancia()->nombre());
                    close();
                    return;
                }
            }
            else if (basedatos::instancia()->codigoControlador(acontrolador) == basedatos::MYSQL) {
                if ( !basedatos::instancia()->abrirBase_s(acontrolador, "mysql",
                    anombreusuario, aclave, ahost, QString::number(apuerto)) ) {
                  QMessageBox::warning( this, tr("Apertura de base de datos"),
                        tr("IMPOSIBLE ABRIR BASE DE DATOS ")+ basedatos::instancia()->nombre());
                  close();
                  return;
                }
            }

            // La base de datos se ha abierto, ya podemos executar comandos
            basedatos::instancia()->borrarDatabase(anombredb);

             {
              basedatos::instancia()->cerrar();
             }
            QSqlDatabase::removeDatabase("qt_sql_default_connection");

          }

     setWindowTitle(tr(APLICACION));
    delete(b);

     activa_sinconexion();
  }
}


void MainWindow::copiaempresa()
{
  // la conexión actual sigue igual


  // QSqlDatabase::removeDatabase("qt_sql_default_connection");

  creaempresa *c = new creaempresa;
  c->setWindowTitle(tr("COPIAR EMPRESA A CONEXIÓN NUEVA"));
  c->preparacopia();
  int resultado, resultadocreabd=0;
  // hasta que no se le dé al botón cancelar o se acepte la configuración perdirá los datos y los comprobará
  do {
    resultado = c-> exec();
    if (resultado == QDialog::Accepted) {
      // crea tablas y datos para la nueva empresa
      resultadocreabd = c->creabd_segunda();
    }
    if (resultadocreabd)
    {
      QSqlDatabase::removeDatabase("SEGUNDA");
    }
  } while ( (resultado == QDialog::Accepted) && (resultadocreabd==0) );

  delete c;

}


void MainWindow::editanotas()
{
    notas *n = new notas();
    n->exec();
    delete(n);
}

void MainWindow::consultanotas()
{
    notas *n = new notas();
    n->activa_modoconsulta();
    n->exec();
    delete(n);
}


void MainWindow::deshab_periodos()
{
    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("BLOQUEAR PERIODOS DE EJERCICIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();
    delete (ej);

    bloqueaper *p= new bloqueaper(elejercicio);
    p->exec();
    delete(p);
}


void  MainWindow::cambiousuario()
{
     cargausuario();
     if (basedatos::instancia()->gestiondeusuarios())
       setWindowTitle(tr(APLICACION)+nombreempresa()+" - Usuario: "+usuario);
      else
       setWindowTitle(tr(APLICACION)+nombreempresa());
      imgdiario();
}

void MainWindow::gestionusuarios()
{
 usuarios *u = new usuarios();
 u->exec();
 delete(u);
}


void MainWindow::cambiaclave()
{
  clave_usuario *c = new clave_usuario(usuario);
  c->exec();
  delete(c);
}


void MainWindow::iusuarios()
{
 import_usuarios *i = new import_usuarios();
 i->exec();
 delete(i);
}


void MainWindow::tabla_codigo_pais()
{
 edtabla *e = new edtabla("codigopais");
 e->exec();
 delete(e);
}

void MainWindow::tabla_id_fiscal()
{
 edtabla *e = new edtabla("claveidfiscal");
 e->exec();
 delete(e);
}
void MainWindow::tabla_op_expedidas()
{
 edtabla *e = new edtabla("clave_op_expedidas");
 e->exec();
 delete(e);
}

void MainWindow::tabla_op_recibidas()
{
 edtabla *e = new edtabla("clave_op_recibidas");
 e->exec();
 delete(e);
}

void MainWindow::tabla_clave_retenciones()
{
 edtabla *e = new edtabla("clave_op_retenciones");
 e->exec();
 delete(e);
}

void MainWindow::tabla_provincias()
{
 edtabla *e = new edtabla("provincias");
 e->exec();
 delete(e);
}

void MainWindow::tabla_concepto_evpn_pymes()
{
 edtabla *e = new edtabla("concepto_evpn_pymes");
 e->exec();
 delete(e);
}


void MainWindow::tabla_tipo_proveedor()
{
 edtabla *e = new edtabla("tipo_proveedor");
 e->exec();
 delete(e);
}

void MainWindow::tabla_homologacion()
{
 edtabla *e = new edtabla("homologacion");
 e->exec();
 delete(e);
}

void MainWindow::tabla_actividades()
{
 edtabla *e = new edtabla("actividades");
 e->exec();
 delete(e);
}

void MainWindow::contenido347()
{
  carta347 *c = new carta347();
  c->exec();
  delete(c);
}




void MainWindow::edplan()
{
 editaplan *e = new editaplan();
 e->exec();
 delete(e);
}


void MainWindow::edsubcuentas()
{
 // subcuentas *s = new subcuentas();
    if (s==NULL)
     {
        s = new subcuentas();
        s->setModal(false);
     }
 s->show();
 // delete(s);
}


void MainWindow::datos_acc()
{
    QString elasiento;
    qlonglong numasiento=punterodiario->asientoactual();
    if (numasiento!=0) elasiento.setNum(numasiento);
    QString cuenta;
    if (elasiento=="")
       {
        buscasubcuenta *s = new buscasubcuenta("");
        int resultado=s->exec();
        if (resultado==QDialog::Accepted)
              cuenta=s->seleccioncuenta();
        delete(s);
        if (cuenta.length()==0) return;
       } else cuenta=punterodiario->subcuentaactual();
    datos_accesorios *d = new datos_accesorios();
    d->cargacodigo( cuenta );
    d->exec();
    delete(d);
}


void MainWindow::ctas_iva_ret()
{
  iva_ret_ctas *i = new iva_ret_ctas();
  i->exec();
  delete(i);
}


void MainWindow::bloqueactas()
{
    bloqueacta *b = new bloqueacta();
    b->exec();
    delete(b);
}




void MainWindow::exportplan()
{
  QString nombre;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajo);
  dir->activa_pide_archivo(tr("plan.pln"));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      nombre=dir->nuevo_conruta();
      if (nombre.length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           if (nombre.right(4)!=".pln" && nombre.right(4)!=".xml")
               nombre=nombre.append(".pln");

          }
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
  QString filtrotexto=tr("Archivos de texto con plan (*.pln)");
  filtros << filtrotexto;
  filtros << tr("Archivos xml con plan (*.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(dirtrabajo);
  dialogofich.setWindowTitle(tr("ARCHIVO PARA EXPORTAR PLAN DE CONTABILIDAD"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      // QMessageBox::information( this, tr("EXPORTAR PLAN"),dialogofich.selectedNameFilter () );
      if (fileNames.at(0).length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           nombre=fileNames.at(0);
           if (dialogofich.selectedNameFilter()==filtrotexto)
              {
               if (nombre.right(4)!=".pln" && nombre.right(4)!=".xml") nombre=nombre+".pln";
              }
              else if (nombre.right(4)!=".xml" && nombre.right(4)!=".pln") nombre=nombre+".xml";
          }
      }

#endif

  QFile plan( nombre  );
  if (plan.exists() && QMessageBox::question(this,
      tr("¿ Sobreescribir ? -- Exportar plan"),
      tr("'%1' ya existe."
         "¿ Desea sobreescribirlo ?")
        .arg( nombre )) == QMessageBox::No )
    return ;
  // QMessageBox::information( this, tr("EXPORTAR PLAN"),adapta(nombre));
  if (nombre.right(4)==".pln")
     {
       if (exportaplan(nombre,false))
          {
           QMessageBox::information( this, tr("EXPORTAR PLAN"),
                   tr("El archivo se ha generado correctamente"));
          }
     }
     else
         {
          if (exportaplanxml(nombre,false))
             {
              QMessageBox::information( this, tr("EXPORTAR PLAN"),
                   tr("El archivo se ha generado correctamente"));
             }
         }

}



bool MainWindow::exportaplan(QString fichero,bool subcuentas)
{

  QFile plan( fichero  );

  if ( !plan.open( QIODevice::WriteOnly ) )
       {
        QMessageBox::warning( this, tr("Exportación de plan contable"),tr("Error: Imposible grabar fichero"));
        return false;
       }
  QTextStream planstream( &plan );
  planstream.setEncoding(QStringConverter::Utf8);

  QSqlQuery query = basedatos::instancia()->selectCodigodescripcionauxiliarplancontablesubcuentasordercodigo(subcuentas);
   if ( query.isActive() )
       while (query.next())
          {
              planstream << query.value(0).toString() << "\t";
              planstream << query.value(1).toString();
              if (cod_longitud_variable())
                 planstream << "\t" << (query.value(2).toBool() ? "true" : "false");
              planstream << "\n";
          }
  // planstream << "\\.";
  plan.close();

  return true;
}

bool MainWindow::exportaplanxml(QString fichero,bool subcuentas)
{
 QDomDocument doc("AccountingPlan");
 QDomElement root = doc.createElement("AccountingPlan");
 doc.appendChild(root);

  QSqlQuery query = basedatos::instancia()->selectCodigodescripcionauxiliarplancontablesubcuentasordercodigo(subcuentas);
   if ( query.isActive() )
       while (query.next())
          {
              QDomElement tag = doc.createElement("Account");
              root.appendChild(tag);
              // codigo,descripcion,subcuenta
              addElementoTextoDom(doc,tag,"Code",filtracadxml(query.value(0).toString()));
              addElementoTextoDom(doc,tag,"Description",filtracadxml(query.value(1).toString()));
              if (esauxiliar(query.value(0).toString()))
                 addElementoTextoDom(doc,tag,"AuxiliaryAccount","true");
                else
                 addElementoTextoDom(doc,tag,"AuxiliaryAccount","false");
          }

   QString xml = doc.toString();


  QFile plan( fichero  );

  if ( !plan.open( QIODevice::WriteOnly ) )
       {
        QMessageBox::warning( this, tr("Exportación de plan contable"),tr("Error: Imposible grabar fichero"));
        return false;
       }
  QTextStream planstream( &plan );
  planstream.setEncoding(QStringConverter::Utf8);

  planstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  planstream << "<?xml-stylesheet type=\"text/xsl\" href=\"plan2html.xslt\"?>\n";

  // ------------------------------------------------------------------------------------
  planstream << xml;

  plan.close();

               // Vemos si hace falta copiar el fichero xslt
           QString fichxslt=dirtrabajobd();
           fichxslt.append(QDir::separator());
           fichxslt.append("plan2html.xslt");
           if (!QFile::exists(fichxslt))
              {
               QString pasa2;
               pasa2=traydoc();
               pasa2.append(QDir::separator());
               pasa2.append("plan2html.xslt");
               if (eswindows()) pasa2=QFile::encodeName(pasa2);
               QFile fichero(pasa2);
               if (eswindows()) fichero.copy(QFile::encodeName(fichxslt));
                  else fichero.copy(fichxslt);
              }

  return true;
}



void MainWindow::exportplansubcuentas()
{
    QString nombre;

  #ifdef NOMACHINE
    directorio *dir = new directorio();
    dir->pasa_directorio(adapta(dirtrabajo));
    dir->activa_pide_archivo(tr("plan_aux.pln"));
    if (dir->exec() == QDialog::Accepted)
      {
        // QMessageBox::information( this, tr("selección"),
        //                         dir->selec_actual() );
        // QMessageBox::information( this, tr("selección"),
        //                         dir->ruta_actual() );
        nombre=dir->nuevo_conruta();
        if (nombre.length()>0)
            {
             // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
             if (nombre.right(4)!=".pln" && nombre.right(4)!=".xml")
                 nombre=nombre.append(".pln");
            }
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
  QString filtrotexto=tr("Archivos de texto con plan (*.pln)");
  filtros << filtrotexto;
  filtros << tr("Archivos xml con plan (*.xml)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo));
  dialogofich.setWindowTitle(tr("ARCHIVO EXPORTAR PLAN DE CONTABILIDAD CON SUBCUENTAS"));
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
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           nombre=fileNames.at(0);
           if (dialogofich.selectedNameFilter()==filtrotexto)
              {
               if (nombre.right(4)!=".pln" && nombre.right(4)!=".xml") nombre=nombre+".pln";
              }
              else if (nombre.right(4)!=".xml" && nombre.right(4)!=".pln") nombre=nombre+".xml";
          }
     }
#endif

  //QMessageBox::information( this, tr("selección"),
  //                         nombre );

  if (nombre.isEmpty()) return;

  QFile plan( adapta(nombre)  );
  if (plan.exists() && QMessageBox::question(this,
      tr("¿ Sobreescribir ? -- Exportar plan"),
      tr("'%1' ya existe."
         "¿ Desea sobreescribirlo ?")
        .arg( nombre ) ) == QMessageBox::No ) return ;
  if (nombre.right(4)==".pln")
     {
       if (exportaplan(nombre,true))
          {
           QMessageBox::information( this, tr("EXPORTAR PLAN"),
                   tr("El archivo se ha generado correctamente"));
          }
     }
     else
         {
          if (exportaplanxml(nombre,true))
             {
              QMessageBox::information( this, tr("EXPORTAR PLAN"),
                   tr("El archivo se ha generado correctamente"));
             }
         }

}




void MainWindow::actualizaplan()
{
  if (QMessageBox::question(
            this, tr("Importar/actualizar plan contable"),
            tr("Este proceso actualiza el plan de cuentas a partir\n"
               "de un fichero de texto. No se efectúa ningún control\n"
               "sobre el contenido de la información. Sólo se dan\n"
               "de alta los códigos inexistentes;\n"
               "sería conveniente realizar una copia de seguridad\n"
               "antes de realizar la operación,\n"
               "¿ Desea continuar ?")) == QMessageBox::No )
                          return;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajo);
  dir->filtrar("*.*");
  if (dir->exec() == QDialog::Accepted)
    {
      QString cadfich=dir->ruta_actual();
      if (importarfichplan(cadfich))
          {
           QMessageBox::information( this, tr("DIARIO"),
                        tr("El archivo seleccionado se ha importado correctamente"));
          }
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

  dialogofich.setNameFilter("*.txt *.pln *.csv");
  dialogofich.setDirectory(dirtrabajo);
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR CUENTAS PLAN"));
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
           QString cadfich=fileNames.at(0);
           if (importarfichplan(cadfich))
               {
                QMessageBox::information( this, tr("DIARIO"),
                             tr("El archivo seleccionado se ha importado correctamente"));
               }
          }
     }
#endif
}

bool MainWindow::importarfichplan(QString qfichero)
{
  if (qfichero.endsWith(".xml") || qfichero.endsWith(".XML"))
    {
     if (carga_saft_plan(qfichero))
      return true;
     else return false;
    }
  QFile fichero(qfichero);
  if (!fichero.exists()) return false;
  if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea;
        while ( !stream.atEnd() )
          {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            QString codigo,descripcion,auxiliar;
            codigo=linea.section('\t',0,0);
            if (existecodigoplan(codigo,&descripcion)) continue;
            if (codigo.length()==0) continue;
            descripcion=linea.section('\t',1,1);
            if (cod_longitud_variable())
                auxiliar=linea.section('\t',2,2);
            if (!cod_longitud_variable() || (cod_longitud_variable() && auxiliar.length()==0))
               {
                basedatos::instancia()->insertPlancontable(codigo, descripcion, false);
               }
               else
                   {
                    if ( (auxiliar == "TRUE") || (auxiliar == "true") || (auxiliar == "1") )
                        basedatos::instancia()->insertPlancontable(codigo, descripcion, true);
                    if ( (auxiliar == "FALSE") || (auxiliar == "false") || (auxiliar == "0") )
                        basedatos::instancia()->insertPlancontable(codigo, descripcion, false);
                   }
         }
        fichero.close();
      } else return false;

  return true;

}


void MainWindow::nuevoasiento()
{
 introfecha *i = new introfecha(punterodiario->fechasel());
 bool noclose=true;
 while (noclose)
  {
   int resultado=i->exec();
   QDate fecha=i->fecha();
   QString diario=i->diario();
   QString doc=i->documento();
   noclose=i->nocerrar();
   if (resultado!=QDialog::Accepted) {delete i; refrescardiario(); return;}
   activa_msj_tabla=i->nocerrar();
   int valor=i->herramienta();
   switch (valor)
    {
     case 0:
       {
        tabla_asientos *t = new tabla_asientos(estilonumerico,!sindecimales,usuario);
        t->pasafechaasiento(fecha);
        t->pasadiario(diario);
        t->pasanocerrar(noclose);
        t->pasa_doc(doc);
        if (punterodiario->borrador()) t->set_borrador();
        t->exec();
        rutacargadocs=!t->ruta_docs().isEmpty() ? t->ruta_docs() : rutacargadocs;
        grabapreferencias();
        delete(t);
       }
      break;

    case 1:
      {
       tabla_asientos *t1 = new tabla_asientos(estilonumerico,!sindecimales,usuario);

       if (activa_msj_tabla) t1->pasanocerrar(true);
       t1->pasa_fecha_iva_aut(fecha);
       t1->soportadoautonomo();
       // tablaasiento->exec();
       delete(t1);
      }
     break;

    case 2:
      apunteaib();
      break;

    case 3:
     {
       tabla_asientos *t = new tabla_asientos(estilonumerico,!sindecimales,usuario);

       if (activa_msj_tabla) t->pasanocerrar(true);

       t->pasa_fecha_iva_aut(fecha);

       t->repercutidoautonomo();
       // tablaasiento->exec();
       delete(t);
     }
    break;

    case 4:
      apunteeib();
      break;

    case 5:
      gestautomaticos();
      break;
    }

   // pasamos a i la fecha, y noclose
   i->pasa_fecha_nocerrar(fecha,noclose);
  }
 delete(i);
 activa_msj_tabla=false;
 refresh_diario2();
}


void MainWindow::apunteaib()
{

  tabla_asientos *t = new tabla_asientos(estilonumerico,!sindecimales, usuario);
  if (activa_msj_tabla) t->pasanocerrar(true);
  t->aibautonomo();
  // tablaasiento->exec();
  delete(t);
  refrescardiario();

}



void MainWindow::apunteeib()
{

  tabla_asientos *t = new tabla_asientos(estilonumerico,!sindecimales, usuario);
  if (activa_msj_tabla) t->pasanocerrar(true);
  t->eibautonomo();
  // tablaasiento->exec();
  delete(t);
  refrescardiario();
}


void MainWindow::gestautomaticos()
{
 QString guardarfiltro=filtroactivo;
 filtroactivo="ejercicio=''";
 refrescardiario();
 //punterodiario->esconde_filtro();

 selecasmod *selec = new selecasmod(usuario,punterodiario->borrador());
 if (activa_msj_tabla) selec->activar_msj_tabla();
 selec->activar_msj_tabla();
 selec->exec();
 delete(selec);
 actualizadockautomaticos();
 filtroactivo=guardarfiltro;
 // refrescardiario();
 //punterodiario->muestra_filtro();
 refresh_diario2();

}


void MainWindow::refresh_diario()
{
     int guardafila=punterodiario->fila_actual();
     QString cadena;
     punterodiario->pasafiltroedlin(filtroactivo);
     punterodiario->pasafiltroedlinb(filtroactivob);
     if (filtroactivo.length()>0) cadena=" WHERE "+filtroactivo+" ";

     if (filtroactivo.isEmpty()) cadena =" WHERE contabilizado";
        else cadena+= " and contabilizado ";
     cadena+=ordenarpor();
     if (!punterodiario->pasafiltro(cadena,estilonumerico,sindecimales))
        {
             QMessageBox::warning(this, tr("KEME-Contabilidad"),
                   tr("El filtro suministrado no es correcto"));
          filtrar_inicio();
        }

     if (filtroactivob.isEmpty()) cadena =" WHERE not contabilizado";
        else cadena= "WHERE " + filtroactivob + " and not contabilizado ";
     cadena+=ordenarpor();

    if (!punterodiario->pasafiltrob(cadena,estilonumerico,sindecimales)) {
        QMessageBox::warning(this, tr("KEME-Contabilidad"),
              tr("El filtro suministrado no es correcto"));
        filtroactivob.clear();
        // qDebug() << cadena;
    }

     if (basedatos::instancia()->essqlite()) punterodiario->refresca(); // nuevo para SQLITE
     punterodiario->cabecera_campo_orden(n_campo_orden());

     punterodiario->pasaanchos(anchocol);
     punterodiario->muestratabla();
     // punterodiario->irfinal();
     punterodiario->irafila(guardafila);

}

void MainWindow::refresh_diario2()
{
     QString cadena;
     punterodiario->pasafiltroedlin(filtroactivo);
     punterodiario->pasafiltroedlinb(filtroactivob);
     if (filtroactivo.length()>0) cadena=" WHERE "+filtroactivo+" ";
     if (filtroactivo.isEmpty()) cadena =" WHERE contabilizado";
        else cadena+= " and contabilizado ";

     cadena+=ordenarpor();
     if (!punterodiario->pasafiltro(cadena,estilonumerico,sindecimales))
        {
             QMessageBox::warning(this, tr("KEME-Contabilidad"),
                   tr("El filtro suministrado no es correcto"));
          filtrar_inicio();
        }

     if (filtroactivob.isEmpty()) cadena =" WHERE not contabilizado";
        else cadena= "WHERE " + filtroactivob + " and not contabilizado ";
     cadena+=ordenarpor();

     if (!punterodiario->pasafiltrob(cadena,estilonumerico,sindecimales)) {
         QMessageBox::warning(this, tr("KEME-Contabilidad"),
               tr("El filtro suministrado no es correcto"));
         filtroactivob.clear();
         // qDebug() << cadena;
     }


     if (basedatos::instancia()->essqlite()) punterodiario->refresca(); // nuevo para SQLITE

     punterodiario->cabecera_campo_orden(n_campo_orden());

     punterodiario->pasaanchos(anchocol);
     punterodiario->muestratabla();

}



void MainWindow::refrescardiario()
{
  refresh_diario();
}


void MainWindow::filtrar_inicio()
{
    filtroactivo=lingu_a_filtro(condicionesfiltrodefecto());
    refrescardiario();

}


void MainWindow::actualizadockautomaticos()
{

 //----------------------------------------------------
 treeWidget->clear();
 //modeloarbol.borraarbol();
 cargaarbol();
 // modeloarbol.resetea();
 // treeView->expandAll();

}


void MainWindow::editaasiento()
{
    QString elasiento;
    qlonglong numasiento=punterodiario->asientoactual();
    QDate fechapase=punterodiario->fechapaseactual();
    if (numasiento!=0) elasiento.setNum(numasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("EDICIÓN DE ASIENTOS"),
         tr("Para editar asiento debe de seleccionar un apunte del mismo en el diario"));
         return;
       }
    QString ejercicio=ejerciciodelafecha(fechapase);
    // fechapase=punterodiario->fechapaseactual();

    if (ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio))
     {
       consultarasiento(elasiento,usuario,ejercicio);
     }
     else
         {
          editarasiento(elasiento,usuario,ejercicio);
          refrescardiario();
         }
}


void MainWindow::editanumasiento()
{
  pidenombre *numas=new pidenombre();
  numas->asignanombreventana(tr("DIARIO"));
  numas->asignaetiqueta(tr("Número de asiento:"));
  numas->exec();
  QString elasiento=numas->contenido();
  delete(numas);

        if (elasiento=="")
       {
         return;
       }

    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("DIARIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString ejercicio=ej->contenido();
    // qlonglong absoluto=primerasiento(elejercicio)+elasiento.toLongLong()-1;
    // elasiento.setNum(absoluto);


      if (!existeasiento(elasiento,ejercicio))
        {
         QMessageBox::warning( this, tr("EDICIÓN DE ASIENTOS"),
                   tr("ERROR: El número suministrado no existe en el diario"));
         return;
        }

      //QDate fechapase;
      //fechapase=fechadeasiento(elasiento);
      // QString ejercicio=ejerciciodelafecha(fechapase);
      if (ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio))
       {
         QMessageBox::warning( this, tr("EDICIÓN DE ASIENTOS"),
                   tr("ERROR: El asiento corresponde a un ejercicio cerrado"));
         return;
       }

    editarasiento(elasiento,usuario,ejercicio);
    refrescardiario();
}


void MainWindow::consultaasiento()
{
    QString elasiento;
    qlonglong numasiento=punterodiario->asientoactual();
    QString ejercicio=ejerciciodelafecha(punterodiario->fechapaseactual());
    if (numasiento!=0) elasiento.setNum(numasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("EDICIÓN DE ASIENTOS"),
         tr("Para consultar asiento debe de seleccionar un pase del mismo en el diario"));
         return;
       }
    consultarasiento(elasiento,usuario,ejercicio);
}


void MainWindow::consultanumasiento()
{
  pidenombre *numas=new pidenombre();
  numas->asignanombreventana(tr("DIARIO"));
  numas->asignaetiqueta(tr("Número de asiento:"));
  numas->exec();
  QString elasiento=numas->contenido();
  delete(numas);

        if (elasiento=="")
       {
         return;
       }

    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("DIARIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();
    //qlonglong absoluto=primerasiento(elejercicio)+elasiento.toLongLong()-1;
    //elasiento.setNum(absoluto);


      if (!existeasiento(elasiento,elejercicio))
        {
         QMessageBox::warning( this, tr("EDICIÓN DE ASIENTOS"),
                               tr("ERROR: El número suministrado no existe en el diario"));
         return;
        }

    consultarasiento(elasiento,usuario,elejercicio);
    refrescardiario();
}


void MainWindow::activa_revisado()
{
    QString elasiento;
    qlonglong numasiento=punterodiario->asientoactual();
    QDate fechapase=punterodiario->fechapaseactual();
    if (numasiento!=0) elasiento.setNum(numasiento);
    if (elasiento.isEmpty())
       {
         QMessageBox::warning( this, tr("EDICIÓN DE ASIENTOS"),
         tr("Para marcar un asiento debe de seleccionar un apunte del mismo en el diario"));
         return;
       }
    QString ejercicio=ejerciciodelafecha(fechapase);
    // fechapase=punterodiario->fechapaseactual();

    if (ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio))
    {
      QMessageBox::warning( this, tr("EDICIÓN DE ASIENTOS"),
                            tr("ERROR: EJERCICIO CERRADO"));
      return;
    }
    bool marc=punterodiario->apunterevisado();
    basedatos::instancia()->marca_apunte_revisado(numasiento, ejercicio, marc);
    refrescardiario();
}


void MainWindow::asigna_ficherodocumento()
{
    QString elasiento;
    qlonglong numasiento=punterodiario->asientoactual();
    if (numasiento!=0) elasiento.setNum(numasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("ASIGNAR FICHERO A DOCUMENTO"),
         tr("Para realizar una asignación debe de seleccionar un apunte en el diario"));
         return;
       }
    QDate fechapase=punterodiario->fechapaseactual();
    QString ejercicio=ejerciciodelafecha(fechapase);

    if (ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio))
     {
        QMessageBox::warning( this, tr("ASIGNAR FICHERO A DOCUMENTO"),
                              tr("ERROR: ejercicio cerrado"));
        return;
     }

    if (basedatos::instancia()->ejercicio_bloqueado(ejercicio))
     {
        QMessageBox::warning( this, tr("ASIGNAR FICHERO A DOCUMENTO"),
                              tr("ERROR: ejercicio bloqueado"));
        return;
     }


    // QMessageBox::warning( this, tr("ASIGNAR FICHERO A DOCUMENTO"),
    //                      punterodiario->documentoactual());

    asigna_fichdoc *a = new asigna_fichdoc(elasiento,punterodiario->documentoactual(),
                                           ejercicio);
    a->pasafichdoc(basedatos::instancia()->copia_docdiario (elasiento,ejercicio));

    a->exec();
    if (!a->ruta_docs().isEmpty())
       {
        rutacargadocs=a->ruta_docs();
        grabapreferencias();
       }
    delete(a);
    refrescardiario();

}


void MainWindow::borraasiento()
{
    QString elasiento;
    qlonglong numasiento=punterodiario->asientoactual();
    QString ejercicio=ejerciciodelafecha(punterodiario->fechapaseactual());

    if (numasiento!=0) elasiento.setNum(numasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("BORRA ASIENTO"),
         tr("Para borrar un asiento debe de seleccionar un pase del mismo en el diario"));
         return;
       }

    borrarasientofunc(elasiento,ejercicio,true);

    if (basedatos::instancia()->papelera_vacia())
       {
        QIcon icono(":/iconos/graf/trash-icon.png");
        ui->actionPapelera->setIcon(icono);
       }
       else
           {
            QIcon icono(":/iconos/graf/Recycle-Bin-full-icon.png");
            ui->actionPapelera->setIcon(icono);
           }

   refrescardiario();
}


void MainWindow::filtrardiario()
{
 filtrodiario *f = new filtrodiario(estilonumerico);
 if (punterodiario->borrador()) {
    f->pasafiltro(filtro_a_lingu(filtroactivob));
 }
 else {
     f->pasafiltro(filtro_a_lingu(filtroactivo));
     f->pasacondicionesfiltrodefecto(condicionesfiltrodefecto());
 }
 int resultado = f->exec();
 if (resultado == QDialog::Accepted)
      {
       if (punterodiario->borrador()){
           filtroactivob=lingu_a_filtro(f->filtro());
       } else {
           filtroactivo=lingu_a_filtro(f->filtro());
           if (filtroactivo.trimmed().isEmpty() && QMessageBox::question(
               this, tr("Filtrar diario"),
               tr("El filtro está vacío\n"
                  "¿ añadir filtro por defecto ?\n")) ==QMessageBox::Yes )
               filtroactivo=condicionesfiltrodefecto();
       }
       refrescardiario();
      }
 delete(f);
}

void MainWindow::visdoc()
{
  QString fichdoc=punterodiario->fichdocumentoactual();

   if (fichdoc.isEmpty())
     {
       int pase=punterodiario->paseactual();
       QString qapunte;
       if (pase>0) qapunte.setNum(pase);
       QString serie, numero;
       if (qapunte!="")
          {
           if (basedatos::instancia()->pase_en_facturas(qapunte,&serie,&numero))
              {

               QString archivopdf=nombre_fich_factura_apunte(qapunte);

               //QMessageBox::warning( this, tr("Edición de documentos"),archivopdf);

               if (!QFile::exists(archivopdf)) {

                  QString fichero=latex_doc(serie,numero);
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
                  archivopdf=fichero;
                  archivopdf.truncate(archivopdf.length()-4);
                  archivopdf.append(".pdf");
               }
               QString url=archivopdf;
               QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
               /*if (!ejecuta(aplicacionabrirfich(extensionfich(archivopdf)),archivopdf))
                     QMessageBox::warning( this, tr("FACTURA EN PDF"),
                                         tr("No se puede abrir ")+archivopdf+tr(" con ")+
                                         aplicacionabrirfich(extensionfich(archivopdf)) + "\n" +
                                         tr("Verifique el diálogo de preferencias"));*/

              }
          }
       return;
     }

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

  /* if (!ejecuta(aplicacionabrirfich(extensionfich(fichdoc)),fichdoc))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+fichdoc+tr(" con ")+
                             aplicacionabrirfich(extensionfich(fichdoc)));*/

}


void MainWindow::copiar()
{
 QItemSelectionModel *seleccion;
 CustomSqlModel *model=punterodiario->modelo();
 seleccion=punterodiario->tabladiario()->selectionModel();
 if (seleccion->hasSelection())
    {

      QList<QModelIndex> listapases;
      listapases=seleccion->selectedRows(9);
      // qlonglong max, min;
      // max=model->datagen(listapases.at(0),Qt::DisplayRole).toLongLong();
      // min=model->datagen(listapases.at(0),Qt::DisplayRole).toLongLong();
      QList<int> pases;
      for (int i = 0; i < listapases.size(); ++i)
          {
           int numero=model->datagen(listapases.at(i),Qt::DisplayRole).toInt();
           pases << numero;
           // if (numero>max) max=numero;
           // if (numero<min) min=numero;
          }
      std::sort(pases.begin(),pases.end());
      QString valores;
      for (int i = 0; i < pases.size(); ++i)
           {
            // asiento,fecha,cuenta,concepto,debe,haber,documento,ci
            QString pase; pase.setNum(pases.at(i));
            QSqlQuery query=basedatos::instancia()->infocopiardepase (pase);
            if (query.first())
               {
                 valores += query.value(0).toString();
                 valores+="\t";
                 valores+=query.value(1).toDate().toString("dd/MM/yyyy");
                 valores+="\t";
                 valores+=query.value(2).toString();
                 valores+="\t";
                 valores+=query.value(3).toString();
                 valores+="\t";
                 if (estilonumerico==COMADECIMAL)
                   valores+=convacoma(query.value(4).toString()); // debe
                  else
                    valores+=query.value(4).toString(); // debe
                 valores+="\t";
                 if (estilonumerico==COMADECIMAL)
                   valores+=convacoma(query.value(5).toString()); // haber
                  else
                    valores+=query.value(5).toString(); // haber
                 valores+="\t";
                 valores+=query.value(6).toString();
                 valores+="\t";
                 valores+=query.value(7).toString();
                 valores+="\n";
               }
           }
      QClipboard *clipboard = QApplication::clipboard();
      clipboard->setText(valores);
      QMessageBox::information( this, tr("DIARIO"),
                             tr("El contenido de la selección se ha pasado al portapapeles"));
      // QMessageBox::warning( this, tr("HOLA"),valores);
    }
}

void MainWindow::pegar()
{
 // escribimos contenido del portapales a fichero
 QString qfichero=dirtrabajo;
 QString cad="/portapales.txt";
 cad[0]=QDir::separator();
 qfichero+=cad;
 QFile fichero(adapta(qfichero));
 if (! fichero.open( QIODevice::WriteOnly ) ) return;
 QTextStream stream( &fichero );
 stream.setEncoding(QStringConverter::Utf8);

 QClipboard *clipboard = QApplication::clipboard();
 stream <<  clipboard->text() ;
 fichero.close();

 if (importarfichasientos(qfichero))
     {
      refrescardiario();
      QMessageBox::information( this, tr("DIARIO"),
                             tr("El contenido del portapapeles se ha pegado en el diario"));
      punterodiario->irfinal();
     }
 // refrescardiario();
 // QModelIndex guardaindice=punterodiario->indiceactual();
 // punterodiario->situate(guardaindice);
}

void MainWindow::cortar()
{
 QItemSelectionModel *seleccion;
 CustomSqlModel *model=punterodiario->modelo();
 seleccion=punterodiario->tabladiario()->selectionModel();
 if (seleccion->hasSelection())
    {
      QList<QModelIndex> listaasientos;
      QList<QModelIndex> listafechas;
      QList<QModelIndex> listacuentas;
      QList<QModelIndex> listaconceptos;
      QList<QModelIndex> listadebe;
      QList<QModelIndex> listahaber;
      QList<QModelIndex> listadocumentos;
      QList<QModelIndex> listapases;
      listaasientos= seleccion->selectedRows ( 2 );
      listafechas=seleccion->selectedRows (1);
      listacuentas=seleccion->selectedRows (0);
      listaconceptos=seleccion->selectedRows (3);
      listadebe=seleccion->selectedRows (4);
      listahaber=seleccion->selectedRows (5);
      listadocumentos=seleccion->selectedRows (6);
      listapases=seleccion->selectedRows (9);
      // averiguamos si el anterior pase al primero pertenece al mismo asiento
      qlonglong anterior=model->datagen(listapases.at(0),Qt::DisplayRole).toLongLong();
      anterior--;
      QString cadnum;
      cadnum.setNum(anterior);
      if (paseenasiento(cadnum,model->datagen(listaasientos.at(0),Qt::DisplayRole).toString() ))
          {
            QMessageBox::warning( this, tr("DIARIO"),
                       tr("ERROR: no se puede cortar una selección con asientos incompletos"));
            return;

          }
      // averiguamos si el posteror pase al último asiento pertenece al mismo asiento
      qlonglong posterior=model->datagen(listapases.at(listaasientos.size()-1),Qt::DisplayRole).toLongLong();
      posterior++;
      cadnum.setNum(posterior);
      if (paseenasiento(cadnum,model->datagen(listaasientos.at(listaasientos.size()-1),Qt::DisplayRole).toString() ))
          {
            QMessageBox::warning( this, tr("DIARIO"),
                       tr("ERROR: no se puede cortar una selección con asientos incompletos"));
            return;

          }
      QString valores;
      for (int i = 0; i < listaasientos.size(); ++i)
           {
            if (fechadejerciciocerrado(model->datagen(listafechas.at(i),Qt::DisplayRole).toDate()))
               {
                QMessageBox::warning( this, tr("DIARIO"),
                       tr("ERROR: No se pueden cortar asientos de ejercicios cerrados"));
                return;
               }
            if (basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(model->datagen(listafechas.at(i),Qt::DisplayRole).toDate())))
               {
                QMessageBox::warning( this, tr("DIARIO"),
                       tr("ERROR: No se pueden cortar asientos de ejercicios bloqueados"));
                return;
               }
            valores+=model->datagen(listaasientos.at(i),Qt::DisplayRole).toString();
            valores+="\t";
            valores+=model->data(listafechas.at(i),Qt::DisplayRole).toString();
            valores+="\t";
            valores+=model->datagen(listacuentas.at(i),Qt::DisplayRole).toString();
            valores+="\t";
            valores+=model->datagen(listaconceptos.at(i),Qt::DisplayRole).toString();
            valores+="\t";
            if (estilonumerico==COMADECIMAL)
                 valores+=convacoma(model->datagen(listadebe.at(i),Qt::DisplayRole).toString());
            else
                 valores+=model->datagen(listadebe.at(i),Qt::DisplayRole).toString();
            valores+="\t";
            if (estilonumerico==COMADECIMAL)
                valores+=convacoma(model->datagen(listahaber.at(i),Qt::DisplayRole).toString());
            else
                valores+=model->datagen(listahaber.at(i),Qt::DisplayRole).toString();
            valores+="\t";
            valores+=model->datagen(listadocumentos.at(i),Qt::DisplayRole).toString();
            valores+="\n";
           }

      if (ejerciciodelafecha(model->datagen(listafechas.at(0),Qt::DisplayRole).toDate())!=
         ejerciciodelafecha(model->datagen(listafechas.at(listaasientos.size()-1),
               Qt::DisplayRole).toDate()))
        {
         QMessageBox::warning( this, tr("DIARIO"),
                             tr("ERROR: en el intervalo hay asientos de diferentes ejercicios"));
         return;
        }

      QString ejercicio;
      ejercicio=ejerciciodelafecha(model->datagen(listafechas.at(0),Qt::DisplayRole).toDate());
      if (amortenintasientos(model->datagen(listaasientos.at(0),Qt::DisplayRole).toString(),
                           model->datagen(listaasientos.at(listaasientos.size()-1),Qt::DisplayRole).toString(),
                           ejercicio))
        {
         QMessageBox::warning( this, tr("DIARIO"),
                             tr("ERROR: en el intervalo hay asientos de amortizaciones"));
         return;
        }

      QMessageBox msgBox;
      QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
      QPushButton *abortButton = msgBox.addButton(tr("No"), QMessageBox::ActionRole);


      QString TextoAviso = tr("Se van a eliminar los asientos del diario seleccionados \n"
                              "junto con los registros de IVA y vencimientos\n\n"
                              "¿ desea continuar ?");
      msgBox.setWindowTitle(tr("ADVERTENCIA"));
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setText(TextoAviso);

      msgBox.exec();

      if (msgBox.clickedButton() == abortButton)
          return;
      else if (msgBox.clickedButton() == aceptarButton)
            {
             QClipboard *clipboard = QApplication::clipboard();
             clipboard->setText(valores);
             // borramos asientos
             borraasientos(model->datagen(listaasientos.at(0),Qt::DisplayRole).toString(),
                           model->datagen(listaasientos.at(listaasientos.size()-1),Qt::DisplayRole).toString(),
                           ejercicio);
            }
      refrescardiario();

      QMessageBox::information( this, tr("DIARIO"),
                             tr("El contenido de la selección se ha pasado al portapapeles"));
      // QMessageBox::warning( this, tr("HOLA"),valores);
      punterodiario->irfinal();
    }
}


bool MainWindow::importarfichasientos(QString qfichero)
{
  QFile fichero(adapta(qfichero));
  if (!fichero.exists()) return false;
  if (qfichero.endsWith(".xml") || qfichero.endsWith(".XML"))
    {
     if (carga_saft_asientos(qfichero,usuario))
      return true;
     else return false;
    }

  int numlineas=0;
  // contamos número de líneas del fichero
   if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        while ( !stream.atEnd() ) {
            QString linea;
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            numlineas++;
            }
        fichero.close();
      } else return false;

    int lineaactu=0;

    QProgressDialog progress(tr("Comprobando diario..."), NULL, 0, numlineas, this);
    // progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration ( 100 );
    //progress.setValue(1);
    QApplication::processEvents();

  // primero comprobamos que la información es correcta
   if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea;
        long numasiento=0;
        long guardaasiento=-1;
        double sumadebe=0;
        double sumahaber=0;
     while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            lineaactu++;
            // numlineas++;
            progress.setValue(lineaactu);
            QApplication::processEvents();
            // analizamos la linea de texto
           //  asiento fecha concepto cuenta debe haber documento ----- separados por tabulador
           QString asiento,fecha,cuenta,concepto,debe,haber;
           asiento=linea.section('\t',0,0);
           numasiento=asiento.toLong();
           if (guardaasiento==-1) guardaasiento=numasiento;
           fecha=linea.section('\t',1,1);
           // corregir orden en fecha
           QString dia=fecha.left(2);
           QString mes=fecha.mid(3,2);
           QString anyo=fecha.right(4);
           if (fecha.length()<9) anyo=fecha.right(2);
           fecha=anyo+".";
           fecha+=mes;
           fecha+=".";
           fecha+=dia;
           QDate lafecha;
           int valanyo=anyo.toInt();
           if (valanyo<100 && valanyo>50) valanyo+=1900;
           if (valanyo<100 && valanyo<=50) valanyo+=2000;
           lafecha.setDate(valanyo,mes.toInt(),dia.toInt());
           if (!fechacorrespondeaejercicio(lafecha) || fechadejerciciocerrado(lafecha) || basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(lafecha)))
             {
                 QMessageBox::warning( this, tr("Importar asientos"),
                       tr("Error, la fecha %1 no es correcta\n"
                          "la operación no se ha realizado.").arg(fecha));
                 return false;
             }
     cuenta=linea.section('\t',2,2);
     concepto=linea.section('\t',3,3);
     debe=convapunto(linea.section('\t',4,4));
     haber=convapunto(linea.section('\t',5,5));
     if (!existesubcuenta(cuenta))
       {
                 QMessageBox::warning( this, tr("Importar asientos"),
                       tr("Error, código de cuenta inexistente %1\n"
                          "la operación no se ha realizado.").arg(cuenta));
                 return false;
       }
     if (escuentadeivasoportado(cuenta) || escuentadeivarepercutido(cuenta))
       {
                 QMessageBox::warning( this, tr("Importar asientos"),
                       tr("Error, no se pueden incorporar asientos con cuentas de IVA \n"
                          "la operación no se ha realizado."));
                 return false;
       }

      if ((debe.toDouble()>0.001 || debe.toDouble()<-0.001)
      && (haber.toDouble()>0.001 || haber.toDouble()<-0.001))
       {
                 QMessageBox::warning( this, tr("Importar asientos"),
                       tr("Error, importes no correctos,\n"
                          "la operación no se ha realizado."));
                 return false;
       }
           if (numasiento!=guardaasiento)
       {
           if (sumadebe-sumahaber>0.001 || sumadebe-sumahaber<-0.001)
             {
                 QString cd,cd1,cd2;
                 cd.setNum(guardaasiento);
                 cd1.setNum(sumadebe,'f',2);
                 cd2.setNum(sumahaber,'f',2);
                 QMessageBox::warning( this, tr("Importar asientos"),
                       tr("Error, asiento %1 descuadrado %2 %3,\n"
                          "la operación no se ha realizado.").arg(cd,cd1,cd2));
                 return false;

             }
         guardaasiento=numasiento;
         sumadebe=0;
         sumahaber=0;
       }
           sumadebe+=debe.toDouble();
           sumahaber+=haber.toDouble();
          }


       if (sumadebe-sumahaber>0.001 || sumadebe-sumahaber<-0.001)
          {
             QString cd,cd1,cd2;
             cd.setNum(guardaasiento);
             cd1.setNum(sumadebe,'f',2);
             cd2.setNum(sumahaber,'f',2);
             QMessageBox::warning( this, tr("Importar asientos"),
               tr("Error, asiento %1 descuadrado %2 %3,\n"
                  "la operación no se ha realizado.").arg(cd,cd1,cd2));
             return false;
          }



        fichero.close();
      } else return false;

      progress.close();

      // volvemos a abrir el fichero para procesar el asiento
      // actualizar saldossubcuentas
      // ojo con la fecha
      // eliminar ' y " de cadenas
      // cambiar comadecimal por punto decimal

   if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea;
        QString cadfecha;
        long numasiento=0;
        long guardaasiento=-1;

        QString cadnumasiento;
        QSqlDatabase::database().transaction();
        basedatos::instancia()->bloquea_para_importar_asientos();

        lineaactu=0;
        QProgressDialog progress2(tr("Importando diario..."), NULL, 0, numlineas, this);
        // progress.setWindowModality(Qt::WindowModal);
        progress2.setMinimumDuration ( 100 );
        //progress.setValue(1);
        QApplication::processEvents();

        while ( !stream.atEnd() ) {
           lineaactu++;
           progress2.setValue(lineaactu);
           QApplication::processEvents();
           linea = stream.readLine(); // linea de texto excluyendo '\n'
           // analizamos la linea de texto
           //  asiento fecha concepto cuenta debe haber documento ----- separados por tabulador
           QString asiento,fecha,cuenta,concepto,debe,haber;
           asiento=linea.section('\t',0,0);
           numasiento=asiento.toLong();
           // if (guardaasiento==-1) guardaasiento=numasiento;
           fecha=linea.section('\t',1,1);
           fecha=quitacomillas(fecha);
           // corregir orden en fecha
           QString dia=fecha.left(2);
           QString mes=fecha.mid(3,2);
           QString anyo;
           anyo=fecha.right(4);
           if (fecha.length()<9)
             {
              int valanyo=fecha.right(2).toInt();
              if (valanyo<50) valanyo+=2000;
                 else valanyo+=1900;
              anyo.setNum(valanyo);
             }
           fecha=anyo+"-";
           fecha+=mes;
           fecha+="-";
           fecha+=dia;
           QDate fechaasiento(anyo.toInt(),mes.toInt(),dia.toInt());
           if (cadfecha.length()==0) cadfecha=fecha;
           cuenta=linea.section('\t',2,2);
           cuenta=quitacomillas(cuenta);
           concepto=linea.section('\t',3,3);
           concepto=quitacomillas(concepto);
           debe=linea.section('\t',4,4);
           debe=convapunto(debe);
           if (debe.isEmpty()) debe="0";
           haber=linea.section('\t',5,5);
           haber=convapunto(haber);
           if (haber.isEmpty()) haber="0";
           QString documento=linea.section('\t',6,6);
           QString ci=linea.section('\t',7,7);
           QString ejercicio=ejerciciodelafecha(fechaasiento);
           /*if (sobreescribeproxasiento(ejercicio) && numasiento==0)
             {
              QMessageBox::warning( this, tr("Importar asientos"),
                                   tr("Error, no se pueden sobreescribir asientos\n"
                                      "revise número de próximo asiento en configuración."));
              break;
             }*/
      if (numasiento!=guardaasiento)
       {
         guardaasiento=numasiento;
         cadfecha=fecha;

         /*if (sobreescribeproxasiento(ejercicio))
           {
                 QMessageBox::warning( this, tr("Importar asientos"),
               tr("Error, no se pueden sobreescribir asientos\n"
                  "revise número de próximo asiento en configuración."));
              break;
           }*/
         // averiguamos número de asiento

         qlonglong vnum = basedatos::instancia()->proximoasiento(ejercicio);

         QString cadnum; cadnum.setNum(vnum+1);
         cadnumasiento.setNum(vnum);

         basedatos::instancia()->update_ejercicio_prox_asiento(
         ejercicio, cadnum);
             //  ----------------------------------------------------------
       }
           // procesamos aquí adición de pase
       QString cadnumpase;
       qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
       cadnumpase.setNum(pase);

       double ddebe = 0, dhaber = 0;
       if (debe.length() != 0)
           ddebe = debe.toDouble();
       if (haber.length() != 0)
           dhaber = haber.toDouble();
       basedatos::instancia()->insertpegaDiario(cadnumasiento, pase, cadfecha, cuenta, ddebe, dhaber,
                                   concepto, documento,ci,usuario,ejercicio);


           // queda actu saldo y proxpase
           /*QString qdebe, qhaber;
           if (debe.length()==0) qdebe = "0";
           else qdebe = debe;
           if (haber.length()==0) qhaber = "0";
           else qhaber = haber;*/

           // basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(qhaber, qdebe, cuenta);

           pase++;
           cadnumpase.setNum(pase);

           basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
           // -------------------------------------------------------------------
          } // final del while
        basedatos::instancia()->desbloquea_y_commit();
        fichero.close();
        // calculaprimerosasientos();
      } else return false;

  return true;

}



void MainWindow::asientosespera()
{
   cargaespera *lacargaespera=new cargaespera();
int resultado=lacargaespera->exec();
if (resultado==QDialog::Accepted)
  {
    // procesamos asiento en espera actual  en cargaespera
    qlonglong AstoEspera=lacargaespera->asientoactual();
    if (AstoEspera>0)
    {
       tabla_asientos *tablaasiento=new tabla_asientos(estilonumerico,!sindecimales, usuario);
       if (punterodiario->borrador()) tablaasiento->set_borrador();

       QSqlQuery query = basedatos::instancia()->select20Borradorasiento(AstoEspera);
       int fila=0;
            if ( query.isActive() ) {
                while ( query.next() ) {
                    if (fila==0) tablaasiento->pasafechaasiento(query.value(32).toDate());
            tablaasiento->pasadatos(fila,query.value(0).toString() ,
                                                          query.value(1).toString(),
                                                          query.value(2).toString(),
                                                          query.value(3).toString(),
                                                          query.value(4).toString(),
                                                          query.value(5).toString(),
                                                          query.value(6).toString(),
                                                          query.value(7).toString(),
                                                          query.value(8).toString(),
                                                          query.value(9).toString(),
                                                          query.value(10).toString(),
                                                          query.value(11).toString(),
                                                          query.value(12).toString(),
                                                          query.value(13).toString(),
                                                          query.value(14).toString(),
                                                          query.value(15).toString(),
                                                          query.value(16).toString(),
                                                          query.value(17).toString(),
                                                          query.value(18).toString(),
                                                          query.value(19).toString(),
                                                          query.value(20).toString(),
                                                          query.value(21).toString(),
                                                          query.value(22).toString(),
                                                          query.value(23).toString(),
                                                          query.value(24).toString(),
                                                          query.value(25).toString(),
                                                          query.value(26).toString(),
                                                          query.value(27).toString(),
                                                          query.value(28).toString(),
                                                          query.value(29).toString(),
                                                          query.value(30).toString(),
                                                          query.value(31).toString(),
                                                          query.value(33).toString(),
                                                          query.value(34).toString(),

                                                          query.value(35).toString(),
                                                          query.value(36).toString(),
                                                          query.value(37).toString(),
                                                          query.value(38).toString(),
                                                          query.value(39).toString(),
                                                          query.value(40).toString(),
                                                          query.value(41).toString(),
                                                          query.value(42).toString(),
                                                          query.value(43).toString(),
                                                          query.value(44).toString(),
                                                          query.value(45).toString(),
                                                          query.value(46).toString(),
                                                          query.value(47).toString(),
                                                          query.value(48).toString(),
                                                          query.value(49).toString(),
                                                          query.value(50).toString(),
                                                          query.value(51).toString(),

                                                          query.value(52).toString(),
                                                          query.value(53).toString(),
                                                          query.value(54).toString(),
                                                          query.value(55).toString(),
                                                          query.value(56).toString(),
                                                          query.value(57).toString(),
                                                          query.value(58).toString(),
                                                          query.value(59).toString(),
                                                          query.value(60).toString(),
                                                          query.value(61).toString()
                                                          ); // 8 cols más y externo + concepto_sii
                    // aquí verificar si analítica tabla y cuenta de analítica
                    if (basedatos::instancia()->analitica_tabla() &&
                        tablaasiento->cuenta_para_analitica(query.value(0).toString()))
                       {
                         tablaasiento->realiza_asignainputdiario(fila,
                           query.value(2).toString().isEmpty() ? query.value(3).toString() :
                           query.value(2).toString());
                       }
            fila++;
        }
        }
        lacargaespera->elimasientoactual();
        tablaasiento->chequeatotales();
        tablaasiento->exec();
        delete(tablaasiento);
        refrescardiario();
       }
  }
 delete(lacargaespera);
}


void MainWindow::impridiario()
{

    QString elasiento;
    qlonglong numasiento=punterodiario->asientoactual();
    QDate fechapase=punterodiario->fechapaseactual();
    if (numasiento!=0) elasiento.setNum(numasiento);
    QString ejercicio=ejerciciodelafecha(fechapase);
    QString filtro2;
    if (elasiento!="")
       {
        filtro2="asiento='";
        filtro2+=elasiento;
        filtro2+="' and ejercicio='";
        filtro2+=ejercicio;
        filtro2+="'";
       }


   imprimediario *laimpri=new imprimediario();
   laimpri->pasafiltro( filtroactivo );
   laimpri->pasafiltro_asiento(filtro2);
   laimpri->exec();
   delete(laimpri);

}



void MainWindow::edicionconceptos()
{
 conceptos *c = new conceptos();
 c->exec();
 delete(c);
}



void MainWindow::regulariza()
{
  regularizacion *reg = new regularizacion(usuario);
  reg->exec();
  delete(reg);
  refrescardiario();
}

void MainWindow::cierra()
{
  cierre *c = new cierre(usuario);
  c->exec();
  delete(c);
  refrescardiario();
}


void MainWindow::suprimircierre()
{
  suprimecierre *c = new suprimecierre();
  c->exec();
  delete(c);
  refrescardiario();
}

void MainWindow::apert()
{
  apertura *a = new apertura(usuario);
  a->exec();
  delete(a);
  refrescardiario();
}


void MainWindow::reg_cierre_apertura()
{
    pideejercicio *p = new pideejercicio;
    p->asignanombreventana(tr("Regularización, cierre, apertura"));
    p->exec();
    QString ejercicio=p->contenido();
    delete(p);

    // ejercicio anterior ? -- lo calculamos
    QDate fecha=finejercicio(ejercicio);
    fecha=fecha.addDays(1);
    QString ejercicio_posterior=ejerciciodelafecha(fecha);
    // QMessageBox::warning( this, tr("Cierre"),fecha.toString("dd-MM-yyyy"));
    if (ejercicio_posterior.isEmpty())
       {
        QMessageBox::warning( this, tr("Cierre"),
                              tr("ERROR: No está definido el ejercicio de nueva apertura"));
        return;
       }

    QProgressDialog progress(tr("Procesando cierre..."), "", 0, 3, this);
    // progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration ( 100 );
    progress.setValue(1);
    QApplication::processEvents();


    bool correcto=true;
    regularizacion *reg = new regularizacion(usuario);
    reg->proc_externo();
    reg->generar_ejercicio(ejercicio);
    correcto=reg->escorrecto();
    delete(reg);

    if (!correcto) return;

    progress.setValue(2);
    QApplication::processEvents();


    cierre *c = new cierre(usuario);
    c->proc_externo();
    c->generar_ejercicio(ejercicio);
    correcto=c->escorrecto();
    delete(c);

    if (!correcto) return;


    apertura *a = new apertura(usuario);
    a->proc_externo();
    a->procesar_ejercicio(ejercicio_posterior);
    correcto=c->escorrecto();
    delete(a);

    progress.setValue(3);
    QApplication::processEvents();

    if (correcto)
        QMessageBox::information( this, tr("Cierre"),tr("Operación realizada"));

    refrescardiario();
}


void MainWindow::ordenarpor_campo()
{
 // orden_campo_diariario=
 int num_campo=punterodiario->columna_actual();


 // cuenta, fecha, asiento, concepto, debe,
 // haber, documento, diario, usuario, pase,ci,
 // clave_ci, copia_doc, ejercicio

 switch (num_campo)
 {
   case 1:
       // orden por fecha
       orden_campo_diario.clear();
       if (ui->actionOrden_numero->isChecked()) ui->actionOrden_numero->setChecked(false);
          else
              qordendiario(false);
       break;
   case 2:
       // orden por asiento
       orden_campo_diario.clear();
       if (!ui->actionOrden_numero->isChecked()) ui->actionOrden_numero->setChecked(true);
          else
              qordendiario(true);
       break;
  case 0: orden_campo_diario="cuenta"; break;
  case 3: orden_campo_diario="concepto"; break;
  case 4: orden_campo_diario="debe"; break;
  case 5: orden_campo_diario="haber"; break;
  case 6: orden_campo_diario="documento"; break;
  case 7: orden_campo_diario="diario"; break;
  case 8: orden_campo_diario="usuario"; break;
  case 9: orden_campo_diario="pase"; break;
  case 10: orden_campo_diario="ci"; break;
  case 11: orden_campo_diario="clave_ci"; break;
  case 12: orden_campo_diario="copia_doc"; break;
  case 13: orden_campo_diario="ejercicio"; break;
  case 14: orden_campo_diario="nrecepcion"; break;
  case 15: orden_campo_diario="revisado"; break;

 }


 if (!orden_campo_diario.isEmpty()) refrescardiario();

}

void MainWindow::tiposdiario()
{
 diarios *d = new diarios();
 d->exec();
 delete(d);
}



void MainWindow::regulparc()
{
  regularizaciones *r = new regularizaciones();
  r->exec();
  delete(r);
}

void MainWindow::ejecuta_regul()
{
  exec_regul *r = new exec_regul(punterodiario->borrador());
  r->exec();
  delete(r);
 refrescardiario();
}

void MainWindow::importa_reg()
{

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajo));
  dir->filtrar("*.reg.xml");
  if (dir->exec() == QDialog::Accepted)
    {
      QString cadfich=dir->ruta_actual();
      if (importarfichregul(cadfich))
          {
           QMessageBox::information( this, tr("FICHERO CON REGULARIZACIÓN"),
                        tr("El archivo seleccionado se ha importado correctamente"));
          }
          else
               {
                QMessageBox::warning( this, tr("FICHERO CON REGULARIZACIÓN"),
                        tr("ERROR: problemas importando archivo"));
               }
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

  dialogofich.setNameFilter("*.reg.xml");
  dialogofich.setDirectory(adapta(dirtrabajo));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR REGULARIZACIÓN"));
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
           QString cadfich=fileNames.at(0);
           if (importarfichregul(cadfich))
               {
                QMessageBox::information( this, tr("FICHERO CON REGULARIZACIÓN"),
                             tr("El archivo seleccionado se ha importado correctamente"));
               }
               else
                    {
                     QMessageBox::warning( this, tr("FICHERO CON REGULARIZACIÓN"),
                             tr("ERROR: problemas importando archivo"));
                    }
          }
     }
#endif
}

bool MainWindow::importarfichregul(QString qfichero)
{
  QDomDocument doc("REGULARIZ");
  QFile fichero(adapta(qfichero));

  if ( !fichero.open( QIODevice::ReadOnly ) ) return false;

  if (!doc.setContent(&fichero)) {
     fichero.close();
     return false;
    }

  // empezamos a procesar el xml
  QString codigo;
  //QString nombre;
  QDomElement docElem = doc.documentElement();

  QDomNode n = docElem.firstChild();
  while(!n.isNull()) {
     QDomElement e = n.toElement(); // try to convert the node to an element.
     if(!e.isNull()) {
              if (e.tagName()=="Code")
                 {
                  QDomNode nn=e.firstChild();
                  QDomText t = nn.toText();
                  QString desc;
                  if (basedatos::instancia()->existecodigoregularizacion(t.data(),&desc))
                    {
                     QMessageBox::warning( this, tr("FICHERO CON REGULARIZACIÓN"),
                             tr("ERROR: código a importar existente: ")+t.data());
                     return false;
                    }
                  codigo=t.data();
                 }
              if (e.tagName()=="Name")
                 {
                  QDomNode nn=e.firstChild();
                  QDomText t = nn.toText();
                  basedatos::instancia()->insertcab_regulariz (codigo,t.data());
                 }
              if (e.tagName()=="Phases")
                 {
                  QDomNode nn=e.firstChild();
                  int orden=1;
                  while (!nn.isNull()) // Phase
                       {
                        QString descripcion;
                        QString cuenta_deudora;
                        QString cuenta_acreedora;
                        QString cuentas;
                        QString condicion;
                        QDomElement ee = nn.toElement();
                        QDomNode nnn=ee.firstChild();
                        while (!nnn.isNull())
                              {
                               QDomElement eee = nnn.toElement();
                               if (eee.tagName()=="Description")
                                  {
                                    QDomNode nnnn=eee.firstChild();
                                    QDomText tt = nnnn.toText();
                                    descripcion=tt.data();
                                  }
                               if (eee.tagName()=="DebitAccount")
                                  {
                                    QDomNode nnnn=eee.firstChild();
                                    QDomText tt = nnnn.toText();
                                    cuenta_deudora=tt.data();
                                  }
                               if (eee.tagName()=="CreditAccount")
                                  {
                                    QDomNode nnnn=eee.firstChild();
                                    QDomText tt = nnnn.toText();
                                    cuenta_acreedora=tt.data();
                                  }
                               if (eee.tagName()=="Accounts")
                                  {
                                    QDomNode nnnn=eee.firstChild();
                                    QDomText tt = nnnn.toText();
                                    cuentas=tt.data();
                                  }
                               if (eee.tagName()=="Condition")
                                  {
                                    QDomNode nnnn=eee.firstChild();
                                    QDomText tt = nnnn.toText();
                                    condicion=tt.data();
                                  }
                               nnn=nnn.nextSibling();
                              }
                        // insertamos detalle
                        basedatos::instancia()->inserta_reg_sec(codigo,orden,descripcion,cuenta_deudora,
                                              cuenta_acreedora, cuentas, condicion, "");
                        orden++;
                        nn=nn.nextSibling();
                       }
                 }
     }
     n = n.nextSibling();
  }
  fichero.close();
  return true;
}


void MainWindow::consultamayor()
{
 consmayor *c = new consmayor(estilonumerico,!sindecimales,usuario);
 c->pasadatos( punterodiario->subcuentaactual(), punterodiario->fechapaseactual(), punterodiario->externoactual() );
 c->exec();
 delete(c);
 refrescardiario();
}


void MainWindow::consultamayor_permanente()
{
 // subcuentas *s = new subcuentas();
    if (c==NULL)
     {
        c = new consmayor(estilonumerico,!sindecimales,usuario);
        c->setModal(false);
     }
 c->show();
 // delete(s);
}


void MainWindow::imprimemayor()
{
 cuadimprimayor *c = new cuadimprimayor(estilonumerico,!sindecimales);
 c->exec();
 delete(c);
}


void MainWindow::genbss()
{
 if (b==NULL)
   {
    b = new bss(usuario);
    b->setModal(false);
   }
 b->show();
 // delete(b);
}


void MainWindow::acummes()
{
  acumuladosmes *a = new acumuladosmes();
  a->pasacodigocuenta(punterodiario->subcuentaactual(),punterodiario->fechapaseactual());
  a->exec();
  delete(a);
}


void MainWindow::consultaplan()
{
  consplan *c = new consplan();
  c->exec();
  delete(c);
}


void MainWindow::dresumen()
{
   diarioresumen *d=new diarioresumen();
   d->exec();
   delete(d);
}


void MainWindow::roper()
{
  relop *r = new relop();
  r->exec();
  delete(r);
}


void MainWindow::conciliacion()
  {
    pidecuentafechas *p=new pidecuentafechas();
    p->adjustSize();
    int resultado=p->exec();
    QString cuenta=p->cuenta();
    QDate fechainicial=p->fechainicial();
    QDate fechafinal=p->fechafinal();
    p->setWindowTitle(tr("Intervalo de cuentas y fechas para conciliación"));
    delete(p);

    if (resultado != QDialog::Accepted) return;

    concilia *c=new concilia(false,cuenta,fechainicial, fechafinal, usuario);

    c->exec();
    delete(c);

    refrescardiario();

  }


void MainWindow::conc_diferencias()
{
 dif_conc *d=new dif_conc();
 d->exec();
 delete(d);
}

void MainWindow::importac43()
{
   importa_conci(false);
}

void MainWindow::importacsv()
{
   importa_conci(true);
}


void MainWindow::importa_conci(bool csv)
{

    pidecuenta *p=new pidecuenta();

    int resultado=p->exec();
    QString cuenta=p->cuenta();
    if (csv) p->setWindowTitle(tr("Importar cuenta conciliación"));
      else p->setWindowTitle(tr("Importar cuaderno 43"));
    delete(p);

    if (resultado != QDialog::Accepted) return;

 if (cuenta.length()==0) return;

  if (QMessageBox::question(
            this, tr("Importar fichero para conciliación"),
            tr("Este proceso añade información para la conciliación\n"
               "de cuentas bancarias. Sólo se añadirán fechas no\n"
               "importadas anteriormente en la cuenta %1.\n"
               "¿ Desea continuar ?").arg(cuenta)) == QMessageBox::No )
                          return;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajo));
  if (dir->exec() == QDialog::Accepted)
    {
      QString cadfich=dir->ruta_actual();
      bool correcto=false;
      if (csv) correcto = importa_csv_cta_conci(cadfich,cuenta);
        else correcto = importarfichc43(cadfich,cuenta);
      if (correcto)
          {
           QMessageBox::information( this, tr("FICHERO CONCILIACIÓN"),
                        tr("El archivo seleccionado se ha importado correctamente"));
          }
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

  // dialogofich.setFilter("*.txt *.pln");
  dialogofich.setDirectory(adapta(dirtrabajo));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR CUADERNO 43"));
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
           QString cadfich=fileNames.at(0);
           bool correcto=false;
           if (csv) correcto = f_importa_csv_cta_conci(cadfich,cuenta);
             else correcto = f_importarfichc43(cadfich,cuenta);
           if (correcto)
               {
                QMessageBox::information( this, tr("FICHERO CONCILIACIÓN"),
                             tr("El archivo seleccionado se ha importado correctamente"));
               }
          }
     }
#endif
}



void MainWindow::borraimportac43()
{
    pidecuentafecha *p=new pidecuentafecha();
    int resultado=p->exec();
    QString cuenta=p->cuenta();
    QDate fecha=p->fecha();
    p->setWindowTitle(tr("Eliminación de pases importados, cuaderno 43"));
    delete(p);

    if (resultado != QDialog::Accepted) return;

    if (QMessageBox::question(
              this,
              tr("Eliminación de pases c43"),
              tr("Se van a borrar todos los pases de la cuenta importada\n"
                 "del cuaderno 43, a partir de la fecha %1,\n"
                 "la operación es irreversible ¿ Desea continuar ?").arg(fecha.toString("dd.MM.yyyy"))) == QMessageBox::Yes )
             {
                basedatos::instancia()->deleteCuenta_ext_concifechacuenta(fecha, cuenta);
                QMessageBox::information( this, tr("Borrar registros"),
                    tr("SE HA REALIZADO LA OPERACIÓN"));
                 }
}


void MainWindow::conciliacion_imp()
  {
    pidecuentafechas *p=new pidecuentafechas();
    p->adjustSize();
    int resultado=p->exec();
    QString cuenta=p->cuenta();
    QDate fechainicial=p->fechainicial();
    QDate fechafinal=p->fechafinal();
    p->setWindowTitle(tr("Intervalo de cuentas y fechas para conciliación"));
    delete(p);

    if (resultado != QDialog::Accepted) return;

    concilia *c=new concilia(true,cuenta,fechainicial, fechafinal, usuario);

    c->exec();
    delete(c);

    refrescardiario();

  }



void MainWindow::contabiliza_venci_c43()
{
    pidecuentafechas *p = new pidecuentafechas();
    p->muestra_check_proc43();
    p->muestra_intervalo();
    if (p->exec()!=QDialog::Accepted) return;

    QDate fechaini,fechafin;
    fechaini=p->fechainicial();
    fechafin=p->fechafinal();
    QString cuenta=p->cuenta();
    int margen=p->intervalo();

    bool proc_ambiguos=p->procesa_ambiguos();
    bool mostrar_tabla=p->mostrartablavenci();

    delete (p);

    if (basedatos::instancia()->fecha_periodo_bloqueado(fechaini) ||
        basedatos::instancia()->fecha_periodo_bloqueado(fechafin) )
     {
      QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR: La fechas corresponden "
                                                            "a periodos bloqueados"));
      return;
     }

    if (basedatos::instancia()->existe_bloqueo_cta(cuenta,fechaini) ||
            basedatos::instancia()->existe_bloqueo_cta(cuenta,fechafin))
    {
       QMessageBox::warning( this, tr("Vencimientos"),tr("ERROR: La cuenta %1 "
                                      "está bloqueada").arg(cuenta));
       return;
    }


    QList<int> numvenci;
    QList<double> importevenci;
    QList<QDate> fecha_venci;
    QList<bool> derecho;

    QList<int> registro_cta_conci;
    QList<QDate> fecha_conci;
    QList<double> debe_conci;
    QList<double> haber_conci;

    // cargamos datos de los vencimientos
    QSqlQuery q=basedatos::instancia()->datos_venci_fechas(cuenta,fechaini,fechafin, margen);
    if (q.isActive())
        while (q.next())
          {
            // select num, importe, fecha_vencimiento, derecho
            numvenci << q.value(0).toInt();
            importevenci << q.value(1).toDouble();
            fecha_venci << q.value(2).toDate();
            derecho << q.value(3).toBool();
          }

    // tendríamos que detectar vencimientos con igual importe y rango de fechas
    // para borrarlos y no procesarlos
    QList<int> vencis_a_borrar;
    for (int veces=0; veces<numvenci.count(); veces++)
        {
         bool encontrado=false;
         int buscarvenci=numvenci.at(veces);
         double importe=importevenci.at(veces);
         bool esderecho=derecho.at(veces);
         QDate fecha=fecha_venci.at(veces);
         QDate intervalo_ini, intervalo_fin;
         intervalo_ini=fecha.addDays(-margen);
         intervalo_fin=fecha.addDays(margen);
         for (int b=veces+1; b<numvenci.count(); b++)
            {
              if (floats_2d_iguales(importe,importevenci.at(b))
                      && fecha_venci.at(b)>=intervalo_ini && fecha_venci.at(b)<=intervalo_fin
                      && derecho.at(b)==esderecho)
                {
                 if (!encontrado) {encontrado=true; vencis_a_borrar << buscarvenci;}
                 vencis_a_borrar << numvenci.at(b);
                }
            }
        }

    // a eliminar ---------------------------------
    /*
    QString caden;
    for (int veces=0; veces< numvenci.count(); veces++)
       {
        QString cadnum;
        cadnum.setNum(numvenci.at(veces));
        caden+=cadnum;
        caden+="\n";
       }
    QMessageBox::warning( this, tr("proc43"), caden); */
    // --------------------------------------------

    if (!proc_ambiguos)
       {
     // bucle para borrar vencis_a_borrar
        for (int veces=0; veces<vencis_a_borrar.count(); veces++)
         {
         for (int b=0; b<numvenci.count(); b++)
            {
             if (numvenci.at(b)==vencis_a_borrar.at(veces))
                {
                  numvenci.removeAt(b);
                  importevenci.removeAt(b);
                  fecha_venci.removeAt(b);
                  derecho.removeAt(b);
                  break;
                }
            }
         }
       }
   //-----------------------------------------------------------------------------------------------------

    // a eliminar ---------------------------------
    /*
    caden.clear();
    for (int veces=0; veces< numvenci.count(); veces++)
       {
        QString cadnum;
        cadnum.setNum(numvenci.at(veces));
        caden+=cadnum;
        caden+="\n";
       }
    QMessageBox::warning( this, tr("proc43"), caden); */
    // --------------------------------------------


    q=basedatos::instancia()->datos_ext_conci_fechas(cuenta, fechaini, fechafin);
    if (q.isActive())
        while (q.next())
          {
            // numero, fecha, debe,haber
            registro_cta_conci << q.value(0).toInt();
            fecha_conci << q.value(1).toDate();
            debe_conci << q.value(2).toDouble();
            haber_conci << q.value(3).toDouble();
          }

    // recorremos vencimientos de inicio a fin
    QProgressDialog progreso("Procesando cuenta ...", 0, 0, numvenci.count());
    progreso.setMinimumDuration ( 0 );
    progreso.show();
    // progreso.setWindowTitle(tr("PROCESANDO..."));
    QApplication::processEvents();

    for (int veces=0; veces<numvenci.count(); veces++)
      {
        // comprobamos si el registro está en la cuenta del banco
        progreso.setValue(veces);
        QApplication::processEvents();
        for (int veces2=0; veces2<registro_cta_conci.count(); veces2++)
           {
            if ((derecho.at(veces) && floats_2d_iguales(haber_conci.at(veces2),importevenci.at(veces)))
                || (!derecho.at(veces) && floats_2d_iguales(debe_conci.at(veces2),importevenci.at(veces))))
              {
                // comparamos fechas
                QDate fecha=fecha_venci.at(veces);
                QDate intervalo_ini, intervalo_fin;
                intervalo_ini=fecha.addDays(-margen);
                intervalo_fin=fecha.addDays(margen);
                if (fecha_conci.at(veces2)>=intervalo_ini && fecha_conci.at(veces2)<=intervalo_fin)
                   {
                     // procesamos vencimiento
                    QString cadnum; cadnum.setNum(numvenci.at(veces));
                    if (!vencimientoprocesado(cadnum))
                      {
                        if (!basedatos::instancia()->fecha_periodo_bloqueado(fecha_conci.at(veces2)) &&
                           !basedatos::instancia()->existe_bloqueo_cta(cuenta,fecha_conci.at(veces2)))
                          {
                            procesavencimiento *procvenci=new procesavencimiento(usuario);
                            cadnum.setNum(numvenci.at(veces));
                            QString documento=basedatos::instancia()->documento_num_operacion_vencimiento(cadnum);
                            procvenci->pasa_documento(documento);
                            procvenci->cargadatos(cadnum);
                            procvenci->pasa_fecha_realizacion(fecha_conci.at(veces2));
                            procvenci->botonprocesarpulsado();
                            delete procvenci;
                          }
                      }
                    break;
                   }
              }
           }
      }

    if(mostrar_tabla)
    {
      listavenci *l = new listavenci(usuario);
      l->pasa_int_fechas_vencimiento(fechaini,fechafin,margen);
      l->pasa_cta_tesoreria(cuenta);
      l->refrescar();
      l->exec();
      delete(l);
    }

   refrescardiario();
}


void MainWindow::retencion()
{
  ret_irpf *r = new ret_irpf(estilonumerico);
  r->exec();
  delete(r);
}


void MainWindow::consulta_retenciones()
{
    consret *r = new consret(usuario);
    r->exec();
    delete(r);
}


void MainWindow::roper182()
{
  relop *r = new relop();
  r->prepara182();
  r->exec();
  delete(r);
}


void MainWindow::consvencipase()
{

    qlonglong nasiento;
    nasiento=punterodiario->asientoactual();
    if (nasiento==0)
       {
         QMessageBox::warning( this, tr("CONSULTA VENCIMIENTOS EN PASE DIARIO"),
         tr("Para consultar vencimientos debe de seleccionar un pase en el diario"));
         return;
       }
    QString cadnumasiento; cadnumasiento.setNum(nasiento);
    QString cadnumpase; cadnumpase.setNum(punterodiario->paseactual());
    if (espasevtoprocesado(cadnumpase))
       {
         QMessageBox::warning( this, tr("CONSULTA VENCIMIENTOS EN PASE DIARIO"),
         tr("El pase seleccionado es un cobro o pago ya efectuado"));
         return;
       }
    QString caddebe;
    caddebe.setNum(punterodiario->debe(),'f',2);
    QString cadhaber;
    cadhaber.setNum(punterodiario->haber(),'f',2);
    consultavencipase *losvenci=new consultavencipase(cadnumpase,
                  cadnumasiento,
                  caddebe,
                  cadhaber,
                  punterodiario->subcuentaactual(),
                  punterodiario->fechapaseactual(),
                  punterodiario->externoactual());
    losvenci->exec();
    delete(losvenci);

}


void MainWindow::procvencimientos()
{
  listavenci *l = new listavenci(usuario);
  if (cdomiciliaciones!="1") l->esconde_domiciliables();
  // l->esconde_domiciliables();
  l->exec();
  delete(l);
  refrescardiario();
}


void MainWindow::tipos_venci_a_cuentas()
{
  venci_ctas *v = new venci_ctas();
  v->exec();
  delete(v);
}


void MainWindow::ejecuta_tiposvenci()
{
  tiposvenci *t = new tiposvenci();
  t->exec();
  delete(t);
}


void MainWindow::vanul()
{
  vtosanulados *v = new vtosanulados();
  v->exec();
  delete(v);
}


void MainWindow::apunteivasoportado()
{

/* if (sobreescribeproxasiento())
    {
      QMessageBox::warning( this, tr("Tabla de asientos"),tr("ERROR: el número de próximo asiento en configuración ya existe. Debe de ser sustituido por uno más elevado\n"));
      return;
    } */

 tabla_asientos *t = new tabla_asientos(estilonumerico,!sindecimales,usuario);

 if (activa_msj_tabla) t->pasanocerrar(true);

 t->soportadoautonomo();
 // tablaasiento->exec();
 delete(t);
 refrescardiario();

}


void MainWindow::apunteivarepercutido()
{
/*  if (sobreescribeproxasiento())
    {
      QMessageBox::warning( this, tr("Tabla de asientos"),tr("ERROR: el número de próximo asiento en configuración ya existe. Debe de ser sustituido por uno más elevado\n"));
      return;
    } */

tabla_asientos *t = new tabla_asientos(estilonumerico,!sindecimales,usuario);

if (activa_msj_tabla) t->pasanocerrar(true);

t->repercutidoautonomo();
// tablaasiento->exec();
delete(t);
refrescardiario();

}


void MainWindow::qtiposiva()
{
  tiposiva *t = new tiposiva();
  t->exec();
  delete(t);
}


void MainWindow::cuentasiva_repercutido()
{
  tiposivacta *t = new tiposivacta(false);
  t->exec();
  delete(t);
}

void MainWindow::cuentasiva_soportado()
{
  tiposivacta *t = new tiposivacta(true);
  t->exec();
  delete(t);
}


void MainWindow::chequea_libro_facturas()
{
    pideejercicio *e = new pideejercicio();
    e->activa_cancelar();
    int resultado=e->exec();
    QString ejercicio=e->contenido();
    delete(e);
    if (resultado != QDialog::Accepted) return;

  cheq_libroiva *c = new cheq_libroiva(0,usuario);
  c->pasafechas(inicioejercicio(ejercicio),finejercicio(ejercicio));
  c->refrescar();
  c->exec();
  delete(c);
  refrescardiario();
}


void MainWindow::relacion_apuntes_no_libroiva()
{
    QProgressDialog progreso("Generando informe ...", 0, 0, 100);
    // progreso.setWindowModality(Qt::WindowModal);
    progreso.setMaximum(0);
    progreso.setValue(1);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(tr("PROCESANDO..."));
    QApplication::processEvents();

    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->cambia_titulo(tr("Apuntes en diario sin representación en libros de IVA"));
    QString contenido=basedatos::instancia()->apuntes_no_libroiva(true);
    contenido+="\n\n";
    contenido+=basedatos::instancia()->apuntes_no_libroiva(false);
    n->pasa_contenido(contenido);
    progreso.close();
    n->exec();
    delete(n);
}

void MainWindow::relacion_cuotas_descuadre()
{
    QProgressDialog progreso("Generando informe ...", 0, 0, 100);
    // progreso.setWindowModality(Qt::WindowModal);
    progreso.setMaximum(0);
    progreso.setValue(1);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(tr("PROCESANDO..."));
    QApplication::processEvents();

    notas *n = new notas(false);
    n->activa_modoconsulta();
    n->cambia_titulo(tr("Apuntes en diario con cuota descuadrada en libros de IVA"));
    QString contenido=basedatos::instancia()->apuntes_descuadre_libroiva();
    n->pasa_contenido(contenido);
    progreso.close();
    n->exec();
    delete(n);
}


void MainWindow::liqiva()
{
  liquidacioniva *l = new liquidacioniva(usuario);
  l->exec();
  delete(l);
  refrescardiario();
}


void MainWindow::relopiva()
{
  rellibiva *r = new rellibiva();
  r->exec();
  delete(r);
}


void MainWindow::librorecibidas()
{
  libro_recibidas *l = new libro_recibidas(usuario);
  l->exec();
  delete(l);
}

void MainWindow::libroemitidas()
{
  libro_emitidas *l = new libro_emitidas(usuario);
  l->exec();
  delete(l);
}


void MainWindow::m340()
{
 modelo340 *m = new modelo340();
 m->exec();
 delete(m);
}


void MainWindow::modelo347()
{
   conta347 *c = new conta347(usuario);
   c->exec();
   delete(c);
}


void MainWindow::ci1()
{
 ci *c = new ci();
 c->fijanivel(1);
 c->exec();
 delete(c);
}


void MainWindow::ci2()
{
 ci *c = new ci();
 c->fijanivel(2);
 c->exec();
 delete(c);
}


void MainWindow::ci3()
{
 ci *c = new ci();
 c->fijanivel(3);
 c->exec();
 delete(c);
}

void MainWindow::cinput()
{
 consinput *c = new consinput(usuario);
 c->pasadatos( punterodiario->ciactual(), punterodiario->fechapaseactual() );
 c->exec();
 delete(c);
}


void MainWindow::imputctas()
{
    if (!basedatos::instancia()->analitica_tabla()) return;
    buscasubcuenta_input *b = new buscasubcuenta_input();
    b->exec();
    delete(b);
}


void MainWindow::verinput_apunte()
{

    QString clave_ci;
    qlonglong qclave_ci=punterodiario->clave_ci_actual();
    QString cuenta=punterodiario->subcuentaactual();
    qlonglong paseactual=punterodiario->paseactual();
    QString cadpaseactual; cadpaseactual.setNum(paseactual);

    bool procesar_analitica=false;
    if (conanalitica() && (escuentadegasto(cuenta) || escuentadeingreso(cuenta)))
       procesar_analitica=true;
    if (conanalitica_parc() && codigo_en_analitica(cuenta))
       procesar_analitica=true;
    if (!procesar_analitica) return;

    if (!basedatos::instancia()->analitica_tabla()) return;

    if (qclave_ci==0) return;
    clave_ci.setNum(qclave_ci);

    asignainputdiario *a = new asignainputdiario(estilonumerico,!sindecimales);

    QSqlQuery query = basedatos::instancia()->carga_de_tabla_ci(clave_ci);
    // t->crea_tabla_ci(fila);
    a->pasacta(cuenta);
    if ( query.isActive() ) {
        double total=0;
        if (punterodiario->debe()>0.00001 ||
            punterodiario->debe()<-0.00001) total=punterodiario->debe();
        if (punterodiario->haber()>0.00001 ||
            punterodiario->haber()<-0.00001) total=punterodiario->haber();
        QString cadtotal;
        cadtotal.setNum(total,'f',2);
        // QMessageBox::information( this, tr("DIARIO"),
        //                       cadtotal);
        while ( query.next() ) {
            a->pasainputacion(query.value(0).toString(),
                              cadtotal,
                              query.value(1).toDouble()/total);
        }

        a->pasatotal(formateanumero(total,estilonumerico,!sindecimales));
        a->actualiza_total();
        a->activa_modoconsulta();
     }
    if (a->exec()==QDialog::Accepted)
       {
         // verificamos si hay edición activada
        if (a->en_edicion_ext())
          {
            if (!a->input_OK())
             {
              QMessageBox::warning( this, tr("Edición de imputaciones"),
               tr("ERROR: Las imputaciones no son correctas"));
              delete(a);
              return;
             }
            // borramos registros antiguos y creamos nuevos
            // tenemos que hacer una transacción
            QSqlDatabase contabilidad=QSqlDatabase::database();
            contabilidad.transaction();
            basedatos::instancia()->bloquea_configuracion();
            basedatos::instancia()->deletediarioci_clave(clave_ci);
            QString cadproxci="0";
            QStringList lista_ci;
            QStringList lista_total;
            lista_ci=a->lista_ci();
            lista_total=a->lista_total();
            qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
            cadproxci.setNum(proxnumci);
            basedatos::instancia()->cambia_clave_ci_apunte_diario(cadpaseactual, cadproxci);
            for (int veces=0; veces<lista_ci.count(); veces++)
                {
                  basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                                  lista_ci.at(veces),
                                                                  convapunto(lista_total.at(veces)));
                }
            basedatos::instancia()->incrementa_prox_num_ci();
            basedatos::instancia()->desbloquea_y_commit();
            refrescardiario();
          }
       }
    delete (a);
}


void MainWindow::cambiacidiario()
{
 cambiaci *c = new cambiaci();
 c->exec();
 delete(c);
}


void MainWindow::reasig_input()
{
  pidecuentafechas *p = new pidecuentafechas();
  p->muestra_debe_haber();
  int resultado=p->exec();
  if (resultado != QDialog::Accepted) return;
  QDate fecha_inicial=p->fechainicial();
  QDate fecha_final=p->fechafinal();
  bool esdebe=p->esdebe();
  QString cuenta=p->cuenta();
  delete(p);

  reasigna_input_diario *r = new reasigna_input_diario(estilonumerico,!sindecimales,usuario);
  r->pasa_datos(fecha_inicial, fecha_final, esdebe, cuenta);
  if (r->cuenta_sin_imputaciones())
     {
      QMessageBox::warning( this, tr("Reasignar Imputaciones"),
                            tr("No hay imputaciones para el intervalo de fechas"));

     }
    else
     r->exec();
  delete(r);
  refrescardiario();
}


void MainWindow::cambia_input_cta_diario()
{
  pidecuentafechas *p = new pidecuentafechas();
  // p->muestra_debe_haber();
  int resultado=p->exec();
  if (resultado != QDialog::Accepted) return;
  QDate fecha_inicial=p->fechainicial();
  QDate fecha_final=p->fechafinal();
  // bool esdebe=p->esdebe();
  QString cuenta=p->cuenta();
  delete(p);
  // ejercicio cerrado en fecha_inicial ?
  if (fechadejerciciocerrado(fecha_inicial))
  {
    QMessageBox::warning( this, tr("Cambiar Imputaciones"),
                          tr("ERROR: Ejercicio cerrado"));
    return;
  }
  QStringList cis;
  QList<double> imputaciones;
  QSqlQuery q=basedatos::instancia()->ci_input(cuenta);
  if (q.isActive())
      while (q.next())
       {
        cis << q.value(0).toString();
        imputaciones << q.value(1).toDouble();
       }
  if (cis.isEmpty())
    {
      QMessageBox::warning( this, tr("Cambiar Imputaciones"),
                            tr("ERROR: No hay imputaciones predefinidas para la cuenta seleccionada"));
      return;
    }

  int registros=basedatos::instancia()->registros_select_apuntes_analitica_diario (cuenta,
                                                                                   fecha_inicial,
                                                                                   fecha_final);
  QProgressDialog progreso("Actualizando imputaciones ...", 0, 0, registros);
  // progreso.setWindowModality(Qt::WindowModal);
  progreso.setMaximum(registros);
  // progreso.setValue(1);
  progreso.setMinimumDuration ( 0 );
  progreso.setWindowTitle(tr("PROCESANDO..."));
  progreso.setLabelText("Sustituyendo imputaciones...");
  QApplication::processEvents();
  int numreg=0;

  q=basedatos::instancia()->select_apuntes_analitica_diario (cuenta,
                                                             fecha_inicial,
                                                             fecha_final);
  if (q.isActive())
      while (q.next())
        {
          numreg++;
          progreso.setValue(numreg);
          QApplication::processEvents();
          // primero borramos imputaciones anteriores
          // asiento, pase, clave_ci, debe+haber
          QString cad_apunte=q.value(1).toString();
          QString clave_ci=q.value(2).toString();
          double valor=q.value(3).toDouble();
          if (valor<0) valor=-1*valor;
          basedatos::instancia()->deletediarioci_clave(clave_ci);

          basedatos::instancia()->bloquea_para_tabla_asientos();

          qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
          QString cadproxci;
          cadproxci.setNum(proxnumci);
          // actualizamos campo clave_ci en diario
          // para el apunte en cuestión
          basedatos::instancia()->asigna_clave_ci_apunte(cad_apunte, cadproxci);
          for (int veces=0; veces<cis.count(); veces++)
             {
              QString cadimporte;
              cadimporte.setNum(redond(valor*imputaciones.at(veces),4),'f',4);
              basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                        cis.at(veces),
                                                        cadimporte);
             }
          basedatos::instancia()->incrementa_prox_num_ci();

          basedatos::instancia()->desbloquea_y_commit();

         }

  QMessageBox::information( this, tr("Cambiar Imputaciones"),
                        tr("Operación concluida"));

}


void MainWindow::gestestados()
{
  selecestado *selec = new selecestado(usuario);
  selec->exec();
  delete(selec);
  actualizadockestados();
}


void MainWindow::actualizadockestados()
{
// estadosList->disconnect(SIGNAL(itemActivated ( QListWidgetItem * item ) ));
 estadosList->clear();
 QSqlQuery query = basedatos::instancia()->selectTitulocabeceraestadosordertitulo();
 if ( query.isActive() ) {
          while ( query.next() )
                   estadosList->addItem (query.value(0).toString()) ;
      }
 if (estadosList->count()>0) estadosList->setCurrentRow(0);
 // connect(estadosList, SIGNAL(itemActivated ( QListWidgetItem * item ) ), this,
 //        SLOT(cal_estado()));
}


void MainWindow::liberacalcmedios()
{

  desbloqueasaldosmedios();
  QMessageBox::information( this, tr("KEME Contabilidad"),
  tr("SE HA LIBERADO EL CÁLCULO DE SALDOS MEDIOS"));

}


void MainWindow::ecpn_pymes()
{
  pideejercicio *e = new pideejercicio();
  e->activa_cancelar();
  int resultado=e->exec();
  QString ejercicio=e->contenido();
  delete(e);
  if (resultado != QDialog::Accepted) return;

  patrimonio *p = new patrimonio();
  p->pasa_ejercicio(ejercicio);
  p->exec();
  delete(p);
}


void MainWindow::concepto_ecpn_pymes()
{
    qlonglong nasiento;
    nasiento=punterodiario->asientoactual();
    if (nasiento==0)
       {
         QMessageBox::warning( this, tr("CONSULTA VENCIMIENTOS EN PASE DIARIO"),
         tr("Para realizar esta operación de seleccionar un apunte en el diario"));
         return;
       }
  QString cadnumpase; cadnumpase.setNum(punterodiario->paseactual());
  QString codigo_var_evp=basedatos::instancia()->select_codigo_var_evpn_pymes_diariopase (cadnumpase);
  QString debe=formateanumero(punterodiario->debe(),estilonumerico,!sindecimales);
  QString haber=formateanumero(punterodiario->haber(),estilonumerico,!sindecimales);
  concepto_evpn *c = new concepto_evpn(cadnumpase,punterodiario->subcuentaactual(),
                                       punterodiario->conceptoactual(),
                                       debe, haber,
                                       codigo_var_evp);
  c->exec();
  delete(c);
}

void MainWindow::qcolumnas_ecpn_pymes()
{
  columnas_ecpn_pymes *c = new columnas_ecpn_pymes();
  c->exec();
  delete(c);
}

void MainWindow::genxbrl()
{
  xbrlpymes *x = new xbrlpymes();
  x->exec();
  delete(x);
}

void MainWindow::genis()
{
  xml_is_pymes *x = new xml_is_pymes();
  x->exec();
  delete(x);
}


void MainWindow::procmemoria()
{
  procesamemoria *p = new procesamemoria();
  p->exec();
  delete(p);
}


void MainWindow::presupuestos()
{
    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("SELECCIÓN EJERCICIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();
    delete(ej);
    presupuesto_ed *p = new presupuesto_ed(true);
    p->pasaejercicio(elejercicio);
    p->ir_fila_pre(fila_presupuesto_tabla);
    p->exec();
    fila_presupuesto_tabla=p->fila_tabla_pre();
    delete(p);
}

void MainWindow::presupuestoscta()
{
    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("SELECCIÓN EJERCICIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();
    delete(ej);
    presupuesto_ed *p = new presupuesto_ed(false);
    p->pasaejercicio(elejercicio);
    p->ir_fila_pre(fila_presupuesto_tabla);
    p->exec();
    fila_presupuesto_tabla=p->fila_tabla_pre();
    delete(p);
}

void MainWindow::desv()
{
    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("SELECCIÓN EJERCICIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();
    delete(ej);
    desviaciones *d = new desviaciones(true,elejercicio);
    // d->pasaejercicio(elejercicio);
    d->exec();
    delete(d);
}

void MainWindow::desvctas()
{
    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("SELECCIÓN EJERCICIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();
    delete(ej);
    desviaciones *d = new desviaciones(false,elejercicio);
    // d->pasaejercicio(elejercicio);
    d->exec();
    delete(d);
}


void MainWindow::plamort()
{
 planamortizaciones *a = new planamortizaciones();
 a->exec();
 delete(a);
}

void MainWindow::asamort()
{
 genamort *g = new genamort(usuario);
 g->exec();
 delete(g);
 refrescardiario();
}

void MainWindow::difbi()
{
 amortbi *a = new amortbi();
 a->exec();
 delete(a);
}


void MainWindow::hamort()
{
  hcoamort *h = new hcoamort();
  h->exec();
  delete(h);
}

void MainWindow::ineto()
{
  inmovneto *i = new inmovneto();
  i->exec();
  delete(i);
}


void MainWindow::importasientos()
{
  QString cadfich;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajo));
  if (dir->exec() == QDialog::Accepted)
    {
      cadfich=dir->ruta_actual();
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
  filtros << tr("Archivos de texto csv (*.txt *.csv)");
  filtros << tr("Archivos de auditoría (*.xml *.XML)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR"));
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
           cadfich=fileNames.at(0);
          }
     }
#endif

  if (cadfich.isEmpty()) return;
  if (importarfichasientos(cadfich))
      {
       refrescardiario();

       QMessageBox::information( this, tr("DIARIO"),
                    tr("El archivo seleccionado se ha importado correctamente"));
       punterodiario->irfinal();
      }


}


void MainWindow::imp_cplus()
{
  import_cplus *i = new import_cplus();
  i->exec();
  delete(i);
  refrescardiario();
}

void MainWindow::marcarcerrado()
{
  marcacerrado *m = new marcacerrado();
  m->exec();
  delete(m);
}


void MainWindow::importafich()
{
    creaempresa *c = new creaempresa;
    if (basedatos::instancia()->gestiondeusuarios())
       c->pasa_usuario(usuario);
    c->importacion();
    delete(c);
    refrescardiario();
}

void MainWindow::asignardiario()
{
    QString elasiento;
    qlonglong nasiento=punterodiario->asientoactual();
    if (nasiento>0) elasiento.setNum(nasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("ASIGNAR DIARIO"),
         tr("Para asignar diario a un asiento debe de seleccionar un pase del mismo en el diario"));
         return;
       }
  QString ejercicio=ejerciciodelafecha(punterodiario->fechapaseactual());
  asignadiario *a = new asignadiario();
  a->pasanumasiento(elasiento,ejercicio);
  a->exec();
  delete(a);
  refrescardiario();
}

void MainWindow::librosagr()
{
  libros *l = new libros();
  l->exec();
  delete(l);
}


void MainWindow::consultatex()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajo));
  dir->filtrar("*.tex");
  if (dir->exec() == QDialog::Accepted)
    {
      QString cadfich=dir->ruta_actual();
      if (cadfich.endsWith(".tex") || cadfich.endsWith(".TEX"))
       {
       int valor=consultalatex2fichabs(cadfich);
       if (valor==1)
        QMessageBox::warning( this, tr("Visualizar TEX"),tr("PROBLEMAS al llamar a Latex"));
       if (valor==2)
        QMessageBox::warning( this, tr("Visualizar TEX"),
                        tr("PROBLEMAS al llamar a ")+visordvi);
       }
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
  filtros << tr("Archivos tex (*.tex)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo));
  dialogofich.setWindowTitle(tr("VISUALIZAR ARCHIVO TEX"));
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
           QString cadfich=fileNames.at(0);
           int valor=consultalatex2fichabs(cadfich);
           if (valor==1)
             QMessageBox::warning( this, tr("Visualizar TEX"),tr("PROBLEMAS al llamar a Latex"));
           if (valor==2)
             QMessageBox::warning( this, tr("Visualizar TEX"),
                             tr("PROBLEMAS al llamar a ")+visordvi);
          }
     }
#endif
}


void MainWindow::generarpdf()
{

  QString qruta_libros=rutalibros();
  QString qdir;
  qdir= qruta_libros.isEmpty() ? dirtrabajo : qruta_libros;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(qdir);
  dir->filtrar("*.tex");
  if (dir->exec() == QDialog::Accepted)
    {
      QProgressDialog progreso("Generando PDF ...", 0, 0, 0);
      progreso.setMinimumDuration ( 0 );
      progreso.show();
      QApplication::processEvents();
      QString cadfich=dir->ruta_actual();
      QApplication::processEvents();
      int valor=generapdffichabs(cadfich);
      // consultalatex2fichabs(cadfich);

      if (cadfich.endsWith(".tex") || cadfich.endsWith(".TEX"))
       {
        if (valor==1)
          QMessageBox::warning( this, tr("Generar PDF"),tr("PROBLEMAS al llamar a Latex"));
        if (valor==2)
          QMessageBox::warning( this, tr("Generar PDF"),
                        tr("PROBLEMAS al llamar a ")+"dvipdf");
        if (valor==0)
          QMessageBox::information( this, tr("Generar PDF"),
                        tr("Se ha generado el archivo PDF"));
      }
    }
  delete(dir);
#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFiles);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Archivos tex (*.tex)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(qdir));
  dialogofich.setWindowTitle(tr("GENERAR PDF"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      for (int veces=0;veces<fileNames.count();veces++)
        if (fileNames.at(veces).length()>0)
          {
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(veces));
           QString cadfich=fileNames.at(veces);

           QProgressDialog progreso("Generando archivo pdf ...", 0, 0, 100);
           // progreso.setWindowModality(Qt::WindowModal);
           progreso.setMaximum(0);
           progreso.setValue(1);
           progreso.setMinimumDuration ( 0 );
           progreso.setWindowTitle(tr("PROCESANDO..."));
           QApplication::processEvents();


           int valor=generapdffichabs(cadfich);
           progreso.hide();
           if (valor==1)
             QMessageBox::warning( this, tr("Generar PDF"),tr("PROBLEMAS al llamar a Latex"));
           if (valor==2)
             QMessageBox::warning( this, tr("Generar PDF"),
                             tr("PROBLEMAS al llamar a ")+"dvipdf");
           if (valor==0)
             QMessageBox::information( this, tr("Generar PDF"),
                             tr("Se ha generado el archivo PDF"));
          }
     }
#endif
}


void MainWindow::seguridad()
{
    if (punterodiario!=NULL)
    {
     delete(punterodiario);
     delete(treeWidget);
     // modeloarbol.borraarbol();
     // modeloarbol
     delete(estadosList);
     delete(dockAutomaticos);
     delete(dockEstados);
     punterodiario=NULL;
     treeWidget=NULL;
     estadosList=NULL;
     dockAutomaticos=NULL;
    }

   copiaseg *c = new copiaseg();
   c->exec();
   delete(c);

   actudatosprincipal();

}


void MainWindow::consolid_sinc()
{
 consolidacion_sinc *c = new consolidacion_sinc();
 c->exec();
 delete(c);
 refrescardiario();

}


void MainWindow::renumera()
{
  if (QMessageBox::question(this,tr("RENUMERAR"),
                tr("ANTES DE RENUMERAR ES CONVENIENTE REALIZAR\n"
                   "COPIA DE SEGURIDAD. LA BASE DE DATOS NO DEBE\n"
                   "DE SER ACCEDIDA POR NINGÚN PUESTO MIENTRAS SE\n"
                   "REALIZA EL PROCESO\n"
                   " ¿ Continuar ?")) == QMessageBox::No ) return;

  QProgressDialog progreso("Borrando papelera ...", 0, 0, 0);
  progreso.setWindowTitle(tr("PROCESANDO..."));
  progreso.setMaximum(0);
  // progreso.setValue(30);
  progreso.setMinimumDuration ( 0 );
  // progreso.forceShow();
  progreso.show();
  QApplication::processEvents();

  basedatos::instancia()->borrar_papelera();

  progreso.close();

  renumerar *r = new renumerar();
  r->exec();
  delete(r);
  refrescardiario();
}

void MainWindow::intercambiaas()
{
  intercambianum *i = new intercambianum();
  i->exec();
  delete(i);
  refrescardiario();
}

void MainWindow::borraint()
{
  borraintasientos *b = new borraintasientos();
  b->exec();
  delete(b);
  refrescardiario();
}


void MainWindow::cambiacuen()
{
  cambiacuenta *c = new cambiacuenta();
  c->exec();
  delete(c);
  refrescardiario();
}

void MainWindow::cambiacuenpase()
{
    QString elasiento;
    qlonglong nasiento=punterodiario->asientoactual();
    if (nasiento>0) elasiento.setNum(nasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("CAMBIAR CUENTA EN PASE"),
         tr("Para cambiar una subcuenta debe de seleccionar un pase en el diario"));
         return;
       }


  cambiacuentapase *c = new cambiacuentapase();

  qlonglong pase=punterodiario->paseactual();
  QString qapunte; qapunte.setNum(pase);
  QString codigo=punterodiario->subcuentaactual();
  QDate qfecha=punterodiario->fechapaseactual();
  double debe=punterodiario->debe();
  QString qdebe; qdebe.setNum(debe);
  double haber=punterodiario->haber();
  QString qhaber; qhaber.setNum(haber);

  c->pasainfo( qapunte, codigo, qfecha, qdebe, qhaber );
  c->exec();
  delete(c);
  refrescardiario();
}


void MainWindow::cambiaciapunte()
{
    QString elasiento;
    qlonglong nasiento=punterodiario->asientoactual();
    if (nasiento>0) elasiento.setNum(nasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("CAMBIAR CI EN APUNTE"),
         tr("Para editar un CI debe de seleccionar un apunte en el diario"));
         return;
       }

  qlonglong pase=punterodiario->paseactual();
  QString qapunte; qapunte.setNum(pase);
  QString codigo=punterodiario->subcuentaactual();
  QDate qfecha=punterodiario->fechapaseactual();
  QString ci= punterodiario->ciactual();

  // evaluar si la cuenta tiene ci editable
  if (conanalitica() &&
    (!escuentadegasto(codigo) && !escuentadeingreso(codigo)))
  {
    QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                             tr("ERROR: La cuenta de origen no es de ingreso o gasto"));
    return;
  }

  if (conanalitica_parc() && !codigo_en_analitica(codigo))
  {
    QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                             tr("ERROR: La cuenta de origen no es correcta para imputaciones"));
    return;
  }


  cambiacipase *c = new cambiacipase();

  // QString qapunte, QString cuenta, QString ci, QDate qfecha
  c->pasainfo( qapunte, codigo,ci, qfecha);
  c->exec();
  delete(c);
  refrescardiario();
}


void MainWindow::editafechaasien()
{
    QString elasiento;
    qlonglong nasiento=punterodiario->asientoactual();
    if (nasiento>0) elasiento.setNum(nasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("CAMBIAR FECHA EN PASE"),
         tr("Para cambiar una fecha debe de seleccionar un pase en el diario"));
         return;
       }
  QDate qfecha=punterodiario->fechapaseactual();

  editafechaasiento *e = new editafechaasiento();

  e->pasadatos( elasiento, qfecha );

  e->exec();
  delete(e);
  refrescardiario();
}


void MainWindow::edcondoc()
{
  qlonglong pase=punterodiario->paseactual();
  QString qapunte;
  if (pase>0) qapunte.setNum(pase);
  if (qapunte=="")
       {
         QMessageBox::warning( this, tr("EDITAR CONCEPTO / DOCUMENTO"),
         tr("Para cambiar datos debe de seleccionar un pase en el diario"));
         return;
       }
  QDate qfecha=punterodiario->fechapaseactual();

  edit_conc_doc *e = new edit_conc_doc();
  e->pasadatos( qapunte, qfecha, punterodiario->conceptoactual(), punterodiario->documentoactual() );
  e->exec();
  delete(e);
  refrescardiario();
}

/*
void MainWindow::carganuevoplan()
{
    QMessageBox::information( this, tr("NUEVO PLAN CONTABLE"),
                           tr("ATENCIÓN: El nuevo plan se ha de importar sin cuentas auxiliares"));

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajo));
  dir->filtrar("*.pln");
  if (dir->exec() == QDialog::Accepted)
    {
      QString cadfich=dir->ruta_actual();
      bool importado=importanuevoplan(cadfich);
      // progress.cancel();
      if (importado)
          {
           QMessageBox::information( this, tr("NUEVO PLAN CONTABLE"),
                        tr("El plan nuevo se ha importado correctamente"));
          }
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
  filtros << tr("Archivos de texto (*.pln)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO CON NUEVO PLAN"));
  QStringList fileNames;

  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           // QProgressDialog progress("Importando nuevo plan contable","Cancelar",0,100,this);
           // progress.setLabelText("Importando nuevo plan contable");
           // progress.setWindowModality(Qt::WindowModal);
           // progress.setMinimum(0);
           // progress.setMaximum(100);
           // progress.setValue(10);
           // processEvents(ExcludeUserInputEvents);
           // progress.setValue(20);
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
           QString cadfich=fileNames.at(0);
           bool importado=importanuevoplan(cadfich);
           // progress.cancel();
           if (importado)
               {
                QMessageBox::information( this, tr("NUEVO PLAN CONTABLE"),
                             tr("El plan nuevo se ha importado correctamente"));
               }
          }
     }
#endif
}

void MainWindow::ednuevoplan()
{
 editanuevoplan *e = new editanuevoplan();
 e->exec();
 delete(e);
}


void MainWindow::equivalenciasplan()
{
 equivalencias *e = new equivalencias();
 e->exec();
 delete(e);
 refrescardiario();
}

void MainWindow::susticuenta()
{
 sustcuenta *s = new sustcuenta();
 s->exec();
 delete(s);
 refrescardiario();
}


void MainWindow::borranuevoplan()
{

  if (QMessageBox::question(
            this,
            tr("Nuevo plan contable"),
            tr("¿ Desea borrar el contenido de la tabla del nuevo plan contable ?"),
            tr("&Sí"), tr("&No"),
            QString(), 0, 1 ) ==0 )
                {
                  borraplannuevo();
                  QMessageBox::information( this, tr("NUEVO PLAN CONTABLE"),
                        tr("El nuevo plan se ha eliminado"));

                }

}
*/

void MainWindow::verificaBD()
{
    if (QMessageBox::question(this,
                    tr("Verificar base de datos"),
                    tr("El proceso puede ser prolongado para bases de datos grandes ¿ desea continuar ?")) == QMessageBox::No )
                  return ;
    bool errenlace=false;
    QProgressDialog progress(tr("Verificando..."), tr("Abortar"), 0, 7, this);

    progress.setMinimumDuration ( 100 );
    progress.setValue(1);
    QApplication::processEvents();
    QSqlQuery query = basedatos::instancia()->pase_bi_libroiva();
    if ( query.isActive() ) {
        QString cadena;
        while ( query.next() ) {
            // la posición 0 es el pase
            if (!basedatos::instancia()->existe_apunte (query.value(0).toString()))
              {
                if (cadena.isEmpty()) cadena=tr("ERROR, no existe en el diario el apunte del libro de facturas:");
                errenlace=true;
                cadena+=tr("\nApunte: ");
                cadena+=query.value(0).toString();
                cadena+=tr(" - Fecha factura: ");
                cadena+=query.value(3).toDate().toString("dd-MM-yyyy");
                cadena+= query.value(4).toBool() ? tr(" - facturas recibidas") : tr(" - facturas emitidas");
              }

        }
        if (!cadena.isEmpty())
           {
            QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),cadena);


           }
    }

    progress.setValue(2);
    QApplication::processEvents();
    if (progress.wasCanceled())
            return;

    // repetimos para comprobar enlace con cuentas de iva en diario
    query = basedatos::instancia()->pase_bi_libroiva();
        if ( query.isActive() ) {

            QString cadena;
            while ( query.next() ) {
            if (!(query.value(2).toDouble()<0.0001 && query.value(2).toDouble()>-0.0001) && // cuota IVA
                !escuentadeivasoportado(basedatos::instancia()->cuenta_pase(query.value(0).toString())) &&
                !escuentadeivarepercutido(basedatos::instancia()->cuenta_pase(query.value(0).toString())))
              {
                errenlace=true;
                QString asiento = basedatos::instancia()->selectAsientodiariopase (query.value(0).toString());
                QDate fecha = basedatos::instancia()->select_fecha_diariopase (query.value(0).toString());
                if (cadena.isEmpty()) cadena=tr("ERROR, la cuenta en el diario enlazada con el libro de facturas no es de IVA:");
                cadena+=tr("\nAsiento: ");
                cadena+=asiento;
                cadena+= tr(" - fecha: ");
                cadena+=fecha.toString("dd-MM-yyyy");
                cadena+= tr(" - apunte: ");
                cadena+=query.value(0).toString();
              }
        }
        if (!cadena.isEmpty())
               {
                QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),cadena);


               }
    }

    progress.setValue(3);
    QApplication::processEvents();
    if (progress.wasCanceled())
            return;

    QSqlQuery query2 = basedatos::instancia()->ctaord_pase_vtos();
    if ( query2.isActive() ) {

        QString cadena;
        while ( query2.next() ) {
                if (!basedatos::instancia()->existe_apunte (query2.value(1).toString()))
                  {
                    if (cadena.isEmpty()) cadena=tr("ERROR, no existe el apunte de vencimientos en el diario:");
                    errenlace=true;
                    cadena+=tr("\nApunte: ");
                    cadena+=query2.value(1).toString();
                    cadena+=tr(" - Fecha operación: ");
                    cadena+=query2.value(2).toDate().toString("dd-MM-yyyy");
                  }
              }
        if (!cadena.isEmpty())
               {
                QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),cadena);

               }
        }

    progress.setValue(4);
    QApplication::processEvents();
    if (progress.wasCanceled())
            return;

    query2 = basedatos::instancia()->ctaord_pase_vtos();
        if ( query2.isActive() ) {

            QString cadena;
            while ( query2.next() ) {
                if (query2.value(0).toString()!=basedatos::instancia()->cuenta_pase(query2.value(1).toString()))
                  {
                   if (cadena.isEmpty()) cadena=tr("ERROR, enlace vencimientos con el diario:");
                   errenlace=true;
                   QString asiento = basedatos::instancia()->selectAsientodiariopase (query2.value(1).toString());
                   QDate fecha = basedatos::instancia()->select_fecha_diariopase (query2.value(1).toString());
                   cadena+="\nAsiento: ";
                   cadena+=asiento;
                   cadena+= tr(" - fecha: ");
                   cadena+=fecha.toString("dd-MM-yyyy");
                   cadena+= tr(" - apunte: ");
                   cadena+=query2.value(1).toString();
              }
        }
        if (!cadena.isEmpty())
              {
               QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),cadena);

              }
    }

  progress.setValue(5);
  QApplication::processEvents();
  if (progress.wasCanceled())
          return;

    query2 = basedatos::instancia()->descuadres_diario();
    if ( query2.isActive() ) {
        QString cadena;
        while ( query2.next() ) {
          errenlace=true;
          if (cadena.isEmpty()) cadena=tr("ERROR, asientos descuadrados:");
          QString asiento= query2.value(0).toString();
          QDate fecha = query2.value(1).toDate();
          cadena+=tr("\nAsiento: ");
          cadena+=asiento;
          cadena+=tr(" - fecha: ");
          cadena+=fecha.toString("dd-MM-yyyy");
        }
        if (!cadena.isEmpty())
              {
               QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),cadena);
               return;
              }
    }

    progress.setValue(6);
    QApplication::processEvents();
    if (progress.wasCanceled())
            return;

    QStringList aux_cuentas=basedatos::instancia()->listacuentas_aux();
    QStringList codigos_plan=basedatos::instancia()->lista_codigos_plan();

    // verificamos que todas tienen asignadas cuentas de 3 dígitos
    QString cadena;
    for (int i=0; i<aux_cuentas.count(); i++) {
        if (!codigos_plan.contains(aux_cuentas.at(i).left(3))) {
            errenlace=true;
            cadena+=tr("ERROR: La cuenta auxuliar ");
            cadena+=aux_cuentas.at(i);
            cadena+=" no está asociada a cuenta de tres dígitos\n";
        }
    }

    if (!cadena.isEmpty()) {
        QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),cadena);
        return;
    }

    progress.setValue(7);
    QApplication::processEvents();

    if (!errenlace)
      QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),
                           "LA BASE DE DATOS PARECE CORRECTA");
     else
        QMessageBox::information( this, tr("VERIFICA BASE DE DATOS"),
                             "EL PROCESO HA CONCLUIDO");

}

void MainWindow::actusaldos()
{
  actualizasaldos();
  QMessageBox::information( this, tr("ACTUALIZACIÓN DE SALDOS"),
  tr("Operación realizada correctamente"));
}


void MainWindow::infoconex()
{
   infobd *i = new infobd();
   i->exec();
   delete(i);
}

void MainWindow::acuerdolicencia()
{
   licencia *lic=new licencia();
   lic->exec();
   delete(lic);

}


void MainWindow::acuerdolicenciaesp()
{
   licencia *lic=new licencia();
   lic->lic_spanish();
   lic->exec();
   delete(lic);

}

void MainWindow::domicilia()
{
   domiciliaciones *d = new domiciliaciones(usuario);
   d->exec();
   delete(d);
   refrescardiario();
}


void MainWindow::ejecuta_automatico()
{
    if (basedatos::instancia()->gestiondeusuarios() && ! privilegios[ejecutar_automaticos]) return;
    if (treeWidget->currentItem()!=0)
      {
        QString codigo=treeWidget->currentItem()->text(0);
        if( basedatos::instancia()->asientomodeloencab_as_modelo(codigo))
           {
            // ejecutamos
            execasientomodelo *e = new execasientomodelo(usuario);
            e->cargadatos(codigo);
            e->exec();
            delete(e);
            refrescardiario();
           }
      }

}


void MainWindow::cal_estado(QListWidgetItem *item)
{
 if (basedatos::instancia()->gestiondeusuarios() && !privilegios[ejecutar_estados]) return;
 calcestado *c = new calcestado();
 c->cargaestado(item->text());
 c->exec();
 delete(c);
}


void MainWindow::m349()
{
    modelo349 *m = new modelo349(usuario);
    m->exec();
    delete(m);
}

void MainWindow::informacion_venci()
{
  infovenci *i = new infovenci();
  i->exec();
  delete(i);
}

void MainWindow::ctas_inversion_sp()
{
  ctas_iva_inversion *c = new ctas_iva_inversion();
  c->exec();
  delete(c);
}

void MainWindow::edformas_pago()
{
    formas_pago *f = new formas_pago();
    f->exec();
    delete(f);
}

void MainWindow::import_ctas_datos()
{
 QString cadfich;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      cadfich=dir->ruta_actual();
    } else return;
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
    filtros << tr("Archivos de texto con formato especial (*.*)");
    dialogofich.setNameFilters(filtros);
    dialogofich.setDirectory(adapta(dirtrabajobd()));
    dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA IMPORTAR"));
    // dialogofich.exec();
    //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
    //                                              dirtrabajo,
    //                                              tr("Ficheros de texto (*.txt)"));
    QStringList fileNames;
    if (dialogofich.exec() == QDialog::Accepted)
       {
        fileNames = dialogofich.selectedFiles();
        if (fileNames.at(0).length()>0)
            {
             // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
             cadfich=fileNames.at(0);
            }
       }
       else return;
#endif

   /* if (QMessageBox::question(this,
          tr("Procesar archivo"),
          tr("Se va a importar el archivo '%1' "
         "¿ Desea continuar ?")
          .arg(cadfich ),
          tr("&Sí"), tr("&No"),
          QString(), 0, 1 ))
              return;*/

    // pide nombre
    pidenombre *p = new pidenombre();
    p->asignanombreventana(tr("Importar datos"));
    p->asignaetiqueta(tr("Cuenta principal:"));
    if (p->exec()!=QDialog::Accepted) return;
    QString cuenta=p->contenido();
    delete(p);

    QString descrip;
    if (!existecodigoplan(cuenta,&descrip))
      {
        QMessageBox::warning(this, tr("KEME-Contabilidad"),tr("ERROR: no existe el código de cuenta '%1'").arg(cuenta));
        return;
      }

    // realizamos la importación

    // averiguamos última número de cuenta auxiliar de la cuenta suministrada
    int num_aux=basedatos::instancia()->maxcodcta(cuenta);

    QFile fichero(cadfich);

    if ( fichero.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea;
          while ( !stream.atEnd() )
            {
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              QString datos[15];
              enum columnas {cuenta_aux,nombre, ncomercial, nif, domicilio, poblacion,
                             cp, provincia, pais, ccc, iban, bic, tfno, email, observaciones };
              for (int veces=0; veces<13; veces++)
                  {
                   datos[veces]=linea.section('\t',veces,veces);
                  }
              num_aux++;
              QString cadnum;
              cadnum.setNum(num_aux);
              QString auxiliar;

//              if (cod_longitud_variable())
//                 {
//                  auxiliar=cuenta+cadnum;
//                 }
//                 else
//                     {
//                      int anchoctas=anchocuentas();
//                      auxiliar=cuenta+"."+cadnum;
//                      if (auxiliar.length()>anchoctas)
//                        {
//                          QMessageBox::warning(this, tr("KEME-Contabilidad"),
//                                               tr("ERROR en longitud de cuenta '%1'").arg(auxiliar));
//                          return;
//                        }
//                      auxiliar=expandepunto(auxiliar,anchoctas);
//                     }
             auxiliar=datos[cuenta_aux];
             if (!existesubcuenta(auxiliar))
                basedatos::instancia()->insertPlancontable(auxiliar, datos[nombre], true);
             basedatos::instancia()->insert_datossubcuenta_import(auxiliar, datos[nombre],
                                          datos[ncomercial], datos[nif],
                                          "", datos[domicilio],
                                          datos[poblacion], datos[cp],
                                          datos[provincia], datos[tfno],
                                          datos[pais], "",
                                          datos[email], datos[observaciones],
                                          datos[ccc],datos[iban],datos[bic], "",
                                          "0",
                                          "",
                                          QDate(2009,10,31), "ES", "1");

           }
          fichero.close();
        } else return;
    QMessageBox::information(this, tr("KEME-Contabilidad"),
                         tr("El archivo se ha procesado"));
}


void MainWindow::regenera_arch_config()
{
    fuerza_copia_arch_config();
    QMessageBox::information(this, tr("KEME-Contabilidad"),
                         tr("Operación realizada"));
}

void MainWindow::edpapelera()
{
  papelera *p = new papelera();
  p->exec();
  delete(p);
  refrescardiario();
  if (basedatos::instancia()->papelera_vacia())
     {
      QIcon icono(":/iconos/graf/trash-icon.png");
      ui->actionPapelera->setIcon(icono);
     }
     else
         {
          QIcon icono(":/iconos/graf/Recycle-Bin-full-icon.png");
          ui->actionPapelera->setIcon(icono);
         }
}

void MainWindow::gest_externos()
{
  QString cod_externo=punterodiario->externoactual();
  externos *e = new externos();
  if (!cod_externo.isEmpty()) e->pasa_codigo(cod_externo);
  e->exec();
  delete(e);
}


void MainWindow::gen_sii_recibidas()
{
    tablaSII *t = new tablaSII(usuario);
    t->exec();
    delete(t);
}

void MainWindow::gen_sii_emitidas()
{
    tablaSII *t = new tablaSII(usuario);
    t->config_emitidas();
    t->exec();
    delete(t);
}

void MainWindow::import_externos()
{
    impor_externos_csv *i = new impor_externos_csv();
    i->exec();
    delete (i);
}

void MainWindow::direc_soap_sii()
{
    direc_sii *d = new direc_sii();
    d->exec();
    delete(d);
}

void MainWindow::asigna_externo_apunte()
{
    QString elasiento;
    qlonglong nasiento=punterodiario->asientoactual();
    if (nasiento>0) elasiento.setNum(nasiento);
    if (elasiento=="")
       {
         QMessageBox::warning( this, tr("ASIGNAR DIARIO"),
         tr("Para asignar diario a un asiento debe de seleccionar un pase del mismo en el diario"));
         return;
       }
  QString ejercicio=ejerciciodelafecha(punterodiario->fechapaseactual());
  /*
  asignadiario *a = new asignadiario();
  a->pasanumasiento(elasiento,ejercicio);
  a->exec();
  delete(a);
  */
  busca_externo *b = new busca_externo();
  if (b->exec()==QDialog::Accepted) {
      basedatos::instancia()->actualiza_asiento_externo(b->codigo_elec(),elasiento,ejercicio);
      refrescardiario();
  }
  delete(b);
}

void MainWindow::util_carga_docs_bd() {
/*

asigna_fichdoc *a = new asigna_fichdoc(elasiento,punterodiario->documentoactual(),
                                           ejercicio);
    a->pasafichdoc(basedatos::instancia()->copia_docdiario (elasiento,ejercicio));


        qlonglong vnumdoc=basedatos::instancia()->select_prox_doc();
        QString cadnumdoc;
        cadnumdoc.setNum(vnumdoc);

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

QFile file(ui.fichdoclineEdit->text());
        file.open(QIODevice::ReadOnly);
        QByteArray inByteArray=file.readAll();
        basedatos::instancia()->inserta_doc_gestor(vnumdoc, copiadocfich, inByteArray);

 */
    // if (dir->exec() == QDialog::Accepted)
    pideejercicio *p = new(pideejercicio);
    int resultado=p->exec();
    QString ejercicio=p->contenido();
    delete(p);
    if (!(resultado == QDialog::Accepted)) return;
    int registros=0;
    QSqlQuery q=basedatos::instancia()->asientos_documentos_ejercicio(ejercicio);
    if (q.isActive())
        while (q.next()) {
          registros++;
        }
    if (registros==0) return;
    QProgressDialog progreso("Procesando archivos ...", NULL, 0, registros, this);
    progreso.setWindowTitle(tr("PROCESANDO..."));
    // progreso.setMaximum(0);
    // progreso.setValue(30);
    progreso.setMinimumDuration ( 0 );
    // progreso.forceShow();
    progreso.show();
    QApplication::processEvents();

    /*
    select asiento, copia_doc from diario where copia_doc != '' and ejercicio='2018'
     group by asiento,copia_doc order by asiento;
    */
    int reg=0;
    if (q.first())
        do {
          reg++;
          progreso.setValue(30);
          QApplication::processEvents();
          QString fichdoc=q.value(1).toString();
          QString asiento=q.value(0).toString();
          QString nuevo_nombre;
          QString cadnum;
          if (!fichdoc.contains('-')) { cadnum=fichdoc.section('.',0,0); nuevo_nombre=fichdoc;}
              else { cadnum=fichdoc.section('-',0,0); nuevo_nombre=fichdoc.section('-',1,1); }
          qlonglong num=cadnum.toLongLong();

          if (basedatos::instancia()->existe_doc_gestor(num)) continue;

          fichdoc=expanderutadocfich(fichdoc);

          qlonglong vnumdoc=basedatos::instancia()->select_prox_doc();
          QString cadnumdoc;
          cadnumdoc.setNum(vnumdoc);

          QString copiadocfich=cadnumdoc.trimmed();
          if (nuevo_nombre.length()>0)
              {
               copiadocfich+="-";
               nuevo_nombre.remove(' ');
               nuevo_nombre.remove('/');
               nuevo_nombre.remove('\\');
               nuevo_nombre.remove(':');
               nuevo_nombre.remove('*');
               nuevo_nombre.remove('?');
               nuevo_nombre.remove('|');
               nuevo_nombre.remove('<');
               nuevo_nombre.remove('>');
               copiadocfich+=nuevo_nombre;
              }
          //copiadocfich.append('.');
          //copiadocfich.append(extensionfich(ui.fichdoclineEdit->text()));


          QFile file(fichdoc);
          if (file.open(QIODevice::ReadOnly)) {
                  QByteArray inByteArray=file.readAll();
                  //QMessageBox::warning(this, tr("KEME-Contabilidad"),copiadocfich);
                  basedatos::instancia()->inserta_doc_gestor(vnumdoc, copiadocfich, inByteArray);
                  basedatos::instancia()->updatecopia_docdiario(asiento, copiadocfich,ejercicio);
                  // incrementamos contador de documento
                  vnumdoc++;
                  QString cadnumdoc;
                  cadnumdoc.setNum(vnumdoc);
                  basedatos::instancia()->updateConfiguracionprox_documento(cadnumdoc);
          }

        } while (q.next());

// falta refrescar diario
    refrescardiario();
}


void MainWindow::inv_inmov()
{
  inv_inmovilizado *i = new inv_inmovilizado();
  i->exec();
  delete(i);
}

void MainWindow::fichero_soltado_en_diario()
{
  fichero_soltado_diario=punterodiario->file_droped();
  QMessageBox::information(this, tr("KEME-Contabilidad"),fichero_soltado_diario);
  procesa_fichero_pdf();
}

void MainWindow::procesa_fichero_pdf() {
    QPdfDocument *pdf = new QPdfDocument(this);
    if (pdf->load(fichero_soltado_diario)==QPdfDocument::Error::None) {
      QString global_tex;
      if (pdf->pageCount()>10) { delete (pdf); return;}
      for (int i=0; i< pdf->pageCount(); i++) {
          QPdfSelection sel=pdf->getAllText(i);
          if (!global_tex.isEmpty()) global_tex.append(" ");
          global_tex+=sel.text();
      }
      qDebug() << global_tex;
      qDebug() << "\n";
      qDebug() << "\n";
      qDebug() << busca_nif(global_tex, basedatos::instancia()->cif());
    } else QMessageBox::information(this, tr("KEME-Contabilidad"),tr("Error al cargar el fichero"));
    delete (pdf);
}

void MainWindow::on_actionModelo_182_registro_donaciones_triggered()
{
  modelo_182 *m = new modelo_182(usuario);
  m->exec();
  delete(m);
}

void MainWindow::on_actionFicheros_de_transferencias_triggered()
{
    domiciliaciones *d = new domiciliaciones(usuario);
    d->para_pagos();
    d->exec();
    delete(d);
    refrescardiario();
}

void MainWindow::on_action_Tipos_de_Cambio_triggered()
{
    tipos_cambio *t = new tipos_cambio;
    t->exec();
    delete(t);
}

void MainWindow::on_diario_tab_cambiado(int index)
{
  if (index==0) {
      ui->actionBorrar_Asiento->setVisible(false);
      ui->actionEditar_Asiento->setVisible(false);
      ui->actionEditar_Asiento_N_mero->setVisible(false);
      if (basedatos::instancia()->solo_borr()) {
          ui->actionNuevo_Asiento->setEnabled(false);
          ui->actionAsientos_Predefinidos->setEnabled(false);
          ui->actionEjecutar_regul->setEnabled(false);      }
  } else {
      ui->actionBorrar_Asiento->setVisible(true);
      ui->actionEditar_Asiento->setVisible(true);
      ui->actionEditar_Asiento_N_mero->setVisible(true);
      ui->actionNuevo_Asiento->setEnabled(true);
      ui->actionAsientos_Predefinidos->setEnabled(true);
      ui->actionEjecutar_regul->setEnabled(true);
  }

}
