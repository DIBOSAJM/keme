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

#include "usuario.h"
#include "basedatos.h"
#include "directorio.h"
#include <QFileDialog>
#include <QMessageBox>
#include "graf_recorte.h"

usuario::usuario() : QDialog() {
    ui.setupUi(this);


    // -------------------------------------------------------------
  connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(grabardatos()));
  connect(ui.fotopushButton,SIGNAL( clicked()),this,SLOT(importafoto()));

  connect(ui.borrafotopushButton,SIGNAL( clicked()),this,SLOT(borrafoto()));

  connect(ui.todopushButton,SIGNAL( clicked()),this,SLOT(chequeatodo()));
  connect(ui.nadapushButton,SIGNAL( clicked()),this,SLOT(chequeanada()));

}

void usuario::importafoto()
{
QString cadfich;

#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(adapta(dirtrabajobd()));
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

  //QStringList filtros;
  //filtros << tr("Archivos de gráficos (*.png *.jpg *.bmp)");
  dialogofich.setNameFilter(tr("Archivos de gráficos (*.png *.jpg *.jpeg *.bmp)"));
  dialogofich.setDirectory(getenv("HOME"));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE FICHERO GRÁFICO"));
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
           cadfich=fileNames.at(0);
          }
     }

#endif

if (cadfich.isEmpty()) return;

  QPixmap imagen(cadfich);
  // ----------------------------------------------------------------------
  graf_recorte *g = new graf_recorte(cadfich,ui.fotolabel->width(),ui.fotolabel->height());
  int resultado=g->exec();
  int altolabel=ui.fotolabel->height();
  int ancholabel=ui.fotolabel->width();
  if (resultado==QDialog::Accepted)
     {
      QPixmap ima=g->recorte();
      //ui.fotolabel->setPixmap(g->recorte());
      //int altopix=ima.height();
      //int anchopix=ima.width();
      QPixmap definitiva;
      definitiva=ima.scaledToHeight(altolabel);
      ui.fotolabel->setScaledContents(false);
      ui.fotolabel->setPixmap(definitiva);
     }
    else
     {
     // QMessageBox::information( this, tr("SUBCUENTAS"),tr("ACEPTADO"));
     // ----------------------------------------------------------------------
      altolabel=ui.fotolabel->height();
      ancholabel=ui.fotolabel->width();
      int altopix=imagen.height();
      int anchopix=imagen.width();
      QPixmap definitiva;
      if (altopix/altolabel>anchopix/ancholabel)
        definitiva=imagen.scaledToHeight(altolabel);
      else
        definitiva=imagen.scaledToWidth(ancholabel);
      ui.fotolabel->setScaledContents(false);
      ui.fotolabel->setPixmap(definitiva);
     }
  delete(g);

}

QString usuario::fototexto()
{
 if (ui.fotolabel->pixmap().isNull()) return "";
 QPixmap foto=ui.fotolabel->pixmap();
 QByteArray bytes;
 QBuffer buffer(&bytes);
 buffer.open(QIODevice::WriteOnly);
 foto.save(&buffer, "PNG"); // writes pixmap into foto in PNG format
 QByteArray cadhexa=bytes.toHex();
 QString cadena(cadhexa);

 return cadena;

}

void usuario::borrafoto()
{
  if (QMessageBox::question(
            this,
            tr("¿ Borrar imagen ?"),
            tr("¿ Desea borrar la imagen actual ?"),
            tr("&Sí"), tr("&No"),
         QString(), 0, 1 ) ==0 )
               {
                QPixmap foto;
                ui.fotolabel->setPixmap(foto);
               }
}


