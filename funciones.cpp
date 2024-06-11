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

#include "funciones.h"
#include "aritmetica.h"
#include "basedatos.h"
#include "privilegios.h"
#include "pide_fich.h"

#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QProgressDialog>
#include <QtGui>
#include <Qt>
#include <QRegExp>
#include <QApplication>
#include <qtrpt.h>


#if QT_VERSION >= 0x050000
 #define QT_WA(unicode, ansi) unicode
#endif

#ifdef _WIN64
   #define WINDOWS 1
   #include "qt_windows.h"
   #include "qwindowdefs_win.h"
   #include <shellapi.h>
#elif _WIN32
   #define WINDOWS 1
   #include "qt_windows.h"
   #include "qwindowdefs_win.h"
   #include <shellapi.h>
#else
   #define WINDOWS 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
  #include <TargetConditionals.h>
  #if TARGET_OS_MAC == 1
      #define OSX 1
  #else
      #define OSX 0
  #endif
#else
  #define OSX 0
#endif


#define COMPRONIVELSUP 0

#define LINEAS_FAC 10

bool privilegios[NUMPRIVILEGIOS];

bool copiardoc=true;

bool control_docs_repetidos=false;

bool hay_visualizador_extendido=false;

bool import_remota=false;

QStringList conexion_rutas, rutas_trabajo, rutas_docs, tipos_rectificativas;

QSplashScreen *splash=NULL;

QString global_ruta_curl;

QString global_url_actu;
QString global_msj_url_actu;

QNetworkAccessManager *mNetMan=nullptr;

bool curl_sii=false;

QString convacoma(QString cad)
{
  QString origen;
  // if (origen[origen.length()-3]=='.') origen[origen.length()-3]=',';
  origen=cad.replace('.',',');
  return origen;
}


QString puntuanumerocomadec(QString origen)
{
 QString origen1=convacoma(origen).remove(' ');
 QString entera=origen1.left(uint(origen1.indexOf(',')));
 QString signo;
 // if (entera[1]=='-') {signo="-"; entera=entera.right(entera.length()-1);}
 if (entera.length()<4) return origen1;
 QString decimal=origen1.right(origen1.length()-uint(origen1.indexOf(',')));
 QString enterapuntos;
 int pasados=0;
 for (int veces=entera.length()-1;veces>=0;veces--)
     {
       pasados++;
       enterapuntos=enterapuntos.insert(0,entera[veces]);
       if (pasados==3 || pasados==6 || pasados==9)
           enterapuntos=enterapuntos.insert(0,'.');
     }
 if (enterapuntos[0]=='.') enterapuntos=enterapuntos.remove(0,1);
 if (enterapuntos[0]=='-' && enterapuntos[1]=='.') enterapuntos=enterapuntos.remove(1,1);
 QString resultado;
 resultado=signo;
 resultado+=enterapuntos;
 resultado+=decimal;
 return resultado;
}

 
QString puntuanumeropuntodec(QString origen)
{
 QString origen1=origen.remove(' ');
 QString entera=origen1.left(uint(origen1.indexOf('.')));
 QString signo;
 // if (entera[1]=='-') {signo="-"; entera=entera.right(entera.length()-1);}
 if (entera.length()<4) return origen1;
 QString decimal=origen1.right(origen1.length()-uint(origen1.indexOf('.')));
 QString enterapuntos;
 int pasados=0;
 for (int veces=entera.length()-1;veces>=0;veces--)
     {
       pasados++;
       enterapuntos=enterapuntos.insert(0,entera[veces]);
       if (pasados==3 || pasados==6 || pasados==9)
           enterapuntos=enterapuntos.insert(0,',');
     }
 if (enterapuntos[0]==',') enterapuntos=enterapuntos.remove(0,1);
 if (enterapuntos[0]=='-' && enterapuntos[1]==',') enterapuntos=enterapuntos.remove(1,1);
 QString resultado;
 resultado=signo;
 resultado+=enterapuntos;
 resultado+=decimal;
 return resultado;
}


QString ejerciciodelafecha(QDate fecha)
{
    return basedatos::instancia()->selectEjerciciodelafecha(fecha);
}

void calculaprimerasientoejercicio(QString ejercicio)
{
    basedatos::instancia()->calculaprimerasientoejercicio(ejercicio);
}


qlonglong primerasiento(QString ejercicio)
{
    return basedatos::instancia()->selectPrimerasiento(ejercicio);
}

QString numrelativa(QString numabsoluta)
{
    return basedatos::instancia()->numrelativa(numabsoluta);
}

qlonglong primasejerciciodelafecha(QDate fecha)
{
    return basedatos::instancia()->primasejerciciodelafecha(fecha);
}


QString trad_fecha()
{
  return QObject::tr("fecha");
}

QString trad_usuario()
{
  return QObject::tr("usuario");
}

QString trad_asiento()
{
  return QObject::tr("asiento");
}

QString trad_cuenta()
{
  return QObject::tr("cuenta");
}

QString trad_concepto()
{
  return QObject::tr("concepto");
}

QString trad_documento()
{
  return QObject::tr("documento");
}

QString trad_debe()
{
  return QObject::tr("debe");
}

QString trad_haber()
{
  return QObject::tr("haber");
}

QString trad_diario()
{
  return QObject::tr("diario");
}

QString trad_pase()
{
  return QObject::tr("pase");
}

QString trad_ejercicio()
{
  return QObject::tr("ejercicio");
}

QString trad_revisado()
{
  return QObject::tr("revisado");
}

QString trad_externo()
{
  return QObject::tr("externo");
}


QString filtro_a_lingu(QString fuente)
{
  QString destino=fuente;
  destino.replace(QString("fecha"),trad_fecha());
  destino.replace(QString("usuario"),trad_usuario());
  destino.replace(QString("asiento"),trad_asiento());
  destino.replace(QString("cuenta"),trad_cuenta());
  destino.replace(QString("concepto"),trad_concepto());
  destino.replace(QString("documento"),trad_documento());
  destino.replace(QString("debe"),trad_debe());
  destino.replace(QString("haber"),trad_haber());
  destino.replace(QString("diario"),trad_diario());
  destino.replace(QString("pase"),trad_pase());
  destino.replace(QString("ejercicio"),trad_ejercicio());
  destino.replace(QString("revisado"),trad_revisado());
  return destino;
}


QString lingu_a_filtro(QString fuente)
{
  QString destino=fuente;
  destino.replace(trad_fecha(),QString("fecha"));
  destino.replace(trad_usuario(),QString("usuario"));
  destino.replace(trad_asiento(),QString("asiento"));
  destino.replace(trad_cuenta(),QString("cuenta"));
  destino.replace(trad_concepto(),QString("concepto"));
  destino.replace(trad_documento(),QString("documento"));
  destino.replace(trad_debe(),QString("debe"));
  destino.replace(trad_haber(),QString("haber"));
  destino.replace(trad_diario(),QString("diario"));
  destino.replace(trad_pase(),QString("pase"));
  return destino;
}


QDate inicioejercicio(QString ejercicio)
{
    return basedatos::instancia()->selectAperturaejercicios(ejercicio);
}

QDate finejercicio(QString ejercicio)
{
    return basedatos::instancia()->selectCierreejercicios(ejercicio);
}

QString ultimonumasiento(QString filtro)
{
    return basedatos::instancia()->selectUltimonumasiento(filtro);
}

QString descripcioncuenta(QString qcodigo)
{
    return basedatos::instancia()->selectDescripcionplancontable(qcodigo);
}

double saldocuentaendiarioejercicio(QString cuenta,QString ejercicio)
{
    return basedatos::instancia()->selectSaldocuentaendiarioejercicio(cuenta,ejercicio);
}


QString nombreempresa()
{
    return basedatos::instancia()->selectEmpresaconfiguracion();
}

QString trayreport()
{
 QString ruta=trayconfig();
 ruta+=QDir::separator();
 ruta+="report";
 return ruta;
}

void verifica_dir_informes(QString pasa) // pasa es la ruta completa de .keme
{
    QString pasa00=pasa;
    pasa00+=QDir::separator();
    pasa00+="report";
    QDir d3(pasa00);
    if (!d3.exists()) d3.mkdir(pasa00);
    QString fich_mayor_jrxml=pasa00;
    fich_mayor_jrxml+=QDir::separator();
    fich_mayor_jrxml+="mayor.jrxml";
    if (eswindows()) fich_mayor_jrxml=QFile::encodeName(fich_mayor_jrxml);
    QFile mayor_informe(fich_mayor_jrxml);
    if (!mayor_informe.exists())
       {
         QString pasa22;
         if (eswindows()) pasa22=QFile::encodeName(traydoc());
            else pasa22=traydoc();
         pasa22+=QDir::separator();
         pasa22+="report";
         QDir dirdocreport(pasa22);
         QStringList filtersr;
         filtersr << "*";
         dirdocreport.setNameFilters(filtersr);
         //************falta copiado
         QStringList ficherosr=dirdocreport.entryList();
         for (int veces=0; veces<ficherosr.count(); veces++)
             {
              QString cad=traydoc();
              cad.append(QDir::separator());
              cad.append("report");
              cad.append(QDir::separator());
              // QString cfich=cfich.fromLocal8bit(ficheros.at(veces));
              QString cfich=ficherosr.at(veces);
              cad+=cfich;
              QString destino=pasa00;
              destino.append(QDir::separator());
              destino+=cfich;
              QString p;
              if (eswindows()) p=QFile::encodeName(cad);
                 else p=cad;
              QFile fichero(p);
              if (eswindows()) fichero.copy(QFile::encodeName(destino));
                 else fichero.copy(destino);
              // qWarning(destino.toAscii().constData());
              // QFile::copy ( cad, destino );
             }
       }
}

void copia_arch_config(QString origen, QString subdir="")
{
    QFile fichero(origen);

    QString directorio;
    if (WINDOWS) directorio=directorio.fromLocal8Bit(getenv("USERPROFILE"));
      else directorio=getenv("HOME");
    //if (es_os_x()) directorio= getenv("HOME")+ directorio.fromLocal8Bit("/Library/Preferences");
    QString nombredir="/.keme";
    //if (es_os_x()) nombredir="/keme5";
    nombredir[0]=QDir::separator();
    directorio+=nombredir;
    if (!subdir.isEmpty())
       {
         directorio+=QDir::separator();
         directorio+=subdir;
       }
    directorio+=QDir::separator();
    QString destino=directorio+origen.mid(origen.lastIndexOf("/")+1);
    //fichero.copy(eswindows() ? QFile::encodeName(destino) : destino);
    fichero.copy(destino);
}

void copia_arch(QString origen, QString qdestino)
{
    QFile fichero(origen);

    QString directorio=qdestino;
    directorio+=QDir::separator();
    QString destino=directorio+origen.mid(origen.lastIndexOf("/")+1);
    fichero.copy(destino);
}


void copia_archivos_config()
{
    // ------------------------------------------------------------
    // pasamos planes contables
    copia_arch_config(":/planes/planes/pgc_pymes_RD1515-2007.pln");
    copia_arch_config(":/planes/planes/pgc2007.pln");
    copia_arch_config(":/planes/planes/pla-pymes-2007-cat.pln");
    copia_arch_config(":/planes/planes/plan-entidades-sin-animo-lucro-simplificado2011.pln");
    copia_arch_config(":/planes/planes/plan-entidades-sin-animo-lucro2011.pln");

    // ------------------------------------------------------------
    copia_estados_informes_dirtrabajo();
}

void fuerza_copia_arch_config()
{
    QString directorio;
    if (WINDOWS) directorio=directorio.fromLocal8Bit(getenv("USERPROFILE"));
      else directorio=getenv("HOME");
    //if (es_os_x()) { directorio= getenv("HOME");
    //                 directorio+="/Library/Preferences";}
    QString nombredir="/.keme";
    //if (es_os_x()) nombredir="/keme5";
     nombredir[0]=QDir::separator();
     directorio+=nombredir;
     QString pasa;
     //if (eswindows()) pasa=QFile::encodeName(directorio);
     //   else pasa=directorio;
     pasa=directorio;
     QDir d2(pasa);
     if (!d2.exists()) d2.mkdir(pasa);
     copia_archivos_config();

}

QString trayconfig(void)
{
    QString directorio;
    if (WINDOWS) directorio=directorio.fromLocal8Bit(getenv("USERPROFILE"));
      else directorio=getenv("HOME");
    //if (es_os_x()) { directorio= getenv("HOME");
    //                 directorio+="/Library/Preferences";}
    QString nombredir="/.keme";
    //if (es_os_x()) nombredir="/keme5";
     nombredir[0]=QDir::separator();
     directorio+=nombredir;
     QString pasa;
     //if (eswindows()) pasa=QFile::encodeName(directorio);
     //   else pasa=directorio;
     pasa=directorio;
     QDir d2(pasa);
     if (!d2.exists())
       {
        d2.mkdir(pasa);
        //d2.mkdir(pasa+QDir::separator()+"report");
        //d2.mkdir(pasa+QDir::separator()+"report"+QDir::separator()+"jasp_xml2pdf_lib");
        // copiamos al directorio "pasa" los archivos del resource file
        copia_archivos_config();

        // ------------------------------------------------------------
       }

     // verifica_dir_informes(pasa);

     return directorio;
}


int anchocuentas()
{
    return basedatos::instancia()->selectAnchocuentasconfiguracion();
}

bool sobreescribeproxasiento(QString ejercicio)
{
    return basedatos::instancia()->sobreescribeproxasiento(ejercicio);
}

QString convapunto(QString cadena)
{
  return cadena.replace(',','.');
}


QString quitacomillas(QString cadena)
{
  QString resultado;
  resultado=cadena.remove('"'); // comillas dobles
  resultado=resultado.remove("'"); // comillas simples
  return resultado;
}


bool fechacorrespondeaejercicio(QDate fecha)
{
 if (ejerciciodelafecha(fecha).length()==0) return false;
 return true;
}

bool fechadejerciciocerrado(QDate fecha)
{
  QString ejercicio;
  ejercicio=ejerciciodelafecha(fecha);
  if (ejerciciocerrado(ejercicio)) return true;
  return false;
}

int existecodigoplan(QString cadena,QString *qdescrip)
{
    return basedatos::instancia()->existecodigoplan(cadena,qdescrip);
}


int existesubcuenta(QString codigo)
{

 QString ladescrip="";
 if (!cod_longitud_variable())
   {
    if (existecodigoplan(codigo,&ladescrip) && codigo.length()==anchocuentas()) return 1;
     else return 0;
   }
   else
       {
        if (existecodigoplan(codigo,&ladescrip) && esauxiliar(codigo)) return 1;
         else return 0;
       }
 return 0;
}


int escuentadeivasoportado(const QString subcuenta)
{
    return basedatos::instancia()->escuentadeivasoportado(subcuenta);
}

int escuentadeivarepercutido(const QString subcuenta)
{
    return basedatos::instancia()->escuentadeivarepercutido(subcuenta);
}

int ejerciciocerrado(QString ejercicio)
{
    return basedatos::instancia()->selectCerradoejercicios(ejercicio);
}

void borraasientos(QString inicial,QString final,QString ejercicio)
{
    basedatos::instancia()->borraasientos(inicial,final,ejercicio);
}

bool amortenintasientos(QString inicial,QString final, QString ejercicio)
{
    return basedatos::instancia()->amortenintasientos(inicial,final,ejercicio);
}


bool paseenasiento(QString pase, QString asiento)
{
    return basedatos::instancia()->paseenasiento(pase,asiento);
}


int subgrupocongrupo(QString cadena)
{
    return basedatos::instancia()->subgrupocongrupo(cadena);
}


int cuentaconsubgrupo(QString cadena)
{
    return basedatos::instancia()->cuentaconsubgrupo(cadena);
}


int codigoconcuenta3digitos(QString cadena)
{
    return basedatos::instancia()->codigoconcuenta3digitos(cadena);
}

void insertaenplan(QString codigo,QString descripcion)
{
    QString cadena;
    if (!existecodigoplan(codigo,&cadena))
    {
        basedatos::instancia()->insertPlancontable(codigo,descripcion,false);
    }
}


void inserta_auxiliar_enplan(QString codigo,QString descripcion)
{
    QString cadena;
    if (!existecodigoplan(codigo,&cadena))
    {
        basedatos::instancia()->insertPlancontable(codigo,descripcion,true);
    }
}

void guardacambiosplan(QString codigo,QString descripcion)
{
    basedatos::instancia()->updatePlancontabledescripcioncodigo(codigo,descripcion);
}

int itemplaneliminado(QString qcodigo)
{
    return basedatos::instancia()->itemplaneliminado(qcodigo);
}

void eliminasubcuenta(QString qcodigo)
{
    basedatos::instancia()->deleteDatossubcuentacuenta(qcodigo);
    basedatos::instancia()->deleteSaldossubcuentacodigo(qcodigo);
    basedatos::instancia()->deletePlancontablecodigo(qcodigo);
}


int cuentaendiario(QString cadena)
{
    return basedatos::instancia()->cuentaendiario(cadena);
}

QString expandepunto(QString cadena,int anchocad)
{
  if (anchocad==0) return cadena;
  int indice=0,longitud=0,veces;
  if (!cadena.contains('.')) return cadena;
  while (cadena.contains('.'))
    {
     indice=cadena.indexOf('.');
     cadena.remove(indice,1);
    }
  longitud=cadena.length();
  for (veces=0;veces<anchocad-longitud;veces++)
       cadena.insert(indice,'0');
  return cadena;
}


int existendatosaccesorios(QString cuenta, QString *razon, QString *ncomercial, QString *cif,
            QString *nifrprlegal, QString *domicilio, QString *poblacion,
            QString *codigopostal, QString *provincia, QString *pais,
            QString *tfno, QString *fax, QString *email, QString *observaciones,
            QString *ccc, QString *cuota, bool *venciasoc, QString *codvenci,
            QString *tesoreria, bool *ivaasoc, QString *cuentaiva, QString *cta_base_iva, QString *tipoiva,
            QString *conceptodiario, QString *web, QString *claveidfiscal, bool *procvto,
            QString *paisdat, QString *tipo_ret, QString *tipo_operacion_ret,
            bool *ret_arrendamiento, int *vfijo, QString *cuenta_ret_asig,
            bool *es_ret_asig_arrend, QString *tipo_ret_asig, QString *tipo_oper_ret_asig,
            bool *dom_bancaria, QString *IBAN, QString *nriesgo, QString *cedente1,
            QString *cedente2, QString *bic, QString *sufijo,
            QString *ref_mandato, QDate *firma_mandato,
            bool *caja_iva, QString *forma_pago,
            QString *imagen)
{
    QSqlQuery query = basedatos::instancia()->selectTodoDatossubcuentacuenta(cuenta);
    // pais_dat, tipo_ret, tipo_operacion_ret, ret_arrendamiento
    if ( (query.isActive()) && (query.first()) )
    {
        *razon = query.value(1).toString();
        *ncomercial = query.value(2).toString();
        *cif = query.value(3).toString();
        *nifrprlegal = query.value(4).toString();
        *domicilio = query.value(5).toString();
        *poblacion = query.value(6).toString();
        *codigopostal = query.value(7).toString();
        *provincia = query.value(8).toString();
        *pais = query.value(9).toString();
        *tfno = query.value(10).toString();
        *fax = query.value(11).toString();
        *email = query.value(12).toString();
        *observaciones = query.value(13).toString();
        *ccc = query.value(14).toString();
        *cuota = query.value(15).toString();
        *venciasoc = query.value(16).toBool();
        *codvenci = query.value(17).toString();
        *tesoreria = query.value(18).toString();
        *ivaasoc = query.value(19).toBool();
        *cuentaiva = query.value(20).toString();
        *tipoiva = query.value(21).toString();
        *conceptodiario = query.value(22).toString();
        *web = query.value(23).toString();
        *claveidfiscal = query.value(24).toString();
        *procvto = query.value(25).toBool();
        *paisdat = query.value(26).toString();
        *tipo_ret = query.value(27).toString();
        *tipo_operacion_ret = query.value(28).toString();
        *ret_arrendamiento = query.value(29).toBool();
        *vfijo=query.value(30).toInt();

        *cuenta_ret_asig=query.value(31).toString();
        *es_ret_asig_arrend=query.value(32).toBool();
        *tipo_ret_asig=query.value(33).toString();
        *tipo_oper_ret_asig=query.value(34).toString();

        // bool *dom_bancaria, QString *IBAN, QString *nriesgo, QString *cedente1, QString *cedente2,
        *dom_bancaria=query.value(35).toBool(),
        *IBAN=query.value(36).toString();
        *nriesgo=query.value(37).toString();
        *cedente1=query.value(38).toString();
        *cedente2=query.value(39).toString();
        *bic=query.value(40).toString();
        *sufijo=query.value(41).toString();
        *caja_iva=query.value(42).toBool();
        *ref_mandato=query.value(43).toString();
        *firma_mandato=query.value(44).toDate();
        *cta_base_iva=query.value(45).toString();
        *forma_pago=query.value(46).toString();
        *imagen=basedatos::instancia()->fotocuenta(cuenta);
        return 1;
    }
    return 0;
}

void guardadatosaccesorios(QString cuenta, QString razon, QString ncomercial,QString cif,
            QString nifrprlegal, QString domicilio, QString poblacion,
            QString codigopostal, QString provincia, QString pais,
            QString tfno, QString fax, QString email, QString observaciones,
            QString ccc, QString cuota, bool ivaasoc, QString tipoiva, QString cuentaiva, QString ctabaseiva,
            bool venciasoc, QString tipovenci, QString cuentatesor, QString conceptodiario,
            QString web, QString codclaveidfiscal, bool procvto,
            QString pais_dat, QString qtipo_ret, bool ret_arrendamiento,
            QString tipo_operacion_ret, int vdiafijo,
            QString cuenta_ret_asig, bool es_ret_asig_arrend,
            QString tipo_ret_asig, QString tipo_oper_ret_asig,
            bool dom_bancaria, QString iban, QString nriesgo, QString cedente1, QString cedente2,
            QString bic, QString sufijo, bool caja_iva, QString ref_mandato, QDate firma_mandato,
            QString forma_pago,
            QString fototexto)
{
    basedatos::instancia()->guardadatosaccesorios(cuenta, razon, ncomercial,cif,
            nifrprlegal, domicilio, poblacion,
            codigopostal, provincia, pais,
            tfno, fax, email, observaciones,
            ccc, cuota, ivaasoc, tipoiva, cuentaiva, ctabaseiva, venciasoc, tipovenci, cuentatesor,
            conceptodiario, web, codclaveidfiscal, procvto,
            pais_dat, qtipo_ret, ret_arrendamiento, tipo_operacion_ret, vdiafijo,
            cuenta_ret_asig, es_ret_asig_arrend,
            tipo_ret_asig, tipo_oper_ret_asig,
            dom_bancaria, iban, nriesgo, cedente1, cedente2, bic, sufijo, caja_iva,
            ref_mandato, firma_mandato, forma_pago,
            fototexto);
}


void eliminadatosaccesorios(QString qcodigo)
{
    basedatos::instancia()->deleteDatossubcuentacuenta(qcodigo);
}

bool conanalitica(void)
{
    return basedatos::instancia()->selectAnaliticaconfiguracion();
}

bool conanalitica_parc(void)
{
    return basedatos::instancia()->selectAnalitica_parc_configuracion();
}

bool codigo_en_analitica(QString codigo)
{
  QString cadcodigos=basedatos::instancia()->select_codigos_en_ctas_analitica();
  cadcodigos.remove(' ');
  QStringList lista;
  lista = cadcodigos.split(",");
  for (int veces=0;veces<lista.size();veces++)
      if (codigo.startsWith(lista.at(veces))) return true;
  return false;
}

bool conigic(void)
{
    return basedatos::instancia()->selectIgicconfiguracion();
}
int existecodigoconcepto(QString cadena,QString *qdescrip)
{
    return basedatos::instancia()->existecodigoconcepto(cadena,qdescrip);
}

void guardacambiosconcepto(QString codigo,QString descripcion)
{
    basedatos::instancia()->updateConceptosdescripcionclave(codigo,descripcion);
}

void insertaenconceptos(QString codigo,QString descripcion)
{
    basedatos::instancia()->insertConceptos(codigo,descripcion);
}

void eliminaconcepto(QString qcodigo)
{
    basedatos::instancia()->deleteConceptosclave(qcodigo);
}


int existeclavetipoiva(QString cadena,double *tipo,double *re,QString *qdescrip) {
    QSqlQuery query = basedatos::instancia()->selectTodotiposivaclave(cadena);
 
    if ( (query.isActive()) && (query.first()) )
    {
        *qdescrip=query.value(3).toString();
	    *tipo=query.value(1).toDouble();
	    *re=query.value(2).toDouble();
        return 1;
    }
    return 0;
}


QString clave_iva_defecto(void)
{
    return basedatos::instancia()->selectClave_iva_defectoconfiguracion();
}

QString formateanumero(double numero,bool comadecimal, bool decimales)
{

 int ndecimales=2;
 if (!decimales) ndecimales=0;
 QString cadena; 
 cadena.setNum(numero,'f',ndecimales);
 if (comadecimal)
     return convacoma(cadena);
 return cadena;

}

QString formateanumero_ndec(double numero,bool comadecimal,  int ndecimales)
{

 QString cadena;
 cadena.setNum(numero,'f',ndecimales);
 if (comadecimal)
     return convacoma(cadena);
 return cadena;

}


int escuentadegasto(const QString subcuenta)
{
    return basedatos::instancia()->escuentadegasto(subcuenta);
}


int escuentadeingreso(const QString subcuenta)
{
    return basedatos::instancia()->escuentadeingreso(subcuenta);
}

int existecodigosaldosubcuenta(QString cadena)
{
    return basedatos::instancia()->existecodigosaldosubcuenta(cadena);
}

void insertasaldosubcuenta(QString codigo)
{
    basedatos::instancia()->insertSaldossubcuenta(codigo,"0","0","0");
}


QString cuentadeivasoportado()
{
    return basedatos::instancia()->cuentadeivasoportado();
}


QString cuentadeivarepercutido()
{
    return basedatos::instancia()->cuentadeivarepercutido();
}


void borraasientosnotransac(QString asientoinicial,QString asientofinal, QString ejercicio)
{
 basedatos::instancia()->borraasientosnotransac(asientoinicial,asientofinal,ejercicio);
}


bool buscaci(QString codigo, QString *descripcion, QString *qnivel)
{
    return basedatos::instancia()->selectTodoCicodigo(codigo,descripcion,qnivel);
}

void guardacambiosci(QString codigo,QString descripcion,int nivel)
{
    basedatos::instancia()->updateCidescripcioncodigo(codigo,descripcion,nivel);
}

void insertaenci(QString codigo,QString descripcion,int nivel)
{
    basedatos::instancia()->insertCi(codigo,descripcion,nivel);
}

void eliminaci(QString codigo)
{
    basedatos::instancia()->deleteCicodigo(codigo);
}

bool existeciendiario(QString codigo, int nivel)
{
    return basedatos::instancia()->existeciendiario(codigo,nivel);
}



bool existecienplanamort(QString codigo, int nivel)
{
    return basedatos::instancia()->existecienplanamort(codigo,nivel);
}


bool ciok(QString ci)
{
  if (ci.length()<3) return false;
  if (ci.length()>3 && ci.length()<6) return false;
  // Extraemos primer trozo
  QString descripcion,qnivel;
  int nnivel;
  if (buscaci(ci.left(3), &descripcion, &qnivel))
      {
       nnivel=qnivel.toInt();
       if (nnivel!=1) return false;
      }
      else return false;

  if (ci.length()==3) return true;

  if (buscaci(ci.mid(3,3), &descripcion, &qnivel))
      {
       nnivel=qnivel.toInt();
       if (nnivel!=2) return false;
      }
      else return false;

  if (ci.length()==6) return true;

  if (buscaci(ci.right(ci.length()-6),&descripcion,&qnivel))
     {
       nnivel=qnivel.toInt();
       if (nnivel!=3) return false;
     }
     else return false;

 return true;
}

QString subcuentaanterior(QString qcodigo)
{
    return basedatos::instancia()->subcuentaanterior(qcodigo);
}


QString subcuentaposterior(QString qcodigo)
{
    return basedatos::instancia()->subcuentaposterior(qcodigo);
}

bool asientoenaib(QString asiento, QString ejercicio)
{
    return basedatos::instancia()->asientoenaib(asiento,ejercicio);}

bool asientoeneib(QString asiento, QString ejercicio)
{
    return basedatos::instancia()->asientoeneib(asiento, ejercicio);
}

int ejerciciocerrando(QString ejercicio)
{
    return basedatos::instancia()->selectCerrandoejercicios(ejercicio);
}

QDate fechadeasiento(QString qasiento,QString ejercicio)
{
    return basedatos::instancia()->selectFechadiarioasiento(qasiento, ejercicio);
}


QString asientoabsdepase(QString pase)
{
    return basedatos::instancia()->selectAsientodiariopase(pase);
}


QString diariodeasiento(QString qasiento, QString ejercicio)
{
    return basedatos::instancia()->selectDiariodiarioasiento(qasiento, ejercicio);
}


int existeasiento(QString asiento, QString ejercicio)
{
    return basedatos::instancia()->existeasiento(asiento,ejercicio);
}

QString diario_apertura()
{
 return QObject::tr("APERTURA");
}

QString diario_general()
{
 return QObject::tr("GENERAL");
}

QString diario_no_asignado()
{
 return QObject::tr("SIN_ASIGNAR");
}

QString diario_cierre()
{
 return QObject::tr("CIERRE");
}

QString previsiones()
{
 return QObject::tr("PREV");
}

QString diario_regularizacion()
{
 return QObject::tr("REGULARIZACION");
}

QString msjdiario_regularizacion()
{
 return QObject::tr("REGULARIZACIÓN");
}

QString dirtrabajodefecto()
{
   QString directorio;
   if (WINDOWS) directorio=directorio.fromLocal8Bit(getenv("USERPROFILE"));
     else
       directorio=getenv("HOME");
   // if (es_os_x()) return directorio+"/Library/Application Support/keme5";
   QString nombredir="/keme";
   nombredir[0]=QDir::separator();
   directorio+=nombredir;
   return directorio;
}

void check_dir_trabajo()
{
    QString nombrefichcompleto=trayconfig();
    QString nombre="/keme.cfg";
    nombre[0]=QDir::separator();
    nombrefichcompleto+=nombre;
    if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
    QFile ficheroini(nombrefichcompleto);
    QString qdirtrabajo;

     if ( ficheroini.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &ficheroini );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea,variable,contenido;
          while ( !stream.atEnd() ) {
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              // analizamos la linea de texto
              if (linea.contains('=')) {
                 variable=linea.section('=',0,0);
                 contenido=linea.section('=',1,1);
                 if (variable.compare("DIRECTORIO")==0) qdirtrabajo=contenido;
                }
              }
          ficheroini.close();
         }
    if (qdirtrabajo.length()==0) qdirtrabajo=dirtrabajodefecto();
    QDir a(qdirtrabajo);
    qdirtrabajo.append(QDir::separator());
    if (!a.exists())
       {
        a.mkdir(qdirtrabajo);
        // comprobamos que exista la empresa de ejemplo SQLITE
        QString fichsqlite=qdirtrabajo+"ejemplo-sqlite";
        if (!QFile::exists(fichsqlite))
           {
              QString pasa2;
              pasa2=":/fich/fich/";
              //pasa2=traydoc();
              //pasa2.append(QDir::separator());
              pasa2.append("ejemplo-sqlite");
              // if (eswindows()) pasa2=QFile::encodeName(pasa2);
              QFile fichero(pasa2);
              //if (eswindows()) fichero.copy(QFile::encodeName(fichsqlite));
              //    else fichero.copy(fichsqlite);
              fichero.copy(fichsqlite);
              if (!eswindows())
                 fijapermisos(fichsqlite, "a+w");
                else
                  fijapermisos(fichsqlite, "-r");
           }
        // comprobamos que exista el fichero de la memoria
        QString fichmemoria=qdirtrabajo+"memoria.tex";
        if (!QFile::exists(fichmemoria))
           {
              QString pasa2;
              pasa2=":/fich/fich/";
              //pasa2=traydoc();
              //pasa2.append(QDir::separator());
              pasa2.append("memoria.tex");
              //if (eswindows()) pasa2=QFile::encodeName(pasa2);
              QFile fichero(pasa2);
              //if (eswindows()) fichero.copy(QFile::encodeName(fichmemoria));
              //    else fichero.copy(fichmemoria);
              fichero.copy(fichmemoria);
           }
         copia_estados_informes_dirtrabajo();
       }

}

void copia_estados_informes_dirtrabajo() {
    // comprobamos existencia de zz_Estados_Contables
    //copia_arch(QString origen, QString qdestino)
    QString qdirtrabajo=dirtrabajobd();
    qdirtrabajo.append(QDir::separator());
    QDir direstados(qdirtrabajo +"zz_Estados_Contables");
    if (!direstados.exists()) direstados.mkdir(qdirtrabajo +"zz_Estados_Contables");

    // copiamos archivos
    copia_arch(":/estados/estados/balance_abreviado_plan_2007.est.xml",qdirtrabajo + "zz_Estados_Contables");
    copia_arch(":/estados/estados/balance_plan_2007.est.xml",qdirtrabajo + "zz_Estados_Contables");
    copia_arch(":/estados/estados/BALANCE_PYMES_IS2018.est.xml",qdirtrabajo + "zz_Estados_Contables");
    copia_arch(":/estados/estados/balance_pymes_oficial.est.xml",qdirtrabajo + "zz_Estados_Contables");
    copia_arch(":/estados/estados/cuenta_PyG_abreviada_plan_2007.est.xml",qdirtrabajo + "zz_Estados_Contables");
    copia_arch(":/estados/estados/cuenta_PyG_plan_2007.est.xml",qdirtrabajo + "zz_Estados_Contables");
    copia_arch(":/estados/estados/CUENTA_PYG_PYMES_IS2018.est.xml",qdirtrabajo + "zz_Estados_Contables");
    copia_arch(":/estados/estados/cuenta_pyg_pymes_oficial.est.xml",qdirtrabajo + "zz_Estados_Contables");

    // comprobamos existencia de zz_Informes
    QDir dirinformes(qdirtrabajo +"zz_Informes");
    if (!dirinformes.exists()) dirinformes.mkdir(qdirtrabajo + "zz_Informes");
    // copiamos archivos
    copia_arch(":/informes/factura.xml",qdirtrabajo + "zz_Informes");
    copia_arch(":/informes/factura_ret.xml",qdirtrabajo + "zz_Informes");
    copia_arch(":/informes/factura_ret_suplidos.xml",qdirtrabajo + "zz_Informes");
    copia_arch(":/informes/factura_suplidos.xml",qdirtrabajo + "zz_Informes");

}


QString dirtrabajo(void)
 {
  QString nombreconex=basedatos::instancia()->nombre();
  // si es SQLITE nombreconex guarda toda la ruta
  if (es_sqlite()) nombreconex=nombreconex.mid(nombreconex.lastIndexOf(QDir::separator())+1);
  for (int veces=0; veces<conexion_rutas.count(); veces++)
    {
      if (conexion_rutas.at(veces)==nombreconex)
        {
          QString caddir=rutas_trabajo.at(veces);
          if (caddir.isEmpty()) break;
          if (!caddir.endsWith(QDir::separator())) caddir.append(QDir::separator());
          if (nombreconex.length()>0)
             {
              caddir.append(nombreconex);
              // if (es_sqlite()) qdirtrabajo=nombrebd(); // el nombre de la base de datos guarda toda la ruta
              caddir.append("-dat");
             }
          // if (eswindows()) qdirtrabajo=QFile::encodeName(qdirtrabajo);
          QDir d(caddir);
          if (!d.exists()) d.mkdir(caddir);

          //QMessageBox::warning( 0, QObject::tr("ruta"),caddir);

          return caddir ;
        }
    }
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  // if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString qdirtrabajo;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("DIRECTORIO")==0) qdirtrabajo=contenido;
              }
            }
        ficheroini.close();
       }
  if (qdirtrabajo.length()==0) qdirtrabajo=dirtrabajodefecto();
  QDir a(qdirtrabajo);
  qdirtrabajo.append(QDir::separator());
  if (!a.exists())
     {
      a.mkdir(qdirtrabajo);
      // comprobamos que exista la empresa de ejemplo SQLITE
      QString fichsqlite=qdirtrabajo+"ejemplo-sqlite";
      if (!QFile::exists(fichsqlite))
         {
            QString pasa2;
            pasa2=":/fich/fich/";
            //pasa2=traydoc();
            //pasa2.append(QDir::separator());
            pasa2.append("ejemplo-sqlite");
            // if (eswindows()) pasa2=QFile::encodeName(pasa2); ---------
            QFile fichero(pasa2);
            //if (eswindows()) fichero.copy(QFile::encodeName(fichsqlite));
            //    else fichero.copy(fichsqlite);
            fichero.copy(fichsqlite);
         }
      // comprobamos que exista el fichero de la memoria
      QString fichmemoria=qdirtrabajo+"memoria.tex";
      if (!QFile::exists(fichmemoria))
         {
            QString pasa2;
            pasa2=":/fich/fich/";
            //pasa2=traydoc();
            //pasa2.append(QDir::separator());
            pasa2.append("memoria.tex");
            // if (eswindows()) pasa2=QFile::encodeName(pasa2);
            QFile fichero(pasa2);
            //if (eswindows()) fichero.copy(QFile::encodeName(fichmemoria));
            //    else fichero.copy(fichmemoria);
            fichero.copy(fichmemoria);
         }
     }
  // -----------------------------------------------------
  if (nombrebd().length()>0)
     {
      qdirtrabajo.append(nombrebd());
      if (es_sqlite()) qdirtrabajo=nombrebd(); // el nombre de la base de datos guarda toda la ruta
      qdirtrabajo.append("-dat");
     }
  // if (eswindows()) qdirtrabajo=QFile::encodeName(qdirtrabajo);
  QDir d(qdirtrabajo);
  if (!d.exists()) d.mkdir(qdirtrabajo);
  return qdirtrabajo;
}

