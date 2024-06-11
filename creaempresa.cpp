/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#include "creaempresa.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>
#include "directorio.h"
#include <QProgressDialog>
#include <QFileDialog>

creaempresa::creaempresa() : QDialog() {
    ui.setupUi(this);

   ui.plancomboBox->insertItem(0,planvacio());
   QDir directorio(trayconfig());

   QStringList filters;
   filters << "*.pln";
   directorio.setNameFilters(filters);

   directorio.setSorting(QDir::Name);
   ui.plancomboBox->insertItems(0,directorio.entryList());


  QStringList list = QSqlDatabase::drivers();
   int veces=0;
   while (veces<list.size())
      {
       // AÑADIMOS SQLITE para realizar pruebas
       if ( !list.at(veces).contains("PSQL") && !list.at(veces).contains("MYSQL"))
        // && !list.at(veces).contains("SQLITE"))
             list.removeAt(veces);
          else veces++;
      }
    list.insert(0,"QSQLITE");

#ifdef SOLOPOSTGRES
    veces=0;
    while (veces<list.size())
       {
        // AÑADIMOS SQLITE para realizar pruebas
        if ( !list.at(veces).contains("PSQL") || list.at(veces).contains("PSQL7"))
              list.removeAt(veces);
           else veces++;
       }
#endif

    ui.controladorcomboBox->addItems(list);

   connect(ui.longvariablecheckBox,SIGNAL(stateChanged(int)),this,SLOT(longvariablecambiado()));
   errores_import=false; nocontinuar=false;
}

void creaempresa::longvariablecambiado()
{
 if (ui.longvariablecheckBox->isChecked())
    {
     ui.digitoslabel->setEnabled(false);
     ui.digitosspinBox->setMinimum(0);
     ui.digitosspinBox->setValue(0);
     ui.digitosspinBox->setEnabled(false);
    }
    else
        {
         ui.digitoslabel->setEnabled(true);
         ui.digitosspinBox->setMinimum(4);
         ui.digitosspinBox->setEnabled(true);
         ui.digitosspinBox->setValue(9);
        }
}

QString creaempresa::planvacio()
{
 return (tr("Plan contable vacío"));
}

bool creaempresa::creabd()
{
    if (ui.bdlineEdit->text().length()==0)
    {
        QMessageBox::warning( this, tr("Nueva empresa"),tr("ERROR EN NOMBRE DE BASE DE DATOS"));
        return false;
    }

    if (ui.bdlineEdit->text().contains(' '))
    {
        QMessageBox::warning( this, tr("Nueva empresa"),
                              tr("ERROR: No se permiten espacios en nombre de base de datos"));
        return false;
    }

    if (ui.importacioncheckBox->isChecked())
    {

        if (QMessageBox::question(this,
                        tr("Nueva empresa"),
                        tr("Los datos para la importación se seleccionarán de un archivo.\n"
                           "La importación solo será válida para códigos de cuenta auxiliar\n"
                           " con longitud fija."
                           "¿ Desea continuar ?")) == QMessageBox::No)
               return false;
    }


    bool creado = basedatos::instancia()->crearBase( ui.controladorcomboBox->currentText() ,
                                                     ui.bdlineEdit->text().toLower() ,
                                                     ui.usuariolineEdit->text().toLower() ,
                                                     ui.clavelineEdit->text() ,
                                                     ui.hostlineEdit->text() ,
                                                     ui.puertolineEdit->text().trimmed() );

    if ( creado ) {
        creatablasiniciales(); // incluye crear claves foráneas
        if (ui.importacioncheckBox->isChecked()) importacion();
            else
              importaplan();
        return true;
    }
    else {
        return false;
    }

}


void creaempresa::preparacopia()
{
    ui.plancomboBox->hide();
    ui.label->hide();
    ui.digitoslabel->hide();
    ui.digitosspinBox->hide();
    ui.longvariablecheckBox->hide();
    ui.importacioncheckBox->hide();
}

bool creaempresa::creabd_segunda()
{
    if (ui.bdlineEdit->text().length()==0)
    {
        QMessageBox::warning( this, tr("Nueva empresa"),tr("ERROR EN NOMBRE DE BASE DE DATOS"));
        return false;
    }

    if (ui.bdlineEdit->text().contains(' '))
    {
        QMessageBox::warning( this, tr("Copia empresa"),
                              tr("ERROR: No se permiten espacios en nombre de base de datos"));
        return false;
    }

    bool creado = basedatos::instancia()->crearBase( ui.controladorcomboBox->currentText() ,
                                                     ui.bdlineEdit->text().toLower() ,
                                                     ui.usuariolineEdit->text().toLower() ,
                                                     ui.clavelineEdit->text() ,
                                                     ui.hostlineEdit->text() ,
                                                     ui.puertolineEdit->text().trimmed(),
                                                     "SEGUNDA");

    if ( creado ) {
        // creatablasiniciales();
        basedatos::instancia()->solotablas(true, "SEGUNDA");
        if ( basedatos::instancia()->cualControlador("SEGUNDA") != 3) //SQLITE
           {
             basedatos::instancia()->crearForaneas(true, "SEGUNDA");
           }
        basedatos::instancia()->copiatablas("SEGUNDA");
        //    importaplan();
        QMessageBox::information( this, tr("Copia empresa"),
                              tr("EL PROCESO HA CONCLUIDO"));
        return true;
    }
    else {
        return false;
    }
 return false;
}




void creaempresa::creatablasiniciales()
{
    basedatos::instancia()->crearTablas( ui.longvariablecheckBox->isChecked() ,
                                         ui.digitosspinBox->text() );
}



void creaempresa::importaplan()
{
    cargaplan();
    basedatos::instancia()->insertImportaplan();
}

void creaempresa::cargaplan()
{

  QString cadfich=trayconfig();
  cadfich.append(QDir::separator());

  cadfich+=ui.plancomboBox->currentText();
   QString pasa;
   if (eswindows()) pasa=QFile::encodeName(cadfich);
       else pasa=cadfich;
  QFile fichero(pasa);
  if (!fichero.exists()) return;
   if ( fichero.open( QIODevice::ReadOnly ) ) 
     {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        QString linea;
        bool longvariable=cod_longitud_variable();
        QProgressDialog progreso(tr("Importando plan ..."), 0, 0, 3);
        progreso.setWindowModality(Qt::WindowModal);
        progreso.setValue(1);
        QApplication::processEvents();
         while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            QString codigo,descripcion,cad1;
            codigo=linea.section('\t',0,0);
            descripcion=linea.section('\t',1,1);
            if (!longvariable || linea.section('\t',2,2).length()==0)
               {
                basedatos::instancia()->insertPlancontable(codigo,descripcion,false);
               }
              else
               {
                QString cadauxiliar=linea.section('\t',2,2);
                bool auxiliar=false;
                if ( (cadauxiliar == "TRUE") || (cadauxiliar == "true") || (cadauxiliar == "1") )
                   auxiliar=true;
                basedatos::instancia()->insertPlancontable(codigo,descripcion,auxiliar);
               }
            QApplication::processEvents();
           }
         progreso.setValue(2);
         QApplication::processEvents();
         fichero.close();
         progreso.setValue(3);
         QApplication::processEvents();
      }

}

