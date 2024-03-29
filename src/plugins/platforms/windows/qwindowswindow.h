/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QWINDOWSWINDOW_H
#define QWINDOWSWINDOW_H

#include "qtwindows_additional.h"
#ifdef Q_OS_WINCE
#  include "qplatformfunctions_wince.h"
#endif
#include "qwindowscursor.h"

#include <qpa/qplatformwindow.h>

#ifdef QT_OPENGL_ES_2
#  include <QtCore/QSharedPointer>
#  include <EGL/egl.h>
#endif

QT_BEGIN_NAMESPACE

class QWindowsOleDropTarget;
class QDebug;

#ifdef QT_OPENGL_ES_2
class QWindowsEGLStaticContext;
#endif

struct QWindowsGeometryHint
{
    QWindowsGeometryHint() {}
    explicit QWindowsGeometryHint(const QWindow *w);
    static QMargins frame(DWORD style, DWORD exStyle);
#ifndef Q_OS_WINCE //MinMax maybe define struct if not available
    void applyToMinMaxInfo(DWORD style, DWORD exStyle, MINMAXINFO *mmi) const;
    void applyToMinMaxInfo(HWND hwnd, MINMAXINFO *mmi) const;
#endif
    bool validSize(const QSize &s) const;

    static inline QPoint mapToGlobal(HWND hwnd, const QPoint &);
    static inline QPoint mapToGlobal(const QWindow *w, const QPoint &);
    static inline QPoint mapFromGlobal(const HWND hwnd, const QPoint &);
    static inline QPoint mapFromGlobal(const QWindow *w, const QPoint &);

    static bool positionIncludesFrame(const QWindow *w);

    QSize minimumSize;
    QSize maximumSize;
};

struct QWindowCreationContext
{
    QWindowCreationContext(const QWindow *w, const QRect &r,
                           DWORD style, DWORD exStyle);
#ifndef Q_OS_WINCE //MinMax maybe define struct if not available
    void applyToMinMaxInfo(MINMAXINFO *mmi) const
        { geometryHint.applyToMinMaxInfo(style, exStyle, mmi); }
#endif

    QWindowsGeometryHint geometryHint;
    DWORD style;
    DWORD exStyle;
    QRect requestedGeometry;
    QRect obtainedGeometry;
    QMargins margins;
    int frameX; // Passed on to CreateWindowEx(), including frame.
    int frameY;
    int frameWidth;
    int frameHeight;
};

class QWindowsWindow : public QPlatformWindow
{
public:
#ifdef QT_OPENGL_ES_2
    typedef QSharedPointer<QWindowsEGLStaticContext> QWindowsEGLStaticContextPtr;
#endif

    enum Flags
    {
        AutoMouseCapture = 0x1, //! Automatic mouse capture on button press.
        WithinSetParent = 0x2,
        FrameDirty = 0x4,            //! Frame outdated by setStyle, recalculate in next query.
        OpenGLSurface = 0x10,
        OpenGLDoubleBuffered = 0x20,
        OpenGlPixelFormatInitialized = 0x40,
        BlockedByModal = 0x80,
        SizeGripOperation = 0x100,
        FrameStrutEventsEnabled = 0x200,
        SynchronousGeometryChangeEvent = 0x400,
        WithinSetStyle = 0x800,
        WithinDestroy = 0x1000
    };

    struct WindowData
    {
        WindowData() : hwnd(0) {}

        Qt::WindowFlags flags;
        QRect geometry;
        QMargins frame; // Do not use directly for windows, see FrameDirty.
        HWND hwnd;
        bool embedded;

        static WindowData create(const QWindow *w,
                                 const WindowData &parameters,
                                 const QString &title);
    };

    QWindowsWindow(QWindow *window, const WindowData &data);
    ~QWindowsWindow();

    virtual QSurfaceFormat format() const { return m_format; }
    virtual void setGeometry(const QRect &rect);
    virtual QRect geometry() const { return m_data.geometry; }

