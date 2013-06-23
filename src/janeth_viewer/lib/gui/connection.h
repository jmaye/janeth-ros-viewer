/***************************************************************************
 *   Copyright (C) 2010 by Ralf Kaestner, Nikolas Engelhard, Yves Pilat    *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore/QObject>

class Connection {
public:
  Connection(const QString& signal = "", const QString& method = "",
    Qt::ConnectionType type = Qt::AutoConnection);
  Connection(const Connection& src);
  ~Connection();

  void setSignal(const QString& signal);
  const QString& getSignal() const;

  void setMethod(const QString& method);
  const QString& getMethod() const;

  void setType(Qt::ConnectionType type);
  Qt::ConnectionType getType() const;

  Connection& operator=(const Connection& src);

  void establish(QObject* sender, QObject* receiver);
protected:
  QString method;
  QString signal;

  Qt::ConnectionType type;
};

#endif