void creaempresa::actufichconex(QString archconex)
{
 QStringList listaconex;

   QString pasa;
   if (eswindows()) pasa=QFile::encodeName(archconex);
       else pasa=archconex;
 QFile fichero(pasa);
  if (!fichero.exists()) return;
    else
     {
      if ( fichero.open( QIODevice::ReadOnly ) ) 
          {
           QTextStream stream( &fichero );
           stream.setEncoding(QStringConverter::Utf8);
           QString linea;
           while ( !stream.atEnd() ) {
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            listaconex << linea;
           }
          } else return;
     }


 for (int veces=0;veces<listaconex.size();veces++)
     {
      if (listaconex[veces].section('\t',0,0)==ui.bdlineEdit->text())
          {
           listaconex.removeAt(veces);
           break;
          }
     }
 QString linea;
 linea=ui.bdlineEdit->text();
 linea+="\t";
 linea+=ui.controladorcomboBox->currentText();
 linea+="\t";
 linea+=ui.usuariolineEdit->text();
 linea+="\t";
 linea+=ui.clavelineEdit->text();
 linea+="\t";
 linea+=ui.hostlineEdit->text();
 if (ui.puertolineEdit->text().length()>0)
    {
     linea+="\t";
     linea+=ui.puertolineEdit->text();
    }


 listaconex.insert(0,linea);


  fichero.close();

  // QFile fichero(archconex);
  if (! fichero.open( QIODevice::WriteOnly ) ) return;
  QTextStream stream( &fichero );
  stream.setEncoding(QStringConverter::Utf8);
  for (int veces=0;veces<listaconex.size();veces++)
     {
      stream << listaconex[veces] << "\n";
     }
  fichero.close();

}

void creaempresa::importacion()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
  QString cadfich;
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      cadfich=dir->ruta_actual();
    }
  delete(dir);
  if (cadfich.length()>0)
      {
       // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
       int resultado=importar_conta(cadfich);
       if (resultado==1)
           {
            QString mensaje=tr("El archivo seleccionado se ha procesado");
            if (errores_import) mensaje= tr("El archivo seleccionado se ha procesado con errores");
            if (basedatos::instancia()->hayerror_consulta())
                mensaje=tr("El proceso ha concluido con errores");
            QMessageBox::information( this, tr("IMPORTAR DATOS CONTABLES"),
                         mensaje);
            basedatos::instancia()->reset_error_consulta();
            return;
           }
         else
           {
            if (resultado==0)
            QMessageBox::warning( this, tr("IMPORTAR DATOS CONTABLES"),
                         tr("ERROR al recuperar el fichero"));
            return;
           }
      }

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
    if (dialogofich.exec())
       {
        fileNames = dialogofich.selectedFiles();
        if (fileNames.at(0).length()>0)
            {
             // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
             QString cadfich=fileNames.at(0);
             int resultado=importar_conta(cadfich);
             if (resultado==1)
                 {
                  QString mensaje=tr("El archivo seleccionado se ha procesado");
                  if (errores_import) mensaje= tr("El archivo seleccionado se ha procesado con errores");
                  if (basedatos::instancia()->hayerror_consulta())
                      mensaje=tr("El proceso ha concluido con errores");
                  QMessageBox::information( this, tr("IMPORTAR DATOS CONTABLES"),
                               mensaje);
                  basedatos::instancia()->reset_error_consulta();
                  return;
                 }
               else
                 {
                  if (resultado==0)
                  QMessageBox::warning( this, tr("IMPORTAR DATOS CONTABLES"),
                               tr("ERROR al recuperar el fichero"));
                  return;
                 }
            }
       }
#endif
}

void creaempresa::pasa_usuario(QString qusuario)
{
      usuario=qusuario;
}

bool creaempresa::noejercicio(QTextStream *stream)
{
    stream->setEncoding(QStringConverter::Utf8);
    QString linea;
    bool inicio_diario=false;
    while ( !stream->atEnd() ) {
         linea=stream->readLine();
         if (linea.isEmpty()) continue;
         if (linea[0]=='#') continue;
         if (linea.contains("{diario}"))
            {
             inicio_diario=true;
             break;
            }
         }
   if (!inicio_diario) return false;
   while ( !stream->atEnd() ) {
        linea=stream->readLine();
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        if (es_lin_etiqueta(linea)) break;

        QDate fecha=QDate::fromString(linea.mid(70,10),"dd-MM-yyyy");
        QString ejercicio=ejerciciodelafecha(fecha);
        if (ejercicio.isEmpty())
           {
            QMessageBox::information( this,
                                      tr("Importación"),
                                      tr("ERROR: No existe ejercicio para la fecha ") +
                                      fecha.toString("dd-MM-yyyy"));
            return true;
           }

    }
   return false;

}


bool creaempresa::facturasrep(QTextStream *stream)
{
    stream->setEncoding(QStringConverter::Utf8);
    QString linea;
    bool inicio_diario=false;
    while ( !stream->atEnd() ) {
         linea=stream->readLine();
         if (linea.isEmpty()) continue;
         if (linea[0]=='#') continue;
         if (linea.contains("{diario}"))
            {
             inicio_diario=true;
             break;
            }
         }
   if (!inicio_diario) return false;

   QStringList cuentas, cod_facturas, ejercicios;
   QSqlQuery q = basedatos::instancia()->codigos_factura_ej_y_cta_diario();
   if (q.isActive())
       while (q.next()) {
          cod_facturas << q.value(0).toString();
          cuentas << q.value(1).toString();
          ejercicios << q.value(2).toString();
       }

   while ( !stream->atEnd() ) {
        linea=stream->readLine();
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        if (es_lin_etiqueta(linea)) break;
        QString cod_factura=linea.right(80).trimmed();

        QDate fecha=QDate::fromString(linea.mid(70,10),"dd-MM-yyyy");
        QString ejercicio=ejerciciodelafecha(fecha);
        QString cuenta=linea.mid(80,30).trimmed();

        if (cod_factura.isEmpty()) continue;

        bool encontrada=false;
        for (int veces=0; veces<cuentas.count(); veces++)  {
            if (cuentas.at(veces)==cuenta && cod_facturas.at(veces)==cod_factura
                    && ejercicios.at(veces)==ejercicio) {
                encontrada=true;
                break;
            }
        }

        if (encontrada)
              {
               QMessageBox::information( this, tr("Importación"),
                                         tr("Factura: ") + cod_factura+ " - "+
                                         tr(" repetida en ejercicio ")+ ejercicio + " - "
                                         + tr("Cuenta: ") +cuenta);
               return true;
              }

    }
   return false;
}



