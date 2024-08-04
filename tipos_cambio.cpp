#include "tipos_cambio.h"
#include "ui_tipos_cambio.h"
#include "pidenombre.h"
#include <qmessagebox.h>
#include "funciones.h"
#include "basedatos.h"
#include "network_connections.h"

tipos_cambio::tipos_cambio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tipos_cambio)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0,350);
    comadecimal=haycomadecimal();

    QString moneda_base=basedatos::instancia()->config_moneda();
    ui->referencia_lineEdit->setText(moneda_base);
    ui->dateEdit->setDate(basedatos::instancia()->fecha_actu_cambios());

    QString codigos_moneda=
        "ADP;Peseta Andorrana*"
        "AED;Dirham de los Emiratos Árabes Unidos*"
        "AFA;Afghani*"
        "ALL;Lek Albanés*"
        "AMD;Dram Armenio*"
        "ANG;Guilder de India Occidental*"
        "AOK;Kwanza Angolés*"
        "ARA;Austral Argentino*"
        "ARS;Peso Argentino*"
        "ATS;Chelín Austriaco*"
        "AUD;Dólar Australiano*"
        "AWG;Guilder de Aruba*"
        "AZM;Manat de Azerbaijan*"
        "BAD;Dinar de Bosnia-Herzogovinia*"
        "BBD;Dólar de Barbados*"
        "BDT;Taka de Bangladesh*"
        "BEF;Franco Belga*"
        "BGL;Lev Bulgaro*"
        "BHD;Dinar de Barein*"
        "BIF;Franco de Burundi*"
        "BMD;Dólar de Bermudas*"
        "BND;Dólar de Brunei*"
        "BOB;Boliviano de Bolivia*"
        "BRL;Real de Brazil*"
        "BRR;Brazil*"
        "BSD;Dólar de Bahamas*"
        "BWP;Pula de Botswana*"
        "BYR;Rublo de Belorusia*"
        "BZD;Dólar de Belize*"
        "CAD;Dólar Canadiense*"
        "CDP;Santo Domingo*"
        "CHF;Franco Suizo*"
        "CLP;Peso Chileno*"
        "CNY;China*"
        "COP;Peso de Colombia*"
        "CRC;Colon de Costa Rica*"
        "CUP;Peso Cubano*"
        "CVE;Escudo de Cabo Verde*"
        "CYP;Libra de Chipre*"
        "CZK;Krona Checa*"
        "DEM;Marco Alemán*"
        "DJF;Franco de Djibouti*"
        "DKK;Corona Danesa*"
        "DOP;Peso Dominicano*"
        "DRP;Peso de la República Dominicana*"
        "DZD;Dinar de Algeria*"
        "ECS;Sucre de Ecuador*"
        "ECU;Moenda Única Europea (EURO)*"
        "EEK;Corona de Estonia*"
        "EGP;Libra Egipcia*"
        "ESP;Peseta Española*"
        "ETB;Birr de Etiopía*"
        "EUR;Moneda de los estados miembros de UE*"
        "FIM;Marco Finlandés*"
        "FJD;Dólar de Fiji*"
        "FKP;Libra de Falkland*"
        "FRF;Franco Francés*"
        "GBP;Libra Británica*"
        "GEK;Kupon de Georgia*"
        "GHC;Cedi de Ghana*"
        "GIP;Libra de Gibraltar*"
        "GMD;Dalasi de Gambia*"
        "GNF;Franco de Guinea*"
        "GRD;Dracma Griego*"
        "GTQ;Quedzal de Guatemala*"
        "GWP;Peso de Guinea*"
        "GYD;Dólar Guayanés*"
        "HKD;Dólar de Hong Kong*"
        "HNL;Lempira de Honduras*"
        "HRD;Dinar de Croacia*"
        "HTG;Gourde de Haiti*"
        "HUF;Florín Húngaro*"
        "IDR;Rupia Indonesia*"
        "IEP;Libra Irlandesa*"
        "ILS;Scheckel Israelí*"
        "INR;Rupia India*"
        "IQD;Dinar Iraquí*"
        "IRR;Rial Iraní*"
        "ISK;Corona Islandesa*"
        "ITL;Lira Italiana*"
        "JMD;Dólar de Jamaica*"
        "JOD;Dinar de Jordania*"
        "JPY;Yen Japonés*"
        "KES;Chelín de Kenya*"
        "KHR;Riel de Camboya*"
        "KIS;Som de Kirghizstan*"
        "KMF;Franco de Comoros*"
        "KPW;Won de Corea del Norte*"
        "KRW;Won de Corea del Sur*"
        "KWD;Dinar de Kuwait*"
        "KYD;Dólar de las Caimán*"
        "KZT;Tenge de Kazakhstán*"
        "LAK;Kip de Letonia*"
        "LBP;Libra Libanesa*"
        "LKR;Rupia de Ceilán*"
        "LRD;Dólar de Liberia*"
        "LSL;Loti de Lesotho*"
        "LTL;Lita de Lituania*"
        "LUF;Franco de Luxemburgo*"
        "LVL;Lat Latonia*"
        "LYD;Dinar Libio*"
        "MAD;Dirham Marroquí*"
        "MDL;Lei de Moldavia*"
        "MGF;Franco de Madagascar*"
        "MNC;Monaco*"
        "MNT;Tugrik de Mongolia*"
        "MOP;Pataca de Macao*"
        "MRO;Ouguiya de Mauritania*"
        "MTL;Lira de Malta*"
        "MUR;Rupia de ""Mauritius""*"
        "MVR;Rufia de Maldivas*"
        "MWK;Kwacha de Malawi*"
        "MXN;Peso Mexicano (nuevo)*"
        "MXP;Peso Mexicano (viejo)*"
        "MYR;Ringgit de Malaysia*"
        "MZM;Metical de Mozambique*"
        "NGN;Naira de Nigeria*"
        "NIC;Nicaragua*"
        "NIO;Cordoba de Nicaragua*"
        "NIS;Isreal*"
        "NLG;Guilder de Holanda*"
        "NOK;Córona Noruega*"
        "NPR;Rupia de Nepal*"
        "NZD;Dólar de Nueva Zelanda*"
        "OMR;Rial de Omán*"
        "PAB;Balboa de Panamá*"
        "PEI;Inti de Perú*"
        "PEN;Sol de Perú - Nuevo*"
        "PES;Sol de Perú*"
        "PGK;Kina de Papua Nueva Guinea*"
        "PHP;Peso Filipino*"
        "PKR;Rupia de Pakistán*"
        "PLN;Zloty de Polonia*"
        "PLZ;Polonia*"
        "PTE;Escudo Portugués*"
        "PYG;Guarani de Paraguay*"
        "QAR;Riyal de Qatar*"
        "RMB;Remnminbi Yuan de China*"
        "ROL;Lei de Rumania*"
        "RUR;Rublo Ruso*"
        "RWF;Franco de Rwanda*"
        "SAR;Riyal de Arabia Saudí*"
        "SBD;Dólar de las Islas Solomon*"
        "SCR;Rupias de Seychelles*"
        "SDP;Libra de Sudán*"
        "SEK;Córona Sueca*"
        "SGD;Dólar de Singapur*"
        "SHP;Libra de St.Helena*"
        "SIT;Tolar de Eslovenia*"
        "SKK;Córona Eslovaca*"
        "SLL;Sierra León*"
        "SOL;Perú*"
        "SOS;Chellín de Somalia*"
        "SRG;Guilder de Surinam*"
        "STD;Sao Tome / Principe Dobra*"
        "SUR;Rublo Ruso (viejo)*"
        "SVC;Colon de El Salvador*"
        "SYP;libra de Siria*"
        "SZL;Lilangeni de Swaziland*"
        "THB;Baht de Tailandia*"
        "TJR;Rublo de Tadzikistán*"
        "TMM;Manat de Turkmenistán*"
        "TND;Dinar de Tunícia*"
        "TOP;Pa'anga de Tonga*"
        "TPE;Escudo de Timor*"
        "TRL;Lira de Turquía*"
        "TTD;Dólar de Trinidad y Tobagor*"
        "TWD;Dólar de Nuevo Taiwán*"
        "TZS;Chellín de Tanzania*"
        "UAK;karbowanez de Ucrainia*"
        "UGS;Chellín de Uganda*"
        "USD;Dólar Americano*"
        "UYP;Nuevo Peso de Uruguay*"
        "UYU;Uruguay*"
        "VEB;Bolivar de Venezuela*"
        "VND;Dong de Vietnám*"
        "VUV;Vatu de Vanuatu*"
        "WST;Tala de Samoa*"
        "XAF;Franco de Gabón*"
        "XCD;Dólar de Caribe Oriental*"
        "XOF;Franco de Benin (C.f.A.)*"
        "YER;Ryal de Yemen*"
        "ZAR;Rand de Sur África*"
        "ZMK;Kwacha de Zambia*"
        "ZRZ;Zaire*"
        "ZWD;Dólar de Zimbawe*";
    QStringList moneda_paises=codigos_moneda.split("*");
    for (int i=0; i<moneda_paises.count(); i++) {
        cod_moneda << moneda_paises.at(i).left(3);
        descrip_moneda << moneda_paises.at(i).mid(4);
    }

    // Cargamos contenido de tabla en base de datos
    int lugar=0;
    QSqlQuery q = basedatos::instancia()->select_registros_monedas();
    if (q.isActive())
        while (q.next()) {
            ui->tableWidget->insertRow(lugar);
            ui->tableWidget->setVerticalHeaderItem(lugar,new QTableWidgetItem(q.value(0).toString()));
            ui->tableWidget->setItem(lugar,0,new QTableWidgetItem(q.value(1).toString()));
            QString cadnum; cadnum.setNum(q.value(2).toDouble(),'f',6);
            ui->tableWidget->setItem(lugar,1,new QTableWidgetItem(cadnum));
            lugar++;
        }

    cambios=false;
}

