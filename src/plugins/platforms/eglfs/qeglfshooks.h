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

#ifndef QEGLFSHOOKS_H
#define QEGLFSHOOKS_H

#include <qpa/qplatformintegration.h>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QImage>
#include <EGL/egl.h>

QT_BEGIN_NAMESPACE

class QEglFSCursor;
class QEglFSScreen;

class QEglFSHooks
{
public:
    virtual ~QEglFSHooks() {};
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
    virtual QEglFSCursor *createCursor(QEglFSScreen *screen) const;
    virtual QPlatformInputContext *inputContext() const;
    virtual QStringList themeNames() const;
    virtual QPlatformTheme *createPlatformTheme(const QString &name) const;
    virtual QVariant styleHint(QPlatformIntegration::StyleHint hint) const;
    virtual void *nativeResourceForIntegration(const QByteArray &resource);
    virtual void *nativeResourceForContext(const QByteArray &resource, QOpenGLContext *context);
};

#ifdef EGLFS_PLATFORM_HOOKS
extern QEglFSHooks *platformHooks;
static QEglFSHooks *hooks = platformHooks;
#else
extern QEglFSHooks stubHooks;
static QEglFSHooks *hooks = &stubHooks;
#endif

QT_END_NAMESPACE

#endif // QEGLFSHOOKS_H