bool creaempresa::diario_asientos_descuadre(QTextStream *stream)
{
    stream->setEncoding(QStringConverter::Utf8);
    QString linea;
    bool inicio_diario=false;
    while ( !stream->atEnd() ) {
         linea=stream->readLine();
         if (linea.isEmpty()) continue;
         if (linea[0]=='#') continue;
         if (linea.contains("{diario}"))
            {
             inicio_diario=true;
             break;
            }
         }
   if (!inicio_diario) return false;
   double sumadebe=0;
   double sumahaber=0;
   int guarda_asiento=0;
   QList<int> apunte_list;
   QList<double> importe_list;
   while ( !stream->atEnd() ) {
        linea=stream->readLine();
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        if (es_lin_etiqueta(linea)) break;


        QString cadnumasiento=linea.left(15).trimmed();
        if (cadnumasiento.toInt()!=guarda_asiento)
          {
            if (sumadebe-sumahaber<-0.001 || sumadebe-sumahaber>0.001)
              {
               // son distintos, devolver descuadre
                // QString caddebe,cadhaber;
                // caddebe.setNum(sumadebe,'f',2);
                // cadhaber.setNum(sumahaber,'f',2);
                // QMessageBox::warning( this, tr("import"),caddebe+" .. "+cadhaber);
                QString cadasiento; cadasiento.setNum(guarda_asiento);
                QMessageBox::warning(this,tr("Importación"),tr("Descuadre en asiento: ")+cadasiento);
               return true;
              }
            // guardamos número de asiento y reseteamos sumadebe y sumahaber
            guarda_asiento=cadnumasiento.toInt();
            sumadebe=0;
            sumahaber=0;
          }
        QString caddebe=linea.mid(210,17);
        if (caddebe.contains('-')) caddebe=caddebe.mid(caddebe.indexOf('-'));
        sumadebe+=convapunto(caddebe).toDouble();
        QString cadhaber=linea.mid(227,17);
        if (cadhaber.contains('-')) cadhaber=cadhaber.mid(cadhaber.indexOf('-'));
        sumahaber+=convapunto(cadhaber).toDouble();

        apunte_list << linea.mid(15,15).toInt();
        double valdebeabs=convapunto(caddebe).toDouble();
        if (valdebeabs<-0.001) valdebeabs=-1*valdebeabs;
        double valhaberabs=convapunto(cadhaber).toDouble();
        if (valhaberabs<-0.001) valhaberabs=-1*valhaberabs;
        if (valdebeabs>0.0001) importe_list << valdebeabs;
           else importe_list << valhaberabs;
       }

   // ahora queda buscar {diario_ci}
   stream->seek(0);

   bool inicio_diario_ci=false;
   while ( !stream->atEnd() ) {
        linea=stream->readLine();
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        if (linea.contains("{diario_ci}"))
           {
            inicio_diario_ci=true;
            break;
           }
        }
  if (!inicio_diario_ci) return false;

  double valor=0;
  int apunte_guard=0;
  double valor_apunte=0;
  int apunte=0;
  while ( !stream->atEnd() ) {
       linea=stream->readLine();
       if (linea.isEmpty()) continue;
       if (linea[0]=='#') continue;
       if (es_lin_etiqueta(linea)) break;
       apunte=linea.left(15).toInt();
       QString importe=linea.mid(65,17).trimmed();
       if (apunte_guard==0) apunte_guard=apunte;
       if (apunte!=apunte_guard)
         {           
           bool encontrado=false;
           valor_apunte=0;
           for (int veces=0; veces<apunte_list.count(); veces++)
             {
               if (apunte_list.at(veces)==apunte_guard)
                  {
                   encontrado=true;
                   valor_apunte=importe_list.at(veces);
                   break;
                  }
             }

           if (!encontrado)
              {
               // error no existe el apunte de la tabla de imputaciones en el diario
               // return true
               QString cad_apunte; cad_apunte.setNum(apunte_guard);
               QMessageBox::warning(this,tr("Importación"),
                                    tr("No existe apunte en diario: ")+cad_apunte);
              return true;

              }

           if (valor-valor_apunte>-0.05 && valor-valor_apunte<0.05)
             { apunte_guard=apunte; valor=0; }
            else
                {
                 QString cad_apunte; cad_apunte.setNum(apunte_guard);
                 QMessageBox::warning(this,tr("Importación"),
                                     tr("Valor de imputaciones no correcto en apunte: ")+cad_apunte);
                 return true;

                }
         }
       valor+=convapunto(importe).toDouble();
   }

   // comprobamos último apunte

   if (apunte_guard!=0)
     {
       apunte_guard=apunte;
       //bool encontrado=false;
       valor_apunte=0;
       for (int veces=0; veces<apunte_list.count(); veces++)
         {
           if (apunte_list.at(veces)==apunte_guard)
              {
               //encontrado=true;
               valor_apunte=importe_list.at(veces);
               break;
              }
         }
       if (!(valor-valor_apunte>-0.05 && valor-valor_apunte<0.05))
            {
             QString cad_apunte; cad_apunte.setNum(apunte_guard);
             QMessageBox::warning(this,tr("Importación"),
                                 tr("Valor de imputaciones no correcto en apunte: ")+cad_apunte);
             return true;

            }

     }

   return false;
}


