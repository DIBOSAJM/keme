/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#include "copiaseg.h"
#include <QProcess>
#include <QSqlDatabase>
#include "funciones.h"
#include "basedatos.h"
#include <QProgressDialog>
#include <QTime>
#include <QMessageBox>


copiaseg::copiaseg() : QDialog() {
    ui.setupUi(this);

    QSqlDatabase bd;
    bd=QSqlDatabase::database ();
    nombre=bd.databaseName();
    usuario=bd.userName();
    host=bd.hostName();
    clave=bd.password();
    controlador=bd.driverName();

    if (basedatos::instancia()->cualControlador() == basedatos::SQLITE)
       {
        int pos=nombre.lastIndexOf(QDir::separator());
        nombrefichero=nombre.right(nombre.length()-pos-1);
       }
       else
          nombrefichero=nombre;
    QDate fecha;
    nombrefichero+="-";
    nombrefichero+=fecha.currentDate().toString("yy.MM.dd");
    nombrefichero+="-";
    QTime hora;
    hora=hora.currentTime();
    nombrefichero+=hora.toString("hhmm");
    nombrefichero+=".sql";
    ui.textLabel2->setText(nombrefichero);

   connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

}


void copiaseg::procesar()
{
  
 QProgressDialog progreso("Generando copia de seguridad ...", 0, 0, 0);
 progreso.setWindowTitle(tr("PROCESANDO..."));
 progreso.setWindowModality(Qt::WindowModal);
 progreso.setMaximum(0);
 // progreso.setValue(30);
 progreso.setMinimumDuration ( 0 );
 // progreso.forceShow();
 progreso.show();
 QApplication::processEvents();

  if (basedatos::instancia()->cualControlador() == basedatos::POSTGRES)
    {
      QString ruta= rutacopiaseg().isEmpty() ? dirtrabajo() : rutacopiaseg();
      ruta.append(QDir::separator());
      ruta+=nombrefichero;
      QApplication::processEvents();
     #ifdef SCRIPT_POSTGRES
       QString cadexec=qscript_copias();
       if (cadexec.isEmpty())
           cadexec="/opt/bin/./copia_seguridad.sh";
       // #/opt/bin/./copia_seguridad.sh nombre_db usuario contraseña "/home/sip2000/MyShares/3eTradeKeme/recibidos/copias"
       cadexec+=" ";
       cadexec+=nombre;
       cadexec+=" ";
       cadexec+=usuario;
       cadexec+=" ";
       cadexec+=clave;
       cadexec+=" ";
       cadexec+=ruta.replace(" ","\\ ");
       // progreso.forceShow();
       QApplication::processEvents();
     #else
       if (clave.length()>0)
         {
           QMessageBox::warning( this, tr("Copia de seguridad"),
                  tr("No se puede realizar la copia cuando la base de datos se accede\n"
                     "mediante clave; utilice el programa 'pg_dump' en su lugar."));
          return;
         }
       QString cadexec="pg_dump";
      /* if (host.length()>0)
          {
           cadexec+=" -h ";
           cadexec+=host;
          }
       if (usuario.length()>0)
          {
           cadexec+=" -U ";
           cadexec+=usuario;
          }
       cadexec+=" ";
       cadexec+=nombre;
       cadexec+=" -f ";
       cadexec.append(adapta(ruta));*/
     #endif

     QObject *parent=NULL;

     QProcess *myProcess = new QProcess(parent);

     QStringList arguments;
     QString cadarg;
     if (host.length()>0)
        {
         arguments << "-h";
         arguments << host;
        }
     if (usuario.length()>0)
       {
         arguments << "-U";
         arguments << usuario;
       }


     arguments << nombre;
     arguments << "-f";
     cadarg=dirtrabajo();
     cadarg.append(QDir::separator());
     cadarg+=nombrefichero;
     arguments << cadarg;
     // arguments << "&";

     //progreso.forceShow();
     QApplication::processEvents();

     myProcess-> setWorkingDirectory(dirtrabajo());

     myProcess->start(cadexec,arguments);

    // QMessageBox::warning( this, tr("COPIA DE SEGURIDAD"),cadexec);

     if (!myProcess->waitForStarted ())
       {
         QMessageBox::warning( this, tr("COPIA DE SEGURIDAD"),tr("ERROR: no se ha podido ejecutar"
                                                                  " el programa externo"));
         delete myProcess;
         return;
       }

     // progreso.forceShow();
     QApplication::processEvents();

     if (!myProcess->waitForFinished (120000 ))
       {
         QMessageBox::warning( this, tr("COPIA DE SEGURIDAD"),tr("ERROR: no se ha podido ejecutar"
                                                                  " el programa externo"));
         delete myProcess;
         return;
       }
     delete myProcess;
    }
  else if (basedatos::instancia()->cualControlador() == basedatos::MYSQL)
          {
             // mysql
           QObject *parent=NULL;
           QString cadexec="mysqldump ";
           QProcess *myProcess = new QProcess(parent);
           QStringList arguments;
           QString cadarg;
           cadexec="mysqldump";
           if (host.length()>0)
              {
                cadarg="--host=";
                cadarg+=host;
                arguments << cadarg;
               }
           if (usuario.length()>0)
             {
              cadarg="--user=";
              cadarg+=usuario;
              arguments << cadarg;
             }
          if (clave.length()>0)
            {
               cadarg="--password=";
               cadarg+=clave;
               arguments << cadarg;
            }
          cadarg=nombre;
          arguments << cadarg;
          cadarg="-r";
          cadarg+=dirtrabajo();
          cadarg.append(QDir::separator());
          cadarg+=nombrefichero;
          arguments << cadarg;
          // cadarg=" &";
          // arguments << cadarg;
          myProcess-> setWorkingDirectory(dirtrabajo());
          QApplication::processEvents();
          myProcess->start(cadexec,arguments);
          if (!myProcess->waitForStarted ())
             {
               QMessageBox::warning( this, tr("COPIA DE SEGURIDAD"),tr("ERROR: no se ha podido ejecutar"
                                                                  " el programa externo mysqldump"));
               delete myProcess;
               return;
             }
          // delete myProcess;
          if (!myProcess->waitForFinished (120000))
           {
            QMessageBox::warning( this, tr("COPIA DE SEGURIDAD"),tr("ERROR: no se ha podido ejecutar"
                                                                  " el programa externo pg_dump"));
            delete myProcess;
            return;
           }
          delete myProcess;
         }


    if (basedatos::instancia()->cualControlador() == basedatos::SQLITE)
          {
             // sqlite

           // para que funcione correctamente sqlite3 tenemos que cerrar la conexión y volverla
           // a abrir una vez hecha la copia

           QString anombredb;
           QString anombreusuario;
           QString aclave;
           QString ahost;
           QString acontrolador;
           QString apuerto;

           anombredb = basedatos::instancia()->nombre();
           anombreusuario = basedatos::instancia()->usuario();
           aclave = basedatos::instancia()->clave();
           ahost = basedatos::instancia()->host();
           acontrolador = basedatos::instancia()->controlador();
           apuerto.setNum( basedatos::instancia()->puerto() );
           basedatos::instancia()->cerrar(); //linea nueva
           QSqlDatabase::removeDatabase("qt_sql_default_connection");

           // ------------------------------------------------------------------------------
           QObject *parent=NULL;
           QString cadexec="sqlite3";
           QProcess *myProcess = new QProcess(parent);
           cadexec+=" ";
           cadexec+=nombre;
           cadexec+=" .dump";
           QString destino=dirtrabajo();
           destino.append(QDir::separator());
           destino+=nombrefichero;


           myProcess-> setWorkingDirectory(dirtrabajo());

          myProcess->setStandardOutputFile(destino);
          myProcess->start(cadexec);

          if (!myProcess->waitForStarted ())
             {
               QMessageBox::warning( this, tr("COPIA DE SEGURIDAD"),tr("ERROR: no se ha podido ejecutar"
                                                                  " el programa externo sqlite3"));
               delete myProcess;
               basedatos::instancia()->abrirBase( acontrolador, anombredb, anombreusuario,
                           aclave, ahost, apuerto);
               return;
             }
          // delete myProcess;
          QApplication::processEvents();
          if (!myProcess->waitForFinished (60000))
           {
            QMessageBox::warning( this, tr("COPIA DE SEGURIDAD"),tr("ERROR: no se ha podido ejecutar"
                                                                  " el programa externo sqlite3"));
            delete myProcess;
            basedatos::instancia()->abrirBase( acontrolador, anombredb, anombreusuario,
                        aclave, ahost, apuerto);
            return;
           }
          delete myProcess;
          basedatos::instancia()->abrirBase( acontrolador, anombredb, anombreusuario,
                      aclave, ahost, apuerto);
         }


     QMessageBox::information( this, tr("Copia de seguridad"),
		tr("La copia de seguridad se ha realizado"));
     accept();
 }
