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


#ifndef QDBF_GLOBAL_H
#define QDBF_GLOBAL_H

#include <QtGlobal>

/*
#if defined(QDBF_LIBRARY)
#  define QDBF_EXPORT Q_DECL_EXPORT
#else
#  define QDBF_EXPORT Q_DECL_IMPORT
#endif
*/

#if defined(QDBF_LIBRARY)
#  define QDBF_EXPORT
#else
#  define QDBF_EXPORT
#endif

#endif // QDBF_GLOBAL_H