int creaempresa::importar_conta(QString cadfich)
{
    apuntes_origen.clear();
    apuntes_conta.clear();
  QString pasa;
    if (eswindows()) pasa=QFile::encodeName(cadfich);
        else pasa=cadfich;

   QProgressDialog progreso(tr("Importando contabilidad ..."), 0, 0, 10);
   // progreso.setWindowModality(Qt::WindowModal);
   int paso=1;
   progreso.setWindowTitle(tr("Proceso..."));
   progreso.setLabelText(tr("Comprobando info..."));
   progreso.setValue(paso);

   QApplication::processEvents();
   basedatos::instancia()->reset_error_consulta();
   progreso.show();
   {
    QFile fichero1(pasa);
    if (!fichero1.exists()) return 0;
    if ( !fichero1.open( QIODevice::ReadOnly ) ) return 0;
    QTextStream stream1( &fichero1 );
    if (noejercicio(&stream1))
      {
        QMessageBox::information( this, tr("Importar"),
                                tr("PROCESO NO LLEVADO A CABO"));
        return -1;

      }
    QApplication::processEvents();
    stream1.seek(0);
    if (diario_asientos_descuadre(&stream1))
      {
        QMessageBox::information( this, tr("Importar"),
                                  tr("ERROR: Cancelado, hay asientos descuadrados en el diario"));
        return -1;
      }
    stream1.seek(0);
    if (facturasrep(&stream1))
      {
       if (QMessageBox::question(this,
           tr("Importar"),
           tr("En el diario hay códigos de facturas coincidentes\n"
              "con la información a importar ¿ se aborta el proceso ?")) == QMessageBox::Yes )
        {
         QMessageBox::information( this, tr("Importar"),
                                 tr("PROCESO NO LLEVADO A CABO"));
         return -1;
        }
      }
     fichero1.close();
    }
   QApplication::processEvents();
   QFile fichero(pasa);
   if (!fichero.exists()) return 0;
   if ( !fichero.open( QIODevice::ReadOnly ) ) return 0;
   QTextStream stream( &fichero );
   stream.setEncoding(QStringConverter::Utf8);
   QString linea;
   // suponemos códigos de longitud fija
   basedatos::instancia()->reset_error_consulta();


   // aquí comenzamos transacción - bloqueamos tablas de configuración y ejercicios

   basedatos::instancia()->transac_fich_import();

   while ( !stream.atEnd() ) {
        if (!es_lin_etiqueta(linea)) linea = stream.readLine(); // linea de texto excluyendo '\n'
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        if (nocontinuar) break;
        if (linea.contains("{plancontable}"))
           {
            progreso.setLabelText(tr("Actualizando plan ..."));
            linea=import_plancontable(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
           }
        if (nocontinuar) break;
        if (linea.contains("{ejercicios}"))
           {
            linea=import_ejercicios(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
           }
        if (nocontinuar) break;
        if (linea.contains("{diario}"))
           {
            progreso.setLabelText(tr("Actualizando diario ..."));
            if (import_remota) linea=import_diario_pg(&stream);
               else
                  linea=import_diario(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
            // QMessageBox::warning( this, tr("EXPORT"),"TRAS LIBRODIARIO");
           }
        if (nocontinuar) break;
        if (linea.contains("{datos_auxiliares}"))
           {
            progreso.setLabelText(tr("Actualizando datos auxiliares ..."));
            linea=import_datos_auxiliares(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
            // QMessageBox::warning( this, tr("EXPORT"),"TRAS DATOS AUXILIARES");
           }

        if (nocontinuar) break;
        if (linea.contains("{plan_amortizaciones}"))
           {
            linea=import_plan_amortizaciones(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
           }
        if (nocontinuar) break;
        if (linea.contains("{libro_facturas}"))
           {
            progreso.setLabelText(tr("Actualizando libro facturas ..."));
            if (import_remota) linea=import_libro_facturas_pg(&stream);
              else
                linea=import_libro_facturas(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
            // QMessageBox::warning( this, tr("EXPORT"),"TRAS LIBROFACTURAS");
           }

        if (nocontinuar) break;
        if (linea.contains("{vencimientos}"))
          {
            linea=import_vencimientos(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
          }
        if (nocontinuar) break;

        if (linea.contains("{ci}"))
          {
            linea=import_ci(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
          }
        if (nocontinuar) break;

        if (linea.contains("{diario_ci}"))
          {
            linea=import_diario_ci(&stream);
            paso++;
            progreso.setValue(paso);
            QApplication::processEvents();
          }
        if (nocontinuar) break;

        if (stream.atEnd()) break;
    }

   basedatos::instancia()->fintransac_fich_import();
   if (nocontinuar)
   {
     return 0;
   }
   // actualizamos apunte y número de asiento para cada ejercicio
   //QMessageBox::warning( this, tr("EXPORT"),"TRAS FIN TRANSACCIÓN");
   progreso.setLabelText(tr("Actualizando saldos ..."));
   basedatos::instancia()->actualiza_prox_asiento_ejercicios();
   // actualizamos próximo pase
   basedatos::instancia()->actualiza_prox_pase();
   // QMessageBox::warning( this, tr("EXPORT"),"ANTES ACTUALIZA SALDOS");
   // actualizamos saldos
   //QMessageBox::warning( this, tr("EXPORT"),"ANTES ACTUALIZA SALDOS");
   progreso.setValue(9);
   // actualizasaldos(); Los saldos se actualizan en línea
 return 1;
}

bool creaempresa::es_lin_etiqueta(QString linea)
{
    if (linea.contains("{plancontable}") || linea.contains("{ejercicios}") ||
        linea.contains("{diario}") || linea.contains("{datos_auxiliares}") ||
        linea.contains("{plan_amortizaciones}") || linea.contains("{libro_facturas}") ||
        linea.contains("{vencimientos}") || linea.contains("{ci}")
        || linea.contains("{diario_ci}")) return true;
  return false;
}

QString creaempresa::import_plancontable(QTextStream *stream)
{
  QStringList cuentas=basedatos::instancia()->lista_codigos_plan();
  QString linea;
  while(!stream->atEnd())
    {
      linea=stream->readLine();
      if (es_lin_etiqueta(linea)) return linea;
      if (linea.isEmpty()) continue;
      if (linea[0]=='#') continue;
      // procesamos los registros
      // codigo 30
      // descripcion 80
      QString codigo=linea.left(30).trimmed();
      QString descripcion=linea.right(80).trimmed();
      if (!cuentas.contains(codigo))
        {
         basedatos::instancia()->insertPlancontable(codigo,descripcion,false);
         chequea_error();
         if (nocontinuar) return QString();
        }
      else {
          basedatos::instancia()->updatePlancontabledescripcioncodigo(codigo,descripcion);
          chequea_error();
          if (nocontinuar) return QString();
      }
     }
  return QString();
}


QString creaempresa::import_ejercicios(QTextStream *stream)
{
    QString linea;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // codigo  30
        // fecha inicial 10
        // fecha final 10  dd-MM-yyyy
        QString codigo=linea.left(30).trimmed();
        QDate inicial; inicial=QDate::fromString(linea.mid(30,10),"dd-MM-yyyy");
        QDate final; final=QDate::fromString(linea.right(10),"dd-MM-yyyy");
        if (!basedatos::instancia()->existe_codigo_ejercicio(codigo))
          {
           basedatos::instancia()->insert6Ejercicios(codigo,inicial,final,false,false);
           chequea_error();
           if (nocontinuar) return QString();
          }
       }
    return QString();
}

QString creaempresa::import_diario(QTextStream *stream)
{
    QString linea;
    qlonglong paseinicial=basedatos::instancia()->selectProx_paseconfiguracion();
    if (paseinicial==0) paseinicial=1;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // Número 15 Entero
        // Apunte 15 Entero
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        // Diario 40 Caracteres
        // Fecha  10 Fecha
        // Código cuenta 30 Caracteres
        // Concepto cargo/abono 100 Caracteres
        // Debe   17 Número 2
        // Haber  17 Número 2
        // Documento 80 Caracteres
        // void basedatos::insertDiario (QString cadnumasiento, qlonglong pase, QString cadfecha,
        //          QString cuentagasto, double debe, double haber,
        //          QString concepto, QString documento, QString diario,
        //          QString ci, QString usuario, QString clave_ci,
        //          QString ejercicio) {
        // pase qlonglong - debe y haber double
        QString cadnumasiento=linea.left(15).trimmed();        
        QString cad_apunte=linea.mid(15,15);        
        qlonglong apunte=cad_apunte.toLongLong() + paseinicial-1;
        apuntes_origen << cad_apunte.toInt();
        apuntes_conta << int(apunte);
        QString diario=linea.mid(30,40).trimmed();
        QDate fecha=QDate::fromString(linea.mid(70,10),"dd-MM-yyyy");
        QString ejercicio=ejerciciodelafecha(fecha);

        qlonglong primer_asiento=basedatos::instancia()->proximoasiento(ejercicio);
        if (primer_asiento==0) primer_asiento=1;
        qlonglong asiento_absoluto=primer_asiento+cadnumasiento.toLongLong()-1;
        cadnumasiento.setNum(asiento_absoluto);

        QString cadfecha=fecha.toString("yyyy-MM-dd");
        QString cuenta=linea.mid(80,30).trimmed();
        QString concepto=linea.mid(110,100).trimmed();
        QString caddebe=linea.mid(210,17);
        if (caddebe.contains('-')) caddebe=caddebe.mid(caddebe.indexOf('-'));
        double debe=convapunto(caddebe).toDouble();
        QString cadhaber=linea.mid(227,17);
        if (cadhaber.contains('-')) cadhaber=cadhaber.mid(cadhaber.indexOf('-'));
        double haber=convapunto(cadhaber).toDouble();
        QString documento=linea.mid(244,80).trimmed();  //linea.right(80).trimmed();
        QString codfactura=linea.right(80).trimmed();
        basedatos::instancia()->insertDiario_imp(cadnumasiento, apunte, cadfecha, cuenta, debe,
                                             haber, concepto, documento, diario, "", usuario,
                                             "", ejercicio, codfactura);
        basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(cuenta,convapunto(cadhaber),convapunto(caddebe));
        chequea_error();
        if (nocontinuar) return QString();
       }
  return QString();
}


QString creaempresa::import_diario_pg(QTextStream *stream)
{
    QString linea;
    qlonglong paseinicial=basedatos::instancia()->selectProx_paseconfiguracion();
    if (paseinicial==0) paseinicial=1;
    QString cadq="INSERT into diario (asiento,pase,fecha, cuenta,debe, haber,concepto,"
        "documento,diario,usuario,ci, clave_ci, ejercicio, codfactura) VALUES(";
    bool primero=true;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea) && primero) return linea;
        if (es_lin_etiqueta(linea)) break;
        if (primero) primero=false;
        else {
            cadq+=", (";
        }
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // Número 15 Entero
        // Apunte 15 Entero
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        // Diario 40 Caracteres
        // Fecha  10 Fecha
        // Código cuenta 30 Caracteres
        // Concepto cargo/abono 100 Caracteres
        // Debe   17 Número 2
        // Haber  17 Número 2
        // Documento 80 Caracteres
        // void basedatos::insertDiario (QString cadnumasiento, qlonglong pase, QString cadfecha,
        //          QString cuentagasto, double debe, double haber,
        //          QString concepto, QString documento, QString diario,
        //          QString ci, QString usuario, QString clave_ci,
        //          QString ejercicio) {
        // pase qlonglong - debe y haber double
        QString cadnumasiento=linea.left(15).trimmed();
        QString cad_apunte=linea.mid(15,15);
        qlonglong apunte=cad_apunte.toLongLong() + paseinicial-1;
        QString cadnumpase; cadnumpase.setNum(apunte);
        apuntes_origen << cad_apunte.toInt();
        apuntes_conta << int(apunte);
        QString diario=linea.mid(30,40).trimmed();
        QDate fecha=QDate::fromString(linea.mid(70,10),"dd-MM-yyyy");
        QString ejercicio=ejerciciodelafecha(fecha);

        qlonglong primer_asiento=basedatos::instancia()->proximoasiento(ejercicio);
        if (primer_asiento==0) primer_asiento=1;
        qlonglong asiento_absoluto=primer_asiento+cadnumasiento.toLongLong()-1;
        cadnumasiento.setNum(asiento_absoluto);

        QString cadfecha=fecha.toString("yyyy-MM-dd");
        QString cuenta=linea.mid(80,30).trimmed();
        QString concepto=linea.mid(110,100).trimmed();
        QString caddebe=linea.mid(210,17);
        if (caddebe.contains('-')) caddebe=caddebe.mid(caddebe.indexOf('-'));
        double debe=convapunto(caddebe).toDouble();
        QString cadhaber=linea.mid(227,17);
        if (cadhaber.contains('-')) cadhaber=cadhaber.mid(cadhaber.indexOf('-'));
        double haber=convapunto(cadhaber).toDouble();
        QString documento=linea.mid(244,80).trimmed();  //linea.right(80).trimmed();
        QString codfactura=linea.right(80).trimmed();


        cadq += cadnumasiento.left(-1).replace("'","''") +",";
        cadq += cadnumpase +",'";
        cadq += cadfecha.left(-1).replace("'","''") +"','";
        cadq += cuenta.left(-1).replace("'","''") +"',";
        QString cadimporte;
        cadimporte.setNum(debe,'f',2);
        cadq += cadimporte;
        cadq += ",";
        cadimporte.setNum(haber,'f',2);
        cadq += cadimporte;
        cadq += ",'";
        cadq += concepto.left(-1).replace("'","''");
        cadq += "','";
        cadq += documento.left(-1).replace("'","''");
        cadq += "','";
        cadq += diario.left(-1).replace("'","''");
        cadq += "',";
        if (usuario.isEmpty())
            cadq += "CURRENT_USER";
        else {
            cadq +="'";
            cadq += usuario;
            cadq +="'";
        }
        cadq += ",'',";
        cadq += "0";
        cadq += ",'";
        cadq += ejercicio.left(-1).replace("'","''");
        cadq += "','";
        cadq += codfactura;
        cadq += "')";
        basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(cuenta,convapunto(cadhaber),convapunto(caddebe));
       }

    //QMessageBox::information( this, tr("Importación"), cadq);

    basedatos::instancia()->ejecutar_publica(cadq);
    chequea_error();
    if (nocontinuar) return QString();
    if (es_lin_etiqueta(linea)) return linea;

  return QString();
}



QString creaempresa::import_datos_auxiliares(QTextStream *stream)
{
    QStringList lrazon, lnombrecomercial, lcif, lweb, ldomicilio,lpoblacion, lcp, lprovincia, ltelefono,
                lpais, lfax, lemail, lobservaciones, lccc, liban, lbic, lsufijo,ldomiciliable, lref_mandato,
                lfirma_mandato, lcodpais, lclaveidfiscal;

    QStringList listacuentas=basedatos::instancia()->listacuentas_datos(lrazon, lnombrecomercial, lcif, lweb, ldomicilio,lpoblacion, lcp, lprovincia, ltelefono,
                                                                        lpais, lfax, lemail, lobservaciones, lccc, liban, lbic, lsufijo,ldomiciliable, lref_mandato,
                                                                        lfirma_mandato, lcodpais, lclaveidfiscal);
    QString linea;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        /*
         Código cuenta 30 Caracteres
         Razón /denominación social 80 Caracteres
         Nombre comercial 80 Caracteres
         cif/nif 40 Caracteres
         página web 80 Caracteres
         domicilio 80 Caracteres
         población 80 Caracteres
         código postal 40 Caracteres
         provincia 80 Caracteres
         teléfono 80 Carateres
         país 80 Caracteres
         fax 80 Caracteres
         email 80 Caracteres
         observaciones 166 Caracteres
         */
        QString codigo=linea.left(30).trimmed();
        QString razon=linea.mid(30,80).trimmed();
        QString nombrecomercial=linea.mid(110,80).trimmed();
        QString cif=linea.mid(190,40).trimmed();
        QString web=linea.mid(230,80).trimmed();
        QString domicilio=linea.mid(310,80).trimmed();
        QString poblacion=linea.mid(390,80).trimmed();
        QString cp=linea.mid(470,40).trimmed();
        QString provincia=linea.mid(510,80).trimmed();
        QString telefono=linea.mid(590,80).trimmed();
        QString pais=linea.mid(670,80).trimmed();
        QString fax=linea.mid(750,80).trimmed();
        QString email=linea.mid(830,80).trimmed();
        // QString observaciones=linea.right(166).trimmed();
        QString observaciones=linea.mid(910,166).trimmed();
        QString ccc_r=linea.mid(1076,20).trimmed();
        QString iban=linea.mid(1096,40).trimmed();
        QString bic=linea.mid(1136,20).trimmed();
        QString sufijo=linea.mid(1156,3).trimmed();
        QString domiciliable=linea.mid(1159,1).trimmed();
        QString ref_mandato=linea.mid(1160,35).trimmed();
        QString cadfechafirma=linea.mid(1195,10);
        QString codpais=linea.mid(1205,2).trimmed();
        if (codpais.isEmpty()) codpais="ES";
        QString claveidfiscal=linea.mid(1207,1).trimmed();
        if (claveidfiscal.isEmpty()) claveidfiscal="1";
        QDate fecha;
        if (!cadfechafirma.isEmpty())
          fecha=QDate::fromString(cadfechafirma,"dd-MM-yyyy");
         else
             fecha=QDate(2009,10,31);

        QString ccc=ccc_r.left(4);
        ccc+="-"+ccc_r.mid(4,4);
        ccc+="-"+ccc_r.mid(8,2);
        ccc+="-"+ccc_r.right(10);
        if (!listacuentas.contains(codigo))
          {
           basedatos::instancia()->insert_datossubcuenta_import(codigo, razon,
                                        nombrecomercial, cif,
                                        web, domicilio,
                                        poblacion, cp,
                                        provincia, telefono,
                                        pais, fax,
                                        email, observaciones,
                                        ccc,iban, bic, sufijo,
                                        domiciliable,
                                        ref_mandato,
                                        fecha, codpais, claveidfiscal);
           chequea_error();
           if (nocontinuar) return QString();
         }
         else
             {
              #ifdef SMART
                QString qcif=basedatos::instancia()->selectCifdatossubcuentacuenta(codigo);
                if (iban.isEmpty() && qcif==cif) continue;
              #endif

              // miramos a ver si hay algún cambio
              int pos=listacuentas.indexOf(codigo);
               //QMessageBox::warning( this, tr("EXPORT"),cif + " - "+lcif.at(pos) + "\n" +
               //                                         fecha.toString("dd-MM-yyyy") + " - " + lfirma_mandato.at(pos));
              if (
                      razon==lrazon.at(pos) &&
                      nombrecomercial==lnombrecomercial.at(pos) &&
                      cif == lcif.at(pos) &&
                      web == lweb.at(pos) &&
                      domicilio == ldomicilio.at(pos) &&
                      poblacion == lpoblacion.at(pos) &&
                      cp == lcp.at(pos) &&
                      provincia == lprovincia.at(pos) &&
                      telefono == ltelefono.at(pos) &&
                      pais == lpais.at(pos) &&
                      fax == lfax.at(pos) &&
                      email == lemail.at(pos) &&
                      observaciones == lobservaciones.at(pos) &&
                      ccc == lccc.at(pos) &&
                      iban == liban.at(pos) &&
                      bic == lbic.at(pos) &&
                      sufijo == lsufijo.at(pos) &&
                      domiciliable == ldomiciliable.at(pos) &&
                      ref_mandato == lref_mandato.at(pos) &&
                      fecha.toString("dd-MM-yyyy") == lfirma_mandato.at(pos) &&
                      codpais == lcodpais.at(pos) &&
                      claveidfiscal == lclaveidfiscal.at(pos)

                      ) continue;
              // QMessageBox::warning( this, tr("EXPORT"),"ACTUALIZA");
              basedatos::instancia()->update_datossubcuenta_import(codigo, razon,
                                          nombrecomercial, cif,
                                          web, domicilio,
                                          poblacion, cp,
                                          provincia, telefono,
                                          pais, fax,
                                          email, observaciones,
                                          ccc,iban, bic, sufijo,
                                          domiciliable,
                                          ref_mandato,
                                          fecha, codpais, claveidfiscal);
              chequea_error();
              if (nocontinuar) return QString();
             }
       }
  return QString();
}

QString creaempresa::import_plan_amortizaciones(QTextStream *stream)
{
    QString linea;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        /*

         Campo                     Ancho  Tipo        Decimales
         Cuenta elemento             30   Caracteres
         Fecha puesta funcionam.     10   Fecha
         Código activo               40   Caracteres
         Cuenta A. Acumulada         30   Caracteres
         Cuenta gasto asociada       30   Caracteres
         Coefeciente amortiz.         7   Coma flotante
         Valor residual              17   Número         2

         */
        QString cuenta_elemento=linea.left(30).trimmed();
        QDate fecha_puesta_func=QDate::fromString(linea.mid(30,10),"dd-MM-yyyy");
        QString codigo_activo=linea.mid(40,40).trimmed();
        QString cuenta_AA=linea.mid(80,30).trimmed();
        QString cuenta_gasto=linea.mid(110,30).trimmed();
        QString coeficiente=linea.mid(140,7);
        QString coeficiente100;
        coeficiente100.setNum(convapunto(coeficiente).toDouble()*100,'f',4);
        QString valor_residual=linea.right(17);
        if (!basedatos::instancia()->cuentaycodigoenplanamort (cuenta_elemento,codigo_activo))
           {
            basedatos::instancia()->insertPlanamortizaciones (cuenta_elemento,
                                                          fecha_puesta_func,
                                                          codigo_activo, cuenta_AA,
                                                          cuenta_gasto, valor_residual,
                                                          coeficiente100,
                                                          "0",
                                                          false,
                                                          false,
                                                          false,
                                                          QDate(2000,1,1),
                                                          QString(),
                                                          QString(), QString());
            chequea_error();
            if (nocontinuar) return QString();
           }
       }
  return QString();

}

QString creaempresa::import_libro_facturas(QTextStream *stream)
{
    QString linea;
    qlonglong paseinicial=basedatos::instancia()->selectProx_paseconfiguracion();
    if (paseinicial==0) paseinicial=1;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        /*
         Campo                        Ancho  Tipo      Decimales
         Apunte en diario               15   Entero
         Cuenta base iva                30   Caracteres
         Base imponible                 17   Número         2
         Clave IVA                      15   Caracteres
         Tipo IVA (%)                    5   Número         2
         Tipo Recargo Equival.(%)        5   Número         2
         Cuota IVA                      17   Número         2
         Cuota REC                      17   Número         2
         Cuenta de factura              30   Caracteres
         Fecha de factura               10   Fecha
         ¿ Es soportado ?                1   Lógico
         ¿ Es AIB ?                      1   Lógico
         ¿ Es A.I. Servicios ?           1   Lógico
         ¿ Adq. serv. extranjero?        1   Lógico
         ¿ Prestación serv. UE ?         1   Lógico
         ¿ Bien inversión ?              1   Lógico
         ¿ EIB ?                         1   Lógico
         Fecha operación                10   Fecha
         EXPORTACIÓN                     1   Lógico
         ¿Factura simplificada?          1   Lógico
         Número facturas                 4   Número
         Factura inicial                30   Caracteres
         Factura final                  30   Caracteres
         Rectificativa                   1   Lógico
         Tipo rectificativa              2   Caracteres
         Agrario/Pesca                   1   Lógico

         */

         QString apunte_diario=linea.left(15);
         qlonglong apunte_absoluto=paseinicial+apunte_diario.toLongLong()-1;
         apunte_diario.setNum(apunte_absoluto);
         QString cuenta_base_iva=linea.mid(15,30).trimmed();
         QString base_imponible=linea.mid(45,17);
         if (base_imponible.contains('-'))
             base_imponible=base_imponible.mid(base_imponible.indexOf('-'));
         QString clave_iva=linea.mid(62,15).trimmed();
         QString tipo_iva=linea.mid(77,5);
         QString tipo_recargo=linea.mid(82,5);
         QString cuota_iva=linea.mid(87,17);
         if (cuota_iva.contains('-'))
             cuota_iva=cuota_iva.mid(cuota_iva.indexOf('-'));
         QString cuota_rec=linea.mid(104,17);
         if (cuota_rec.contains('-'))
             cuota_rec=cuota_rec.mid(cuota_rec.indexOf('-'));

/*         bool bien;
         double bi = convapunto(base_imponible).toDouble(& bien);
         if (!bien) bi=0;
         double tipo = convapunto(tipo_iva).toDouble(& bien)/100;
         if (!bien) tipo=0;
          QString cuota_cad;
          double cuota=CutAndRoundNumberToNDecimals (bi*tipo, 2);

          double re = convapunto(tipo_recargo).toDouble( & bien)/100;
          if (!bien) re=0;
          cuota += CutAndRoundNumberToNDecimals (bi*re, 2);
          cuota_cad.setNum(cuota,'f',2);
*/

         double cuota=convapunto(cuota_iva).toDouble()+convapunto(cuota_rec).toDouble();
         QString cuota_cad;
         cuota_cad.setNum(cuota,'f',2);

         QString cuenta_factura=linea.mid(121,30).trimmed();
         QDate fecha_factura=QDate::fromString(linea.mid(151,10),"dd-MM-yyyy");
         bool soportado = (linea.mid(161,1) == "1");
         bool aib = (linea.mid(162,1) == "1");
         bool ais = (linea.mid(163,1) == "1");
         bool adq_serv_extranjero = (linea.mid(164,1) == "1");
         bool prest_serv_ue = (linea.mid(165,1) == "1");
         bool bien_inversion = (linea.mid(166,1) == "1");
         bool eib=   (linea.mid(167,1) == "1");
         QDate fecha_operacion=QDate::fromString(linea.mid(168,10),"dd-MM-yyyy");
         bool exportacion= (linea.mid(178,1) == "1");

         // ¿Factura simplificada?          1   Lógico
         bool simplificada= (linea.mid(179,1) == "1");

         // Número facturas                 4   Número
         QString cad_num_facturas=linea.mid(180,4).trimmed();

         // Factura inicial                30   Caracteres
         QString factura_inicial=linea.mid(184,30).trimmed();

         // Factura final                  30   Caracteres
         QString factura_final=linea.mid(214,30).trimmed();

         // Rectificativa                   1   Lógico
         bool rectificativa= (linea.mid(244,1) == "1");

         // Tipo rectificativa              2   Caracteres
         QString tipo_rectificativa=linea.mid(245,2);

         // Régimen agrario                   1   Lógico
         bool agrario= (linea.mid(247,1) == "1");

         bool exento= (linea.mid(248,1) == "1");
         bool importacion= (linea.mid(249,1) == "1");

         // if (!soportado)
         //    QMessageBox::warning( this, tr("EXPORT"),apunte_diario + " - "+base_imponible);


         // nuevos campos
         //bool simplificada, QString nfacturas, QString finicial,
         //QString ffinal, bool rectificativa, QString tipo_rectificativa

         basedatos::instancia()->insert_libroiva_import(
                         apunte_diario,
                         cuenta_base_iva,
                         base_imponible,
                         clave_iva,
                         tipo_iva,
                         tipo_recargo,
                         cuota_cad,
                         cuenta_factura,
                         fecha_factura,
                         soportado,
                         aib,
                         ais,
                         adq_serv_extranjero,
                         prest_serv_ue,
                         bien_inversion,
                         eib,
                         fecha_operacion,
                         exportacion, simplificada, cad_num_facturas, factura_inicial, factura_final,
                         rectificativa, tipo_rectificativa, agrario, exento, importacion
                         );

         // ---------------------------------------------------------------
         QString cadnumrecibidas;

         if (soportado && basedatos::instancia()->hay_secuencia_recibidas())
            {
             // primero - si el apunte correspondiente no tiene secuencia --> asignar al asiento
             QString asiento,qejercicio;
             int numsec=basedatos::instancia()->recepcion_mas_datos_diario(
                     apunte_diario, &asiento, &qejercicio);
             if (numsec==0)
                {
                 qlonglong recep=basedatos::instancia()->proximo_nrecepcion(qejercicio);
                 cadnumrecibidas.setNum(recep);
                 QString cadpaso; cadpaso.setNum(recep+1);
                 basedatos::instancia()->pasa_prox_nrecepcion_ejercicio(qejercicio,cadpaso);
                 // actualizamos para asiento y ejercicio con cadnumrecibidas
                 basedatos::instancia()->actu_recep_asto_ejercicio(
                         cadnumrecibidas, asiento, qejercicio);
                }
            }

         // ---------------------------------------------------------------
         chequea_error();
         if (nocontinuar) return QString();

       }
  return QString();

}




