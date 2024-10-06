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

#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>
#include <QDomDocument>
#include <QtGui>
#include <QSplashScreen>
#include "network_connections.h"

#define NUMPRIVILEGIOS 88

#define LINEASPAGINA 35

extern bool privilegios[NUMPRIVILEGIOS];

extern bool copiardoc;

extern bool control_docs_repetidos;

extern bool hay_visualizador_extendido;

extern bool import_remota;

extern QStringList conexion_rutas, rutas_trabajo, rutas_docs, tipos_rectificativas;

extern QSplashScreen *splash;

extern QString global_ruta_curl;

extern QNetworkAccessManager *mNetMan;

extern bool curl_sii;

extern QString global_url_actu;
extern QString global_msj_url_actu;

QString convacoma(QString cad);

QString puntuanumerocomadec(QString origen);
 
QString ejerciciodelafecha(QDate fecha);

void calculaprimerasientoejercicio(QString ejercicio);

qlonglong primerasiento(QString ejercicio);

QString numrelativa(QString numabsoluta);

qlonglong primasejerciciodelafecha(QDate fecha);

QString puntuanumeropuntodec(QString origen);

QString filtro_a_lingu(QString fuente);

QString lingu_a_filtro(QString fuente);

QDate inicioejercicio(QString ejercicio);

QDate finejercicio(QString ejercicio);

QString ultimonumasiento(QString filtro);

QString descripcioncuenta(QString qcodigo);

double saldocuentaendiarioejercicio(QString cuenta,QString ejercicio);

QString nombreempresa();

QString trayreport();

void verifica_dir_informes(QString pasa); // pasa es la ruta completa de .keme

void copia_archivos_config();

void copia_arch(QString origen, QString qdestino);

QString trayconfig(void);

void fuerza_copia_arch_config();

int anchocuentas();

QString trad_fecha();

QString trad_usuario();

QString trad_asiento();

QString trad_cuenta();

QString trad_concepto();

QString trad_documento();

QString trad_debe();

QString trad_haber();

QString trad_diario();

QString trad_pase();

bool sobreescribeproxasiento(QString ejercicio);

QString convapunto(QString cadena);

QString quitacomillas(QString cadena);

bool fechacorrespondeaejercicio(QDate fecha);

bool fechadejerciciocerrado(QDate fecha);

int existecodigoplan(QString cadena,QString *qdescrip);

int existesubcuenta(QString codigo);

int escuentadeivasoportado(const QString subcuenta);

int escuentadeivarepercutido(const QString subcuenta);

int ejerciciocerrado(QString ejercicio);

void borraasientos(QString inicial,QString final,QString ejercicio);

bool paseenasiento(QString pase, QString );

int subgrupocongrupo(QString cadena);

int cuentaconsubgrupo(QString cadena);

int codigoconcuenta3digitos(QString cadena);

void insertaenplan(QString codigo,QString descripcion);

void guardacambiosplan(QString codigo,QString descripcion);

int itemplaneliminado(QString qcodigo);

void eliminasubcuenta(QString qcodigo);

int cuentaendiario(QString cadena);

QString expandepunto(QString cadena,int anchocad);


int existendatosaccesorios(QString cuenta, QString *razon, QString *ncomercial, QString *cif,
            QString *nifrprlegal, QString *domicilio, QString *poblacion,
            QString *codigopostal, QString *provincia, QString *pais,
            QString *tfno, QString *fax, QString *email, QString *observaciones,
            QString *ccc, QString *cuota, bool *venciasoc, QString *codvenci,
            QString *tesoreria, bool *ivaasoc, QString *cuentaiva, QString *cta_base_iva, QString *tipoiva,
            QString *conceptodiario, QString *web, QString *claveidfiscal, bool *procvto,
            QString *paisdat, QString *tipo_ret, QString *tipo_operacion_ret, bool *ret_arrendamiento,
            int *vfijo, QString *cuenta_ret_asig,
            bool *es_ret_asig_arrend, QString *tipo_ret_asig, QString *tipo_oper_ret_asig,
            bool *dom_bancaria, QString *IBAN, QString *nriesgo, QString *cedente1,
            QString *cedente2, QString *bic, QString *sufijo,
            QString *ref_mandato, QDate *firma_mandato, bool *caja_iva, QString *forma_pago,
            QString *imagen);