tipos_cambio::~tipos_cambio()
{
    delete ui;

}


void tipos_cambio::on_nuevo_pushButton_clicked()
{
    cambios=true;
    pidenombre *p = new pidenombre;
    p->asignaetiqueta(tr("Introduzca código de divisa"));
    int result=p->exec();
    QString codigo=p->contenido();
    delete(p);
    if (result!=QDialog::Accepted) return;
    bool posicion_encontrada=false;
    int lugar=0;
    for (int i=0; i< ui->tableWidget->rowCount(); i++) {
        if (ui->tableWidget->verticalHeaderItem(i)->text()==codigo) return;
        if (ui->tableWidget->verticalHeaderItem(i)->text()>codigo) {
            posicion_encontrada=true;
            lugar=i;
            break;
        }
    }
    if (!posicion_encontrada) lugar=ui->tableWidget->rowCount();
    //if (lugar<(ui->tableWidget->rowCount()-1)) lugar++;
    ui->tableWidget->insertRow(lugar);
    ui->tableWidget->setVerticalHeaderItem(lugar,new QTableWidgetItem(codigo));
    int pos_cod=cod_moneda.indexOf(codigo);
    ui->tableWidget->setItem(lugar,0,new QTableWidgetItem(descrip_moneda.at(pos_cod)));
    ui->tableWidget->setItem(lugar,1,new QTableWidgetItem(""));

}

