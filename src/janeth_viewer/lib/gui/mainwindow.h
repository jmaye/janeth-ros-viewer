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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>

#include <QtGui/QMainWindow>

#include "gui/view.h"
#include "gui/control.h"

class Ui_MainWindow;

class MainWindow :
  public QMainWindow {
Q_OBJECT
public:
  MainWindow(const QString& subtitle = "");
  ~MainWindow();

  void setSubtitle(const QString& subtitle);
  const QString& getSubtitle() const;

  template <class V> V& getView();
  template <class V> const V& getView() const;
  template <class C> C& getControl();
  template <class C> const C& getControl() const;

  template <class V, typename... P> void addView(const QString& title,
    const P&... parameters);
  void addView(const QString& title, View& view);
  template <class C, typename... P> void addControl(const QString& title,
    const P&... parameters);
  void addControl(const QString& title, Control& control);
protected:
  Ui_MainWindow* ui;

  QString subtitle;

  QActionGroup viewActions;
  QActionGroup controlActions;

  std::map<QString, View*> views;
  std::map<QString, Control*> controls;
protected slots:
  void viewChanged(int index);
  void controlChanged(int index);

  void viewTriggered(QAction* action);
  void controlTriggered(QAction* action);
signals:
  void viewAdded(View& view);
  void controlAdded(Control& control);
};

#include "mainwindow.tpp"

#endif
