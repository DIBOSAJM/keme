#include "directorio.h"
#include "ui_directorio.h"

directorio::directorio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::directorio)
{
    ui->setupUi(this);
    // model.setRootPath(QDir::currentPath());
    // model.sort(0);
    // ui->treeView->setModel(&model);
    //setHorizontalHeaderLabels ( const QStringList & labels )

    //model.setHeaderData(0,Qt::Horizontal,tr("Nombre"));
    ui->treeView->setHeaderHidden(true);
    ui->archivolabel->hide();
    ui->nombrelineEdit->hide();
    solodirs=false;
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(fila_clicked()));
}

directorio::~directorio()
{
    delete ui;
}

void directorio::pasa_directorio(QString dir)
{
  model.setRootPath(QDir::rootPath());
  // model.sort(0);
  if (solodirs) model.setFilter(QDir::Dirs);
  raiz=dir.remove("..");
  ui->treeView->setModel(&model);
  ui->treeView->setRootIndex(model.index(raiz));
  ui->treeView->header()->hideSection(1);
  ui->treeView->header()->hideSection(2);
  ui->treeView->header()->hideSection(3);
  ui->treeView->setColumnWidth(0,200);

  ui->treeView->setCurrentIndex(model.index(0));

  setWindowTitle(dir.remove(".."));

  // ui->treeView->setSortingEnabled(true);
  // ui->treeView->sortByColumn(0);
}

bool directorio::selec_esdirectorio()
{
    if (ui->treeView->currentIndex().isValid())
       {
        return model.isDir(ui->treeView->currentIndex());
       }
    return false;

}

void directorio::activa_pide_archivo(QString nombre_defecto)
{
  ui->archivolabel->show();
  ui->nombrelineEdit->show();
  ui->nombrelineEdit->setText(nombre_defecto);
}

QString directorio::respuesta_arch()
{
  return ui->nombrelineEdit->text();
}

QString directorio::selec_actual()
{
 if (ui->treeView->currentIndex().isValid())
    {
     if (!model.isDir(ui->treeView->currentIndex()))
     return model.fileName(ui->treeView->currentIndex());
    }
 return QString();
}

void directorio::fila_clicked()
{
    if (ui->treeView->currentIndex().isValid())
       {
        ui->nombrelineEdit->setText(model.fileName(ui->treeView->currentIndex()));
       }

}


QString directorio::ruta_actual()
{
 if (ui->treeView->currentIndex().isValid())
    {
     return model.filePath(ui->treeView->currentIndex());
    }
 return QString();
}


QString directorio::directorio_actual()
{
    if (ui->treeView->currentIndex().isValid())
       {
        if (model.isDir(ui->treeView->currentIndex()))
            return ruta_actual().remove("..");
       }
   return raiz.remove("..");
}

QString directorio::nuevo_conruta()
{
    QString rutadir=directorio_actual();
    if (!rutadir.endsWith(QDir::separator())) rutadir.append(QDir::separator());
    if (!respuesta_arch().isEmpty())
      rutadir.append(respuesta_arch());
    else rutadir.clear();
    return rutadir;
}

void directorio::solo_directorios()
{
  solodirs=true;
}

void directorio::filtrar(QString filtro)
{
  QStringList filtros;
  filtros << filtro;
  model.setNameFilters(filtros);
  model.setNameFilterDisables(false);
}
