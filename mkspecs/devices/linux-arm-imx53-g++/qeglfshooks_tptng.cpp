#include "qeglfshooks.h"
#include "qtptnginputcontext.h"

#include <QtPlatformSupport/private/qevdevtouch_p.h>
#include <QtPlatformSupport/private/qevdevkeyboardmanager_p.h>

QT_BEGIN_NAMESPACE

class QEglFSTptNgHooks : public QEglFSHooks
{
public:
    QEglFSTptNgHooks();
    virtual void platformInit();
    virtual void platformDestroy();
    virtual EGLNativeDisplayType platformDisplay() const;
    virtual QSize screenSize() const;
    virtual int screenDepth() const;
    virtual QImage::Format screenFormat() const;
    virtual QSurfaceFormat surfaceFormatFor(const QSurfaceFormat &inputFormat) const;
    virtual EGLNativeWindowType createNativeWindow(const QSize &size, const QSurfaceFormat &format);
    virtual void destroyNativeWindow(EGLNativeWindowType window);
    virtual bool hasCapability(QPlatformIntegration::Capability cap) const;
    QEglFSCursor *createCursor(QEglFSScreen *screen) const;
    virtual QPlatformInputContext *inputContext() const;
    virtual void *nativeResourceForIntegration(const QByteArray &resource);
    virtual void *nativeResourceForContext(const QByteArray &resource, QOpenGLContext *context);

private:
    QEvdevKeyboardManager *m_keyboardHandler;
    QEvdevTouchScreenHandlerThread *m_touchHandler;
    QTptNgInputContext *m_inputContext;
};

QEglFSTptNgHooks::QEglFSTptNgHooks()
    : m_keyboardHandler(0)
    , m_touchHandler(0)
    , m_inputContext(0)
{
}

void QEglFSTptNgHooks::platformInit()
{
    m_keyboardHandler = new QEvdevKeyboardManager(QLatin1String("EvdevKeyboard"), QString() /* spec */);
    m_touchHandler = new QEvdevTouchScreenHandlerThread(QString() /* spec */);
    m_inputContext = new QTptNgInputContext();

    // Turn off the cursor
    qputenv("QT_QPA_EGLFS_HIDECURSOR", "1");
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
    return QSize(800, 480);
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