    virtual void setVisible(bool visible);
    bool isVisible() const;
    virtual bool isExposed() const { return m_windowState != Qt::WindowMinimized && isVisible(); }
    virtual bool isActive() const;
    virtual bool isEmbedded(const QPlatformWindow *parentWindow) const;
    virtual QPoint mapToGlobal(const QPoint &pos) const;
    virtual QPoint mapFromGlobal(const QPoint &pos) const;

    virtual void setWindowFlags(Qt::WindowFlags flags);
    virtual void setWindowState(Qt::WindowState state);

    HWND handle() const { return m_data.hwnd; }

    virtual WId winId() const { return WId(m_data.hwnd); }
    virtual void setParent(const QPlatformWindow *window);

    virtual void setWindowTitle(const QString &title);
    virtual void raise();
    virtual void lower();

    void windowEvent(QEvent *event);

    virtual void propagateSizeHints();
    virtual QMargins frameMargins() const;

    virtual void setOpacity(qreal level);
    virtual void setMask(const QRegion &region);
    qreal opacity() const { return m_opacity; }
    virtual void requestActivateWindow();

    virtual bool setKeyboardGrabEnabled(bool grab);
    virtual bool setMouseGrabEnabled(bool grab);
    inline bool hasMouseCapture() const { return GetCapture() == m_data.hwnd; }

    virtual bool startSystemResize(const QPoint &pos, Qt::Corner corner);

    void setFrameStrutEventsEnabled(bool enabled);
    bool frameStrutEventsEnabled() const { return testFlag(FrameStrutEventsEnabled); }

#ifdef QT_OPENGL_ES_2
    EGLSurface eglSurfaceHandle() const { return m_eglSurface;}
    EGLSurface ensureEglSurfaceHandle(const QWindowsEGLStaticContextPtr &staticContext, EGLConfig config);
#endif

    inline unsigned style() const
        { return GetWindowLongPtr(m_data.hwnd, GWL_STYLE); }
    void setStyle(unsigned s) const;
    inline unsigned exStyle() const
        { return GetWindowLongPtr(m_data.hwnd, GWL_EXSTYLE); }
    void setExStyle(unsigned s) const;

    bool handleWmPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void handleMoved();
    void handleResized(int wParam);
    void handleShown();
    void handleHidden();

    static inline HWND handleOf(const QWindow *w);
    static inline QWindowsWindow *baseWindowOf(const QWindow *w);
    static QWindow *topLevelOf(QWindow *w);
    static inline void *userDataOf(HWND hwnd);
    static inline void setUserDataOf(HWND hwnd, void *ud);

    static bool setWindowLayered(HWND hwnd, Qt::WindowFlags flags, bool hasAlpha, qreal opacity);

    HDC getDC();
    void releaseDC();
#ifndef Q_OS_WINCE // maybe available on some SDKs revisit WM_GETMINMAXINFO
    void getSizeHints(MINMAXINFO *mmi) const;
#endif

#ifndef QT_NO_CURSOR
    QWindowsWindowCursor cursor() const { return m_cursor; }
#endif
    void setCursor(const QWindowsWindowCursor &c);
    void applyCursor();

    QWindowsWindow *childAt(const QPoint &clientPoint,
                                unsigned cwexflags = CWP_SKIPINVISIBLE) const;
    QWindowsWindow *childAtScreenPoint(const QPoint &screenPoint,
                                           unsigned cwexflags = CWP_SKIPINVISIBLE) const;

    static QByteArray debugWindowFlags(Qt::WindowFlags wf);

    inline bool testFlag(unsigned f) const  { return (m_flags & f) != 0; }
    inline void setFlag(unsigned f) const   { m_flags |= f; }
    inline void clearFlag(unsigned f) const { m_flags &= ~f; }

    void setEnabled(bool enabled);
    bool isEnabled() const;
    void setWindowIcon(const QIcon &icon);

#ifndef Q_OS_WINCE
    void alertWindow(int durationMs = 0);
    void stopAlertWindow();
#endif

private:
    inline void show_sys() const;
    inline void hide_sys() const;
    inline void setGeometry_sys(const QRect &rect) const;
    inline QRect frameGeometry_sys() const;
    inline QRect geometry_sys() const;
    inline WindowData setWindowFlags_sys(Qt::WindowFlags wt, unsigned flags = 0) const;
    inline bool isFullScreen_sys() const;
    inline void setWindowState_sys(Qt::WindowState newState);
    inline void setParent_sys(const QPlatformWindow *parent) const;
    void destroyWindow();
    void registerDropSite();
    void unregisterDropSite();
    void handleGeometryChange();
    void handleWindowStateChange(Qt::WindowState state);
    inline void destroyIcon();

