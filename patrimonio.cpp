/* ----------------------------------------------------------------------------------
    "patrimonio", estado de cambios en el patrimonio neto de pymes
     complemento de KEME-Contabilidad 2.0

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


#include "patrimonio.h"
#include "funciones.h"
#include "pidenombre.h"
#include "basedatos.h"
#include "directorio.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

patrimonio::patrimonio() : QDialog() {
    ui.setupUi(this);
 columnas << tr("CONCEPTOS") ;

 columnas << tr("Capital Escritur.");
 columnas << tr("Capital no exigido");
 columnas << tr("Prima de emisión");
 columnas << tr("Reservas");
 columnas << tr("(Acc. y p.propias)");
 columnas << tr("Resultados ej. anter.");
 columnas << tr("Otras ap. socios");
 columnas << tr("Resultado ejercicio");
 columnas << tr("(Dividendo a cuenta)");
 columnas << tr("Ajustes de pat. neto");
 columnas << tr("Sub. don. leg. rec.");
 columnas << tr("TOTAL");

 ui.tabla->setHorizontalHeaderLabels(columnas);

 ui.tabla->setColumnWidth(0,200);

 for (int veces=1;veces<=11;veces++)
      ui.tabla->setColumnWidth(veces,150);

 ui.tabla->setItem(0,0,new QTableWidgetItem(tr("A. SALDO, FINAL DEL AÑO 200X-2")));
 ui.tabla->item(0,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(1,0,new QTableWidgetItem(tr("I. Ajustes por cambios de criterio 200X-2 y anteriores")));
 ui.tabla->item(1,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(2,0,new QTableWidgetItem(tr("II. Ajustes por errores 200X-2 y anteriores")));
 ui.tabla->item(2,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(3,0,new QTableWidgetItem(tr("B. SALDO AJUS. INICIO DEL AÑO 200X-1")));
 ui.tabla->item(3,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(4,0,new QTableWidgetItem(tr("I. Resultado de la cuenta de pérdidas y ganancias")));
 ui.tabla->item(4,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(5,0,new QTableWidgetItem(tr("II. Ingresos y gastos reconocidos en patrimonio neto")));
 ui.tabla->item(5,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(6,0,new QTableWidgetItem(tr("1. Ingresos fiscales a distribuir en varios ejercicios")));
 ui.tabla->item(6,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(7,0,new QTableWidgetItem(tr("2. Otros ingresos y gastos reconocidos en pat. neto")));
 ui.tabla->item(7,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(8,0,new QTableWidgetItem(tr("III. Operaciones con socios o propietarios")));
 ui.tabla->item(8,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(9,0,new QTableWidgetItem(tr("1. Aumentos de capital")));
 ui.tabla->item(9,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(10,0,new QTableWidgetItem(tr("2. (-) Reducciones de capital")));
 ui.tabla->item(10,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(11,0,new QTableWidgetItem(tr("3. Otras operaciones con socios o propietarios")));
 ui.tabla->item(11,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(12,0,new QTableWidgetItem(tr("IV. Otras variaciones del patrimonio neto")));
 ui.tabla->item(12,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(13,0,new QTableWidgetItem(tr("1. Movimientos Reserva Revalorización")));
 ui.tabla->item(12,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(14,0,new QTableWidgetItem(tr("2. Otras variaciones")));
 ui.tabla->item(12,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);


 ui.tabla->setItem(15,0,new QTableWidgetItem(tr("C. SALDO, FINAL DEL AÑO N-1")));
 ui.tabla->item(15,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(16,0,new QTableWidgetItem(tr("I. Ajustes por cambios de criterio 200X-1")));
 ui.tabla->item(16,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(17,0,new QTableWidgetItem(tr("II. Ajustes por errores 200X-1")));
 ui.tabla->item(17,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(18,0,new QTableWidgetItem(tr("D. SALDO AJUS., INICIO DEL AÑO 200X")));
 ui.tabla->item(18,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(19,0,new QTableWidgetItem(tr("I. Resultado de la cuenta de pérdidas y ganancias")));
 ui.tabla->item(19,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(20,0,new QTableWidgetItem(tr("II. Ingresos y gastos reconocidos en el patrimonio neto")));
 ui.tabla->item(20,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(21,0,new QTableWidgetItem(tr("1. Ingresos fiscales a distribuir en varios ejercicios")));
 ui.tabla->item(21,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(22,0,new QTableWidgetItem(tr("2. Otros ingresos y gastos reconocidos en pat. neto")));
 ui.tabla->item(22,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);


 ui.tabla->setItem(23,0,new QTableWidgetItem(tr("III. Operaciones con socios o propietarios")));
 ui.tabla->item(23,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(24,0,new QTableWidgetItem(tr("1. Aumentos de capital")));
 ui.tabla->item(24,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(25,0,new QTableWidgetItem(tr("2. (-) Reducciones de capital")));
 ui.tabla->item(25,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(26,0,new QTableWidgetItem(tr("3. Otras operaciones con socios o propietarios")));
 ui.tabla->item(26,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(27,0,new QTableWidgetItem(tr("IV. Otras variaciones del patrimonio neto")));
 ui.tabla->item(27,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(28,0,new QTableWidgetItem(tr("1. Movimiento Reservas Revalorización")));
 ui.tabla->item(28,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(29,0,new QTableWidgetItem(tr("2. Otras variaciones")));
 ui.tabla->item(29,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 ui.tabla->setItem(30,0,new QTableWidgetItem(tr("E.SALDO, FINAL DEL AÑO 200X")));
 ui.tabla->item(30,0)->setTextAlignment (Qt::AlignJustify | Qt::AlignVCenter);

 fichero=tr("varpat");

    for (int veces=0;veces<31;veces++)
        {
         for (int veces2=1;veces2<13;veces2++)
            {
             ui.tabla->setItem(veces,veces2,new QTableWidgetItem(""));
             ui.tabla->item(veces,veces2)->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
            }
        }


 connect(ui.texpushButton,SIGNAL(clicked()),this,SLOT(gentex()));
 connect(ui.salirpushButton,SIGNAL(clicked()),this,SLOT(salir()));
 connect(ui.tabla,SIGNAL( cellChanged ( int , int )),this,
           SLOT(contenidocambiado(int,int )));
 connect(ui.imprimirpushButton,SIGNAL(clicked()),this,SLOT(impritex()));
 connect(ui.grabarpushButton,SIGNAL(clicked()),this,SLOT(grabafich()));
 connect(ui.recuperarpushButton,SIGNAL(clicked()),this,SLOT(recuperafich()));
 connect(ui.copiarpushButton,SIGNAL(clicked()),this,SLOT(copiar()));
 connect(ui.calcularpushButton,SIGNAL(clicked()),this,SLOT(calculo()));

 comadecimal=haycomadecimal();
 decimales=haydecimales();
 pregunta_salir=true;

}


void patrimonio::generalatex()
{
    QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero+=fichero;
   qfichero+=".tex";

   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero2(pasa);

    if (! fichero2.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero2 );
    stream.setEncoding(QStringConverter::Utf8);
    if (es_os_x()) stream << soporte_xelatex();
    stream << "\\documentclass[11pt,a4paper]{article}" << "\n";
    if (es_os_x()) stream << config_font_xelatex();
       else
            {
             stream << "\\usepackage{ucs}" << "\n";
             stream << "\\usepackage[utf8x]{inputenc}" << "\n";
            }
    stream << "\\usepackage[spanish]{babel}" << "\n";
    stream << "\\usepackage{longtable}" << "\n";
    stream << "\\usepackage{lscape}" << "\n";
    stream << "\\usepackage{anysize}\n";
    stream << "\\begin{document}" << "\n";
    stream << "% CUERPO" << "\n";

    stream << "\\marginsize{1.5cm}{1cm}{2cm}{1cm}\n";
    stream << "\\begin{landscape}" << "\n";
    stream << "\\begin{center}";
    stream << "\\begin{longtable}{|p{6cm}|r|r|r|r|r|r|r|r|r|r|r|r|}";
    stream << "\\hline";
    stream << "\\multicolumn{13}{|c|} {\\scriptsize{";
    stream << ui.titulolineEdit->text();
    stream << "}} \\\\\n\\hline";
    stream << "{\\tiny }";
    for (int veces=1;veces<12;veces++)
        stream << " & \\multicolumn{1}{|p{1.2cm}|}{\\tiny{" << columnas.at(veces) << "}}";
    stream << " & {\\tiny{" << columnas.at(12) << "}} \\\\\n";
    stream << "\\hline";
    stream << "\\endfirsthead\n\\hline";

    stream << "{\\tiny }";
    for (int veces=1;veces<12;veces++)
        stream << " & \\multicolumn{1}{|p{1.2cm}|}{\\tiny{" << columnas.at(veces) << "}}";
    stream << " & {\\tiny{" << columnas.at(12) << "}} \\\\\n";
    stream << "\\hline\n";
    stream << "\\endhead\n";

    // ahora va el contenido de todo el cuadro

    for (int veces=0;veces<31;veces++)
        {
         stream << "{\\tiny{";
         if (ui.tabla->item(veces,0)!=0)
            {
             stream << ui.tabla->item(veces,0)->text();
             stream << "}}";
            }
         for (int veces2=1;veces2<13;veces2++)
            {
             stream <<  " & {\\tiny{";
             if (ui.tabla->item(veces,veces2)->text().length()>0)
                 {
                  double valor= convapunto(ui.tabla->item(veces,veces2)->text()).toDouble();
                  stream << formateanumerosep(valor,true,true);
                 }
             stream << "}}";
            }
         stream << " \\\\\n";

         switch(veces)
	     {
	      case 0 : 
              case 2 :
              case 3 :
              case 4 :
              case 7 :
              case 11 :
              case 14:
              case 15:
              case 17:
              case 18:
              case 19:
              case 22:
             case 26:
             case 29:
                    stream << "\\hline\n";
	       break;
	     }
        }

    stream << "\\hline\n";
    stream << "\\end{longtable}\n";
    stream << "\\end{center}\n";
    stream << "\\end{landscape}\n";
    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}\n";

    fichero2.close();
}

void patrimonio::gentex()
{
   pidenombre *p=new pidenombre();
   int resultado=0;
   p->asignanombreventana(tr("Estado de variaciones patrimonio neto"));
   p->asignaetiqueta(tr("NOMBRE FICHERO (sin extensión):"));
   resultado=p->exec();
   if (resultado==QDialog::Accepted)
      {
       if (p->contenido().length()>0) fichero=p->contenido();
       fichero.remove('*');
       fichero.remove("..");
       fichero.remove('/');
       fichero.replace(' ','_');
      }
   p->~pidenombre();
   generalatex();

   int valor=consultalatex2(fichero);
   if (valor==1)
       QMessageBox::warning( this, tr("VISTA PREVIA"),
                             tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("VISTA PREVIA"),
                             tr("PROBLEMAS al llamar a ")+visordvi());

 fichero=tr("varpat");


}


void patrimonio::contenidocambiado(int fila,int col)
{
 if (col==0) return;
 bool ok;
 double valor=convapunto(ui.tabla->item(fila,col)->text()).toDouble(&ok);
 if (ok)
    {
     ui.tabla->item(fila,col)->setText(
             formateanumero(valor,comadecimal,decimales));
    }
    else ui.tabla->item(fila,col)->setText("");
}

QString patrimonio::contenido(int fila, int columna)
{
  if (ui.tabla->item(fila,columna)==NULL) return QString();

  return ui.tabla->item(fila,columna)->text();
}

void patrimonio::salir()
{
   if (pregunta_salir)
     if (QMessageBox::question(
            this,
            tr("SALIR"),
             tr("¿ Desea salir ?")) == QMessageBox::Yes )
                          accept();
  accept();
}


void patrimonio::modo_deposito()
{
  pregunta_salir=false;
  ui.salirpushButton->setText(tr("&Aceptar"));
}

void patrimonio::impritex()
{
 fichero=tr("varpat");
    generalatex();

   int valor=imprimelatex2(tr("varpat"));
   if (valor==1)
       QMessageBox::warning( this, tr("IMPRIMIR ESTADO"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("IMPRIMIR ESTADO"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("IMPRIMIR ESTADO"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


bool patrimonio::grabado(QString nombre)
{
  QFile fichero( adapta(nombre)  );

  if ( !fichero.open( QIODevice::WriteOnly ) ) 
       {
        QMessageBox::warning( this, tr("GRABAR ESTADO"),tr("Error: Imposible grabar fichero"));
        return false;
       }
  QTextStream stream( &fichero );
  stream.setEncoding(QStringConverter::Utf8);

  stream << ui.titulolineEdit->text() << "\n";

    for (int veces=0;veces<=12;veces++)
       {
        stream << columnas.at(veces);
        if (veces!=12) stream << "\t";
            else stream << "\n";
       }

    // ahora va el contenido de todo el cuadro

    for (int veces=0;veces<31;veces++)
        {
         for (int veces2=0;veces2<13;veces2++)
            {
             if (ui.tabla->item(veces,veces2)->text().length()>0)
                  stream << ui.tabla->item(veces,veces2)->text();
             if (veces2==12) stream << "\n";
                else stream << "\t";
            }
        }

  fichero.close();
  return true;
}

bool patrimonio::cargado(QString nombre)
{
  QFile fichero(adapta(nombre));
  if (!fichero.exists()) return false;
   if ( fichero.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &fichero );
        stream.setEncoding(QStringConverter::Utf8);
        if ( !stream.atEnd() ) {
            QString linea;
            linea = stream.readLine(); // linea de texto excluyendo '\n'
            ui.titulolineEdit->setText(linea);
           } else return false;
        int pos=-1;
        while ( !stream.atEnd() && pos<ui.tabla->rowCount()) {
             QString linea = stream.readLine();
             if (pos<0)
                 {
                  columnas.clear();
                  for (int veces=0;veces<=12 ; veces++)
                       {
                        columnas << linea.section('\t',veces,veces);
                       }
                 }
                 else
                      {
                       for (int veces=0;veces<=12 ; veces++)
                          ui.tabla->item(pos,veces)->setText(linea.section('\t',veces,veces));
                      }
             pos++;
            }
        fichero.close();
      } else return false;
  return true;
}


void patrimonio::grabafich()
{
  QString nombre;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajo());
  dir->activa_pide_archivo(tr("ecpn.txt"));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      nombre=dir->respuesta_arch();
      QString rutadir=dir->directorio_actual();
      if (!rutadir.endsWith(QDir::separator())) rutadir.append(QDir::separator());
      nombre=rutadir+nombre;
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
  filtros << tr("Archivos de texto (*.txt)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("GRABAR CONTENIDO DEL ESTADO CONTABLE"));
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
           nombre=fileNames.at(0);
          }
     }

#endif

  if (nombre.isEmpty()) return;

  if (nombre.right(4)!=".txt") nombre=nombre+".txt";
   QFile plan( nombre  );
   if (plan.exists() && QMessageBox::question(this,
       tr("¿ Sobreescribir ?"),
       tr("'%1' ya existe."
          "¿ Desea sobreescribirlo ?")
         .arg( nombre )) == QMessageBox::No)
     return ;
   if (grabado(nombre))
      {
       QMessageBox::information( this, tr("GRABAR ESTADO EN FICHERO"),
                    tr("El archivo se ha generado correctamente"));
      }

}

void patrimonio::recuperafich()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajo()));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      if (cargado(dir->ruta_actual()))
          {
           QMessageBox::information( this, tr("DIARIO"),
                        tr("El archivo seleccionado se ha cargado correctamente"));
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
  filtros << tr("Archivos de texto (*.txt)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE ARCHIVO PARA CARGAR"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           if (cargado(fileNames.at(0)))
               {
                QMessageBox::information( this, tr("DIARIO"),
                             tr("El archivo seleccionado se ha cargado correctamente"));
               }
          }
     }
#endif
}

void patrimonio::copiar()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;


  global=ui.titulolineEdit->text();
  global+="\n";
    for (int veces=0;veces<=12;veces++)
       {
        global+=columnas.at(veces);
        if (veces!=12) global+= "\t";
            else global+= "\n";
       }

    // ahora va el contenido de todo el cuadro

    for (int veces=0;veces<31;veces++)
        {
         for (int veces2=0;veces2<13;veces2++)
            {
             if (ui.tabla->item(veces,veces2)->text().length()>0)
                  global+= ui.tabla->item(veces,veces2)->text();
             if (veces2==12) global+= "\n";
                else global+= "\t";
            }
        }



    cb->setText(global);
    QMessageBox::information( this, tr("Variaciones en el patrimonio neto"),
                             tr("Se ha pasado el contenido al portapapeles") );

}

void patrimonio::pasa_ejercicio(QString qejercicio)
{
    ejercicio=qejercicio;
 // "ESTADO DE CAMBIOS EN EL PATRIMONIO NETO DE PYMES CORRESPONDIENTE AL EJERCICIO TERMINADO EL .... DE 200X""
    QString cabecera=tr("ESTADO DE CAMBIOS EN EL PATRIMONIO NETO DE PYMES CORRESPONDIENTE AL EJERCICIO TERMINADO EL ");
    cabecera+=finejercicio(qejercicio).toString("dd-MM-yyyy");
    ui.titulolineEdit->setText(cabecera);

    QDate fecha_inicio_ej2=inicioejercicio(ejercicio);
    fecha_inicio_ej2=fecha_inicio_ej2.addYears(-2);
    if (fechacorrespondeaejercicio(fecha_inicio_ej2))
      {
        ui.tabla->item(0,0)->setText(tr("A. SALDO, FINAL DEL EJERCICIO ")+
                                     ejerciciodelafecha(fecha_inicio_ej2));
        QString cadena=tr("I. Ajustes por cambios de criterio ");
        cadena+= ejerciciodelafecha(fecha_inicio_ej2);
        cadena += tr(" y anteriores");
        ui.tabla->item(1,0)->setText(cadena);

        cadena=tr("II. Ajustes por errores ");
        cadena+= ejerciciodelafecha(fecha_inicio_ej2);
        cadena += tr(" y anteriores");
        ui.tabla->item(2,0)->setText(cadena);
      }

    QDate fecha_inicio_ej1=inicioejercicio(ejercicio);
    fecha_inicio_ej1=fecha_inicio_ej1.addYears(-1);
    if (fechacorrespondeaejercicio(fecha_inicio_ej1))
      {
       QString cadena=tr("B. SALDO AJUS. INICIO DEL EJERCICIO ");
       cadena+= ejerciciodelafecha(fecha_inicio_ej1);
       ui.tabla->item(3,0)->setText(cadena);

       cadena=tr("C. SALDO, FINAL DEL EJERCICIO ");
       cadena+=ejerciciodelafecha(fecha_inicio_ej1);
       ui.tabla->item(15,0)->setText(cadena);

       cadena=tr("I. Ajustes por cambios de criterio ");
       cadena+=ejerciciodelafecha(fecha_inicio_ej1);
       ui.tabla->item(16,0)->setText(cadena);

       cadena=tr("I. Ajustes por errores ");
       cadena+=ejerciciodelafecha(fecha_inicio_ej1);
       ui.tabla->item(17,0)->setText(cadena);

      }

    ui.tabla->item(18,0)->setText(tr("D. SALDO AJUS. INICIO DEL EJERCICIO ")+ejercicio);
    ui.tabla->item(30,0)->setText(tr("E.SALDO, FINAL DEL EJERCICIO ")+ejercicio);

    QString ejercicio_2,ejercicio_1;
    if (fechacorrespondeaejercicio(fecha_inicio_ej2))
        ejercicio_2=ejerciciodelafecha(fecha_inicio_ej2);
    if (fechacorrespondeaejercicio(fecha_inicio_ej1))
        ejercicio_1=ejerciciodelafecha(fecha_inicio_ej1);

    QSqlQuery q=basedatos::instancia()->carga_cols_evpn_pymes();

    if (q.isActive())
        while (q.next())
        {
         int numero=q.value(0).toInt();
         QString contenido=q.value(2).toString(); // lista de cuentas
         //QSqlQuery select_cods_evpn_pymes_diario (bool apertura, QString ejercicio,
         //                                               QString cadenacuentas);
         // datos de apertura de ejercicio
         if (!ejercicio_1.isEmpty())
           {
            QSqlQuery q2=basedatos::instancia()->select_apertura_diario(ejercicio_1,
                                                                     contenido);
            if (q2.isActive())
               if (q2.next())
                {
                  // QMessageBox::warning( this, tr("Estados Contables"),q.value(0).toString());

                  double resultado=q2.value(0).toDouble()-q2.value(1).toDouble();
                  if (!ejercicio_2.isEmpty() && (resultado>0.0001 || resultado<-0.0001))
                    ui.tabla->item(0,numero)->setText(formateanumero(resultado,comadecimal,decimales));
                  if (resultado>0.0001 || resultado<-0.0001)
                  ui.tabla->item(3,numero)->setText(formateanumero(resultado,comadecimal,decimales));
                }
           }
         // datos del ejercicio-1
         if (!ejercicio_1.isEmpty())
           {
            QSqlQuery q2=basedatos::instancia()->select_cods_evpn_pymes_diario(ejercicio_1,
                                                                     contenido);
            if (q2.isActive())
               while (q2.next())
                {
                  // QMessageBox::warning( this, tr("Estados Contables"),q.value(0).toString());

                  double resultado=q2.value(1).toDouble()-q2.value(2).toDouble();
                  int numcod=q2.value(0).toInt();
                  int incremento=0;
                  if (numcod>1) incremento++;
                  if (numcod>3) incremento++; //if (numcod>4) numcod++;
                  if (numcod>6) incremento++; //if (numcod>7) numcod++;
                  ui.tabla->item(4+numcod-1+incremento,numero)->setText(formateanumero(resultado,comadecimal,decimales));
                }
           }
         QSqlQuery q2=basedatos::instancia()->select_cods_evpn_pymes_diario(ejercicio,
                                                                  contenido);
         if (q2.isActive())
            while (q2.next())
             {
               // QMessageBox::warning( this, tr("Estados Contables"),q.value(0).toString());

               double resultado=q2.value(1).toDouble()-q2.value(2).toDouble();
               int numcod=q2.value(0).toInt();
               int incremento=0;
               if (numcod>1) incremento++;
               if (numcod>3) incremento++; //if (numcod>4) numcod++;
               if (numcod>6) incremento++; //if (numcod>7) numcod++;
               ui.tabla->item(19+numcod-1+incremento,numero)->setText(formateanumero(resultado,comadecimal,decimales));
             }

        }
    calculo();
}

void patrimonio::calculo()
{
    for (int veces=1; veces<12; veces++)
      {
        // calculamos saldo ajustado inicio año-1
       double resultado=0;
       resultado+=!ui.tabla->item(0,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(0,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(1,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(1,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(2,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(2,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(3,veces)->setText(formateanumero(resultado,comadecimal,decimales));

       // calculamos fila 5: suma de dos partidas --> ingresos fiscales y otros ingresos y gastos
       resultado=0;
       resultado+=!ui.tabla->item(6,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(6,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(7,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(7,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(5,veces)->setText(formateanumero(resultado,comadecimal,decimales));


       // calculamos fila (año -1) ahora es la 8 = suma 9, 10 ,11
       resultado=0;
       resultado+=!ui.tabla->item(9,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(9,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(10,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(10,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(11,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(11,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(8,veces)->setText(formateanumero(resultado,comadecimal,decimales));

       // calculamos - línea otras variaciones del patrimonio neto 12= 13+14
       resultado=0;
       resultado+=!ui.tabla->item(13,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(13,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(14,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(14,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(12,veces)->setText(formateanumero(resultado,comadecimal,decimales));

       // calculamos  - saldo final ejercicio n-1 ahora es la 15 =
       resultado=0;
       resultado+=!ui.tabla->item(3,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(3,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(4,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(4,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(6,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(6,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(7,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(7,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(9,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(9,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(10,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(10,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(11,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(11,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(12,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(12,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(15,veces)->setText(formateanumero(resultado,comadecimal,decimales));

       // calculamos fila 18 -- saldo ajustado inicio ejercicio n
       resultado=0;
       resultado+=!ui.tabla->item(15,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(15,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(16,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(16,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(17,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(17,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(18,veces)->setText(formateanumero(resultado,comadecimal,decimales));

       // calculamos fila 20
       // calculamos fila 20: suma de dos partidas --> ingresos fiscales y otros ingresos y gastos
       resultado=0;
       resultado+=!ui.tabla->item(21,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(21,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(22,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(22,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(20,veces)->setText(formateanumero(resultado,comadecimal,decimales));

       // calculamos fila 23 - suma de las tres siguientes
       resultado=0;
       resultado+=!ui.tabla->item(24,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(24,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(25,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(25,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(26,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(26,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(23,veces)->setText(formateanumero(resultado,comadecimal,decimales));

       // calculamos fila 27 Otras variaciones = suma de 28 +29
       resultado=0;
       resultado+=!ui.tabla->item(28,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(28,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(29,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(29,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(27,veces)->setText(formateanumero(resultado,comadecimal,decimales));


       // calculamos línea saldos finales
       resultado=0;
       resultado+=!ui.tabla->item(18,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(18,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(19,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(19,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(21,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(21,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(22,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(22,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(24,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(24,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(25,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(25,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(26,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(26,veces)->text()).toDouble() : 0;
       resultado+=!ui.tabla->item(27,veces)->text().isEmpty() ?
                  convapunto(ui.tabla->item(27,veces)->text()).toDouble() : 0;
       if (resultado>0.0001 || resultado < -0.0001)
           ui.tabla->item(30,veces)->setText(formateanumero(resultado,comadecimal,decimales));

      }

    // calculamos totales de las filas

    for (int veces=0; veces<=30; veces++)
      {
        double resultado=0;
        for (int col=1; col<=11; col++)
           {
            resultado+=!ui.tabla->item(veces,col)->text().isEmpty() ?
                       convapunto(ui.tabla->item(veces,col)->text()).toDouble() : 0;

           }
        if (resultado>0.0001 || resultado < -0.0001)
            ui.tabla->item(veces,12)->setText(formateanumero(resultado,comadecimal,decimales));

      }


}