QString creaempresa::import_libro_facturas_pg(QTextStream *stream)
{
    QString linea;
    qlonglong paseinicial=basedatos::instancia()->selectProx_paseconfiguracion();
    if (paseinicial==0) paseinicial=1;

    QString cadquery= "insert into libroiva (pase, cta_base_iva, base_iva, clave_iva,";
            cadquery += "tipo_iva, tipo_re, cuota_iva, cuenta_fra,";
            cadquery += "fecha_fra,soportado, aib, autofactura, autofactura_no_ue,";
            cadquery += "pr_servicios_ue, bien_inversion, eib, fecha_operacion, "
                        "exportacion, clave_operacion, nfacturas, finicial, ffinal, "
                        "rectificativa, rectificada, agrario, exento_dcho_ded, importacion) ";
            cadquery += "values(";

    bool primero=true;

    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea) && primero) return linea;
        if (es_lin_etiqueta(linea)) break;

        if (primero) primero=false;
        else {
            cadquery+=", (";
        }

        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;

         QString apunte_diario=linea.left(15);
         qlonglong apunte_absoluto=paseinicial+apunte_diario.toLongLong()-1;
         apunte_diario.setNum(apunte_absoluto);
         QString cuenta_base_iva=linea.mid(15,30).trimmed();
         QString base_imponible=linea.mid(45,17);
         if (base_imponible.contains('-'))
             base_imponible=base_imponible.mid(base_imponible.indexOf('-'));
         QString clave_iva=linea.mid(62,15).trimmed();
         QString tipo_iva=linea.mid(77,5);
         QString tipo_recargo=linea.mid(82,5);
         QString cuota_iva=linea.mid(87,17);
         if (cuota_iva.contains('-'))
             cuota_iva=cuota_iva.mid(cuota_iva.indexOf('-'));
         QString cuota_rec=linea.mid(104,17);
         if (cuota_rec.contains('-'))
             cuota_rec=cuota_rec.mid(cuota_rec.indexOf('-'));


         double cuota=convapunto(cuota_iva).toDouble()+convapunto(cuota_rec).toDouble();
         QString cuota_cad;
         cuota_cad.setNum(cuota,'f',2);

         QString cuenta_factura=linea.mid(121,30).trimmed();
         QDate fecha_factura=QDate::fromString(linea.mid(151,10),"dd-MM-yyyy");
         bool soportado = (linea.mid(161,1) == "1");
         bool aib = (linea.mid(162,1) == "1");
         bool ais = (linea.mid(163,1) == "1");
         bool adq_serv_extranjero = (linea.mid(164,1) == "1");
         bool prest_serv_ue = (linea.mid(165,1) == "1");
         bool bien_inversion = (linea.mid(166,1) == "1");
         bool eib=   (linea.mid(167,1) == "1");
         QDate fecha_operacion=QDate::fromString(linea.mid(168,10),"dd-MM-yyyy");
         bool exportacion= (linea.mid(178,1) == "1");

         // ¿Factura simplificada?          1   Lógico
         bool simplificada= (linea.mid(179,1) == "1");

         // Número facturas                 4   Número
         QString cad_num_facturas=linea.mid(180,4).trimmed();

         // Factura inicial                30   Caracteres
         QString factura_inicial=linea.mid(184,30).trimmed();

         // Factura final                  30   Caracteres
         QString factura_final=linea.mid(214,30).trimmed();

         // Rectificativa                   1   Lógico
         bool rectificativa= (linea.mid(244,1) == "1");

         // Tipo rectificativa              2   Caracteres
         QString tipo_rectificativa=linea.mid(245,2);

         // Régimen agrario                   1   Lógico
         bool agrario= (linea.mid(247,1) == "1");

         bool exento= (linea.mid(248,1) == "1");
         bool importacion= (linea.mid(249,1) == "1");

         cadquery += apunte_diario.left(-1).replace("'","''");
         cadquery += ",'";
         cadquery += cuenta_base_iva.left(-1).replace("'","''");
         cadquery += "',";
         cadquery += base_imponible.left(-1).replace("'","''");
         cadquery += ",'";
         cadquery += clave_iva.left(-1).replace("'","''");
         cadquery += "',";
         cadquery += tipo_iva.left(-1).replace("'","''");
         cadquery += ",";
         cadquery += tipo_recargo.left(-1).replace("'","''");
         cadquery += ",";
         cadquery += cuota_cad.left(-1).replace("'","''");
         cadquery += ",'";
         cadquery += cuenta_factura.left(-1).replace("'","''");
         cadquery += "','";

         QString cadfecha=fecha_factura.toString("yyyy-MM-dd");
         cadquery += cadfecha;
         cadquery += "',";

         cadquery+=(soportado ? "true" : "false");
         cadquery += ",";
         cadquery+=(aib ? "true" : "false");
         cadquery += ",";
         cadquery+=(ais ? "true" : "false");
         cadquery += ",";
         cadquery+=(adq_serv_extranjero ? "true" : "false");
         cadquery += ",";
         cadquery+=(prest_serv_ue ? "true" : "false");
         cadquery += ",";
         cadquery+=(bien_inversion ? "true" : "false");
         cadquery += ",";
         cadquery+=(eib ? "true" : "false");

         cadquery+=", '";
         cadquery+=fecha_operacion.toString("yyyy-MM-dd");
         cadquery+="',";
         cadquery+= exportacion ? "true":"false";
         cadquery+=", ";
         cadquery+= simplificada ? "'J', " : "'', ";
         if (cad_num_facturas.isEmpty()) cad_num_facturas="0";
         cadquery+=cad_num_facturas;
         cadquery+=", '"+factura_inicial.left(-1).replace("'","''")+"','"+factura_final.left(-1).replace("'","''")+"', ";
         cadquery+= rectificativa ? "true": "false";
         cadquery+=", '";
         cadquery+=tipo_rectificativa.left(-1).replace("'","''");
         cadquery+="',";
         cadquery+= agrario ? "true": "false";
         cadquery+=",";
         cadquery+= exento ? "true": "false";
         cadquery+=",";
         cadquery+= importacion ? "true": "false";
         cadquery+=")";

         // ---------------------------------------------------------------

         // ---------------------------------------------------------------
       }
    // QMessageBox::information( this, tr("Importación"), cadquery);

    basedatos::instancia()->ejecutar_publica(cadquery);
    chequea_error();
    if (nocontinuar) return QString();
    if (es_lin_etiqueta(linea)) return linea;

  return QString();

}




