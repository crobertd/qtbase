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

#ifndef QWINDOWSINPUTCONTEXT_H
#define QWINDOWSINPUTCONTEXT_H

#include <QPointer>
#include <qpa/qplatforminputcontext.h>

QT_BEGIN_NAMESPACE

class QInputMethodEvent;

class QTptNgInputContext : public QPlatformInputContext
{
    Q_OBJECT

    Q_PROPERTY(QPlatformInputContext *surrogate READ surrogate WRITE setSurrogate)

public:
    explicit QTptNgInputContext();
    ~QTptNgInputContext();

    QPlatformInputContext *surrogate() const;
    void setSurrogate(QPlatformInputContext *surrogate);

    // QPlatformInputContext
    virtual bool isValid() const;

    virtual void reset();
    virtual void commit();
    virtual void update(Qt::InputMethodQueries);
    virtual void invokeAction(QInputMethod::Action, int cursorPosition);
    virtual bool filterEvent(const QEvent *event);
    virtual QRectF keyboardRect() const;

    virtual bool isAnimating() const;

    virtual void showInputPanel();
    virtual void hideInputPanel();
    virtual bool isInputPanelVisible() const;

    virtual QLocale locale() const;
    virtual Qt::LayoutDirection inputDirection() const;

    virtual void setFocusObject(QObject *object);

private:
    QPointer<QPlatformInputContext> m_surrogate;
};

QT_END_NAMESPACE

#endif // QWINDOWSINPUTCONTEXT_H