QString dirtrabajodocs(QString qdoc)
{
    QString dir=dirtrabajo();
    dir.append(QDir::separator());
    dir.append(qdoc);
    QDir d(dir);
    if (!d.exists()) d.mkdir(dir);
    return dir;
}


QString dirtrabajo_sii()
{
    QString dir=dirtrabajo();
    dir.append(QDir::separator());
    dir.append(QObject::tr("SII"));
    QDir d(dir);
    if (!d.exists()) d.mkdir(dir);
    return dir;
}


QString dirtrabajo_sii_env()
{
    QString dir=dirtrabajo();
    dir.append(QDir::separator());
    dir.append(QObject::tr("SII"));
    dir.append(QDir::separator());
    dir.append(QObject::tr("ENV"));
    QDir d(dir);
    if (!d.exists()) d.mkdir(dir);
    return dir;
}


QString dirtrabajo_certificados()
{
    QString dir=dirtrabajobd();
    dir.append(QDir::separator());
    dir.append(QObject::tr("CERTIFICADOS"));
    QDir d(dir);
    if (!d.exists()) d.mkdir(dir);
    return dir;
}


QString dirtrabajobd(void)
{

    QString nombreconex=basedatos::instancia()->nombre();
    if (es_sqlite()) nombreconex=nombreconex.mid(nombreconex.lastIndexOf(QDir::separator())+1);
    QString caddir;
    for (int veces=0; veces<conexion_rutas.count(); veces++)
      {
        if (conexion_rutas.at(veces)==nombreconex)
          {
            caddir=rutas_trabajo.at(veces);
            break;
          }
      }
  // QMessageBox::warning( 0, QObject::tr("ruta"),caddir);
  QString qdirtrabajo=caddir;

  if (caddir.isEmpty())
     {
      QString nombrefichcompleto=trayconfig();
      QString nombre="/keme.cfg";
      nombre[0]=QDir::separator();
      nombrefichcompleto+=nombre;
      //if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
      QFile ficheroini(nombrefichcompleto);

      if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("DIRECTORIO")==0) qdirtrabajo=contenido;
              }
            }
        ficheroini.close();
       }
     }

  if (qdirtrabajo.length()==0) qdirtrabajo=dirtrabajodefecto();
  //if (eswindows()) qdirtrabajo=QFile::encodeName(qdirtrabajo);
  QDir d(qdirtrabajo);
  if (!d.exists()) d.mkdir(qdirtrabajo);
  return qdirtrabajo;
  // return dirtrabajo();
}


int idiomaconfig(void)
 {
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  //if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  int idioma=0;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("IDIOMA")==0) idioma=contenido.toInt();
              }
            }
        ficheroini.close();
       }
  return idioma;
}

QString fich_catalan()
{
 QString nombre=":/translate/";
 nombre.append(QDir::separator());
 nombre+="keme4_cat";
 if (eswindows()) nombre=QFile::encodeName(nombre);
 return nombre;
}

QString fich_gallego()
{
 QString nombre=":/translate/";
 nombre.append(QDir::separator());
 nombre+="keme4_gl";
 if (eswindows()) nombre=QFile::encodeName(nombre);
 return nombre;
}


QString fich_ingles()
{
 QString nombre=":/translate/";
 nombre.append(QDir::separator());
 nombre+="keme4_eng";
 if (eswindows()) nombre=QFile::encodeName(nombre);
 return nombre;
}


QString fich_portugues()
{
 QString nombre=":/translate/";
 nombre.append(QDir::separator());
 nombre+="keme4_pt";
 if (eswindows()) nombre=QFile::encodeName(nombre);
 return nombre;
}

QString fich_euskera()
{
 QString nombre=":/translate/";
 nombre.append(QDir::separator());
 nombre+="keme4_eus";
 if (eswindows()) nombre=QFile::encodeName(nombre);
 return nombre;
}


QString filtracad(QString elorigen)
{
  QString origen=elorigen;
  origen.replace("\"","'"); // covertimos a comillas simples

  QString caracteres="ABCDEFGHIJKLMNÑOPQRSTUVWXYZabcdefghijklmnñopqrstuvwxyz 1234567890áéíóú"
                     "àèìòùäëïöüÁÉÍÓÚÀÈÌÒÙÄËÏÖÜçÇâêîôûÂÊÎÔÛ!$%&/()='?¡¿*-+_.:,;{}[]^<>ºª€~\\";
  QString destino;
  destino=origen;
  for (int veces=0; veces<origen.length(); veces++)
     {
       if (!caracteres.contains(origen[veces]))
           destino=destino.remove(origen[veces]);
     }
  destino.replace("\\","/");
  //destino.remove("º");
  //destino.remove("ª");
  destino.replace("#","\\#");
  destino.replace("$","\\$");
  destino.replace("%","\\%");
  destino.replace("^","\\^");
  destino.replace("&","\\&");
  destino.replace("{","\\{");
  destino.replace("}","\\}");
  destino.replace("~","\\~");
  destino.replace("_","\\_");
  destino.replace("<","\\flq");
  destino.replace(">","\\frq");
  destino.replace("€","\\texteuro ");
  return destino;
}


QString mcortaa(QString cadena, uint num)
{
    uint longitud=0;
    QString mays=cadena.toUpper();
    QString numeros="0123456789.,' ";
    float medias=0;
    for (int veces=0; veces<int(cadena.length()); veces++)
        {
	    longitud++;
	    if (cadena[veces]==mays[veces]
                && numeros.contains(cadena[veces])==0) 
	       {
		   medias=medias+0.5;
	       }
	    if (longitud+uint(medias)>=num)
	        {
    	         QString devuelve=cadena; devuelve.truncate(uint(veces));
                 return devuelve;
	        } 
	}
    return cadena;
}


QString programa_imprimir()
{

  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  
  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString qimprimir;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("IMPRIMIR")==0) qimprimir=contenido;
              }
            }
        ficheroini.close();
       }
  if (qimprimir.length()==0) qimprimir=programa_imprimir_defecto();
  return qimprimir;

}


QString programa_imprimir_defecto()
{
  if (es_os_x()) return "Preview";
  return "okular";
}


QString visordvidefecto()
{
  if (WINDOWS) return "yap";
  if (es_os_x()) return "Preview";
  return "okular";
}

QString visordvi()
{
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;

  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString qvisor;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("VISORDVI")==0) qvisor=contenido;
              }
            }
        ficheroini.close();
       }
  if (qvisor.length()==0) qvisor=visordvidefecto();
  return qvisor;

}


bool filtrartexaxtivado()
{
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;

  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString cnum;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("FILTRARTEX")==0) cnum=contenido;
              }
            }
        ficheroini.close();
       }
       // else { if ( WINDOWS ) return true; }
  if (cnum.length()==0) return false;

  return cnum.toInt();

}

QString adapta(QString cadena)
{
 //if (eswindows()) return QFile::encodeName(cadena);
 //  else return cadena;
 return cadena;
}

void filtratex(QString cadfich)
{

 QString qfichero=cadfich;
 QString fichmod=qfichero+"2";

 QFile escritura( adapta(fichmod) );

  if ( !escritura.open( QIODevice::WriteOnly ) ) return; 
  QTextStream streamescritura( &escritura );
  streamescritura.setEncoding(QStringConverter::Utf8);


 QFile lectura( adapta(qfichero)  );
 
   if ( lectura.open(  QIODevice::ReadOnly  ) )
      {
        QTextStream streamlectura( &lectura );
        streamlectura.setEncoding(QStringConverter::Utf8);
        QString linea;
         while ( !streamlectura.atEnd() )
	   {
	     linea = streamlectura.readLine(); // linea de texto, excluye '\n'
             if (linea.length()==0) continue;
             if (linea.contains("usepackage{ucs}")) continue;
             if (linea.contains("usepackage[utf8x]")) continue;
             linea.replace(QObject::tr("á"),QString("\\'a"));
             linea.replace(QObject::tr("é"),QString("\\'e"));
             linea.replace(QObject::tr("í"),QString("\\'i"));
             linea.replace(QObject::tr("ó"),QString("\\'o"));
             linea.replace(QObject::tr("ú"),QString("\\'u"));
             linea.replace(QObject::tr("Á"),QString("\\'A"));
             linea.replace(QObject::tr("É"),QString("\\'E"));
             linea.replace(QObject::tr("Í"),QString("\\'I"));
             linea.replace(QObject::tr("Ó"),QString("\\'O"));
             linea.replace(QObject::tr("Ú"),QString("\\'U"));
             linea.replace(QObject::tr("ñ"),QString("\\~n"));
             linea.replace(QObject::tr("Ñ"),QString("\\~N"));
             linea.replace(QObject::tr("à"),QString("\\`a"));
             linea.replace(QObject::tr("è"),QString("\\`e"));
             linea.replace(QObject::tr("ì"),QString("\\`i"));
             linea.replace(QObject::tr("ò"),QString("\\`o"));
             linea.replace(QObject::tr("ù"),QString("\\`u"));
             linea.replace(QObject::tr("À"),QString("\\`A"));
             linea.replace(QObject::tr("È"),QString("\\`E"));
             linea.replace(QObject::tr("Ì"),QString("\\`I"));
             linea.replace(QObject::tr("Ò"),QString("\\`O"));
             linea.replace(QObject::tr("Ù"),QString("\\`U"));
             linea.replace(QObject::tr("ã"),QString("\\~a"));
             linea.replace(QObject::tr("ẽ"),QString("\\~e"));
             linea.replace(QObject::tr("ĩ"),QString("\\~i"));
             linea.replace(QObject::tr("õ"),QString("\\~o"));
             linea.replace(QObject::tr("ũ"),QString("\\~u"));
             linea.replace(QObject::tr("Ã"),QString("\\~A"));
             linea.replace(QObject::tr("Ẽ"),QString("\\~E"));
             linea.replace(QObject::tr("Ĩ"),QString("\\~I"));
             linea.replace(QObject::tr("Õ"),QString("\\~O"));
             linea.replace(QObject::tr("Ũ"),QString("\\~U"));
             linea.replace(QObject::tr("â"),QString("\\^a"));
             linea.replace(QObject::tr("ê"),QString("\\^e"));
             linea.replace(QObject::tr("î"),QString("\\^i"));
             linea.replace(QObject::tr("ô"),QString("\\^o"));
             linea.replace(QObject::tr("û"),QString("\\^u"));
             linea.replace(QObject::tr("Â"),QString("\\^A"));
             linea.replace(QObject::tr("Ê"),QString("\\^E"));
             linea.replace(QObject::tr("Î"),QString("\\^I"));
             linea.replace(QObject::tr("Ô"),QString("\\^O"));
             linea.replace(QObject::tr("Û"),QString("\\^U"));
             linea.replace(QObject::tr("ç"),QString("\\c c"));
             linea.replace(QObject::tr("Ç"),QString("\\c C"));
             if (!linea.startsWith("%")) linea.replace(QObject::tr("%"),QString("\\%"));
             streamescritura << linea << "\n";
           }
        lectura.close();
       } else return;
  escritura.close();

 //queda copiar escritura en lectura y borrar escritura
 QDir directorio(adapta(dirtrabajo()));
 directorio.remove(adapta(qfichero));
 directorio.rename(adapta(fichmod),adapta(qfichero));

}



bool haycomadecimal()
{
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;

  QFile ficheroini(adapta(nombrefichcompleto));
  QString cnum;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("ESTILONUMERICO")==0) cnum=contenido;
              }
            }
        ficheroini.close();
       }
  if (cnum.length()==0) return true;

  return cnum.toInt();

}


bool haydecimales()
{
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;

  QFile ficheroini(adapta(nombrefichcompleto));
  QString cnum;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("SINDECIMALES")==0) cnum=contenido;
              }
            }
        ficheroini.close();
       }
  if (cnum.length()==0) return true;

  return !cnum.toInt();

}


QString splash_pref()
{
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;

  QFile ficheroini(adapta(nombrefichcompleto));
  QString cadena;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("SPLASH")==0) cadena=contenido;
              }
            }
        ficheroini.close();
       }

  return cadena;

}


QString formatea_redondeado_sep(double numero,bool comadecimal, bool decimales)
{
    numero=CutAndRoundNumberToNDecimals (numero, 2);
    return formateanumerosep(numero,comadecimal,decimales);
}

QString formatea_redondeado(double numero,bool comadecimal, bool decimales)
{
    numero=CutAndRoundNumberToNDecimals (numero, 2);
    return formateanumero(numero,comadecimal,decimales);
}

QString formateanumerosep(double numero,bool comadecimal, bool decimales)
{
 QString cad; 
 cad.setNum(numero,'f',2);
 if (comadecimal) cad=convacoma(cad);

 if (comadecimal)
     {
       if (decimales)
           return puntuanumerocomadec(cad);
        else
             {
               QString cad2=puntuanumerocomadec(cad);
               cad2.truncate(cad2.length()-3);
               return cad2;
             }
     }
     else
        {
          if (decimales)
              return puntuanumeropuntodec(cad);
           else
                {
                  QString cad2=puntuanumeropuntodec(cad);
                  cad2.truncate(cad2.length()-3);
                  return cad2;
                }
        }

}


bool cifiltrook(QString ci)
{
  if (ci.length()<3) return false;
  if (ci.length()>3 && ci.length()<6) return false;
  // Extraemos primer trozo
  QString descripcion,qnivel;
  int nnivel;
  if (ci.left(3)!=QString("???"))
   {
    if (buscaci(ci.left(3), &descripcion, &qnivel))
       {
        nnivel=qnivel.toInt();
        if (nnivel!=1) return false;
       }
      else return false;
    }

  if (ci.length()==3) return true;

  if (ci.mid(3,3)!=QString("???"))
    {
     if (buscaci(ci.mid(3,3), &descripcion, &qnivel))
       {
        nnivel=qnivel.toInt();
        if (nnivel!=2) return false;
       }
      else return false;
     }

  if (ci.length()==6) return true;

  if (ci[6]!='*' && ci.length()!=7)
   {
    if (buscaci(ci.right(ci.length()-6),&descripcion,&qnivel))
     {
       nnivel=qnivel.toInt();
       if (nnivel!=3) return false;
     }
     else return false;
   }

 return true;
}


QString filtroci(QString codigo, bool diario_ci)
{
    QString filtro;
    QString adon;
    if (diario_ci) adon="diario_ci.";

    if (!codigo.startsWith("???") && codigo.length()>2)
    {
      if (codigo.length()==3)
        filtro=adon+"ci='"+codigo.replace("'","''")+"'";
      else
       {
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            filtro = adon+"ci like '"+ codigo.left(3).replace("'","''") +"%'";
        }
        else {
            filtro = "position('"+ codigo.left(3).replace("'","''") +"' in "+adon+"ci)=1";
        }
       }
    }

    if (codigo.startsWith("???") && codigo.length()==3)
    {
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            filtro = "length("+adon+"ci)=3";
        }
        else {
            filtro = "char_length("+adon+"ci)=3";
        }
    }


    if (codigo.length()>5 && codigo.mid(3,3)!=QString("???"))
    {
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            if (filtro.length()>0) filtro += " and ";
            else filtro = "";
            filtro += adon+"ci like '___"+ codigo.mid(3,3).replace("'","''") +"%'";
        }
        else {
            if (filtro.length()>0) filtro += " and position('";
            else filtro = "position('";
            filtro += codigo.mid(3,3).replace("'","''") +"' in "+adon+"ci)=4";
        }
    }

    if (codigo.length()==6)
    {
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            if (filtro.length()>0) filtro += " and ";
            filtro += "length("+adon+"ci)=6";
        }
        else {
            if (filtro.length()>0) filtro += " and ";
            filtro += "char_length("+adon+"ci)=6";
        }
    }

    if (codigo.length()>6 && codigo[6]!='*')
    {
        if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
            if (filtro.length()>0) filtro += " and ";
            filtro+= "substr("+adon+"ci,7,length("+adon+"ci)-6)='";
            filtro+= codigo.right(codigo.length()-6).replace("'","''");
            filtro+="'";
            // filtro += adon+"ci like '______"+ codigo.right(codigo.length()-6).replace("'","''") +"%'";
        }
        else {
            if (filtro.length()>0) filtro += " and ";
            filtro+="char_length("+adon+"ci)>5 and ";
            filtro+="substring("+adon+"ci from 7 for char_length("+adon+"ci)-6)='";
            filtro+=codigo.right(codigo.length()-6).replace("'","''");
            filtro+="'";
            /*
            filtro += "position('";
            filtro += codigo.right(codigo.length()-6).replace("'","''");
            filtro += "' in "+adon+"ci)=7";*/
        }
     }

    if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
        if (filtro.length()==0) filtro = "length("+adon+"ci)>0";
    }
    else {
        if (filtro.length()==0) filtro = "char_length("+adon+"ci)>0";
    }

    return filtro;
}


bool parentesisbalanceados(QString cadena)
{
  evaluadorparentesis evparentesis;
  evparentesis.inicializa(cadena);
  if (!evparentesis.okparentesis()) return 0;
  return 1;
}

QString cadenamedia()
{
 return QObject::tr("MEDIA");
}

QString cadpositivo()
{
  return QObject::tr(";POSITIVO");
}


QString tipoNumerico()
{
  return QObject::tr("Numérico");
}

QString tipoFecha()
{
  return QObject::tr("Fecha");
}

QString tipoCaracteres()
{
  return QObject::tr("Caracteres");
}

QString tipoSubcuenta()
{
  return QObject::tr("Subcuenta");
}

QString tipoNIF()
{
  return QObject::tr("NIF");
}


QString tipoCI()
{
  return QObject::tr("CI");
}

QString tipoCALC()
{
  return QObject::tr("Calculado");
}

QString tipoDEBE()
{
  return QObject::tr("DEBE");
}

QString tipoHABER()
{
  return QObject::tr("HABER");
}

QString tipoConceptoSII()
{
    return QObject::tr("Concepto_SII");
}

bool chequeaformula(QString formula)
{
  if (formula.length()==0) return 1;
  if (!parentesisbalanceados(formula)) return 0;
  if (formula.contains("(+")) return 0;
  if (formula.contains("(/")) return 0;
  if (formula.contains("(*")) return 0;
  if (formula.contains("/)")) return 0;
  if (formula.contains("*)")) return 0;
  if (formula.contains("+)")) return 0;
  if (formula.contains("-)")) return 0;
  if (formula.contains("()")) return 0;
  if (formula.contains(")(")) return 0;
  if (formula.contains("++")) return 0;
  if (formula.contains("+-")) return 0;
  if (formula.contains("+*")) return 0;
  if (formula.contains("+/")) return 0;
  if (formula.contains("--")) return 0;
  if (formula.contains("-+")) return 0;
  if (formula.contains("-*")) return 0;
  if (formula.contains("-/")) return 0;
  if (formula.contains("**")) return 0;
  if (formula.contains("*+")) return 0;
  if (formula.contains("*/")) return 0;
  if (formula.contains("//")) return 0;
  if (formula.contains("/+")) return 0;
  if (formula.contains("/*")) return 0;
  if (formula.endsWith("+")) return 0;
  if (formula.endsWith("-")) return 0;
  if (formula.endsWith("/")) return 0;
  if (formula.endsWith("*")) return 0;
  if (formula.endsWith("(")) return 0;
  
  if (formula.contains("][")) return 0;
  if (formula.contains("[[")) return 0;
  if (formula.contains("]]")) return 0;
  if (formula.contains("[]")) return 0;
    
  if (formula.indexOf(']',0)<formula.indexOf('[',0)) return 0;
  
  int indice=0;
  while (indice<formula.length() && formula.indexOf('[',indice)>=0)
    {
             int base=formula.indexOf('[',indice);
             if (formula.indexOf(']',indice)<0) return 0;
             if (base+1<formula.length() && formula.indexOf('[',base+1)>=0)
                  if (formula.indexOf('[',base+1)<formula.indexOf(']',base)) return 0; 
             indice=formula.indexOf(']',indice)+1;
             if (indice<formula.length() && formula.indexOf(']',indice)>=0 && 
             	 (formula.indexOf(']',indice)<formula.indexOf('[',indice) || formula.indexOf('[',indice)<0))
                 return 0;
    }

// hacer lo propio con las llaves  
  if (formula.contains("}{")) return 0;
  if (formula.contains("{{")) return 0;
  if (formula.contains("}}")) return 0;
  if (formula.contains("{}")) return 0;

  if (formula.indexOf('}',0)<formula.indexOf('{',0)) return 0;
  
  indice=0;
  while (indice<formula.length() && formula.indexOf('{',indice)>=0)
    {
      int base=formula.indexOf('{',indice);
      if (formula.indexOf('}',indice)<0) return 0;
      if (base+1<formula.length() &&formula.indexOf('{',base+1)>=0)
	  if (formula.indexOf('{',base+1)<formula.indexOf('}',base)) return 0; 
      indice=formula.indexOf('}',indice)+1;
      if (indice<formula.length() && formula.indexOf('}',indice)>=0 && 
	  (formula.indexOf('}',indice)<formula.indexOf('{',indice) || formula.indexOf('{',indice)<0)) return 0;
    }

// -------------------------------------------------------------  
  QString cadpru;
  cadpru=formula;
  int final;
  int inicio;
  indice=0;
  while (indice<cadpru.length())
      {
         if (cadpru[indice]=='[')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!=']') final++;
              cadpru.remove(inicio,final-inicio+1);
              continue;
           }
        if (cadpru[indice]=='{')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!='}') final++;
              cadpru.remove(inicio,final-inicio+1);
              continue;
           }
       indice++;
      }
    
  QString cadcods="()+-/*0123456789.?:<=!>siSI";
  
  indice=0;
  cadpru.remove(' ');
  while (indice<cadpru.length())
      {
        if (!cadcods.contains(cadpru[indice])) return 0;
        indice++;
      }
 
  return 1;   

}

void cadvalorestipoiva(QString cadena,QString *tipo,QString *re)
{
    basedatos::instancia()->selectTiporetiposivaclave(cadena,tipo,re);
}

bool cadnumvalido(QString cadnum)
{
  QString num;
  num=cadnum.trimmed();
  QString digitos="0123456789.-";
  for (int veces=0;veces<num.length();veces++)
    {
      if (!digitos.contains(num[veces])) return false;
    }
  if (num.count('.')>1) return false;
  if (num.count('-')>1) return false;
  if (num.count('-')==1 && num.indexOf('-')>0) return false;
  return true;
}

double tipoivaefectivo(QString clave)
{
    QString qtipo, qre;
    double tipo=0,re=0;

    if ( basedatos::instancia()->selectTiporetiposivaclave(clave,&qtipo,&qre) ) {
        tipo = qtipo.toDouble();
        re = qre.toDouble();
        return ( (tipo+re)/100 );
    }
    return 0;
}

double tipoivanore(QString clave)
{
    QString qtipo, qre;
    double tipo=0;

    if ( basedatos::instancia()->selectTiporetiposivaclave(clave,&qtipo,&qre) ) {
        tipo = qtipo.toDouble();
        return ( tipo/100 );
    }
    return 0;
}


bool cadfechavalida(QString cadfecha)
{
  // formato dd/mm/aaaa

  // 1 comprobamos longitud
  if (cadfecha.length()!=10) return false;

  // comprobamos separadores
  if (cadfecha[2]!='/' && cadfecha[2]!='-' &&cadfecha[2]!='.') return false;

  if (cadfecha[2]=='/' && cadfecha[5]!='/') return false;
  if (cadfecha[2]=='-' && cadfecha[5]!='-') return false;
  if (cadfecha[2]=='.' && cadfecha[5]!='.') return false;

  // comprobaciones básicas
  // en las demás posiciones deben de haber números
  QString numeros="0123456789";
  if (!numeros.contains(cadfecha[0])) return false;
  if (!numeros.contains(cadfecha[1])) return false;
  if (!numeros.contains(cadfecha[3])) return false;
  if (!numeros.contains(cadfecha[4])) return false;
  if (!numeros.contains(cadfecha[6])) return false;
  if (!numeros.contains(cadfecha[7])) return false;
  if (!numeros.contains(cadfecha[8])) return false;
  if (!numeros.contains(cadfecha[9])) return false;

  // días, meses y años han de ser enteros positivos
  int dia,mes,anyo;
  QString cadextract;

  cadextract=cadfecha.mid(0,2);
  dia=cadextract.toInt();

  cadextract=cadfecha.mid(3,2);
  mes=cadextract.toInt();

  cadextract=cadfecha.mid(6,4);
  anyo=cadextract.toInt();
  
  if (QDate::isValid(anyo,mes,dia)) return true;
    else return false;

}


bool cuentaycodigoenplanamort(QString cuenta,QString codigo)
{
    return basedatos::instancia()->cuentaycodigoenplanamort(cuenta,codigo);
}


bool practamort(QString ejercicio)
{
    return basedatos::instancia()->practamort(ejercicio);
}

void actualizasaldo(QString cuenta,QString importe,bool cargo)
{
    basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(cuenta,importe,cargo);
}


double saldocuentaendiario(QString cuenta)
{
    return basedatos::instancia()->selectDebehaberdiariocuenta(cuenta);
}


double totalamortfiscal(QString cuenta)
{
    return basedatos::instancia()->selectSumDotacion_fiscalamortfiscalycontablecuenta_activo(cuenta);
}

QString ciamort()
{
 return QObject::tr("AMR");
}


bool espasevtoprocesado(QString qpase)
{
    return basedatos::instancia()->espasevtoprocesado(qpase);
}


QString ejercicio_igual_o_maxigual(QDate fecha)
{
    return basedatos::instancia()->ejercicio_igual_o_maxigual(fecha);
}

QString numasientovto(QString vto)
{   
    return basedatos::instancia()->numasientovto(vto);
}

QString numasientovtovto(QString vto)
{
    return basedatos::instancia()->numasientovtovto(vto);
}


bool vencimientoprocesado(QString vto)
{
    return basedatos::instancia()->selectPendientevencimientosnum(vto);
}


bool existevencimiento(QString vto)
{
    return basedatos::instancia()->existevencimiento(vto);
}

void anulavencimiento(QString vto)
{
    basedatos::instancia()->updateVencimientosanuladonum(vto,true);
}


void borravenci(QString vto)
{
    basedatos::instancia()->deleteVencimientosnum(vto);
}


QString cadmes(int mes)
{
  switch (mes)
    {
      case 1 : return QObject::tr("ENERO");
         break;
      case 2 : return QObject::tr("FEBRERO");
         break;
      case 3 : return QObject::tr("MARZO");
         break;
      case 4 : return QObject::tr("ABRIL");
         break;
      case 5 : return QObject::tr("MAYO");
         break;
      case 6 : return QObject::tr("JUNIO");
         break;
      case 7 : return QObject::tr("JULIO");
         break;
      case 8 : return QObject::tr("AGOSTO");
         break;
      case 9 : return QObject::tr("SEPTIEMBRE");
         break;
      case 10 : return QObject::tr("OCTUBRE");
         break;
      case 11 : return QObject::tr("NOVIEMBRE");
         break;
      case 12 : return QObject::tr("DICIEMBRE");
         break;
    }
 return "";
}

QString cadmes_minus(int mes)
{
  switch (mes)
    {
      case 1 : return QObject::tr("enero");
         break;
      case 2 : return QObject::tr("febrero");
         break;
      case 3 : return QObject::tr("marzo");
         break;
      case 4 : return QObject::tr("abril");
         break;
      case 5 : return QObject::tr("mayo");
         break;
      case 6 : return QObject::tr("junio");
         break;
      case 7 : return QObject::tr("julio");
         break;
      case 8 : return QObject::tr("agosto");
         break;
      case 9 : return QObject::tr("septiembre");
         break;
      case 10 : return QObject::tr("octubre");
         break;
      case 11 : return QObject::tr("noviembre");
         break;
      case 12 : return QObject::tr("diciembre");
         break;
    }
 return "";
}

QString fechaliteral(QDate fecha)
{
 QString cadfecha,cadnum;
 cadnum.setNum(fecha.day());
 cadfecha=cadnum+" de ";
 cadfecha+=cadmes_minus(fecha.month());
 cadfecha+=" de ";
 cadnum.setNum(fecha.year());
 cadfecha+=cadnum;
 return cadfecha;
}

void borraasientomodelo(QString asiento)
{
    basedatos::instancia()->deleteDet_as_modeloasientomodelo(asiento);
    basedatos::instancia()->deleteVar_as_modeloasientomodelo(asiento);
    basedatos::instancia()->deleteCab_as_modeloasientomodelo(asiento);
}

void fijapermisos(QString ruta, QString permisos)
{
    //if (WINDOWS) return;

    QStringList arguments;
    arguments << permisos;
    arguments << ruta;
    QProcess *myProcess = new QProcess(NULL);
    if (WINDOWS) myProcess->start("attrib",arguments);
      else
        myProcess->start("chmod",arguments);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return ;
       }
    if (!myProcess->waitForFinished (1000))
       {
         delete myProcess;
         return ;
       }
}

QString cad_tiempo()
{
    QString nombrefichero;
    QDate fecha;
    nombrefichero+="-";
    nombrefichero+=fecha.currentDate().toString("yy.MM.dd");
    nombrefichero+="-";
    QTime hora;
    hora=hora.currentTime();
    nombrefichero+=hora.toString("hhmmss");
    return nombrefichero;
}

int imprimelatex(QString qfichero)
{
    if (WINDOWS) return consultalatex(qfichero);

    QProgressDialog progreso(QObject::tr("Generando informe ... ")
                             , 0, 0, 0);
    // progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(QObject::tr("Generando  ... "));;
    progreso.show();
    progreso.update();
    QApplication::processEvents();
    QApplication::processEvents();

    QString pasafich=dirtrabajo();
    pasafich.append(QDir::separator());
    pasafich.append(qfichero);
    pasafich.append(".tex");
    if (filtrartexaxtivado()) filtratex(pasafich);

    QString rutapdf=rutacopiapdf();

    QString fichero=qfichero;

    QString cadexec="pdflatex";
    if (es_os_x()) cadexec="/Library/TeX/texbin/xelatex";
    QString cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;

    QObject *parent=NULL;

     QStringList arguments;
     arguments << adapta(cadarg);

     QProcess *myProcess = new QProcess(parent);

     myProcess-> setWorkingDirectory(adapta(dirtrabajo()));

     QApplication::processEvents();

     myProcess->start(cadexec,arguments);

     QApplication::processEvents();

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished (60000))
       {
         delete myProcess;
         return 1;
       }

    arguments.clear();

    QApplication::processEvents();

    // ---------------------------------------------------------------
    // borramos aux y log
    QString fich_aux=cadarg+".aux";
    QString fich_log=cadarg+".log";

    QFile faux( fich_aux);
    faux.remove();
    QFile flog( fich_log);
    flog.remove();
    // ---------------------------------------------------------------


 /*
    cadexec="dvips";
    arguments << "-Pcmz";
    cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;
    cadarg+".dvi";
    arguments << adapta(cadarg);
    arguments << "-o";
    QString cadfich=qfichero; cadfich.append(".ps");
    arguments << cadfich;
    myProcess->start(cadexec,arguments);

    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 2;
       }
    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 2;
       }

*/
    arguments.clear();
    // cadexec=programa_imprimir();
    QString qfich;
    qfich=dirtrabajo();
    qfich.append(QDir::separator());
    qfich+=fichero;
    qfich+=".pdf";
 /*   if (programa_imprimir()=="kprinter")
        {
         arguments << "--caption";
         arguments << "KEME-Contabilidad";
         cadarg=dirtrabajo();
         cadarg.append(QDir::separator());
         cadarg+=fichero;
         cadarg+=".ps";
         arguments << adapta(cadarg);
         // arguments << "&";
         myProcess->start(cadexec,arguments);
         if (!myProcess->waitForStarted ())
           {
            delete myProcess;
            return 3;
           }
         myProcess->waitForFinished(-1);
         delete myProcess;
         return 0;
        }
     else */

    QApplication::processEvents();

     if (rutapdf.isEmpty() || pref_forzar_ver_pdf())
        {
          /* if (es_os_x()) cadexec="open";
          cadexec+=" ";
          cadexec+=qfich;
          if (! myProcess->startDetached(cadexec))
            {
             delete myProcess;
             return 3;
            }*/
          QString url=qfich;
          QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

        }

  delete myProcess;

  QApplication::processEvents();

  // copiar aquí pdf
  QString rpdf=rutapdf;
  if (!rpdf.isEmpty())
     {
      // qWarning(rpdf.toAscii().constData());
      // qWarning(qfich.toAscii().constData());
      QString destino=rpdf;
      destino.append(QDir::separator());
      destino.append(fichero);
      destino.append(cad_tiempo());
      destino.append(".pdf");
      QFile fich(qfich);
      //if (eswindows()) fich.copy(QFile::encodeName(destino));
      //   else fich.copy(destino);
      fich.copy(destino);
     }
  return 0;

}



int imprimelatex2(QString qfichero)
{
    if (WINDOWS) return consultalatex2(qfichero);

    QProgressDialog progreso(QObject::tr("Generando informe... ")
                             , 0, 0, 0);
    // progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(QObject::tr("Generando  ... "));;
    progreso.show();
    progreso.update();
    QApplication::processEvents();

    QString pasafich=dirtrabajo();
    pasafich.append(QDir::separator());
    pasafich.append(qfichero);
    pasafich.append(".tex");
    if (filtrartexaxtivado()) filtratex(pasafich);

    QString fichero=qfichero;

    QString cadexec="pdflatex";
    if (es_os_x()) cadexec= "/Library/TeX/texbin/xelatex";
    QString cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;

    QObject *parent=NULL;

     QStringList arguments;
     arguments << cadarg;

     QProcess *myProcess = new QProcess(parent);

     myProcess-> setWorkingDirectory(dirtrabajo());

     QApplication::processEvents();

     myProcess->start(cadexec,arguments);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished (60000))
       {
         delete myProcess;
         return 1;
       }

     //segunda pasada
     myProcess->start(cadexec,arguments);
     QApplication::processEvents();

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished (60000))
       {
         delete myProcess;
         return 1;
       }

    QApplication::processEvents();

    // ---------------------------------------------------------------
    // borramos aux y log
    QString fich_aux=cadarg+".aux";
    QString fich_log=cadarg+".log";

    QFile faux(eswindows() ? QFile::encodeName(fich_aux): fich_aux);
    faux.remove();
    QFile flog(eswindows() ? QFile::encodeName(fich_log) : fich_log);
    flog.remove();
    // ---------------------------------------------------------------


/*    arguments.clear();
    cadexec="dvips";
    arguments << "-Pcmz";
    cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;
    cadarg+".dvi";
    arguments << cadarg;
    arguments << "-o";
    QString cadfich=qfichero; cadfich.append(".ps");
    arguments << cadfich;
    myProcess->start(cadexec,arguments);

    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 2;
       }
    if (!myProcess->waitForFinished (60000))
       {
         delete myProcess;
         return 2;
       }
*/
    QApplication::processEvents();

    arguments.clear();
    //cadexec=programa_imprimir();
    QString qfich;
    qfich=dirtrabajo();
    qfich.append(QDir::separator());
    qfich+=fichero;
    qfich+=".pdf";
/*    if (programa_imprimir()=="kprinter")
        {
         arguments << "--caption";
         arguments << "KEME-Contabilidad";
         cadarg=dirtrabajo();
         cadarg.append(QDir::separator());
         cadarg+=fichero;
         cadarg+=".ps";
         arguments << adapta(cadarg);
         // arguments << "&";
         myProcess->start(cadexec,arguments);
         if (!myProcess->waitForStarted ())
           {
            delete myProcess;
            return 3;
           }
         myProcess->waitForFinished(-1);
         delete myProcess;
         return 0;
        }
     else */
    QApplication::processEvents();

    QString rpdf=rutacopiapdf();
     if (rpdf.isEmpty() || pref_forzar_ver_pdf())
        {
          /*if (es_os_x()) cadexec="open";
          cadexec+=" ";
          cadexec+=qfich;
          if (! myProcess->startDetached(cadexec))
            {
             delete myProcess;
             return 3;
            }*/
          QString url=qfich;
          QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
        }

     QApplication::processEvents();

    if (!rpdf.isEmpty())
       {
        QFile fich(qfich);
        QString destino=rpdf;
        destino.append(QDir::separator());
        destino.append(fichero);
        destino.append(cad_tiempo());
        destino.append(".pdf");
        if (eswindows()) fich.copy(QFile::encodeName(destino));
           else fich.copy(destino);

       }

    QApplication::processEvents();

  delete myProcess;
  return 0;
}


void desanulavencimiento(QString vto)
{
    basedatos::instancia()->updateVencimientosanuladonum(vto,false);
}


int existeclaveiva(QString cadena,QString *qtipo,QString *qre,QString *qdescrip)
{
    return basedatos::instancia()->selectTodoTiposivaclave(cadena,qtipo,qre,qdescrip);
}

void guardacambiostiposiva(QString clave,QString tipo,QString re,QString descripcion)
{
    basedatos::instancia()->updateTiposivatiporedescripcionclave(clave,tipo,re,descripcion);
}

void insertatipoiva(QString clave,QString tipo, QString re, QString descripcion)
{
    basedatos::instancia()->insertTiposiva(clave,tipo,re,descripcion);
}