void tipos_cambio::on_borrar_pushButton_clicked()
{
    cambios=true;
    if (ui->tableWidget->currentIndex().isValid()) {
        int i=ui->tableWidget->currentRow();
        if (QMessageBox::question(
               this,
               tr("TIPOS DE CAMBIO"),
                tr("¿ Desea borrar '%1' ?").arg(ui->tableWidget->verticalHeaderItem(i)->text())) == QMessageBox::No )
                             return;
        ui->tableWidget->removeRow(i);
    }
}

void tipos_cambio::on_tableWidget_cellChanged(int row, int column)
{
    cambios=true;
    if (column==1) {
        if (ui->tableWidget->item(row,column)==NULL) return;
        QString contenido=ui->tableWidget->item(row,column)->text();
        if (comadecimal) ui->tableWidget->item(row,column)->setText(convacoma(contenido));
          else ui->tableWidget->item(row,column)->setText(convapunto(contenido));
        if (convapunto(contenido).toDouble()<0.0000001) ui->tableWidget->item(row,column)->setText(QString());

    }
}

void tipos_cambio::on_cancelar_pushButton_clicked()
{
    if (cambios) {
        if (QMessageBox::question(
               this,
               tr("TIPOS DE CAMBIO"),
                tr("HAY CAMBIOS ¿ Desea cancelar y salir ?")) == QMessageBox::No )
                             return;
    }
    reject();

}

void tipos_cambio::on_actualizar_pushButton_clicked()
{
    currency_exchange c;
    qDebug() << tr("procedimiento actualizar");
    c.restrictions(QDate::currentDate(),ui->referencia_lineEdit->text());
    c.load_data();
    if (c.data_loaded()) {
        if (c.conn_error()) {
            QMessageBox::warning(this,tr("ENTRY"),tr("CONNECTION ERROR:")+QString().setNum(c.conn_error_number()) + " "+c.conn_error_string());
            return;
        }
        QJsonObject jsresponse=c.values();
        QJsonObject rates=jsresponse["rates"].toObject();

        if (ui->tableWidget->rowCount()==0) {
                int lugar=0;
                for (int i=0; i<cod_moneda.count();i++) {
                   //if (rate.contains(cod_moneda.at(i))) {
                    if (rates.value(cod_moneda.at(i))!=QJsonValue::Undefined) {
                     ui->tableWidget->insertRow(lugar);
                     ui->tableWidget->setVerticalHeaderItem(lugar,new QTableWidgetItem(cod_moneda.at(i)));
                     ui->tableWidget->setItem(lugar,0,new QTableWidgetItem(descrip_moneda.at(i)));
                     QString cadnum; cadnum.setNum(rates[cod_moneda.at(i)].toDouble(),'f',6);
                     ui->tableWidget->setItem(lugar,1,new QTableWidgetItem(cadnum));
                    lugar++;
                }
          }
        } else {
            // actualizamos cada línea de la tabla
            for (int i=0; i< ui->tableWidget->rowCount(); i++) {
                if (rates.value(ui->tableWidget->verticalHeaderItem(i)->text())!=QJsonValue::Undefined) {
                    QString cadnum; cadnum.setNum(rates[ui->tableWidget->verticalHeaderItem(i)->text()].toDouble(),'f',6);
                    ui->tableWidget->item(i,1)->setText(cadnum);
                }
            }
        }
        //QJsonDocument doc(jsresponse);
        //qDebug() << doc.toJson();
        ui->dateEdit->setDate(QDate::currentDate());
    } // else qDebug() << tr("NO SE HA CARGADO NADA");
}