void guardadatosaccesorios(QString cuenta, QString razon, QString ncomercial, QString cif,
            QString nifrprlegal, QString domicilio, QString poblacion,
            QString codigopostal, QString provincia, QString pais,
            QString tfno, QString fax, QString email, QString observaciones,
            QString ccc, QString cuota, bool ivaasoc, QString tipoiva, QString cuentaiva, QString ctabaseiva,
            bool venciasoc, QString tipovenci, QString cuentatesor, QString conceptodiario,
            QString web, QString claveidfiscal, bool procvto,
            QString pais_dat, QString qtipo_ret, bool ret_arrendamiento,
            QString tipo_operacion_ret, int vdiafijo,
            QString cuenta_ret_asig, bool es_ret_asig_arrend,
            QString tipo_ret_asig, QString tipo_oper_ret_asig,
            bool dom_bancaria, QString iban, QString nriesgo, QString cedente1, QString cedente2,
            QString bic, QString sufijo, bool caja_iva, QString ref_mandato, QDate firma_mandato, QString forma_pago,
            QString foto);

void eliminadatosaccesorios(QString qcodigo);

bool conanalitica(void);

bool conanalitica_parc(void);

bool conigic(void);

int existecodigoconcepto(QString cadena,QString *qdescrip);

void insertaenconceptos(QString codigo,QString descripcion);

void guardacambiosconcepto(QString codigo,QString descripcion);

void eliminaconcepto(QString qcodigo);

int existeclavetipoiva(QString cadena,double *tipo,double *re,QString *qdescrip);

QString clave_iva_defecto(void);

QString formateanumero(double numero,bool comadecimal, bool decimales);

QString formateanumero_ndec(double numero,bool comadecimal,  int ndecimales);

int escuentadegasto(const QString subcuenta);

int escuentadeingreso(const QString subcuenta);

int existecodigosaldosubcuenta(QString cadena);

void insertasaldosubcuenta(QString codigo);

QString cuentadeivasoportado();

QString cuentadeivarepercutido();

void borraasientosnotransac(QString inicial,QString final,QString ejercicio);

void borrasnotransac(QString inicial,QString final);

bool buscaci(QString codigo, QString *descripcion, QString *qnivel);

void guardacambiosci(QString codigo,QString descripcion,int nivel);

void insertaenci(QString codigo,QString descripcion,int nivel);

void eliminaci(QString codigo);

bool existeciendiario(QString codigo, int nivel);

bool ciok(QString ci);

QString subcuentaanterior(QString qcodigo);

QString subcuentaposterior(QString qcodigo);

bool asientoenaib(QString, QString );

bool asientoeneib(QString, QString );

int ejerciciocerrando(QString ejercicio);

QDate fechadeasiento(QString q, QString ejercicio);

QString diariodeasiento(QString q, QString ejercicio);

int existeasiento(QString numero, QString ejercicio );

QString diario_apertura();

QString diario_general();

QString diario_no_asignado();

QString diario_cierre();

QString diario_regularizacion();

QString dirtrabajodefecto();

void check_dir_trabajo();

QString dirtrabajo(void);

void copia_estados_informes_dirtrabajo();

QString filtracad(QString origen);

QString mcortaa(QString cadena, uint num);

QString programa_imprimir();

QString programa_imprimir_defecto();

QString visordvidefecto();

QString visordvi();

bool filtrartexaxtivado();

void filtratex(QString cadfich);

bool haycomadecimal();

bool haydecimales();

QString splash_pref();

QString formateanumerosep(double numero,bool comadecimal, bool decimales);

QString formatea_redondeado_sep(double numero,bool comadecimal, bool decimales);

QString formatea_redondeado(double numero,bool comadecimal, bool decimales);

bool cifiltrook(QString ci);

QString filtroci(QString codigo, bool diario_ci);

bool parentesisbalanceados(QString cadena);

QString cadenamedia();

QString cadpositivo();

QString tipoNumerico();

QString tipoFecha();

QString tipoCaracteres();

QString tipoSubcuenta();

QString tipoNIF();

QString tipoDEBE();

QString tipoHABER();

QString tipoCI();

QString tipoCALC();

QString tipoConceptoSII();

bool chequeaformula(QString formula);

void cadvalorestipoiva(QString cadena,QString *tipo,QString *re);

bool cadnumvalido(QString cadnum);

double tipoivanore(QString clave);

double tipoivaefectivo(QString clave);

bool cadfechavalida(QString cadfecha);

bool cuentaycodigoenplanamort(QString cuenta,QString codigo);

bool practamort(QString ejercicio);

void actualizasaldo(QString cuenta,QString importe,bool cargo);

double saldocuentaendiario(QString cuenta);