void eliminatipoiva(QString qclave)
{
    basedatos::instancia()->deleteTiposivaclave(qclave);
}

/* QString nifcomunitariocta(QString codigo)
{
    return basedatos::instancia()->selectNifcomunitariodatossubcuentacuenta(codigo);
}
*/

QString cifcta(QString codigo)
{
    return basedatos::instancia()->selectCifdatossubcuentacuenta(codigo);
}


QString ruta_java()
{
  // return "qrc:/java/java";

  if (eswindows())
    {
      QString ruta;
      //ruta=windirprogram();
      //return adapta( ruta+"\\keme4\\java");
      ruta=trayconfig();
      return adapta (ruta+"\\report");

    }
  QString ruta=trayconfig();
  ruta+="/report";
  return ruta;// "/usr/share/java/keme";
}

void informe_jasper_xml(QString ficherojasper, QString ficheroxml,
                        QString expresionxpath, QString fichpdf,
                        QString ruta_graf)
{

    // QString cadexec="java -jar /usr/share/java/keme/jasp_xml2pdf.jar ";

    QString cadexec="java"; // /usr/share/java/keme/jasp_xml2pdf.jar ";
    QStringList args;
    args << "-jar";
    args << ruta_java()+QDir::separator()+"jasp_xml2pdf.jar";

    // QMessageBox::warning( 0, QObject::tr("Generar informe"),ruta_java());

    QObject *parent=NULL;

   if (eswindows())
       {
        ficherojasper=adapta(ficherojasper);
        ficheroxml=adapta(ficheroxml);
        fichpdf=adapta(fichpdf);
        ruta_graf=adapta(ruta_graf);
       }
/*     cadexec+=ficherojasper;
     cadexec+=" ";
     cadexec+=ficheroxml;
     cadexec+=" ";
     cadexec+= expresionxpath;
     cadexec+=" ";
     cadexec+= fichpdf;
     cadexec+=" ";
     cadexec+=ruta_graf;*/

    args << ficherojasper;
    args << ficheroxml;
    args << expresionxpath;
    args << fichpdf;
    args << ruta_graf;

    // QMessageBox::information( 0, QObject::tr("Generar informe"),cadexec + " -jar " +
    //                          ruta_java()+QDir::separator()+"jasp_xml2pdf.jar "+
    //                      ficherojasper + " " + ficheroxml + " " + expresionxpath + " "
    //                      + fichpdf + " "+ ruta_graf);

     QProcess *myProcess = new QProcess(parent);

     // myProcess->setWorkingDirectory(adapta(dirtrabajo()));

     myProcess->start(cadexec,args);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         QMessageBox::warning( 0, QObject::tr("Generar informe"),
                               QObject::tr("Problemas procesando archivo XML"));
         delete myProcess;
         return ;
       }
    if (!myProcess->waitForFinished (300000))
       {
        QMessageBox::warning( 0, QObject::tr("Generar informe"),
                              QObject::tr("Problemas procesando archivo XML"));
         delete myProcess;
         return ;
       }

    delete myProcess;

    QString rpdf=rutacopiapdf();
    QStringList arguments; arguments<<fichpdf;
   if (rpdf.isEmpty() || pref_forzar_ver_pdf())
     {
       if (WINDOWS)
         {
           #ifdef _WIN32
           QT_WA({

           ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

           } , {

           ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

           });
           #endif
           #ifdef _WIN64
           QT_WA({

           ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

           } , {

           ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

           });
           #endif
         }
       else
        {
         QString url=fichpdf;
         QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

         //if (!ejecuta(aplicacionabrirfich(extensionfich(fichpdf)),fichpdf))
         // QMessageBox::warning( 0, QObject::tr("FACTURA EN PDF"),
         //                     QObject::tr("No se puede abrir ")+fichpdf+QObject::tr(" con ")+
         //                     aplicacionabrirfich(extensionfich(fichpdf)) + "\n" +
         //                     QObject::tr("Verifique el diálogo de preferencias"));
        }
      }
    if (!rpdf.isEmpty())
           { // vamos a copiar el pdf al destino
            QFile fich(fichpdf);
            QString destino=rpdf;
            destino.append(QDir::separator());
            QString nombrefich=fichpdf.mid(fichpdf.lastIndexOf(QDir::separator())+1);
            destino.append(nombrefich);
            destino.remove(".pdf");
            destino.append(cad_tiempo());
            destino.append(".pdf");
            // destino.append(".pdf");
            //if (eswindows()) fich.copy(QFile::encodeName(destino));
            //  else fich.copy(destino);
            fich.copy(destino);
         }
}


int consultalatex(QString qfichero)
{
    QProgressDialog progreso(QObject::tr("Generando informe ... ")
                             , 0, 0, 0);
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(QObject::tr("Generando  ... "));;
    progreso.show();
    progreso.update();
    QApplication::processEvents();


    QString pasafich=dirtrabajo();
    pasafich.append(QDir::separator());
    pasafich.append(qfichero);
    pasafich.append(".tex");
    if (filtrartexaxtivado()) filtratex(pasafich);

    QString fichero=qfichero;

    QString cadexec;
    // if (WINDOWS) cadexec="latex";
    //    else cadexec="pdflatex";
    cadexec="pdflatex";
    if (es_os_x()) cadexec="/Library/TeX/texbin/xelatex";
    QString cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;

    QObject *parent=NULL;

     QStringList arguments;
     arguments << adapta(cadarg);

     QProcess *myProcess = new QProcess(parent);

     myProcess-> setWorkingDirectory(adapta(dirtrabajo()));

     QApplication::processEvents();

     myProcess->start(cadexec,arguments);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished (60000))
       {
         delete myProcess;
         return 1;
       }


    QApplication::processEvents();

    // ---------------------------------------------------------------
    // borramos aux y log
    QString fich_aux=cadarg+".aux";
    QString fich_log=cadarg+".log";

    QFile faux( fich_aux);
    faux.remove();
    QFile flog( fich_log);
    flog.remove();
    // ---------------------------------------------------------------

    QString rpdf=rutacopiapdf();
    /*
    if (eswindows() && !rpdf.isEmpty())
       {
        QString cadexec2="pdf";
        cadexec2+=cadexec;
        myProcess->start(cadexec2,arguments);
        if (!myProcess->waitForStarted ())
           {
             delete myProcess;
             return 1;
           }
        if (!myProcess->waitForFinished (60000))
           {
             delete myProcess;
             return 1;
           }

       }*/

    arguments.clear();

    QApplication::processEvents();

    cadexec=visordvi();
    if (WINDOWS)cadexec="start";
    if (es_os_x()) cadexec="open";
    cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;
    QString cadfichpdf=cadarg+".pdf";
    /*if (WINDOWS) cadarg+=".dvi";
       else cadarg+=".pdf";*/
    cadarg+=".pdf";
    arguments << adapta(cadarg);
    // arguments << "&";

    QApplication::processEvents();

    if (rpdf.isEmpty() || pref_forzar_ver_pdf())
     {
        if (WINDOWS)
          {
            #ifdef _WIN32
            QT_WA({

            ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

            } , {

            ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

            });
            #endif
            #ifdef _WIN64
            QT_WA({

            ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

            } , {

            ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

            });
            #endif
          }
        else
         {
            QString url=cadfichpdf;
            QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

            /*if (! myProcess->startDetached(cadexec,arguments)) // cadexec,arguments
            {
             delete myProcess;
             return 2;
            }*/
         }
     }

    delete myProcess;

    QApplication::processEvents();

    if (!rpdf.isEmpty())        
       {
        //
        QFile fich(cadfichpdf);
        QString destino=rpdf;
        destino.append(QDir::separator());
        destino.append(fichero);
        destino.append(cad_tiempo());
        destino.append(".pdf");
        //if (eswindows()) fich.copy(QFile::encodeName(destino));
        //   else fich.copy(destino);
        fich.copy(destino);
       }

    QApplication::processEvents();

  return 0;
}




int consultalatex2(QString qfichero)
{
    QProgressDialog progreso(QObject::tr("Generando informe ... ")
                             , 0, 0, 0);
    progreso.setWindowModality(Qt::WindowModal);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(QObject::tr("Generando  ... "));;
    progreso.show();
    progreso.update();
    QApplication::processEvents();

    QString pasafich=dirtrabajo();
    pasafich.append(QDir::separator());
    pasafich.append(qfichero);
    pasafich.append(".tex");
    if (filtrartexaxtivado()) filtratex(pasafich);
    QApplication::processEvents();

    QString fichero=qfichero;

    QString cadexec;
    /*if (WINDOWS) cadexec="latex";
       else cadexec="pdflatex";*/
    cadexec="pdflatex";
    if (es_os_x()) cadexec="/Library/TeX/texbin/xelatex";
    QString cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;

    QObject *parent=NULL;

     QStringList arguments;
     arguments << adapta(cadarg);

     QProcess *myProcess = new QProcess(parent);

     myProcess-> setWorkingDirectory(dirtrabajo());

     QApplication::processEvents();

     myProcess->start(cadexec,arguments);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 1;
       }

    QApplication::processEvents();

     myProcess->start(cadexec,arguments);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 1;
       }

    QApplication::processEvents();

    // si es Windows,  se ha generado el pdf


    // ---------------------------------------------------------------
    // borramos aux y log
    QString fich_aux=cadarg+".aux";
    QString fich_log=cadarg+".log";

    QFile faux( fich_aux);
    faux.remove();
    QFile flog( fich_log);
    flog.remove();
    // ---------------------------------------------------------------


    QString rpdf=rutacopiapdf();
    /*
    if (eswindows() && !rpdf.isEmpty())
       {
        QString cadexec2="pdf";
        cadexec2+=cadexec;
        myProcess->start(cadexec2,arguments);
        if (!myProcess->waitForStarted ())
           {
             delete myProcess;
             return 1;
           }
        if (!myProcess->waitForFinished (60000))
           {
             delete myProcess;
             return 1;
           }

       }
     */
    QApplication::processEvents();

    arguments.clear();
    cadexec=visordvi();
    if (WINDOWS) cadexec="start";
    if (es_os_x()) cadexec="open";

    cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;
    QString cadfichpdf=cadarg+".pdf";
    /*if (WINDOWS) cadarg+=".dvi";
       else cadarg+=".pdf";*/
    cadarg+=".pdf";
    arguments << adapta(cadarg);
    // arguments << "&";

    QApplication::processEvents();

    if (rpdf.isEmpty() || pref_forzar_ver_pdf())
      {
        if (WINDOWS)
          {
            #ifdef _WIN32
            QT_WA({

            ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

            } , {

            ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

            });
            #endif
            #ifdef _WIN64
            QT_WA({

            ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

            } , {

            ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

            });
            #endif
          }
        else
         {
            QString url=cadfichpdf;
            QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
           /*
           if (! myProcess->startDetached(cadexec,arguments)) // cadexec,arguments
            {
             delete myProcess;
             return 2;
            }*/
         }
      }

    delete myProcess;

    QApplication::processEvents();

    if (!rpdf.isEmpty())
       {
        //
        QFile fich(cadfichpdf);
        QString destino=rpdf;
        destino.append(QDir::separator());
        destino.append(fichero);
        destino.append(cad_tiempo());
        destino.append(".pdf");
        //if (eswindows()) fich.copy(QFile::encodeName(destino));
        //   else fich.copy(destino);
        fich.copy(destino);
       }

    QApplication::processEvents();

  return 0;
}


int consultalatex2fichabs(QString qfichero)
{
    QProgressDialog progreso(QObject::tr("Generando informe ... ")
                             , 0, 0, 0);
    progreso.setMinimumDuration ( 0 );
    progreso.setWindowTitle(QObject::tr("Generando  ... "));;
    progreso.show();
    progreso.update();
    QApplication::processEvents();

    if (WINDOWS && !qfichero.contains(QDir::separator()))
        qfichero.replace('/',QDir::separator());
    if (filtrartexaxtivado()) filtratex(qfichero);

    QString fichero=qfichero;
    fichero.truncate(qfichero.length()-4);
    QString cadarg=fichero;

    QObject *parent=NULL;

     QStringList arguments;
     arguments << adapta(cadarg);

     QString cadexec;
     /*if (WINDOWS) cadexec="latex";
        else cadexec="pdflatex";*/
     cadexec="pdflatex";
     if (es_os_x()) cadexec="/Library/TeX/texbin/xelatex";
     QProcess *myProcess = new QProcess(parent);

     QApplication::processEvents();

     QString dirtrab=qfichero.left(qfichero.lastIndexOf(QDir::separator()));
     QString nombrefich=qfichero.mid(qfichero.lastIndexOf(QDir::separator())+1);
     nombrefich.remove(".tex");

     myProcess-> setWorkingDirectory(adapta(dirtrab));

     myProcess->start(cadexec,arguments);

     QApplication::processEvents();

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 1;
       }

     myProcess->start(cadexec,arguments);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    QApplication::processEvents();
    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 1;
       }

    // si es Windows,  se ha generado el pdf
    QApplication::processEvents();


    // ---------------------------------------------------------------
    // borramos aux y log
    QString fich_aux=cadarg+".aux";
    QString fich_log=cadarg+".log";

    QFile faux( fich_aux);
    faux.remove();
    QFile flog( fich_log);
    flog.remove();
    // ---------------------------------------------------------------


    QString rpdf=rutacopiapdf();
    /*if (eswindows() && !rpdf.isEmpty())
       {
        QString cadexec2="pdf";
        cadexec2+=cadexec;
        myProcess->start(cadexec2,arguments);
        if (!myProcess->waitForStarted ())
           {
             delete myProcess;
             return 1;
           }
        if (!myProcess->waitForFinished (60000))
           {
             delete myProcess;
             return 1;
           }

       }*/

    // si es Windows, se ha generado el pdf (segunda pasada)

/*    if (eswindows() && !rpdf.isEmpty())
       {
        QString cadexec2="pdf";
        cadexec2+=cadexec;
        myProcess->start(cadexec2,arguments);
        if (!myProcess->waitForStarted ())
           {
             delete myProcess;
             return 1;
           }
        if (!myProcess->waitForFinished (60000))
           {
             delete myProcess;
             return 1;
           }

       } */


    arguments.clear();
    //cadexec=visordvi();
    //if (WINDOWS) cadexec="start";
    //if (es_os_x()) cadexec= "open";
    cadarg=fichero;

    QString cadfichpdf=cadarg+".pdf";
    /* if (WINDOWS) cadarg+=".dvi";
       else cadarg+=".pdf";*/
    cadarg+=".pdf";

    QString url=cadarg;
    QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));

    /*arguments << adapta(cadarg);

    QApplication::processEvents();

    if (rpdf.isEmpty() || pref_forzar_ver_pdf())
      {
        if (WINDOWS)
          {
            #ifdef _WIN32
            QT_WA({

            ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

            } , {

            ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

            });
            #endif
            #ifdef _WIN64
            QT_WA({

            ShellExecute(0, 0, (TCHAR *)arguments.at(0).utf16(), 0, 0, SW_SHOWNORMAL);

            } , {

            ShellExecuteA(0, 0, arguments.at(0).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);

            });
            #endif
          }
        else
         {
           if (! myProcess->startDetached(cadexec,arguments)) // cadexec,arguments
            {
             delete myProcess;
             return 2;
            }
           QString url=qfich;
           QDesktopServices::openUrl(QUrl(url.prepend("file:"),QUrl::TolerantMode));
         }
      }

    delete myProcess;

    QApplication::processEvents();
    */
    if (!rpdf.isEmpty())
       {
        //
        QFile fich(cadfichpdf);
        QString destino=rpdf;
        destino.append(QDir::separator());
        destino.append(nombrefich);
        destino.append(cad_tiempo());
        destino.append(".pdf");
        if (eswindows()) fich.copy(destino);
           else fich.copy(destino);

       }

    QApplication::processEvents();

  return 0;
}



void borraestado(QString estado)
{
    basedatos::instancia()->deleteEstadostitulo(estado);
    basedatos::instancia()->deleteCabeceraestadostitulo(estado);
}

void desbloqueasaldosmedios()
{
    basedatos::instancia()->updateConfiguracionbloqsaldosmedios();
}

bool exportarestado(QString estado, QString nombre)
{
  // ----------------------------------------------------------------------------------
 QDomDocument doc("EstadoContable");
 QDomElement root = doc.createElement("EstadoContable");
 doc.appendChild(root);

 QDomElement tag = doc.createElement("Heading");
 root.appendChild(tag);

  // titulo,cabecera,parte1,parte2, observaciones,formulabasepor,'2000-01-01', "
  //      "ejercicio1,ejercicio2,diarios,estadosmedios,"
  //      "valorbasepor1, valorbasepor2,analitica,haycolref,colref,ci,grafico

  QSqlQuery query = basedatos::instancia()->selectTodoespecialcabeceraestadostitulo(estado);

   if ( query.isActive() )
       while (query.next())
          {
            QDomElement tagtit = doc.createElement("Titulo");
            tag.appendChild(tagtit);
            QDomText texttit = doc.createTextNode(filtracadxml(query.value(0).toString()));
            tagtit.appendChild(texttit);

            QDomElement tagcabecera = doc.createElement("Cabecera");
            tag.appendChild(tagcabecera);
            QDomText textcabecera = doc.createTextNode(filtracadxml(query.value(1).toString()));
            tagcabecera.appendChild(textcabecera);

            QDomElement tagparte1 = doc.createElement("Parte1");
            tag.appendChild(tagparte1);
            QDomText textparte1 = doc.createTextNode(filtracadxml(query.value(2).toString()));
            tagparte1.appendChild(textparte1);

            QDomElement tagparte2 = doc.createElement("Parte2");
            tag.appendChild(tagparte2);
            QDomText textparte2 = doc.createTextNode(filtracadxml(query.value(3).toString()));
            tagparte2.appendChild(textparte2);

            QDomElement tagobservaciones = doc.createElement("Observaciones");
            tag.appendChild(tagobservaciones);
            QDomText textobservaciones = doc.createTextNode(filtracadxml(query.value(4).toString()));
            tagobservaciones.appendChild(textobservaciones);

            QDomElement tagformulabasepor = doc.createElement("FormulaBase");
            tag.appendChild(tagformulabasepor);
            QDomText textformulabasepor = doc.createTextNode(filtracadxml(query.value(5).toString()));
            tagformulabasepor.appendChild(textformulabasepor);

            QDomElement tagejercicio1 = doc.createElement("Ejercicio1");
            tag.appendChild(tagejercicio1);
            QDomText textejercicio1 = doc.createTextNode(filtracadxml(query.value(7).toString()));
            tagejercicio1.appendChild(textejercicio1);

            QDomElement tagejercicio2 = doc.createElement("Ejercicio2");
            tag.appendChild(tagejercicio2);
            QDomText textejercicio2 = doc.createTextNode(filtracadxml(query.value(8).toString()));
            tagejercicio2.appendChild(textejercicio2);

            QDomElement tagdiarios = doc.createElement("Diarios");
            tag.appendChild(tagdiarios);
            QDomText textdiarios = doc.createTextNode(filtracadxml(query.value(9).toString()));
            tagdiarios.appendChild(textdiarios);

            QDomElement tagestadosmedios = doc.createElement("EstadosMedios");
            tag.appendChild(tagestadosmedios);
            QDomText textestadosmedios = doc.createTextNode(query.value(10).toBool() ? "true" : "false");
            tagestadosmedios.appendChild(textestadosmedios);

            QDomElement tagvalorbasepor1 = doc.createElement("valorbasepor1");
            tag.appendChild(tagvalorbasepor1);
            QDomText textvalorbasepor1 = doc.createTextNode(filtracadxml(query.value(11).toString()));
            tagvalorbasepor1.appendChild(textvalorbasepor1);

            QDomElement tagvalorbasepor2 = doc.createElement("valorbasepor2");
            tag.appendChild(tagvalorbasepor2);
            QDomText textvalorbasepor2 = doc.createTextNode(filtracadxml(query.value(12).toString()));
            tagvalorbasepor2.appendChild(textvalorbasepor2);

            QDomElement taganalitica = doc.createElement("analitica");
            tag.appendChild(taganalitica);
            QDomText textanalitica = doc.createTextNode(query.value(13).toBool() ? "true" : "false");
            taganalitica.appendChild(textanalitica);

            QDomElement taghaycolref = doc.createElement("haycolref");
            tag.appendChild(taghaycolref);
            QDomText texthaycolref = doc.createTextNode(query.value(14).toBool() ? "true" : "false");
            taghaycolref.appendChild(texthaycolref);

            QDomElement tagcolref = doc.createElement("colref");
            tag.appendChild(tagcolref);
            QDomText textcolref = doc.createTextNode(filtracadxml(query.value(15).toString()));
            tagcolref.appendChild(textcolref);

            QDomElement tagci = doc.createElement("ci");
            tag.appendChild(tagci);
            QDomText textci = doc.createTextNode(filtracadxml(query.value(16).toString()));
            tagci.appendChild(textci);

            QDomElement taggrafico = doc.createElement("grafico");
            tag.appendChild(taggrafico);
            QDomText textgrafico = doc.createTextNode(query.value(17).toBool() ? "true" : "false");
            taggrafico.appendChild(textgrafico);

            QDomElement tagdesglose = doc.createElement("desglose");
            tag.appendChild(tagdesglose);
            QDomText textdesglose = doc.createTextNode(query.value(18).toBool() ? "true" : "false");
            tagdesglose.appendChild(textdesglose);

            QDomElement tagdesglose_ctas = doc.createElement("desglose_ctas");
            tag.appendChild(tagdesglose_ctas);
            QDomText textdesglose_ctas = doc.createTextNode(query.value(19).toBool() ? "true" : "false");
            tagdesglose_ctas.appendChild(textdesglose_ctas);

            QDomElement tagdescabecera = doc.createElement("descabecera");
            tag.appendChild(tagdescabecera);
            QDomText textdescabecera = doc.createTextNode(filtracadxml(query.value(20).toString()));
            tagdescabecera.appendChild(textdescabecera);

            QDomElement tagdespie = doc.createElement("despie");
            tag.appendChild(tagdespie);
            QDomText textdespie = doc.createTextNode(filtracadxml(query.value(21).toString()));
            tagdespie.appendChild(textdespie);
         }

 QDomElement tag3 = doc.createElement("Cuerpo");
 root.appendChild(tag3);

   // titulo,apartado,parte1,clave,nodo,formula
   query = basedatos::instancia()->selectTituloapartadoparte1clavenodoformulaestadostitulo(estado);
   if ( query.isActive() )
       while (query.next())
          {
              QDomElement tag4 = doc.createElement("Registro");
              tag3.appendChild(tag4);

              QDomElement tagtitulo = doc.createElement("titulocod");
              tag4.appendChild(tagtitulo);
              QDomText texttitulo = doc.createTextNode(filtracadxml(query.value(0).toString()));
              tagtitulo.appendChild(texttitulo);

              QDomElement tagapartado = doc.createElement("apartado");
              tag4.appendChild(tagapartado);
              QDomText textapartado = doc.createTextNode(filtracadxml(query.value(1).toString()));
              tagapartado.appendChild(textapartado);

              QDomElement tagparte1 = doc.createElement("parte1");
              tag4.appendChild(tagparte1);
              QDomText textparte1 = doc.createTextNode(query.value(2).toBool() ? "true" : "false");
              tagparte1.appendChild(textparte1);

              QDomElement tagclave = doc.createElement("clave");
              tag4.appendChild(tagclave);
              QDomText textclave = doc.createTextNode(filtracadxml(query.value(3).toString()));
              tagclave.appendChild(textclave);

              QDomElement tagnode = doc.createElement("nodo");
              tag4.appendChild(tagnode);
              QDomText textnode = doc.createTextNode(filtracadxml(query.value(4).toString()));
              tagnode.appendChild(textnode);

              QDomElement tagformula = doc.createElement("formula");
              tag4.appendChild(tagformula);
              QDomText textformula = doc.createTextNode(filtracadxml(query.value(5).toString()));
              tagformula.appendChild(textformula);
         }


 QString xml = doc.toString();
  // -----------------------------------------------------------------------------------

  QFile fichero( adapta(nombre)  );

  if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

  QTextStream stream( &fichero );
  stream.setEncoding(QStringConverter::Utf8);

  stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  stream << "<?xml-stylesheet type=\"text/xsl\" href=\"estado2html.xslt\"?>";

  // bool esmysql = ( basedatos::instancia()->cualControlador() == basedatos::MYSQL );

  // ------------------------------------------------------------------------------------
  stream << xml;
  fichero.close();

           QString fichxslt=dirtrabajobd();
           fichxslt.append(QDir::separator());
           fichxslt.append("estado2html.xslt");
           if (!QFile::exists(fichxslt))
              {
               QString pasa2;
               pasa2=traydoc();
               pasa2.append(QDir::separator());
               pasa2.append("estado2html.xslt");
               if (eswindows()) pasa2=QFile::encodeName(pasa2);
               QFile fichero(pasa2);
               if (eswindows()) fichero.copy(QFile::encodeName(fichxslt));
                  else fichero.copy(fichxslt);
              }

  return true;
  // ------------------------------------------------------------------------------------
}


int importarestado(QString nombre)
{
  QDomDocument doc("EstadoContable");
  QFile fichero(adapta(nombre));
  if (!fichero.exists()) return false;

  if ( !fichero.open( QIODevice::ReadOnly ) ) return false;

  if (!doc.setContent(&fichero)) {
     fichero.close();
     return false;
    }
 fichero.close();

 QString titulo=etiquetadenodo(doc,"Titulo");

 // QMessageBox::warning( 0, QObject::tr("importar"),titulo);

  //buscamos el nombre
  if ( basedatos::instancia()->sihaytitulocabeceraestados(titulo) == 2 ) { return 2; }

            QString cabecera,parte1,parte2, observaciones,formulabasepor,fechacalculo,
                     ejercicio1,ejercicio2,diarios,estadosmedios,
                     valorbasepor1, valorbasepor2,analitica,haycolref,colref,ci,grafico,
                     desglose,desglose_ctas,descabecera,despie;
            cabecera=etiquetadenodo(doc,"Cabecera");
            parte1=etiquetadenodo(doc,"Parte1");
            parte2=etiquetadenodo(doc,"Parte2");
            observaciones=etiquetadenodo(doc,"Observaciones");
            formulabasepor=etiquetadenodo(doc,"FormulaBase");
            fechacalculo="2000-01-01";
            ejercicio1=etiquetadenodo(doc,"Ejercicio1");
            ejercicio2=etiquetadenodo(doc,"Ejercicio2");
            diarios=etiquetadenodo(doc,"Diarios");
            estadosmedios=etiquetadenodo(doc,"EstadosMedios");
            valorbasepor1=etiquetadenodo(doc,"valorbasepor1");
            valorbasepor2=etiquetadenodo(doc,"valorbasepor2");
            analitica=etiquetadenodo(doc,"analitica");
            haycolref=etiquetadenodo(doc,"haycolref");
            colref=etiquetadenodo(doc,"colref");
            ci=etiquetadenodo(doc,"ci");
            grafico=etiquetadenodo(doc,"grafico");
            if (grafico.length()==0) grafico="false";
            desglose=etiquetadenodo(doc,"desglose");
            desglose_ctas=etiquetadenodo(doc,"desglose_ctas");
            if (desglose_ctas.isEmpty()) desglose_ctas="false";
            descabecera=etiquetadenodo(doc,"descabecera");
            despie=etiquetadenodo(doc,"despie");
            // QMessageBox::warning( 0, QObject::tr("importar"),desglose_ctas);

            basedatos::instancia()->insert18Cabeceraestados(titulo,cabecera,parte1,parte2,
                observaciones,formulabasepor,fechacalculo,ejercicio1, 
                ejercicio2,diarios,estadosmedios,valorbasepor1,valorbasepor2, 
                analitica,haycolref,colref,ci,grafico,desglose,desglose_ctas,descabecera,despie);

   // importamos el resto de fichero
   // buscamo la etiqueta cuerpo
 QDomNodeList listanodos=doc.elementsByTagName ("Cuerpo");
 if (listanodos.size()==0) return false;
 QDomNode nodo= listanodos.at(0);
 QDomNode n=nodo.firstChild(); //registro

  while(!n.isNull()) {
     QDomElement e = n.toElement(); // try to convert the node to an element.
     QDomNode nn=e.firstChild();
     QString apartado,lparte1,clave,nodo,formula,importe1,importe2,calculado,referencia;
     while (!nn.isNull())
          {
            QDomElement ee = nn.toElement();
            if(!ee.isNull()) {
              if (ee.tagName()=="apartado")
                 {
                  QDomNode nnn=ee.firstChild();
                  QDomText t = nnn.toText();
                  apartado= t.data();
                 }
              if (ee.tagName()=="parte1")
                 {
                  QDomNode nnn=ee.firstChild();
                  QDomText t = nnn.toText();
                  lparte1= t.data();
                 }
              if (ee.tagName()=="clave")
                 {
                  QDomNode nnn=ee.firstChild();
                  QDomText t = nnn.toText();
                  clave= t.data();
                 }
              if (ee.tagName()=="nodo")
                 {
                  QDomNode nnn=ee.firstChild();
                  QDomText t = nnn.toText();
                  nodo= t.data();
                 }
              if (ee.tagName()=="formula")
                 {
                  QDomNode nnn=ee.firstChild();
                  QDomText t = nnn.toText();
                  formula= t.data();
                 }
              }
            nn=nn.nextSibling();
           }

     importe1="0";
     importe2="0";
     calculado="false";
     referencia="";
     basedatos::instancia()->insertEstados(titulo,apartado,lparte1,clave,nodo,
                formula,importe1,importe2,calculado,referencia);
     n=n.nextSibling();
  }
 return 1;
}

QString etiquetadenodo(QDomDocument doc,QString etiqueta)
{
 QDomNodeList listanodos=doc.elementsByTagName (etiqueta);
 if (listanodos.size()==0) return "";
 QDomNode nodo= listanodos.at(0);
 QDomNode nn=nodo.firstChild();
 QDomText t = nn.toText();
 return t.data();
}


QString textodenodo(QDomNode nodo)
{
 QDomNode nn=nodo.firstChild();
 QDomText t = nn.toText();
 return t.data();
}


int generapdffichabs(QString qfichero)
{
    if (filtrartexaxtivado()) filtratex(qfichero);

    QString fichero=qfichero;
    fichero.truncate(qfichero.length()-4);
    QString cadarg=fichero;

    QObject *parent=NULL;

     QStringList arguments;
     arguments << adapta(cadarg);

     QString cadexec="latex";
     QProcess *myProcess = new QProcess(parent);

     QApplication::processEvents();
     QString qdirtrabajo=qfichero.left(qfichero.lastIndexOf(QDir::separator())+1);
     myProcess-> setWorkingDirectory(adapta(qdirtrabajo));

     myProcess->start(cadexec,arguments);

     QApplication::processEvents();

    // system(cadexec);
     myProcess->start(cadexec,arguments);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }

    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 1;
       }

     myProcess->start(cadexec,arguments);

     QApplication::processEvents();

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 1;
       }
    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 1;
       }


    arguments.clear();
    cadexec="dvipdf";
    if (eswindows()) cadexec="dvipdfm";

    cadarg=adapta(fichero);
    cadarg+=".dvi";
    arguments << cadarg;
    // arguments << "&";

    myProcess->start(cadexec,arguments);

    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return 2;
       }

    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return 2;
       }

  delete myProcess;
  QFile::remove(fichero+".dvi");
  QFile::remove(fichero+".aux");
  QFile::remove(fichero+".log");
  return 0;
}


bool int_asientos_ej_cerrado(QString asientoinicial,QString asientofinal)
{
    return basedatos::instancia()->int_asientos_ej_cerrado(asientoinicial,asientofinal);
}

void pasasaldo(QString cuenta,QString importe,bool cargo)
{
    basedatos::instancia()->updateSaldossubcuentasaldocodigo(cuenta,importe,cargo);
}

bool cuentaenlibroiva(QString cuenta)
{
    return basedatos::instancia()->cuentaenlibroiva(cuenta);
}

bool paseapertura(QString pase)
{
    return basedatos::instancia()->paseapertura(pase);
}

bool paseconvto(QString pase)
{
    return basedatos::instancia()->paseconvto(pase);
}

bool pasepagovto(QString pase)
{
    return basedatos::instancia()->pasepagovto(pase);
}

bool paseenlibroivaeib(QString pase)
{
    return basedatos::instancia()->paseenlibroivaeib(pase);
}


void actualizasaldos()
{
    basedatos::instancia()->actucodigos_saldossubcuenta();
    basedatos::instancia()->updateSaldossubcuentasaldo0();
    // QMessageBox::warning( 0, "EXPORT","ANTES ACTUALIZA Sentencia SQL");
    if (import_remota) {
        QDate fecha;
        QSqlQuery q=basedatos::instancia()->ejecutar_publica("select max(cierre) from ejercicios where cerrado");
        QString cadexec;
        if (q.isActive()) {
            if (q.next()) {
                fecha = q.value(0) .toDate();
                QString cadfecha=fecha.toString("yyyy-MM-dd");
                if (cadfecha.isEmpty()) cadfecha="2000-01-01";
                cadexec="update saldossubcuenta set saldo=subquery.saldo from "
                          "(select cuenta, sum(debe-haber) as saldo from diario where fecha>'";
                cadexec+=cadfecha;
                          cadexec+="' group by cuenta) as subquery "
                          "where saldossubcuenta.codigo=subquery.cuenta";
                }
                else cadexec="update saldossubcuenta set saldo=subquery.saldo from "
                             "(select cuenta, sum(debe-haber) as saldo from diario "
                             "group by cuenta) as subquery "
                             "where saldossubcuenta.codigo=subquery.cuenta";
        }
        basedatos::instancia()->ejecutar_publica(cadexec);
        return;
    }
    QSqlQuery query = basedatos::instancia()->selectCodigosaldossubcuenta();
    if ( query.isActive() )
    {
        while (query.next())
        {
            double saldo = saldocuentaendiario(query.value(0).toString());
	        QString cadnum;
	        cadnum.setNum(saldo,'f',2);
            basedatos::instancia()->updateSaldossubcuentasaldocodigo( query.value(0).toString() , cadnum , true);
        }
    }
}


QString versionbd()
{
    return basedatos::instancia()->selectVersionconfiguracion();
}



QString traydoc(void)
{
 QString bundle;
#ifdef Q_WS_MAC
    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                           kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath,
                                           CFStringGetSystemEncoding());
    bundle=bundle.fromLocal8Bit(pathPtr) + "/Contents/Resources/doc";
    // qDebug("Path = %s", pathPtr);
    CFRelease(appUrlRef);
    CFRelease(macPath);
#endif
  //if (es_os_x()) return bundle;

  if (!WINDOWS) return "/usr/share/doc/keme4";
   else
        {
         QString cadena=cadena.fromLocal8Bit(getenv("ProgramFiles"));
         cadena+="\\keme4\\doc";
         return cadena;
        }
}
 
QString traynombrefichayuda(void)
 {
     QString nombrefich;
     nombrefich=traydoc();
     if (WINDOWS) nombrefich+="\\manual\\index.html";
        else nombrefich+="/manual/index.html";
     return nombrefich;
 }


bool importanuevoplan(QString qfichero)
{
  QFile fichero(adapta(qfichero));
  if (!fichero.exists()) return false;
   if ( fichero.open( QIODevice::ReadOnly ) ) 
     {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea;
         while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            QString codigo,descripcion;
            codigo=linea.section('\t',0,0);
            descripcion=linea.section('\t',1,1);
            basedatos::instancia()->insertNuevoplan(codigo,descripcion,false);
           }
         fichero.close();
      } else return false;
   return true;
}

void borraplannuevo()
{
    basedatos::instancia()->deleteNuevoplan();
}



int subgrupocongruponuevoplan(QString cadena)
{
    return basedatos::instancia()->subgrupocongruponuevoplan(cadena);
}


int cuentaconsubgruponuevoplan(QString cadena)
{
    return basedatos::instancia()->cuentaconsubgruponuevoplan(cadena);
}


int codigoconcuenta3digitosnuevoplan(QString cadena)
{
    return basedatos::instancia()->codigoconcuenta3digitosnuevoplan(cadena);
}

void guardacambiosplannuevo(QString codigo,QString descripcion)
{
    basedatos::instancia()->updateNuevoplandescripcioncodigo(codigo,descripcion);
}

int existecodigoplannuevo(QString cadena,QString *qdescrip)
{
    return basedatos::instancia()->existecodigoplannuevo(cadena,qdescrip);
}


void insertaenplannuevo(QString codigo,QString descripcion)
{
    basedatos::instancia()->insertNuevoplan(codigo,descripcion,false);
}

int itemplaneliminadonuevo(QString qcodigo)
{
    if ( basedatos::instancia()->siCodigoennuevoplanconlength(qcodigo) == 0 ) { return 0; }
    basedatos::instancia()->deleteNuevoplancodigo(qcodigo);
    return 1;
}


QString descripcioncuentanuevoplan(QString qcodigo)
{
    return basedatos::instancia()->selectDescripcionnuevoplancodigo(qcodigo);
}

bool codsubdivis(QString qcodigo)
{
    return basedatos::instancia()->codsubdivis(qcodigo,"plancontable");
}


bool codsubdivisplannuevo(QString qcodigo)
{
    return basedatos::instancia()->codsubdivis(qcodigo,"nuevoplan");
}


