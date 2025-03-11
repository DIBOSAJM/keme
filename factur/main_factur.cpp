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

#include <QtGui>
#include <QProgressDialog>
#include <QEventLoop>
#include <QDomDocument>
#include <QtAlgorithms>

#include "main_factur.h"
#include "funciones.h"
#include "listaseries.h"
#include "listadocs.h"
#include "inicio.h"
#include "inicial.h"
#include "creaempresa.h"
#include "conexion.h"
#include "basedatos.h"
#include "listarefs.h"
#include "tiposivacta.h"
#include "facturas.h"
#include "ret_irpf.h"
#include "login.h"
#include "subcuentas.h"
#include "externos.h"
#include "lista_fact_predef.h"
#include "lotes_fact.h"
#include "pidecuenta.h"
#include "config_verifactu.h"
#include <QSplashScreen>
#include <QMessageBox>

#define COMADECIMAL 1

main_factur::main_factur(){
    ui.setupUi(this);

    QString directorio;
    QString nombredir;
    directorio=dirtrabajodefecto();

    dirtrabajo=directorio;

    dirconfig=trayconfig();

    nombredir="/keme.cfg";
    nombredir[0]=QDir::separator();
    fichconfig=dirconfig+nombredir;

    fichconexiones=fichconex();
    aplicacionimprimir=programa_imprimir_defecto();
    navegador="konqueror";
    if (eswindows())
      {
       navegador=windirprogram();
       navegador+="\\Internet Explorer\\iexplore";
      }
    estilonumerico=COMADECIMAL;
    sindecimales=false;

    connect(ui.actionSeries, SIGNAL(triggered()), this,
              SLOT (seriesfac()));

    connect(ui.actionTipos_documento, SIGNAL(triggered()), this,
              SLOT (tiposdoc()));

    connect(ui.actionReferencias, SIGNAL(triggered()), this,
              SLOT (referencias()));

    connect(ui.actionCuentas_de_IVA, SIGNAL(triggered()), this,
              SLOT (cuentasiva()));

//    connect(ui.actionFacturas, SIGNAL(triggered()), this,
//              SLOT (listafac()));

    connect(ui.actionIrpf, SIGNAL(triggered()), this,
              SLOT (retencion()));

    connect(ui.actionSalir, SIGNAL(triggered()), this,
              SLOT (salir()));

    connect(ui.actionConexion, SIGNAL(triggered()), this,
              SLOT (conexionc()));

    connect(ui.actionCuentas, SIGNAL(triggered()), this,
              SLOT(auxiliares()));

    connect(ui.actionFacturacion_predefinida, SIGNAL(triggered()), this,
              SLOT(fpredf()));

    connect(ui.actionLotes_facturas, SIGNAL(triggered()), this, SLOT(lotes()));

    connect(ui.actionCuenta_anticipos,SIGNAL(triggered(bool)),this, SLOT(cuenta_anticipo()));

    connect(ui.actionExternos,SIGNAL(triggered(bool)),this,SLOT(gest_externos()));

    }

void main_factur::seriesfac()
{
    listaseries *l = new listaseries();
    l->exec();
    delete(l);
    if (centralWidget()!=NULL)
        p_facturas=static_cast<facturas*>(centralWidget());
    if (p_facturas!=NULL) {
        p_facturas->carga_combos_series();
        p_facturas->refresca();
    }
}


bool main_factur::oknosalir()
{
     bool bdabierta=false;
     //
     bool hayfich=false;
     int valor;
     bool hayfichpref=false;
     // QString fichsplash = a.applicationDirPath ();
     // QString fichsplash=fsplash();
     QSplashScreen *splash=NULL;
     //if (QFile::exists(adapta(fichsplash)))
     // {
       QPixmap pixmap( ":/splash/graf/splash.png" );
       splash = new QSplashScreen( pixmap );
       splash->show();
       hayfich=true;
     // }
     if (!cargapreferencias(&bdabierta, &hayfichpref) || !bdabierta )
      {
         if (hayfich)
          {
           splash->finish( this );
           delete splash;
           hayfich=false;
          }
         // procesamos menú abrir bd, crear nueva, o salir
         if (!hayfichpref)
           {
            inicio *i = new inicio;
            i->exec();
            delete(i);
           }
         inicial *pinicial = new inicial;

       while (true)
        {
         valor = pinicial->exec();
         if (pinicial->salir() || (valor==0))
          {
             delete(pinicial);
            return false;
           }
         if (pinicial->abrir())
           {
            if (conexion0()) break;
           }
         if (pinicial->crear())
           {
            creaempresa *c = new creaempresa;
            int resultado = c->exec();
            if (resultado == QDialog::Accepted)
                {
                  if (c->creabd())
                     {
                      c->actufichconex(fichconexiones);
                      delete(c);
                      break;
                     }
                }
            delete(c);
           }
        }
       delete(pinicial);
      }
     if (hayfich)
        {
         splash->finish( this );
         delete splash;
      }
     setWindowTitle(tr("KEME Contabilidad - ")+nombreempresa());
     facturas *f = new facturas(usuario);
     f->escondesalir();
     f->activaconfiltro();
     setCentralWidget(f);
     p_facturas=f;
     return true;
}