double totalamortfiscal(QString cuenta);

QString ciamort();

bool espasevtoprocesado(QString qpase);

QString ejercicio_igual_o_maxigual(QDate fecha);

QString numvto(QString vto);

QString numvtovto(QString vto);

bool vencimientoprocesado(QString vto);

void anulavencimiento(QString vto);

void borravenci(QString vto);

bool existevencimiento(QString vto);

QString cadmes(int mes);

void borramodelo(QString );

int imprimelatex(QString qfichero);

int imprimelatex2(QString qfichero);

void desanulavencimiento(QString vto);

int existeclaveiva(QString cadena,QString *qtipo,QString *qre,QString *qdescrip);

void guardacambiostiposiva(QString clave,QString tipo,QString re,QString descripcion);

void insertatipoiva(QString clave,QString tipo, QString re, QString descripcion);

void eliminatipoiva(QString qclave);

// QString nifcomunitariocta(QString codigo);

QString cifcta(QString codigo);

int consultalatex(QString qfichero);

int consultalatex2(QString qfichero);

void borraestado(QString estado);

void desbloqueasaldosmedios();

bool exportarestado(QString estado, QString nombre);

int importarestado(QString nombre);

int consultalatex2fichabs(QString qfichero);

int generapdffichabs(QString qfichero);

bool int_asientos_ej_cerrado(QString asientoinicial,QString asientofinal);

void pasasaldo(QString cuenta,QString importe,bool cargo);

bool cuentaenlibroiva(QString cuenta);

bool paseapertura(QString pase);

bool paseconvto(QString pase);

bool pasepagovto(QString pase);

bool paseenlibroivaeib(QString pase);

void actualizasaldos();

QString traydoc(void);

QString traynombrefichayuda(void);

QString versionbd();

bool importanuevoplan(QString qfichero);

void borraplannuevo();

int subgrupocongruponuevoplan(QString cadena);

int cuentaconsubgruponuevoplan(QString cadena);

int codigoconcuenta3digitosnuevoplan(QString cadena);

void guardacambiosplannuevo(QString codigo,QString descripcion);

int existecodigoplannuevo(QString cadena,QString *qdescrip);

void insertaenplannuevo(QString codigo,QString descripcion);

int itemplaneliminadonuevo(QString qcodigo);

QString descripcioncuentanuevoplan(QString qcodigo);

bool codsubdivis(QString qcodigo);

bool codsubdivisplannuevo(QString qcodigo);

void cambiacuentaconta(QString origen, QString destino);

QString fsplash();

bool eswindows();

QString windirprogram();

QString adapta(QString cadena);

bool exportarasmodelo(QString ,QString nombre);

int importarasmodelo(QString nombre);

int existecodigodiario(QString cadena,QString *qdescrip);

void guardacambiosdiario(QString codigo,QString descripcion);

void insertaendiarios(QString codigo,QString descripcion);

int eliminadiario(QString qcodigo);

QString noejercicio();

bool existectapres(QString cuenta,QString ejercicio);

QString previsiones();

double prevision(QString codigo,QString ejercicio);

int idiomaconfig(void);

QString fich_ingles();

QString fich_portugues();

QString fich_euskera();

QString comandodescrip();

double previsionsegunsaldo(QString codigo,QString ejercicio,bool saldopos);

double previsionsegunsaldo_aux(QString codigo,QString ejercicio,bool saldopos);

bool cod_longitud_variable();

bool esauxiliar(QString codigo);

void inserta_auxiliar_enplan(QString codigo,QString descripcion);

bool es_long_var_agregadora(QString codigo);

bool prorrata_especial();

double prorrata_iva();

bool cuentaenplanamort(QString codigo);

bool compronivelsup();

bool existecodigodif_conc(QString cadena);

void guardacambiosdif_conc(QString codigo,QString descripcion);

void insertaendif_conc(QString codigo,QString descripcion);

QString absdepase(QString pase);

QString descripciondiferencia(QString qcodigo);

void eliminaconceptodif(QString qcodigo);

bool conceptodif_usado(QString cadena);

bool cuentaenpresupuesto(QString cadena);

bool amortenintasientos(QString inicial,QString final, QString ejercicio);

bool existecienplanamort(QString codigo, int nivel);

bool cuentaen_ajustes_conci(QString cadena);

bool cuentaencuenta_ext_conci(QString cadena);

QString fich_catalan();

QString fich_gallego();

QString dirtrabajobd(void);

QString dirtrabajodocs(QString qdoc);