void cambiacuentaconta(QString origen, QString destino)
{
    basedatos::instancia()->updateAmortfiscalycontablecuenta_activocuenta_activo(origen,destino);
    basedatos::instancia()->updateAmortfiscalycontablecuenta_am_acumcuenta_am_acum(origen,destino);
    basedatos::instancia()->updataAmortfiscalycontablecuenta_gastocuenta_gasto(origen,destino);

    basedatos::instancia()->updateBorradorcuentacuenta(origen,destino);
    basedatos::instancia()->updateDatossubcuentacuentacuenta(origen,destino);
    basedatos::instancia()->updateDiariocuentacuenta(origen,destino);

    basedatos::instancia()->updateLibroivacta_base_ivacta_base_iva(origen,destino);
    basedatos::instancia()->updatelibroivacuenta_fracuenta_fra(origen,destino);

    basedatos::instancia()->updateplanamortizacionescuenta_activocuenta_activo(origen,destino);
    basedatos::instancia()->updatePlanamortizacionescuenta_am_acumcuenta_am_acum(origen,destino);
    basedatos::instancia()->updatePlanamortizacionescuenta_gastocuenta_gasto(origen,destino);

    basedatos::instancia()->updateAmortperscuentacuenta(origen,destino);
    basedatos::instancia()->updateSaldossubcuentacodigocodigo(origen,destino);
    basedatos::instancia()->updateVencimientoscta_ordenantecta_ordenante(origen,destino);
    basedatos::instancia()->updateVencimientoscta_tesoreriacta_tesoreria(origen,destino);
}


QString fsplash()
{
    QString fichsplash;
    if (!WINDOWS)
     {
      fichsplash="/usr/share/keme4";
      fichsplash+="/splash.png";
     }
      else 
           {
            fichsplash=fichsplash.fromLocal8Bit(getenv("ProgramFiles"));
            fichsplash+="\\keme4\\splash.png";
           }
    return fichsplash;
}

bool eswindows()
{
  return WINDOWS;
}

bool es_os_x()
{
  return OSX;
}


QString windirprogram()
{
 QString fichero=fichero.fromLocal8Bit(getenv("ProgramFiles"));
 return fichero;
}


bool exportarasmodelo(QString nombre,QString nfich)
{

 QDomDocument doc("AsientoAutomatico");
 QDomElement root = doc.createElement("AsientoAutomatico");
 doc.appendChild(root);

 // nombre y fecha
 QDomElement tag = doc.createElement("Heading");
 root.appendChild(tag);
 QSqlQuery query = basedatos::instancia()->selectAsientomodelofechacab_as_modeloasientomodelo(nombre);
 if ( query.isActive() )
    if (query.next())
       {
        addElementoTextoDom(doc,tag,"Nombre",filtracadxml(query.value(0).toString()));
        addElementoTextoDom(doc,tag,"Fecha",query.value(1).toString());
        addElementoTextoDom(doc,tag,"Aib",query.value(2).toBool() ? "true" : "false");
        addElementoTextoDom(doc,tag,"Autofactura_ue",query.value(3).toBool() ? "true" : "false");
        addElementoTextoDom(doc,tag,"Autofactura_no_ue",query.value(4).toBool() ? "true" : "false");
        addElementoTextoDom(doc,tag,"Eib",query.value(5).toBool() ? "true" : "false");
        addElementoTextoDom(doc,tag,"Eib_servicios",query.value(6).toBool() ? "true" : "false");
        addElementoTextoDom(doc,tag,"Diario",query.value(7).toString());
        addElementoTextoDom(doc,tag,"Importacion",query.value(8).toBool() ? "true" : "false");
        addElementoTextoDom(doc,tag,"Exportacion",query.value(9).toBool() ? "true" : "false");
        addElementoTextoDom(doc,tag,"Isp_interior",query.value(10).toBool() ? "true" : "false");
       }

 // variables
 QDomElement tag2 = doc.createElement("Variables");
 root.appendChild(tag2);

    query = basedatos::instancia()->selectVariabletipodescripcionvar_as_modeloasientomodelo(nombre);
   if ( query.isActive() )
       while (query.next())
          {
              QDomElement tag3 = doc.createElement("Variable");
              tag2.appendChild(tag3);
              // variable,tipo,descripcion,valor,orden,guardar
              addElementoTextoDom(doc,tag3,"NombreVar",filtracadxml(query.value(0).toString()));
              addElementoTextoDom(doc,tag3,"Tipo",filtracadxml(query.value(1).toString()));
              addElementoTextoDom(doc,tag3,"Descripcion",filtracadxml(query.value(2).toString()));
              addElementoTextoDom(doc,tag3,"Valor",filtracadxml(query.value(3).toString()));
              addElementoTextoDom(doc,tag3,"Orden",query.value(4).toString());
              addElementoTextoDom(doc,tag3,"Guardar",query.value(5).toBool() ? "true" : "false");
         }

 
 // detalle asiento
 QDomElement tagd = doc.createElement("Detalle");
 root.appendChild(tagd);
 query = basedatos::instancia()->select_det_as_modeloasientomodelo(nombre);
   if ( query.isActive() )
       while (query.next())
          {
              QDomElement tag3 = doc.createElement("Linea");
              tagd.appendChild(tag3);
              // cuenta,concepto,expresion,d_h,ci,baseiva,cuentafra,cuentabaseiva,
              // claveiva,documento,fecha_factura
              addElementoTextoDom(doc,tag3,"Cuenta",filtracadxml(query.value(0).toString()));
              addElementoTextoDom(doc,tag3,"Concepto",filtracadxml(query.value(1).toString()));
              addElementoTextoDom(doc,tag3,"Expresion",filtracadxml(query.value(2).toString()));
              addElementoTextoDom(doc,tag3,"DH",query.value(3).toString());
              addElementoTextoDom(doc,tag3,"CI",filtracadxml(query.value(4).toString()));
              addElementoTextoDom(doc,tag3,"BaseIva",filtracadxml(query.value(5).toString()));
              addElementoTextoDom(doc,tag3,"CuentaFra",filtracadxml(query.value(6).toString()));
              addElementoTextoDom(doc,tag3,"CuentaBaseIva",filtracadxml(query.value(7).toString()));
              addElementoTextoDom(doc,tag3,"ClaveIva",filtracadxml(query.value(8).toString()));
              addElementoTextoDom(doc,tag3,"Documento",filtracadxml(query.value(9).toString()));
              addElementoTextoDom(doc,tag3,"FechaFra",filtracadxml(query.value(10).toString()));
              addElementoTextoDom(doc,tag3,"Prorrata_e",filtracadxml(query.value(11).toString()));
              addElementoTextoDom(doc,tag3,"Rectificativa",
                                  query.value(12).toBool() ? "true" : "false");
              // addElementoTextoDom(doc,tag3,"Autofactura",query.value(13).toBool() ? "true" : "false");
              addElementoTextoDom(doc,tag3,"Fecha_op",filtracadxml(query.value(13).toString()));
              addElementoTextoDom(doc,tag3,"Clave_op",filtracadxml(query.value(14).toString()));
              addElementoTextoDom(doc,tag3,"Bicoste",filtracadxml(query.value(15).toString()));
              addElementoTextoDom(doc,tag3,"Rectificada",filtracadxml(query.value(16).toString()));
              addElementoTextoDom(doc,tag3,"Numfacts",filtracadxml(query.value(17).toString()));
              addElementoTextoDom(doc,tag3,"Facini",filtracadxml(query.value(18).toString()));
              addElementoTextoDom(doc,tag3,"Facfinal",filtracadxml(query.value(19).toString()));
              addElementoTextoDom(doc,tag3,"BienInversion",
                                  query.value(20).toBool() ? "true" : "false");
              // autofactura, afecto, agrario, nombre,  cif, "
              //"cta_retenido, arrendamiento, clave, base_ret, tipo_ret, "
              // "retencion, ing_a_cta, ing_a_cta_rep, nombre_ret, cif_ret, provincia "
              addElementoTextoDom(doc,tag3,"Afecto",filtracadxml(query.value(21).toString()));
              addElementoTextoDom(doc,tag3,"Agrario",
                                  query.value(22).toBool() ? "true" : "false");
              addElementoTextoDom(doc,tag3,"Nombre",filtracadxml(query.value(23).toString()));
              addElementoTextoDom(doc,tag3,"Cif",filtracadxml(query.value(24).toString()));
              addElementoTextoDom(doc,tag3,"Cta_retenido",filtracadxml(query.value(25).toString()));
              addElementoTextoDom(doc,tag3,"Arrendamiento",
                                  query.value(26).toBool() ? "true" : "false");
              addElementoTextoDom(doc,tag3,"Clave",filtracadxml(query.value(27).toString()));
              addElementoTextoDom(doc,tag3,"Base_ret",filtracadxml(query.value(28).toString()));
              addElementoTextoDom(doc,tag3,"Tipo_ret",filtracadxml(query.value(29).toString()));
              addElementoTextoDom(doc,tag3,"Retencion",filtracadxml(query.value(30).toString()));
              addElementoTextoDom(doc,tag3,"Ing_a_cta",filtracadxml(query.value(31).toString()));
              addElementoTextoDom(doc,tag3,"Ing_a_cta_rep",filtracadxml(query.value(32).toString()));
              addElementoTextoDom(doc,tag3,"Nombre_ret",filtracadxml(query.value(33).toString()));
              addElementoTextoDom(doc,tag3,"Cif_ret",filtracadxml(query.value(34).toString()));
              addElementoTextoDom(doc,tag3,"Provincia",filtracadxml(query.value(35).toString()));

         }

  QString xml = doc.toString();
  // -----------------------------------------------------------------------------------

  QFile fichero( adapta(nfich)  );

  if ( !fichero.open( QIODevice::WriteOnly ) ) return false;

  QTextStream stream( &fichero );
  stream.setEncoding(QStringConverter::Utf8);

  stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  stream << "<?xml-stylesheet type=\"text/xsl\" href=\"automat2html.xslt\"?>\n";

  // bool esmysql = ( basedatos::instancia()->cualControlador() == basedatos::MYSQL );

  // ------------------------------------------------------------------------------------
  stream << xml;
  fichero.close();

           // Vemos si hace falta copiar el fichero xslt
           QString fichxslt=dirtrabajobd();
           fichxslt.append(QDir::separator());
           fichxslt.append("automat2html.xslt");
           if (!QFile::exists(fichxslt))
              {
               QString pasa2;
               pasa2=traydoc();
               pasa2.append(QDir::separator());
               pasa2.append("automat2html.xslt");
               if (eswindows()) pasa2=QFile::encodeName(pasa2);
               QFile fichero(pasa2);
               if (eswindows()) fichero.copy(QFile::encodeName(fichxslt));
                  else fichero.copy(fichxslt);
              }


  return true;
}


int importarasmodelo(QString nombre)
{
  QDomDocument doc("AsientoAutomatico");
  QFile fichero(adapta(nombre));
  if (!fichero.exists()) return false;

  if ( !fichero.open( QIODevice::ReadOnly ) ) return false;

  if (!doc.setContent(&fichero)) {
     fichero.close();
     return false;
    }
 fichero.close();


 QString qnombre=etiquetadenodo(doc,"Nombre");
 // buscamos el nombre
 if ( basedatos::instancia()->asientomodeloencab_as_modelo(qnombre) == 2 ) {return 2; }
 QString asientomodelo,fecha;
 asientomodelo=qnombre;
 fecha=etiquetadenodo(doc,"Fecha");
 bool aib= etiquetadenodo(doc,"Aib") == "true" ? true : false;
 bool autofactura_ue= etiquetadenodo(doc,"Autofactura_ue") == "true" ? true : false;
 bool autofactura_no_ue = etiquetadenodo(doc,"Autofactura_no_ue") == "true" ? true : false;
 bool eib= etiquetadenodo(doc,"Eib") == "true" ? true : false;
 bool eib_servicios= etiquetadenodo(doc,"Eib_servicios") == "true" ? true : false;
 QString diario=etiquetadenodo(doc,"Diario");
 bool importacion=etiquetadenodo(doc,"Importacion") == "true" ? true : false;
 bool exportacion=etiquetadenodo(doc,"Exportacion") == "true" ? true : false;
 bool isp_op_interiores=etiquetadenodo(doc,"Isp_interior") == "true" ? true : false;

 //addElementoTextoDom(doc,tag,"Aib",query.value(2).toString());
 // addElementoTextoDom(doc,tag,"Autofactura_ue",query.value(3).toString());
 //addElementoTextoDom(doc,tag,"Autofactura_no_ue",query.value(4).toString());
 //addElementoTextoDom(doc,tag,"Eib",query.value(5).toString());
 //addElementoTextoDom(doc,tag,"Eib_servicios",query.value(6).toString());
 //addElementoTextoDom(doc,tag,"Diario",query.value(7).toString());

 // QString asientomodelo, QString fecha, QString tipo,
 // bool aib, bool autofactura_ue, bool autofactura_no_ue,
 // bool eib, bool eib_servicios, QString diario
 basedatos::instancia()->insertCab_as_modelo(asientomodelo,fecha,
                                             QObject::tr("GENERAL"), aib, autofactura_ue,
                                             autofactura_no_ue, eib, eib_servicios,
                                             isp_op_interiores,importacion, exportacion,
                                             diario);

 // pasamos a importar variables
 QDomNodeList listanodosnombreVar=doc.elementsByTagName ("NombreVar");
 QDomNodeList listanodostipo=doc.elementsByTagName ("Tipo");
 QDomNodeList listanodosdescripcion=doc.elementsByTagName ("Descripcion");
 QDomNodeList listanodosvalor=doc.elementsByTagName ("Valor");
 QDomNodeList listanodosorden=doc.elementsByTagName ("Orden");
 QDomNodeList listanodosguardar=doc.elementsByTagName ("Guardar");

 for (int veces=0; veces<listanodosnombreVar.count();veces++)
     {
       QString variable=textodenodo(listanodosnombreVar.at(veces));
       QString tipo=textodenodo(listanodostipo.at(veces));
       QString descripcion=textodenodo(listanodosdescripcion.at(veces));
       QString valor=textodenodo(listanodosvalor.at(veces));
       int orden=textodenodo(listanodosorden.at(veces)).toInt();
       bool guardar;
       guardar=textodenodo(listanodosguardar.at(veces)) == "true" ? true : false;
       basedatos::instancia()->insertVar_as_modelo(asientomodelo, variable,
                                 tipo, descripcion,
                                 valor, guardar,orden);
     }

 // importamos líneas de detalle
 QDomNodeList listanodoscuenta=doc.elementsByTagName ("Cuenta");
 QDomNodeList listanodosconcepto=doc.elementsByTagName ("Concepto");
 QDomNodeList listanodosexpresion=doc.elementsByTagName ("Expresion");
 QDomNodeList listanodosd_h=doc.elementsByTagName ("DH");
 QDomNodeList listanodosci=doc.elementsByTagName ("CI");
 QDomNodeList listanodosbaseiva=doc.elementsByTagName ("BaseIva");
 QDomNodeList listanodoscuentafra=doc.elementsByTagName ("CuentaFra");
 QDomNodeList listanodoscuentabaseiva=doc.elementsByTagName ("CuentaBaseIva");
 QDomNodeList listanodosclaveiva=doc.elementsByTagName ("ClaveIva");
 QDomNodeList listanodosdocumento=doc.elementsByTagName ("Documento");
 QDomNodeList listanodosfecha_factura=doc.elementsByTagName ("FechaFra");
 QDomNodeList listanodosprorrata_e=doc.elementsByTagName ("Prorrata_e");
 QDomNodeList listanodosrectificativa=doc.elementsByTagName ("Rectificativa");
 // QDomNodeList listanodosautofactura=doc.elementsByTagName ("Autofactura");
 QDomNodeList listanodosfecha_op=doc.elementsByTagName ("Fecha_op");
 QDomNodeList listanodosclave_op=doc.elementsByTagName ("Clave_op");
 QDomNodeList listanodosbicoste=doc.elementsByTagName ("Bicoste");
 QDomNodeList listanodosrectificada=doc.elementsByTagName ("Rectificada");
 QDomNodeList listanodosnumfacts=doc.elementsByTagName ("Numfacts");
 QDomNodeList listanodosfacini=doc.elementsByTagName ("Facini");
 QDomNodeList listanodosfacfinal=doc.elementsByTagName ("Facfinal");
 QDomNodeList listanodosbinversion=doc.elementsByTagName ("BienInversion");
 QDomNodeList listanodosafecto=doc.elementsByTagName ("Afecto");
 QDomNodeList listanodosagrario=doc.elementsByTagName ("Agrario");
 QDomNodeList listanodosnombre=doc.elementsByTagName ("Nombre");
 QDomNodeList listanodoscif=doc.elementsByTagName ("Cif");
 QDomNodeList listanodoscta_retenido=doc.elementsByTagName ("Cta_retenido");
 QDomNodeList listanodosarrendamiento=doc.elementsByTagName ("Arrendamiento");
 QDomNodeList listanodosclave=doc.elementsByTagName ("Clave");
 QDomNodeList listanodosbase_ret=doc.elementsByTagName ("Base_ret");
 QDomNodeList listanodostipo_ret=doc.elementsByTagName ("Tipo_ret");
 QDomNodeList listanodosretencion=doc.elementsByTagName ("Retencion");
 QDomNodeList listanodosing_a_cta=doc.elementsByTagName ("Ing_a_cta");
 QDomNodeList listanodosing_a_cta_rep=doc.elementsByTagName ("Ing_a_cta_rep");
 QDomNodeList listanodosnombre_ret=doc.elementsByTagName ("Nombre_ret");
 QDomNodeList listanodoscif_ret=doc.elementsByTagName ("Cif_ret");
 QDomNodeList listanodosprovincia=doc.elementsByTagName ("Provincia");


 for (int veces=0; veces<listanodoscuenta.count();veces++)
     {

      QString cuenta,concepto,expresion,d_h,ci,baseiva,cuentafra,cuentabaseiva,claveiva,documento;
      QString fecha_factura, prorrata_e;
      bool rectificativa, binversion;
      QString fecha_op, clave_op, bicoste;
      QString rectificada, numfacts, facini, facfinal;

      // afecto, agrario, nombre,cif, cta_retenido, arrendamiento,
      //             clave, base_ret, tipo_ret, retencion, ing_a_cta, ing_a_cta_rep, nombre_ret,
      //             cif_ret, provincia

      QString afecto;
      bool agrario;
      QString nombre, cif, cta_retenido;
      bool arrendamiento;
      QString clave, base_ret, tipo_ret, retencion, ing_a_cta;
      QString ing_a_cta_rep, nombre_ret, cif_ret, provincia;

      cuenta=textodenodo(listanodoscuenta.at(veces));
      concepto=textodenodo(listanodosconcepto.at(veces)).replace("&quot;","\"");
      expresion=textodenodo(listanodosexpresion.at(veces));
      d_h=textodenodo(listanodosd_h.at(veces));
      ci=textodenodo(listanodosci.at(veces));
      baseiva=textodenodo(listanodosbaseiva.at(veces));
      cuentafra=textodenodo(listanodoscuentafra.at(veces));
      cuentabaseiva=textodenodo(listanodoscuentabaseiva.at(veces));
      claveiva=textodenodo(listanodosclaveiva.at(veces));
      documento=textodenodo(listanodosdocumento.at(veces));
      fecha_factura=textodenodo(listanodosfecha_factura.at(veces));
      prorrata_e=textodenodo(listanodosprorrata_e.at(veces));
      rectificativa=textodenodo(listanodosrectificativa.at(veces))=="true" ? true : false;
      // autofactura=textodenodo(listanodosautofactura.at(veces))=="true" ? true : false;
      fecha_op=textodenodo(listanodosfecha_op.at(veces));
      clave_op=textodenodo(listanodosclave_op.at(veces));
      bicoste=textodenodo(listanodosbicoste.at(veces));


      rectificada=textodenodo(listanodosrectificada.at(veces));
      numfacts=textodenodo(listanodosnumfacts.at(veces));
      facini=textodenodo(listanodosfacini.at(veces));
      facfinal=textodenodo(listanodosfacfinal.at(veces));
      binversion=textodenodo(listanodosbinversion.at(veces))=="true" ? true : false;


      afecto=textodenodo(listanodosafecto.at(veces));
      agrario=textodenodo(listanodosagrario.at(veces))=="true" ? true : false;
      nombre=textodenodo(listanodosnombre.at(veces));
      cif=textodenodo(listanodoscif.at(veces));
      cta_retenido=textodenodo(listanodoscta_retenido.at(veces));
      arrendamiento=textodenodo(listanodosarrendamiento.at(veces))=="true" ? true : false;
      clave=textodenodo(listanodosclave.at(veces));
      base_ret=textodenodo(listanodosbase_ret.at(veces));
      tipo_ret=textodenodo(listanodostipo_ret.at(veces));
      retencion=textodenodo(listanodosretencion.at(veces));
      ing_a_cta=textodenodo(listanodosing_a_cta.at(veces));
      ing_a_cta_rep=textodenodo(listanodosing_a_cta_rep.at(veces));
      nombre_ret=textodenodo(listanodosnombre_ret.at(veces));
      cif_ret=textodenodo(listanodoscif_ret.at(veces));
      provincia=textodenodo(listanodosprovincia.at(veces));


      basedatos::instancia()->insertDet_as_modelo (asientomodelo , cuenta ,
            concepto , expresion , d_h , ci , baseiva , cuentabaseiva , cuentafra ,
            claveiva , documento, fecha_factura, prorrata_e, rectificativa,
            fecha_op,clave_op, bicoste, rectificada, numfacts, facini, facfinal,
            binversion, afecto, agrario, nombre,cif, cta_retenido, arrendamiento,
            clave, base_ret, tipo_ret, retencion, ing_a_cta, ing_a_cta_rep, nombre_ret,
            cif_ret, provincia,
            veces);

     }
 return 1;
}


int existecodigodiario(QString cadena,QString *qdescrip)
{
    return basedatos::instancia()->existecodigodiario(cadena,qdescrip);
}

void guardacambiosdiario(QString codigo,QString descripcion)
{
    basedatos::instancia()->updateDiariosdescripcioncodigo(codigo,descripcion);
}

void insertaendiarios(QString codigo,QString descripcion)
{
    basedatos::instancia()->insertDiarios(codigo,descripcion);
}

int eliminadiario(QString qcodigo)
{
    return basedatos::instancia()->eliminadiario(qcodigo);
}

QString noejercicio()
{
 return "- -";
}

bool existectapres(QString cuenta,QString ejercicio)
{
    return basedatos::instancia()->existectapres(cuenta,ejercicio);
}

double prevision(QString codigo,QString ejercicio)
{
    QSqlQuery query = basedatos::instancia()->selectPresupuestopresupuestoejerciciocuenta(codigo,ejercicio);

    if ( (query.isActive()) && (query.first()) )
    {
        return query.value(0).toDouble();
    }
    else
    {
       query = basedatos::instancia()->selectSumpresupuestopresupuestoejerciciocuenta(codigo,ejercicio);
       if ( (query.isActive()) &&(query.first()) ) return query.value(0).toDouble();
    }
    return 0;
}


double previsionsegunsaldo(QString codigo,QString ejercicio,bool saldopos)
{
    double total=0;
    QSqlQuery query = basedatos::instancia()->selectPresupuestopresupuestoejerciciocuenta(codigo,ejercicio);
    if ( (query.isActive()) && (query.first()) )
    {
        double valor=query.value(0).toDouble();
        if (saldopos) 
        {
            if (valor > 0) return valor; else return 0;
        }
        else 
        {
            if (valor< 0) return valor ; else return 0;
        }
    }
    else
    {
        query = basedatos::instancia()->selectPresupuestopresupuestoEjercicioPosCuenta(codigo,ejercicio);
        if (query.isActive()) {
            while (query.next()) 
            {
                if (saldopos && query.value(0).toDouble()>0) total+=query.value(0).toDouble();
                if (saldopos && query.value(0).toDouble()<0) total+=query.value(0).toDouble();
            }
        }
    }
    return total;
}


double previsionsegunsaldo_aux(QString codigo,QString ejercicio,bool saldopos)
{
    QSqlQuery query = basedatos::instancia()->selectPresupuestopresupuestoEjercicioCuenta(codigo,ejercicio);
    if ( (query.isActive()) && (query.first()) )
    {
        double valor=query.value(0).toDouble();
        if (saldopos) 
        {
            if (valor > 0) return valor; else return 0;
        }
        else 
        {
            if (valor< 0) return valor ; else return 0;
        }
    }
    return 0;
}


QString comandodescrip()
{
    return QObject::tr("DESCRIP");
}

bool cod_longitud_variable()
{
    return anchocuentas()==0;
}

bool esauxiliar(QString codigo)
{
    if (cod_longitud_variable())
    {
        QSqlQuery query = basedatos::instancia()->selectCodigoauxiliarplancontablecodigo(codigo);
        if ( (query.isActive()) && (query.first()) )
        {
            return query.value(1).toBool();
        }
    }
    else return codigo.length()==anchocuentas();
    return false;
}

bool es_long_var_agregadora(QString codigo)
{
    QSqlQuery query = basedatos::instancia()->selectCodigoauxiliarplancontablecodigo(codigo);
    if ( (query.isActive()) && (query.first()) )
    {
        return query.value(1).toBool();
    }
    return 0; 
}


bool prorrata_especial()
{
    return basedatos::instancia()->prorrata_especial();
}

double prorrata_iva()
{
    return basedatos::instancia()->prorrata_iva();
}


bool cuentaenplanamort(QString codigo)
{
    return basedatos::instancia()->cuentaenplanamort(codigo);
}

bool compronivelsup()
{
 return COMPRONIVELSUP ;
}


bool existecodigodif_conc(QString cadena)
{
    return basedatos::instancia()->existecodigodif_conc(cadena);
}

void eliminaconceptodif(QString qcodigo)
{
    basedatos::instancia()->deleteDif_conciliacioncodigo(qcodigo);
}


bool conceptodif_usado(QString cadena)
{
    QSqlQuery query = basedatos::instancia()->selectDif_conciliaciondiariodif_conciliacion(cadena);
    if ( (query.isActive()) && (query.first()) )
    {
        return true;
    }

    query = basedatos::instancia()->selectDif_conciliacioncuenta_ext_concidif_conciliacion(cadena);
    if ( (query.isActive()) && (query.first()) )
    {
        return true;
    }

    query = basedatos::instancia()->selectDif_conciliacionajustes_concidif_conciliacion(cadena);
    if ( (query.isActive()) && (query.first()) )
    {
        return true;
    }

    return false;
}


void guardacambiosdif_conc(QString codigo,QString descripcion)
{
    basedatos::instancia()->updateDif_conciliaciondescripcioncodigo(codigo,descripcion);
}


void insertaendif_conc(QString codigo,QString descripcion)
{
    basedatos::instancia()->insertDif_conciliacion(codigo,descripcion);
}


QString descripciondiferencia(QString qcodigo)
{
    return basedatos::instancia()->selectDescripciondif_conciliacioncodigo(qcodigo);
}


bool cuentaenpresupuesto(QString cadena)
{
    return basedatos::instancia()->cuentaenpresupuesto(cadena);
}


bool cuentaen_ajustes_conci(QString cadena)
{
    return basedatos::instancia()->cuentaen_ajustes_conci(cadena);
}


bool cuentaencuenta_ext_conci(QString cadena)
{
    return basedatos::instancia()->cuentaencuenta_ext_conci(cadena);
}

bool es_sqlite()
{
    return ( basedatos::instancia()->cualControlador() == basedatos::SQLITE );
}

QString usuario_sistema()
{
 QString usuario;
 if (WINDOWS) usuario=usuario.fromLocal8Bit(getenv("USER"));
      else usuario=getenv("USER");
 return usuario;
}

QString nombrebd()
{
    return basedatos::instancia()->nombre();
}

QString fichconex()
{
     QString dirconfig=trayconfig();

     QString nombredir="/conexiones.cfg";
     nombredir[0]=QDir::separator();
     return dirconfig+nombredir;
}


QString tipo_exento()
{
  return QObject::tr("EXENTO");
}

QString tipo_exento_nodeduc()
{
  return QObject::tr("EXENT_NO_DED");
}

QString tipo_nosujeto()
{
  return QObject::tr("NO_SUJETO");
}


QString soporte_xelatex()
{
  QString cadena;
  cadena="%!TEX TS-program = xelatex\n";
  cadena="%!TEX encoding = UTF-8 Unicode\n";
  return cadena;
}

QString config_font_xelatex()
{
  QString cadena;
  cadena="\\usepackage{fontspec,xltxtra,xunicode}\n";
  return cadena;
}

QString cabeceralatex()
{
 QString cadena;
 if (es_os_x()) cadena=soporte_xelatex();
 
    cadena+="\\documentclass[11pt,a4paper]{article}\n";
    cadena+="\\usepackage{tgtermes}\n";
    cadena+="\\usepackage[table]{xcolor}\n";

 if (es_os_x()) cadena+=config_font_xelatex();
    else cadena+=
           "\\usepackage{ucs}\n"
           "\\usepackage[utf8x]{inputenc}\n";
 cadena+=
           "\\usepackage[spanish]{babel}\n"
           "\\usepackage{lscape}\n"
           "\\usepackage{longtable}\n"
           "\\usepackage{anysize}\n"
           "\\usepackage{textcomp}\n"
           "\\begin{document}\n"
           "% CUERPO\n";
 return cadena;
}

QString cabeceralatex_graficos()
{
 QString cadena;
 if (es_os_x()) cadena=soporte_xelatex();

 cadena="\\documentclass[11pt,a4paper]{article}\n";
 cadena+="\\usepackage{utopia}\n"
         "\\usepackage[table]{xcolor}\n";
 
 if (es_os_x()) cadena+=config_font_xelatex();
    else cadena+=
           "\\usepackage{ucs}\n"
           "\\usepackage[utf8x]{inputenc}\n";
 cadena+=
           "\\usepackage[spanish]{babel}\n"
           "\\usepackage{lscape}\n"
           "\\usepackage{longtable}\n"
           "\\usepackage{anysize}\n"
           "\\usepackage{textcomp}\n"
           "\\usepackage{graphicx}\n"
           "\\begin{document}\n"
           "% CUERPO\n";
 return cadena;
}


QString margen_latex()
 {
   // margen para la memoria
   return "\\marginsize{3cm}{2cm}{2cm}{2cm}\n";
 }

QString margen_extra_latex()
 {
   // margen para listados contables
   return "\\marginsize{1.5cm}{1cm}{2cm}{2cm}\n";
 }


void ejecuta_regularizacion(QString cuenta_reg, QString cuentas, QString descripcion,
                            QDate fechainicial,QDate fechafinal,QDate fechaasiento, QString diario)
{

    if (!fechacorrespondeaejercicio(fechaasiento))
      {
	QMessageBox::warning( 0, QObject::tr("Tabla de apuntes"),QObject::tr("ERROR: La fecha del asiento no corresponde a ningún ejercicio definido"));
	return;
      }

   if (ejerciciocerrado(ejerciciodelafecha(fechaasiento)) ||
        ejerciciocerrando(ejerciciodelafecha(fechaasiento)))
    {
         QMessageBox::warning( 0, QObject::tr("Regularización"),
                                      QObject::tr("Error, el ejercicio seleccionado está cerrado o\n"
				  "se está cerrando."));
         return;
    }

 if (basedatos::instancia()->fecha_periodo_bloqueado((fechaasiento)))
    {
         QMessageBox::warning( 0, QObject::tr("Regularización"),
                               QObject::tr("Error, la fecha suministrada pertenece a periodo bloqueado"));
         return;
    }

  if (basedatos::instancia()->existe_bloqueo_cta(cuenta_reg,fechaasiento))
     {
       QMessageBox::warning( 0, QObject::tr("Regularización"),QObject::tr("ERROR EN FASE: La cuenta %1 "
                               "está bloqueada").arg(cuenta_reg));
                             return;
     }

  if (sobreescribeproxasiento(ejerciciodelafecha(fechaasiento)))
       {
              QMessageBox::warning( 0, QObject::tr("Regularización"),
				   QObject::tr("Error, no se pueden sobreescribir asientos\n"
				      "revise número de próximo asiento en configuración."));
             return;
      }

   QString cadinicio=fechainicial.toString("yyyy-MM-dd");
   QString cadfin=fechafinal.toString("yyyy-MM-dd");
   QString cadfechaasiento=fechaasiento.toString("yyyy-MM-dd");
   QString filtro;
   QString ejercicio=ejerciciodelafecha(fechaasiento);
   int partes=cuentas.count(',');
   if (cuentas.length()>0)
      {
       for (int veces=0;veces<=partes;veces++)
           {
             if (veces==0) filtro=" and (";
             if (basedatos::instancia()->cualControlador() == basedatos::SQLITE) {
                 filtro += "codigo like '"+ cuentas.section(',',veces,veces).replace("'","''") +"%'";
                }
                else {
                       filtro+="position('";
                       filtro+=cuentas.section(',',veces,veces).replace("'","''");
                       filtro+="' in codigo)=1";
                     }
             if (veces!=partes)          // filtrogastos+=")";
                  filtro+=" or ";
            }
         filtro+=")";
        }
 // -------------------------------------------------------------------------------------------------   

   // empezamos una nueva transacción y averiguamos número de asiento
   // QSqlDatabase contabilidad=QSqlDatabase::database();
   // contabilidad.transaction();

   // basedatos::instancia()->bloquea_para_regularizacion();

    qlonglong vnum = basedatos::instancia()->proximoasiento(ejercicio);

    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento; cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(ejercicio, cadnum);
   QString cadanchocuentas; cadanchocuentas.setNum(anchocuentas());
   QSqlQuery query = basedatos::instancia()->selectCodigoplancontablelengthauxiliarfiltro(cadanchocuentas, filtro);
   QString codigo;
   int veces=0;
   double saldo=0;
   QString cadsaldo;

    if ( query.isActive() ) {
          while ( query.next() )
               {
                 veces++;

                 codigo=query.value(0).toString();
                 // vamos calculando el saldo de las cuentas para el intervalo de fechas
                 QSqlQuery query2 = basedatos::instancia()->selectSaldocuentaendiariofechas(codigo,
                           cadinicio, cadfin);
                 if (query2.isActive())
                 if (query2.next())
                     {
                        if (query2.value(0).toDouble()>0.001 || query2.value(0).toDouble()<-0.001)
                        {
                         // vemos si la cuenta está bloqueada
                         if (basedatos::instancia()->existe_bloqueo_cta(codigo,fechaasiento))
                            {
                              QMessageBox::warning( 0, QObject::tr("Regularización"),QObject::tr("ERROR EN FASE: La cuenta %1 "
                                               "está bloqueada").arg(codigo));
                              return;
                            }
                        }
                    }
              }
      }

   query = basedatos::instancia()->selectCodigoplancontablelengthauxiliarfiltro(cadanchocuentas, filtro);
   veces=0;
   saldo=0;
   cadsaldo.clear();


    if ( query.isActive() ) {
          while ( query.next() )
               {
	         veces++;

	         codigo=query.value(0).toString();
	         // vamos calculando el saldo de las cuentas para el intervalo de fechas
	         QSqlQuery query2 = basedatos::instancia()->selectSaldocuentaendiariofechas(codigo,
                           cadinicio, cadfin);
	         if (query2.isActive())
	         if (query2.next())
		     {
		        if (query2.value(0).toDouble()>0.001 || query2.value(0).toDouble()<-0.001)
		        {
			 // insertamos nuevo apunte
			 saldo+=query2.value(0).toDouble();
                         qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                         QString cadnumpase;
                         cadnumpase.setNum(pase);

                         QString qdebe="0";
                         QString qhaber="0";
			 if (query2.value(0).toDouble()<0) 
			        { 
			            qdebe = query2.value(0).toString();
			            qdebe.remove('-');
			        }
			 if (query2.value(0).toDouble()>0) qhaber = query2.value(0).toString();
                         basedatos::instancia()->insertDiarioRegulParc(cadnumasiento,
                                                                       cadnumpase,
                          cadfechaasiento, codigo, qdebe, qhaber,descripcion, ejercicio, diario);
			        // queda actu saldo y proxpase
			        // queda actu saldo y proxpase

			 if (query2.value(0).toDouble()>0)
                            {
                             basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(
                                codigo, query2.value(0).toString(), false);
                            }
                           else 
                                {
				  QString lacad=query2.value(0).toString();
				  lacad.remove('-');
                                  basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo,
                                     lacad, true);
			        }

                        // actu proxpase
                        pase++;
                        cadnumpase.setNum(pase);
                        basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
			 // --------------------------------------------------
		        } // if value
		      } // if query2 next
	      } // while query next
          } // if query isactive
    // ---------------------------------
    if (saldo>0.001 || saldo<-0.001)
      {
        // contabilizamos contrapartida
         QString cadnumpase;
         qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
         cadnumpase.setNum(pase);

         QString qdebe="0";
         QString qhaber="0";
         cadsaldo.setNum(saldo,'f',2);
         if (saldo>0) qdebe = cadsaldo;
         else qdebe = "0";
         if (saldo<0) 
           { 
            cadsaldo.remove('-');
            qhaber = cadsaldo; 
           }
	   else qhaber = "0";

         basedatos::instancia()->insertDiarioRegulParc(cadnumasiento, cadnumpase,
                    cadfechaasiento, cuenta_reg, qdebe, qhaber,descripcion, ejercicio,diario);

         // queda actu saldo y proxpase
        if (saldo>0) 
	    {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, true);
        }
        else 
	    {
            basedatos::instancia()->updateSaldossubcuentasaldocargocodigo(codigo, cadsaldo, false);
	    }

          // actu proxpase
          pase++;
          cadnumpase.setNum(pase);
          basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);

      //----------------------------------------------------------------------------------------------
      }
   // basedatos::instancia()->desbloquea_y_commit();

}


QString xmlAbreTagLin(int pos,QString cadena)
{
  QString salida="<"+cadena;
  for (int veces=0;veces<pos*2;veces++) salida.prepend(' ');
  salida.append(">\n");
  return salida;
 }
 
