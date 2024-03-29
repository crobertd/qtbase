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

#include "qplatformintegration.h"

#include <qpa/qplatformfontdatabase.h>
#include <qpa/qplatformclipboard.h>
#include <qpa/qplatformaccessibility.h>
#include <qpa/qplatformtheme.h>
#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/private/qpixmap_raster_p.h>
#include <qpa/qplatformscreen_p.h>
#include <private/qdnd_p.h>

QT_BEGIN_NAMESPACE

/*!
    Accessor for the platform integration's fontdatabase.

    Default implementation returns a default QPlatformFontDatabase.

    \sa QPlatformFontDatabase
*/
QPlatformFontDatabase *QPlatformIntegration::fontDatabase() const
{
    static QPlatformFontDatabase *db = 0;
    if (!db) {
        db = new QPlatformFontDatabase;
    }
    return db;
}

/*!
    Accessor for the platform integration's clipboard.

    Default implementation returns a default QPlatformClipboard.

    \sa QPlatformClipboard

*/

#ifndef QT_NO_CLIPBOARD

QPlatformClipboard *QPlatformIntegration::clipboard() const
{
    static QPlatformClipboard *clipboard = 0;
    if (!clipboard) {
        clipboard = new QPlatformClipboard;
    }
    return clipboard;
}

#endif

#ifndef QT_NO_DRAGANDDROP
/*!
    Accessor for the platform integration's drag object.

    Default implementation returns 0, implying no drag and drop support.

*/
QPlatformDrag *QPlatformIntegration::drag() const
{
    return 0;
}
#endif

QPlatformNativeInterface * QPlatformIntegration::nativeInterface() const
{
    return 0;
}

QPlatformServices *QPlatformIntegration::services() const
{
    return 0;
}

/*!
    \class QPlatformIntegration
    \since 4.8
    \internal
    \preliminary
    \ingroup qpa
    \brief The QPlatformIntegration class is the entry for WindowSystem specific functionality.

    QPlatformIntegration is the single entry point for windowsystem specific functionality when
    using the QPA platform. It has factory functions for creating platform specific pixmaps and
    windows. The class also controls the font subsystem.

    QPlatformIntegration is a singleton class which gets instantiated in the QGuiApplication
    constructor. The QPlatformIntegration instance do not have ownership of objects it creates in
    functions where the name starts with create. However, functions which don't have a name
    starting with create acts as accessors to member variables.

    It is not trivial to create or build a platform plugin outside of the Qt source tree. Therefore
    the recommended approach for making new platform plugin is to copy an existing plugin inside
    the QTSRCTREE/src/plugins/platform and develop the plugin inside the source tree.

    The minimal platform integration is the smallest platform integration it is possible to make,
    which makes it an ideal starting point for new plugins. For a slightly more advanced plugin,
    consider reviewing the directfb plugin, or the testlite plugin.
*/

/*!
    \fn QPlatformPixmap *QPlatformIntegration::createPlatformPixmap(QPlatformPixmap::PixelType type) const

    Factory function for QPlatformPixmap. PixelType can be either PixmapType or BitmapType.
    \sa QPlatformPixmap
*/

/*!
    \fn QPlatformWindow *QPlatformIntegration::createPlatformWindow(QWindow *window) const

    Factory function for QPlatformWindow. The \a window parameter is a pointer to the top level
    window which the QPlatformWindow is supposed to be created for.

    All top level windows have to have a QPlatformWindow, and it will be created when the
    QPlatformWindow is set to be visible for the first time. If the top level window's flags are
    changed, or if the top level window's QPlatformWindowFormat is changed, then the top level
    window's QPlatformWindow is deleted and a new one is created.

    In the constructor, of the QPlatformWindow, the window flags, state, title and geometry
    of the \a window should be applied to the underlying window. If the resulting flags or state
    differs, the resulting values should be set on the \a window using QWindow::setWindowFlags()
    or QWindow::setWindowState(), respectively.

    \sa QPlatformWindow, QPlatformWindowFormat
    \sa createPlatformBackingStore()
*/

/*!
    \fn QPlatformBackingStore *QPlatformIntegration::createPlatformBackingStore(QWindow *window) const

    Factory function for QPlatformBackingStore. The QWindow parameter is a pointer to the
    top level widget(tlw) the window surface is created for. A QPlatformWindow is always created
    before the QPlatformBackingStore for tlw where the widget also requires a backing store.

    \sa QBackingStore
    \sa createPlatformWindow()
*/

/*!
    \enum QPlatformIntegration::Capability

    Capabilities are used to determing specific features of a platform integration

    \value ThreadedPixmaps The platform uses a pixmap implementation that is reentrant
    and can be used from multiple threads, like the raster paint engine and QImage based
    pixmaps.

    \value OpenGL The platform supports OpenGL

    \value ThreadedOpenGL The platform supports using OpenGL outside the GUI thread.

    \value SharedGraphicsCache The platform supports a shared graphics cache

    \value BufferQueueingOpenGL The OpenGL implementation on the platform will queue
    up buffers when swapBuffers() is called and block only when its buffer pipeline
    is full, rather than block immediately.

    \value MultipleWindows The platform supports multiple QWindows, i.e. does some kind
    of compositing either client or server side. Some platforms might only support a
    single fullscreen window.
 */