void usuario::chequeatodo()
{

    ui.checkBox1->setChecked(true);
    ui.checkBox2->setChecked(true);
    ui.checkBox3->setChecked(true);
    ui.checkBox4->setChecked(true);
    ui.checkBox5->setChecked(true);
    ui.checkBox6->setChecked(true);
    ui.checkBox7->setChecked(true);
    ui.checkBox8->setChecked(true);
    ui.checkBox9->setChecked(true);
    ui.checkBox10->setChecked(true);
    ui.checkBox11->setChecked(true);
    ui.checkBox12->setChecked(true);
    ui.checkBox13->setChecked(true);
    ui.checkBox14->setChecked(true);
    ui.checkBox15->setChecked(true);
    ui.checkBox16->setChecked(true);
    ui.checkBox17->setChecked(true);
    ui.checkBox18->setChecked(true);
    ui.checkBox19->setChecked(true);
    ui.checkBox20->setChecked(true);
    ui.checkBox21->setChecked(true);
    ui.checkBox22->setChecked(true);
    ui.checkBox23->setChecked(true);
    ui.checkBox24->setChecked(true);
    ui.checkBox25->setChecked(true);
    ui.checkBox26->setChecked(true);
    ui.checkBox27->setChecked(true);
    ui.checkBox28->setChecked(true);
    ui.checkBox29->setChecked(true);
    ui.checkBox30->setChecked(true);
    ui.checkBox31->setChecked(true);
    ui.checkBox32->setChecked(true);
    ui.checkBox33->setChecked(true);
    ui.checkBox34->setChecked(true);
    ui.checkBox35->setChecked(true);
    ui.checkBox36->setChecked(true);
    ui.checkBox37->setChecked(true);
    ui.checkBox38->setChecked(true);
    ui.checkBox39->setChecked(true);
    ui.checkBox40->setChecked(true);
    ui.checkBox41->setChecked(true);
    ui.checkBox42->setChecked(true);
    ui.checkBox43->setChecked(true);
    ui.checkBox44->setChecked(true);
    ui.checkBox45->setChecked(true);
    ui.checkBox46->setChecked(true);
    ui.checkBox47->setChecked(true);
    ui.checkBox48->setChecked(true);
    ui.checkBox49->setChecked(true);
    ui.checkBox50->setChecked(true);
    ui.checkBox51->setChecked(true);
    ui.checkBox52->setChecked(true);
    ui.checkBox53->setChecked(true);
    ui.checkBox54->setChecked(true);
    ui.checkBox55->setChecked(true);
    ui.checkBox56->setChecked(true);
    ui.checkBox57->setChecked(true);
    ui.checkBox58->setChecked(true);
    ui.checkBox59->setChecked(true);
    ui.checkBox60->setChecked(true);
    ui.checkBox61->setChecked(true);
    ui.checkBox62->setChecked(true);
    ui.checkBox63->setChecked(true);
    ui.checkBox64->setChecked(true);
    ui.checkBox65->setChecked(true);
    ui.checkBox66->setChecked(true);
    ui.checkBox67->setChecked(true);
    ui.checkBox68->setChecked(true);
    ui.checkBox69->setChecked(true);
    ui.checkBox70->setChecked(true);
    ui.checkBox71->setChecked(true);
    ui.checkBox72->setChecked(true);
    ui.checkBox73->setChecked(true);
  ui.checkBox74->setChecked(true);
  ui.checkBox75->setChecked(true);
  ui.checkBox76->setChecked(true);
  ui.checkBox77->setChecked(true);
  ui.checkBox78->setChecked(true);
  ui.checkBox79->setChecked(true);
  ui.checkBox80->setChecked(true);
  ui.checkBox81->setChecked(true);
  ui.checkBox82->setChecked(true);
  ui.checkBox83->setChecked(true);
  ui.checkBox84->setChecked(true);

}