QString xmlCierraTagLin(int pos,QString cadena)
 {
  QString salida="</"+cadena;
  for (int veces=0;veces<pos*2;veces++) salida.prepend(' ');
  salida.append(">\n");
  return salida;
 }
 
 
QString xmlAbreTag(int pos,QString cadena)
 {
  QString salida="<"+cadena;
  for (int veces=0;veces<pos*2;veces++) salida.prepend(' ');
  salida.append(">");
  return salida;
 }
 
 
QString xmlCierraTag(QString cadena)
 {
  QString salida="</"+cadena;
  salida.append(">\n");
  return salida;
 }

void addElementoTextoDom(QDomDocument doc,QDomElement padre,QString tagnombre,QString tagtexto)
{
  QDomElement taghijo = doc.createElement(tagnombre);
  padre.appendChild(taghijo);
  QDomText texthijo = doc.createTextNode(tagtexto);
  taghijo.appendChild(texthijo);
}


QString filtracadxml(QString cadena)
{
// Y comercial 	&amp; 	& 	&#38;#38;
// Abrir corchete angular 	&lt; 	< 	&#38;#60;
// Cerrar corchete angular 	&gt; 	> 	&#62;
// Comilla recta 	&quot; 	" 	&#39;
// Apóstrofo 	&apos; 	' 	&#34;
// cadena.replace("&","&amp;");
cadena.replace("<","&lt;");
cadena.replace(">","&gt;");
//cadena.replace("'","&#39;"); // cadena.replace("'","&apos;");
cadena.replace('"',"&quot;");
return cadena;
}

QString filtracadxml2(QString cadena)
{
cadena.replace("&","&amp;");
cadena.replace("<","&lt;");
cadena.replace(">","&gt;");
cadena.replace("'","&apos;");
cadena.replace('"',"&quot;");
return cadena;
}


QString cadena_descrip_ci (QString codigo)
{
     QString caddescripci;
     QString cadena,descripcion;
     QString qnivel=0;

     if (codigo.startsWith("???"))
                   {
                    caddescripci=QObject::tr("CUALQUIERA");
                   }
               else
                   {
                     bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
                     if (encontrada && qnivel.toInt()==1)
                         caddescripci=descripcion;
                   }
     if (codigo.length()>3) caddescripci += " - ";
     if (codigo.length()>3 && codigo.mid(3,3)==QString("???")) caddescripci += QObject::tr("CUALQUIERA");
               else
                    {
                     bool encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
                     int elnivel=qnivel.toInt();
                     if (encontrada && elnivel==2)
                     caddescripci += descripcion;
                    }

     if (codigo.length()>6) caddescripci += " - ";
     if (codigo.length()==7 && codigo[6]=='*')  caddescripci += QObject::tr("CUALQUIERA");
        else
            {
              bool encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
              if (encontrada && qnivel.toInt()==3)
                 caddescripci+=descripcion;
            }
    return caddescripci;
}

QString tipoivaexento()
{
    return QObject::tr("EXENTO");
}


bool hay_prorratageneral()
{
    if (prorrata_especial()) return false;
    double prorrata=prorrata_iva();
    if (prorrata>0.001 && prorrata< 9999) return true;
    return false;
}

void borrarasientofunc(QString elasiento, QString ejercicio, bool papelera)
{
    // QDate fechapase=fechadeasiento(elasiento,ejercicio);
    if (amortenintasientos(elasiento,elasiento,ejercicio))
       {
         QMessageBox::warning( 0, QObject::tr("BORRA ASIENTO"),
         QObject::tr("ERROR: El asiento seleccionado forma parte de la tabla de amortizaciones"));
         return;
       }

      // QString ejercicio=ejerciciodelafecha(fechapase);
      if (ejerciciocerrado(ejercicio) || ejerciciocerrando(ejercicio))
       {
         QMessageBox::warning( 0, QObject::tr("BORRA ASIENTO"),
                               QObject::tr("ERROR: El pase seleccionado corresponde a un ejercicio cerrado"));
         return;
       }
      if (basedatos::instancia()->asiento_en_periodo_bloqueado(elasiento,ejercicio))
       {
         QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                               QObject::tr("ERROR: El asiento seleccionado corresponde a un periodo bloqueado"));
         return;
       }

      if (basedatos::instancia()->asiento_con_cuentas_bloqueadas(elasiento,ejercicio))
       {
         QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                               QObject::tr("ERROR: El asiento seleccionado posee cuentas bloqueadas"));
         return;
       }

      //comprobamos enlace externo del asiento
      if (basedatos::instancia()->asiento_con_enlace(elasiento,ejercicio))
         {
          QMessageBox::warning( 0, QObject::tr("EDICIÓN DE ASIENTOS"),
                                QObject::tr("El asiento a editar está enlazado con una aplicación externa,\n"
                                            "la edición y borrado no se puede realizar desde KEME "
                                            ));
          return;
         }

  /*QString relas;
  if (numeracionrelativa())
      relas=numrelativa(elasiento);
    else relas=elasiento;*/

  if (QMessageBox::question(
            0,
            QObject::tr("¿ Borrar asiento ?"),
            QObject::tr("¿ Desea borrar el asiento %1 ?").arg(elasiento)) ==QMessageBox::No )
                          return;
  basedatos::instancia()->borraasientos(elasiento,elasiento,ejercicio,papelera);
}

QString adaptacad_aeat(QString cadena)
{
  QString destino=cadena;
  destino.replace(QObject::tr("á"),"a");
  destino.replace(QObject::tr("é"),"e");
  destino.replace(QObject::tr("í"),"i");
  destino.replace(QObject::tr("ó"),"o");
  destino.replace(QObject::tr("ú"),"u");
  destino.replace(QObject::tr("Á"),"A");
  destino.replace(QObject::tr("É"),"E");
  destino.replace(QObject::tr("Í"),"I");
  destino.replace(QObject::tr("Ó"),"O");
  destino.replace(QObject::tr("Ú"),"U");
  destino.replace(QObject::tr("ñ"),QObject::tr("Ñ"));
  destino.replace(QObject::tr("ç"),QObject::tr("Ç"));
  destino.replace(","," ");
  destino=destino.toUpper();
  for (int veces=0; veces< destino.length(); veces++)
      if (!QObject::tr("ABCDEFGHIJKLMNÑOPQRSTUVWXYZÇ1234567890 ").contains(destino[veces]))
          destino.remove(destino[veces]);
  //destino.replace("Ñ","-");
  return destino;
}


QString completacadnum(QString cadena, int espacio)
{
   if (cadena.length()>espacio) cadena=cadena.left(espacio);
   int longitud=cadena.length();
   for (int veces=0 ; veces<espacio-longitud; veces++) cadena="0"+cadena;
   return cadena;
}

QString completacadcad(QString cadena, int espacio)
{
   int longitud=cadena.length();
   for (int veces=0 ; veces<espacio-longitud; veces++) cadena=cadena+" ";
   if (cadena.length()>espacio) cadena=cadena.left(espacio);
   return cadena;
}


QString rutadocs(void)
 {

    QString nombreconex=basedatos::instancia()->nombre();
    if (es_sqlite()) nombreconex=nombreconex.mid(nombreconex.lastIndexOf(QDir::separator())+1);
    for (int veces=0; veces<conexion_rutas.count(); veces++)
      {
        if (conexion_rutas.at(veces)==nombreconex)
          {
            QString caddir=rutas_docs.at(veces);
            if (caddir.isEmpty()) break;
            // if (eswindows()) qdirtrabajo=QFile::encodeName(qdirtrabajo);
            QDir d(caddir);
            if (!d.exists()) d.mkdir(caddir);

            //QMessageBox::warning( 0, QObject::tr("ruta"),caddir);

            return caddir ;
          }
      }

  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  //if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString ruta=0;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("RUTADOCS")==0) ruta=contenido;
              }
            }
        ficheroini.close();
       }
  return ruta;
}



QString rutacargadocs(void)
 {
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString ruta=0;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("RUTACARGADOCS")==0) ruta=contenido;
              }
            }
        ficheroini.close();
       }
  return ruta;
}

bool pref_forzar_ver_pdf()
{
    QString nombrefichcompleto=trayconfig();
    QString nombre="/keme.cfg";
    nombre[0]=QDir::separator();
    nombrefichcompleto+=nombre;
    if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
    QFile ficheroini(nombrefichcompleto);
    QString cad_forzar="";

     if ( ficheroini.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &ficheroini );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea,variable,contenido;
          while ( !stream.atEnd() ) {
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              // analizamos la linea de texto
              if (linea.contains('=')) {
                 variable=linea.section('=',0,0);
                 contenido=linea.section('=',1,1);
                 if (variable.compare("FORZAR_VER_PDF")==0) cad_forzar=contenido;
                }
              }
          ficheroini.close();
         }
    bool forzar=false;
    if (cad_forzar=="1") forzar=true;
    return forzar;
}


QString rutacopiapdf(void)
 {
  if (basedatos::instancia()->gestiondeusuarios() && !privilegios[activa_visor_local])
      return QString();

  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString ruta=0;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("RUTACOPIAPDF")==0) ruta=contenido;
              }
            }
        ficheroini.close();
       }
  return ruta;
}



QString qscript_copias(void)
 {

  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString cadvalor;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("SCRIPT_COPIAS")==0) cadvalor=contenido;
              }
            }
        ficheroini.close();
       }
  return cadvalor;
}



QString rutacopiaseg(void)
 {
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString ruta=0;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("RUTASEG")==0) ruta=contenido;
              }
            }
        ficheroini.close();
       }
  return ruta;
}


QString rutalibros(void)
 {
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString ruta=0;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("RUTALIBROS")==0) ruta=contenido;
              }
            }
        ficheroini.close();
       }
  return ruta;
}



void visualizadores(QString *ext1, QString *vis1, QString *ext2, QString *vis2,
                    QString *ext3,QString *vis3, QString *ext4, QString *vis4)
{
  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;
  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString ruta=0;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("EXT1")==0) *ext1=contenido;
               if (variable.compare("VIS1")==0) *vis1=contenido;
               if (variable.compare("EXT2")==0) *ext2=contenido;
               if (variable.compare("VIS2")==0) *vis2=contenido;
               if (variable.compare("EXT3")==0) *ext3=contenido;
               if (variable.compare("VIS3")==0) *vis3=contenido;
               if (variable.compare("EXT4")==0) *ext4=contenido;
               if (variable.compare("VIS4")==0) *vis4=contenido;
              }
            }
        ficheroini.close();
       }

}

QString aplicacionabrirfich(QString extension)
{
  QString ext1, vis1, ext2, vis2, ext3, vis3, ext4, vis4;

  visualizadores(&ext1, &vis1, &ext2, &vis2,
                 &ext3, &vis3, &ext4, &vis4);
  QStringList list;
  list = ext1.split(",");

  if (list.contains(extension)) return vis1;

  list = ext2.split(",");
  if (list.contains(extension)) return vis2;

  list = ext3.split(",");
  if (list.contains(extension)) return vis3;

  list = ext4.split(",");
  if (list.contains(extension)) return vis4;

  return QString();
}


bool ejecuta(QString aplicacion, QString qfichero)
{

    QObject *parent=NULL;

     QStringList arguments;
     arguments << qfichero;

     QProcess *myProcess = new QProcess(parent);

    if (! myProcess->startDetached(aplicacion,arguments))
       {
         delete myProcess;
         return false;
       }

    delete myProcess;


  return true;
}

QString extensionfich(QString fichero)
{
  int pospunto=fichero.lastIndexOf('.');
  if (!(pospunto>0)) return QString();
  return fichero.right(fichero.length()-pospunto-1);
}

QString expanderutadocfich(QString fichdoc)
{
  if (fichdoc.isEmpty()) return QString();
   QString caddestino=rutadocs();
   caddestino+=QDir::separator();
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
   caddestino+=QDir::separator();
   // averiguar número del documento
   int numdir=fichdoc.contains('-') ? (fichdoc.section('-',0,0).toInt()/100+1)*100 :
             (fichdoc.section('.',0,0).toInt()/100+1)*100 ;
   QString cadnumdir;
   cadnumdir.setNum(numdir);
   cadnumdir=cadnumdir.trimmed();
   caddestino+=cadnumdir;
   caddestino+=QDir::separator();
   caddestino+=fichdoc;
   return caddestino;
}


QString dirdocsdefecto()
{
   QString directorio;
   if (WINDOWS) directorio=directorio.fromLocal8Bit(getenv("USERPROFILE"));
     else
       directorio=getenv("HOME");
   QString nombredir="/keme";
   nombredir[0]=QDir::separator();
   directorio+=nombredir;
   directorio.append(QDir::separator());
   directorio+="docs";
   return directorio;
}


bool es_cuenta_para_analitica(QString codigo)
{
  bool procesar_analitica=false;
  if (conanalitica() && (escuentadegasto(codigo) || escuentadeingreso(codigo)))
     procesar_analitica=true;

  if (conanalitica_parc() && codigo_en_analitica(codigo))
     procesar_analitica=true;
  return procesar_analitica;
}


QString editor_latex()
{

  QString nombrefichcompleto=trayconfig();
  QString nombre="/keme.cfg";
  nombre[0]=QDir::separator();
  nombrefichcompleto+=nombre;

  if (eswindows()) nombrefichcompleto=QFile::encodeName(nombrefichcompleto);
  QFile ficheroini(nombrefichcompleto);
  QString qeditor;

   if ( ficheroini.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &ficheroini );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("EDITORLATEX")==0) qeditor=contenido;
              }
            }
        ficheroini.close();
       }
  return qeditor;

}


int editalatex(QString qfichero)
{
    QString pasafich=dirtrabajo();
    pasafich.append(QDir::separator());
    pasafich.append(qfichero);
    pasafich.append(".tex");
    if (filtrartexaxtivado()) filtratex(pasafich);

    QString fichero=qfichero;

    QObject *parent=NULL;
    QProcess *myProcess = new QProcess(parent);

    QStringList arguments;

    QString cadexec=editor_latex();
    // QString cadexec="kile";
    QString cadarg=dirtrabajo();
    cadarg.append(QDir::separator());
    cadarg+=fichero;
    cadarg+=".tex";
    arguments << adapta(cadarg);
    // arguments << "&";

    if (! myProcess->startDetached(cadexec,arguments))
       {
         delete myProcess;
         return 1;
       }

    delete myProcess;

  return 0;
}

int editalatex_abs(QString qfichero)
{
    if (filtrartexaxtivado()) filtratex(qfichero);

    QString fichero=qfichero;

    QObject *parent=NULL;
    QProcess *myProcess = new QProcess(parent);

    QStringList arguments;

    QString cadexec=editor_latex();
    // QString cadexec="kile";
    QString cadarg=fichero;
    arguments << adapta(cadarg);
    // arguments << "&";

    if (! myProcess->startDetached(cadexec,arguments))
       {
         delete myProcess;
         return 1;
       }

    delete myProcess;

  return 0;
}


bool carga_saft_plan(QString qfichero)
{

    QDomDocument doc("AuditFile");
    QFile fichero(qfichero);
    if (!fichero.exists()) return false;

    if ( !fichero.open( QIODevice::ReadOnly ) ) return false;

    if (!doc.setContent(&fichero)) {
       fichero.close();
       return false;
      }
   fichero.close();

   QStringList codigos;
   QStringList descrip;
   QList<bool> auxiliar;

   QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        bool masterfiles=false;
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (e.tagName()=="MasterFiles")
               {
                masterfiles=true;
                //QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),e.tagName());
                QDomNode jj=e.firstChild();
                while (!jj.isNull())
                   {
                    QDomElement aa = jj.toElement();
                    // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),aa.tagName());
                    if (aa.tagName()=="GeneralLedger")
                      {
                       QDomNode nn=aa.firstChild();
                       while (!nn.isNull())
                          {
                           QDomElement ee = nn.toElement();
                           if(!ee.isNull()) {
                             if (ee.tagName()=="AccountID")
                              {
                               QDomNode nnn=ee.firstChild();
                               QDomText t = nnn.toText();
                               // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),t.data());
                               codigos << t.data();
                               auxiliar << false;
                              }
                             if (ee.tagName()=="AccountDescription")
                              {
                               QDomNode nnn=ee.firstChild();
                               QDomText t = nnn.toText();
                               descrip << t.data();
                               // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),t.data());
                              }

                           }
                           nn=nn.nextSibling();
                          }
                        }
                     jj=jj.nextSibling();
                    }
                }
        }
        if (masterfiles) break;
        n = n.nextSibling();
    }

    // creamos una replica de codigos para su ordenación
    QStringList codigosrep;
    codigosrep=codigos;
    codigosrep.sort();

    for (int i=0; i< codigosrep.size(); ++i)
      {
        if (i==codigosrep.size()-1)
          {
            auxiliar[i]=true;
            break;
          }
        // vemos si el codigo actual está contenido en el siguiente
        if (!codigosrep.at(i+1).startsWith(codigosrep.at(i))) auxiliar[i]=true;
      }


    // ahora falta buscar las descripciones correctas
    // y asignarlas a descriprep
    QStringList descriprep;
    for (int i=0; i< codigosrep.size(); ++i)
      {
        QString buscar=codigosrep.at(i);
        for (int veces=0; veces<codigos.size(); ++veces)
           {
            if (codigos.at(veces)==buscar)
              {
                descriprep << descrip.at(veces);
                break;
              }
           }
      }

    // tenemos las listas codigosrep, descriprep, auxiliar con la información
    // necesaria para la importación
    QProgressDialog progreso(QObject::tr("Insertando registros ..."), 0, 0, codigosrep.size());
    progreso.setWindowModality(Qt::WindowModal);

    for (int veces=0; veces<codigosrep.size(); ++veces)
      {
        QString descripcion;
        if (existecodigoplan(codigosrep.at(veces),&descripcion)) continue;
        if (codigosrep.at(veces).length()==0) continue;
        basedatos::instancia()->insertPlancontable(codigosrep.at(veces),
                                                   descriprep.at(veces),
                                                   auxiliar.at(veces));
        progreso.setValue(veces);
        // QApplication::processEvents();
      }

   return true;
}

bool carga_saft_asientos(QString qfichero, QString usuario)
{
    // Habría que cargar primero el asiento de apertura a partir de la info de cuentas
    QDomDocument doc("AuditFile");
    QFile fichero(adapta(qfichero));
    if (!fichero.exists()) return false;

    if ( !fichero.open( QIODevice::ReadOnly ) ) return false;

    if (!doc.setContent(&fichero)) {
       fichero.close();
       return false;
      }
   fichero.close();

   QStringList codigos;
   QStringList debe;
   QStringList haber;
   QDate fecha_apertura;

   QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        bool masterfiles=false;
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (e.tagName()=="Header")
              {
                QDomNode jj=e.firstChild();
                while (!jj.isNull())
                   {
                    QDomElement aa = jj.toElement();
                    // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),aa.tagName());
                    if (aa.tagName()=="StartDate")
                       {
                        QDomNode nnn=aa.firstChild();
                        QDomText t = nnn.toText();
                        fecha_apertura=QDate::fromString (t.data(), "yyyy-MM-dd");
                        // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),t.data());
                       }
                    jj=jj.nextSibling();
                   }
              }
            if (e.tagName()=="MasterFiles")
               {
                masterfiles=true;
                //QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),e.tagName());
                QDomNode jj=e.firstChild();
                while (!jj.isNull())
                   {
                    QDomElement aa = jj.toElement();
                    // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),aa.tagName());
                    if (aa.tagName()=="GeneralLedger")
                      {
                       QDomNode nn=aa.firstChild();
                       while (!nn.isNull())
                          {
                           QDomElement ee = nn.toElement();
                           if(!ee.isNull()) {
                             if (ee.tagName()=="AccountID")
                              {
                               QDomNode nnn=ee.firstChild();
                               QDomText t = nnn.toText();
                               // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),t.data());
                               codigos << t.data();
                              }
                             if (ee.tagName()=="OpeningDebitBalance")
                              {
                               QDomNode nnn=ee.firstChild();
                               QDomText t = nnn.toText();
                               debe << t.data();
                               // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),t.data());
                              }
                             if (ee.tagName()=="OpeningCreditBalance")
                              {
                               QDomNode nnn=ee.firstChild();
                               QDomText t = nnn.toText();
                               haber << t.data();
                               // QMessageBox::warning( 0, QObject::tr("CÓDIGOS"),t.data());
                              }

                           }
                           nn=nn.nextSibling();
                          }
                        }
                     jj=jj.nextSibling();
                    }
                }
        }
        if (masterfiles) break;
        n = n.nextSibling();
    }


    // tenemos las listas codigosrep, debe, haber con la información
    // necesaria para la importación
    QProgressDialog progreso(QObject::tr("Insertando asiento apertura ..."), 0, 0, codigos.size()-1);
    progreso.setWindowModality(Qt::WindowModal);

    QString qejercicio=ejerciciodelafecha(fecha_apertura);
    if (sobreescribeproxasiento(qejercicio))
      {
        QMessageBox::warning( 0, QObject::tr("Importar asientos"),
                              QObject::tr("Error, no se pueden sobreescribir asientos\n"
                               "revise número de próximo asiento en configuración."));
       return false;
      }

    qlonglong vnum = basedatos::instancia()->proximoasiento(qejercicio);
    QString cadnum; cadnum.setNum(vnum+1);
    QString cadnumasiento;
    cadnumasiento.setNum(vnum);

    basedatos::instancia()->update_ejercicio_prox_asiento(qejercicio, cadnum);
    // procesamos asiento de apertura
    for (int veces=0; veces<codigos.size(); ++veces)
      {
        if ((debe.at(veces).toDouble()<0.001 && debe.at(veces).toDouble()>-0.001) &&
            (haber.at(veces).toDouble()<0.001 && haber.at(veces).toDouble()>-0.001))
            continue;
        if (!existesubcuenta(codigos.at(veces)))
           {
            continue;
           }

        // procesamos aquí adición de pase
        QString cadnumpase;
        qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
        cadnumpase.setNum(pase);
        basedatos::instancia()->insertpegaDiario(cadnumasiento, pase,
                                                 fecha_apertura.toString("yyyy-MM-dd"),
                                                 codigos.at(veces),
                                                 debe.at(veces).toDouble(),
                                                 haber.at(veces).toDouble(),
                                                 QObject::tr("Asiento de apertura"),
                                                 "","",usuario,qejercicio);

        basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(
                haber.at(veces), debe.at(veces), codigos.at(veces));
        basedatos::instancia()->updateDiariodiarioasiento(diario_apertura(),cadnumasiento,qejercicio);
        pase++;
        cadnumpase.setNum(pase);

        basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);


        progreso.setValue(veces);
        // QApplication::processEvents();
      }
     // QApplication::processEvents();
     progreso.close();

     n = docElem.firstChild();
     int entradas=0;
     while(!n.isNull()) {
         QDomElement e = n.toElement(); // try to convert the node to an element.
         if(!e.isNull()) {
             if (e.tagName()=="GeneralLedgerEntries")
             {
               QProgressDialog progres(QObject::tr("Insertando asientos ..."), 0, 0, entradas);
               progres.setWindowModality(Qt::WindowModal);
               progres.open();
               int valprog=1;
               QDomNode jj=e.firstChild();
               while (!jj.isNull())
                    {
                      QDomElement aa = jj.toElement();
                      if (aa.tagName()=="NumberOfEntries")
                         {
                          QDomNode ccc=aa.firstChild();
                          QDomText t = ccc.toText();
                          QString cadentradas=t.data();
                          entradas=cadentradas.toInt();
                         }
                      if (aa.tagName()=="Journal")
                        {
                          QDomNode nn=aa.firstChild();
                          while (!nn.isNull())
                             {
                              QDomElement ee = nn.toElement();
                              if(!ee.isNull()) {
                                if (ee.tagName()=="Transaction")
                                  {
                                    // esto representaría un asiento
                                    QDate fecha;
                                    progres.setValue(valprog++);
                                    QDomNode hh = ee.firstChild();
                                    // número de asiento
                                    qlonglong vnum=-1;
                                    QString cadnumasiento; // a utilizar
                                    QString qdescrip;
                                    while (!hh.isNull())
                                      {
                                        QDomElement ii = hh.toElement();
                                        if (!ii.isNull()) {
                                            if (ii.tagName()=="TransactionDate")
                                              {
                                               // esta es la fecha del asiento
                                                QDomNode ccc=ii.firstChild();
                                                QDomText t = ccc.toText();
                                                fecha=QDate::fromString (t.data(), "yyyy-MM-dd");
                                                if (vnum==-1) // número de asiento no asignado
                                                   {
                                                    // asignamos próx. asiento conforme a la fecha
                                                    QString ejercicio=ejerciciodelafecha(fecha);
                                                    vnum = basedatos::instancia()->proximoasiento(ejercicio);
                                                    QString cadnum; cadnum.setNum(vnum+1);
                                                    cadnumasiento.setNum(vnum);
                                                    basedatos::instancia()->update_ejercicio_prox_asiento(ejercicio,cadnum);
                                                   }
                                              }
                                            if (ii.tagName()=="Description")
                                               {
                                                // descripción
                                                QDomNode ccc = ii.firstChild();
                                                QDomText t = ccc.toText();
                                                qdescrip=t.data();
                                               }
                                            if (ii.tagName()=="Line")
                                              {
                                                // este sería un apunte del asiento
                                                QDomNode pp = ii.firstChild();
                                                QString qcuenta, qdocumento;
                                                QString qdebe, qhaber;
                                                while (!pp.isNull())
                                                  {
                                                   QDomElement jej = pp.toElement();
                                                   if (jej.tagName()=="AccountID")
                                                      {
                                                       // cuenta
                                                       QDomNode ccc = jej.firstChild();
                                                       QDomText t = ccc.toText();
                                                       qcuenta=t.data();
                                                      }
                                                   if (jej.tagName()=="SourceDocument")
                                                      {
                                                       // documento
                                                       QDomNode ccc = jej.firstChild();
                                                       QDomText t = ccc.toText();
                                                       qdocumento=t.data();
                                                      }
                                                   if (jej.tagName()=="DebitAmount")
                                                      {
                                                       // debe
                                                       QDomNode ccc = jej.firstChild();
                                                       QDomText t = ccc.toText();
                                                       qdebe=t.data();
                                                      }
                                                   if (jej.tagName()=="CreditAmount")
                                                      {
                                                       // haber
                                                       QDomNode ccc = jej.firstChild();
                                                       QDomText t = ccc.toText();
                                                       qhaber=t.data();
                                                      }
                                                   pp=pp.nextSibling();
                                                  }
                                                // aquí procesamos el apunte
                                                if (existesubcuenta(qcuenta))
                                                   {
                                                    QString cadnumpase;
                                                    qlonglong pase = basedatos::instancia()->selectProx_paseconfiguracion();
                                                    cadnumpase.setNum(pase);
                                                    basedatos::instancia()->insertpegaDiario(cadnumasiento, pase,
                                                                                         fecha.toString("yyyy-MM-dd"),
                                                                                         qcuenta,
                                                                                         qdebe.toDouble(),
                                                                                         qhaber.toDouble(),
                                                                                         qdescrip,
                                                                                         qdocumento,"",usuario,qejercicio);

                                                     basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(
                                                        qhaber.isEmpty() ? "0" : qhaber, qdebe.isEmpty() ? "0" : qdebe, qcuenta);
                                                     pase++;
                                                     cadnumpase.setNum(pase);
                                                     basedatos::instancia()->updateConfiguracionprox_pase(cadnumpase);
                                                  }
                                              }
                                           }
                                        hh=hh.nextSibling();
                                      }
                                  }
                               }
                               nn=nn.nextSibling();
                              }
                        }
                      jj=jj.nextSibling();
                    }
              }
       }
       n=n.nextSibling();
    }
   return true;

}

double redond(double valor, int decimales)
{
    return CutAndRoundNumberToNDecimals (valor, decimales);
}

double CutAndRoundNumberToNDecimals (double dValue, int nDec)
{
    // jsala
    /*****
    double dTemp = 0, dFract = 0, dInt = 0, dRes = 0;

    dTemp = dValue*pow(10,nDec);

    dFract = modf (dTemp, &dInt);

    if (dFract >= 0.5)
    dInt++;

    dRes = dInt/pow(10,nDec);

    return dRes;
    ******/
    // jsala El punto medio lo redondea hacia arriba sobre el valor absoluto.
      double val = ((dValue < 0)?-dValue:dValue) * pow(10,nDec);
      double fVal = floor(val);
      int inc = ((val - fVal) < (0.5 - 1e-4))?0:1;
      double r = (fVal + inc)/pow(10,nDec);
      return (dValue < 0)?-r:r;
}