/*!

    \fn QAbstractEventDispatcher *QPlatformIntegration::guiThreadEventDispatcher() const = 0

    Accessor function for the event dispatcher. The platform plugin should create
    an instance of the QAbstractEventDispatcher in its constructor and set it
    on the application using QGuiApplicationPrivate::instance()->setEventDispatcher().
    The event dispatcher is owned by QGuiApplication, the accessor should return
    a flat pointer.
    \sa QGuiApplicationPrivate
*/

bool QPlatformIntegration::hasCapability(Capability cap) const
{
    Q_UNUSED(cap);
    return false;
}

QPlatformPixmap *QPlatformIntegration::createPlatformPixmap(QPlatformPixmap::PixelType type) const
{
    return new QRasterPlatformPixmap(type);
}

#ifndef QT_NO_OPENGL
QPlatformOpenGLContext *QPlatformIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    Q_UNUSED(context);
    qWarning("This plugin does not support createPlatformOpenGLContext!");
    return 0;
}
#endif

/*!
   Factory function for QPlatformSharedGraphicsCache. This function will return 0 if the platform
   integration does not support any shared graphics cache mechanism for the given \a cacheId.
*/
QPlatformSharedGraphicsCache *QPlatformIntegration::createPlatformSharedGraphicsCache(const char *cacheId) const
{
    qWarning("This plugin does not support createPlatformSharedGraphicsBuffer for cacheId: %s!",
             cacheId);
    return 0;
}

/*!
   Factory function for QPaintEngine. This function will return 0 if the platform
   integration does not support creating any paint engine the given \a paintDevice.
*/
QPaintEngine *QPlatformIntegration::createImagePaintEngine(QPaintDevice *paintDevice) const
{
    Q_UNUSED(paintDevice)
    return 0;
}

/*!
  Returns the platforms input context.

  The default implementation returns 0, implying no input method support.
*/
QPlatformInputContext *QPlatformIntegration::inputContext() const
{
    return 0;
}

#ifndef QT_NO_ACCESSIBILITY

/*!
  Returns the platforms accessibility.

  The default implementation returns 0, implying no accessibility support.
*/
QPlatformAccessibility *QPlatformIntegration::accessibility() const
{
    return 0;
}

#endif

QVariant QPlatformIntegration::styleHint(StyleHint hint) const
{
    switch (hint) {
    case CursorFlashTime:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::CursorFlashTime);
    case KeyboardInputInterval:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::KeyboardInputInterval);
    case KeyboardAutoRepeatRate:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::KeyboardAutoRepeatRate);
    case MouseDoubleClickInterval:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::MouseDoubleClickInterval);
    case StartDragDistance:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::StartDragDistance);
    case StartDragTime:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::StartDragTime);
    case ShowIsFullScreen:
        return false;
    case PasswordMaskDelay:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::PasswordMaskDelay);
    case FontSmoothingGamma:
        return qreal(1.7);
    case StartDragVelocity:
        return QPlatformTheme::defaultThemeHint(QPlatformTheme::StartDragVelocity);
    case UseRtlExtensions:
        return QVariant(false);
    case SynthesizeMouseFromTouchEvents:
        return true;
    }

    return 0;
}

Qt::KeyboardModifiers QPlatformIntegration::queryKeyboardModifiers() const
{
    return QGuiApplication::keyboardModifiers();
}

/*!
  Should be used to obtain a list of possible shortcuts for the given key
  event. As that needs system functionality it cannot be done in qkeymapper.

  One example for more than 1 possibility is the key combination of Shift+5.
  That one might trigger a shortcut which is set as "Shift+5" as well as one
  using %. These combinations depend on the currently set keyboard layout
  which cannot be obtained by Qt functionality.
*/
QList<int> QPlatformIntegration::possibleKeys(const QKeyEvent *) const
{
    return QList<int>();
}

/*!
  Should be called by the implementation whenever a new screen is added.

  The first screen added will be the primary screen, used for default-created
  windows, GL contexts, and other resources unless otherwise specified.

  This adds the screen to QGuiApplication::screens(), and emits the
  QGuiApplication::screenAdded() signal.

  The screen is automatically removed when the QPlatformScreen is destroyed.
*/
void QPlatformIntegration::screenAdded(QPlatformScreen *ps)
{
    QScreen *screen = new QScreen(ps);
    ps->d_func()->screen = screen;
    QGuiApplicationPrivate::screen_list << screen;
    emit qGuiApp->screenAdded(screen);
}

QStringList QPlatformIntegration::themeNames() const
{
    return QStringList();
}

class QPlatformTheme *QPlatformIntegration::createPlatformTheme(const QString &name) const
{
    Q_UNUSED(name)
    return new QPlatformTheme;
}

QT_END_NAMESPACE
