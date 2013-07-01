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

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtGui/QFileDialog>

#include <FTGL/ftgl.h>

#include "gui/glview.h"

#include "ui_glview.h"

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

GLView::GLView() :
  ui(new Ui_GLView()),
  quadric(0),
  font(0) {
  ui->setupUi(this);

  menu.addAction("Set Font...", this, SLOT(fontBrowseClicked()));
  shadeAction = menu.addAction("Shaded", this, SLOT(shadedClicked()),
    Qt::CTRL+Qt::Key_S);
  shadeAction->setCheckable(true);
  menu.addSeparator();
  menu.addAction("Dump Frame", this, SLOT(dumpClicked()), Qt::Key_Print);

  connect(ui->display, SIGNAL(paint()), this, SLOT(render()));
  connect(ui->display,
    SIGNAL(mousePressed(const QPoint&, Qt::MouseButtons)), this,
    SLOT(mousePressed(const QPoint&, Qt::MouseButtons)));
  connect(ui->display,
    SIGNAL(mouseMoved(const QPoint&, int, Qt::MouseButtons)), this,
    SLOT(mouseMoved(const QPoint&, int, Qt::MouseButtons)));

  quadric = gluNewQuadric();
  gluQuadricNormals(quadric, GLU_SMOOTH);

  setFont("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf");

  setDumpDirectory(QDir::current().path());
  setDumpFrameSize(1280, 720);
  setDumpFormat("dump3d%06d.png");
  setDumpAll(false);
}

GLView::~GLView() {
  if (font)
    delete font;

  delete ui;
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

GLDisplay& GLView::getDisplay() {
  return *ui->display;
}

const GLDisplay& GLView::getDisplay() const {
  return *ui->display;
}

void GLView::setFont(const QString& filename) {
  if (fontFilename != filename) {
    if (font) {
      delete font;
      font = 0;
    }
    this->fontFilename = filename;

    emit fontChanged(fontFilename);
    update();
  }
}

const QString& GLView::getFont() const {
  return fontFilename;
}

GLView::Size GLView::getSize() const {
  return Size(
    ui->display->context()->device()->width(),
    ui->display->context()->device()->height());
}

GLView::Viewport GLView::getViewport() const {
  Viewport viewport;
  glGetIntegerv(GL_VIEWPORT, viewport.data());

  return viewport;
}

void GLView::setProjection(const Projection& projection) {
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projection.data());
}

GLView::Projection GLView::getProjection() const {
  Projection projection;
  glGetDoublev(GL_PROJECTION_MATRIX, projection.data());

  return projection;
}

void GLView::setTransformation(const Transformation& transformation) {
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(transformation.data());
}

GLView::Transformation GLView::getTransformation() const {
  Transformation transformation;
  glGetDoublev(GL_MODELVIEW_MATRIX, transformation.data());

  return transformation;
}

void GLView::setDumpDirectory(const QString& dirname) {
  QDir dir(dirname);
  ui->dumpDirEdit->setText(dir.absolutePath());
}

void GLView::setDumpFrameSize(size_t width, size_t height) {
  ui->dumpFrameWidthSpinBox->setValue(width);
  ui->dumpFrameHeightSpinBox->setValue(height);
}

void GLView::setDumpFormat(const QString& format) {
  ui->dumpFormatEdit->setText(format);
}