void ffacturae(QString serie, QString numero)
{

    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie, numero);
    QString tipo_doc, cuenta, externo, tipo_ret, retencion, notas, pie1, pie2;
    QString c_a_rol1, c_a_rol2, c_a_rol3;
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
        c_a_rol1=q.value(22).toString();
        c_a_rol2=q.value(23).toString();
        c_a_rol3=q.value(24).toString();
        } else return;
    }

    QString qnombre_empresa, qdomicilio, qcp, qpoblacion;
    QString qprovincia, qpais_emisor, qcifempresa, qid_registral;
    basedatos::instancia()->datos_empresa_tipo_doc(tipo_doc,
                                &qnombre_empresa,
                                &qdomicilio,
                                &qcp,
                                &qpoblacion,
                                &qprovincia,
                                &qpais_emisor,
                                &qcifempresa,
                                &qid_registral
                                    );

   QString fe_idioma, fe_libro, fe_registro, fe_hoja, fe_folio, fe_seccion, fe_volumen,fe_datos_adic;

   basedatos::instancia()->datos_facturae_tipo_doc(tipo_doc, &fe_idioma,
                                            &fe_libro, &fe_registro,
                                            &fe_hoja, &fe_folio,
                                            &fe_seccion, &fe_volumen, &fe_datos_adic);


    // obtenemos información de la cuenta cliente, para los datos de la factura
    if (externo.isEmpty())
       q=basedatos::instancia()->selectTodoDatossubcuentacuenta (cuenta);
     else
        q=basedatos::instancia()->selectTodoDatosexterno (externo);

    // "SELECT cuenta,razon,nombrecomercial,cif,nifrprlegal,domicilio,poblacion,";
    // "codigopostal,provincia,pais,tfno,fax,email,observaciones,ccc,cuota, ";
    // "venciasoc,codvenci,tesoreria,ivaasoc,cuentaiva,tipoiva,conceptodiario, ";
    //  "web, claveidfiscal, procesavenci "
    QString razon,nombrecomercial,cif,domicilio,poblacion,
      codigopostal,provincia,pais,codvenci, tfno, email,cta_tesoreria, forma_pago;
    QString iban;
    if (q.isActive())
     {
       if (q.next())
        {
         razon=q.value(1).toString();
         nombrecomercial=q.value(2).toString();
         cif=q.value(3).toString();
         domicilio=q.value(5).toString();
         poblacion=q.value(6).toString();
         codigopostal=q.value(7).toString();
         provincia=q.value(8).toString();
         pais=q.value(26).toString();
         tfno=q.value(10).toString();
         email=q.value(12).toString();
         codvenci=q.value(17).toString();
         cta_tesoreria=q.value(18).toString();
         forma_pago=q.value(46).toString();
        }
     }

    if (!cta_tesoreria.isEmpty()) {
       iban=basedatos::instancia()->iban_cuenta_banco(cta_tesoreria);
    }

    QString cadenav;
    if (externo.isEmpty())
       cadenav=basedatos::instancia()->cadvenci_subcuenta(cuenta);
     else
        cadenav=basedatos::instancia()->cadvenci_externo(externo);

    int dias=cadenav.section(',',0,0).toInt();

    QDomDocument doc("Facturae");
    //QDomElement root = doc.createElementNS("xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\" "
    //                                       "xmlns:fe=\"http://www.facturae.es/Facturae/2014/v3.2.1/Facturae\"","fe:Facturae");
    QDomElement root = doc.createElement("fe:Facturae");
    doc.appendChild(root);

    QDomElement tag = doc.createElement("FileHeader");
    root.appendChild(tag);
    addElementoTextoDom(doc,tag,"SchemaVersion","3.2.1");
    addElementoTextoDom(doc,tag,"Modality","I");
    addElementoTextoDom(doc,tag,"InvoiceIssuerType","EM");

    QDomElement tag10 = doc.createElement("Batch");
    tag.appendChild(tag10);

    addElementoTextoDom(doc,tag10,"BatchIdentifier",cif+serie+numero);
    addElementoTextoDom(doc,tag10,"InvoicesCount","1");

    QDomElement tag2 = doc.createElement("Parties");
    root.appendChild(tag2);

    QDomElement tag20 = doc.createElement("SellerParty");
    tag2.appendChild(tag20);

    QDomElement tag201 = doc.createElement("TaxIdentification");
    tag20.appendChild(tag201);
    QString fisicas1caracter="0123456789XYZxyz";
    bool juridica=true;
    if (fisicas1caracter.contains(qcifempresa[0])) juridica=false;
    addElementoTextoDom(doc,tag201,"PersonTypeCode",juridica ? "J" : "F");
    addElementoTextoDom(doc,tag201,"ResidenceTypeCode","R");
    addElementoTextoDom(doc,tag201,"TaxIdentificationNumber",qcifempresa);

    QDomElement tag202 = doc.createElement("LegalEntity");
    tag20.appendChild(tag202);
    addElementoTextoDom(doc,tag202,"CorporateName",qnombre_empresa);

    QDomElement tag202A = doc.createElement("RegistrationData");
    tag202.appendChild(tag202A);
    addElementoTextoDom(doc,tag202A,"Book",fe_libro);
    addElementoTextoDom(doc,tag202A,"RegisterOfCompaniesLocation",fe_registro);
    addElementoTextoDom(doc,tag202A,"Sheet",fe_hoja);
    addElementoTextoDom(doc,tag202A,"Folio",fe_folio);
    addElementoTextoDom(doc,tag202A,"Section",fe_seccion);
    addElementoTextoDom(doc,tag202A,"Volume",fe_volumen);
    addElementoTextoDom(doc,tag202A,"AdditionalRegistrationData",fe_datos_adic);




    QDomElement tag2021 = doc.createElement("AddressInSpain");
    tag202.appendChild(tag2021);
    addElementoTextoDom(doc,tag2021,"Address",qdomicilio);
    addElementoTextoDom(doc,tag2021,"PostCode",qcp);
    addElementoTextoDom(doc,tag2021,"Town",qpoblacion);
    addElementoTextoDom(doc,tag2021,"Province",qprovincia);
    addElementoTextoDom(doc,tag2021,"CountryCode","ESP");
    QDomElement tag2022 = doc.createElement("ContactDetails");
    tag202.appendChild(tag2022);
    addElementoTextoDom(doc,tag2022,"Telephone",basedatos::instancia()->tfno());
    addElementoTextoDom(doc,tag2022,"ElectronicMail",basedatos::instancia()->email());

    QDomElement tag21 = doc.createElement("BuyerParty");
    tag2.appendChild(tag21);
    QDomElement tag211 = doc.createElement("TaxIdentification");
    tag21.appendChild(tag211);
    fisicas1caracter="0123456789XYZxyz";
    juridica=true;
    if (fisicas1caracter.contains(qcifempresa[0])) juridica=false;
    addElementoTextoDom(doc,tag211,"PersonTypeCode",juridica ? "J" : "F");
    addElementoTextoDom(doc,tag211,"ResidenceTypeCode","R");
    addElementoTextoDom(doc,tag211,"TaxIdentificationNumber",cif);

    // ojo puede no ser necesario
    if (!c_a_rol1.isEmpty() || !c_a_rol2.isEmpty() || !c_a_rol3.isEmpty())
      {
       QStringList roles;
       roles << c_a_rol1;
       roles << c_a_rol2;
       roles << c_a_rol3;
       QDomElement tag212 = doc.createElement("AdministrativeCentres");
       tag21.appendChild(tag212);
       for (int veces=0; veces< roles.count(); veces++)
       if (!roles.at(veces).isEmpty())
          {
           QDomElement tag2121 = doc.createElement("AdministrativeCentre");
           tag212.appendChild(tag2121);
           addElementoTextoDom(doc,tag2121,"CentreCode",roles.at(veces));
           QString num; num.setNum(veces+1); num=num.trimmed(); num="0"+num;
           addElementoTextoDom(doc,tag2121,"RoleTypeCode",num);
           QDomElement tag21211 = doc.createElement("AddressInSpain");
           tag2121.appendChild(tag21211);
           addElementoTextoDom(doc,tag21211,"Address",domicilio);
           addElementoTextoDom(doc,tag21211,"PostCode",codigopostal);
           addElementoTextoDom(doc,tag21211,"Town",poblacion);
           addElementoTextoDom(doc,tag21211,"Province",provincia);
           addElementoTextoDom(doc,tag21211,"CountryCode","ESP");
           QString descrip;
           if (veces==0) descrip="Oficina Contable";
           if (veces==1) descrip="Órgano Gestor";
           if (veces==2) descrip="Unidad Tramitadora";
           addElementoTextoDom(doc,tag2121,"CentreDescription",descrip);
          }
      }

    QDomElement tag213 = doc.createElement("LegalEntity");
    tag21.appendChild(tag213);
    addElementoTextoDom(doc,tag213,"CorporateName",razon);
    QDomElement tag2131 = doc.createElement("AddressInSpain");
    tag213.appendChild(tag2131);
    addElementoTextoDom(doc,tag2131,"Address",domicilio);
    addElementoTextoDom(doc,tag2131,"PostCode",codigopostal);
    addElementoTextoDom(doc,tag2131,"Town",poblacion);
    addElementoTextoDom(doc,tag2131,"Province",provincia);
    addElementoTextoDom(doc,tag2131,"CountryCode","ESP");
    QDomElement tag2132 = doc.createElement("ContactDetails");
    tag213.appendChild(tag2132);
    addElementoTextoDom(doc,tag2132,"Telephone",tfno);
    addElementoTextoDom(doc,tag2132,"ElectronicMail",email);

    QDomElement tag3 = doc.createElement("Invoices");
    root.appendChild(tag3);
    QDomElement tag31 = doc.createElement("Invoice");
    tag3.appendChild(tag31);
    QDomElement tag311 = doc.createElement("InvoiceHeader");
    tag31.appendChild(tag311);
    addElementoTextoDom(doc,tag311,"InvoiceNumber",numero);
    addElementoTextoDom(doc,tag311,"InvoiceSeriesCode",serie);
    addElementoTextoDom(doc,tag311,"InvoiceDocumentType","FC");
    addElementoTextoDom(doc,tag311,"InvoiceClass","OO");
    // InvoiceDocumentType FC = Factura completa
    // InvoiceClass OO = Original

    QDomElement tag312 = doc.createElement("InvoiceIssueData");
    tag31.appendChild(tag312);
    addElementoTextoDom(doc,tag312,"IssueDate",fecha_fac.toString("yyyy-MM-dd"));
    addElementoTextoDom(doc,tag312,"OperationDate",fecha_op.toString("yyyy-MM-dd"));
    addElementoTextoDom(doc,tag312,"InvoiceCurrencyCode","EUR");
    addElementoTextoDom(doc,tag312,"TaxCurrencyCode","EUR");
    addElementoTextoDom(doc,tag312,"LanguageName",fe_idioma.isEmpty() ? "es": fe_idioma);

    QDomElement tag313 = doc.createElement("TaxesOutputs");
    tag31.appendChild(tag313);

    q = basedatos::instancia()->select_iva_lin_doc (clave);
    if (q.isActive())
      {
        while (q.next())
        {
         QDomElement tag3131 = doc.createElement("Tax");
         tag313.appendChild(tag3131);
         addElementoTextoDom(doc,tag3131,"TaxTypeCode","01");
         addElementoTextoDom(doc,tag3131,"TaxRate",formateanumero(
                                 redond(q.value(2).toDouble(),2),false,true));
         QDomElement tag31311 = doc.createElement("TaxableBase");
         tag3131.appendChild(tag31311);
         addElementoTextoDom(doc,tag31311,"TotalAmount",formateanumero(
                                 redond(q.value(0).toDouble(),2),false,true));
         QDomElement tag31312 = doc.createElement("TaxAmount");
         tag3131.appendChild(tag31312);
         addElementoTextoDom(doc,tag31312,"TotalAmount",formateanumero(
                                 redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2),false,true));
        }
      }


    QDomElement tag314 = doc.createElement("InvoiceTotals");
    tag31.appendChild(tag314);
    // esperamos al final de la factura para confeccionar

    QDomElement tag315 = doc.createElement("Items");
    tag31.appendChild(tag315);

    // bucle aquí de líneas de factura
    q = basedatos::instancia()->select_lin_doc(clave);
    double totalgrossamount=0;
    double totaltaxes=0;
    if (q.isActive())
       while (q.next())
        {
           //"clave, codigo, descripcion, cantidad, precio, "
           //"clave_iva, tipo_iva, tipo_re, dto, notas, ci, pedido, albaran, expediente, contrato"
        QDomElement tag3151 = doc.createElement("InvoiceLine");
        tag315.appendChild(tag3151);

        if (!q.value(11).toString().isEmpty())
          addElementoTextoDom(doc,tag3151,"ReceiverTransactionReference",q.value(11).toString());

        if (!q.value(13).toString().isEmpty())
           addElementoTextoDom(doc,tag3151,"FileReference",q.value(13).toString());

        if (!q.value(12).toString().isEmpty())
           {
            QDomElement tag31511 = doc.createElement("DeliveryNotesReferences");
            tag3151.appendChild(tag31511);
            QDomElement tag315111 = doc.createElement("DeliveryNote");
            tag31511.appendChild(tag315111);
            addElementoTextoDom(doc,tag315111,"DeliveryNoteNumber",q.value(12).toString());
           }


        bool precision=basedatos::instancia()->ref_precision(q.value(1).toString());

        addElementoTextoDom(doc,tag3151,"ItemDescription",filtracadxml(q.value(2).toString()));
        addElementoTextoDom(doc,tag3151,"Quantity",q.value(3).toString());
        addElementoTextoDom(doc,tag3151,"UnitOfMeasure","01");
        QString cadprecio;
        if (precision)
            cadprecio=formateanumero_ndec(
                redond(q.value(4).toDouble(),4),false,4);
           else
            cadprecio=formateanumero(
                redond(q.value(4).toDouble(),2),false,true);
        addElementoTextoDom(doc,tag3151,"UnitPriceWithoutTax",cadprecio);
        double totallin=q.value(3).toDouble()*q.value(4).toDouble();
        QString cadtotallin=formateanumero_ndec(
                    redond(totallin,4),false,4);
        addElementoTextoDom(doc,tag3151,"TotalCost",cadtotallin);
        addElementoTextoDom(doc,tag3151,"GrossAmount",cadtotallin);
        totalgrossamount+=totallin;

        QDomElement tag31512 = doc.createElement("TaxesOutputs");
        tag3151.appendChild(tag31512);
        QDomElement tag315121 = doc.createElement("Tax");
        tag31512.appendChild(tag315121);
        addElementoTextoDom(doc,tag315121,"TaxTypeCode","01");
        addElementoTextoDom(doc,tag315121,"TaxRate",formateanumero(
                                redond(q.value(6).toDouble(),2),false,true));
        QDomElement tag3151211 = doc.createElement("TaxableBase");
        tag315121.appendChild(tag3151211);
        addElementoTextoDom(doc,tag3151211,"TotalAmount",cadtotallin);
        QDomElement tag3151212 = doc.createElement("TaxAmount");
        tag315121.appendChild(tag3151212);
        double liniva=totallin*q.value(6).toDouble()/100;
        totaltaxes+=liniva;
        QString cadliniva=formateanumero_ndec(
                    redond(liniva,2),false,2);
        addElementoTextoDom(doc,tag3151212,"TotalAmount",cadliniva);

        if (!q.value(9).toString().isEmpty())
          addElementoTextoDom(doc,tag3151,"AdditionalLineItemInformation",filtracadxml(q.value(9).toString()));

       }

    QString cadtotalgrossamount;
    QString cadtotaltaxes;
    cadtotalgrossamount=formateanumero_ndec(
                        redond(totalgrossamount,2),false,2);
    cadtotaltaxes=formateanumero_ndec(
                redond(totaltaxes,2),false,2);
    double invoicetotal=totalgrossamount+totaltaxes;
    QString cadinvoicetotal;
    cadinvoicetotal=formateanumero_ndec(
                redond(invoicetotal,2),false,2);

    addElementoTextoDom(doc,tag314,"TotalGrossAmount",cadtotalgrossamount);
    addElementoTextoDom(doc,tag314,"TotalGeneralDiscounts","0");
    addElementoTextoDom(doc,tag314,"TotalGeneralSurcharges","0");
    addElementoTextoDom(doc,tag314,"TotalGrossAmountBeforeTaxes",cadtotalgrossamount);
    addElementoTextoDom(doc,tag314,"TotalTaxOutputs",cadtotaltaxes);
    addElementoTextoDom(doc,tag314,"TotalTaxesWithheld","0");
    addElementoTextoDom(doc,tag314,"InvoiceTotal",cadinvoicetotal);
    addElementoTextoDom(doc,tag314,"TotalOutstandingAmount",cadinvoicetotal);
    addElementoTextoDom(doc,tag314,"TotalExecutableAmount",cadinvoicetotal);

    QDomElement tag101 = doc.createElement("TotalInvoicesAmount");
    tag10.appendChild(tag101);
    addElementoTextoDom(doc,tag101,"TotalAmount",cadinvoicetotal);

    QDomElement tag102 = doc.createElement("TotalOutstandingAmount");
    tag10.appendChild(tag102);
    addElementoTextoDom(doc,tag102,"TotalAmount",cadinvoicetotal);

    QDomElement tag103 = doc.createElement("TotalExecutableAmount");
    tag10.appendChild(tag103);
    addElementoTextoDom(doc,tag103,"TotalAmount",cadinvoicetotal);

    addElementoTextoDom(doc,tag10,"InvoiceCurrencyCode","EUR");

    if (forma_pago=="TRANSFERENCIA") {
      QDomElement tag399 = doc.createElement("PaymentDetails");
      tag31.appendChild(tag399);
      QDomElement tag3999 = doc.createElement("Installment");
      tag399.appendChild(tag3999);
      QDate fecha_transf=fecha_fac.addDays(dias);
      addElementoTextoDom(doc,tag3999,"InstallmentDueDate",fecha_transf.toString("yyyy-MM-dd"));
      addElementoTextoDom(doc,tag3999,"InstallmentAmount",cadinvoicetotal);
      addElementoTextoDom(doc,tag3999,"PaymentMeans","04");
      QDomElement tag39999 = doc.createElement("AccountToBeCredited");
      tag3999.appendChild(tag39999);
      addElementoTextoDom(doc,tag39999,"IBAN",iban);
    }

    QString xml = doc.toString();
    xml.replace("<fe:Facturae","<fe:Facturae xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\" "
                "xmlns:fe=\"http://www.facturae.es/Facturae/2014/v3.2.1/Facturae\"");
    xml.remove("<!DOCTYPE Facturae>");
    // -----------------------------------------------------------------------------------
    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString cadfich=serie+numero+".FACe.xml";
    qfichero=qfichero+cadfich;

    QFile fichero( adapta(qfichero)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

    stream << xml;
    fichero.close();

}

void fxmldoc(QString serie, QString numero)
{
    // generamos primero el xml
    QDomDocument doc("Documento");
    QDomElement root = doc.createElement("Documento");
    doc.appendChild(root);

    QStringList lista_suplidos=basedatos::instancia()->lista_ref_suplidos();
    double suplidos=0;
    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie, numero);
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

    QString qnombre_empresa, qdomicilio, qcp, qpoblacion;
    QString qprovincia, qpais_emisor, qcif, qid_registral;
    basedatos::instancia()->datos_empresa_tipo_doc(tipo_doc,
                                &qnombre_empresa,
                                &qdomicilio,
                                &qcp,
                                &qpoblacion,
                                &qprovincia,
                                &qpais_emisor,
                                &qcif,
                                &qid_registral
                                    );

    QDomElement tag = doc.createElement("Emisor");
    root.appendChild(tag);
    addElementoTextoDom(doc,tag,"NombreEmpresa",
                        filtracadxml(qnombre_empresa));
    addElementoTextoDom(doc,tag,"Domicilio",filtracadxml(qdomicilio));
    addElementoTextoDom(doc,tag,"Poblacion",filtracadxml(qpoblacion));
    addElementoTextoDom(doc,tag,"CP",filtracadxml(qcp));
    addElementoTextoDom(doc,tag,"Provincia",filtracadxml(qprovincia));
    addElementoTextoDom(doc,tag,"CIF",filtracadxml(qcif));
    addElementoTextoDom(doc,tag,"Pais",filtracadxml(qpais_emisor));
    addElementoTextoDom(doc,tag,"ID_REGISTRAL",filtracadxml(qid_registral));






    QString documento, cantidad, referencia, descripref, precio, totallin, msbi;
    QString mstipoiva, mstipore, mscuota, mscuotare, totallineas, totalfac, msnotas, venci;
    QString cif_empresa, cif_cliente, msnumero, msfecha, mscliente, msdescuento, msuplidos, msretencion;
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
                                   &cif_empresa,
                                   &cif_cliente,
                                   &msnumero,
                                   &msfecha,
                                   &mscliente,
                                   &msdescuento,
                                   &msuplidos,
                                   &msretencion);
    // añadimos mensajes de los documentos al archivo xml
    QDomElement tag11 = doc.createElement("Etiquetas");
    root.appendChild(tag11);
    addElementoTextoDom(doc,tag11,"MSJ_Documento",filtracadxml(documento));
    addElementoTextoDom(doc,tag11,"MSJ_Cantidad",filtracadxml(cantidad));
    addElementoTextoDom(doc,tag11,"MSJ_Referencia",filtracadxml(referencia));
    addElementoTextoDom(doc,tag11,"MSJ_Descrip",filtracadxml(descripref));
    addElementoTextoDom(doc,tag11,"MSJ_Precio",filtracadxml(precio));
    addElementoTextoDom(doc,tag11,"MSJ_Total_linea",filtracadxml(totallin));
    addElementoTextoDom(doc,tag11,"MSJ_BI",filtracadxml(msbi));
    addElementoTextoDom(doc,tag11,"MSJ_Tipoiva",filtracadxml(mstipoiva));
    addElementoTextoDom(doc,tag11,"MSJ_Tipore",filtracadxml(mstipore));
    addElementoTextoDom(doc,tag11,"MSJ_Cuota",filtracadxml(mscuota));
    addElementoTextoDom(doc,tag11,"MSJ_Cuotare",filtracadxml(mscuotare));
    addElementoTextoDom(doc,tag11,"MSJ_Total_factura",filtracadxml(totalfac));
    addElementoTextoDom(doc,tag11,"MSJ_Total_lineas",filtracadxml(totallineas));
    addElementoTextoDom(doc,tag11,"MSJ_Notas",filtracadxml(msnotas));
    addElementoTextoDom(doc,tag11,"MSJ_Venci",filtracadxml(venci));
    addElementoTextoDom(doc,tag11,"MSJ_CIF_Empresa",filtracadxml(cif_empresa));
    addElementoTextoDom(doc,tag11,"MSJ_CIF_Cliente",filtracadxml(cif_cliente));
    addElementoTextoDom(doc,tag11,"MSJ_Numero",filtracadxml(msnumero));
    addElementoTextoDom(doc,tag11,"MSJ_Fecha",filtracadxml(msfecha));
    addElementoTextoDom(doc,tag11,"MSJ_Cliente",filtracadxml(mscliente));
    addElementoTextoDom(doc,tag11,"MSJ_Descuento",filtracadxml(msdescuento));

    // --------------------------------------------------


    QDomElement tag2 = doc.createElement("Datos_cabecera");
    root.appendChild(tag2);
    addElementoTextoDom(doc,tag2,"Serie",filtracadxml(serie));
    addElementoTextoDom(doc,tag2,"Numero",filtracadxml(numero));
    addElementoTextoDom(doc,tag2,"Fecha_fac",filtracadxml(fecha_fac.toString("dd/MM/yyyy")));
    addElementoTextoDom(doc,tag2,"Fecha_op",filtracadxml(fecha_op.toString("dd/MM/yyyy")));

    // QString imagen=basedatos::instancia()->imagendoc(tipo_doc);
    QString descrip_doc;
    basedatos::instancia()->existecodigotipos_doc (tipo_doc,&descrip_doc);


    addElementoTextoDom(doc,tag2,"Tipo_doc",filtracadxml(tipo_doc));


    // obtenemos información de la cuenta cliente, para los datos de la factura
    if (externo.isEmpty())
       q=basedatos::instancia()->selectTodoDatossubcuentacuenta (cuenta);
     else
        q=basedatos::instancia()->selectTodoDatosexterno (externo);

    // "SELECT cuenta,razon,nombrecomercial,cif,nifrprlegal,domicilio,poblacion,";
    // "codigopostal,provincia,pais,tfno,fax,email,observaciones,ccc,cuota, ";
    // "venciasoc,codvenci,tesoreria,ivaasoc,cuentaiva,tipoiva,conceptodiario, ";
    //  "web, claveidfiscal, procesavenci "
    QString razon,nombrecomercial,cif,domicilio,poblacion,
      codigopostal,provincia,pais,codvenci;
    if (q.isActive())
     {
       if (q.next())
        {
         razon=q.value(1).toString();
         nombrecomercial=q.value(2).toString();
         cif=q.value(3).toString();
         domicilio=q.value(5).toString();
         poblacion=q.value(6).toString();
         codigopostal=q.value(7).toString();
         provincia=q.value(8).toString();
         pais=q.value(26).toString();
         codvenci=q.value(10).toString();

        }
     }
    QDomElement tag3 = doc.createElement("Destinatario");
    root.appendChild(tag3);
    addElementoTextoDom(doc,tag3,"Nombre_comercial",filtracadxml(nombrecomercial));
    addElementoTextoDom(doc,tag3,"Razon",filtracadxml(razon));
    addElementoTextoDom(doc,tag3,"Cif",filtracadxml(cif));
    addElementoTextoDom(doc,tag3,"Domicilio",filtracadxml(domicilio));
    addElementoTextoDom(doc,tag3,"Ciudad",filtracadxml(poblacion));
    addElementoTextoDom(doc,tag3,"Codigo_postal",filtracadxml(codigopostal));
    addElementoTextoDom(doc,tag3,"Provincia",filtracadxml(provincia));
    addElementoTextoDom(doc,tag3,"Nacion",filtracadxml(pais));

    QDomElement tag4 = doc.createElement("Detalle");
    root.appendChild(tag4);


    q = basedatos::instancia()->select_lin_doc(clave);
    double totalfactura=0;
    if (q.isActive())
       while (q.next())
        {
        QDomElement tag5 = doc.createElement("Linea");
        tag4.appendChild(tag5);
        // clave, codigo, descripcion, cantidad, precio, clave_iva, tipo_iva, tipo_re
        bool precision=basedatos::instancia()->ref_precision(q.value(1).toString());
        addElementoTextoDom(doc,tag5,"Codigo",filtracadxml(q.value(1).toString()));
        addElementoTextoDom(doc,tag5,"Descrip",filtracadxml(q.value(2).toString()));
        addElementoTextoDom(doc,tag5,"Notas",filtracadxml(q.value(9).toString()));
        addElementoTextoDom(doc,tag5,"Cantidad",filtracadxml(convacoma(q.value(3).toString())));
        if (precision)
            addElementoTextoDom(doc,tag5,"Precio",filtracadxml(formateanumero_ndec(
                redond(q.value(4).toDouble(),4),true,4)));
           else
            addElementoTextoDom(doc,tag5,"Precio",filtracadxml(formateanumero(
                redond(q.value(4).toDouble(),2),true,true)));

        addElementoTextoDom(doc,tag5,"Tipo_iva",filtracadxml(formateanumero(
                redond(q.value(6).toDouble(),2),true,true)));
        addElementoTextoDom(doc,tag5,"Tipo_re",filtracadxml(formateanumero(
                redond(q.value(7).toDouble(),2),true,true)));
        if (q.value(8).toDouble()>-0.001 && q.value(8).toDouble()<0.001)
            addElementoTextoDom(doc,tag5,"Descuento","");
          else
            addElementoTextoDom(doc,tag5,"Descuento",filtracadxml(convacoma(q.value(8).toString())));
        totalfactura+=q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100)+
         q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100)*q.value(6).toDouble()/100+
         q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100)*q.value(7).toDouble()/100;
        if (lista_suplidos.contains(q.value(1).toString()))
           {
             suplidos+= q.value(3).toDouble()*q.value(4).toDouble()*(1-q.value(8).toDouble()/100);
           }
        }


    // datos del pie de la factura
    // incluiría total fra. y desgloses por tipos de IVA

    QDomElement tag6 = doc.createElement("Datos_pie");
    root.appendChild(tag6);
    addElementoTextoDom(doc,tag6,"Con_ret",con_ret ? "true" : "false");
    addElementoTextoDom(doc,tag6,"Con_re",con_re ? "true" : "false");
    addElementoTextoDom(doc,tag6,"Tipo_ret",filtracadxml(convacoma(tipo_ret)));
    addElementoTextoDom(doc,tag6,"Retencion",filtracadxml(formateanumero(convapunto(retencion).toDouble(),true,true)));
    addElementoTextoDom(doc,tag6,"Notas",filtracadxml(notas));
    addElementoTextoDom(doc,tag6,"Pie1",filtracadxml(pie1));
    addElementoTextoDom(doc,tag6,"Pie2",filtracadxml(pie2));
    // -----------------------------------------------------------------------------------
    if (suplidos>0.001 || suplidos <-0.001)
        addElementoTextoDom(doc,tag6,"Suplidos",convacoma(formateanumero(redond(suplidos,2),true,true)));

    q = basedatos::instancia()->select_iva_lin_doc (clave);
    if (q.isActive())
      {
        if (q.next())
        {
         addElementoTextoDom(doc,tag6,"Base1",formateanumero(redond(q.value(0).toDouble(),2),true,true));
         addElementoTextoDom(doc,tag6,"Tipo_iva1",convacoma(q.value(2).toString()));
         addElementoTextoDom(doc,tag6,"Tipo_re1",convacoma(q.value(3).toString()));
         addElementoTextoDom(doc,tag6,"Cuota_iva1",formateanumero(
            redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2),true,true));
         addElementoTextoDom(doc,tag6,"Cuota_re1",formateanumero(
            redond(q.value(0).toDouble()*q.value(3).toDouble()/100,2),true,true));
        }
        else
            {
            addElementoTextoDom(doc,tag6,"Base1","");
            addElementoTextoDom(doc,tag6,"Tipo_iva1","");
            addElementoTextoDom(doc,tag6,"Tipo_re1","");
            addElementoTextoDom(doc,tag6,"Cuota_iva1","");
            addElementoTextoDom(doc,tag6,"Cuota_re1","");
            }
        if (q.next())
        {
         addElementoTextoDom(doc,tag6,"Base2",formateanumero(redond(q.value(0).toDouble(),2),true,true));
         addElementoTextoDom(doc,tag6,"Tipo_iva2",convacoma(q.value(2).toString()));
         addElementoTextoDom(doc,tag6,"Tipo_re2",convacoma(q.value(3).toString()));
         addElementoTextoDom(doc,tag6,"Cuota_iva2",formateanumero(
            redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2),true,true));
         addElementoTextoDom(doc,tag6,"Cuota_re2",formateanumero(
            redond(q.value(0).toDouble()*q.value(3).toDouble()/100,2),true,true));
        }
        else
            {
            addElementoTextoDom(doc,tag6,"Base2","");
            addElementoTextoDom(doc,tag6,"Tipo_iva2","");
            addElementoTextoDom(doc,tag6,"Tipo_re2","");
            addElementoTextoDom(doc,tag6,"Cuota_iva2","");
            addElementoTextoDom(doc,tag6,"Cuota_re2","");
            }
        if (q.next())
        {
         addElementoTextoDom(doc,tag6,"Base3",formateanumero(redond(q.value(0).toDouble(),2),true,true));
         addElementoTextoDom(doc,tag6,"Tipo_iva3",convacoma(q.value(2).toString()));
         addElementoTextoDom(doc,tag6,"Tipo_re3",convacoma(q.value(3).toString()));
         addElementoTextoDom(doc,tag6,"Cuota_iva3",formateanumero(
            redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2),true,true));
         addElementoTextoDom(doc,tag6,"Cuota_re3",formateanumero(
            redond(q.value(0).toDouble()*q.value(3).toDouble()/100,2),true,true));
        }
        else
            {
            addElementoTextoDom(doc,tag6,"Base3","");
            addElementoTextoDom(doc,tag6,"Tipo_iva3","");
            addElementoTextoDom(doc,tag6,"Tipo_re3","");
            addElementoTextoDom(doc,tag6,"Cuota_iva3","");
            addElementoTextoDom(doc,tag6,"Cuota_re3","");
            }
      }
    // -----------------------------------------------------------------------------------
    /*QDomElement tag7 = doc.createElement("Desglose_iva");
    tag6.appendChild(tag7);

    q = basedatos::instancia()->select_iva_lin_doc (clave);
    if (q.isActive())
       while (q.next())
        {
         // "sum(cantidad*precio*(1-dto/100)), "
         // "clave_iva, max(tipo_iva), max(tipo_re) "
        addElementoTextoDom(doc,tag7,"Codigo",filtracadxml(q.value(1).toString()));
        addElementoTextoDom(doc,tag7,"Base",q.value(0).toString());
        addElementoTextoDom(doc,tag7,"Tipo_iva",q.value(2).toString());
        addElementoTextoDom(doc,tag7,"Tipo_re",q.value(3).toString());
        addElementoTextoDom(doc,tag7,"Cuota_iva",formateanumero(
                q.value(0).toDouble()*q.value(2).toDouble()/100,false,true));
        addElementoTextoDom(doc,tag7,"Cuota_re",formateanumero(
                q.value(0).toDouble()*q.value(3).toDouble()/100,false,true));
        }
    */
    totalfactura=redond(totalfactura-retencion.toDouble(),2);
    addElementoTextoDom(doc,tag6,"Total",formateanumero(totalfactura,true,true));

    QString xml = doc.toString();
    // -----------------------------------------------------------------------------------
    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString cadfich=serie+numero+".xml";
    QString rutagraf=qfichero+tipo_doc+".png";
    qfichero=qfichero+cadfich;

    QFile fichero( adapta(qfichero)  );

    if ( !fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    stream << "<?xml-stylesheet type=\"text/xsl\" href=\"automat2html.xslt\"?>\n";

    // bool esmysql = ( basedatos::instancia()->cualControlador() == basedatos::MYSQL );

    // ------------------------------------------------------------------------------------
    stream << xml;
    fichero.close();

    // generamos imagen del logo
   /* QPixmap foto;
    if (imagen.length()>0)
          {
           QByteArray byteshexa;
           byteshexa.append ( imagen );
           QByteArray bytes;
           bytes=bytes.fromHex ( byteshexa );
           foto.loadFromData ( bytes, "PNG");
           foto.save(rutagraf,"PNG");
          }*/

}


QString nombre_fich_factura_apunte(QString qapunte) {
    QString serie, numero;
    if (!basedatos::instancia()->pase_en_facturas(qapunte,&serie,&numero)) return QString();
    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie, numero);
    QString tipo_doc;
    if (q.isActive()) {
        if (q.next())
            tipo_doc=q.value(12).toString();
          else return QString();
    } else return QString();


    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString cadfich=serie+numero+".pdf";
    qfichero.append(cadfich);
    return qfichero;
}


QString latex_doc(QString serie, QString numero)
{
    bool coma=haycomadecimal();
    bool decimales=haydecimales();

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
    q = basedatos::instancia()->select_cabecera_doc (serie, numero);
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
        } else return QString();
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


    int tipo_operacion=basedatos::instancia()->tipo_operacion_tipo_doc(tipo_doc);

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

    QString descripdoc=basedatos::instancia()->descrip_doc(tipo_doc);
    QString moneda;
    if (basedatos::instancia()->es_euro_tipo_doc(tipo_doc))
      {
        moneda="\\texteuro ";
      }
    int lineas_doc=basedatos::instancia()->lineas_doc(tipo_doc);
    // obtenemos información de la cuenta cliente, para los datos de la factura
    if (externo.isEmpty())
      q=basedatos::instancia()->selectTodoDatossubcuentacuenta (cuenta);
     else
        q=basedatos::instancia()->selectTodoDatosexterno (externo);

    // "SELECT cuenta,razon,nombrecomercial,cif,nifrprlegal,domicilio,poblacion,";
    // "codigopostal,provincia,pais,tfno,fax,email,observaciones,ccc,cuota, ";
    // "venciasoc,codvenci,tesoreria,ivaasoc,cuentaiva,tipoiva,conceptodiario, ";
    //  "web, claveidfiscal, procesavenci "
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


    QString qfichero=dirtrabajodocs(tipo_doc); // el directorio será dirtrabajo() + ruta tipo doc
    qfichero.append(QDir::separator());
    QString rutagraf=qfichero+tipo_doc+".png";
    QString cadfich=serie+numero+".tex";
    qfichero.append(cadfich);

    // generamos imagen del logo
    QString imagen=basedatos::instancia()->imagendoc(tipo_doc);
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


    QFile fichero(qfichero);
    if (! fichero.open( QIODevice::WriteOnly ) ) return QString();
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex_graficos();
    stream << margen_extra_latex();

    QString cabecera_factura; // asignar la cabecera factura
    QString pie_factura; // asignar el pie factura

    if (!imagen.isEmpty())
      {
       cabecera_factura="\\begin{flushright}\n";
       cabecera_factura+="\\includegraphics[width=80pt]{";
       if (WINDOWS) rutagraf=rutagraf.replace("\\","/");
       cabecera_factura+= rutagraf  +"}\n";
       cabecera_factura+="\\end{flushright}\n";
      }

    cabecera_factura+="\\begin{center}\n";
    cabecera_factura+="{\\LARGE \\textbf {";
    cabecera_factura+=filtracad(documento);
    cabecera_factura+=" \\newline \\newline}} \\\\ \n";
    cabecera_factura+="\\end{center}\n\n";

    cabecera_factura+="\\begin{flushleft}\n";
    if (!nombre_empresa.isEmpty())
        cabecera_factura+="\\bf {\\Large " + filtracad(nombre_empresa)+"} \\\\\n";
    if (!domicilio.isEmpty()) cabecera_factura+=filtracad(domicilio)+"\\\\\n";
    cabecera_factura+=filtracad(cp)+" - ";
    if (!poblacion.isEmpty()) cabecera_factura+=filtracad(poblacion)+"\\\\\n";
    if (!provincia.isEmpty()) cabecera_factura+=filtracad(provincia)+"\\\\\n";
    if (!pais_emisor.isEmpty()) cabecera_factura+=filtracad(pais_emisor)+"\\\\\n";
    cabecera_factura+=filtracad(mscif_empresa)+": "+cif+"\\\\\n";
    cabecera_factura+="\\end{flushleft}\n\n";

    cabecera_factura+="\\begin{flushleft}\n";
    cabecera_factura+="\\hspace{5cm}\n";
    cabecera_factura+="\\begin{tabular}{|p{8cm}|}\n";
    cabecera_factura+="\\hline\n";

    if (!razon.isEmpty()) cabecera_factura+="\\bf {\\large "+ filtracad(razon) + "} \\\\\n";
    if (!domicilio2.isEmpty()) cabecera_factura+=filtracad(domicilio2) + "\\\\\n";
    cabecera_factura+=filtracad(codigopostal) + " - " + filtracad(poblacion2) + "\\\\\n";
    // qWarning(pais.toAscii().constData());
    // QString elpais=basedatos::instancia()->descrip_pais(pais);
    // qWarning(elpais.toAscii().constData());
    cabecera_factura+=filtracad(provincia2) + "\\\\\n"; // + " - " + filtracad(elpais) + "\\\\\n";
    if (!pais.isEmpty()) cabecera_factura+=filtracad(pais) +  "\\\\\n";
    if (!cif2.isEmpty())
       cabecera_factura+=filtracad(mscif_cliente) + ": " + filtracad(cif2) + "\\\\\n";
    cabecera_factura+="\\hline\n";
    cabecera_factura+="\\end{tabular}\n";
    cabecera_factura+="\\end{flushleft}\n\n";
    cabecera_factura+="\\vspace{0.1cm}\n";

    // fecha y número de factura
    cabecera_factura+="\\begin{flushright}\n";
    cabecera_factura+="\\begin{tabular}{|l|r|l|r|}\n";
    cabecera_factura+="\\hline\n";
    cabecera_factura+="\\rowcolor{gray!30}\n";
    cabecera_factura+="\\scriptsize ";
    cabecera_factura+=filtracad(msfecha)+" & \\scriptsize " +
                      fecha_fac.toString("dd-MM-yyyy") + " & \\scriptsize " ;
    cabecera_factura+=filtracad(msnumero)+" & \\scriptsize "+ serie+numero + "\\\\\n";
    cabecera_factura+="\\hline\n";
    cabecera_factura+="\\end{tabular}\n\n";
    cabecera_factura+="\\end{flushright}\n\n";
    // cabecera_factura+="\\vspace{0.1cm}\n";
    // codigo, descripcion, cantidad, precio, descuento, total
    cabecera_factura+="\\begin{center}\n";
    cabecera_factura+="\\begin{tabular}{|l|p{7cm}|r|r|r|r|}\n";
    cabecera_factura+="\\hline\n";
    // cantidad, referencia, descripref, precio, totallin
    cabecera_factura+="\\rowcolor{gray!30}\n";
    cabecera_factura+="\\scriptsize "+ filtracad(referencia)+ "&"+
                      "\\scriptsize "+ filtracad(descripref) + "&" +
                      "\\scriptsize " + filtracad(cantidad) + "&" +
                      "\\scriptsize " + filtracad(precio) +
                      "&" + "\\scriptsize " +filtracad(msdescuento) + "&" +
                      "\\scriptsize " + filtracad(totallin) + "\\\\\n";
    cabecera_factura+="\\hline\n";

    q = basedatos::instancia()->select_lin_doc(clave);
    // double totalfactura=0;
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

    // Pie de la factura
    // escribimos notas de factura e identidad registral
    pie_factura += "\\multicolumn{5}{|r|}{\\scriptsize ";
    pie_factura += totallineas + "} & "+
              "\\scriptsize " + formateanumerosep(redond(sumalineas,2),coma,decimales) + moneda + "\\\\\n";
    pie_factura +="\\hline\n";
    pie_factura +="\\end{tabular}\n";
    pie_factura+="\\end{center}\n\n";
    double totalfactura=redond(sumalineas,2);
    if (tipo_operacion==1) // operacion convencional
       {
        // base imponible, tipo, cuota, precio, descuento, total
        // pie_factura +="\\vspace{0.1cm}\n";
        pie_factura+="\\begin{flushright}\n";
        if (con_re) pie_factura += "\\begin{tabular}{|r|r|r|r|r|}\n";
           else pie_factura += "\\begin{tabular}{|r|r|r|}\n";
        pie_factura += "\\hline\n";
        pie_factura+="\\rowcolor{gray!30}\n";

        // cantidad, referencia, descripref, precio, totallin
        if (con_re)
          pie_factura += "\\footnotesize " + filtracad(msbi) + "&"+
                         "\\footnotesize " + filtracad(mstipoiva)
                         + "&" + "\\footnotesize " + filtracad(mstipore) + "&"
                         + "\\footnotesize " +filtracad(mscuota) + " & " +
                         "\\footnotesize " + filtracad(mscuotare) + "\\\\\n";
          else
              pie_factura += "\\footnotesize " + filtracad(msbi) + "&"+
                             "\\footnotesize " +filtracad(mstipoiva)
                             + "&"
                             + "\\footnotesize " + filtracad(mscuota) + "\\\\\n";
        pie_factura += "\\hline\n";
        q = basedatos::instancia()->select_iva_lin_doc (clave);
        if (q.isActive())
            while (q.next())
             {
             // "sum(cantidad*precio*(1-dto/100)), "
             // "clave_iva, max(tipo_iva), max(tipo_re) "
             pie_factura  += "\\footnotesize " + formateanumerosep(redond(q.value(0).toDouble(),2),coma,decimales);
             pie_factura += " & ";
             pie_factura  += "\\footnotesize " + formateanumerosep(redond(q.value(2).toDouble(),2),coma,decimales);
             pie_factura += " & ";
             if (con_re)
               {
                pie_factura  += "\\footnotesize " + formateanumerosep(redond(q.value(3).toDouble(),2),coma,decimales);
                pie_factura += " & ";
               }
             pie_factura  += "\\footnotesize " +
                     formateanumerosep(redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2)
                                         ,coma,decimales);
             totalfactura+=redond(q.value(0).toDouble()*q.value(2).toDouble()/100,2);
             if (con_re)
               {
                pie_factura += " & ";
                pie_factura  += "\\footnotesize " + formateanumerosep(redond(q.value(0).toDouble()*q.value(3).toDouble()/100,2),
                                              coma,decimales);
                totalfactura+=redond(q.value(0).toDouble()*q.value(3).toDouble()/100,2);
               }
             pie_factura += "\\\\\\hline\n";
            }
        pie_factura +="\\end{tabular}\n";
        pie_factura+="\\end{flushright}\n";
       }

    // retención y total factura
    // con_ret
    // tipo_ret
    // retencion
    //pie_factura +="\\vspace{0.1cm}\n";
    pie_factura+="\\begin{flushright}\n";
    pie_factura += "\\begin{tabular}{|r|r|}\n";
    pie_factura += "\\hline\n";
    if (con_ret)
      {
        pie_factura += "\\footnotesize " + mretencion; // QObject::tr("Retención IRPF (");
        pie_factura += " ("+tipo_ret;
        pie_factura += ")";
        pie_factura += " & ";
        pie_factura += "\\footnotesize " + formateanumerosep(redond(retencion.toDouble(),2),coma,decimales);
        pie_factura += "\\\\\\hline\n";
        totalfactura -= redond(retencion.toDouble(),2);
      }
    if (suplidos>0.001 || suplidos<-0.001)
       {
        pie_factura += "\\footnotesize "+msuplidos+ " & \\footnotesize " +formateanumerosep(suplidos,coma,decimales);
        pie_factura += moneda + "\\\\\\hline\n";
       }
    pie_factura+="\\rowcolor{gray!30}\n";
    pie_factura += "\\footnotesize " + totalfac;
    pie_factura += " & ";
    pie_factura += "\\footnotesize " + formateanumerosep(totalfactura,coma,decimales);
    pie_factura += moneda + "\\\\\\hline\n";
    pie_factura +="\\end{tabular}\n";
    pie_factura+="\\end{flushright}\n\n";

    if (!notas.isEmpty())
     {
      pie_factura+="\\begin{center}\n";
      pie_factura+="\\begin{tabular}{|p{15cm}|}\n";
      pie_factura+="\\hline\n";
      pie_factura+="\\scriptsize " + notas+"\\\\\n\n";
      pie_factura +="\\hline\n";
      pie_factura +="\\end{tabular}\n";
      pie_factura +="\\end{center}\n";
     }
    if (!pie1.isEmpty()) pie_factura+="\\scriptsize " + pie1+"\\\\\n";
    if (!pie2.isEmpty()) pie_factura+="\\scriptsize " + pie2+"\n\n";
    pie_factura+="\\begin{center}\n";
    pie_factura+="\\tiny " + id_registral+"\n\n";
    pie_factura+="\\end{center}\n";
    // repetimos para imprimir cuadro de la factura
    int linea=1;
    q = basedatos::instancia()->select_lin_doc(clave);
    if (q.isActive())
       while (q.next())
        {
        if (linea==1) stream << cabecera_factura;
        bool precision=basedatos::instancia()->ref_precision(q.value(1).toString());
        stream << "\\scriptsize " << filtracad(q.value(1).toString()); // codigo
        stream << " & ";
        stream << "\\scriptsize " << filtracad(q.value(2).toString());
        if (!q.value(9).toString().isEmpty()) stream << "\n\n" << q.value(9).toString(); // descripción
        stream << " & ";
        QString qcantidad=q.value(3).toString();
        if (qcantidad.contains(".000")) qcantidad.remove(".000");

        stream << "\\scriptsize " << convacoma(qcantidad); // cantidad
        stream << " & ";
        if (precision)
           stream << "\\scriptsize " << formateanumero_ndec(redond(q.value(4).toDouble(),4),coma,4); // precio
          else
            stream << "\\scriptsize " << formateanumerosep(redond(q.value(4).toDouble(),2),coma,decimales); // precio
        stream << " & ";
        stream << "\\scriptsize " << (q.value(8).toDouble() !=0 ? q.value(8).toString(): ""); // descuento
        stream << " & ";
        if (precision)
            stream << "\\scriptsize " << formateanumero_ndec(redond(q.value(3).toDouble()*q.value(4).toDouble()*
                                    (1-q.value(8).toDouble()/100),4),coma,
                                    4);
           else
            stream << "\\scriptsize " << formateanumerosep(redond(q.value(3).toDouble()*q.value(4).toDouble()*
                                    (1-q.value(8).toDouble()/100),2),coma,
                                    decimales);
        stream << "\\\\";
        stream << "\\hline\n";
        linea++;
        if (linea>lineas_doc)
           {
            // pie cuadro y factura
            linea=1;
            stream << pie_factura;
            // nueva página
            stream << "\\newpage\n";
           }
        }

    if (linea==lineas_doc) stream << pie_factura;

    if (linea<lineas_doc && linea>1)
       {
         // completamos cuadro fra. y pie
        for (int veces=linea; veces<lineas_doc; veces++)
           {
            stream << " & & & & & \\\\";
            stream << "\\hline\n";
           }
        stream << pie_factura;
       }

    // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

  return qfichero;
}


