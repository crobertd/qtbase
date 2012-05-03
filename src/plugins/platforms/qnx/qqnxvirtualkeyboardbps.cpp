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

#include "qqnxvirtualkeyboardbps.h"

#include <QDebug>

#include <bps/event.h>
#include <bps/locale.h>
#include <bps/virtualkeyboard.h>

QT_BEGIN_NAMESPACE

QQnxVirtualKeyboardBps::QQnxVirtualKeyboardBps(QObject *parent)
    : QQnxAbstractVirtualKeyboard(parent)
{
    if (locale_request_events(0) != BPS_SUCCESS)
        qWarning("QQNX: Failed to register for locale events");

    if (virtualkeyboard_request_events(0) != BPS_SUCCESS)
        qWarning("QQNX: Failed to register for virtual keyboard events");

    int height = 0;
    if (virtualkeyboard_get_height(&height) != BPS_SUCCESS)
        qWarning("QQNX: Failed to get virtual keyboard height");

    setHeight(height);
}

bool QQnxVirtualKeyboardBps::handleEvent(bps_event_t *event)
{
    const int eventDomain = bps_event_get_domain(event);
    if (eventDomain == locale_get_domain())
        return handleLocaleEvent(event);

    if (eventDomain == virtualkeyboard_get_domain())
        return handleVirtualKeyboardEvent(event);

    return false;
}

bool QQnxVirtualKeyboardBps::showKeyboard()
{
#if defined(QQNXVIRTUALKEYBOARD_DEBUG)
    qDebug() << Q_FUNC_INFO << "current visibility=" << isVisible();
#endif

    virtualkeyboard_show();
    return true;
}

bool QQnxVirtualKeyboardBps::hideKeyboard()
{
#if defined(QQNXVIRTUALKEYBOARD_DEBUG)
    qDebug() << Q_FUNC_INFO << "current visibility=" << isVisible();
#endif

    virtualkeyboard_hide();
    return true;
}

void QQnxVirtualKeyboardBps::applyKeyboardMode(KeyboardMode mode)
{
    virtualkeyboard_layout_t layout = VIRTUALKEYBOARD_LAYOUT_DEFAULT;

    switch (mode) {
    case Url:
        layout = VIRTUALKEYBOARD_LAYOUT_URL;
        break;

    case Email:
        layout = VIRTUALKEYBOARD_LAYOUT_EMAIL;
        break;

    case Web:
        layout = VIRTUALKEYBOARD_LAYOUT_WEB;
        break;

    case NumPunc:
        layout = VIRTUALKEYBOARD_LAYOUT_NUM_PUNC;
        break;

    case Symbol:
        layout = VIRTUALKEYBOARD_LAYOUT_SYMBOL;
        break;

    case Phone:
        layout = VIRTUALKEYBOARD_LAYOUT_PHONE;
        break;

    case Pin:
        layout = VIRTUALKEYBOARD_LAYOUT_PIN;
        break;

    case Default: // fall through
    default:
        layout = VIRTUALKEYBOARD_LAYOUT_DEFAULT;
        break;
    }

#if defined(QQNXVIRTUALKEYBOARD_DEBUG)
    qDebug() << Q_FUNC_INFO << "mode=" << mode;
#endif

    virtualkeyboard_change_options(layout, VIRTUALKEYBOARD_ENTER_DEFAULT);
}

bool QQnxVirtualKeyboardBps::handleLocaleEvent(bps_event_t *event)
{
    if (bps_event_get_code(event) == LOCALE_INFO) {
        const QString language = QString::fromAscii(locale_event_get_language(event));
        const QString country  = QString::fromAscii(locale_event_get_country(event));

        const QLocale newLocale(language + QLatin1Char('_') + country);

#if defined(QQNXVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "current locale" << locale() << "new locale=" << newLocale;
#endif
        setLocale(newLocale);
        return true;
    }

#if defined(QQNXVIRTUALKEYBOARD_DEBUG)
    qDebug() << "QQNX: Unhandled locale event. code=" << bps_event_get_code(event);
#endif

    return false;
}

bool QQnxVirtualKeyboardBps::handleVirtualKeyboardEvent(bps_event_t *event)
{
    switch (bps_event_get_code(event)) {
    case VIRTUALKEYBOARD_EVENT_VISIBLE:
        #if defined(QQNXVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "EVENT VISIBLE: current visibility=" << isVisible();
        #endif

        setVisible(true);
        break;

    case VIRTUALKEYBOARD_EVENT_HIDDEN:
        #if defined(QQNXVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "EVENT HIDDEN: current visibility=" << isVisible();
        #endif

        setVisible(false);
        break;

    case VIRTUALKEYBOARD_EVENT_INFO: {
        const int newHeight = virtualkeyboard_event_get_height(event);

        #if defined(QQNXVIRTUALKEYBOARD_DEBUG)
        qDebug() << Q_FUNC_INFO << "EVENT INFO: current height=" << height() << "new height=" << newHeight;
        #endif

        setHeight(newHeight);
        break;
    }

    default:
        #if defined(QQNXVIRTUALKEYBOARD_DEBUG)
        qDebug() << "QQNX: Unhandled virtual keyboard event. code=" << bps_event_get_code(event);
        #endif

        return false;
    }

    return true;
}

QT_END_NAMESPACE