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

#include <stdexcept>

#include "gui/connection.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

Connection::Connection(const QString& signal, const QString& method,
    Qt::ConnectionType type) :
  signal(signal),
  method(method),
  type(type) {
}

Connection::Connection(const Connection& src) :
  signal(src.signal),
  method(src.method),
  type(src.type) {
}

Connection::~Connection() {
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void Connection::setSignal(const QString& signal) {
  this->signal = signal;
}

const QString& Connection::getSignal() const {
  return signal;
}

void Connection::setMethod(const QString& method) {
  this->method = method;
}

const QString& Connection::getMethod() const {
  return method;
}

void Connection::setType(Qt::ConnectionType type) {
  this->type = type;
}

Qt::ConnectionType Connection::getType() const {
  return type;
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

Connection& Connection::operator=(const Connection& src) {
  signal = src.signal;
  method = src.method;

  type = src.type;

  return *this;
}

void Connection::establish(QObject* sender, QObject* receiver) {
  if (!receiver->connect(sender, signal.toAscii().constData(),
      method.toAscii().constData(), type))
    throw std::runtime_error("Connection failed");
}