double total_documento(QString serie, QString numero)
{
    QSqlQuery q;
    q = basedatos::instancia()->select_cabecera_doc (serie, numero);
    QString tipo_ret, retencion;
    bool con_ret=false, con_re=false;
    int clave=0;
    double totalfactura=0;
    if (q.isActive())
    {
       if (q.next())
        {
         // "select serie, numero, cuenta, fecha, fecha_fac, fecha_op, "
         // "contabilizado,contabilizable, con_ret, con_re, tipo_ret, retencion, "
         // "tipo_doc, notas, pie1, pie2, pase_diario_cta, clave "
        con_ret=q.value(8).toBool();
        con_re=q.value(9).toBool();
        tipo_ret=q.value(10).toString();
        retencion=q.value(11).toString();
        clave=q.value(17).toInt();
        } else return 0;
    }
    // int tipo_operacion=basedatos::instancia()->tipo_operacion_tipo_doc(tipo_doc);
    q = basedatos::instancia()->select_lin_doc(clave);
    double sumalineas=0;
    if (q.isActive())
       while (q.next())
        {
         double suma=q.value(3).toDouble()*q.value(4).toDouble() *
                    (1-q.value(8).toDouble()/100);
         sumalineas+=suma;
        }

    totalfactura+=sumalineas;
    q = basedatos::instancia()->select_iva_lin_doc (clave);
    if (q.isActive())
        while (q.next())
         {
         // "sum(cantidad*precio*(1-dto/100)), "
         // "clave_iva, max(tipo_iva), max(tipo_re) "
         // QMessageBox::warning( 0, QObject::tr("iva"),q.value(0).toString());

         totalfactura+=q.value(0).toDouble()*q.value(2).toDouble()/100;
         if (con_re)
            totalfactura+=q.value(0).toDouble()*q.value(3).toDouble()/100;
        }

    if (con_ret)
        totalfactura -= retencion.toDouble();

    return totalfactura;

}


bool genera_pdf_latex(QString qfichero)
{
    if (filtrartexaxtivado()) filtratex(qfichero);

    QString fichero=qfichero;
    // fichero.truncate(qfichero.length()-4);
    QString cadarg=fichero;

    QObject *parent=NULL;

     QStringList arguments;
     arguments << adapta(cadarg);

     QString cadexec="pdflatex";
     QProcess *myProcess = new QProcess(parent);

     int pos=qfichero.lastIndexOf(QDir::separator());
     QString directorio=qfichero.left(pos);

     myProcess-> setWorkingDirectory(adapta(directorio));

     myProcess->start(cadexec,arguments);

    // system(cadexec);
    if (!myProcess->waitForStarted ())
       {
         delete myProcess;
         return false;
       }
    if (!myProcess->waitForFinished ())
       {
         delete myProcess;
         return false;
       }

  // borramos ficheros .log y .aux
  pos=qfichero.lastIndexOf('.');
  QString raiz=qfichero.left(pos);

  QFile p(raiz+".log"); p.remove();
  QFile p2(raiz+".aux"); p2.remove();

  return true;
}

QString tipo_asiento_general()
{
    return QObject::tr("GENERAL");
}


QString logodefecto()
{
#ifndef SMART
    QString logo="";
#else
 QString logo=
            "89504e470d0a1a0a0000000d49484452000000c800000060080600000058173d1d0000000473424954080808087c086488000000097048597300000ec400000ec401952b0e1b0000200049444154789ced9d797c55c5ddffdfdf39e76e594880802ceed6aaad5dacadf5a9adb19a04dcbb415dea56ab800a512a46457b898a084895b084a04fb5fb53b03fb5ee04b75a15ad58f7a7ae8f5b41658740729733dfdf1fe7dc8d4502242c7a3faf17243967e6cc9cb9f3b933df75842200d8ebb6ea688793ec11493965293103d5e12ba007007b1be82faa5528e51809012a90109155827e82d5ff60e42d51792dadf6151c677924125af3ced0f9ab76f47b15b16d901ddd811d8a99b57dc10c12c77edf15dd5f604f57ec0023b80238a218515c51424631a85f2f1835510151441524f81d50cb2a317c80eabb22bac05abdffbdb39efed78e7acd22b61e9f2f822842d3e030d1f41978fc12630e0cae83a2c18cdf6475234ac4b1c41c4bd4f17032d7b17e35f149e373455555318e880282aeb1e8cda9b0bdf9c35717aca411dbddaf5bc4b6e3f34190969a3d5133183819380e20580cfcff256f1c142df83bef065a78d9314ad858a2c623e228116371445140d47f9431822a6a4405154414451e53f40ed749dff7faa9cfbcdb0d6f5c4417e1b34d90e6c15f42bdc98856a3a604b3a9f7f5bfe337fa67b0abcafe6d553122d9eb412109d69e52d7a33294266caccfb5a0a8f18ba964c9a78ae81a545ad77ac9618bcefae7b22e78e322ba189f3d824caf2ec30d7d1b953822df4375092a1f217c82f00af01a1e6fe29af749b57fcc458fb76df439b79e544e6a5d3f8cb317d6ee077c193818952a44ab40fa006e1e910a5816762c95a11451c71232367b23b38b137f89f17f02aa729ba24d6ffeeca917ba69648ad80a7cb608d25c7729e8b920ef03f700cf62ec47105dcab07bd775491b2d279490b27d08d9be78f6100cc7839e0062800d88628c123596ca709a32279ddbd305c4405123be9c8fe85ad027adb117bf79ea33ffdb25fd2d629bb0eb13245eed3220fa55d4bb02e46e86b7fe6187f46366cd20c45c08fa3d90528410009a5171f9abca6e9124116331c6d70c1008f528082862c5ff5b66244289b1ef0c5d585415ef40ecda0469393484ad1a8812e182875edfd1dd01e0b6ea4a3aa2d56007034311e905f8b28b3ff58938968a508a8a503a2bd467898255f15718417509c83442ab26be36f4b5e48e7aa5cf33766d82ecccd0606c67d50c01b91991fefef56045096c26fda2492a4229fcab026291408d260aaaaa38f289889ef8ea294fff73c7bdd0e713bb0c41f6baad3ada01fd44b50f22bd45b597880c54b1fd10aa0c54aa4a196889888444ada8112b2a49d0b522b489b252b14b0cce27c062c42ef7ac2c17bc6525693e79fd174fade9b6176819743c9e3d0f916310ca80ac862ce27854455294bbe93ce15d324647358eff39a97a2d8837fed5539ffba0dbfa594401766a82f4bffd7b758ad488528de86e6a2526a22522c454c41132d26d76f742d6a82de4a957832fedc02208c17dc543e8c06a3b46d689ca1ac5fe4b847f889547ffefac7fbcd1a52f148f1b063e3380b4578fc8a53e03722b4ad828fda209cadd341a6cb954c148c636631158a1962b5e397d414b97f6ad888d62e7208822ccaeee1d22d4bfd47827184787a01c923341f85fa046d4df85e4d91d0c8a88e65e24d02115d81fc89045b305440a8a676d14820d266720352b8f08dca39e3c4238f571cc5db3a24be4815bea7a91d669889e00d20300eb33bd4728cd6ed1046109c86f329d0de41310c5cef75cf7dcd7863ef9fe36f7a5884d62c712a4a5a6022ba7201c8bca5710dd37d0e56c1ac16c36f8ae1f46fc9f215142c612319690c999c9337687cd12848cb748401002a35e1ef3d4b248843751f9178ef7e0dba73ffdd0368f4173cdfe20e722d2000a5614418c2815a134fda2095ca3a8e6c9279a11f8f56385292f9fba60f236f7a3888d62fb1344119a6bbf8c700322c777cd330b2de12210733c4a5d8fa8c9b93c19d9028208d932e495415525b33c298a63eff090a69e6d1dcf2c3c7f611af9547a6f1aff3db80f493b17e548dfe21f90c5a8ec11eba022ec21d8401d9c7519532388b5fa28e97527be74e64b6bb7aaed223689ed4790e6c15f02ef64444e030eeed6b6f2092310311e51a3441c2feb2fe56f9f02ab36d9a29b2748f0bb5a544cb025f36589a5601ec4d26a42e9bf6fb58fd5ac9a63b0d28091dafc15a52c94a64f24490f37676c44c0581483a0ba52d14b5e3cf599dbb7aadd22368aee27c8ecda7d48d382e1bf5029f367d7469c0421e728b84987c1ad40fef77920ab94381e15e1341163031b048110bf0504c97ab85b9f67c1b2a2aa9e60d62afa9aa74c78fb8ca7feb6c57d9e33c461c58affc2f2ff10d3c7974d1009b65d7b96b4071d118cdf17bf0b6a558d3cfa82d9a38ea173bd2d6eb7880dd03d048957bb0c081f8aa70d18f3c3ec7555504921ba1295360c6bb174203609268da8efe1a7e220ea02219028680942292ae508a5feb3f27abf4584ca79e5bac6d2239426e6584241ec07e89611446cb6f1c0201ec83a123465d7aa325591b96d6b136f2c1eb6b0f32e2ff1b8a1dfd33701a723f4ce10c588b247490715a1344672fdc55a152322d8368bfcf485539eb9bfd36d15b151743d415a6abe8b351310fd4e204b83c893a08f6165018e2c02598549b691f0d6b2840e1a1f4f6ff45973863874ac8c92724a486a29c62bc7732a410f020e43f408440e2c604ba7c992238a042ad672374d452895b99b551dc396122468c16a60c350ac95a4a06f227227e9d4cdffde12efdde9755fc0d57120a7e76fbb4a5c8f3d621d94ba9eaf00568b202a0651b569c5dcdf1e353f7bfd07dd68dff98ca3eb0832ad66002eb3c09c88ea2a8c3c02cc66f8bc07bbac8d4d6166cd208c0cc58ff528034ad6731edc703bb73ef2dcda7b8552f408a730e47bdd1692a5530451f27dae7c3a0aa228aafc45b057a77af67cffade31e4c74ee3d077d0fa37f0606e613a55f2c41df4812477c858448a0e9f2a34f9659ccb12ffcf4e9e7b65a81f039c6b61364fa71fd30c9eb30e6fb280f60f46f84937fe79cc73bbaa07f5b8e96ba0351fd2670242a8310f6ccdeebccea9289aa15a5d4f52877d394baf9db79d93a82e435ecab9f33735517a8983bd5737effef339e58bcd9f79b5e5d4628721156c7225296d976451d8f7ed124bd23c9ac7a5b058cda8c3de796a4176e78f9f47facd86c1b4564b16d0499597b0a700d46aea657c51d3b9560a808e3e2c2c00507e1795721e614728eb59bad4c36fad6b7b1f48d268939feeb894a46aedf0682a0824aa0955044d2a073053bfa95d39ef978b35d9c3ab80f117b2770045635d06a495928cd7e65edb8e22b1b8c5a3fc0cb6f794d5aec892f0c7deef14e8ee2e71e5b479099b50763f47894b58c983fbd8bfbd43d8857bbf40b9f05fc10e4db0855402756959cac1271fc68c132d70bbc7065eb0992299fe1ac6f1e07b1287aa7815b3c27fac46b433711d0954173dd99885e0df2859ced041918eba02a92242c36b3c70c946d8a22bf497a5cfb6231dc77b3d87282b4d4fc026b5ec2845fe9b220a4ed8939431c962deb87b875a85e83c8ee40e789227eb693ca508adee194bf1464b45e6c334154c48aaa00d613e44355f9ef574e7ffada4f7da75b4f2a27d95e8fc8b50864b65d2163d9a3a49daa480a4b4011b272d5328449cf0e7d76d2d60ce3e7059d27c8cce37be224bfc5b0d679ddd89fed0f5fc0bf063810951e593b4d27c86244e91b4b50ea78b8b9d059b2ae535b4a10023b0c82c9f2ce5f5614fb2be3c8ed2fbeba60f12633a2cc3ee68b58e77fc07e0d8cc9f4b3229c62af920e228e0de474df95c6df2bda051e9cfddc90e7de280af11b62f3045184dbab2b488694619fe14468cd755f43f547089720525e6868d918725baf50604fe91d49f936908cffd7b61024d74c602404abba1ae4618337ebc5539fddf81755bcdaa57fe4c728bf461880fa6644312abd2229f68cf944090ca4811ca46990b969ece50b872edc359d1f6fa9ebc579f39677f5633b4790cfdb37cbacba06d03810010c88bf6df9b4ac28810770bfa8ef0e9299fd264867b2d5040910b8befb1ec7bed6f81dabe6ec97fffdd4931b5d517c9fb7168c9c976d35500bef5ed241ff58c227b328c68aaaf82e66169d948ed8c685276d8141734741119a077d11478f66d8bce6ee68a2fb5d4d76554c393c4659f979583903e19b9b5d5020ab228e38969ee114156e1ac728da5504c9d4d35c0022eabdac6a6e4b4bfa8e8d0652cdac3b14d15f219c9425ba2011c7d22f96a04f38e9a72852203032a2f663456620eeef9efec9d3ef6dcdf0753b66d41c8f9133501672416bb779331709b2394c1d1c21a48721fa5b847d366f78cc6cb11447a05f34e187d44281ebfd361184acff1522d677d0813615b98f8ec88817cf797c6541e578b5cb80d821a877272a03b3b58d8a1118184bb07bac3d705791ac9bbfa86db7c25d292ba3ff39f49f1f75d1886e1b9a6b4e45642288c1ca31dd9d8ba048902d4173dd99a09722f2156033db2ec87815c75c8fbe9124258ee7dbb6f3b55e5b4b1072e54182d0dc4028b2f207c4ce081bf3e282a10bdad77b87b1a81d8e31bef62e4826e118658f92767a85d3bebd473312960a0614fdabaa3343c579f5e91f3ffdc9368de396a0e58412bcc4be0827029720d207b5f3d0c8295c705fb71b3d8b04d952dc561d25113a02e47a90c3804fd77a655cef45893996be9124e5a1343688ebe83282a81f9aab820449b6536ae5352bfafb174e79664ac1c35aaaabb0915380c908d1fcd444aeb194873c064413f40efb5679bfc1a083d6be0bbcacf0fb7f0c5938775b87739398597338c69c81e57b88ee8750824a07e8190c6ffdebf6928b8b04d916ccacfb31e80c842a449c4fdf7ee5b45e256e9a81b1042151ff73f6f74adb4e10b3811d460d2a88dab49571366c5a0ef0765f3637e3f1e06bbca6839e8e4a69c0d8c0791e091bcbee251df48d267133dece8a4a10fa0c60551f13953938ee03b1882e77d35e7af162529b0d1e53e4d0d987bafbf60cb96bca6ce8ff56ba659e95a3de6a2b3919e52788e48640d503f907a57a3c67b6764d50d8f4ea3212098f5faeb7c2ae872241b615738638ac58f9333cbd0463beb6f90a39357045606c2c75bd40ed9ab3cc77094100a3fe8c0f26f45a541ef00c77fc6be8337fc93ebca5664fac9c8e703ecade81ca2cbbaa88283d42697a843c2add54264420f0f2caf545adac10b1ef212c426509a2cbc5cf3fdca1a29e2b6a4425ea182d77a0675bdaa9faa423dc7f6532b4e71acfedbb3aed66fd7008c603d12750bd8111f3bbc4757fc81c9cfb561e337a5d2af418173eb8d9344a45827425661ef3638cf90d488f4f3736e65613801ee1140363095cd1ecbcec328290e74d6c7c4f6254ac281d6a7462e9dad8a4c7338ea5f1b8a1df532703cd88ece6d723e7c016d04180a8b1f48ea4e8194ed1c34d6bd4b1bea74c5e028da0b866126d786a58997459960ac92789306bd34eaea304a4f40b0bca328c9ec6f9adad5db59d3af1ee43befa465becaed7d7949cc7f0f90f77a64e91205d8d3943c22c5d3916e12c44f602321ffc66b45ed03792a0329ccac6d16703a1e802829017879fd92665da40ff68d5fc251992675eca08e0be87c159a81c8eb04f5e770b15de99c92dfe51102147fd83878282568594153aac011bf42cb34204069d3cf55e0a780af4f78c68fdef2d1ffc8d63f0dd877cd5f3f4e277d6959df3f6caf2f3b9e8815b3a5bb74890eec26dd59524226783de147cf89feebe12cc19c72815217f45c94cb2cce4ee32820475b3118a062cd6132b9f6058603d3be1b9539ef3b71f2dd555d8f001c0b90867f991f8d90682772a5c113f15c28663a1ac046ec4d13fd1ee2ca2be93f1319bc177effd6ecff254dbadc670cc3f5754547cd21139764be3938a04e96e4c393c4649f9adc0718854029dd67a0d8826a80ca5718d0dc48242a7c86d254846cd6c4cb6e98c508f453f1695c98e67ef0a39ed1f3f3ef435dfabb8a5e6bb58198ac8f7819e6810a09649d6bdb1f7c92d5d1eb00e68436425300f91df31eca1e7b76264378a435b0e0df5e9c39e6ab4de888e4c788667975724da92ce302e68fded963eaf4890ed85e6c15f42ec2f50bd242b046f6edb85efe75519f6f363e54eaf92acd6b54b09129457f5a928a258d5b4887945e03915fbb7053f79ee9e6c375baaab483b0390505fb05508fd41aa8032d0107e22afb5c07294c5185d82279f60ddff70d1fd5d6b7854a4e6aeaf9f6bd4fc548c1e1132c43eea08f3f2caf27422e5fc800b1fba6f6b1e5b24c8f6c64dd595c422ff035a4b7e58f066057aa55f2c49df48d22f18845a750741b2e5735dc86eac14c5a83c64e1bf55cdbd7bb04772ee90b91937e1ed074586cc1d62569837f7c531973ba23f17cd9a5e797b6d4cde5c55b206c3b18c687d726b9b2912644761e6a0ef81370a313f66b3a98e722b8a6b2c5581f628161cf3a6c1bd6e230814384be6e9b550f55e14e159b1f20222af7be2bed55dfe5b87fdfeb01e25e5de41ae7a5f41cde122fa7d23ec9bb9ef28dae639f2bf6bca58d21e7a17474e67d8bca7b6a5cd22417624e60c7158bafa0b88f71bc47c67b3e5f3bea38d2815e1347bc4da7145b18a7f6028dd4890827a7e86493fdba3643453092061908462df52e159f59ce71db12f2fd5d237371b1d9987eadbaaa34e8fb62f61bc4344e570d41c25502546232a1a31605425d8128a1aa3b2685d545f5d53269ee5255caf8ef31ed97ce8f2665024c8ce8219b57518c621f22dc0ddbcd62ba76dad8a24e91d4e52e2daac1d229335b2db0892fddd66fb919f513f63ef934cbad75c77d7004b45a50da31d2869458c412308a5a25461b467a69d0cf904c965b3945c827103b47b0e6fb595f09f755140e7134d9ed85549438a04d99930674898652bbf85c83528472374c221d2ff21a2943896aa882fa72860c9ac26db9f20d97ac1f3fcc92d9998f8f556b9dc03fd60977cc366615f8353b8100107786b4d89beb73646528da07a191f7f670a8d8d5d76067d91203b2b66d47d07430b70c02655a8eb23204b26f3626528852b4ae6c0d0ec192aec0082e465ab5cff3905e5a5b0fdf5b67518f189bf26edf2c28a724d788e20b423fa6386b53ed0a971da021409b2b36356dd60e05ce0270081544e2e95cffac809f4bed1314dcf508acab04f16cb7af6945d8120c156cd004b92613e581bd5258970663dbc0f2b23b8705eb79cba5524c8ae82e9c7ed85936e4638362041a7d315114cae3ed12403a209dff00805daafa0c6ce4510cdf57d55dae5b55565ac493bb9f732f63c86cdbfb5b343b83528126457c3ccda83318c41a905e91fccb24e678c44a12c9ca677384599eb11733cff801ef227eb8e25887fe29c2f7caf4c86f8a03dc2aa8e902f8bf92ef78f62b49e61f35fded2e1db521409b2abe296a37723ed9e843001e80d6c1951f06515d7f8aeec7d22492a43e9ac5e803c4749e86682e4ad1422ca924498f7d7455993724959937b2fd57588398da43cd855fe5a9bc3d613a4b9a61e31872376f80e4907346bd070b0df21e19cb7bd066ba7454bdd69285780ee0d5216902090cc37f719e76fc5fcc4ddbd2229cadc34ae049eb9a28564215f8ed91282e49e93795cda0a5685759ec3c789081fb587b12a59937d50b20de52eccb29f336c616a6b87696be06e754d91cb81752475eb9fb12d507b0df009e995eb874e7cfe306cde9f803fd132e81ba83d0ed17351b377e7b65f9939e813657932c4f264080422c6120ece7d2c713d4a1c4bccf188b936906302ad9866b848c14fc81d7b27a2782ab4a71dd67a0e6bd30e6d69870ecfa1dd33a46dfec7a8816a5bc0ea74c46b66c423af75d5706d09369cdcf1b861bfa762ace5eb08bd48f3017d2b5f63c8dc14f9fe364a0f44df271cfbd490c56e83481f549fa52cb5f1b3453e8ff0bd629f07aea3a5a61a6bae038e40d5664358374918d9603f91b0868435ac517c1b797e69816870a45d3838d6ce35996029c12a782a24ad90b48676cf216d37d6ac16f629230e59fe86e79dcbc8873b7f8e4a37a0b0c7cd75b50817a3d6827ce00faa1e027218ca1f30894b18f6f852bf6c6d3bf00f3efacea0ae34cc740a8ad05267516e65f8bcf3d9de8e72db8ae69afd11e9cff0d6bf777f5b470f449c1ff88647f92ed0377b6fab8fbacbdb966d69bdfcbdda0646507d11957b30a9db19f6e8db5bd1409723b782ccaabd10f42aac398aaa9e6f658f329833c461c5aa6f034f42a805f807b75547e94010566d777200ccaef68552d18f773972f8988aea2aa0fb0932e291ff0033983364169f7c12c3840e061981c81905e4d822b26cb8da74ba5ea62d5f3acaecd1fe84d11b20fa36c377ae84e83e4166d69d004cc7e3682e5c2f11974f94a798557b351a2cd3ab63e5843d41650725133315c12f9b3f70666784984341276ed736fdcfb10d5810fc3b8b96da6fa372327024a2bb017d417a6c50776b561a45b3e9e473c4005889b018f837f01786cffbcb46ebef2470993ab807e24d02eee6c2d647375952e449acfaab4a28558e3806dd411354dc72ac026cb3b7e60e425f2c2fece84e30acf519e0192008ad75fa22a63f56be81e19b285f45e4c08d9223e77c98f747de450996196511a2cfa3b200b10bb0ce87246531f50faeeef6f7eb02b8b8f6608483f09c533eb5e4b07939f23852eaebe264fb65d8cb874769e080b07487b4bf2d68a9d9334835bd63c66e53f065cba5c06b4061c68fdfd596d26e0ec0d37d300c00ad42a40288821a9434e83a9015288b71e47d52c937b9e8d1f776c09b74295c1cce04e0c2075fea742d8f528c0a9e2cd9e09e1f837d0eb02fa26fd1abe72d597966fa71fd70d3d588ee4687d35260bf6819bc1f9ef70d46b4deb151b9a2b9e654c47c1dd565401baa16c7166a38a60eee41c4fe1c651fe0034ce276863dbed43fdb24558dd21709ff8161f7aef30fd2597906429a61ad7fccb6d952f3152c4350798c0b5a1fd9a01f336a0ec2911f63a517a22f31a2f5f6ecbddfd596b2d6d480b73769fd03231f5e963d53506525c31f9a05f26db01e621ce60c09b3a2dddde90f22f293b56534649f2b18ac5663e58d2dab2625600c8e169ec770537525a5e5cfe19f349b46b999a5abde63eae008336b8fc6494e46b501ab9388da430bea5a3b0c238d8c8bafa7593b7a20b36a17a3a68eb4fc0ee421e068c0236d72499a67d61c4ed87b0bd1d781df0083d1f0874c3ba63792b809cbf9a8b6904a0e60eae008cb56dd0f721bc8ef69a93d0180964135a87909e46a8c3c4cf3d1030bfa32abeed7182ec67217469703b731abc64f4f735b759475fc11b5c3812938a6160027b210ab13503b8de69ac7f1180f78c06d2c5bf92e36f924f178d196b393c245d817f4952daa65280505cfcb4dd02987c788455e07fd05235afdc0fee641cf63f42f84bdc98c681d053cc2acba73506ef1b768f9b05f43294c463cf3f89e907a0a4b9c0be6cdce5e6fae5d8870228ef5f7b12d2794a0c93b107e9add0aceaabd06e461a2ea72dec367d3525381ca4ac2a603cf3b1b2481ca9160ff029ccdd4c1f350ef46447e88c87e2837823318f0093073d019a09730627ea6dfafd05c7b10989f03e706013a3f6076edd7b166306a57d05c773be8bf809f2352c3f0d6469aeba6002763967d3b6715fe6c1ddaf5598241711029dfa25a6a622010727207d4979437a0aa0c6fcd65bd18f1d01c94ff033d9adfd596fa75350662410ac32f558e01292488495e03bc58400e1f478278f4ece93fc3acf385762fd82e0248e4393cf6cd865d262905014f5d84cb30e153b8e0a12710790ca584a8771222931836ef2e441f064d83e6bed9c5b6a3404bddf7f3fafc218a32e5f058f69a4769605ade17d5dd18d17a0a235a9f64f8bc46ff39ba1fca3a526591ce0d76113b122e860f81fdb7ac9a2df175e19ebf77f6ed227508f337282a3c009c4a7be09222f4c2e2e1d81c19660cfe2ae201ac635ca3d248600c948bc0feb4e079cd470f44643f80ac6c73de231fd35cf728c2d934d7ee0e4e3d3d636f31f4de5c8c804b29d0063a0e955f31fcde7528c22c5c84c5a8fc10373a0cf027b9a8012727488f68bd838c8ae696ba5e24dc3026d51f858204c84a04832272389e8ede603c943e081d78b1edea5354c4d6c1a0f2180a340f3aaad3b5c4440148c59200a42325c0de20ffd9a0acea6a54da714a7cdd8d6537c47ac42427608bbd00781091dc39ebcd753ff275e9ce7a8130a123502d3c2006a077c5b9a88e063d12f15e65d9cabb993d78f75c3f4c8f40dbf24d46ccfb63aea22d031988ea1bfce26ffe8a28f4026330b2a8a08d5935c730abe6cf787a2e6eaa06952f82143a61184a037b51828eb677371c3be9814892e5ed3bcf99f2456c1206952007aaded4e95aeaf9db0327e14ffa0e4f405dd0fe1b9415d90b917fd3239299480310f1b269ec67d6f645b40d218ce2e4eae917c1a610c9f95a29027638be91a91043e72619d17a13664519c84dc0603c2f4feba215a09588fe297b695c5cc094205a47da4ecb7b5a052858cd69e95a6a47a1f200921ac9f0d6c9a49377211a45b4d0166329f57dc6650da3176c98384009a31bd1d215b153c2e0841682fe13e1eb34d79ebbc992b36abe49cb607f6b2399838e537eac7485e900f900956337a867f538943b193ad75f6d442a50c9774f198298bfa29422f4ce6ab1840462c2785e1590f1bf9a8f328af5939f4f1ddc83965abfed610b530c9f371a951311fad05c136c1fa567e09f7d4f415dd14ae0dd02a738d12808980e5fc6b9f5a472df9d9ca6ac2f9a13ae45652f944267cd8c3c27ba896395b503b1317a243a17675ec40e8561d8bdeb70eccf82531c5bfc388bf5d05c3b1e355348b5fbdf9642c2d76285fb0110adecc057c554d1523b2a57afe63c8c2c63c4bce9d96b7edd129a079d4c73ed05183d93e10f3d0dbc8e7264d6b7cbcae37e0fe5529a8ffe12b36a1fc6da3f7341eb2ba8b4a3d6d052e3bb9c84533f40e57e6eabaeccb6a3e67dacae63c4fc37fde7d83ea84d902447842fbf26a8f4c45298d0d8fa2ea688db0b80647b0c9572d06ff9ef5577a6efcba4f780ec5d3858b6026b3d60e39a41d597b1721065657d005f7e6b39b448969d14bee07cfec36f307bf01e58fb3fa8cea4b97612c2e3585c440e407880e1f3aa73d5f46d545218e6d05cf70a2b965ec18847af6056cd61587323b3eafe0bf443e06bc8b2030b5a54bd0db406d1bb5059443ae4274cf3cff5fe19cdb54f0277933453097bf311a901f75554a77341107f2cbc0a2682e54166d5bc8d3011cb3324c2efd05c7733d8e5e0fd08233fccb62ba612f475ca92eb19e5545179b6e092e8bb2009d2e65e9a6b5fc03005d5bf829cc9cc3a0bfc9de1f38e6246dd0f309c4873ed5f115d88677e8f6a0c918ff1121bf746759c5fe3d9d3b0bcc6ccdafb68c7c5ab3c1fd82a21a01800000c6c49444154a16edd456c1c85b68896434348d517f174006a63186709c2bb0c7ba8d0e72a5eed32c03d84b4b4f1c911af67bff5e70c7158b1fa6b78b6126415ceb297361a013663f057315e39d1e4abe49fc8da5cb33fe25465dbf4adf2df006963f8bc97b25b96dfd596d2cec1905e9a758b9e5e5d8671f7c7482f8404a9f05b050992a71fd78f5032ccf9f33f28d8fa34d7ec8f89fea7c09a1daf76e91ffa3ac669e73f1dafd3f8789a5b4f2ac76bff129ea4a9aa7821ab419b39e8004cba1729798f91f31731fdb87e845331ce6b7d77939ec6cd83bf846a6fe093ee3ea5b588228a28a288228a28a288228a28a288228a28a288228af8aca39859f13380783cee8693cb7b26c3bd5634363616d32075218a04d9453167c810e7cd7dfbff549071a0fba364725fbda9e8b8b1139bfeb4b967ec4a983364883364ee5c2b14da965ace3f3f346cf6ec6ef38c2e46b2ed8298d050bff75bfbf47f5455061be5e42b2736c995939a1cc43900f411943f5e3ba6fed41dddcfaec284868bbef6d63e03debc7eccc823f3af5fd730f2c4a53d63c9ebc7d49fd45d6def98b4a1456c35e2a3475759d2cf2072c75513a75e987fefca1b7efd06307c7c437dcaf861cf9f095835df14d8c78643ef14de317d04f08c74faecc32d457105d985a02011277d1de08e5d8f1cf9f0ac366dc76e753f44f642e0eaf1bf2e880dba6ae2d4df1874afab27debc61728d2e429120bb10c68d1c598e9161a85cfe69e5bc929eefe1e8dfb657bfba13713fa1c59eaabad1c422974f6c7abf3bdb2f6eb176214462e66c8064cafcf5d3ca35363626592fa99e824c6c68e861a5cd75bc9264c3e4c9d97c0273860c715efd529f58ac23ed5c3e71f6aafcf2edd1355e63e3ccec1666ca2597c412916449e95ad3563f6d5a2253f6d7975c125d6d6d84cacab67c4dda0d0d0d152aab4d47a4efaac6c0a975ca2597c482b2eb82be663171cc98f27dde7d77ddd0b9be33e880c58b9da595d13d405ecd7f17019d3a7264a4231a0d5f3679729b80c6e37153d6d6166b2b4b3ae31aa7adc908f4937ff9cbd254a823da7b796af5c604fa783c1e66f5eab27211a322ed63a64c590b70fd65f54d452dd62e84f163ea9f403828ecb97b653ec4ce60eac89191b55166a84889080759cb5e6298347662d30d131a461e623127a33a18e14023a97daeb8a179c5f597d5dfaca2bf10e52311a9b962e2d477af19336a7fd7700fc8010abf4d462bcf67f9f268b844ce11cb412a72b6c265574d9ada0470fd65a32601f5002a3a7eecc469d78c6f1875b95a3910f407469876e5a4a6ab33fd8cc7e36eb863c5720fef885f4d9cf1f2f831a38e41a8453817e503449f164c15c2ffbbf286a97fb9be61d42ce05b890e39864a6b23ede64285a3040e77347d50c3e4198bae6b1879a251694618a8ca836327351504f58dbff4e22330f642853705f932d88341ee42098991d1c52dd6ae04a3872abaa2ad6c4da74376e3f1b8692b61a11867c1d8494da725a23dbf25f05b9009e31b46fdec8a89d3feb5ff3b8bae15f8832015a164cc193f66d47556f931ca5c15d95755cf017085c7adf26755fea56a8786932b7a364e9bb67aff77164f8fa49d4b40178bea1e00d7358cbcd5c2f156ec308b7aa2e6826bc78ede2319ed7963c4ba171ab4dcc25ef97d8d2456ef0d526e9cf072806449cf475d8d8c1770d5306becc4691726a2956724229573832a87a050e6baa9c6c669abaf9834f5066bed05083dd438ee8486fabd05331b18ab2a770b32f8dacb467e27d3de7597d67f1dc73e8ec3f5574d6a8a2763954345e431a0418457ac23038a04d98520484cb6705b1c695f71064aec8a8937df0ad0d8d89876f1268bb25a826ff7a173e77a6248a24a2292ac12f8fe5593a7ee317652d3398aa655a4d7f886513701cd574d6a6a14a3c30c1273d39e93a95fb6766d5a55d322b2fcbacb461e2e98f055939abe7cd5c4e9b78bc81588ee2649dba7b1b131dd56569640c00805da27abf6480144bd74d057bbae6495075269d077836bc9cc564d2d950aabdbcaca12fef8a01276d601a8a4d6284cc4dad3ae9cd4f45bd4dea4821a357dc1dfa619a3f582bc337642d32b99f6ac35b3052cc2b7aebafee6c54582ec4a505d8d487f703bffb9899c83484106f586c93316817d1fa4f78da3475701a8258c0858f30d45afca564710b4afc2ee574e6aba1640d3260c90769df533b38455f43d23f24b8c8ec95cf4d4cc4705c7a8af7a5eb9b20c15d46a416e65517eaaa8079a97b3c03528d8b4d93097b1913284b5f9328f6389a9e2a1a1a35491b193a73d0a601d27e467d3f6b3e9348d1c19b6e841aab620138f6bd6bdad228a1283a2166b97820a4f88128976389dce6366e1bbead90d344016f950552523a95bb531858f8043931d2c8440185615947dc5333767ea8ab037c8eacc373dc0a2fefdc58888288350797eec84a69c9240d53f1d146f15801bb2bdad111172c767c4e3179489680c249d4c84b25bc89275a58280aab7e139984a4c6ce1d9575e5a7b20ba4c55c789498fc85c37d6abf4dfd35b04104d24ac605620e6bf0ac645ca7613d451f113781709b20b4155a62160ad8cef6c1d5175c4381b2485304215b06adcaf7fbdccffdb44451145d38dd3a6ad0698d8d0d043555144c2eabc997da6a3bb5b65a5d1b202bf2f151c154ebe72d2d409f9d75d747f2c1de9b45d0920863e02a8e45685487ba85ed13f8b6a28124e6409629d3617206c74c3835a45c39a9f4b0d50871ea8f451e47fafbc61465e9cbf54004435fc11c0b0d9b35388fe512032bea17e5cb63d4d8f56e4deb1139bfe004582ec5248c52a1f55abff2b468e9dd0507fd4a6cacd1932c489c7e36100413a4428281b8fc78daa7e41e0ce9c6f9344157653e5ee6c415d572e2288ead2bdde7f3f3bd9d46a6f31bab63dba263b397b2d5f2eaa54a8ca8cf5fb638d9e8ed17f1ef0fe920ffda69cde02622c2bc1570523ba97c1795dc12452d16c7e34d11e5651d2ee8607fb888a42c1e99fb8d89e0262c414a6abb55a024abef66fecc4a63fa0fa7355ae1a7fd9a8e7c78f19f5a4b1a4c74e9c7a62a64c9120bb101a1b1b9302972a8a55fe3cfed28b8f58bfcc75632efee65bfb0ef85d78dd8aef01287a2770da8486fabd3365421dab8e059627623df357a25211b87ad2b4a732173c8f1e8a0822af65ec12001671b184062c4e67bfd597f75a2d46a4d4116f4e7e7fc65f31ea60a332149131b967784641adb1df9838664cb947c755aa3c628df79188100e7bd92da4934ea705412dd9491b8f9f150d7e5d0ba634bf3dcf9adea058635fcdbfee3b72165a356e1c3dba4a91a1c670ba1867683229c75f39b9e9a2fc324543e12e862b2737dd3f7eccc5470ade63087f1fdf50ff94a8bddb2231417f845ac74ba74fbc7aca4cfff01a8febd5e80f2dbae0ba86facb04b5602fb79e777446b88dc7e346db97f744a5e0380b75e821a20e42810c63b08b30e60b4b2ba3bfbdae6154c811d3d0c18a45a85a1573e7848691977424dd2722115bab1e37abca51636fb8f9994c7d312c108ba05c9b968e71587964ece4a631f178dc8db4af58425aff67c2a5a32ef61c4db645cb1e08b52fffa7883361fc65a3be884a1fda7509701ea26f88ca51d78da9ffbd880dedff7f1f9dfe96d8fe0a4b931d6e814150d1358230feb251ad0ad62a1725247da82083557966ecc49b367a145c7105d9053176f2cd4f44d26e99a27154ad624e1391c34475d2d8c94d5fcd9203183ba5e9156bd3078a31f30c8c1095c38ca4be975f665c63a31a91bfa1529059532d4b44f5979ee3de977fddd1688baafe1961612adaf3f42b6eb8f99d1eab7b0922c6f5d2dfb16a8e8b84bc56558eb521e7b0b1936f7ea2a0ff13a62d51d53a15794c30e3c74e6e3a017c15b4c59e20469e52c3d76349777e636363da313202d5fb05898ac384b1939ace03b0aa9759e16f62f481b193a69d3274ee5ccf629e17752e1eb076ed9afc36c3eadd6fadfd2dc2a3112ff4a35f4d6e7a33ec85ee017d5e207e7dc3281d7f59fdb3e3c78cba6e4243fd91f1fafa4a28c68314d14588d7d7578623ac48b6db8a8c90bfb363c2e5f5433c4f8762cced466d950ac7a0fa134162167d296cbd41c52d56115d0237647b23c6a3d78eee49e770fde5a38eb3566fb86a72d37e79977f0b9c39be61d42f45e546cf75ce2a6eb18ae81288a10f56db4bd64577faccf5f178dc60b9c7a8ac7f3013006327364d1125e9596d2f12a4882e8151d317a31deb4ad6eef4048975acd81d11e389463776ffda5f5eb40f46c2ae98bb8a0429a26be0e800b146cbda623b3d412e9fd8f4be557dd420a3c78f19f97d0d64f1783cee4e68b8f868e39afbacc739974f6c7abf288314d125103569157b5f3899dc25b2aaa4621d2784db63a7238cbbbe61549feba19df615fff190475cb4eef21b9b3e04f8fff5d9f36ed69c8e2f0000000049454e44ae426082";
#endif

 return logo;
}