void GLView::setDumpAll(bool dumpAll) {
  if (dumpAll)
    connect(this, SIGNAL(cleanup(View&)), this, SLOT(dumpFrame()));
  else
    disconnect(SIGNAL(cleanup(View&)));

  ui->dumpAllCheckBox->setChecked(dumpAll);
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void GLView::saveTransformation() {
  glPushMatrix();
}

void GLView::restoreTransformation() {
  glPopMatrix();
}

void GLView::transform(const Transformation& transformation) {
  glMatrixMode(GL_MODELVIEW);
  glMultMatrixd(transformation.data());
}

void GLView::translate(const Translation& translation) {
  glTranslated(translation[0], translation[1], translation[2]);
}

void GLView::rotate(const Rotation& rotation) {
  glRotated(rotation[2]*180.0/M_PI, 1, 0, 0);
  glRotated(rotation[1]*180.0/M_PI, 0, 1, 0);
  glRotated(rotation[0]*180.0/M_PI, 0, 0, 1);
}

void GLView::scale(const Scale& scale) {
  glScaled(scale[0], scale[1], scale[2]);
}

void GLView::clear(const QColor& color) {
  glClearColor(color.redF(), color.greenF(), color.blueF(), 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLView::enableFog(const QColor& color, double start, double end,
    double density) {
  float fogColor[] = {(float)color.redF(), (float)color.greenF(),
    (float)color.blueF(), 1.0};

  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);

  glFogi(GL_FOG_MODE, GL_LINEAR);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
  glFogf(GL_FOG_START, start);
  glFogf(GL_FOG_END, end);
  glFogf(GL_FOG_DENSITY, density);
}

void GLView::render(const Points<double, 3>& vertices, const QColor& color,
    double size, bool smooth) {
  if (size > 1.0)
    glPointSize(size);
  else
    glPointSize(1.0);
  if (smooth)
    glEnable(GL_POINT_SMOOTH);
  else
    glDisable(GL_POINT_SMOOTH);
  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());

  glBegin(GL_POINTS);
  for (int i = 0; i < vertices.getNumPoints(); ++i)
    glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
  glEnd();

  glDisable(GL_POINT_SMOOTH);
  glPointSize(1.0);
}

void GLView::render(const Points<double, 3>& vertices, const
    std::vector<double>& weights, const QColor& fromColor, const QColor&
    toColor, double fromSize, double toSize, bool smooth){
  if (smooth)
    glEnable(GL_POINT_SMOOTH);
  else
    glDisable(GL_POINT_SMOOTH);

  for (int i = 0; i < vertices.getNumPoints(); ++i) {
    double size = (1.0-weights[i])*fromSize+weights[i]*toSize;
    if (size > 1.0)
      glPointSize(size);
    else
      glPointSize(1.0);

    glBegin(GL_POINTS);
    glColor4f((1.0-weights[i])*fromColor.redF()+weights[i]*toColor.redF(),
      (1.0-weights[i])*fromColor.greenF()+weights[i]*toColor.greenF(),
      (1.0-weights[i])*fromColor.blueF()+weights[i]*toColor.blueF(),
      (1.0-weights[i])*fromColor.alphaF()+weights[i]*toColor.alphaF());
    glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
    glEnd();
  }

  glDisable(GL_POINT_SMOOTH);
  glPointSize(1.0);
}

void GLView::render(const Line<double, 3>& edges, const QColor& color) {
  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < edges.getNumPoints(); ++i)
    glVertex3f(edges[i][0], edges[i][1], edges[i][2]);
  glEnd();
}

void GLView::render(const Line<double, 3>& edges, double weight, const QColor&
    fromColor, const QColor& toColor) {
  glColor4f((1.0-weight)*fromColor.redF()+weight*toColor.redF(),
    (1.0-weight)*fromColor.greenF()+weight*toColor.greenF(),
    (1.0-weight)*fromColor.blueF()+weight*toColor.blueF(),
    (1.0-weight)*fromColor.alphaF()+weight*toColor.alphaF());

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < edges.getNumPoints(); ++i)
    glVertex3f(edges[i][0], edges[i][1], edges[i][2]);
  glEnd();
}

void GLView::render(const QString& text, const QColor& color) {
  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());

  if (!font) {
    QFileInfo fileInfo(fontFilename);
    if (fileInfo.isFile() && fileInfo.isReadable()) {
      font = new FTPolygonFont(fontFilename.toAscii().constData());
      font->UseDisplayList(false);
      font->FaceSize(1);
    }
  }

  if (font)
    font->Render(text.toAscii().constData());

  // memory leak in FTGL
  if (font) {
    delete font;
    font = 0;
  }
}