bool main_factur::cargapreferencias(bool *bdabierta, bool *hayfichpref)
{
  QString nombrefichcompleto=fichconfig;
  QString vpghost,vpgdbname,vpguser,vpgpassword,vcontrolador,vpuerto;
  QFile fichero(adapta(nombrefichcompleto));
  if (!fichero.exists())
     {
      *bdabierta=false;
      return false;
     }

   if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea,variable,contenido;
        *hayfichpref=true;
        while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            // analizamos la linea de texto
            if (linea.contains('=')) {
               variable=linea.section('=',0,0);
               contenido=linea.section('=',1,1);
               if (variable.compare("PGHOST")==0) vpghost=contenido;
               if (variable.compare("DBNAME")==0) vpgdbname=contenido;
               if (variable.compare("PGUSER")==0) vpguser=contenido;
               if (variable.compare("PASSWORD")==0) vpgpassword=contenido;
               if (variable.compare("CONTROLADOR")==0) vcontrolador=contenido;
               if (variable.compare("PORT")==0) vpuerto=contenido;

               // ---------------------------------------------------------------------------------------
               if (variable.compare("DIRECTORIO")==0) dirtrabajo=contenido;
               if (variable.compare("IMPRIMIR")==0) aplicacionimprimir=contenido;
               if (variable.compare("NAVEGADOR")==0) navegador=contenido;
               if (variable.compare("SINDECIMALES")==0) sindecimales=contenido.toInt();
               if (variable.compare("ESTILONUMERICO")==0) estilonumerico=contenido.toInt();
               if (variable.compare("GUARDACONEXION")==0) guardaconexion=contenido.toInt();

            }
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

bool main_factur::conexion0()
{

  conexion *laconex = new conexion;
  laconex->pasaarchconex(fichconexiones);

  int resultado=laconex->exec();
  if (resultado==QDialog::Rejected) return false;
  QString nbd;
  if (laconex->controlador().contains("SQLITE"))
     {
      nbd=dirtrabajobd();
      nbd+=QDir::separator();
     }
  nbd+=laconex->basedatos().toLower();
  if (laconex->controlador().contains("SQLITE")) nbd=adapta(nbd);
  if (resultado==QDialog::Accepted)
    {
     if (laconex->controlador().contains("SQLITE"))
        {
         if (!QFile::exists(nbd))
            {
             QMessageBox::warning(this, tr("KEME-Contabilidad"),
                   tr("No existe la base de datos especificada"));
             delete(laconex);
             return false;
            }
        }
     if (!abrebasedatos(laconex->host(),
                     nbd,
                     laconex->usuario().toLower(),
                     laconex->clave(),
                     laconex->controlador(),
                     laconex->puerto()))
         {
          QMessageBox::warning(this, tr("KEME-Contabilidad"),
                   tr("No se ha podido abrir la base de datos especificada"));
          delete(laconex);
          return false;
         }
          else
                {
                 if (!cargausuario()) return false;
                 laconex->actulistaconex();
                 laconex->actufichconex();
                }
     }
  delete(laconex);
  return true;
}


bool main_factur::abrebasedatos(QString vpghost,QString vpgdbname,QString vpguser,
                            QString vpgpassword,QString vcontrolador,QString vpuerto)
{
        if (! basedatos::instancia()->abrirBase( vcontrolador, vpgdbname, vpguser,
            vpgpassword, vpghost, vpuerto) )
          return false;
        if (!cargausuario()) return false;
    return true;
}


void main_factur::tiposdoc()
{
    listadocs *l = new listadocs();
    l->exec();
    delete(l);
}


void main_factur::referencias()
{
    listarefs *l = new listarefs();
    l->exec();
    delete(l);
}

void main_factur::cuentasiva()
{
  tiposivacta *t = new tiposivacta();
  t->exec();
  delete(t);
}


void main_factur::listafac()
{
  facturas *f = new facturas(usuario);
  f->activaconfiltro();
  f->exec();
  delete(f);
}

void main_factur::retencion()
{
  ret_irpf *r = new ret_irpf(estilonumerico);
  r->exec();
  delete(r);
}


bool main_factur::cargausuario()
{
     if (basedatos::instancia()->gestiondeusuarios())
        {
         // repetir dos veces petición de clave hasta correcta
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
                 // QString privileg=basedatos::instancia()->privilegios(l->usuario());
                 // for (int veces=0; veces<NUMPRIVILEGIOS; veces++)
                 //    privilegios[veces]= privileg[veces]=='1' ? true : false;
                 // QMessageBox::warning( this, tr("TABLA DE ASIENTOS"), privileg);
                 // actualizar con método menús
                 // actuprivilegios();
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
     } else { usuario.clear(); /*resetprivilegios();*/ }
     return true;
 }

void main_factur::salir()
{
  close();
}


void main_factur::conexionc()
{
    // primero nos quedamos con los parámetros de la anterior conexión
    // este es un slot para cambiar conexión no para partir desde cero

     QString anombredb;
     QString anombreusuario;
     QString aclave;
     QString ahost;
     QString acontrolador;
     QString apuerto;

     QWidget *fac = centralWidget();
     fac->close();
     delete(fac);
     bool haygestionusuarios=false;

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
          nbd=dirtrabajobd();
          nbd+=QDir::separator();
         }
       nbd+=laconex->basedatos().toLower();
      if (laconex->controlador().contains("SQLITE")) nbd=adapta(nbd);

       if (laconex->controlador().contains("SQLITE"))
          {
           if (!QFile::exists(nbd))
              {
               QMessageBox::warning(this, tr("KEME-Contabilidad"),
                     tr("No existe la base de datos especificada"));
               abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
               // falta cargar el diario, etc.
               usuario.clear();
               laconex->actulistaconex();
               laconex->actufichconex();
               delete(laconex);
               if (!cargausuario())
                    {
                      QMessageBox::warning(this, tr("KEME-Contabilidad"),
                         tr("No se ha podido entrar con el usuario especificado"));
                      abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
                    }
                    else
                        haygestionusuarios=basedatos::instancia()->gestiondeusuarios();
               if (!haygestionusuarios) usuario.clear();

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
                      abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
                    }
                     else haygestionusuarios=basedatos::instancia()->gestiondeusuarios();
                   if (!haygestionusuarios) usuario.clear();

                  }
       }
        else abrebasedatos(ahost,anombredb,anombreusuario,aclave,acontrolador,apuerto);
    delete(laconex);

    setWindowTitle(tr("KEME Contabilidad - ")+nombreempresa());
    facturas *f = new facturas(usuario);
    f->escondesalir();
    f->activaconfiltro();
    setCentralWidget(f);

}


void main_factur::auxiliares()
{
    subcuentas *s = new subcuentas();
    s->exec();
    delete(s);

}


void main_factur::fpredf()
{
    lista_fact_predef *l = new lista_fact_predef();
    l->exec();
    delete(l);
}

void main_factur::lotes()
{
    lotes_fact *l = new lotes_fact();
    l->exec();
    delete(l);
    /*
    if (p_facturas!=NULL)
      p_facturas->refresca();*/
    return;
}

void main_factur::cuenta_anticipo()
{
  pidecuenta *p = new pidecuenta();
  p->pasa_cuenta(basedatos::instancia()->cuenta_anticipo());
  p->cambia_titulo(tr("Cuenta de anticipos de clientes"));
  int resultado=p->exec();

  if (resultado==QDialog::Accepted)
    {
     // actualizar
      basedatos::instancia()->actualiza_cta_anticipo(p->cuenta());
    }
  delete(p);
}


void main_factur::gest_externos()
{
    externos *e = new externos();
    e->exec();
    delete(e);
}

void main_factur::on_actionConfiguraci_n_Veri_Factu_triggered()
{
    Config_Verifactu *c = new Config_Verifactu;
    c->exec();
    delete(c);
}

