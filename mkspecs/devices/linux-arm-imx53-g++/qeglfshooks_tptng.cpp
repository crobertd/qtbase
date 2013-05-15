#include "qeglfshooks.h"
#include "qtptnginputcontext.h"
#include "qtptngtheme.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include <QtPlatformSupport/private/qevdevtouch_p.h>
#include <QtPlatformSupport/private/qevdevkeyboardmanager_p.h>

QT_BEGIN_NAMESPACE

class QEglFSTptNgHooks : public QEglFSHooks
{
public:
    QEglFSTptNgHooks();
    void platformInit();
    void platformDestroy();
    EGLNativeDisplayType platformDisplay() const;
    QSize screenSize() const;
    int screenDepth() const;
    QImage::Format screenFormat() const;
    QSurfaceFormat surfaceFormatFor(const QSurfaceFormat &inputFormat) const;
    EGLNativeWindowType createNativeWindow(const QSize &size, const QSurfaceFormat &format);
    void destroyNativeWindow(EGLNativeWindowType window);
    bool hasCapability(QPlatformIntegration::Capability cap) const;
    QEglFSCursor *createCursor(QEglFSScreen *screen) const;
    QPlatformInputContext *inputContext() const;
    QStringList themeNames() const;
    QPlatformTheme *createPlatformTheme(const QString &name) const;
    QVariant styleHint(QPlatformIntegration::StyleHint hint) const;
    void *nativeResourceForIntegration(const QByteArray &resource);
    void *nativeResourceForContext(const QByteArray &resource, QOpenGLContext *context);

private:
    QEvdevKeyboardManager *m_keyboardHandler;
    QEvdevTouchScreenHandlerThread *m_touchHandler;
    QTptNgInputContext *m_inputContext;
    QSize m_screenSize;
};

QEglFSTptNgHooks::QEglFSTptNgHooks()
    : m_keyboardHandler(0)
    , m_touchHandler(0)
    , m_inputContext(0)
    , m_screenSize(800, 480)
{
}

void QEglFSTptNgHooks::platformInit()
{
    m_keyboardHandler = new QEvdevKeyboardManager(QLatin1String("EvdevKeyboard"), QString() /* spec */);
    m_touchHandler = new QEvdevTouchScreenHandlerThread(QString() /* spec */);
    m_inputContext = new QTptNgInputContext();

    // Turn off the cursor
    qputenv("QT_QPA_EGLFS_HIDECURSOR", "1");

    // Read the screen width and height
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd >= 0) {
        struct fb_var_screeninfo vinfo;
        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == 0) {
            m_screenSize.setWidth(vinfo.xres);
            m_screenSize.setHeight(vinfo.yres);
        }
        close(fbfd);
    }
}

void QEglFSTptNgHooks::platformDestroy()
{
    if (m_keyboardHandler) delete m_keyboardHandler;
    if (m_touchHandler) delete m_touchHandler;
    if (m_inputContext) delete m_inputContext;
}

EGLNativeDisplayType QEglFSTptNgHooks::platformDisplay() const
{
    return EGL_DEFAULT_DISPLAY;
}

QSize QEglFSTptNgHooks::screenSize() const
{
    return m_screenSize;
}

int QEglFSTptNgHooks::screenDepth() const
{
    return 32;
}

QImage::Format QEglFSTptNgHooks::screenFormat() const
{
    return QImage::Format_RGB32;
}

QSurfaceFormat QEglFSTptNgHooks::surfaceFormatFor(const QSurfaceFormat &inputFormat) const
{
    return inputFormat;
}

EGLNativeWindowType QEglFSTptNgHooks::createNativeWindow(const QSize &size, const QSurfaceFormat &format)
{
    Q_UNUSED(size);
    Q_UNUSED(format);
    return 0;
}

void QEglFSTptNgHooks::destroyNativeWindow(EGLNativeWindowType window)
{
    Q_UNUSED(window);
}

bool QEglFSTptNgHooks::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
        case QPlatformIntegration::ThreadedPixmaps:
        case QPlatformIntegration::OpenGL:
        case QPlatformIntegration::ThreadedOpenGL:
            return true;
        default:
            return false;
    }
}

QEglFSCursor *QEglFSTptNgHooks::createCursor(QEglFSScreen *screen) const
{
    Q_UNUSED(screen);
    return 0;
}

QPlatformInputContext *QEglFSTptNgHooks::inputContext() const
{
    return m_inputContext;
}

QStringList QEglFSTptNgHooks::themeNames() const
{
    return QStringList(QLatin1String(QTptNgTheme::name));
}

QPlatformTheme *QEglFSTptNgHooks::createPlatformTheme(const QString &name) const
{
    if (name == QLatin1String(QTptNgTheme::name))
        return new QTptNgTheme;
    return 0;
}

QVariant QEglFSTptNgHooks::styleHint(QPlatformIntegration::StyleHint hint) const
{
    Q_UNUSED(hint);
    return QVariant();
}

void *QEglFSTptNgHooks::nativeResourceForIntegration(const QByteArray &resource)
{
    QByteArray lowerCaseResource = resource.toLower();

    if (lowerCaseResource == "inputcontext")
        return m_inputContext;

    return 0;
}

void *QEglFSTptNgHooks::nativeResourceForContext(const QByteArray &resource, QOpenGLContext *context)
{
    Q_UNUSED(resource);
    Q_UNUSED(context);
    return 0;
}

QEglFSTptNgHooks eglFSTptNgHooks;
QEglFSHooks *platformHooks = &eglFSTptNgHooks;

QT_END_NAMESPACE
