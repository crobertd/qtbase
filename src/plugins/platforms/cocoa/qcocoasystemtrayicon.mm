/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2012 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Christoph Schleifenbaum <christoph.schleifenbaum@kdab.com>
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

/****************************************************************************
**
** Copyright (c) 2007-2008, Apple, Inc.
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
**   * Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**   * Redistributions in binary form must reproduce the above copyright notice,
**     this list of conditions and the following disclaimer in the documentation
**     and/or other materials provided with the distribution.
**
**   * Neither the name of Apple, Inc. nor the names of its contributors
**     may be used to endorse or promote products derived from this software
**     without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#define QT_MAC_SYSTEMTRAY_USE_GROWL

#include "qcocoasystemtrayicon.h"
#include <qtemporaryfile.h>
#include <qimagewriter.h>
#include <qdebug.h>

#include "qcocoamenu.h"

#include "qt_mac_p.h"
#include "qcocoahelpers.h"

#import <AppKit/AppKit.h>

QT_USE_NAMESPACE

@class QT_MANGLE_NAMESPACE(QNSMenu);
@class QT_MANGLE_NAMESPACE(QNSImageView);

@interface QT_MANGLE_NAMESPACE(QNSStatusItem) : NSObject {
@public
    QCocoaSystemTrayIcon *systray;
    NSStatusItem *item;
    QCocoaMenu *menu;
    bool menuVisible;
    QIcon icon;
    QT_MANGLE_NAMESPACE(QNSImageView) *imageCell;
}
-(id)initWithSysTray:(QCocoaSystemTrayIcon *)systray;
-(void)dealloc;
-(NSStatusItem*)item;
-(QRectF)geometry;
- (void)triggerSelector:(id)sender button:(Qt::MouseButton)mouseButton;
- (void)doubleClickSelector:(id)sender;
@end

@interface QT_MANGLE_NAMESPACE(QNSImageView) : NSImageView {
    BOOL down;
    QT_MANGLE_NAMESPACE(QNSStatusItem) *parent;
}
-(id)initWithParent:(QT_MANGLE_NAMESPACE(QNSStatusItem)*)myParent;
-(void)menuTrackingDone:(NSNotification*)notification;
-(void)mousePressed:(NSEvent *)mouseEvent button:(Qt::MouseButton)mouseButton;
@end

@interface QT_MANGLE_NAMESPACE(QNSMenu) : NSMenu <NSMenuDelegate> {
    QPlatformMenu *qmenu;
}
-(QPlatformMenu*)menu;
-(id)initWithQMenu:(QPlatformMenu*)qmenu;
@end

QT_BEGIN_NAMESPACE
class QSystemTrayIconSys
{
public:
    QSystemTrayIconSys(QCocoaSystemTrayIcon *sys) {
        item = [[QT_MANGLE_NAMESPACE(QNSStatusItem) alloc] initWithSysTray:sys];
    }
    ~QSystemTrayIconSys() {
        [[[item item] view] setHidden: YES];
        [item release];
    }
    QT_MANGLE_NAMESPACE(QNSStatusItem) *item;
};

void QCocoaSystemTrayIcon::init()
{
    if (!m_sys)
        m_sys = new QSystemTrayIconSys(this);
}

QRect QCocoaSystemTrayIcon::geometry() const
{
    if (!m_sys)
        return QRect();

    const QRectF geom = [m_sys->item geometry];
    if (!geom.isNull())
        return geom.toRect();
    else
        return QRect();
}

void QCocoaSystemTrayIcon::cleanup()
{
    delete m_sys;
    m_sys = 0;
}

void QCocoaSystemTrayIcon::updateIcon(const QIcon &icon)
{
    if (!m_sys)
        return;

    m_sys->item->icon = icon;

    const bool menuVisible = m_sys->item->menu && m_sys->item->menuVisible;

    CGFloat hgt = [[[NSApplication sharedApplication] mainMenu] menuBarHeight];
    const short scale = hgt - 4;

    QPixmap pm = m_sys->item->icon.pixmap(QSize(scale, scale),
                                          menuVisible ? QIcon::Selected : QIcon::Normal);
    if (pm.isNull()) {
        pm = QPixmap(scale, scale);
        pm.fill(Qt::transparent);
    }
    NSImage *nsimage = static_cast<NSImage *>(qt_mac_create_nsimage(pm));
    [(NSImageView*)[[m_sys->item item] view] setImage: nsimage];
    [nsimage release];
}

void QCocoaSystemTrayIcon::updateMenu(QPlatformMenu *menu)
{
    if (!m_sys)
        return;

    m_sys->item->menu = static_cast<QCocoaMenu *>(menu);
    if (menu && [m_sys->item->menu->nsMenu() numberOfItems] > 0) {
        [[m_sys->item item] setHighlightMode:YES];
    } else {
        [[m_sys->item item] setHighlightMode:NO];
    }
}

void QCocoaSystemTrayIcon::updateToolTip(const QString &toolTip)
{
    if (!m_sys)
        return;
    [[[m_sys->item item] view] setToolTip:QCFString::toNSString(toolTip)];
}

bool QCocoaSystemTrayIcon::isSystemTrayAvailable() const
{
    return true;
}

bool QCocoaSystemTrayIcon::supportsMessages() const
{
    return true;
}

void QCocoaSystemTrayIcon::showMessage(const QString &title, const QString &message,
                                       const QIcon& icon, MessageIcon, int)
{
    if (!m_sys)
        return;

#ifdef QT_MAC_SYSTEMTRAY_USE_GROWL
    // Make sure that we have Growl installed on the machine we are running on.
    QCFType<CFURLRef> cfurl;
    OSStatus status = LSGetApplicationForInfo(kLSUnknownType, kLSUnknownCreator,
                                              CFSTR("growlTicket"), kLSRolesAll, 0, &cfurl);
    if (status == kLSApplicationNotFoundErr)
        return;
    QCFType<CFBundleRef> bundle = CFBundleCreate(0, cfurl);

    if (CFStringCompare(CFBundleGetIdentifier(bundle), CFSTR("com.Growl.GrowlHelperApp"),
                kCFCompareCaseInsensitive |  kCFCompareBackwards) != kCFCompareEqualTo)
        return;
    QPixmap notificationIconPixmap = icon.pixmap(32, 32);
    QTemporaryFile notificationIconFile;
    QString notificationType(QLatin1String("Notification")), notificationIcon, notificationApp(qt_mac_applicationName());
    if (notificationApp.isEmpty())
        notificationApp = QLatin1String("Application");
    if (!notificationIconPixmap.isNull() && notificationIconFile.open()) {
        QImageWriter writer(&notificationIconFile, "PNG");
        if (writer.write(notificationIconPixmap.toImage()))
            notificationIcon = QLatin1String("image from location \"file://") + notificationIconFile.fileName() + QLatin1String("\"");
    }
    const QString script(QLatin1String(
        "tell application \"System Events\"\n"
        "set isRunning to (count of (every process whose bundle identifier is \"com.Growl.GrowlHelperApp\")) > 0\n"
        "end tell\n"
        "if isRunning\n"
        "tell application id \"com.Growl.GrowlHelperApp\"\n"
        "-- Make a list of all the notification types (all)\n"
        "set the allNotificationsList to {\"") + notificationType + QLatin1String("\"}\n"

        "-- Make a list of the notifications (enabled)\n"
        "set the enabledNotificationsList to {\"") + notificationType + QLatin1String("\"}\n"

        "-- Register our script with growl.\n"
        "register as application \"") + notificationApp + QLatin1String("\" all notifications allNotificationsList default notifications enabledNotificationsList\n"

        "-- Send a Notification...\n") +
        QLatin1String("notify with name \"") + notificationType +
        QLatin1String("\" title \"") + title +
        QLatin1String("\" description \"") + message +
        QLatin1String("\" application name \"") + notificationApp +
        QLatin1String("\" ")  + notificationIcon +
        QLatin1String("\nend tell\nend if"));
    qt_mac_execute_apple_script(script, 0);
#else
    Q_UNUSED(icon);
    Q_UNUSED(title);
    Q_UNUSED(message);
#endif
}
QT_END_NAMESPACE

@implementation NSStatusItem (Qt)
@end

@implementation QT_MANGLE_NAMESPACE(QNSImageView)
-(id)initWithParent:(QT_MANGLE_NAMESPACE(QNSStatusItem)*)myParent {
    self = [super init];
    parent = myParent;
    down = NO;
    return self;
}

-(void)menuTrackingDone:(NSNotification*)notification
{
    Q_UNUSED(notification);
    down = NO;

    CGFloat hgt = [[[NSApplication sharedApplication] mainMenu] menuBarHeight];
    const short scale = hgt - 4;

    QPixmap pm = parent->icon.pixmap(QSize(scale, scale), QIcon::Normal);
    if (pm.isNull()) {
        pm = QPixmap(scale, scale);
        pm.fill(Qt::transparent);
    }
    NSImage *nsaltimage = static_cast<NSImage *>(qt_mac_create_nsimage(pm));
    [self setImage: nsaltimage];
    [nsaltimage release];

    parent->menuVisible = false;

    [self setNeedsDisplay:YES];
}

-(void)mousePressed:(NSEvent *)mouseEvent button:(Qt::MouseButton)mouseButton
{
    down = YES;
    int clickCount = [mouseEvent clickCount];
    [self setNeedsDisplay:YES];

    CGFloat hgt = [[[NSApplication sharedApplication] mainMenu] menuBarHeight];
    const short scale = hgt - 4;

    QPixmap pm = parent->icon.pixmap(QSize(scale, scale),
                                     parent->menuVisible ? QIcon::Selected : QIcon::Normal);
    if (pm.isNull()) {
        pm = QPixmap(scale, scale);
        pm.fill(Qt::transparent);
    }
    NSImage *nsaltimage = static_cast<NSImage *>(qt_mac_create_nsimage(pm));
    [self setImage: nsaltimage];
    [nsaltimage release];

    if (clickCount == 2) {
        [self menuTrackingDone:nil];
        [parent doubleClickSelector:self];
    } else {
        [parent triggerSelector:self button:mouseButton];
    }
}

-(void)mouseDown:(NSEvent *)mouseEvent
{
    [self mousePressed:mouseEvent button:Qt::LeftButton];
}

-(void)mouseUp:(NSEvent *)mouseEvent
{
    Q_UNUSED(mouseEvent);
    [self menuTrackingDone:nil];
}

- (void)rightMouseDown:(NSEvent *)mouseEvent
{
    [self mousePressed:mouseEvent button:Qt::RightButton];
}

-(void)rightMouseUp:(NSEvent *)mouseEvent
{
    Q_UNUSED(mouseEvent);
    [self menuTrackingDone:nil];
}

- (void)otherMouseDown:(NSEvent *)mouseEvent
{
    [self mousePressed:mouseEvent button:cocoaButton2QtButton([mouseEvent buttonNumber])];
}

-(void)otherMouseUp:(NSEvent *)mouseEvent
{
    Q_UNUSED(mouseEvent);
    [self menuTrackingDone:nil];
}

-(void)drawRect:(NSRect)rect {
    [[parent item] drawStatusBarBackgroundInRect:rect withHighlight:down];
    [super drawRect:rect];
}
@end

@implementation QT_MANGLE_NAMESPACE(QNSStatusItem)

-(id)initWithSysTray:(QCocoaSystemTrayIcon *)sys
{
    self = [super init];
    if (self) {
        item = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength] retain];
        menu = 0;
        menuVisible = false;
        systray = sys;
        imageCell = [[QT_MANGLE_NAMESPACE(QNSImageView) alloc] initWithParent:self];
        [item setView: imageCell];
    }
    return self;
}

-(void)dealloc {
    [[NSStatusBar systemStatusBar] removeStatusItem:item];
    [imageCell release];
    [item release];
    [super dealloc];

}

-(NSStatusItem*)item {
    return item;
}
-(QRectF)geometry {
    if (NSWindow *window = [[item view] window]) {
        NSRect screenRect = [[window screen] frame];
        NSRect windowRect = [window frame];
        return QRectF(windowRect.origin.x, screenRect.size.height-windowRect.origin.y-windowRect.size.height, windowRect.size.width, windowRect.size.height);
    }
    return QRectF();
}

- (void)triggerSelector:(id)sender button:(Qt::MouseButton)mouseButton {
    Q_UNUSED(sender);
    if (!systray)
        return;

    if (mouseButton == Qt::MidButton)
        emit systray->activated(QPlatformSystemTrayIcon::MiddleClick);
    else
        emit systray->activated(QPlatformSystemTrayIcon::Trigger);

    if (menu) {
        NSMenu *m = menu->nsMenu();
        [[NSNotificationCenter defaultCenter] addObserver:imageCell
         selector:@selector(menuTrackingDone:)
             name:NSMenuDidEndTrackingNotification
                 object:m];
        menuVisible = true;
        [item popUpStatusItemMenu: m];
    }
}

- (void)doubleClickSelector:(id)sender {
    Q_UNUSED(sender);
    if (!systray)
        return;
    emit systray->activated(QPlatformSystemTrayIcon::DoubleClick);
}

@end

class QSystemTrayIconQMenu : public QPlatformMenu
{
public:
    void doAboutToShow() { emit aboutToShow(); }
private:
    QSystemTrayIconQMenu();
};

@implementation QT_MANGLE_NAMESPACE(QNSMenu)
-(id)initWithQMenu:(QPlatformMenu*)qm {
    self = [super init];
    if (self) {
        self->qmenu = qm;
        [self setDelegate:self];
    }
    return self;
}
-(QPlatformMenu*)menu {
    return qmenu;
}
@end
