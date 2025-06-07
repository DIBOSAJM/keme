#include "check_herramientas.h"
#include "ui_check_herramientas.h"
#include <QMovie>
#include "funciones.h"

Check_herramientas::Check_herramientas(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Check_herramientas)
{
    ui->setupUi(this);
    QString cad=ui->editor_latex_t_label->text();
    QString ed=editor_latex();
    cad.append(" ");
    if (ed.isEmpty()) cad.append(tr("No existente en preferencias"));
    else cad.append(ed);
    ui->editor_latex_t_label->setText(cad);
}

Check_herramientas::~Check_herramientas()
{
    delete ui;
}

void Check_herramientas::evalua()
{
    QMovie *movie1 = new QMovie(":/iconos/graf/trabajando.gif");
    movie1->setScaledSize(ui->latex_label->size());
    ui->latex_label->setMovie(movie1);
    ui->editor_text_label->setMovie(movie1);
    ui->pdf_images_label->setMovie(movie1);
    ui->tesseract_label->setMovie(movie1);
    // ui->editor_text_label->setMovie(movie1);
    movie1->start();

    QEventLoop loop;
    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    QString cadexec="pdflatex";
    if (es_os_x()) cadexec="/Library/TeX/texbin/xelatex";
    QString cadarg="--help";

    QObject *parent=NULL;

    QStringList arguments;
    arguments << adapta(cadarg);

    QProcess *myProcess = new QProcess(parent);

    myProcess->start(cadexec,arguments);

    // system(cadexec);
    QString ruta;
    if (!myProcess->waitForStarted ())
    {
        ruta=":/iconos/graf/borrar.png";
    }
    else {
        ruta=":/iconos/graf/ok-256x256.png";
    }
    delete myProcess;
    QPixmap icono(ruta);
    QSize size(30, 30); // Tamaño deseado
    QPixmap scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->latex_label->setPixmap(scaledPixmap);

    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    cadexec=editor_latex();
    if (!cadexec.isEmpty()) {
       myProcess = new QProcess(parent);
       myProcess->start(cadexec,arguments);
       if (!myProcess->waitForStarted ())
       {
           ruta=":/iconos/graf/borrar.png";
       }
       else {
           ruta=":/iconos/graf/ok-256x256.png";
       }
    } else ruta=":/iconos/graf/borrar.png";
    icono.load(ruta);
    scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->editor_text_label->setPixmap(scaledPixmap);

    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    cadexec="pdfimages";
    if (es_os_x()) cadexec="/opt/homebrew/bin/pdfimages";
    myProcess = new QProcess(parent);
    myProcess->start(cadexec,arguments);
    if (!myProcess->waitForStarted ())
        ruta=":/iconos/graf/borrar.png";
    else
        ruta=":/iconos/graf/ok-256x256.png";
    icono.load(ruta);
    scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->pdf_images_label->setPixmap(scaledPixmap);

    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    cadexec="tesseract";
    if (es_os_x()) cadexec="/opt/homebrew/bin/tesseract";
    myProcess = new QProcess(parent);
    myProcess->start(cadexec,arguments);
    if (!myProcess->waitForStarted ())
        ruta=":/iconos/graf/borrar.png";
    else
        ruta=":/iconos/graf/ok-256x256.png";
    icono.load(ruta);
    scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->tesseract_label->setPixmap(scaledPixmap);


}

void Check_herramientas::on_comprobar_pushButton_clicked()
{
    evalua();
}

