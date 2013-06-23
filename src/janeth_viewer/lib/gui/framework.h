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

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <vector>
#include <map>
#include <set>

#include <QtGui/QApplication>

#include "gui/mainwindow.h"
#include "gui/widget.h"

class Framework :
  public QApplication {
Q_OBJECT
friend class MainWindow;
friend class Widget;
public:
  Framework(int& argc, char** argv);
  ~Framework();

  static Framework& getInstance();

  const QString& getProjectName() const;
  size_t getProjectMajorVersion() const;
  size_t getProjectMinorVersion() const;
  size_t getProjectPatchVersion() const;
  const QString& getProjectRelease() const;
  const QString& getProjectBuildType() const;

  QString getProjectFullName() const;

  size_t getNumArguments() const;
  bool hasArgument() const;
  bool hasArgument(const QString& key) const;

  const QString& operator[](int i) const;
  const QString& operator[](const QString& key) const;

  MainWindow& getWindow();
  const MainWindow& getWindow() const;

  template <class C> C& getWidget();
  template <class C> const C& getWidget() const;
protected:
  static Framework* instance;

  QString projectName;
  size_t projectMajorVersion;
  size_t projectMinorVersion;
  size_t projectPatchVersion;
  QString projectRelease;
  QString projectBuildType;

  std::vector<QString> plainArguments;
  std::map<QString, QString> keyedArguments;

  MainWindow* window;
  std::set<Widget*> widgets;

  void registerWindow(MainWindow* window);
  void registerWidget(Widget* widget);
signals:
  void windowRegistered(MainWindow& window);
  void widgetRegistered(Widget& widget);
};

#include "gui/framework.tpp"

#endif
