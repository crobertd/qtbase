/****************************************************************************
 **
 ** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtGui module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include <QtCore>
#include <QtGui>
#include <QtWidgets>


class PixmapPainter : public QWidget
{
public:
    PixmapPainter();
    void paintEvent(QPaintEvent *event);

    QPixmap pixmap1X;
    QPixmap pixmap2X;
    QPixmap pixmapLarge;
    QImage image1X;
    QImage image2X;
    QImage imageLarge;
    QIcon qtIcon;
};


PixmapPainter::PixmapPainter()
{
    pixmap1X = QPixmap(":/qticon.png");
    pixmap2X = QPixmap(":/qticon@2x.png");
    pixmapLarge = QPixmap(":/qticon_large.png");

    image1X = QImage(":/qticon.png");
    image2X = QImage(":/qticon@2x.png");
    imageLarge = QImage(":/qticon_large.png");

    qtIcon.addFile(":/qticon.png");
    qtIcon.addFile(":/qticon@2x.png");
}

void PixmapPainter::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(QRect(QPoint(0, 0), size()), QBrush(Qt::gray));

    int pixmapPointSize = 64;
    int y = 30;
    int dy = 150;

    int x = 10;
    int dx = 80;
    // draw at point
//          qDebug() << "paint pixmap" << pixmap1X.devicePixelRatio();
          p.drawPixmap(x, y, pixmap1X);
    x+=dx;p.drawPixmap(x, y, pixmap2X);
    x+=dx;p.drawPixmap(x, y, pixmapLarge);
  x+=dx*2;p.drawPixmap(x, y, qtIcon.pixmap(QSize(pixmapPointSize, pixmapPointSize)));
    x+=dx;p.drawImage(x, y, image1X);
    x+=dx;p.drawImage(x, y, image2X);
    x+=dx;p.drawImage(x, y, imageLarge);

    // draw at 64x64 rect
    y+=dy;
    x = 10;
          p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), pixmap1X);
    x+=dx;p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), pixmap2X);
    x+=dx;p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), pixmapLarge);
    x+=dx;p.drawPixmap(QRect(x, y, pixmapPointSize, pixmapPointSize), qtIcon.pixmap(QSize(pixmapPointSize, pixmapPointSize)));
    x+=dx;p.drawImage(QRect(x, y, pixmapPointSize, pixmapPointSize), image1X);
    x+=dx;p.drawImage(QRect(x, y, pixmapPointSize, pixmapPointSize), image2X);
    x+=dx;p.drawImage(QRect(x, y, pixmapPointSize, pixmapPointSize), imageLarge);


    // draw at 128x128 rect
    y+=dy - 50;
    x = 10;
               p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), pixmap1X);
    x+=dx * 2; p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), pixmap2X);
    x+=dx * 2; p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), pixmapLarge);
    x+=dx * 2; p.drawPixmap(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), qtIcon.pixmap(QSize(pixmapPointSize, pixmapPointSize)));
    x+=dx * 2; p.drawImage(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), image1X);
    x+=dx * 2; p.drawImage(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), image2X);
    x+=dx * 2; p.drawImage(QRect(x, y, pixmapPointSize * 2, pixmapPointSize * 2), imageLarge);
 }

class Labels : public QWidget
{
public:
    Labels();

    QPixmap pixmap1X;
    QPixmap pixmap2X;
    QPixmap pixmapLarge;
    QIcon qtIcon;
};

Labels::Labels()
{
    pixmap1X = QPixmap(":/qticon.png");
    pixmap2X = QPixmap(":/qticon@2x.png");
    pixmapLarge = QPixmap(":/qticon_large.png");

    qtIcon.addFile(":/qticon.png");
    qtIcon.addFile(":/qticon@2x.png");
    setWindowIcon(qtIcon);
    setWindowTitle("Labels");

    QLabel *label1x = new QLabel();
    label1x->setPixmap(pixmap1X);
    QLabel *label2x = new QLabel();
    label2x->setPixmap(pixmap2X);
    QLabel *labelIcon = new QLabel();
    labelIcon->setPixmap(qtIcon.pixmap(QSize(64,64)));
    QLabel *labelLarge = new QLabel();
    labelLarge->setPixmap(pixmapLarge);

    QHBoxLayout *layout = new QHBoxLayout(this);
//    layout->addWidget(label1x); //expected low-res on high-dpi displays
    layout->addWidget(label2x);
//    layout->addWidget(labelIcon);
//    layout->addWidget(labelLarge); // expected large size and low-res
    setLayout(layout);
}

class MainWindow : public QMainWindow
{
public:
    MainWindow();

    QIcon qtIcon;
    QIcon qtIcon1x;
    QIcon qtIcon2x;

    QToolBar *fileToolBar;
};

MainWindow::MainWindow()
{
    qtIcon.addFile(":/qticon.png");
    qtIcon.addFile(":/qticon@2x.png");
    qtIcon1x.addFile(":/qticon.png");
    qtIcon2x.addFile(":/qticon@2x.png");
    setWindowIcon(qtIcon);
    setWindowTitle("MainWindow");

    fileToolBar = addToolBar(tr("File"));
//    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fileToolBar->addAction(new QAction(qtIcon, QString("1x and 2x"), this));
    fileToolBar->addAction(new QAction(qtIcon1x, QString("1x"), this));
    fileToolBar->addAction(new QAction(qtIcon2x, QString("2x"), this));
}

class StandardIcons : public QWidget
{
public:
    void paintEvent(QPaintEvent *event)
    {
        int x = 10;
        int y = 10;
        int dx = 50;
        int dy = 50;
        int maxX = 500;

        for (int iconIndex = QStyle::SP_TitleBarMenuButton; iconIndex < QStyle::SP_MediaVolumeMuted; ++iconIndex) {
            QIcon icon = qApp->style()->standardIcon(QStyle::StandardPixmap(iconIndex));
            QPainter p(this);
            p.drawPixmap(x, y, icon.pixmap(dx - 5, dy - 5));
            if (x + dx > maxX)
                y+=dy;
            x = ((x + dx) % maxX);
        }
    };
};

class Caching : public QWidget
{
public:
    void paintEvent(QPaintEvent *event)
    {
        QSize layoutSize(75, 75);

        QPainter widgetPainter(this);
        widgetPainter.fillRect(QRect(QPoint(0, 0), this->size()), Qt::gray);

        {
            const qreal devicePixelRatio = this->windowHandle()->devicePixelRatio();
            QPixmap cache(layoutSize * devicePixelRatio);
            cache.setDevicePixelRatio(devicePixelRatio);

            QPainter cachedPainter(&cache);
            cachedPainter.fillRect(QRect(0,0, 75, 75), Qt::blue);
            cachedPainter.fillRect(QRect(10,10, 55, 55), Qt::red);
            cachedPainter.drawEllipse(QRect(10,10, 55, 55));

            QPainter widgetPainter(this);
            widgetPainter.drawPixmap(QPoint(10, 10), cache);
        }

        {
            const qreal devicePixelRatio = this->windowHandle()->devicePixelRatio();
            QImage cache = QImage(layoutSize * devicePixelRatio, QImage::QImage::Format_ARGB32_Premultiplied);
            cache.setDevicePixelRatio(devicePixelRatio);

            QPainter cachedPainter(&cache);
            cachedPainter.fillRect(QRect(0,0, 75, 75), Qt::blue);
            cachedPainter.fillRect(QRect(10,10, 55, 55), Qt::red);
            cachedPainter.drawEllipse(QRect(10,10, 55, 55));

            QPainter widgetPainter(this);
            widgetPainter.drawImage(QPoint(95, 10), cache);
        }

    }
};

class Style : public QWidget {
public:
    QPushButton *button;
    QLineEdit *lineEdit;
    QSlider *slider;
    QHBoxLayout *row1;

    Style() {
        row1 = new QHBoxLayout();
        setLayout(row1);

        button = new QPushButton();
        button->setText("Test Button");
        row1->addWidget(button);

        lineEdit = new QLineEdit();
        lineEdit->setText("Test Lineedit");
        row1->addWidget(lineEdit);

        slider = new QSlider();
        row1->addWidget(slider);

        row1->addWidget(new QSpinBox);
        row1->addWidget(new QScrollBar);

        QTabBar *tab  = new QTabBar();
        tab->addTab("Foo");
        tab->addTab("Bar");
        row1->addWidget(tab);
    }
};

class Fonts : public QWidget
{
public:
    void paintEvent(QPaintEvent *event)
    {
        QPainter painter(this);
        int y = 40;
        for (int fontSize = 2; fontSize < 18; fontSize += 2) {
            QFont font;
            font.setPointSize(fontSize);
            QString string = QString(QStringLiteral("%1 The quick brown fox jumped over the lazy Doug.")).arg(fontSize);
            painter.setFont(font);
            painter.drawText(10, y, string);
            y += (fontSize  * 2.5);
        }
    }
};


template <typename T>
void apiTestdevicePixelRatioGetter()
{
    if (0) {
        T *t = 0;
        t->devicePixelRatio();
    }
}

template <typename T>
void apiTestdevicePixelRatioSetter()
{
    if (0) {
        T *t = 0;
        t->setDevicePixelRatio(2.0);
    }
}

void apiTest()
{
    // compile call to devicePixelRatio getter and setter (verify spelling)
    apiTestdevicePixelRatioGetter<QWindow>();
    apiTestdevicePixelRatioGetter<QScreen>();
    apiTestdevicePixelRatioGetter<QGuiApplication>();

    apiTestdevicePixelRatioGetter<QImage>();
    apiTestdevicePixelRatioSetter<QImage>();
    apiTestdevicePixelRatioGetter<QPixmap>();
    apiTestdevicePixelRatioSetter<QPixmap>();
}

int main(int argc, char **argv)
{
    qputenv("QT_HIGHDPI_AWARE", "1");
    QApplication app(argc, argv);

    PixmapPainter pixmapPainter;

//  Enable for lots of pixmap drawing
    pixmapPainter.show();

    Labels label;
    label.resize(200, 200);
    label.show();

    MainWindow mainWindow;
    mainWindow.show();

    StandardIcons icons;
    icons.resize(510, 510);
    icons.show();

    Caching caching;
    caching.resize(300, 300);
    caching.show();

    Style style;
    style.show();

    Fonts fonts;
    fonts.show();

    return app.exec();
}
