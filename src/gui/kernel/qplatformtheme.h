/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPLATFORMTHEME_H
#define QPLATFORMTHEME_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QtGlobal>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QMenu;
class QMenuBar;
class QPlatformMenu;
class QPlatformMenuBar;
class QPlatformDialogHelper;
class QVariant;
class QPalette;
class QFont;

class Q_GUI_EXPORT QPlatformTheme
{
public:
    enum ThemeHint {
        TextCursorWidth,
        DropShadow,
        MaximumScrollBarDragDistance,
        ToolButtonStyle,
        ToolBarIconSize,
        ItemViewActivateItemOnSingleClick,
        SystemIconThemeName,
        SystemIconFallbackThemeName,
        IconThemeSearchPaths,
        StyleNames,
        WindowAutoPlacement,
        DialogButtonBoxLayout,
        DialogButtonBoxButtonsHaveIcons,
        UseFullScreenForPopupMenu,
        KeyboardScheme,
        UiEffects
    };

    enum DialogType {
        FileDialog,
        ColorDialog,
        FontDialog
    };

    enum Palette {
        SystemPalette,
        ToolTipPalette,
        ToolButtonPalette,
        ButtonPalette,
        HeaderPalette,
        ComboBoxPalette,
        ItemViewPalette,
        MessageBoxLabelPelette,
        TabBarPalette,
        LabelPalette,
        GroupBoxPalette,
        MenuPalette,
        MenuBarPalette,
        TextEditPalette,
        TextLineEditPalette,
        NPalettes
    };

    enum Font {
        SystemFont,
        MenuFont,
        MenuBarFont,
        MenuItemFont,
        MessageBoxFont,
        LabelFont,
        TipLabelFont,
        StatusBarFont,
        TitleBarFont,
        MdiSubWindowTitleFont,
        DockWidgetTitleFont,
        PushButtonFont,
        ToolButtonFont,
        ItemViewFont,
        ListViewFont,
        HeaderViewFont,
        ListBoxFont,
        ComboMenuItemFont,
        ComboLineEditFont,
        SmallFont,
        MiniFont,
        NFonts
    };

    enum KeyboardSchemes
    {
        WindowsKeyboardScheme,
        MacKeyboardScheme,
        X11KeyboardScheme,
        KdeKeyboardScheme,
        GnomeKeyboardScheme,
        CdeKeyboardScheme
    };

    enum UiEffect
    {
        GeneralUiEffect = 0x1,
        AnimateMenuUiEffect = 0x2,
        FadeMenuUiEffect = 0x4,
        AnimateComboUiEffect = 0x8,
        AnimateTooltipUiEffect = 0x10,
        FadeTooltipUiEffect = 0x20,
        AnimateToolBoxUiEffect = 0x40
    };

    virtual ~QPlatformTheme();

    virtual QPlatformMenu *createPlatformMenu(QMenu *menu = 0) const;
    virtual QPlatformMenuBar *createPlatformMenuBar(QMenuBar *menuBar = 0) const;

    virtual bool usePlatformNativeDialog(DialogType type) const;
    virtual QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const;

    virtual const QPalette *palette(Palette type = SystemPalette) const;

    virtual const QFont *font(Font type = SystemFont) const;

    virtual QVariant themeHint(ThemeHint hint) const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLATFORMTHEME_H