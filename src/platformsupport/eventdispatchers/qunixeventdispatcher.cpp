/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplatformdefs.h"
#include "qcoreapplication.h"
#include "qunixeventdispatcher_qpa_p.h"
#include "private/qguiapplication_p.h"

#include <qpa/qwindowsysteminterface.h>
#include <QtCore/QElapsedTimer>
#include <QtCore/QAtomicInt>
#include <QtCore/QSemaphore>

#include <QtCore/QDebug>

#include <errno.h>

QT_BEGIN_NAMESPACE

QT_USE_NAMESPACE


QUnixEventDispatcherQPA::QUnixEventDispatcherQPA(QObject *parent)
    : QEventDispatcherUNIX(parent)
{ }

QUnixEventDispatcherQPA::~QUnixEventDispatcherQPA()
{ }

bool QUnixEventDispatcherQPA::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    bool didSendEvents = QWindowSystemInterface::sendWindowSystemEvents(this, flags);

    if (QEventDispatcherUNIX::processEvents(flags)) {
        return true;
    }

    return didSendEvents;
}

bool QUnixEventDispatcherQPA::hasPendingEvents()
{
    extern uint qGlobalPostedEventsCount(); // from qapplication.cpp
    return qGlobalPostedEventsCount() || QWindowSystemInterface::windowSystemEventsQueued();
}

void QUnixEventDispatcherQPA::flush()
{
    if(qApp)
        qApp->sendPostedEvents();
}

QT_END_NAMESPACE