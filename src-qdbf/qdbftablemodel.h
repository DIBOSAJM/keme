/***************************************************************************
**
** Copyright (C) 2018 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the QDbf - Qt DBF library.
**
** The QDbf is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** The QDbf is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with the QDbf.  If not, see <http://www.gnu.org/licenses/>.
**
***************************************************************************/


#ifndef QDBFTABLEMODEL_H
#define QDBFTABLEMODEL_H

//#include "qdbf_compat.h"
#include "qdbf_global.h"
#include "qdbftable.h"

#include <QAbstractTableModel>

namespace QDbf {
namespace Internal {
class QDbfTableModelPrivate;
} // namespace Internal

class QDBF_EXPORT QDbfTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit QDbfTableModel(QObject *parent = Q_NULLPTR);
    QDbfTableModel(const QString &filePath, QObject *parent = Q_NULLPTR);
    ~QDbfTableModel();

    bool open(const QString &filePath, bool readOnly = false);
    bool open(bool readOnly = false);
    void close();

    bool readOnly() const;
    QDbfTable::DbfTableError error() const;
    QDate lastUpdate() const;

    int rowCount(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::DisplayRole) Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    bool insertRows(int row, int count, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE;
    bool insertRow(int row, const QModelIndex &index = QModelIndex());

    bool removeRows(int row, int count, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRow(int row, const QModelIndex &index = QModelIndex());

    bool canFetchMore(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;
    void fetchMore(const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QDbfTableModel)
    Internal::QDbfTableModelPrivate *const d;

    friend class Internal::QDbfTableModelPrivate;
};

} // namespace QDbf

#endif // QDBFTABLEMODEL_H
