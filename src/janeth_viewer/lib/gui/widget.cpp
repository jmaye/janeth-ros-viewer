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

#include "gui/widget.h"

#include "gui/framework.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

Widget::Widget(QWidget* parent) :
  QWidget(parent) {
  connect(&Framework::getInstance(), SIGNAL(widgetRegistered(Widget&)), this,
    SLOT(frameworkWidgetRegistered(Widget&)));
}

Widget::~Widget() {
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void Widget::connect(const QString& className, const QString& signal, const
    QString& method, Qt::ConnectionType type) {
  Connection connection(signal, method, type);
  connections.insert(std::make_pair(className, connection));

  for (std::set<Widget*>::iterator it =
      Framework::getInstance().widgets.begin();
      it != Framework::getInstance().widgets.end(); ++it)
    if ((*it)->inherits(className.toAscii().constData()))
      connection.establish(*it, this);
}

void Widget::frameworkWidgetRegistered(Widget& widget) {
  for (std::multimap<QString, Connection>::iterator it = connections.begin();
      it != connections.end(); ++it)
    if (widget.inherits(it->first.toAscii().constData()))
      it->second.establish(&widget, this);
}
