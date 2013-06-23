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

#ifndef WIDGET_H
#define WIDGET_H

#include <map>

#include <QtGui/QWidget>

#include "gui/connection.h"

class Widget :
  public QWidget {
Q_OBJECT
friend class MainWindow;
public:
  Widget(QWidget* parent = 0);
  ~Widget();

  void connect(const QString& className, const QString& signal, const
    QString& method, Qt::ConnectionType type = Qt::AutoConnection);
  template <class C> void connect(const QString& signal, const
    QString& method, Qt::ConnectionType type = Qt::AutoConnection);
  using QWidget::connect;
protected:
  std::multimap<QString, Connection> connections;
protected slots:
  void frameworkWidgetRegistered(Widget& widget);
};

#include "gui/widget.tpp"

#endif
