/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef CONTROLS_H
#define CONTROLS_H

#include <QGroupBox>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QRadioButton;
class QButtonGroup;
QT_END_NAMESPACE

enum { ControlLayoutMargin = 4 };

// Control for the hint part of Qt::WindowFlags
class HintControl : public QGroupBox
{
    Q_OBJECT
public:
    explicit HintControl(QWidget *parent= 0);

    Qt::WindowFlags hints() const;
    void setHints(Qt::WindowFlags hints);

signals:
    void changed(Qt::WindowFlags);

private slots:
    void slotCheckBoxChanged();

private:
    QCheckBox *msWindowsFixedSizeDialogCheckBox;
    QCheckBox *x11BypassWindowManagerCheckBox;
    QCheckBox *framelessWindowCheckBox;
    QCheckBox *windowTitleCheckBox;
    QCheckBox *windowSystemMenuCheckBox;
    QCheckBox *windowMinimizeButtonCheckBox;
    QCheckBox *windowMaximizeButtonCheckBox;
    QCheckBox *windowCloseButtonCheckBox;
    QCheckBox *windowContextHelpButtonCheckBox;
    QCheckBox *windowShadeButtonCheckBox;
    QCheckBox *windowStaysOnTopCheckBox;
    QCheckBox *windowStaysOnBottomCheckBox;
    QCheckBox *customizeWindowHintCheckBox;
};

// Control for the Qt::WindowState enum, optional with a "visible" QCheckbox
class WindowStateControl : public QWidget {
    Q_OBJECT
public:
    enum Flags {
        WantVisibleCheckBox = 0x1,
        WantMinimizeRadioButton = 0x2
    };

    explicit WindowStateControl(unsigned flags, QWidget *parent= 0);

    Qt::WindowState state() const;
    void setState(Qt::WindowState s);

    bool visibleValue() const;
    void setVisibleValue(bool);

signals:
    void changed();

private:
    QButtonGroup *group;
    QCheckBox    *visibleCheckBox;
    QRadioButton *restoreButton;
    QRadioButton *minimizeButton;
    QRadioButton *maximizeButton;
    QRadioButton *fullscreenButton;
};

// Control for the Qt::WindowStates flags (normal, maximized, fullscreen exclusively
// combined with minimized and optionally, with a "visible" QCheckbox)
class WindowStatesControl : public QGroupBox
{
    Q_OBJECT
public:
    enum Flags {
        WantVisibleCheckBox = 0x1
    };

    explicit WindowStatesControl(unsigned flags, QWidget *parent= 0);

    Qt::WindowStates states() const;
    void setStates(Qt::WindowStates s);

    bool visibleValue() const;
    void setVisibleValue(bool);

signals:
    void changed();

private:
    QCheckBox *visibleCheckBox;
    QCheckBox *minimizeCheckBox;
    WindowStateControl *stateControl;
};

// Control for the type part of Qt::WindowFlags
class TypeControl : public QGroupBox
{
    Q_OBJECT
public:
    explicit TypeControl(QWidget *parent= 0);

    Qt::WindowFlags type() const;
    void setType(Qt::WindowFlags);

signals:
    void changed(Qt::WindowFlags);

private slots:
    void slotChanged();

private:
    QButtonGroup *group;
    QRadioButton *windowRadioButton;
    QRadioButton *dialogRadioButton;
    QRadioButton *sheetRadioButton;
    QRadioButton *drawerRadioButton;
    QRadioButton *popupRadioButton;
    QRadioButton *toolRadioButton;
    QRadioButton *toolTipRadioButton;
    QRadioButton *splashScreenRadioButton;
};

#endif // CONTROLS_H