void tipos_cambio::on_mueve_arriba_pushButton_clicked()
{
    if (ui->tableWidget->currentRow()<0 || ui->tableWidget->currentRow()>(ui->tableWidget->rowCount()-1) ) return;

    if (ui->tableWidget->currentRow()==0) return;

    ui->tableWidget->disconnect(SIGNAL(cellChanged(int,int)));
    // guardamos contenidos de la fila superior

    QString cols[ui->tableWidget->columnCount()];
    int pos=ui->tableWidget->currentRow();
    if (ui->tableWidget->item(pos,0)==0) return;


    for (int veces=0;veces<ui->tableWidget->columnCount();veces++)
    {
        cols[veces]=ui->tableWidget->item(pos-1,veces)->text();
    }
    QString verticalHeader=ui->tableWidget->verticalHeaderItem(pos-1)->text();

    // asignamos valores a la línea previa
    for (int veces=0;veces<ui->tableWidget->columnCount();veces++)
        ui->tableWidget->item(pos-1,veces)->setText(
            ui->tableWidget->item(pos,veces)->text());
    ui->tableWidget->verticalHeaderItem(pos-1)->setText(ui->tableWidget->verticalHeaderItem(pos)->text());

    // asignamos valores a la línea siguiente
    for (int veces=0;veces<ui->tableWidget->columnCount();veces++)
        ui->tableWidget->item(pos,veces)->setText(cols[veces]);
    ui->tableWidget->verticalHeaderItem(pos)->setText(verticalHeader);


    connect(ui->tableWidget,SIGNAL( cellChanged ( int , int )),this,
            SLOT(on_tableWidget_cellChanged(int, int)));

    int fila=pos-1;
    ui->tableWidget->setCurrentCell(fila,0);

}


void tipos_cambio::on_mueve_abajo_pushButton_clicked()
{
    if (ui->tableWidget->currentRow()<0 || ui->tableWidget->currentRow()>=(ui->tableWidget->rowCount()-1) ) return;

    if (ui->tableWidget->item(ui->tableWidget->currentRow(),0)==0) return;

    // guardamos contenidos de la fila inferior
    QString cols[ui->tableWidget->columnCount()];

    for (int veces=0;veces<ui->tableWidget->columnCount();veces++)
        cols[veces]=ui->tableWidget->item(ui->tableWidget->currentRow()+1,veces)->text();
    QString verticalHeader=ui->tableWidget->verticalHeaderItem(ui->tableWidget->currentRow()+1)->text();

    ui->tableWidget->disconnect(SIGNAL(cellChanged(int,int)));

    // asignamos valores a la fila inferior
    for (int veces=0;veces<ui->tableWidget->columnCount();veces++)
        ui->tableWidget->item(ui->tableWidget->currentRow()+1,veces)->setText(
            ui->tableWidget->item(ui->tableWidget->currentRow(),veces)->text());
    ui->tableWidget->verticalHeaderItem(ui->tableWidget->currentRow()+1)->setText(
        ui->tableWidget->verticalHeaderItem(ui->tableWidget->currentRow())->text());


    // asignamos valores (guardados) a la fila en curso
    for (int veces=0;veces<ui->tableWidget->columnCount();veces++)
        ui->tableWidget->item(ui->tableWidget->currentRow(),veces)->setText(cols[veces]);
    ui->tableWidget->verticalHeaderItem(ui->tableWidget->currentRow())->setText(verticalHeader);

    connect(ui->tableWidget,SIGNAL( cellChanged ( int , int )),this,
            SLOT(on_tableWidget_cellChanged(int, int)));

    int fila=ui->tableWidget->currentRow()+1;
    ui->tableWidget->setCurrentCell(fila,0);

}


void tipos_cambio::on_aceptar_pushButton_clicked()
{
    basedatos::instancia()->borra_registros_monedas();
    for (int veces=0;veces<ui->tableWidget->rowCount();veces++) {
        basedatos::instancia()->insert_registro_moneda(veces+1,ui->tableWidget->verticalHeaderItem(veces)->text(),
                                                       ui->tableWidget->item(veces,0)->text(),convapunto(ui->tableWidget->item(veces,1)->text()).toDouble());
    }
    basedatos::instancia()->update_fecha_actu_cambios(ui->dateEdit->date());
    accept();
}

