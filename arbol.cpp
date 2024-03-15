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


 #include <QStringList>
 #include <QtGui>

 #include "arbol.h"

 TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
 {
     parentItem = parent;
     itemData = data;
 }

 TreeItem::~TreeItem()
 {
     qDeleteAll(childItems);
 }

 void TreeItem::appendChild(TreeItem *item)
 {
     childItems.append(item);
 }

 TreeItem *TreeItem::child(int row)
 {
     return childItems.value(row);
 }

 int TreeItem::childCount() const
 {
     return childItems.count();
 }

 int TreeItem::columnCount() const
 {
     return itemData.count();
 }

 QVariant TreeItem::data(int column) const
 {
     return itemData.value(column);
 }

QList<QVariant> TreeItem::lositemdata()
{
 return itemData;
}

void TreeItem::pasacad(int column,QString cadena)
{
  itemData.replace(column,cadena);
 
}


 TreeItem *TreeItem::parent()
 {
     return parentItem;
 }

 int TreeItem::row() const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

     return 0;
 }


QList<TreeItem*> TreeItem::ItemsHijos()
{
 return childItems;
}

void TreeItem::eliminalistahijos()
{
 childItems.clear();
}


 TreeModel::TreeModel(QObject *parent)
     : QAbstractItemModel(parent)
 {    
     rootData << tr("CÓDIGO") << tr("DESCRIPCIÓN");
     rootItem = new TreeItem(rootData);
     // setupModelData(data.split(QString("\n")), rootItem);
 }

 TreeModel::~TreeModel()
 {
     delete rootItem;
 }

 int TreeModel::columnCount(const QModelIndex &parent) const
 {
     if (parent.isValid())
         return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
     else
         return rootItem->columnCount();
 }

 QVariant TreeModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

     return item->data(index.column());
 }

 Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
 {

     if (!index.isValid())
         return Qt::ItemIsEnabled;

     // return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
 }

 QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return rootItem->data(section);

     return QVariant();
 }

 QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
             const
 {
     TreeItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<TreeItem*>(parent.internalPointer());

     TreeItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex TreeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
     TreeItem *parentItem = childItem->parent();

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }

QModelIndex TreeModel::creaindice(TreeItem *item)
{
 return createIndex(item->row(),0,item);
}



 int TreeModel::rowCount(const QModelIndex &parent) const
 {
     TreeItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<TreeItem*>(parent.internalPointer());

     return parentItem->childCount();
 }

 
TreeItem *TreeModel::buscacadena(TreeItem *raiz, QString cad,bool *encontrada)
{
 *encontrada=false;

 if (raiz->childCount()==0) return NULL;
 for (int veces=0;veces<raiz->childCount();veces++)
      {
        if (raiz->ItemsHijos().at(veces)->data(0).toString()==cad)
            {
              *encontrada=true;
              return raiz->ItemsHijos().at(veces);
              // return raiz->child(veces);
            }
        else 
            {
             TreeItem *it;
             it=buscacadena(raiz->child(veces),cad,encontrada);
             if (*encontrada) return it;
            }
      }
  return NULL;
}


TreeItem *TreeModel::raiz()
{
 return rootItem;
}


QList<QVariant> TreeModel::datosfiladeindice(QModelIndex indice)
{
   TreeItem *item = static_cast<TreeItem*>(indice.internalPointer());

   return item->lositemdata();
 
}

void TreeModel::actu_elemento(TreeItem *elemento)
{
  QModelIndex index2=createIndex(elemento->row(),1,elemento);
  emit dataChanged(index2, index2);
}

 bool TreeModel::setData(TreeItem *elemento,
                               const QString cadena, int role)
 {
     elemento->pasacad(1,cadena);
     // elemento->lositemdata().replace(1,cadena);
     QModelIndex index2=createIndex(elemento->row(),1,elemento);
     if (index2.isValid() && role == Qt::EditRole) 
        {
         emit dataChanged(index2, index2);
         return true;
        }
     return false;
 }

void TreeModel::borraarbol()
{
 for (int veces=0;veces<rootItem->childCount();veces++)
      {
        delete rootItem->child(veces);
      }
 rootItem->eliminalistahijos();
 delete rootItem;
 rootItem = new TreeItem(rootData);
}

void TreeModel::resetea()
{
  resetInternalData();

}
