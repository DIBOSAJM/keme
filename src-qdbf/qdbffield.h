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


#ifndef QDBFFIELD_H
#define QDBFFIELD_H

#include "qdbf_global.h"

#include <QVariant>

namespace QDbf {
namespace Internal {
class QDbfFieldPrivate;
class QDbfTablePrivate;
} // namespace Internal

class QDBF_EXPORT QDbfField
{
public:
    explicit QDbfField(const QString &fieldName = QLatin1String(""));
    QDbfField(const QDbfField &other);
    bool operator==(const QDbfField &other) const;
    bool operator!=(const QDbfField &other) const;
    QDbfField &operator=(const QDbfField &other);
    ~QDbfField();

    enum QDbfType
    {
        Undefined = -1,
        Character,
        Date,
        FloatingPoint,
        Logical,
        Memo,
        Number,
        Integer,
        DateTime
    };

    void setValue(const QVariant &value);
    inline QVariant value() const { return val; }

    void setName(const QString &name);
    QString name() const;

    bool isNull() const;

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    void clear();

    void setType(QDbfType type);
    QDbfType type() const;

    void setLength(int length);
    int length() const;

    void setPrecision(int precision);
    int precision() const;

    void setOffset(int offset);
    int offset() const;

    void setDefaultValue(const QVariant &value);
    QVariant defaultValue() const;

private:
    Internal::QDbfFieldPrivate *d;
    QVariant val;
    void detach();

    friend class Internal::QDbfTablePrivate;
};

} // namespace QDbf

QDebug operator<<(QDebug, const QDbf::QDbfField&);

#endif // QDBFFIELD_H