QString dirtrabajo_sii();

QString dirtrabajo_sii_env();

QString dirtrabajo_certificados();

bool es_sqlite();

QString usuario_sistema();

QString nombrebd();

QString fichconex();

QString tipo_exento();

QString tipo_exento_nodeduc();

QString tipo_nosujeto();

QString soporte_xelatex();

QString config_font_xelatex();

QString cabeceralatex();

QString cabeceralatex_graficos();

QString margen_extra_latex();

QString margen_latex();

QString cadmes_minus(int mes);

QString fechaliteral(QDate fecha);

void borraasientomodelo(QString asiento);

void ejecuta_regularizacion(QString cuenta_reg, QString cuentas, QString descripcion,
                            QDate fechainicial,QDate fechafinal,QDate fecha, QString diario);

QString xmlAbreTagLin(int pos,QString cadena);

QString xmlCierraTagLin(int pos,QString cadena);

QString xmlAbreTag(int pos,QString cadena);

QString xmlCierraTag(QString cadena);

bool codigo_en_analitica(QString codigo);

QString etiquetadenodo(QDomDocument doc,QString etiqueta);

void addElementoTextoDom(QDomDocument doc,QDomElement padre,QString tagnombre,QString tagtexto);

QString textodenodo(QDomNode nodo);

QString numasientovto(QString vto);

QString numasientovtovto(QString vto);

QString asientoabsdepase(QString pase);

QString filtracadxml(QString cadena);

QString filtracadxml2(QString cadena);

QString cadena_descrip_ci (QString codigo);

QString tipoivaexento();

bool hay_prorratageneral();

void borrarasientofunc(QString elasiento, QString ejercicio, bool papelera);

QString adaptacad_aeat(QString cadena);

QString completacadnum(QString cadena, int espacio);

QString completacadcad(QString cadena, int espacio);

QString rutadocs(void);

QString rutacopiaseg(void);

QString rutalibros(void);

void visualizadores(QString *ext1, QString *vis1, QString *ext2, QString *vis2,
                    QString *ext3,QString *vis3, QString *ext4, QString *vis4);

QString aplicacionabrirfich(QString extension);

bool ejecuta(QString aplicacion, QString qfichero);

QString extensionfich(QString fichero);

QString rutacargadocs(void);

QString rutacopiapdf(void);

bool pref_forzar_ver_pdf();

QString expanderutadocfich(QString fichdoc);

QString dirdocsdefecto();

bool es_cuenta_para_analitica(QString codigo);

int editalatex(QString qfichero);

int editalatex_abs(QString qfichero);

QString editor_latex();

bool carga_saft_plan(QString qfichero);

bool carga_saft_asientos(QString qfichero, QString usuario);

QString trad_ejercicio();

QString trad_revisado();

QString trad_externo();

double CutAndRoundNumberToNDecimals (double dValue, int nDec);

double redond(double valor, int decimales);

bool es_os_x();

void ffacturae(QString serie, QString numero);

void fxmldoc(QString serie, QString numero);

QString nombre_fich_factura_apunte(QString qapunte);

QString latex_doc(QString serie, QString numero);

double total_documento(QString serie, QString numero);

bool genera_pdf_latex(QString qfichero);

QString msjdiario_regularizacion();

QString tipo_asiento_general();

QString logodefecto();

QString icono_smart();

void informe_jasper_xml(QString ficherojasper, QString ficheroxml,
                        QString expresionxpath, QString fichpdf,
                        QString ruta_graf);

int isNifCifNie(QString dni);

bool ccc_correcto(QString ccc);

bool IBAN_correcto(QString codigo);

QString digito_cotrol_IBAN(QString codigo);

void fijapermisos(QString ruta, QString permisos);

QString cad_tiempo();

QString qscript_copias(void);

QString logo_facturacion_defecto();

QString add_longitud_auxiliar(QStringList cuentas, QString auxiliar, int ampliar);

bool floats_2d_iguales(double num1, double num2);

bool nif_ue_correcto(QString nif_iva, QString &nombre, QString &direccion, bool &hayerror);

QString cadnum2dec_cadena(double num,int longitud);

QString cadnum2dec_cadena_signo(double num,int longitud);

bool f_importa_csv_cta_conci(QString qfichero,QString cuenta);

bool f_importarfichc43(QString qfichero,QString cuenta);

QString xml_aeat_ok_ids(QStringList nif, QStringList nombre);

bool casi_cero(double numero);

QString busca_nif(QString contenido, QString nif_propio);