    mutable WindowData m_data;
    mutable unsigned m_flags;
    HDC m_hdc;
    Qt::WindowState m_windowState;
    qreal m_opacity;
#ifndef QT_NO_CURSOR
    QWindowsWindowCursor m_cursor;
#endif
    QWindowsOleDropTarget *m_dropTarget;
    unsigned m_savedStyle;
    QRect m_savedFrameGeometry;
    const QSurfaceFormat m_format;
#ifdef QT_OPENGL_ES_2
    EGLSurface m_eglSurface;
    QSharedPointer<QWindowsEGLStaticContext> m_staticEglContext;
#endif
#ifdef Q_OS_WINCE
    bool m_previouslyHidden;
#endif
    HICON m_iconSmall;
    HICON m_iconBig;
};

// Conveniences for window frames.
inline QRect operator+(const QRect &r, const QMargins &m)
{
    return r.adjusted(-m.left(), -m.top(), m.right(), m.bottom());
}

inline QRect operator-(const QRect &r, const QMargins &m)
{
    return r.adjusted(m.left(), m.top(), -m.right(), -m.bottom());
}

// Debug
QDebug operator<<(QDebug d, const RECT &r);
#ifndef Q_OS_WINCE // maybe available on some SDKs revisit WM_GETMINMAXINFO/WM_NCCALCSIZE
QDebug operator<<(QDebug d, const MINMAXINFO &i);
QDebug operator<<(QDebug d, const NCCALCSIZE_PARAMS &p);
#endif

// ---------- QWindowsGeometryHint inline functions.
QPoint QWindowsGeometryHint::mapToGlobal(HWND hwnd, const QPoint &qp)
{
    POINT p = { qp.x(), qp.y() };
    ClientToScreen(hwnd, &p);
    return QPoint(p.x, p.y);
}

QPoint QWindowsGeometryHint::mapFromGlobal(const HWND hwnd, const QPoint &qp)
{
    POINT p = { qp.x(), qp.y() };
    ScreenToClient(hwnd, &p);
    return QPoint(p.x, p.y);
}

QPoint QWindowsGeometryHint::mapToGlobal(const QWindow *w, const QPoint &p)
    { return QWindowsGeometryHint::mapToGlobal(QWindowsWindow::handleOf(w), p); }

QPoint QWindowsGeometryHint::mapFromGlobal(const QWindow *w, const QPoint &p)
    { return QWindowsGeometryHint::mapFromGlobal(QWindowsWindow::handleOf(w), p); }


// ---------- QWindowsBaseWindow inline functions.

QWindowsWindow *QWindowsWindow::baseWindowOf(const QWindow *w)
{
    if (w)
        if (QPlatformWindow *pw = w->handle())
            return static_cast<QWindowsWindow *>(pw);
    return 0;
}

HWND QWindowsWindow::handleOf(const QWindow *w)
{
    if (const QWindowsWindow *bw = QWindowsWindow::baseWindowOf(w))
        return bw->handle();
    return 0;
}

void *QWindowsWindow::userDataOf(HWND hwnd)
{
    return (void *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

void QWindowsWindow::setUserDataOf(HWND hwnd, void *ud)
{
    SetWindowLongPtr(hwnd, GWLP_USERDATA, LONG_PTR(ud));
}

inline void QWindowsWindow::destroyIcon()
{
    if (m_iconBig) {
        DestroyIcon(m_iconBig);
        m_iconBig = 0;
    }
    if (m_iconSmall) {
        DestroyIcon(m_iconSmall);
        m_iconSmall = 0;
    }
}

QT_END_NAMESPACE

#endif // QWINDOWSWINDOW_H
