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

#include "qtptnginputcontext.h"

#include <QRect>
#include <QLocale>
#include <stdio.h>

QT_BEGIN_NAMESPACE

QTptNgInputContext::QTptNgInputContext()
{
}

QTptNgInputContext::~QTptNgInputContext()
{
}

QPlatformInputContext *QTptNgInputContext::surrogate() const
{
    return m_surrogate;
}

void QTptNgInputContext::setSurrogate(QPlatformInputContext *surrogate)
{
    m_surrogate = surrogate;
}

bool QTptNgInputContext::isValid() const
{
    return m_surrogate ? m_surrogate->isValid() : QPlatformInputContext::isValid();
}

void QTptNgInputContext::reset()
{
    m_surrogate ? m_surrogate->reset() : QPlatformInputContext::reset();
}

void QTptNgInputContext::commit()
{
    m_surrogate ? m_surrogate->commit() : QPlatformInputContext::commit();
}

void QTptNgInputContext::update(Qt::InputMethodQueries queries)
{
    m_surrogate ? m_surrogate->update(queries) : QPlatformInputContext::update(queries);
}

void QTptNgInputContext::invokeAction(QInputMethod::Action action, int cursorPosition)
{
    m_surrogate ? m_surrogate->invokeAction(action, cursorPosition) : QPlatformInputContext::invokeAction(action, cursorPosition);
}

bool QTptNgInputContext::filterEvent(const QEvent *event)
{
    return m_surrogate ? m_surrogate->filterEvent(event) : QPlatformInputContext::filterEvent(event);
}

QRectF QTptNgInputContext::keyboardRect() const
{
    return m_surrogate ? m_surrogate->keyboardRect() : QPlatformInputContext::keyboardRect();
}

bool QTptNgInputContext::isAnimating() const
{
    return m_surrogate ? m_surrogate->isAnimating() : QPlatformInputContext::isAnimating();
}

void QTptNgInputContext::showInputPanel()
{
    m_surrogate ? m_surrogate->showInputPanel() : QPlatformInputContext::showInputPanel();
}

void QTptNgInputContext::hideInputPanel()
{
    m_surrogate ? m_surrogate->hideInputPanel() : QPlatformInputContext::hideInputPanel();
}

bool QTptNgInputContext::isInputPanelVisible() const
{
    return m_surrogate ? m_surrogate->isInputPanelVisible() : QPlatformInputContext::isInputPanelVisible();
}

QLocale QTptNgInputContext::locale() const
{
    return m_surrogate ? m_surrogate->locale() : QPlatformInputContext::locale();
}

Qt::LayoutDirection QTptNgInputContext::inputDirection() const
{
    return m_surrogate ? m_surrogate->inputDirection() : QPlatformInputContext::inputDirection();
}

void QTptNgInputContext::setFocusObject(QObject *object)
{
    m_surrogate ? m_surrogate->setFocusObject(object) : QPlatformInputContext::setFocusObject(object);
}

QT_END_NAMESPACE
