#include "mainwindow.h"
#include <QApplication>
#include "funciones.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int idioma=idiomaconfig();

    QTranslator translator;

    if (idioma>0 && idioma<6)
       {
        // if (idioma!=3) QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::Spain));
        if (idioma==1) translator.load(fich_catalan());
        if (idioma==2) translator.load(fich_gallego());
        if (idioma==3) translator.load(fich_ingles());
        if (idioma==4) translator.load(fich_portugues());
        if (idioma==5) translator.load(fich_euskera());
        a.installTranslator(&translator);
       }


    MainWindow w;
    w.show();

    return a.exec();
}