void usuario::chequeanada()
{

    ui.checkBox1->setChecked(false);
    ui.checkBox2->setChecked(false);
    ui.checkBox3->setChecked(false);
    ui.checkBox4->setChecked(false);
    ui.checkBox5->setChecked(false);
    ui.checkBox6->setChecked(false);
    ui.checkBox7->setChecked(false);
    ui.checkBox8->setChecked(false);
    ui.checkBox9->setChecked(false);
    ui.checkBox10->setChecked(false);
    ui.checkBox11->setChecked(false);
    ui.checkBox12->setChecked(false);
    ui.checkBox13->setChecked(false);
    ui.checkBox14->setChecked(false);
    ui.checkBox15->setChecked(false);
    ui.checkBox16->setChecked(false);
    ui.checkBox17->setChecked(false);
    ui.checkBox18->setChecked(false);
    ui.checkBox19->setChecked(false);
    ui.checkBox20->setChecked(false);
    ui.checkBox21->setChecked(false);
    ui.checkBox22->setChecked(false);
    ui.checkBox23->setChecked(false);
    ui.checkBox24->setChecked(false);
    ui.checkBox25->setChecked(false);
    ui.checkBox26->setChecked(false);
    ui.checkBox27->setChecked(false);
    ui.checkBox28->setChecked(false);
    ui.checkBox29->setChecked(false);
    ui.checkBox30->setChecked(false);
    ui.checkBox31->setChecked(false);
    ui.checkBox32->setChecked(false);
    ui.checkBox33->setChecked(false);
    ui.checkBox34->setChecked(false);
    ui.checkBox35->setChecked(false);
    ui.checkBox36->setChecked(false);
    ui.checkBox37->setChecked(false);
    ui.checkBox38->setChecked(false);
    ui.checkBox39->setChecked(false);
    ui.checkBox40->setChecked(false);
    ui.checkBox41->setChecked(false);
    ui.checkBox42->setChecked(false);
    ui.checkBox43->setChecked(false);
    ui.checkBox44->setChecked(false);
    ui.checkBox45->setChecked(false);
    ui.checkBox46->setChecked(false);
    ui.checkBox47->setChecked(false);
    ui.checkBox48->setChecked(false);
    ui.checkBox49->setChecked(false);
    ui.checkBox50->setChecked(false);
    ui.checkBox51->setChecked(false);
    ui.checkBox52->setChecked(false);
    ui.checkBox53->setChecked(false);
    ui.checkBox54->setChecked(false);
    ui.checkBox55->setChecked(false);
    ui.checkBox56->setChecked(false);
    ui.checkBox57->setChecked(false);
    ui.checkBox58->setChecked(false);
    ui.checkBox59->setChecked(false);
    ui.checkBox60->setChecked(false);
    ui.checkBox61->setChecked(false);
    ui.checkBox62->setChecked(false);
    ui.checkBox63->setChecked(false);
    ui.checkBox64->setChecked(false);
    ui.checkBox65->setChecked(false);
    ui.checkBox66->setChecked(false);
    ui.checkBox67->setChecked(false);
    ui.checkBox68->setChecked(false);
    ui.checkBox69->setChecked(false);
    ui.checkBox70->setChecked(false);
    ui.checkBox71->setChecked(false);
    ui.checkBox72->setChecked(false);
    ui.checkBox73->setChecked(false);
    ui.checkBox74->setChecked(false);
    ui.checkBox75->setChecked(false);
    ui.checkBox76->setChecked(false);
    ui.checkBox77->setChecked(false);
    ui.checkBox78->setChecked(false);
    ui.checkBox79->setChecked(false);
    ui.checkBox80->setChecked(false);
    ui.checkBox81->setChecked(false);
    ui.checkBox82->setChecked(false);
    ui.checkBox83->setChecked(false);
    ui.checkBox84->setChecked(false);

}

void usuario::grabardatos()
{
    /*
             "codigo         varchar(40) default '',"
             "nombre         varchar(254) default '',"
             "clave          varchar(40) default '',"
             "nif            varchar(40) default '',"
             "domicilio      varchar(80) default '',"
             "poblacion      varchar(80) default '',"
             "codigopostal   varchar(40) default '',"
             "provincia      varchar(80) default '',"
             "pais           varchar(80) default '',"
             "tfno           varchar(80) default '',"
             "email          varchar(80) default '',"
             "observaciones  varchar(255) default '',"
             "privilegios    varchar(255) default '',"
             "imagen         text,"
     */
if (ui.clave1lineEdit->text()!=ui.clave2lineEdit->text())
    {
       QMessageBox::warning( this, tr("usuarios"),
         tr("ERROR: La verificación de clave no coincide"));
         return;
     }

QString privilegios;

privilegios=ui.checkBox1->isChecked() ? "1" : "0";
privilegios.append(ui.checkBox2->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox3->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox4->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox5->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox6->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox7->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox8->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox9->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox10->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox11->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox12->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox13->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox14->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox15->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox16->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox17->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox18->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox19->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox20->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox21->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox22->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox23->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox24->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox25->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox26->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox27->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox28->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox29->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox30->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox31->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox32->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox33->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox34->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox35->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox36->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox37->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox38->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox39->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox40->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox41->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox42->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox43->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox44->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox45->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox46->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox47->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox48->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox49->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox50->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox51->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox52->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox53->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox54->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox55->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox56->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox57->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox58->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox59->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox60->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox61->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox62->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox63->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox64->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox65->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox66->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox67->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox68->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox69->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox70->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox71->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox72->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox73->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox74->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox75->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox76->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox77->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox78->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox79->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox80->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox81->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox82->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox83->isChecked() ? '1' : '0');
privilegios.append(ui.checkBox84->isChecked() ? '1' : '0');


basedatos::instancia()->grabarusuario(ui.codigolineEdit->text(),
                                    ui.nombrelineEdit->text(),
                                    ui.clave1lineEdit->text(),
                                    ui.niflineEdit->text(),
                                    ui.domiciliolineEdit->text(),
                                    ui.poblacionlineEdit->text(),
                                    ui.cplineEdit->text(),
                                    ui.provincialineEdit->text(),
                                    ui.paislineEdit->text(),
                                    ui.tfnolineEdit->text(),
                                    ui.emaillineEdit->text(),
                                    ui.observacioneslineEdit->text(),
                                    privilegios);
// ahora grabamos la fotografía

basedatos::instancia()->grabaimagenusuario(ui.codigolineEdit->text(), fototexto());
accept();
}

