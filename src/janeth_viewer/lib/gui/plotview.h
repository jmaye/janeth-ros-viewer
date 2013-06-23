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

#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <list>

#include <QtCore/QTextStream>
#include <QtCore/QProcess>

#include "gui/view.h"

#include "gui/pdfdisplay.h"
#include "gui/camera.h"
#include "gui/scene.h"
#include "gui/palette.h"

class Ui_PlotView;

class PlotView :
  public View {
Q_OBJECT
public:
  PlotView();
  ~PlotView();

  PDFDisplay& getDisplay();
  const PDFDisplay& getDisplay() const;

  void setFont(const QFont& font);
  void setFont(const QString& family, size_t size);
  const QFont& getFont() const;

  Size getSize() const;

  void setProjection(const Projection& projection);
  Projection getProjection() const;
  void setTransformation(const Transformation& transformation);
  Transformation getTransformation() const;

  void setTerminalSize(double width, double height);

  void saveTransformation();
  void restoreTransformation();

  void transform(const Transformation& transformation);
  using View::transform;
  void translate(const Translation& translation);
  using View::translate;
  void rotate(const Rotation& rotation);
  using View::rotate;
  void scale(const Scale& scale);
  using View::scale;

  void clear(const QColor& color);

  void enableFog(const QColor& color, double start, double end,
    double density);

  void render(const Points<double, 3>& vertices, const QColor& color,
    double size, bool smooth);
  void render(const Points<double, 3>& vertices, const std::vector<double>&
    weights, const QColor& fromColor, const QColor& toColor, double fromSize,
    double toSize, bool smooth);
  void render(const Line<double, 3>& edges, const QColor& color);
  void render(const Line<double, 3>& edges, double weight, const QColor&
    fromColor, const QColor& toColor);
  void render(const QString& text, const QColor& color);
  using View::render;

  void exportFrame(const QString& filename);
public slots:
  virtual void render();
protected:
  Ui_PlotView* ui;

  QFont font;
  QProcess plotProcess;
  QProcess conversionProcess;
  QString filename;

  std::list<Transformation> transformations;

  std::map<QString, std::vector<Points<double, 3> > > points;
  std::map<QString, std::vector<Points<double, 3> > > palettePoints;
  std::map<QString, std::vector<std::vector<double> > > palettePointWeights;
  std::map<QString, std::vector<Line<double, 3> > > lines;
  std::map<QString, std::vector<Line<double, 3> > > paletteLines;
  std::map<QString, std::vector<double> > paletteLineWeights;
  std::map<QString, std::vector<QString> > labels;

  Projection projection;
  Transformation transformation;

  bool project(Point& point) const;
  bool project(Line<double, 3>& line, std::vector<bool>& clipped) const;
  void interpolate(const Point& fixed, Point& variable, double ratio) const;

  void writePlotHeader(QTextStream& stream) const;
  void writeDataHeader(QTextStream& stream) const;

  void writePlot(QTextStream& stream, const QString& filename, const
    std::vector<QString>& styles, const QString& palette, const
    std::vector<QString>& labels, const std::vector<QString>& plots) const;
  void writeData(QTextStream& stream, const Point& point) const;
  void writeData(QTextStream& stream, const Point& point,
    double weight) const;
protected slots:
  void fontBrowseClicked();

  void plotProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void conversionProcessFinished(int exitCode, QProcess::ExitStatus
    exitStatus);

  void exportClicked();
signals:
  void fontChanged(const QFont& font);
};

#endif
