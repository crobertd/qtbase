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

#ifndef QGUIAPPLICATION_H
#define QGUIAPPLICATION_H

#include <QtCore/qcoreapplication.h>
#include <QtGui/qwindowdefs.h>
#include <QtGui/qinputmethod.h>
#include <QtCore/qlocale.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QSessionManager;
class QGuiApplicationPrivate;
class QPlatformNativeInterface;
class QPlatformIntegration;
class QPalette;
class QScreen;
class QStyleHints;

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<QGuiApplication *>(QCoreApplication::instance()))

#if defined(qGuiApp)
#undef qGuiApp
#endif
#define qGuiApp (static_cast<QGuiApplication *>(QCoreApplication::instance()))

class Q_GUI_EXPORT QGuiApplication : public QCoreApplication
{
    Q_OBJECT
    Q_PROPERTY(QString applicationDisplayName READ applicationDisplayName WRITE setApplicationDisplayName)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection)
    Q_PROPERTY(QString platformName READ platformName STORED false)
    Q_PROPERTY(bool quitOnLastWindowClosed  READ quitOnLastWindowClosed WRITE setQuitOnLastWindowClosed)

public:
#ifdef Q_QDOC
    QGuiApplication(int &argc, char **argv);
#else
    QGuiApplication(int &argc, char **argv, int = ApplicationFlags);
#endif
    virtual ~QGuiApplication();

    static void setApplicationDisplayName(const QString &name);
    static QString applicationDisplayName();

    static QWindowList allWindows();
    static QWindowList topLevelWindows();
    static QWindow *topLevelAt(const QPoint &pos);

    static QString platformName();

    static QWindow *modalWindow();

    static QWindow *focusWindow();
    static QObject *focusObject();

    static QScreen *primaryScreen();
    static QList<QScreen *> screens();
    qreal devicePixelRatio() const;

#ifndef QT_NO_CURSOR
    static QCursor *overrideCursor();
    static void setOverrideCursor(const QCursor &);
    static void changeOverrideCursor(const QCursor &);
    static void restoreOverrideCursor();
#endif

    static QFont font();
    static void setFont(const QFont &);

#ifndef QT_NO_CLIPBOARD
    static QClipboard *clipboard();
#endif

    static QPalette palette();
    static void setPalette(const QPalette &pal);

    static Qt::KeyboardModifiers keyboardModifiers();
    static Qt::KeyboardModifiers queryKeyboardModifiers();
    static Qt::MouseButtons mouseButtons();

    static void setLayoutDirection(Qt::LayoutDirection direction);
    static Qt::LayoutDirection layoutDirection();

    static inline bool isRightToLeft() { return layoutDirection() == Qt::RightToLeft; }
    static inline bool isLeftToRight() { return layoutDirection() == Qt::LeftToRight; }

    static QStyleHints *styleHints();
    static void setDesktopSettingsAware(bool on);
    static bool desktopSettingsAware();

    static QInputMethod *inputMethod();

    static QPlatformNativeInterface *platformNativeInterface();

    static void setQuitOnLastWindowClosed(bool quit);
    static bool quitOnLastWindowClosed();

    static int exec();
    bool notify(QObject *, QEvent *);

#ifndef QT_NO_SESSIONMANAGER
    // session management
    bool isSessionRestored() const;
    QString sessionId() const;
    QString sessionKey() const;
    bool isSavingSession() const;
#endif

Q_SIGNALS:
    void fontDatabaseChanged();
    void screenAdded(QScreen *screen);
    void lastWindowClosed();
    void focusObjectChanged(QObject *focusObject);
    void focusWindowChanged(QWindow *focusWindow);
#ifndef QT_NO_SESSIONMANAGER
    void commitDataRequest(QSessionManager &sessionManager);
    void saveStateRequest(QSessionManager &sessionManager);
#endif

protected:
    bool event(QEvent *);
    bool compressEvent(QEvent *, QObject *receiver, QPostEventList *);

    QGuiApplication(QGuiApplicationPrivate &p);

private:
    Q_DISABLE_COPY(QGuiApplication)
    Q_DECLARE_PRIVATE(QGuiApplication)

    Q_PRIVATE_SLOT(d_func(), void _q_updateFocusObject(QObject *object))

#ifndef QT_NO_GESTURES
    friend class QGestureManager;
#endif
    friend class QFontDatabasePrivate;
    friend class QPlatformIntegration;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGUIAPPLICATION_H