void usuario::cargar(QString codigo)        
{

    ui.codigolineEdit->setText(codigo);
    QSqlQuery q=basedatos::instancia()->recuperarusuario(codigo);
    QString privilegios,imagen;
    if (q.isActive())
      if (q.next())
        {
          ui.nombrelineEdit->setText(q.value(0).toString());
          ui.clave1lineEdit->setText(q.value(1).toString());
          ui.clave2lineEdit->setText(q.value(1).toString());
          ui.niflineEdit->setText(q.value(2).toString());
          ui.domiciliolineEdit->setText(q.value(3).toString());
          ui.poblacionlineEdit->setText(q.value(4).toString());
          ui.cplineEdit->setText(q.value(5).toString());
          ui.provincialineEdit->setText(q.value(6).toString());
          ui.paislineEdit->setText(q.value(7).toString());
          ui.tfnolineEdit->setText(q.value(8).toString());
          ui.emaillineEdit->setText(q.value(9).toString());
          ui.observacioneslineEdit->setText(q.value(10).toString());
          privilegios=q.value(11).toString();
          imagen=q.value(12).toString();
          QPixmap foto;
          if (imagen.length()>0)
                {
                 QByteArray byteshexa;
                 byteshexa.append ( imagen.toUtf8() );
                 QByteArray bytes;
                 bytes=bytes.fromHex ( byteshexa );
                 foto.loadFromData ( bytes, "PNG");
                }
          ui.fotolabel->setPixmap(foto);
        }
    // ajustamos ahora los premisos
    //privilegios=ui.checkBox->isChecked() ? "1" : "0";
    //privilegios.append(ui.checkBox_2->isChecked() ? '1' : '0');
    ui.checkBox1->setChecked(privilegios[0]=='1' ? true : false);
    ui.checkBox2->setChecked(privilegios[1]=='1' ? true : false);
    ui.checkBox3->setChecked(privilegios[2]=='1' ? true : false);
    ui.checkBox4->setChecked(privilegios[3]=='1' ? true : false);
    ui.checkBox5->setChecked(privilegios[4]=='1' ? true : false);
    ui.checkBox6->setChecked(privilegios[5]=='1' ? true : false);
    ui.checkBox7->setChecked(privilegios[6]=='1' ? true : false);
    ui.checkBox8->setChecked(privilegios[7]=='1' ? true : false);
    ui.checkBox9->setChecked(privilegios[8]=='1' ? true : false);
    ui.checkBox10->setChecked(privilegios[9]=='1' ? true : false);
    ui.checkBox11->setChecked(privilegios[10]=='1' ? true : false);
    ui.checkBox12->setChecked(privilegios[11]=='1' ? true : false);
    ui.checkBox13->setChecked(privilegios[12]=='1' ? true : false);
    ui.checkBox14->setChecked(privilegios[13]=='1' ? true : false);
    ui.checkBox15->setChecked(privilegios[14]=='1' ? true : false);
    ui.checkBox16->setChecked(privilegios[15]=='1' ? true : false);
    ui.checkBox17->setChecked(privilegios[16]=='1' ? true : false);
    ui.checkBox18->setChecked(privilegios[17]=='1' ? true : false);
    ui.checkBox19->setChecked(privilegios[18]=='1' ? true : false);
    ui.checkBox20->setChecked(privilegios[19]=='1' ? true : false);
    ui.checkBox21->setChecked(privilegios[20]=='1' ? true : false);
    ui.checkBox22->setChecked(privilegios[21]=='1' ? true : false);
    ui.checkBox23->setChecked(privilegios[22]=='1' ? true : false);
    ui.checkBox24->setChecked(privilegios[23]=='1' ? true : false);
    ui.checkBox25->setChecked(privilegios[24]=='1' ? true : false);
    ui.checkBox26->setChecked(privilegios[25]=='1' ? true : false);
    ui.checkBox27->setChecked(privilegios[26]=='1' ? true : false);
    ui.checkBox28->setChecked(privilegios[27]=='1' ? true : false);
    ui.checkBox29->setChecked(privilegios[28]=='1' ? true : false);
    ui.checkBox30->setChecked(privilegios[29]=='1' ? true : false);
    ui.checkBox31->setChecked(privilegios[30]=='1' ? true : false);
    ui.checkBox32->setChecked(privilegios[31]=='1' ? true : false);
    ui.checkBox33->setChecked(privilegios[32]=='1' ? true : false);
    ui.checkBox34->setChecked(privilegios[33]=='1' ? true : false);
    ui.checkBox35->setChecked(privilegios[34]=='1' ? true : false);
    ui.checkBox36->setChecked(privilegios[35]=='1' ? true : false);
    ui.checkBox37->setChecked(privilegios[36]=='1' ? true : false);
    ui.checkBox38->setChecked(privilegios[37]=='1' ? true : false);
    ui.checkBox39->setChecked(privilegios[38]=='1' ? true : false);
    ui.checkBox40->setChecked(privilegios[39]=='1' ? true : false);
    ui.checkBox41->setChecked(privilegios[40]=='1' ? true : false);
    ui.checkBox42->setChecked(privilegios[41]=='1' ? true : false);
    ui.checkBox43->setChecked(privilegios[42]=='1' ? true : false);
    ui.checkBox44->setChecked(privilegios[43]=='1' ? true : false);
    ui.checkBox45->setChecked(privilegios[44]=='1' ? true : false);
    ui.checkBox46->setChecked(privilegios[45]=='1' ? true : false);
    ui.checkBox47->setChecked(privilegios[46]=='1' ? true : false);
    ui.checkBox48->setChecked(privilegios[47]=='1' ? true : false);
    ui.checkBox49->setChecked(privilegios[48]=='1' ? true : false);
    ui.checkBox50->setChecked(privilegios[49]=='1' ? true : false);
    ui.checkBox51->setChecked(privilegios[50]=='1' ? true : false);
    ui.checkBox52->setChecked(privilegios[51]=='1' ? true : false);
    ui.checkBox53->setChecked(privilegios[52]=='1' ? true : false);
    ui.checkBox54->setChecked(privilegios[53]=='1' ? true : false);
    ui.checkBox55->setChecked(privilegios[54]=='1' ? true : false);
    ui.checkBox56->setChecked(privilegios[55]=='1' ? true : false);
    ui.checkBox57->setChecked(privilegios[56]=='1' ? true : false);
    ui.checkBox58->setChecked(privilegios[57]=='1' ? true : false);
    ui.checkBox59->setChecked(privilegios[58]=='1' ? true : false);
    ui.checkBox60->setChecked(privilegios[59]=='1' ? true : false);
    ui.checkBox61->setChecked(privilegios[60]=='1' ? true : false);
    ui.checkBox62->setChecked(privilegios[61]=='1' ? true : false);
    ui.checkBox63->setChecked(privilegios[62]=='1' ? true : false);
    ui.checkBox64->setChecked(privilegios[63]=='1' ? true : false);
    ui.checkBox65->setChecked(privilegios[64]=='1' ? true : false);
    ui.checkBox66->setChecked(privilegios[65]=='1' ? true : false);
    ui.checkBox67->setChecked(privilegios[66]=='1' ? true : false);
    ui.checkBox68->setChecked(privilegios[67]=='1' ? true : false);
    ui.checkBox69->setChecked(privilegios[68]=='1' ? true : false);
    ui.checkBox70->setChecked(privilegios[69]=='1' ? true : false);
    ui.checkBox71->setChecked(privilegios[70]=='1' ? true : false);
    ui.checkBox72->setChecked(privilegios[71]=='1' ? true : false);
    ui.checkBox73->setChecked(privilegios[72]=='1' ? true : false);
    ui.checkBox74->setChecked(privilegios[73]=='1' ? true : false);
    ui.checkBox75->setChecked(privilegios[74]=='1' ? true : false);
    ui.checkBox76->setChecked(privilegios[75]=='1' ? true : false);
    ui.checkBox77->setChecked(privilegios[76]=='1' ? true : false);
    ui.checkBox78->setChecked(privilegios[77]=='1' ? true : false);
    ui.checkBox79->setChecked(privilegios[78]=='1' ? true : false);
    ui.checkBox80->setChecked(privilegios[79]=='1' ? true : false);
    ui.checkBox81->setChecked(privilegios[80]=='1' ? true : false);
    ui.checkBox82->setChecked(privilegios[81]=='1' ? true : false);
    ui.checkBox83->setChecked(privilegios[82]=='1' ? true : false);
    ui.checkBox84->setChecked(privilegios[83]=='1' ? true : false);

    if (codigo=="admin") ui.tabWidget->setTabEnabled(1,false);

}