void GLView::render(const Ellipsoid<double, 3>& ellipsoid, size_t numSegments,
    const QColor& color) {
  if (shadeAction->isChecked()) {
    saveTransformation();

    translate(ellipsoid.getOrigin());
    rotate(ellipsoid.getOrientation());
    scale(ellipsoid.getSize());

    glEnable(GL_LIGHTING);

    float specular[] = {0.0, 0.0, 0.0, 1.0};
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    gluSphere(quadric, 1.0, numSegments, numSegments);

    glDisable(GL_LIGHTING);

    restoreTransformation();
  }
  else
    View::render(ellipsoid, numSegments, color);
}

bool GLView::dumpFrame(const QString& filename, size_t width, size_t height) {
  static bool dumping = false;

  if (!dumping) {
    dumping = true;
    QPixmap pixmap = ui->display->renderPixmap(width, height);
    dumping = false;

    return pixmap.save(filename);
  }
  else
    return false;
}

void GLView::update() {
  ui->display->update();
}

void GLView::render() {
  emit prepare(*this);

  glEnable(GL_LIGHT0);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);

  float lightPosition[] = {0.0, 0.0, 1000.0, 0.0};
  float lightAmbient[] = {0.5, 0.5, 0.5, 1.0};
  float lightDiffuse[] = {0.75, 0.75, 0.75, 1.0};
  float lightSpecular[] = {1.0, 1.0, 1.0, 1.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

  emit render(*this);
  emit cleanup(*this);
}

void GLView::dumpFrame() {
  QDir dir(ui->dumpDirEdit->text());

  if (dir.isReadable()) {
    QString filename;
    filename.sprintf(ui->dumpFormatEdit->text().toAscii().constData(),
      ui->dumpFrameSpinBox->value());

    QFileInfo fileInfo(dir, filename);
    if (dumpFrame(fileInfo.absoluteFilePath(),
        ui->dumpFrameWidthSpinBox->value(),
        ui->dumpFrameHeightSpinBox->value()))
      ui->dumpFrameSpinBox->setValue(ui->dumpFrameSpinBox->value()+1);
  }
}

void GLView::mousePressed(const QPoint& position, Qt::MouseButtons buttons) {
  this->mousePosition = position;
}

void GLView::mouseMoved(const QPoint& position, int wheel, Qt::MouseButtons
    buttons) {
  emit transform(*this, this->mousePosition, position, wheel, buttons);
  this->mousePosition = position;
}

void GLView::fontBrowseClicked() {
  QString filename = QFileDialog::getOpenFileName(this,
    "Open Font File", fontFilename, "TrueType font files (*.ttf)");

  if (!filename.isNull())
    setFont(filename);
}

void GLView::shadedClicked() {
  update();
}

void GLView::dumpDirBrowseClicked() {
  QString dirname = QFileDialog::getExistingDirectory(this,
    "Select Dump Directory", ui->dumpDirEdit->text());

  if (!dirname.isNull())
    ui->dumpDirEdit->setText(dirname);
}

void GLView::dumpClicked() {
  dumpFrame();
}

void GLView::dumpAllToggled(bool checked) {
  setDumpAll(checked);
}

void GLView::render(const QImage& image, const QRectF& target,
    const Transformation& transformation, const std::string& serial,
    size_t imageId) {
//  saveTransformation();
//  translate(transformation.translation());
//  Eigen::Vector3d angles = transformation.rotation().eulerAngles(2, 1, 0);
//  rotate(angles);
//  static GLuint textureID = -1;
//  static int prevImageId = -1;
//  if (imageId != prevImageId) {
//    if (textureID > 0)
//      getDisplay().deleteTexture(textureID);
//    textureID = getDisplay().bindTexture(image, GL_TEXTURE_2D, GL_LUMINANCE);
//  }
//  QColor color = Qt::lightGray;
//  glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
//  getDisplay().drawTexture(target, textureID);
//  prevImageId = imageId;
//  restoreTransformation();
}
