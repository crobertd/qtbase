/***************************************************************************
**
** Copyright (C) 2012 Research In Motion
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

#include "qqnxbpseventfilter.h"

#include <QAbstractEventDispatcher>
#include <QDebug>

#include <bps/event.h>

QT_BEGIN_NAMESPACE

static QQnxBpsEventFilter *s_instance = 0;

QQnxBpsEventFilter::QQnxBpsEventFilter(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(s_instance == 0);

    s_instance = this;
}

QQnxBpsEventFilter::~QQnxBpsEventFilter()
{
    Q_ASSERT(s_instance == this);

    s_instance = 0;
}

void QQnxBpsEventFilter::installOnEventDispatcher(QAbstractEventDispatcher *dispatcher)
{
#if defined(QQNXBPSEVENTFILTER_DEBUG)
    qDebug() << Q_FUNC_INFO << "dispatcher=" << dispatcher;
#endif

    QAbstractEventDispatcher::EventFilter previousEventFilter = dispatcher->setEventFilter(dispatcherEventFilter);

    // the QPA plugin creates the event dispatcher so we are the first event
    // filter assert on that just in case somebody adds another event filter
    // in the QQnxIntegration constructor instead of adding a new section in here
    Q_ASSERT(previousEventFilter == 0);
    Q_UNUSED(previousEventFilter);
}

bool QQnxBpsEventFilter::dispatcherEventFilter(void *message)
{
#if defined(QQNXBPSEVENTFILTER_DEBUG)
    qDebug() << Q_FUNC_INFO;
#endif

    if (s_instance == 0)
        return false;

    bps_event_t *event = static_cast<bps_event_t *>(message);
    return s_instance->bpsEventFilter(event);
}

bool QQnxBpsEventFilter::bpsEventFilter(bps_event_t *event)
{
#if defined(QQNXBPSEVENTFILTER_DEBUG)
    qDebug() << Q_FUNC_INFO << "event=" << event << "domain=" << bps_event_get_domain(event);
#else
    Q_UNUSED(event);
#endif

    return false;
}

QT_END_NAMESPACE