QString icono_smart()
{
   return QString();
}

bool testNIF(QString a)
{
    QString dni=a.toUpper();
    int numero=dni.left(dni.length()-1).toInt();
    QString let=dni.right(1);
    let=let.toUpper();
    numero=numero%23;
    QString letra="TRWAGMYFPDXBNJZSQVHLCKET";
    QString lletra=letra.mid(numero,1);
    return (lletra == let);//devuelve true o false
}


bool testNIE(QString a)
{
    QString dni=a.toUpper();
    QString pre=dni.left(1);
    QString prev="0";
    if (pre=="X") {
        prev="0";
      } else if (pre == "Y") {
        prev="1";
         } else if (pre == "Z") {
              prev="2";
             }
    QString numero=prev+dni.mid(1,dni.length()-1);
    return testNIF(numero);//una vez tratamos el NIE lo comprobamos como un NIF
}

bool testCIF(QString cif) {
    int pares=0;
    int impares=0;
    QString suma;
    QString ultima;
    int unumero;
    QString uletra="JABCDEFGHI";
    QString xxx;
    QString texto=cif.toUpper();
    QRegExp regular ("^[ABCDEFGHJKLMNPQRSV]\\d{7}[0-9,A-J]$");
    if (! regular.exactMatch(texto)) {
        return false;
    }
    ultima=texto.mid(8,1);
    int cont;
    for (cont = 1; cont <7; cont ++) {
        QString conv; conv.setNum(2*texto.mid(cont++,1).toInt());
        xxx=conv+"0";
        // xxx = (2 * parseInt(texto.substr(cont++,1))).toString() + "0";
        //impares+=parseInt(xxx.substr(0,1))+parseInt(xxx.substr(1,1));
        impares+=xxx.mid(0,1).toInt()+xxx.mid(1,1).toInt();
        // pares+=parseInt(texto.substr(cont,1));
        pares+=texto.mid(cont,1).toInt();
    }
    QString conv; conv.setNum(2*texto.mid(cont,1).toInt());
    xxx = conv + "0";
    // xxx = (2 * parseInt(texto.substr(cont,1))).toString() + "0";
    // impares+=parseInt(xxx.substr(0,1))+parseInt(xxx.substr(1,1));
    impares+=xxx.mid(0,1).toInt()+xxx.mid(1,1).toInt();
    suma.setNum(pares+impares);
    // suma = (pares + impares).toString();
    // unumero=parseInt(suma.substr(suma.length-1,1));
    unumero=suma.mid(suma.length()-1,1).toInt();
    unumero = (10 - unumero);
    if (unumero==10) {
        unumero=0;
    }
    QString cad_unumero; cad_unumero.setNum(unumero);
    if (ultima == cad_unumero || ultima == uletra.mid(unumero,1))
        {
        return true;
        } else
           {
            return false;
           }
}


int isNifCifNie(QString dni)
  {
    QString cadena =dni.toUpper();//convertimos a mayusculas
    QString pre=cadena.mid(0,1);//extraemos el primer digito o letra
    if (pre=="X"||pre=="Y"||pre=="Z") {//Si el primer digito es igual a X,Y o Z entonces es un NIE
        if (testNIE(dni)) {//llamamos a la funcion testNIE(); pasandole por parametro el dni. Devolvera true o false
            return 1;//Si es true devolvemos 1, 1 = NIE correcto.
        } else {
            return -1;//Si es false devolvemos -1, -1 = NIE incorrecto.
        }
    } else {//Si no es un NIE comprobamos si es un CIF
        QRegExp letrasCIF("[ABCDEFGHJKLMNPQRSUVW]");//Estan son las letras por la que empieza un CIF
        if (letrasCIF.exactMatch(pre)) {//Si la primera letra de la cadena coincide con alguna del patron letrasCIF entonces es un CIF
            if (testCIF(dni)) {//llamamos a la funcion testCIF(); pasandole por parametro el dni. Devolvera true o false
                return 2;//Si es true devolvemos 2, 2 = CIF correcto.
            } else {
                return -2;//Si es false devolvemos -2, -2 = CIF incorrecto.
            }
        } else {//Si no es un CIF comprobamos si es un NIF
            QRegExp numerosNIF("[1234567890]");
            if (numerosNIF.exactMatch(pre)) {//Si el primer digito de la cadena coincide con el patron numerosNIF entonces es un NIF
                if (testNIF(dni)) {//llamamos a la funcion testNIF(); pasandole por parametro el dni. Devolvera true o false
                    return 3;//Si es true devolvemos 3, 3 = NIF correcto.
                } else {
                    return -3;//Si es false devolvemos -3, -3 = NIF incorrecto.
                }
            } else {//Si tampoco es un NIF entonces no es un dni valido de ningun tipo
                //si no es ninguno devolvemos 0
                return 0;
            }
        }
    }
}




QString CalculaDigitoCCC(QString Entidad, QString Oficina, QString Numero)
{
    /*
nsiString __fastcall TForm1::CalculaDigito(AnsiString Entidad, AnsiString Oficina, AnsiString Numero)
{
     AnsiString Cadena;
     int nResto;
     int aPesos[10] = {6,3,7,9,10,5,8,4,2,1};

     Cadena = "";
     nResto = 0;
     for (int nItem = 1; nItem <= (Entidad + Oficina).Length(); nItem ++)
          nResto += StrToInt((Entidad + Oficina).SubString(nItem, 1)) * aPesos[8 - nItem];

     nResto = 11 - (nResto % 11);
     if (nResto == 11)
          nResto = 0;
     else if (nResto == 10)
          nResto = 1;
     Cadena = FormatFloat("0", nResto);

     nResto = 0;
     for (int nItem = 1; nItem <= Numero.Length(); nItem ++)
          nResto += StrToInt(Numero.SubString(nItem, 1)) * aPesos[10 - nItem];

     nResto = 11 - (nResto % 11);
     if (nResto == 11)
          nResto = 0;
     else if (nResto == 10)
          nResto = 1;
     Cadena = Cadena + FormatFloat("0", nResto);
     return (Cadena);
}
//---------------------------------------------------------------------------
[editar]     */
     QString cadena;
     int nResto;
     int aPesos[10] = {6,3,7,9,10,5,8,4,2,1};

     nResto = 0;
     QString entidad_oficina=Entidad+Oficina;

     for (int nItem = 1; nItem <= entidad_oficina.length(); nItem ++)
          //nResto += StrToInt((Entidad + Oficina).SubString(nItem, 1)) * aPesos[8 - nItem];
          nResto += entidad_oficina.mid(nItem-1,1).toInt()*aPesos[8-nItem];

     nResto = 11 - (nResto % 11);
     if (nResto == 11)
          nResto = 0;
     else if (nResto == 10)
          nResto = 1;
     cadena.setNum(nResto); // = FormatFloat("0", nResto);

     nResto = 0;
     for (int nItem = 1; nItem <= Numero.length(); nItem ++)
          // nResto += StrToInt(Numero.SubString(nItem, 1)) * aPesos[10 - nItem];
          nResto += Numero.mid(nItem-1,1).toInt()*aPesos[10-nItem];

     nResto = 11 - (nResto % 11);
     if (nResto == 11)
          nResto = 0;
     else if (nResto == 10)
          nResto = 1;
     QString cadnum; cadnum.setNum(nResto);
     cadena = cadena + cadnum;
     return (cadena);
}

bool ccc_correcto(QString ccc)
{
 if (ccc.length()!=23) return false;
 // 1234-1234-12-1234567890
 QString control=CalculaDigitoCCC(ccc.left(4), ccc.mid(5,4), ccc.right(10));
 // qWarning(ccc.toAscii().constData());
 // qWarning(control.toAscii().constData());
 if (control==ccc.mid(10,2)) return true;
   else return false;
}


bool IBAN_correcto(QString codigo)
{
  QString cadena;
  cadena=codigo.mid(4)+codigo.left(4);
  QString abc="ABCDEFGHIJKLMNOPQRSTUVWXZ";
  QString destino=cadena;
  for (int veces=0;veces<cadena.length();veces++)
      if (abc.contains(cadena.at(veces)))
         {
          int pos=abc.indexOf(cadena.at(veces))+10;
          QString cadnum; cadnum.setNum(pos);
          destino.replace(cadena.at(veces),cadnum);
         }
  /*
In this example, the above algorithm for D mod 97 will be applied to D = 3214282912345698765432161182. (The digits are colour-coded to aid the description below.) If the result is one, the IBAN corresponding to D passes the check digit test.
1. Construct N from the first 9 digits of D.
N = 321428291.
2. Calculate N mod 97 = 70.
3. Construct a new 9-digit N from the above result (step 2) followed by the next 7 digits of D.
N = 702345698
4. Calculate N mod 97 = 29.
5. Construct a new 9-digit N from the above result (step 4) followed by the next 7 digits of D.
N = 297654321
6. Calculate N mod 97 = 24.
7. Construct a new N from the above result (step 6) followed by the remaining 5 digits of D.
N = 2461182
8. Calculate N mod 97 = 1.
From step 8, the final result is D mod 97 = 1 and the IBAN has passed this check digit test.
[edit]  */

  QString extract=destino.left(9);
  int num=extract.toInt();
  num=num%97;
  QString cadnum; cadnum.setNum(num);

  QString cad_proceso;
  cad_proceso=cadnum+destino.mid(9);

  while (cad_proceso.length()>9)
    {
     // buscamos otros nueve caracteres de cad_proceso
      extract=cad_proceso.left(9);
      num=extract.toInt();
      num=num%97;
      cadnum.setNum(num);
      cad_proceso=cadnum+cad_proceso.mid(9);
    }

  int valor; valor=cad_proceso.toInt();
  // QString rdo; rdo.setNum(valor % 97); qWarning(rdo.toAscii().constData());
  if (valor % 97 == 1) return true;
  return false;
}



/*

Validación de dígito de control IBAN:
A continuación vamos a validar el dígito de control de un número de cuenta de un cliente de Bélgica. Como podemos ver en la tabla de códigos de cuenta, el formato para este país es:

BE XX seguido de un número de 12 dígitos, siendo XX los dos dígitos de control. El mecanismo que habrá que realizar es el siguiente:

Mover los cuatro primeros caracteres del número IBA a la derecha:
Ej: BE89 9999 9999 9999 -> 999999999999BE89
Convertir las letras a números según la siguiente tabla:
A=10	G=16	M=22	S=28	Y=34
B=11	H=17	N=23	T=29	Z=35
C=12	I=18	O=24	U=30
D=13	J=19	P=25	V=31
E=14	K=20	Q=26	W=32
F=15	L=21	R=27	X=33

Ej: 999999999999BE89 -> 999999999999111489
Sobre el número resultante, calcular el módulo 97, si el resultado es correcto, la operación dará como resultado 1.

Generación del dígito de control IBAN:

Componer el código IBAN de cuenta con dos dígito de control 0.
Ej: IBAN BEXX 9999-9999-9999 -> BE00999999999999
Mover los cuatro primeros caracteres del número a la derecha.
Ej: BE00999999999999 -> 999999999999BE00
Convertir las letras en caracteres numéricos de acuerdo con la tabla de conversión.
Ej: 999999999999BE00 ->999999999999111400
Aplicar la operación módulo 97 y eliminar al resultado 98. Si el resultado consta de sólo un dígito, insertar un cero a la izquierda.
Ej: 98 - 9 = 89; IBAN = BE89999999999999

 */

QString digito_cotrol_IBAN(QString codigo)
{
    QString cadena;
    cadena=codigo.mid(4)+codigo.left(4);
    QString abc="ABCDEFGHIJKLMNOPQRSTUVWXZ";
    QString destino=cadena;
    for (int veces=0;veces<cadena.length();veces++)
        if (abc.contains(cadena.at(veces)))
           {
            int pos=abc.indexOf(cadena.at(veces))+10;
            QString cadnum; cadnum.setNum(pos);
            destino.replace(cadena.at(veces),cadnum);
           }
    QString extract=destino.left(9);
    int num=extract.toInt();
    num=num%97;
    QString cadnum; cadnum.setNum(num);

    QString cad_proceso;
    cad_proceso=cadnum+destino.mid(9);

    while (cad_proceso.length()>9)
      {
       // buscamos otros nueve caracteres de cad_proceso
        extract=cad_proceso.left(9);
        num=extract.toInt();
        num=num%97;
        cadnum.setNum(num);
        cad_proceso=cadnum+cad_proceso.mid(9);
      }

    int valor; valor=cad_proceso.toInt();
    // QString rdo; rdo.setNum(valor % 97); qWarning(rdo.toAscii().constData());
    // if (valor % 97 == 1) return true;
    // return false;
    int resultado=98-valor%97;
    QString cad; cad.setNum(resultado);
    if (cad.length()==1) cad="0"+cad;
    return cad;
}

QString logo_facturacion_defecto()
{
   QString logo=
    "89504e470d0a1a0a0000000d49484452000000c80000006408020000004ce4e85c000000097048597300000ec400000ec401952b0e1b00000c3749444154789ced9cdf6f13c716c76776bd76bc4b7e19120707c71020490926ad485a52a19644d036ea0b155255aaaaeffd8bfa52a92f55914a8b4a79a91484a8445b0850c7492b1a20b48d15620c2e8963c7716ccfdc87117b573bb3f6669369dc7bcfe7c91eefcc1cef7e7dce993393604a290280ed46d9690380ff4d405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880144058801440588014405880147c5b1f82528a3176f391d395354670fa94357ae8e8794606212493c9e472b962b158a9545455f5fbfd8661ecdebd7bd7ae5d7547de16aad5ead3a74fad36689a66184628146a6969916d83cbc1f1b6fc73db4f3ffd54d3345b63b95cfee4934fea5ed9dbdb3b3e3e5edb5c5b2febc8172e5c28140a7c97582c76e6cc991a364f4e4efef5d75f7cbb6118e7cf9f37df9a0a4ea7d3d3d3d38b8b8baaaa2a8a625a4b29a59412420281403c1e3f78f0a0611835e6bd76eddaa3478ff8f6b1b1b183070f0abb301b08214b4b4bd3d3d34b4b4b4e36a8aa1a8fc7fbfbfb9d6c28168b5f7cf18567d9099fa9906df05808a14020c00b4b51047196bf32954acdcecec6e3f11adab2f5b28e3c3c3cfcf3cf3ff31d5756566adbbcb2b21208046c8d94d2e1e161eb5b8c71a150f8f6db6fabd52ac698efc2a656559510924c267ff9e597c3870f8f8e8e0abf3e4248d3347e10e470bb4c1b565656befbee3bf6ba860d08a1df7efb6d6666e6a5975e7af5d557f9dbc2ba7b16969391822bbd4db08d288a9248244aa592876f4b298d4422d56a95ff68757555e8c918f97c7e7575956faf56abdddddda617c718dfbf7fffd2a54b4c556e4c5255757e7efeebafbf2e97cbdb120d30c6c964f2f2e5cbee039caaaa73737357ae5c4108edd4bf5bdf796131d88ddbec5dc0181b86d1d9d9c977541425994c3a759c9999e17f7c94d27038aceb3a8b7d08a1b9b9b91b376ea0172ec1bd55a552893998ad3c57d637994cdebd7b1763bc591b9697976fdcb8213be173a2218485315e5f5fbf7dfbb6f94437c5e8e828dfa828cac2c28270344ae9c2c282d0ab9b43618c8bc5e2ad5bb7f810cf46a8be801022b4aa50285cbb76cddb37326d585e5e4e2412756d104ec1dcad308ffc07d89e1c6beb608cefddbbb76fdfbe4824b2d9beadadad9aa6552a155b7ba5522997cb7ebfdfd65e2e97f98b29a59aa699ab2a84100b25fc6518e35028343232d2d1d1512c167ffdf5d7b9b939c479358c712a955a5e5e6e6b6bdbec37422ff2f1efbfff9e654ebc0d9148646464a4b5b5359d4efff8e38f854281774e9aa6ddbd7b37168bb999919f4868954bfb1b455808214551ae5ebd7afefc799fcfb72907aeaaeadebd7b1716166cbd0821e974baa7a7c7767d3a9d2684d82ea694767777b39bcb1276a7b4ef8d37de301f956118afbdf6dad0d0d0c58b17f91c4855d5ab57af9e3b77ce433c62ee6a6363439880bff5d65be17098bdedeaea3a77eedcd4d414d3b78d5c2eb7b4b4b477efdedad3050281f7df7fdf8d612e53bd860885562627273d848f13274e0843d2f4f4b4cb464aa935a4debe7d9bb781ad1562b198eda3a6a6a6f7de7b8f8fad2cc4572a156fd1706a6a8a6f248444a3d170386c1d93523a323262de3716253736367c3e5f4f4fcfdada9a87d99d70f92369208f8510c21867329999999963c78eb95f04514a9b9a9a42a190adc48031ce66b31b1b1bd668582a95b2d9accf67ffe27bf6ecd134cdac5a3d7af488cf6c0821a74e9d42a29b6b18463018e4e31121e4d9b367751d064fb55a5d5c5c1466576fbef9a6cd06f67a7878389148ecdab5aba5a5a5bbbb3b1a8d9a8589ba3773db73fc861096f56babaa9a4c26eb9619ad3029c462b1e9e9699bdb5055f5d9b367d6bc2d9bcdf2c9042164fffefda619abababc2d4bea5a58557a469437f7f3f5bbed93efae38f3f3c08cbc986d6d656a7dd8b81818181810127f36a4f572c163ffffcf3ba567df0c107c160b0ee65a81142e1f0f070b95cb6b6504a59e9c87d04c118c7e371db38e845066d6de153318450a552191c1c34dbd7d6d6844fa2b6d6bbbbbb85e1f8e9d3a775ede7595f5f17daa0ebbaf0fa2dba1c8cb1af1eaaaaba9f65e785a5ebfad8d898f591300ff4d34f3f6dea66298a22dc12999f9faff196d1d7d7679dab5c2e0ba7e6179856d85e218fb0125b1761da5ed786c661e7854508e9eded8d442256ff84317ef0e0c1c3870fdd8f4329edefefe79ddcfafa7a369b65afb3d96ca954e23b1e3e7cd8da5158ca47f516e42c45e3db793fea06a7f298fb4d959da521aca4948e8f8ffbfd7eeb83d134ede6cd9b4ef7970763dcd5d54508b13d5d9fcf677aa9f9f9795b92c4f66ec3e1309f0b7b40d8d1292df330d4bf88861016c65855d577df7dd7262342c8a54b97dc8fa328cae0e0203f78269361af9f3c79c23fb0575e7985af3f09c7e7cbaa568ac5a2b05db8e55c17271b9cbce916a1946ed46353bb9f0db12a4408514a9b9b9be3f1f8ececac794f31c66b6b6bc27d3d27e2f1f8efbfff6e6bfcfbefbfcd17b6a11445e15752cc77f212e4c3a86d16a19b696f6f7763b98d402020b4a146605d5b5b7bfcf8f1810307dcd4d06d0483c18f3ffe78d3563ad328c26277f0f8f1e34b4b4bcf9f3fb7b6271209f777caeff7070201db928a10f2f0e1437e4b98521a0c06f974d8300ce14f339fcf23e79a502a9512b6777676ba34de8d0dacda29b4616e6e2e994c5ebf7ebda3a3a3b7b7371c0eb7b7b7b34a58dd3ad6b61f826814613158b275f1e245eb66fea6d255160defdcb963bd8f8aa2cccecef24311425e7ef96561b553783c359fcfe77239b69f68331b21f4e79f7f0a4dededed756fbf89aeeb8aa2f036b02230bf1f85319e9d9d5514251008e472b94422c1d2c75028343434b47fff7e0f366c8586c8b14c30c6baae4f4c4c6c6590c1c1419b87c318e7f3f97c3e6f7b1e7ebfffd0a1434e83f08daaaa4e4e4ef29e8f1dc41306299fcfd7d4d4e4cd1f0c0d0d09db6fdebc696bc1183f7efc18590487315614c5e7f33d7ffedc5bbd638b3496b0104294d2cecece6834ea7e3dc8c3471f611ceceaea721ae1e8d1a3c2bdbf42a170fdfa759b409f3c7972e5ca15e1de22db29f2b6c41b1818e06d5014e5fefdfbb6aa6f2e97e3ad62a8aa7af4e8d1ba73fd6f6ee9586131686c6cec9b6fbef1b67bca0a5a8b8b8bd675beb0da7ee4c811a7e4a3a9a969cf9e3d994c467818e6cb2fbfecebeb6b6b6b2b97cba9542a954af9fd7ee13104b6afe78d402010894452a9944d5e18e3c9c9c96834dad3d3a3284a269379f0e081300da594eedbb7cf8d688ac5e2679f7de6c6aa1a67f3ad349cb0d00b6d8d8f8f5fbe7cd9c30207631c8bc5ea568f745dafe1b128a5efbcf3ce850b17f82a3cc6b85aaddebb77cf6c1156c30921478e1cf1566b306d181f1fffeaabaff8ed1d4dd3d2e9743a9d666f6bd426464747dd6ce763d1517a212e53de860b850c8c717b7bfbebafbfee79b5128fc76b5f70ecd8b1ba834c4c4c78ab9b23847a7a7a464646b6beda9a9898a85d3f73829dc50806833b526b6d5061218428a57d7d7d1d1d1d1e9e0d3bec50234bab56abd168b4c6c8ec61b4b5b59d3d7b7653dbe16cf043870e9d3a75cafdc99f1a363437379f3d7b96df51a86bc38913270e1c38f0fffec7143c2c20befdf6dbdea2617b7bbb6d8fc88452dad2d2d2dcdcece6a9efdebdfba38f3e628b89daeb094a69a552090683274f9e646706b7cb558442a10f3ffc90fd3d921b1b745d3f73e60cdb39dda9ada1edc9b10cc3e01fbff0909aaeebb6eca74632c4aa59a74f9ffee1871faca1dde50effe8e8e89d3b77841f9d3c79d2cd080c5555c7c6c6aad5ead4d4542a9532ff7e81a9962dec5555d5757d6868c83c5feaf444fd7ebff04853eda4d0e7f39d3e7dba542addba752b9d4e572a15e6c3ccb9d8b698dfef3f7efc383b5b26b4811574dc7ff7cddaf9df89b6ee2a6bfc2c6c1f79f801397571534adee2054ebd98eb32ef1b7e01fb69fd034e82b70159f4fdcfd85097edf9137b00b0d1b83916f0af06840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548018405480184054801840548e13f4962dbdeba40af160000000049454e44ae426082";
   return logo;
}


QString add_longitud_auxiliar(QStringList cuentas, QString auxiliar, int ampliar)
{
    QString ceros;
    for (int veces=0; veces<ampliar; veces++) ceros.append("0");
    for (int veces=auxiliar.length()-1; veces>=0; veces--)
       {
        if (cuentas.contains(auxiliar.left(veces+1)))
          {
             QString sustituida;
             sustituida=auxiliar;
             sustituida.insert(veces,ceros);
             return sustituida;
          }
       }
    return auxiliar+ceros;
}


bool floats_2d_iguales(double num1, double num2)
{
  if (num1-num2>-0.0001 && num1-num2<0.0001) return true;
  return false;
}


bool nif_ue_correcto(QString nif_iva, QString &nombre, QString &direccion, bool &hayerror)
{
    vies_vatNumber vies_verification;
    hayerror=false;
    vies_verification.validate(nif_iva);
    bool processed=vies_verification.query_processed();

    if (vies_verification.soap_fault() || !processed) {
        hayerror=true;
        return false;
    }
    if (!vies_verification.valid_code()) {
        return false;
    }
    else {
        nombre=vies_verification.response_name();
        direccion=vies_verification.response_address();
    }
    return true;
}


QString cadnum2dec_cadena(double num,int longitud)
{
    double numred=redond(num, 2);
    QString cadcifra;
    cadcifra.setNum(numred,'f',2);
    QString cadentera=cadcifra.section(".",0,0).trimmed();
    QString caddecimal=cadcifra.section(".",1,1).trimmed();
    cadentera=completacadnum(cadentera,longitud-2);
    caddecimal=completacadnum(caddecimal,2);
    return cadentera+caddecimal;
}


QString cadnum2dec_cadena_signo(double num,int longitud)
{
    double numred=redond(num, 2);
    QString cadcifra;
    cadcifra.setNum(numred,'f',2);
    QString cadentera=cadcifra.section(".",0,0).trimmed();
    cadentera=cadentera.remove('-');
    QString caddecimal=cadcifra.section(".",1,1).trimmed();
    cadentera=completacadnum(cadentera,longitud-2);
    if (num<0) cadentera[0]='N';
    caddecimal=completacadnum(caddecimal,2);
    return cadentera+caddecimal;
}


bool f_importa_csv_cta_conci(QString qfichero,QString cuenta)
{
    QFile fichero(adapta(qfichero));
    if (!fichero.exists()) return false;
    int lineas=0;

    if ( fichero.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea;
          while ( !stream.atEnd() )
            {
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              lineas++;
              QString fecha=linea.section('\t',0,0);
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
              if (!fechacorrespondeaejercicio(lafecha) || fechadejerciciocerrado(lafecha))
                 {
                    QMessageBox::warning( 0,QObject::tr("Importar cuenta"),
                         QObject::tr("Error, la fecha %1 no es correcta\n"
                             "la operación no se ha realizado.").arg(fecha));
                    return false;
                 }
              QString debe_haber=convapunto(linea.section('\t',2,2));
              if (debe_haber.toDouble()>-0.001 && debe_haber.toDouble()<0.001)
                 {
                  QMessageBox::warning( 0,QObject::tr("Importar cuenta"),
                       QObject::tr("Error, los importes no son correctos\n"
                           "la operación no se ha realizado.").arg(fecha));
                  return false;

                 }
             }
          fichero.close();
        } else return false;

    QProgressDialog progreso("Procesando archivo...", 0, 0, lineas);
    progreso.setWindowModality(Qt::WindowModal);
    // progreso.setMaximum(0);
    int proceso=1;
    progreso.setValue(proceso);
    progreso.setMinimumDuration ( 0 );
    progreso.show();
    // progreso.setWindowTitle(tr("PROCESANDO..."));
    QApplication::processEvents();

       // comprobamos fecha máxima
    QDate maxfecha = basedatos::instancia()->selectMaxfechacuenta_ext_concicuenta(cuenta);

    if (QMessageBox::question(
              0,QObject::tr("Importar fichero para conciliación"),
             QObject::tr("Se va a importar a partir de la fecha %1\n"
                 "en la cuenta %2.\n"
                 "¿ Desea continuar ?").arg(maxfecha.toString("dd.MM.yyyy"),cuenta)) ==QMessageBox::No )
                            return false;


    if ( fichero.open( QIODevice::ReadOnly ) ) {
          QTextStream stream( &fichero );
          stream.setEncoding(QStringConverter::Utf8);
          QString linea;
          while ( !stream.atEnd() )
            {
              linea = stream.readLine(); // linea de texto excluyendo '\n'
              proceso++;
              progreso.setValue(proceso);
              // Empezamos por la fecha
              QString fecha=linea.section('\t',0,0);
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
              if (lafecha<=maxfecha)
                {
                  continue;
                }


              // ahora el concepto
              QString concepto=linea.section('\t',1,1);

              // debe y haber
              double debe=convapunto(linea.section('\t',2,2)).toDouble()<0.001 ? convapunto(linea.section('\t',2,2)).toDouble()*-1 :0 ;
              double haber=convapunto(linea.section('\t',2,2)).toDouble()>0.001 ? convapunto(linea.section('\t',2,2)).toDouble() :0 ;
              double saldo=convapunto(linea.section('\t',3,3)).toDouble();

              basedatos::instancia()->insert6Cuenta_ext_conci(cuenta, lafecha, debe, haber, concepto, saldo);

              QApplication::processEvents();
            }
          fichero.close();
        } else return false;


    return true;

}

bool f_importarfichc43(QString qfichero,QString cuenta)
{
  QFile fichero(adapta(qfichero));
  if (!fichero.exists()) return false;
  int lineas=0;
  QString cc;
  if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        // stream.setCodec("Windows-1252");
        stream.setEncoding(QStringConverter::System);
        QString linea;
        while ( !stream.atEnd() )
          {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            lineas++;
            if (linea.startsWith("11"))
               {
                QString leecuenta=linea.mid(2,18);
                if (cc.length()==0) cc=leecuenta;
                if (cc!=leecuenta)
                  {
                   QMessageBox::information( 0,QObject::tr("FICHERO SEGÚN CUADERNO 43"),
                            QObject::tr("ERROR: Sólo debe de haber información de una cuenta en el fichero"));
                   return false;
                  }
               }
         }
        fichero.close();
      } else return false;

  QProgressDialog progreso("Procesando cuaderno ...", 0, 0, lineas);
  progreso.setWindowModality(Qt::WindowModal);
  // progreso.setMaximum(0);
  int proceso=1;
  progreso.setValue(proceso);
  progreso.setMinimumDuration ( 0 );
  progreso.show();
  // progreso.setWindowTitle(tr("PROCESANDO..."));
  QApplication::processEvents();

     // comprobamos fecha máxima
  QDate maxfecha = basedatos::instancia()->selectMaxfechacuenta_ext_concicuenta(cuenta);

  if (QMessageBox::question(
            0,QObject::tr("Importar fichero según Cuaderno 43"),
           QObject::tr("Se va a importar la c/c %1\n"
               "a partir de la fecha %2\n"
               "en la cuenta %3.\n"
               "¿ Desea continuar ?").arg(cc,maxfecha.toString("dd.MM.yyyy"),cuenta)) ==QMessageBox::No )
                          return false;


  if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        // stream.setCodec("Windows-1252");
        QString linea;
        QString cc;
        double saldo=0;
        bool haysaldo=false;
        while ( !stream.atEnd() )
          {
            if (linea.isEmpty()) linea = stream.readLine(); // linea de texto excluyendo '\n'
            proceso++;
            progreso.setValue(proceso);
            if (stream.atEnd()) break;
            if (linea.startsWith("11"))
               {
                haysaldo=true;
                saldo=convapunto(linea.mid(33,14)).toDouble()/100;
                bool esdebe=linea.mid(32,1)=="1";
                if (esdebe) saldo=-1*saldo;
                if (!stream.atEnd()) linea = stream.readLine();
               }
            if (linea.startsWith("22") && !stream.atEnd())
               {
                QString cadfecha=linea.mid(10,6);
                int anyo=cadfecha.left(2).toInt();
                if (anyo<50) anyo+=2000; else anyo+=1900;
                int mes=cadfecha.mid(2,2).toInt();
                int dia=cadfecha.right(2).toInt();
                QDate fecha(anyo,mes,dia);
                if (fecha<=maxfecha)
                  {
                    linea = stream.readLine();
                    continue;
                  }
                bool esdebe=linea.mid(27,1)=="1";
                double importe=convapunto(linea.mid(28,14)).toDouble()/100;
                double debe=0;
                double haber=0;
                if (esdebe) debe=importe;
                    else haber=importe;
                if (haysaldo) saldo+=haber-debe;
                QString referencia=linea.right(28).trimmed();
                if (referencia.startsWith("000000")) referencia.clear();
                //QMessageBox::information( this,QObject::tr("conc"),linea);
                do
                  {
                   linea=stream.readLine();
                   proceso++;
                   if (stream.atEnd()) break;
                   if (linea.startsWith("23"))
                      {
                        referencia=referencia+" "+linea.mid(4,38).simplified();
                        referencia=referencia+" "+linea.mid(42,38).simplified();
                      }
                  } while (linea.startsWith("23"));
                //QMessageBox::information( this,QObject::tr("conc"),referencia);
                // insertamos registro en cuestión
                basedatos::instancia()->insert6Cuenta_ext_conci(cuenta, fecha, debe, haber, referencia, saldo);
               }
            if (!stream.atEnd() && !linea.startsWith("11") && !linea.startsWith("22")) linea = stream.readLine();
            QApplication::processEvents();
          }
        fichero.close();
      } else return false;


  return true;

}


vies_vatNumber vies_verification;

QString xml_aeat_ok_ids(QStringList nif, QStringList nombre) {
    // Construimos XML
    QDomDocument doc("ENVIO");
    QDomElement root = doc.createElement("soapenv:Body");
    doc.appendChild(root);
    QDomElement tag = doc.createElement("vnif:VNifV2Ent");
    root.appendChild(tag);
    int elementos=nif.count();
    if (elementos==0) return QString();
    for (int el=0; el<elementos; el++) {
        QDomElement tag1 = doc.createElement("vnif:Contribuyente");
        tag.appendChild(tag1);
        addElementoTextoDom(doc,tag1,"vnif:Nif",nif.at(el));
        addElementoTextoDom(doc,tag1,"vnif:Nombre",nombre.at(el));
    }

    QString cadini="<?xml version='1.0' encoding='UTF-8'?>\n";
    cadini+="<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
    cadini+="xmlns:vnif=\"http://www2.agenciatributaria.gob.es/static_files/common/internet/dep/aplicaciones/es/aeat/burt/jdit/ws/VNifV2Ent.xsd\">\n";
    cadini+="<soapenv:Header/>";
    QString xml = doc.toString();
    xml.remove("<!DOCTYPE ENVIO>");
    xml.prepend(cadini);
    xml.append("</soapenv:Envelope>\n");
    return xml;
}


bool casi_cero(double numero) {
    if (numero<0.0001 && numero >-0.0001) return true;
    return false;
}
