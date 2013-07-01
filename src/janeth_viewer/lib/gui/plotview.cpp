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

#include <QtGui/QFileDialog>
#include <QtGui/QFontDialog>

#include "gui/plotview.h"

#include "gui/framework.h"

#include "ui_plotview.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

PlotView::PlotView() :
  ui(new Ui_PlotView()),
  projection(Projection::Identity()),
  transformation(Transformation::Identity()) {
  ui->setupUi(this);

  menu.addAction("Set Font...", this, SLOT(fontBrowseClicked()));

  connect(&plotProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
    this, SLOT(plotProcessFinished(int, QProcess::ExitStatus)));
  connect(&conversionProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
    this, SLOT(conversionProcessFinished(int, QProcess::ExitStatus)));

  setFont("Arial", 10);
  setTerminalSize(20.0, 14.0);
}

PlotView::~PlotView() {
  delete ui;
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

PDFDisplay& PlotView::getDisplay() {
  return *ui->display;
}

const PDFDisplay& PlotView::getDisplay() const {
  return *ui->display;
}

void PlotView::setFont(const QFont& font) {
  this->font = font;
  emit fontChanged(this->font);
}

void PlotView::setFont(const QString& family, size_t size) {
  QFont font(family, size);
  setFont(font);
}

const QFont& PlotView::getFont() const {
  return font;
}

PlotView::Size PlotView::getSize() const {
  return Size(ui->terminalWidthSpinBox->value(),
    ui->terminalHeightSpinBox->value());
}

void PlotView::setProjection(const Projection& projection) {
  this->projection = projection;
}

PlotView::Projection PlotView::getProjection() const {
  return projection;
}

void PlotView::setTransformation(const Transformation& transformation) {
  this->transformation = transformation;
}

PlotView::Transformation PlotView::getTransformation() const {
  return transformation;
}

void PlotView::setTerminalSize(double width, double height) {
  ui->terminalWidthSpinBox->setValue(width);
  ui->terminalHeightSpinBox->setValue(height);
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void PlotView::saveTransformation() {
  transformations.push_back(transformation);
}

void PlotView::restoreTransformation() {
  if (!transformations.empty()) {
    transformation = transformations.back();
    transformations.pop_back();
  }
  else
    throw std::runtime_error("Empty transformation stack");
}

void PlotView::transform(const Transformation& transformation) {
  this->transformation = this->transformation*transformation;
}

void PlotView::translate(const Translation& translation) {
  Transformation transformation;
  transformation = Eigen::Translation<double, 3>(translation);

  transform(transformation);
}

void PlotView::rotate(const Rotation& rotation) {
  Eigen::AngleAxis<double> R_yaw(rotation[0],
    Eigen::Matrix<double, 3, 1>::UnitZ());
  Eigen::AngleAxis<double> R_pitch(rotation[1],
    Eigen::Matrix<double, 3,1>::UnitY());
  Eigen::AngleAxis<double> R_roll(rotation[2],
    Eigen::Matrix<double, 3, 1>::UnitX());

  Transformation transformation;
  transformation = R_roll*R_pitch*R_yaw;

  transform(transformation);
}

void PlotView::scale(const Scale& scale) {
  Transformation transformation;
  transformation = Eigen::Scaling(scale);

  transform(transformation);
}

void PlotView::clear(const QColor& color) {
}

void PlotView::enableFog(const QColor& color, double start, double end,
    double density) {
}

void PlotView::render(const Points<double, 3>& vertices, const QColor& color,
    double size, bool smooth) {
  std::vector<Points<double, 3> >& points = this->points[color.name()];

  points.push_back(Points<double, 3>(0));
  for (int i = 0; i < vertices.getNumPoints(); ++i) {
    Point point = vertices[i];
    if (project(point))
      points.back() += point;
  }

  if (!points.back().getNumPoints())
    points.pop_back();
}

void PlotView::render(const Points<double, 3>& vertices, const
    std::vector<double>& weights, const QColor& fromColor, const QColor&
    toColor, double fromSize, double toSize, bool smooth) {
  QString palette = "0 '"+fromColor.name()+"', 1 '"+toColor.name()+"'";
  std::vector<Points<double, 3> >& points = palettePoints[palette];
  std::vector<std::vector<double> >& pointWeights =
    palettePointWeights[palette];

  points.push_back(Points<double, 3>(0));
  pointWeights.push_back(std::vector<double>());
  for (int i = 0; i < vertices.getNumPoints(); ++i) {
    Point point = vertices[i];
    if (project(point)) {
      points.back() += point;
      pointWeights.back().push_back(weights[i]);
    }
  }

  if (!points.back().getNumPoints()) {
    points.pop_back();
    pointWeights.pop_back();
  }
}

void PlotView::render(const Line<double, 3>& edges, const QColor& color) {
  std::vector<Line<double, 3> >& lines = this->lines[color.name()];

  lines.push_back(Line<double, 3>(0));
  for (int i = 0, j = 1; j < edges.getNumPoints(); ++i, ++j) {
    Line<double, 3> line(edges[i], edges[j]);
    std::vector<bool> clipped;

    if (project(line, clipped)) {
      lines.back() += line[0];

      if (j+1 == edges.getNumPoints())
        lines.back() += line[1];
      else if (clipped[1]) {
        lines.back() += line[1];
        lines.push_back(Line<double, 3>(0));
      }
    }
  }

  if (lines.back().getNumPoints() < 2)
    lines.pop_back();
}

void PlotView::render(const Line<double, 3>& edges, double weight, const
    QColor& fromColor, const QColor& toColor) {
  QString palette = "0 '"+fromColor.name()+"', 1 '"+toColor.name()+"'";
  std::vector<Line<double, 3> >& lines = paletteLines[palette];
  std::vector<double>& lineWeights = paletteLineWeights[palette];

  lines.push_back(Line<double, 3>(0));
  lineWeights.push_back(weight);
  for (int i = 0, j = 1; j < edges.getNumPoints(); ++i, ++j) {
    Line<double, 3> line(edges[i], edges[j]);
    std::vector<bool> clipped;

    if (project(line, clipped)) {
      lines.back() += line[0];

      if (j+1 == edges.getNumPoints())
        lines.back() += line[1];
      else if (clipped[1]) {
        lines.back() += line[1];
        lines.push_back(Line<double, 3>(0));
      }
    }
  }

  if (!lines.back().getNumPoints()) {
    lines.pop_back();
    lineWeights.pop_back();
  }
}

void PlotView::render(const QString& text, const QColor& color) {
  std::vector<QString>& labels = this->labels[color.name()];

  if (!text.isEmpty()) {
    Point p_1(0.0, 0.0, 0.0), p_2(0.0, 1.0, 0.0);
    if (project(p_1) | project(p_2)) {
      QString label = QString().sprintf(
        "'%s' at %f, %f, %f font '%s, %f' offset character 0, 0.5",
        text.toAscii().constData(), p_1[0], p_1[1], p_1[2],
        font.family().toAscii().constData(), (p_2[1]-p_1[1])*48.0);

      labels.push_back(label);
    }
  }
}

void PlotView::exportFrame(const QString& filename) {
  QDir outputDir = QFileInfo(filename).absoluteDir();
  QFile outputFile(outputDir.absolutePath()+"/"+
    QFileInfo(filename).baseName()+".eps");
  QFile plotFile(outputDir.absolutePath()+"/"+
    QFileInfo(filename).baseName()+".plot");
  QFile dataFile(outputDir.absolutePath()+"/"+
    QFileInfo(filename).baseName()+".dat");

  if ((plotFile.open(QIODevice::WriteOnly | QIODevice::Text)) &&
      (dataFile.open(QIODevice::WriteOnly | QIODevice::Text))) {
    QTextStream plotStream(&plotFile);
    QTextStream dataStream(&dataFile);

    ui->display->clear();
    ui->terminalWidthSpinBox->setEnabled(false);
    ui->terminalHeightSpinBox->setEnabled(false);
    ui->exportButton->setEnabled(false);

    ui->exportProgressBar->setRange(0, 5);
    ui->exportProgressBar->setTextVisible(true);
    ui->exportProgressBar->setFormat("Rendering... (%p%)");
    render();
    ui->exportProgressBar->setValue(1);

    ui->exportProgressBar->setFormat("Writing data... (%p%)");
    int index = 0;
    std::vector<QString> styles;
    QString palette;
    std::vector<QString> labels;
    std::vector<QString> plots;

    writeDataHeader(dataStream);
    for (std::map<QString, std::vector<QString> >::const_iterator
      it = this->labels.begin(); it != this->labels.end(); ++it)
        if (!it->second.empty()) {
      styles.push_back(QString().sprintf("lc rgbcolor '%s'",
        it->first.toAscii().constData()));

      for (int i = 0; i < it->second.size(); ++i)
        labels.push_back(QString().sprintf("%s tc ls %d",
          it->second[i].toAscii().constData(), (int)styles.size()));
    }

    for (std::map<QString, std::vector<Points<double, 3> > >::const_iterator
      it = points.begin(); it != points.end(); ++it)
        if (!it->second.empty()) {
      const std::vector<Points<double, 3> >& points = it->second;

      styles.push_back(QString().sprintf("pt 7 lc rgbcolor '%s'",
        it->first.toAscii().constData()));
      plots.push_back(QString().sprintf("'%s' index %d:%d with points ls %d",
        QFileInfo(dataFile).fileName().toAscii().constData(), index,
        index+(int)points.size()-1, (int)styles.size()));

      for (int i = 0; i < points.size(); ++i) {
          for (int j = 0; j < points[i].getNumPoints(); ++j)
        writeData(dataStream, points[i][j]);
        dataStream << "\n\n";
        ++index;
      }
    }

    for (std::map<QString, std::vector<Points<double, 3> > >::const_iterator
      it = palettePoints.begin(); it != palettePoints.end(); ++it)
        if (!it->second.empty()) {
      const std::vector<Points<double, 3> >& points = it->second;
      const std::vector<std::vector<double> >& weights =
        palettePointWeights[it->first];

      palette = it->first;
      styles.push_back("pt 7 lc palette");
      plots.push_back(QString().sprintf("'%s' index %d:%d with points ls %d",
        QFileInfo(dataFile).fileName().toAscii().constData(), index,
        index+(int)points.size()-1, (int)styles.size()));

      for (int i = 0; i < points.size(); ++i) {
          for (int j = 0; j < points[i].getNumPoints(); ++j)
        writeData(dataStream, points[i][j], weights[i][j]);
        dataStream << "\n\n";
        ++index;
      }
    }

    for (std::map<QString, std::vector<Line<double, 3> > >::const_iterator
      it = lines.begin(); it != lines.end(); ++it)
        if (!it->second.empty()) {
      const std::vector<Line<double, 3> >& lines = it->second;

      styles.push_back(QString().sprintf("lt 1 lc rgbcolor '%s'",
        it->first.toAscii().constData()));
      plots.push_back(QString().sprintf("'%s' index %d:%d with lines ls %d",
        QFileInfo(dataFile).fileName().toAscii().constData(), index,
        index+(int)lines.size()-1, (int)styles.size()));

      for (int i = 0; i < lines.size(); ++i) {
        for (int j = 0; j < lines[i].getNumPoints(); ++j)
          writeData(dataStream, lines[i][j]);
        dataStream << "\n\n";
        ++index;
      }
    }

    for (std::map<QString, std::vector<Line<double, 3> > >::const_iterator
      it = paletteLines.begin(); it != paletteLines.end(); ++it)
        if (!it->second.empty()) {
      const std::vector<Line<double, 3> >& lines = it->second;
      const std::vector<double>& weights = paletteLineWeights[it->first];

      palette = it->first;
      styles.push_back("pt 7 lc palette");
      plots.push_back(QString().sprintf("'%s' index %d:%d with lines ls %d",
        QFileInfo(dataFile).fileName().toAscii().constData(), index,
        index+(int)lines.size()-1, (int)styles.size()));

      for (int i = 0; i < lines.size(); ++i) {
        for (int j = 0; j < lines[i].getNumPoints(); ++j)
          writeData(dataStream, lines[i][j], weights[i]);
        dataStream << "\n\n";
        ++index;
      }
    }
    ui->exportProgressBar->setValue(2);

    ui->exportProgressBar->setFormat("Writing plot... (%p%)");
    writePlotHeader(plotStream);
    writePlot(plotStream, QFileInfo(outputFile).fileName(),
      styles, palette, labels, plots);
    ui->exportProgressBar->setValue(3);

    plotFile.close();
    dataFile.close();

    this->filename = filename;
    ui->exportProgressBar->setFormat("Calling gnuplot... (%p%)");
    plotProcess.setWorkingDirectory(outputDir.absolutePath());
    QStringList plotProcessArguments;
    plotProcessArguments << plotFile.fileName();
    plotProcess.start("gnuplot", plotProcessArguments);

    points.clear();
    palettePoints.clear();
    palettePointWeights.clear();
    lines.clear();
    paletteLines.clear();
    paletteLineWeights.clear();
    this->labels.clear();
  }
}

void PlotView::render() {
  projection = Projection::Identity();
  transformation = Transformation::Identity();

  View::render();

  transformations.clear();
}

bool PlotView::project(Point& point) const {
  Size size = getSize();
  Vertex v = Vertex::Ones();
  v.segment(0, 3) = point;

  v = projection*transformation*v;
  if (v[3] != 0.0) {
    point[0] = v[0]/v[3];
    point[1] = v[1]/v[3];
    point[2] = v[2]/v[3];
    map(point);

    return (point[0] >= 0) && (point[0] <= size[0]) &&
      (point[1] >= 0) && (point[1] <= size[1]) &&
      (point[2] > 0.0) && (point[2] < 1.0);
  }
  else
    return false;
}

bool PlotView::project(Line<double, 3>& line, std::vector<bool>& clipped)
    const {
  if (line.getNumPoints() == 2) {
    clipped.resize(2);
    clipped[0] = false;
    clipped[1] = false;

    Vertex v_1 = Vertex::Ones();
    v_1.segment(0, 3) = line[0];
    Vertex v_2 = Vertex::Ones();
    v_2.segment(0, 3) = line[1];
    v_1 = projection*transformation*v_1;
    v_2 = projection*transformation*v_2;

    line[0] = v_1.segment(0, 3);
    line[1] = v_2.segment(0, 3);
    Eigen::Matrix<double, 2, 1> w(v_1[3], v_2[3]);

    if ((w[0] <= 0.0) && (w[1] <= 0.0))
      return false;
    else if ((w[0] > 0.0) != (w[1] > 0.0)) {
      double w_min = 1e-6;
      int i = (w[0] <= 0.0) ? 0 : 1;
      int j = (w[0] <= 0.0) ? 1 : 0;

      interpolate(line[j], line[i], (w[j]-w_min)/(w[j]-w[i]));
      w[i] = w_min;
      clipped[i] = true;
    }

    for (int i = 0; i < 2; ++i) {
      for (int k = 0; k < 3; ++k)
        line[i][k] /= w[i];
    }

    for (int k = 0; k < 2; ++k) {
      if ((line[0][k] < -1.0) && (line[1][k] < -1.0))
        return false;
      else if ((line[0][k] < -1.0) != (line[1][k] < -1.0)) {
        int i = (line[0][k] < -1.0) ? 0 : 1;
        int j = (line[0][k] < -1.0) ? 1 : 0;

        interpolate(line[j], line[i], (-1.0-line[j][k])/
          (line[i][k]-line[j][k]));
        line[i][k] = -1.0;
        clipped[i] = true;
      }

      if ((line[0][k] > 1.0) && (line[1][k] > 1.0))
        return false;
      else if ((line[0][k] > 1.0) != (line[1][k] > 1.0)) {
        int i = (line[0][k] > 1.0) ? 0 : 1;
        int j = (line[0][k] > 1.0) ? 1 : 0;

        interpolate(line[j], line[i], (1.0-line[j][k])/
          (line[i][k]-line[j][k]));
        line[i][k] = 1.0;
        clipped[i] = true;
      }
    }

    map(line[0]);
    map(line[1]);

    return true;
  }
  else
    return false;
}

void PlotView::interpolate(const Point& fixed, Point& variable, double ratio)
    const {
  variable = fixed+(variable-fixed)*ratio;
}

void PlotView::writePlotHeader(QTextStream& stream) const {
  stream << "# Plot generated by " <<
    Framework::getInstance().getProjectFullName() << "\n\n";
}

void PlotView::writeDataHeader(QTextStream& stream) const {
  stream << "# Data generated by " <<
    Framework::getInstance().getProjectFullName() << "\n";
  stream << "# Format: x, y, z\n\n";
}

void PlotView::writePlot(QTextStream& stream, const QString& filename,
    const std::vector<QString>& styles, const QString& palette, const
    std::vector<QString>& labels, const std::vector<QString>& plots) const {
  Size size = getSize();

  stream << "set terminal postscript eps color size " <<
    size[0] << "cm, " << size[1] << "cm " <<
    "font '" << font.family() << ", " << font.pointSize() << "'\n";
  stream << "set output '" << filename << "'\n";
  stream << "set datafile separator ','\n";
  stream << "set view 0, 0\n";
  stream << "set border 0\n";
  stream << "set lmargin at screen 0\n";
  stream << "set rmargin at screen 1\n";
  stream << "set bmargin at screen 0\n";
  stream << "set tmargin at screen 1\n";
  stream << "set xrange [0:" << size[0] << "]\n";
  stream << "set yrange [0:" << size[1] << "]\n";
  stream << "set zrange [0:1] reverse\n";
  if (!palette.isEmpty())
    stream << "set cbrange [0:1]\n";
  stream << "unset key\n";
  stream << "unset tics\n";
  stream << "set pointsize 0.3\n";
  stream << "set hidden3d\n";
  if (!palette.isEmpty())
    stream << "set view map\n";

  if (!styles.empty()) {
    stream << "\n";
    for (int i = 0; i < styles.size(); ++i)
      stream << "set style line " << i+1 << " " << styles[i] << "\n";
  }
  if (!palette.isEmpty())
    stream << "set palette defined (" << palette << ")\n";
  if (!labels.empty()) {
    stream << "\n";
    for (int i = 0; i < labels.size(); ++i)
      stream << "set label front " << labels[i] << "\n";
  }
  if (!plots.empty()) {
    stream << "\n";
    stream << "splot ";
    for (int i = 0; i < plots.size(); ++i) {
      if (i > 0)
        stream << ", \\\n";
      stream << plots[i];
    }
  }
}

void PlotView::writeData(QTextStream& stream, const Point& point) const {
  stream << point[0] << ", " << point[1] << ", " << point[2] << "\n";
}

void PlotView::writeData(QTextStream& stream, const Point& point, double
    weight) const {
  stream << point[0] << ", " << point[1] << ", " << point[2] << ", " <<
    weight << "\n";
}

void PlotView::fontBrowseClicked() {
  bool ok = false;
  QFont font = QFontDialog::getFont(&ok, this->font, this, "Select Font");

  if (ok)
    setFont(font);
}

void PlotView::plotProcessFinished(int exitCode, QProcess::ExitStatus
    exitStatus) {
  ui->exportProgressBar->setValue(4);

  if ((exitStatus == QProcess::NormalExit) && !exitCode) {
    QDir outputDir = QFileInfo(filename).absoluteDir();
    QFile outputFile(outputDir.absolutePath()+"/"+
      QFileInfo(filename).baseName()+".eps");

    ui->exportProgressBar->setFormat("Calling epstopdf... (%p%)");
    conversionProcess.setWorkingDirectory(outputDir.absolutePath());
    QStringList conversionProcessArguments;
    conversionProcessArguments << "--outfile" << filename <<
      outputFile.fileName();
    conversionProcess.start("epstopdf", conversionProcessArguments);
  }
  else {
    ui->terminalWidthSpinBox->setEnabled(true);
    ui->terminalHeightSpinBox->setEnabled(true);
    ui->exportButton->setEnabled(true);
    ui->exportProgressBar->reset();
  }
}

void PlotView::conversionProcessFinished(int exitCode, QProcess::ExitStatus
    exitStatus) {
  ui->exportProgressBar->setValue(5);

  ui->terminalWidthSpinBox->setEnabled(true);
  ui->terminalHeightSpinBox->setEnabled(true);
  ui->exportButton->setEnabled(true);
  ui->exportProgressBar->reset();

  if ((exitStatus == QProcess::NormalExit) && !exitCode)
    ui->display->setFilename(filename);
}

void PlotView::exportClicked() {
  QString filename = QFileDialog::getSaveFileName(this,
    "Export Frame", "plot.pdf", "PDF files (*.pdf)");

  if (!filename.isNull())
    exportFrame(filename);
}

void PlotView::render(const QImage& image, const QRectF& target,
    const Transformation& transformation, const std::string& serial,
    size_t imageId) {
}