QString creaempresa::import_vencimientos(QTextStream *stream)
{
    QString linea;
    qlonglong paseinicial=basedatos::instancia()->selectProx_paseconfiguracion();
    if (paseinicial==0) paseinicial=1;
    qlonglong vnum = 0; vnum= basedatos::instancia()->proximovencimiento();
    if (vnum==0) vnum=1;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        /*
          Campo                  Ancho   Tipo        Decimales
          Número vencimiento        15   Entero
          Cuenta ordenante          30   Caracteres
          Fecha operación           10   Fecha
          Importe                   17   Número        2
          Cuenta Tesorería asoc.    30   Caracteres
          Fecha vencimiento         10   Fecha
          Derecho                    1   Lógico
          Pendiente                  1   Lógico
          Apunte oper. diario       15   Entero
          Apunte vto. procesado     15   Entero
          Domiciliable               1   Lógico
          */
        QString numvencimiento=linea.left(15);
        qlonglong numvenciabs=numvencimiento.toLongLong()+vnum-1;
        numvencimiento=numvencimiento.setNum(numvenciabs);

        QString cuenta_ordenante=linea.mid(15,30).trimmed();
        QDate fecha_operacion=QDate::fromString(linea.mid(45,10),"dd-MM-yyyy");
        QString importe=linea.mid(55,17);
        if (importe.contains('-'))
            importe=importe.mid(importe.indexOf('-'));
        QString cuenta_tesoreria=linea.mid(72,30).trimmed();
        QDate fecha_vencimiento=QDate::fromString(linea.mid(102,10),"dd-MM-yyyy");
        bool derecho=linea.mid(112,1)=="1";
        bool pendiente=linea.mid(113,1)=="1";
        QString apunte_diario=linea.mid(114,15);
        qlonglong apunte_absoluto=paseinicial+apunte_diario.toLongLong()-1;
        apunte_diario.setNum(apunte_absoluto);
        QString apunte_vto=linea.mid(129,15);
        bool domiciliable=linea.mid(144,1)=="1";
        apunte_absoluto=paseinicial+apunte_vto.toLongLong()-1;
        apunte_vto.setNum(apunte_absoluto);
        basedatos::instancia()->insertavenci_import(numvencimiento,
                                    cuenta_ordenante,
                                    fecha_operacion,
                                    importe,
                                    cuenta_tesoreria,
                                    fecha_vencimiento,
                                    derecho,
                                    pendiente,
                                    apunte_diario,
                                    apunte_vto,
                                    domiciliable
                                    );
        chequea_error();
        if (nocontinuar) return QString();
        QString cadnum; cadnum.setNum(numvencimiento.toLongLong()+1);
        basedatos::instancia()->updateConfiguracionprox_vencimiento(cadnum);

      }
  return QString();

}


