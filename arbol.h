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

 #ifndef ARBOL_H
 #define ARBOL_H

 #include <QAbstractItemModel>
 #include <QModelIndex>
 #include <QVariant>

class TreeItem
 {
 public:
     TreeItem(const QList<QVariant> &data, TreeItem *parent = 0);
     ~TreeItem();

     void appendChild(TreeItem *child);

     TreeItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;
     QList<QVariant> lositemdata();
     void pasacad(int column,QString cadena);
     int row() const;
     TreeItem *parent();
     QList<TreeItem*> ItemsHijos();
     void eliminalistahijos();

 private:

     QList<TreeItem*> childItems;
     QList<QVariant> itemData;
     TreeItem *parentItem;
 };


 
 
 class TreeModel : public QAbstractItemModel
 {
     Q_OBJECT

 public:
     TreeModel(QObject *parent = 0);
     ~TreeModel();

     QVariant data(const QModelIndex &index, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
     QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;
     TreeItem *buscacadena(TreeItem *raiz,QString cad,bool *encontrada);
     TreeItem *raiz();
     QModelIndex creaindice(TreeItem *item);
     QList<QVariant> datosfiladeindice(QModelIndex indice);
     bool setData(TreeItem *elemento,
                  const QString cadena, int role);
     void borraarbol();
     void resetea();
     void actu_elemento(TreeItem *elemento);
 private:

     TreeItem *rootItem;
     QList<QVariant> rootData;
 };




 #endif