QString creaempresa::import_ci(QTextStream *stream)
{
    QString linea;
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        /*
          Campo                  Ancho   Tipo        Decimales
          Código                    40   Caracteres
          Descripción               80   Caracteres
          Nivel                      1   Entero
          */
        QString codigo=linea.left(40).trimmed();
        QString descripcion=linea.mid(40,80).trimmed();
        int nivel=linea.mid(120,1).toInt();
        if (nivel<1 || nivel>3) continue;
        if (nivel<3) codigo=codigo.left(3);
        QString cadnivel;
        QString cadena;
        bool encontrada=buscaci(codigo,&cadena,&cadnivel);
        if (encontrada)
             {
              guardacambiosci(codigo,descripcion,nivel);

             }
         else
             {
              insertaenci(codigo,descripcion,nivel);
             }

        chequea_error();
        if (nocontinuar) return QString();
      }
  return QString();
}


QString creaempresa::import_diario_ci(QTextStream *stream)
{
    QString linea;
    int guarda_apunte=0;
    QString cadproxci="0";
    while(!stream->atEnd())
      {
        linea=stream->readLine();
        if (es_lin_etiqueta(linea)) return linea;
        if (linea.isEmpty()) continue;
        if (linea[0]=='#') continue;
        // procesamos los registros
        // 01234567890123456789012345678901234567890123456789012345678901234567890
        // 12345678901234567890123456789012345678901234567890123456789012345678901
        // --------------}--------------]-----------------------------]
        /*
          Campo                  Ancho   Tipo        Decimales
          Apunte en diario          15   Entero
          Código de imputación      50   Caracteres
          Importe                   17   Número        4
          */
        int apunte=linea.left(15).toInt();
        bool encontrado=false;
        for (int veces=0; veces< apuntes_origen.count();veces++)
          {
            if (apuntes_origen.at(veces)==apunte)
              {
               apunte=apuntes_conta.at(veces);
               encontrado=true;
               break;
              }
          }
        if (!encontrado) continue;
        if (guarda_apunte!=apunte)
           {
            // generamos código de enlace
            qlonglong proxnumci=basedatos::instancia()->prox_num_ci();
            cadproxci.setNum(proxnumci);
            // buscamos apunte en diario y le asignamos numci
            QString cadapunte; cadapunte.setNum(apunte);
            basedatos::instancia()->asigna_clave_ci_apunte(cadapunte, cadproxci);
            basedatos::instancia()->incrementa_prox_num_ci();
            guarda_apunte=apunte;
           }

        QString codigo_imputacion=linea.mid(15,50).trimmed();
        QString importe=linea.mid(65,17).trimmed();

        basedatos::instancia()->inserta_diario_ci(cadproxci,
                                                  codigo_imputacion,
                                                  importe);

        chequea_error();
        if (nocontinuar) return QString();
      }
  return QString();
}




void creaempresa::chequea_error()
{
    if (basedatos::instancia()->hayerror_consulta())
      {
        basedatos::instancia()->reset_error_consulta();
        errores_import=true;
        /*
        if (QMessageBox::question(this,
            tr("IMPORTACIÓN"),
            tr("Hay errores de importación.\n"
               "¿ Desea continuar ?"),
            tr("&Sí"), tr("&No"),
            QString::null, 0, 1 ) )
          {
            nocontinuar=true;
          } */
        nocontinuar=true;
      }
}
