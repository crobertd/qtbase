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

#include "qwindowsdialoghelpers.h"

#include "qwindowscontext.h"
#include "qwindowswindow.h"
#include "qwindowsintegration.h"
#include "qwindowstheme.h" // Color conversion helpers

#include <QtGui/QGuiApplication>
#include <QtGui/QColor>

#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtCore/QTimer>
#include <QtCore/QDir>
#include <QtCore/QScopedArrayPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QSysInfo>
#include <QtCore/private/qsystemlibrary_p.h>

#include "qtwindows_additional.h"

#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <shlwapi.h>

// #define USE_NATIVE_COLOR_DIALOG /* Testing purposes only */

#ifdef Q_CC_MINGW  /* Add missing declarations for MinGW */

#ifndef __IShellLibrary_FWD_DEFINED__

/* Constants obtained by running the below stream operator for
 * CLSID, IID on the constants in the Windows SDK libraries. */

static const IID   IID_IFileOpenDialog   = {0xd57c7288, 0xd4ad, 0x4768, {0xbe, 0x02, 0x9d, 0x96, 0x95, 0x32, 0xd9, 0x60}};
static const IID   IID_IFileSaveDialog   = {0x84bccd23, 0x5fde, 0x4cdb,{0xae, 0xa4, 0xaf, 0x64, 0xb8, 0x3d, 0x78, 0xab}};
#ifdef __MINGW64_VERSION_MAJOR
static const IID   q_IID_IShellItem      = {0x43826d1e, 0xe718, 0x42ee, {0xbc, 0x55, 0xa1, 0xe2, 0x61, 0xc3, 0x7b, 0xfe}};
#define IID_IShellItem q_IID_IShellItem
#else
static const IID   IID_IShellItem        = {0x43826d1e, 0xe718, 0x42ee, {0xbc, 0x55, 0xa1, 0xe2, 0x61, 0xc3, 0x7b, 0xfe}};
#endif
static const IID   IID_IFileDialogEvents = {0x973510db, 0x7d7f, 0x452b,{0x89, 0x75, 0x74, 0xa8, 0x58, 0x28, 0xd3, 0x54}};
static const CLSID CLSID_FileOpenDialog  = {0xdc1c5a9c, 0xe88a, 0x4dde, {0xa5, 0xa1, 0x60, 0xf8, 0x2a, 0x20, 0xae, 0xf7}};
static const CLSID CLSID_FileSaveDialog  = {0xc0b4e2f3, 0xba21, 0x4773,{0x8d, 0xba, 0x33, 0x5e, 0xc9, 0x46, 0xeb, 0x8b}};

typedef struct _COMDLG_FILTERSPEC
{
    LPCWSTR pszName;
    LPCWSTR pszSpec;
} COMDLG_FILTERSPEC;


#define FOS_OVERWRITEPROMPT        0x2
#define FOS_STRICTFILETYPES        0x4
#define FOS_NOCHANGEDIR        0x8
#define FOS_PICKFOLDERS        0x20
#define FOS_FORCEFILESYSTEM        0x40
#define FOS_ALLNONSTORAGEITEMS 0x80
#define FOS_NOVALIDATE         0x100
#define FOS_ALLOWMULTISELECT   0x200
#define FOS_PATHMUSTEXIST      0x800
#define FOS_FILEMUSTEXIST      0x1000
#define FOS_CREATEPROMPT       0x2000
#define FOS_SHAREAWARE         0x4000
#define FOS_NOREADONLYRETURN   0x8000
#define FOS_NOTESTFILECREATE   0x10000
#define FOS_HIDEMRUPLACES      0x20000
#define FOS_HIDEPINNEDPLACES   0x40000
#define FOS_NODEREFERENCELINKS 0x100000
#define FOS_DONTADDTORECENT    0x2000000
#define FOS_FORCESHOWHIDDEN    0x10000000
#define FOS_DEFAULTNOMINIMODE  0x20000000
#define FOS_FORCEPREVIEWPANEON 0x40000000

#if __MINGW64_VERSION_MAJOR < 2
typedef int GETPROPERTYSTOREFLAGS;
#define GPS_DEFAULT               0x00000000
#define GPS_HANDLERPROPERTIESONLY 0x00000001
#define GPS_READWRITE             0x00000002
#define GPS_TEMPORARY             0x00000004
#define GPS_FASTPROPERTIESONLY    0x00000008
#define GPS_OPENSLOWITEM          0x00000010
#define GPS_DELAYCREATION         0x00000020
#define GPS_BESTEFFORT            0x00000040
#define GPS_MASK_VALID            0x0000007F
#endif

typedef int (QT_WIN_CALLBACK* BFFCALLBACK)(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
// message from browser
#define BFFM_INITIALIZED        1
#define BFFM_SELCHANGED         2
#define BFFM_ENABLEOK           (WM_USER + 101)
// Browsing for directory.
#define BIF_NONEWFOLDERBUTTON  0x0200
#define BIF_NOTRANSLATETARGETS 0x0400
#define BIF_BROWSEFORCOMPUTER  0x1000
#define BIF_BROWSEFORPRINTER   0x2000
#define BIF_BROWSEINCLUDEFILES 0x4000
#define BIF_SHAREABLE          0x8000

//the enums
typedef enum {
    SIATTRIBFLAGS_AND   = 0x1,
    SIATTRIBFLAGS_OR    = 0x2,
    SIATTRIBFLAGS_APPCOMPAT     = 0x3,
    SIATTRIBFLAGS_MASK  = 0x3
}       SIATTRIBFLAGS;
#ifndef __MINGW64_VERSION_MAJOR
typedef enum {
    SIGDN_NORMALDISPLAY = 0x00000000,
    SIGDN_PARENTRELATIVEPARSING = 0x80018001,
    SIGDN_PARENTRELATIVEFORADDRESSBAR = 0x8001c001,
    SIGDN_DESKTOPABSOLUTEPARSING = 0x80028000,
    SIGDN_PARENTRELATIVEEDITING = 0x80031001,
    SIGDN_DESKTOPABSOLUTEEDITING = 0x8004c000,
    SIGDN_FILESYSPATH = 0x80058000,
    SIGDN_URL = 0x80068000
} SIGDN;
#endif
typedef enum {
    FDAP_BOTTOM = 0x00000000,
    FDAP_TOP = 0x00000001
} FDAP;
typedef enum {
    FDESVR_DEFAULT = 0x00000000,
    FDESVR_ACCEPT = 0x00000001,
    FDESVR_REFUSE = 0x00000002
} FDE_SHAREVIOLATION_RESPONSE;
typedef FDE_SHAREVIOLATION_RESPONSE FDE_OVERWRITE_RESPONSE;

//the structs
typedef struct {
    LPCWSTR pszName;
    LPCWSTR pszSpec;
} qt_COMDLG_FILTERSPEC;
typedef struct {
    GUID fmtid;
    DWORD pid;
} qt_PROPERTYKEY;

typedef struct {
    USHORT      cb;
    BYTE        abID[1];
} qt_SHITEMID, *qt_LPSHITEMID;
typedef struct {
    qt_SHITEMID mkid;
} qt_ITEMIDLIST, *qt_LPITEMIDLIST;
typedef const qt_ITEMIDLIST *qt_LPCITEMIDLIST;
typedef struct {
    HWND          hwndOwner;
    qt_LPCITEMIDLIST pidlRoot;
    LPWSTR        pszDisplayName;
    LPCWSTR       lpszTitle;
    UINT          ulFlags;
    BFFCALLBACK   lpfn;
    LPARAM        lParam;
    int           iImage;
} qt_BROWSEINFO;

#endif // __IShellLibrary_FWD_DEFINED__

#ifndef __IFileDialogEvents_FWD_DEFINED__
DECLARE_INTERFACE(IFileDialogEvents);
#endif

#ifndef __IShellItem_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IShellItem, IUnknown)
{
    STDMETHOD(BindToHandler)(THIS_ IBindCtx *pbc, REFGUID bhid, REFIID riid, void **ppv) PURE;
    STDMETHOD(GetParent)(THIS_ IShellItem **ppsi) PURE;
    STDMETHOD(GetDisplayName)(THIS_ SIGDN sigdnName, LPWSTR *ppszName) PURE;
    STDMETHOD(GetAttributes)(THIS_ ULONG sfgaoMask, ULONG *psfgaoAttribs) PURE;
    STDMETHOD(Compare)(THIS_ IShellItem *psi, DWORD hint, int *piOrder) PURE;
};
#endif

#ifndef __IShellItemFilter_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IShellItemFilter, IUnknown)
{
    STDMETHOD(IncludeItem)(THIS_ IShellItem *psi) PURE;
    STDMETHOD(GetEnumFlagsForItem)(THIS_ IShellItem *psi, DWORD *pgrfFlags) PURE;
};
#endif

#ifndef __IShellEnumItems_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IEnumShellItems, IUnknown)
{
    STDMETHOD(Next)(THIS_ ULONG celt, IShellItem **rgelt, ULONG *pceltFetched) PURE;
    STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
    STDMETHOD(Reset)(THIS_) PURE;
    STDMETHOD(Clone)(THIS_ IEnumShellItems **ppenum) PURE;
};
#endif

#ifndef __IShellItemArray_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IShellItemArray, IUnknown)
{
    STDMETHOD(BindToHandler)(THIS_ IBindCtx *pbc, REFGUID rbhid, REFIID riid, void **ppvOut) PURE;
    STDMETHOD(GetPropertyStore)(THIS_ GETPROPERTYSTOREFLAGS flags, REFIID riid, void **ppv) PURE;
    STDMETHOD(GetPropertyDescriptionList)(THIS_ const qt_PROPERTYKEY *keyType, REFIID riid, void **ppv) PURE;
    STDMETHOD(GetAttributes)(THIS_ SIATTRIBFLAGS dwAttribFlags, ULONG sfgaoMask, ULONG *psfgaoAttribs) PURE;
    STDMETHOD(GetCount)(THIS_ DWORD *pdwNumItems) PURE;
    STDMETHOD(GetItemAt)(THIS_ DWORD dwIndex, IShellItem **ppsi) PURE;
    STDMETHOD(EnumItems)(THIS_ IEnumShellItems **ppenumShellItems) PURE;
};
#endif

#ifndef __IModalWindow_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IModalWindow, IUnknown)
{
    STDMETHOD(Show)(THIS_ HWND hwndParent) PURE;
};
#endif

#ifndef __IFileDialog_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IFileDialog, IModalWindow)
{
    STDMETHOD(SetFileTypes)(THIS_ UINT cFileTypes, const COMDLG_FILTERSPEC *rgFilterSpec) PURE;
    STDMETHOD(SetFileTypeIndex)(THIS_ UINT iFileType) PURE;
    STDMETHOD(GetFileTypeIndex)(THIS_ UINT *piFileType) PURE;
    STDMETHOD(Advise)(THIS_ IFileDialogEvents *pfde, DWORD *pdwCookie) PURE;
    STDMETHOD(Unadvise)(THIS_ DWORD dwCookie) PURE;
    STDMETHOD(SetOptions)(THIS_ DWORD fos) PURE;
    STDMETHOD(GetOptions)(THIS_ DWORD *pfos) PURE;
    STDMETHOD(SetDefaultFolder)(THIS_ IShellItem *psi) PURE;
    STDMETHOD(SetFolder)(THIS_ IShellItem *psi) PURE;
    STDMETHOD(GetFolder)(THIS_ IShellItem **ppsi) PURE;
    STDMETHOD(GetCurrentSelection)(THIS_ IShellItem **ppsi) PURE;
    STDMETHOD(SetFileName)(THIS_ LPCWSTR pszName) PURE;
    STDMETHOD(GetFileName)(THIS_ LPWSTR *pszName) PURE;
    STDMETHOD(SetTitle)(THIS_ LPCWSTR pszTitle) PURE;
    STDMETHOD(SetOkButtonLabel)(THIS_ LPCWSTR pszText) PURE;
    STDMETHOD(SetFileNameLabel)(THIS_ LPCWSTR pszLabel) PURE;
    STDMETHOD(GetResult)(THIS_ IShellItem **ppsi) PURE;
    STDMETHOD(AddPlace)(THIS_ IShellItem *psi, FDAP fdap) PURE;
    STDMETHOD(SetDefaultExtension)(THIS_ LPCWSTR pszDefaultExtension) PURE;
    STDMETHOD(Close)(THIS_ HRESULT hr) PURE;
    STDMETHOD(SetClientGuid)(THIS_ REFGUID guid) PURE;
    STDMETHOD(ClearClientData)(THIS_) PURE;
    STDMETHOD(SetFilter)(THIS_ IShellItemFilter *pFilter) PURE;
};
#endif

#ifndef __IFileDialogEvents_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IFileDialogEvents, IUnknown)
{
    STDMETHOD(OnFileOk)(THIS_ IFileDialog *pfd) PURE;
    STDMETHOD(OnFolderChanging)(THIS_ IFileDialog *pfd, IShellItem *psiFolder) PURE;
    STDMETHOD(OnFolderChange)(THIS_ IFileDialog *pfd) PURE;
    STDMETHOD(OnSelectionChange)(THIS_ IFileDialog *pfd) PURE;
    STDMETHOD(OnShareViolation)(THIS_ IFileDialog *pfd, IShellItem *psi, FDE_SHAREVIOLATION_RESPONSE *pResponse) PURE;
    STDMETHOD(OnTypeChange)(THIS_ IFileDialog *pfd) PURE;
    STDMETHOD(OnOverwrite)(THIS_ IFileDialog *pfd, IShellItem *psi, FDE_OVERWRITE_RESPONSE *pResponse) PURE;
};
#endif

#ifndef __IFileOpenDialog_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IFileOpenDialog, IFileDialog)
{
    STDMETHOD(GetResults)(THIS_ IShellItemArray **ppenum) PURE;
    STDMETHOD(GetSelectedItems)(THIS_ IShellItemArray **ppsai) PURE;
};
#endif

#ifndef __IPropertyStore_FWD_DEFINED__
typedef IUnknown IPropertyStore;
#endif

#ifndef __IFileOperationProgressSink_FWD_DEFINED__
typedef IUnknown IFileOperationProgressSink;
#endif

#ifndef __IFileSaveDialog_INTERFACE_DEFINED__
DECLARE_INTERFACE_(IFileSaveDialog, IFileDialog)
{
public:
    STDMETHOD(SetSaveAsItem)(THIS_ IShellItem *psi) PURE;
    STDMETHOD(SetProperties)(THIS_ IPropertyStore *pStore) PURE;
    STDMETHOD(SetCollectedProperties)(THIS_ IPropertyStore *pStore) PURE;
    STDMETHOD(GetProperties)(THIS_ IPropertyStore **ppStore) PURE;
    STDMETHOD(ApplyProperties)(THIS_ IShellItem *psi, IPropertyStore *pStore, HWND hwnd, IFileOperationProgressSink *pSink) PURE;
};
#endif

#endif // Q_CC_MINGW

QT_BEGIN_NAMESPACE

/* Output UID (IID, CLSID) as C++ constants.
 * The constants are contained in the Windows SDK libs, but not for MinGW. */
static inline QString guidToString(const GUID &g)
{
    QString rc;
    QTextStream str(&rc);
    str.setIntegerBase(16);
    str.setNumberFlags(str.numberFlags() | QTextStream::ShowBase);
    str << '{' << g.Data1 << ", " << g.Data2 << ", " << g.Data3;
    str.setFieldWidth(2);
    str.setFieldAlignment(QTextStream::AlignRight);
    str.setPadChar(QLatin1Char('0'));
    str << ",{" << g.Data4[0] << ", " << g.Data4[1]  << ", " << g.Data4[2]  << ", " << g.Data4[3]
        << ", " << g.Data4[4] << ", " << g.Data4[5]  << ", " << g.Data4[6]  << ", " << g.Data4[7]
        << "}};";
    return rc;
}

inline QDebug operator<<(QDebug d, const GUID &g)
{ d.nospace() << guidToString(g); return d; }

namespace QWindowsDialogs
{
/*!
    \fn eatMouseMove()

    After closing a windows dialog with a double click (i.e. open a file)
    the message queue still contains a dubious WM_MOUSEMOVE message where
    the left button is reported to be down (wParam != 0).
    remove all those messages (usually 1) and post the last one with a
    reset button state.

    \ingroup qt-lighthouse-win
*/

void eatMouseMove()
{
    MSG msg = {0, 0, 0, 0, 0, {0, 0} };
    while (PeekMessage(&msg, 0, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
        ;
    if (msg.message == WM_MOUSEMOVE)
        PostMessage(msg.hwnd, msg.message, 0, msg.lParam);
    if (QWindowsContext::verboseDialogs)
        qDebug("%s triggered=%d" , __FUNCTION__, msg.message == WM_MOUSEMOVE);
}

} // namespace QWindowsDialogs

/*!
    \class QWindowsNativeDialogBase
    \brief Base class for Windows native dialogs.

    Base classes for native dialogs (using the CLSID-based
    dialog interfaces "IFileDialog", etc. available from Windows
    Vista on) that mimick the behaviour of their QDialog
    counterparts as close as possible.

    A major difference is that there is only an exec(), which
    is a modal, blocking call; there is no non-blocking show().
    There 2 types of native dialogs:

    \list
    \li Dialogs provided by the Comdlg32 library (ChooseColor,
       ChooseFont). They only provide a modal, blocking
       function call (with idle processing).
    \li File dialogs are classes derived from IFileDialog. They
       inherit IModalWindow and their exec() method (calling
       IModalWindow::Show()) is similarly blocking, but methods
       like close() can be called on them from event handlers.
    \endlist

    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsNativeDialogBase : public QObject
{
    Q_OBJECT
public:
    virtual void setWindowTitle(const QString &title) = 0;
    virtual void exec(HWND owner = 0) = 0;
    virtual QPlatformDialogHelper::DialogCode result() const = 0;

signals:
    void accepted();
    void rejected();

public slots:
    virtual void close() = 0;

protected:
    QWindowsNativeDialogBase() {}
};

/*!
    \class QWindowsDialogHelperBase
    \brief Helper for native Windows dialogs.

    Provides basic functionality and introduces new virtuals.
    The native dialog is created in setVisible_sys() since
    then modality and the state of DontUseNativeDialog is known.

    Modal dialogs are then started via the platformNativeDialogModalHelp(),
    platformNativeDialogModalHelp() slots.
    Non-modal dialogs are shown using a separate thread should
    they support it.

    \sa QWindowsDialogThread
    \internal
    \ingroup qt-lighthouse-win
*/

template <class BaseClass>
QWindowsDialogHelperBase<BaseClass>::QWindowsDialogHelperBase() :
    m_nativeDialog(0),
    m_ownerWindow(0)
{
}

template <class BaseClass>
QWindowsDialogHelperBase<BaseClass>::~QWindowsDialogHelperBase()
{
    delete m_nativeDialog;
}

template <class BaseClass>
QWindowsNativeDialogBase *QWindowsDialogHelperBase<BaseClass>::nativeDialog() const
{
    if (!m_nativeDialog) {
         qWarning("%s invoked with no native dialog present.", __FUNCTION__);
         return 0;
    }
    return m_nativeDialog;
}

template <class BaseClass>
QWindowsNativeDialogBase *QWindowsDialogHelperBase<BaseClass>::ensureNativeDialog()
{
    // Create dialog and apply common settings.
    if (!m_nativeDialog)
        m_nativeDialog = createNativeDialog();
    return m_nativeDialog;
}

/*!
    \class QWindowsDialogThread
    \brief Run a non-modal native dialog in a separate thread.

    \sa QWindowsDialogHelperBase
    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsDialogThread : public QThread
{
public:
    QWindowsDialogThread(QWindowsNativeDialogBase *dialog,
                             HWND owner = 0) :
        m_dialog(dialog), m_owner(owner) {}

    void run();

private:
    QWindowsNativeDialogBase *m_dialog;
    const HWND m_owner;
};

void QWindowsDialogThread::run()
{
    if (QWindowsContext::verboseDialogs)
        qDebug(">%s" , __FUNCTION__);
    m_dialog->exec(m_owner);
    deleteLater();
    if (QWindowsContext::verboseDialogs)
        qDebug("<%s" , __FUNCTION__);
}

template <class BaseClass>
bool QWindowsDialogHelperBase<BaseClass>::show(Qt::WindowFlags,
                                                   Qt::WindowModality windowModality,
                                                   QWindow *parent)
{
    const bool modal = (windowModality == Qt::ApplicationModal);
    if (parent) {
        m_ownerWindow = QWindowsWindow::handleOf(parent);
    } else {
        m_ownerWindow = 0;
    }
    if (QWindowsContext::verboseDialogs)
        qDebug("%s modal=%d native=%p parent=%p" ,
               __FUNCTION__, modal, m_nativeDialog, m_ownerWindow);
    if (!modal && !supportsNonModalDialog())
        return false; // Was it changed in-between?
    if (!ensureNativeDialog())
        return false;
    if (!modal) { // Modal dialogs are shown in separate slot.
        QWindowsDialogThread *thread = new QWindowsDialogThread(m_nativeDialog, m_ownerWindow);
        thread->start();
    }
    return true;
}

template <class BaseClass>
void QWindowsDialogHelperBase<BaseClass>::hide()
{
    if (m_nativeDialog)
        m_nativeDialog->close();
    m_ownerWindow = 0;
}

template <class BaseClass>
void QWindowsDialogHelperBase<BaseClass>::exec()
{
    if (QWindowsContext::verboseDialogs)
        qDebug("%s" , __FUNCTION__);
    if (QWindowsNativeDialogBase *nd = nativeDialog())
         nd->exec(m_ownerWindow);
}

static inline bool snapToDefaultButtonHint()
{
    BOOL snapToDefault = false;
    if (SystemParametersInfo(SPI_GETSNAPTODEFBUTTON, 0, &snapToDefault, 0))
        return snapToDefault;
    return false;
}

template <class BaseClass>
QVariant QWindowsDialogHelperBase<BaseClass>::styleHint(QPlatformDialogHelper::StyleHint hint) const
{
    switch (hint) {
    case QPlatformDialogHelper::SnapToDefaultButton:
        return QVariant(snapToDefaultButtonHint());
    }
    return BaseClass::styleHint(hint);
}

/*!
    \class QWindowsNativeFileDialogEventHandler
    \brief Listens to IFileDialog events and forwards them to QWindowsNativeFileDialogBase

    Events like 'folder change' that have an equivalent signal
    in QFileDialog are forwarded.

    \sa QWindowsNativeFileDialogBase, QWindowsFileDialogHelper
    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsNativeFileDialogBase;

class QWindowsNativeFileDialogEventHandler : public IFileDialogEvents
{
public:
    static IFileDialogEvents *create(QWindowsNativeFileDialogBase *nativeFileDialog);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (riid != IID_IUnknown && riid != IID_IFileDialogEvents) {
            *ppv = NULL;
            return ResultFromScode(E_NOINTERFACE);
        }
        *ppv = this;
        AddRef();
        return NOERROR;
    }

    IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_ref); }

    IFACEMETHODIMP_(ULONG) Release()
    {
        const long ref = InterlockedDecrement(&m_ref);
        if (!ref)
            delete this;
        return ref;
    }

    // IFileDialogEvents methods
    IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; }
    IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; }
    IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *);
    IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; }
    IFACEMETHODIMP OnSelectionChange(IFileDialog *);
    IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; }
    IFACEMETHODIMP OnTypeChange(IFileDialog *);
    IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; }

    QWindowsNativeFileDialogEventHandler(QWindowsNativeFileDialogBase *nativeFileDialog) :
        m_ref(1), m_nativeFileDialog(nativeFileDialog) {}
    ~QWindowsNativeFileDialogEventHandler() {}

private:
    long m_ref;
    QWindowsNativeFileDialogBase *m_nativeFileDialog;
};

IFileDialogEvents *QWindowsNativeFileDialogEventHandler::create(QWindowsNativeFileDialogBase *nativeFileDialog)
{
    IFileDialogEvents *result;
    QWindowsNativeFileDialogEventHandler *eventHandler = new QWindowsNativeFileDialogEventHandler(nativeFileDialog);
    if (FAILED(eventHandler->QueryInterface(IID_IFileDialogEvents, reinterpret_cast<void **>(&result)))) {
        qErrnoWarning("Unable to obtain IFileDialogEvents");
        return 0;
    }
    eventHandler->Release();
    return result;
}

/*!
    \class QWindowsNativeFileDialogBase
    \brief Windows native file dialog wrapper around IFileOpenDialog, IFileSaveDialog.

    Provides convenience methods.
    Note that only IFileOpenDialog has multi-file functionality.

    \sa QWindowsNativeFileDialogEventHandler, QWindowsFileDialogHelper
    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsNativeFileDialogBase : public QWindowsNativeDialogBase
{
    Q_OBJECT
    Q_PROPERTY(bool hideFiltersDetails READ hideFiltersDetails WRITE setHideFiltersDetails)
public:
    ~QWindowsNativeFileDialogBase();

    inline static QWindowsNativeFileDialogBase *create(QFileDialogOptions::AcceptMode am);

    virtual void setWindowTitle(const QString &title);
    inline void setMode(QFileDialogOptions::FileMode mode, QFileDialogOptions::FileDialogOptions options);
    inline void setDirectory(const QString &directory);
    inline QString directory() const;
    virtual void exec(HWND owner = 0);
    inline void setNameFilters(const QStringList &f);
    inline void selectNameFilter(const QString &filter);
    inline QString selectedNameFilter() const;
    void selectFile(const QString &fileName) const;
    bool hideFiltersDetails() const    { return m_hideFiltersDetails; }
    void setHideFiltersDetails(bool h) { m_hideFiltersDetails = h; }
    void setDefaultSuffix(const QString &s);
    inline void setLabelText(QFileDialogOptions::DialogLabel l, const QString &text);

    virtual QPlatformDialogHelper::DialogCode result() const
        { return fileResult(); }
    virtual QPlatformDialogHelper::DialogCode fileResult(QStringList *fileResult = 0) const = 0;
    virtual QStringList selectedFiles() const = 0;

    inline void onFolderChange(IShellItem *);
    inline void onSelectionChange();
    inline void onTypeChange();

signals:
    void directoryEntered(const QString& directory);
    void currentChanged(const QString& file);
    void filterSelected(const QString & filter);

public slots:
    virtual void close() { m_fileDialog->Close(S_OK); }

protected:
    QWindowsNativeFileDialogBase();
    bool init(const CLSID &clsId, const IID &iid);
    inline IFileDialog * fileDialog() const { return m_fileDialog; }
    static QString itemPath(IShellItem *item);
    static int itemPaths(IShellItemArray *items, QStringList *fileResult = 0);
    static IShellItem *shellItem(const QString &path);

private:
    IFileDialog *m_fileDialog;
    IFileDialogEvents *m_dialogEvents;
    DWORD m_cookie;
    QStringList m_nameFilters;
    bool m_hideFiltersDetails;
};

QWindowsNativeFileDialogBase::QWindowsNativeFileDialogBase() :
    m_fileDialog(0), m_dialogEvents(0), m_cookie(0), m_hideFiltersDetails(false)
{
}

QWindowsNativeFileDialogBase::~QWindowsNativeFileDialogBase()
{
    if (m_dialogEvents && m_fileDialog)
        m_fileDialog->Unadvise(m_cookie);
    if (m_dialogEvents)
        m_dialogEvents->Release();
    if (m_fileDialog)
        m_fileDialog->Release();
}

bool QWindowsNativeFileDialogBase::init(const CLSID &clsId, const IID &iid)
{
    HRESULT hr = CoCreateInstance(clsId, NULL, CLSCTX_INPROC_SERVER,
                                  iid, reinterpret_cast<void **>(&m_fileDialog));
    if (FAILED(hr)) {
        qErrnoWarning("CoCreateInstance failed");
        return false;
    }
    m_dialogEvents = QWindowsNativeFileDialogEventHandler::create(this);
    if (!m_dialogEvents)
        return false;
    // Register event handler
    hr = m_fileDialog->Advise(m_dialogEvents, &m_cookie);
    if (FAILED(hr)) {
        qErrnoWarning("IFileDialog::Advise failed");
        return false;
    }
    if (QWindowsContext::verboseDialogs)
        qDebug("%s %p %p cookie=%lu" , __FUNCTION__, m_fileDialog, m_dialogEvents, m_cookie);

    return true;
}

void QWindowsNativeFileDialogBase::setWindowTitle(const QString &title)
{
    m_fileDialog->SetTitle(reinterpret_cast<const wchar_t *>(title.utf16()));
}

IShellItem *QWindowsNativeFileDialogBase::shellItem(const QString &path)
{
#ifndef Q_OS_WINCE
    if (QWindowsContext::shell32dll.sHCreateItemFromParsingName) {
        IShellItem *result = 0;
        const QString native = QDir::toNativeSeparators(path);
        const HRESULT hr =
            QWindowsContext::shell32dll.sHCreateItemFromParsingName(reinterpret_cast<const wchar_t *>(native.utf16()),
                                                                    NULL, IID_IShellItem,
                                                                    reinterpret_cast<void **>(&result));
        if (SUCCEEDED(hr))
            return result;
    }
#endif
    qErrnoWarning("%s: SHCreateItemFromParsingName()) failed", __FUNCTION__);
    return 0;
}

void QWindowsNativeFileDialogBase::setDirectory(const QString &directory)
{
    if (IShellItem *psi = QWindowsNativeFileDialogBase::shellItem(directory)) {
        m_fileDialog->SetFolder(psi);
        psi->Release();
    }
}

QString QWindowsNativeFileDialogBase::directory() const
{
#ifndef Q_OS_WINCE
    IShellItem *item = 0;
    if (m_fileDialog && SUCCEEDED(m_fileDialog->GetFolder(&item)) && item)
        return QWindowsNativeFileDialogBase::itemPath(item);
#endif
    return QString();
}

void QWindowsNativeFileDialogBase::exec(HWND owner)
{
    if (QWindowsContext::verboseDialogs)
        qDebug(">%s on %p", __FUNCTION__, (void *)owner);
    const HRESULT hr = m_fileDialog->Show(owner);
    QWindowsDialogs::eatMouseMove();
    if (QWindowsContext::verboseDialogs)
        qDebug("<%s returns 0x%lx", __FUNCTION__, hr);
    if (hr == S_OK) {
        emit accepted();
    } else {
        emit rejected();
    }
}

void QWindowsNativeFileDialogBase::setMode(QFileDialogOptions::FileMode mode, QFileDialogOptions::FileDialogOptions options)
{
    DWORD flags = FOS_PATHMUSTEXIST | FOS_FORCESHOWHIDDEN;
    if (options & QFileDialogOptions::DontResolveSymlinks)
        flags |= FOS_NODEREFERENCELINKS;
    switch (mode) {
    case QFileDialogOptions::AnyFile:
        flags |= FOS_NOREADONLYRETURN;
        if (!(options & QFileDialogOptions::DontConfirmOverwrite))
            flags |= FOS_OVERWRITEPROMPT;
        break;
    case QFileDialogOptions::ExistingFile:
        flags |= FOS_FILEMUSTEXIST;
        break;
    case QFileDialogOptions::Directory:
    case QFileDialogOptions::DirectoryOnly:
        flags |= FOS_PICKFOLDERS | FOS_FILEMUSTEXIST;
        break;
    case QFileDialogOptions::ExistingFiles:
        flags |= FOS_FILEMUSTEXIST | FOS_ALLOWMULTISELECT;
        break;
    }
    if (QWindowsContext::verboseDialogs)
        qDebug().nospace()
            << __FUNCTION__ << " mode=" << mode << " options"
            << options << " results in 0x" << flags;

    if (FAILED(m_fileDialog->SetOptions(flags)))
        qErrnoWarning("%s: SetOptions() failed", __FUNCTION__);
}

QString QWindowsNativeFileDialogBase::itemPath(IShellItem *item)
{
    QString result;
    LPWSTR name = 0;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &name))) {
        result = QDir::cleanPath(QString::fromWCharArray(name));
        CoTaskMemFree(name);
    }
    return result;
}

int QWindowsNativeFileDialogBase::itemPaths(IShellItemArray *items,
                                            QStringList *result /* = 0 */)
{
    DWORD itemCount = 0;
    if (result)
        result->clear();
    if (FAILED(items->GetCount(&itemCount)))
        return 0;
    if (result && itemCount) {
        result->reserve(itemCount);
        for (DWORD i = 0; i < itemCount; ++i) {
            IShellItem *item = 0;
            if (SUCCEEDED(items->GetItemAt(i, &item)))
                result->push_back(QWindowsNativeFileDialogBase::itemPath(item));
        }
   }
    return itemCount;
}

// Split a list of name filters into description and actual filters
struct FilterSpec
{
    QString description;
    QString filter;
};

static QList<FilterSpec> filterSpecs(const QStringList &filters,
                                     bool hideFilterDetails,
                                     int *totalStringLength)
{
    QList<FilterSpec> result;
    result.reserve(filters.size());
    *totalStringLength = 0;

    const QRegExp filterSeparatorRE(QStringLiteral("[;\\s]+"));
    const QString separator = QStringLiteral(";");
    Q_ASSERT(filterSeparatorRE.isValid());
    // Split filter specification as 'Texts (*.txt[;] *.doc)'
    // into description and filters specification as '*.txt;*.doc'
    foreach (const QString &filterString, filters) {
        const int openingParenPos = filterString.lastIndexOf(QLatin1Char('('));
        const int closingParenPos = openingParenPos != -1 ?
            filterString.indexOf(QLatin1Char(')'), openingParenPos + 1) : -1;
        FilterSpec filterSpec;
        filterSpec.filter = closingParenPos == -1 ?
            QString(QLatin1Char('*')) :
            filterString.mid(openingParenPos + 1, closingParenPos - openingParenPos - 1).trimmed();
        filterSpec.filter.replace(filterSeparatorRE, separator);
        filterSpec.description = filterString;
        if (hideFilterDetails && openingParenPos != -1) { // Do not show pattern in description
            filterSpec.description.truncate(openingParenPos);
            while (filterSpec.description.endsWith(QLatin1Char(' ')))
                filterSpec.description.truncate(filterSpec.description.size() - 1);
        }
        *totalStringLength += filterSpec.filter.size() + filterSpec.description.size();
        result.push_back(filterSpec);
    }
    return result;
}

void QWindowsNativeFileDialogBase::setNameFilters(const QStringList &filters)
{
    /* Populates an array of COMDLG_FILTERSPEC from list of filters,
     * store the strings in a flat, contiguous buffer. */
    m_nameFilters = filters;
    int totalStringLength = 0;
    const QList<FilterSpec> specs = filterSpecs(filters, m_hideFiltersDetails, &totalStringLength);
    const int size = specs.size();

    QScopedArrayPointer<WCHAR> buffer(new WCHAR[totalStringLength + 2 * size]);
    QScopedArrayPointer<COMDLG_FILTERSPEC> comFilterSpec(new COMDLG_FILTERSPEC[size]);

    const QString matchesAll = QStringLiteral(" (*)");
    WCHAR *ptr = buffer.data();
    // Split filter specification as 'Texts (*.txt[;] *.doc)'
    // into description and filters specification as '*.txt;*.doc'

    for (int i = 0; i < size; ++i) {
        // Display glitch (CLSID only): 'All files (*)' shows up as 'All files (*) (*)'
        QString description = specs[i].description;
        if (!m_hideFiltersDetails && description.endsWith(matchesAll))
            description.truncate(description.size() - matchesAll.size());
        // Add to buffer.
        comFilterSpec[i].pszName = ptr;
        ptr += description.toWCharArray(ptr);
        *ptr++ = 0;
        comFilterSpec[i].pszSpec = ptr;
        ptr += specs[i].filter.toWCharArray(ptr);
        *ptr++ = 0;
    }

    m_fileDialog->SetFileTypes(size, comFilterSpec.data());
}

void QWindowsNativeFileDialogBase::setDefaultSuffix(const QString &s)
{
    wchar_t *wSuffix = const_cast<wchar_t *>(reinterpret_cast<const wchar_t *>(s.utf16()));
    m_fileDialog->SetDefaultExtension(wSuffix);
}

void QWindowsNativeFileDialogBase::setLabelText(QFileDialogOptions::DialogLabel l, const QString &text)
{
    wchar_t *wText = const_cast<wchar_t *>(reinterpret_cast<const wchar_t *>(text.utf16()));
    switch (l) {
        break;
    case QFileDialogOptions::FileName:
        m_fileDialog->SetFileNameLabel(wText);
        break;
    case QFileDialogOptions::Accept:
        m_fileDialog->SetOkButtonLabel(wText);
        break;
    case QFileDialogOptions::LookIn:
    case QFileDialogOptions::Reject:
    case QFileDialogOptions::FileType:
    case QFileDialogOptions::DialogLabelCount:
        break;
    }
}

void QWindowsNativeFileDialogBase::selectFile(const QString &fileName) const
{
    m_fileDialog->SetFileName((wchar_t*)fileName.utf16());
}

// Return the index of the selected filter, accounting for QFileDialog
// sometimes stripping the filter specification depending on the
// hideFilterDetails setting.
static int indexOfNameFilter(const QStringList &filters, const QString &needle)
{
    const int index = filters.indexOf(needle);
    if (index >= 0)
        return index;
    for (int i = 0; i < filters.size(); ++i)
        if (filters.at(i).startsWith(needle))
            return i;
    return -1;
}

void QWindowsNativeFileDialogBase::selectNameFilter(const QString &filter)
{
    const int index = indexOfNameFilter(m_nameFilters, filter);
    if (index >= 0) {
        m_fileDialog->SetFileTypeIndex(index + 1); // one-based.
    } else {
        qWarning("%s: Invalid parameter '%s' not found in '%s'.",
                 __FUNCTION__, qPrintable(filter),
                 qPrintable(m_nameFilters.join(QStringLiteral(", "))));
    }
}

QString QWindowsNativeFileDialogBase::selectedNameFilter() const
{
    UINT uIndex = 0;
    if (SUCCEEDED(m_fileDialog->GetFileTypeIndex(&uIndex))) {
        const int index = uIndex - 1; // one-based
        if (index < m_nameFilters.size())
            return m_nameFilters.at(index);
    }
    return QString();
}

void QWindowsNativeFileDialogBase::onFolderChange(IShellItem *item)
{
    if (item) {
        const QString directory = QWindowsNativeFileDialogBase::itemPath(item);
        emit directoryEntered(directory);
    }
}

void QWindowsNativeFileDialogBase::onSelectionChange()
{
    const QStringList current = selectedFiles();
    if (current.size() == 1)
        emit currentChanged(current.front());
}

void QWindowsNativeFileDialogBase::onTypeChange()
{
    emit filterSelected(selectedNameFilter());
}

HRESULT QWindowsNativeFileDialogEventHandler::OnFolderChanging(IFileDialog *, IShellItem *item)
{
    m_nativeFileDialog->onFolderChange(item);
    return S_OK;
}

HRESULT QWindowsNativeFileDialogEventHandler::OnSelectionChange(IFileDialog *)
{
    m_nativeFileDialog->onSelectionChange();
    return S_OK;
}

HRESULT QWindowsNativeFileDialogEventHandler::OnTypeChange(IFileDialog *)
{
    m_nativeFileDialog->onTypeChange();
    return S_OK;
}

/*!
    \class QWindowsNativeSaveFileDialog
    \brief Windows native file save dialog wrapper around IFileSaveDialog.

    Implements single-selection methods.

    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsNativeSaveFileDialog : public QWindowsNativeFileDialogBase
{
public:
    virtual QPlatformDialogHelper::DialogCode fileResult(QStringList *fileResult = 0) const;
    virtual QStringList selectedFiles() const;
};

// Append a suffix from the name filter "Foo files (*.foo;*.bar)"
// unless the file name already has one.
static inline QString appendSuffix(const QString &fileName, const QString &filter)
{
    const int lastDot = fileName.lastIndexOf(QLatin1Char('.'));
    const int lastSlash = fileName.lastIndexOf(QLatin1Char('/'));
    if (lastDot >= 0 && (lastSlash == -1 || lastDot > lastSlash))
        return fileName;
    int suffixPos = filter.indexOf(QLatin1String("(*."));
    if (suffixPos < 0)
        return fileName;
    suffixPos += 3;
    int endPos = filter.indexOf(QLatin1Char(' '), suffixPos + 1);
    if (endPos < 0)
        endPos = filter.indexOf(QLatin1Char(';'), suffixPos + 1);
    if (endPos < 0)
        endPos = filter.indexOf(QLatin1Char(')'), suffixPos + 1);
    if (endPos < 0)
        return fileName;
    return fileName + QLatin1Char('.') + filter.mid(suffixPos, endPos - suffixPos);
}

QPlatformDialogHelper::DialogCode QWindowsNativeSaveFileDialog::fileResult(QStringList *result /* = 0 */) const
{
    if (result)
        result->clear();
    IShellItem *item = 0;
    const HRESULT hr = fileDialog()->GetResult(&item);
    if (FAILED(hr) || !item)
        return QPlatformDialogHelper::Rejected;
    if (result)
        result->push_back(appendSuffix(QWindowsNativeFileDialogBase::itemPath(item), selectedNameFilter()));
    return QPlatformDialogHelper::Accepted;
}

QStringList QWindowsNativeSaveFileDialog::selectedFiles() const
{
    QStringList result;
    IShellItem *item = 0;
    const HRESULT hr = fileDialog()->GetCurrentSelection(&item);
    if (SUCCEEDED(hr) && item)
        result.push_back(QWindowsNativeSaveFileDialog::itemPath(item));
    return result;
}

/*!
    \class QWindowsNativeOpenFileDialog
    \brief Windows native file save dialog wrapper around IFileOpenDialog.

    Implements multi-selection methods.

    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsNativeOpenFileDialog : public QWindowsNativeFileDialogBase
{
public:
    virtual QPlatformDialogHelper::DialogCode fileResult(QStringList *fileResult = 0) const;
    virtual QStringList selectedFiles() const;

private:
    inline IFileOpenDialog *openFileDialog() const
        { return static_cast<IFileOpenDialog *>(fileDialog()); }
};

QPlatformDialogHelper::DialogCode QWindowsNativeOpenFileDialog::fileResult(QStringList *result /* = 0 */) const
{
    if (result)
        result->clear();
    IShellItemArray *items = 0;
    const HRESULT hr = openFileDialog()->GetResults(&items);
    if (SUCCEEDED(hr) && items && QWindowsNativeFileDialogBase::itemPaths(items, result) > 0)
        return QPlatformDialogHelper::Accepted;
    return QPlatformDialogHelper::Rejected;
}

QStringList QWindowsNativeOpenFileDialog::selectedFiles() const
{
    QStringList result;
    IShellItemArray *items = 0;
    const HRESULT hr = openFileDialog()->GetSelectedItems(&items);
    if (SUCCEEDED(hr) && items)
        QWindowsNativeFileDialogBase::itemPaths(items, &result);
    return result;
}

/*!
    \brief Factory method for QWindowsNativeFileDialogBase returning
    QWindowsNativeOpenFileDialog or QWindowsNativeSaveFileDialog depending on
    QFileDialog::AcceptMode.
*/

QWindowsNativeFileDialogBase *QWindowsNativeFileDialogBase::create(QFileDialogOptions::AcceptMode am)
{
    QWindowsNativeFileDialogBase *result = 0;
    if (am == QFileDialogOptions::AcceptOpen) {
        result = new QWindowsNativeOpenFileDialog;
        if (!result->init(CLSID_FileOpenDialog, IID_IFileOpenDialog)) {
            delete result;
            return 0;
        }
    } else {
        result = new QWindowsNativeSaveFileDialog;
        if (!result->init(CLSID_FileSaveDialog, IID_IFileSaveDialog)) {
            delete result;
            return 0;
        }
    }
    return result;
}

/*!
    \class QWindowsFileDialogHelper
    \brief Helper for native Windows file dialogs

    Non-modal dialogs are disabled for now. The functionality is
    implemented in principle, however there are failures
    when querying the results from a dialog run in another thread.
    This could probably be fixed be calling CoInitializeEx() with
    the right parameters from each thread. The problem is though
    that calls to CoInitialize() occur in several places in Qt.

    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsFileDialogHelper : public QWindowsDialogHelperBase<QPlatformFileDialogHelper>
{
public:
    QWindowsFileDialogHelper() {}
    virtual bool supportsNonModalDialog() const { return false; }

    virtual bool defaultNameFilterDisables() const
        { return true; }
    virtual void setDirectory(const QString &directory);
    virtual QString directory() const;
    virtual void selectFile(const QString &filename);
    virtual QStringList selectedFiles() const;
    virtual void setFilter();
    virtual void setNameFilters(const QStringList &filters);
    virtual void selectNameFilter(const QString &filter);
    virtual QString selectedNameFilter() const;

private:
    virtual QWindowsNativeDialogBase *createNativeDialog();
    inline QWindowsNativeFileDialogBase *nativeFileDialog() const
        { return static_cast<QWindowsNativeFileDialogBase *>(nativeDialog()); }
};

QWindowsNativeDialogBase *QWindowsFileDialogHelper::createNativeDialog()
{
    QWindowsNativeFileDialogBase *result = QWindowsNativeFileDialogBase::create(options()->acceptMode());
    if (!result)
        return 0;
    QObject::connect(result, SIGNAL(accepted()), this, SIGNAL(accept()));
    QObject::connect(result, SIGNAL(rejected()), this, SIGNAL(reject()));
    QObject::connect(result, SIGNAL(directoryEntered(QString)),
                     this, SIGNAL(directoryEntered(QString)));
    QObject::connect(result, SIGNAL(currentChanged(QString)),
                     this, SIGNAL(currentChanged(QString)));
    QObject::connect(result, SIGNAL(filterSelected(QString)),
                     this, SIGNAL(filterSelected(QString)));

    // Apply settings.
    const QSharedPointer<QFileDialogOptions> &opts = options();
    result->setWindowTitle(opts->windowTitle());
    result->setMode(opts->fileMode(), opts->options());
    result->setHideFiltersDetails(opts->testOption(QFileDialogOptions::HideNameFilterDetails));
    const QStringList nameFilters = opts->nameFilters();
    if (!nameFilters.isEmpty())
        result->setNameFilters(nameFilters);
    if (opts->isLabelExplicitlySet(QFileDialogOptions::FileName))
        result->setLabelText(QFileDialogOptions::FileName, opts->labelText(QFileDialogOptions::FileName));
    if (opts->isLabelExplicitlySet(QFileDialogOptions::Accept))
        result->setLabelText(QFileDialogOptions::Accept, opts->labelText(QFileDialogOptions::Accept));
    const QString initialDirectory = opts->initialDirectory();
    if (!initialDirectory.isEmpty())
        result->setDirectory(initialDirectory);
    const QStringList initialSelection = opts->initiallySelectedFiles();
    if (initialSelection.size() > 0) {
        QFileInfo info(initialSelection.front());
        if (!info.isDir())
            result->selectFile(info.fileName());
    }
    const QString initialNameFilter = opts->initiallySelectedNameFilter();
    if (!initialNameFilter.isEmpty())
        result->selectNameFilter(initialNameFilter);
    const QString defaultSuffix = opts->defaultSuffix();
    if (!defaultSuffix.isEmpty())
        result->setDefaultSuffix(defaultSuffix);
    return result;
}

void QWindowsFileDialogHelper::setDirectory(const QString &directory)
{
    if (QWindowsContext::verboseDialogs)
        qDebug("%s %s" , __FUNCTION__, qPrintable(directory));

    if (QWindowsNativeFileDialogBase *nfd = nativeFileDialog())
        nfd->setDirectory(directory);
}

QString QWindowsFileDialogHelper::directory() const
{
    if (const QWindowsNativeFileDialogBase *nfd = nativeFileDialog())
        return nfd->directory();
    return QString();
}

void QWindowsFileDialogHelper::selectFile(const QString &fileName)
{
    if (QWindowsContext::verboseDialogs)
        qDebug("%s %s" , __FUNCTION__, qPrintable(fileName));

    if (QWindowsNativeFileDialogBase *nfd = nativeFileDialog())
        nfd->selectFile(fileName);
}

QStringList QWindowsFileDialogHelper::selectedFiles() const
{
    QStringList files;
    if (const QWindowsNativeFileDialogBase *nfd = nativeFileDialog())
        nfd->fileResult(&files);
    if (QWindowsContext::verboseDialogs)
        qDebug("%s files='%s'" , __FUNCTION__,
               qPrintable(files.join(QStringLiteral(", "))));
    return files;
}

void QWindowsFileDialogHelper::setFilter()
{
    if (QWindowsContext::verboseDialogs)
        qDebug("%s" , __FUNCTION__);
}

void QWindowsFileDialogHelper::setNameFilters(const QStringList &filters)
{
    if (QWindowsContext::verboseDialogs)
        qDebug("%s" , __FUNCTION__);
    if (QWindowsNativeFileDialogBase *nfd = nativeFileDialog())
        nfd->setNameFilters(filters);
}

void QWindowsFileDialogHelper::selectNameFilter(const QString &filter)
{
    if (QWindowsNativeFileDialogBase *nfd = nativeFileDialog())
        nfd->selectNameFilter(filter);
}

QString QWindowsFileDialogHelper::selectedNameFilter() const
{
    if (const QWindowsNativeFileDialogBase *nfd = nativeFileDialog())
        return nfd->selectedNameFilter();
    return QString();
}

#ifndef Q_OS_WINCE

/*!
    \class QWindowsXpNativeFileDialog
    \brief Native Windows directory dialog for Windows XP using SHlib-functions.

    Uses the synchronous GetOpenFileNameW(), GetSaveFileNameW() from ComDlg32
    or SHBrowseForFolder() for directories.

    \internal
    \sa QWindowsXpFileDialogHelper

    \ingroup qt-lighthouse-win
*/

class QWindowsXpNativeFileDialog : public QWindowsNativeDialogBase
{
    Q_OBJECT
public:
    typedef QSharedPointer<QFileDialogOptions> OptionsPtr;

    static QWindowsXpNativeFileDialog *create(const OptionsPtr &options);

    virtual void setWindowTitle(const QString &t) { m_title =  t; }
    virtual void exec(HWND owner = 0);
    virtual QPlatformDialogHelper::DialogCode result() const { return m_result; }

    void setDirectory(const QString &d) { m_directory = d; }
    QString directory() const { return m_directory; }
    void selectFile(const QString &f) { m_initialFile = f; }
    QStringList selectedFiles() const { return m_selectedFiles; }
    void setNameFilters(const QStringList &n) { m_nameFilters = n; }
    void selectNameFilter(const QString &f);
    QString selectedNameFilter() const { return m_selectedNameFilter; }

    int existingDirCallback(HWND hwnd, UINT uMsg, LPARAM lParam);

public slots:
    virtual void close() {}

private:
    typedef BOOL (APIENTRY *PtrGetOpenFileNameW)(LPOPENFILENAMEW);
    typedef BOOL (APIENTRY *PtrGetSaveFileNameW)(LPOPENFILENAMEW);

    explicit QWindowsXpNativeFileDialog(const OptionsPtr &options);
    void populateOpenFileName(OPENFILENAME *ofn, HWND owner) const;
    QStringList execExistingDir(HWND owner);
    QStringList execFileNames(HWND owner, int *selectedFilterIndex) const;

    const OptionsPtr m_options;
    QString m_title;
    QString m_directory;
    QString m_initialFile;
    QStringList m_selectedFiles;
    QString m_selectedNameFilter;
    QStringList m_nameFilters;
    QPlatformDialogHelper::DialogCode m_result;

    static PtrGetOpenFileNameW m_getOpenFileNameW;
    static PtrGetSaveFileNameW m_getSaveFileNameW;
};

QWindowsXpNativeFileDialog::PtrGetOpenFileNameW QWindowsXpNativeFileDialog::m_getOpenFileNameW = 0;
QWindowsXpNativeFileDialog::PtrGetSaveFileNameW QWindowsXpNativeFileDialog::m_getSaveFileNameW = 0;

QWindowsXpNativeFileDialog *QWindowsXpNativeFileDialog::create(const OptionsPtr &options)
{
    // GetOpenFileNameW() GetSaveFileName() are resolved
    // dynamically as not to create a dependency on Comdlg32, which
    // is used on XP only.
    if (!m_getOpenFileNameW) {
        QSystemLibrary library(QStringLiteral("Comdlg32"));
        m_getOpenFileNameW = (PtrGetOpenFileNameW)(library.resolve("GetOpenFileNameW"));
        m_getSaveFileNameW = (PtrGetSaveFileNameW)(library.resolve("GetSaveFileNameW"));
    }
    if (m_getOpenFileNameW && m_getSaveFileNameW)
        return new QWindowsXpNativeFileDialog(options);
    return 0;
}

QWindowsXpNativeFileDialog::QWindowsXpNativeFileDialog(const OptionsPtr &options) :
    m_options(options), m_result(QPlatformDialogHelper::Rejected)
{
    const QStringList nameFilters = m_options->nameFilters();
    if (!nameFilters.isEmpty())
        setNameFilters(nameFilters);
    const QString initialDirectory = m_options->initialDirectory();
    if (!initialDirectory.isEmpty())
        setDirectory(initialDirectory);
    const QString initialNameFilter = m_options->initiallySelectedNameFilter();
    if (!initialNameFilter.isEmpty())
        selectNameFilter(initialNameFilter);
    const QStringList selectedFiles = m_options->initiallySelectedFiles();
    if (!selectedFiles.isEmpty())
        selectFile(selectedFiles.front());
    setWindowTitle(m_options->windowTitle());
}

void QWindowsXpNativeFileDialog::selectNameFilter(const QString &f)
{
    const int index = indexOfNameFilter(m_nameFilters, f);
    if (index >= 0)
        m_selectedNameFilter = m_nameFilters.at(index);
}

void QWindowsXpNativeFileDialog::exec(HWND owner)
{
    int selectedFilterIndex = -1;
    m_selectedFiles = m_options->fileMode() == QFileDialogOptions::DirectoryOnly ?
        execExistingDir(owner) : execFileNames(owner, &selectedFilterIndex);
    QWindowsDialogs::eatMouseMove();
    if (m_selectedFiles.isEmpty()) {
        m_result = QPlatformDialogHelper::Rejected;
        emit rejected();
    } else {
        if (selectedFilterIndex >= 0 && selectedFilterIndex < m_nameFilters.size()) {
            m_selectedNameFilter = m_nameFilters.at(selectedFilterIndex);
        } else {
            m_selectedNameFilter.clear();
        }
        m_directory = QFileInfo(m_selectedFiles.front()).absolutePath();
        m_result = QPlatformDialogHelper::Accepted;
        emit accepted();
    }
}

// Callback for QWindowsNativeXpFileDialog directory dialog.
// MFC Directory Dialog. Contrib: Steve Williams (minor parts from Scott Powers)

static int CALLBACK xpFileDialogGetExistingDirCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    QWindowsXpNativeFileDialog *dialog = reinterpret_cast<QWindowsXpNativeFileDialog *>(lpData);
    return dialog->existingDirCallback(hwnd, uMsg, lParam);
}

#ifdef Q_CC_MINGW
typedef ITEMIDLIST *qt_LpItemIdList;
#else
typedef PIDLIST_ABSOLUTE qt_LpItemIdList;
#endif

int QWindowsXpNativeFileDialog::existingDirCallback(HWND hwnd, UINT uMsg, LPARAM lParam)
{
    switch (uMsg) {
    case BFFM_INITIALIZED:
        if (!m_initialFile.isEmpty())
            SendMessage(hwnd, BFFM_SETSELECTION, TRUE, LPARAM(m_initialFile.utf16()));
        break;
    case BFFM_SELCHANGED: {
        wchar_t path[MAX_PATH];
        const bool ok = SHGetPathFromIDList(reinterpret_cast<qt_LpItemIdList>(lParam), path)
                        && path[0];
        SendMessage(hwnd, BFFM_ENABLEOK, ok ? 1 : 0, 1);
    }
        break;
    }
    return 0;
}

QStringList QWindowsXpNativeFileDialog::execExistingDir(HWND owner)
{
    BROWSEINFO bi;
    wchar_t initPath[MAX_PATH];
    initPath[0] = 0;
    bi.hwndOwner = owner;
    bi.pidlRoot = NULL;
    //### This does not seem to be respected? - the dialog always displays "Browse for folder"
    bi.lpszTitle = (wchar_t*)m_title.utf16();
    bi.pszDisplayName = initPath;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;
    bi.lpfn = xpFileDialogGetExistingDirCallbackProc;
    bi.lParam = LPARAM(this);
    QStringList selectedFiles;
    if (qt_LpItemIdList pItemIDList = SHBrowseForFolder(&bi)) {
        wchar_t path[MAX_PATH];
        path[0] = 0;
        if (SHGetPathFromIDList(pItemIDList, path) && path[0])
            selectedFiles.push_back(QDir::cleanPath(QString::fromWCharArray(path)));
        IMalloc *pMalloc;
        if (SHGetMalloc(&pMalloc) == NOERROR) {
            pMalloc->Free(pItemIDList);
            pMalloc->Release();
        }
    }
    return selectedFiles;
}

// Return an allocated wchar_t array from a QString, reserve more memory if desired.
static wchar_t *qStringToWCharArray(const QString &s, size_t reserveSize = 0)
{
    const size_t stringSize = s.size();
    wchar_t *result = new wchar_t[qMax(stringSize + 1, reserveSize)];
    s.toWCharArray(result);
    result[stringSize] = 0;
    return result;
}

// Open/Save files
void QWindowsXpNativeFileDialog::populateOpenFileName(OPENFILENAME *ofn, HWND owner) const
{
    ZeroMemory(ofn, sizeof(OPENFILENAME));
    ofn->lStructSize = sizeof(OPENFILENAME);
    ofn->hwndOwner = owner;

    // Create a buffer with the filter strings.
    int totalStringLength = 0;
    QList<FilterSpec> specs =
        filterSpecs(m_options->nameFilters(), m_options->options() & QFileDialogOptions::HideNameFilterDetails, &totalStringLength);
    const int size = specs.size();
    wchar_t *ptr = new wchar_t[totalStringLength + 2 * size + 1];
    ofn->lpstrFilter = ptr;
    foreach (const FilterSpec &spec, specs) {
        ptr += spec.description.toWCharArray(ptr);
        *ptr++ = 0;
        ptr += spec.filter.toWCharArray(ptr);
        *ptr++ = 0;
    }
    *ptr = 0;
    const int nameFilterIndex = indexOfNameFilter(m_nameFilters, m_selectedNameFilter);
    if (nameFilterIndex >= 0)
        ofn->nFilterIndex = nameFilterIndex + 1; // 1..n based.
    // lpstrFile receives the initial selection and is the buffer
    // for the target. If it contains any invalid character, the dialog
    // will not show.
    ofn->nMaxFile = 65535;
    const QString initiallySelectedFile =
        QDir::toNativeSeparators(m_initialFile).remove(QLatin1Char('<')).
            remove(QLatin1Char('>')).remove(QLatin1Char('"')).remove(QLatin1Char('|'));
    ofn->lpstrFile = qStringToWCharArray(initiallySelectedFile, ofn->nMaxFile);
    ofn->lpstrInitialDir = qStringToWCharArray(QDir::toNativeSeparators(m_directory));
    ofn->lpstrTitle = (wchar_t*)m_title.utf16();
    // Determine lpstrDefExt. Note that the current MSDN docs document this
    // member wrong. It should rather be documented as "the default extension
    // if no extension was given and if the current filter does not have an
    // extension (e.g (*)). If the current filter has an extension, use
    // the extension of the current filter".
    if (m_options->acceptMode() == QFileDialogOptions::AcceptSave) {
        QString defaultSuffix = m_options->defaultSuffix();
        if (defaultSuffix.startsWith(QLatin1Char('.')))
            defaultSuffix.remove(0, 1);
        if (!defaultSuffix.isEmpty())
            ofn->lpstrDefExt = qStringToWCharArray(defaultSuffix);
    }
    // Flags.
    ofn->Flags = (OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST);
    if (m_options->fileMode() == QFileDialogOptions::ExistingFile
        || m_options->fileMode() == QFileDialogOptions::ExistingFiles)
        ofn->Flags |= (OFN_FILEMUSTEXIST);
    if (m_options->fileMode() == QFileDialogOptions::ExistingFiles)
        ofn->Flags |= (OFN_ALLOWMULTISELECT);
    if (!(m_options->options() & QFileDialogOptions::DontConfirmOverwrite))
        ofn->Flags |= OFN_OVERWRITEPROMPT;
}

QStringList QWindowsXpNativeFileDialog::execFileNames(HWND owner, int *selectedFilterIndex) const
{
    *selectedFilterIndex = -1;
    OPENFILENAME ofn;
    populateOpenFileName(&ofn, owner);
    QStringList result;
    const bool isSave = m_options->acceptMode() == QFileDialogOptions::AcceptSave;
    if (isSave ? m_getSaveFileNameW(&ofn) : m_getOpenFileNameW(&ofn)) {
        *selectedFilterIndex = ofn.nFilterIndex - 1;
        result.push_back(QDir::cleanPath(QString::fromWCharArray(ofn.lpstrFile)));
        // For multiselection, the first item is the path followed
        // by "\0<file1>\0<file2>\0\0".
        if (ofn.Flags & (OFN_ALLOWMULTISELECT)) {
            wchar_t *ptr = ofn.lpstrFile + result.front().size() + 1;
            if (*ptr) {
                const QString path = result.takeAt(0) + QLatin1Char('/');
                while (*ptr) {
                    const QString fileName = QString::fromWCharArray(ptr);
                    result.push_back(path + fileName);
                    ptr += fileName.size() + 1;
                } // extract multiple files
            } // has multiple files
        } // multiple flag set
    }
    delete [] ofn.lpstrFile;
    delete [] ofn.lpstrInitialDir;
    delete [] ofn.lpstrFilter;
    delete [] ofn.lpstrDefExt;
    return result;
}

/*!
    \class QWindowsXpFileDialogHelper
    \brief Dialog helper using QWindowsXpNativeFileDialog

    \sa QWindowsXpNativeFileDialog
    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsXpFileDialogHelper : public QWindowsDialogHelperBase<QPlatformFileDialogHelper>
{
public:
    QWindowsXpFileDialogHelper() {}
    virtual bool supportsNonModalDialog() const { return false; }

    virtual bool defaultNameFilterDisables() const
        { return true; }
    virtual void setDirectory(const QString &directory);
    virtual QString directory() const;
    virtual void selectFile(const QString &filename);
    virtual QStringList selectedFiles() const;
    virtual void setFilter() {}
    virtual void setNameFilters(const QStringList &);
    virtual void selectNameFilter(const QString &);
    virtual QString selectedNameFilter() const;

private:
    virtual QWindowsNativeDialogBase *createNativeDialog();
    inline QWindowsXpNativeFileDialog *nativeFileDialog() const
        { return static_cast<QWindowsXpNativeFileDialog *>(nativeDialog()); }
};

QWindowsNativeDialogBase *QWindowsXpFileDialogHelper::createNativeDialog()
{
    if (QWindowsNativeDialogBase *result = QWindowsXpNativeFileDialog::create(options())) {
        QObject::connect(result, SIGNAL(accepted()), this, SIGNAL(accept()));
        QObject::connect(result, SIGNAL(rejected()), this, SIGNAL(reject()));
        return result;
    }
    return 0;
}

void QWindowsXpFileDialogHelper::setDirectory(const QString &directory)
{
    if (QWindowsXpNativeFileDialog *nfd = nativeFileDialog())
        nfd->setDirectory(directory);
}

QString QWindowsXpFileDialogHelper::directory() const
{
    if (const QWindowsXpNativeFileDialog *nfd = nativeFileDialog())
        return nfd->directory();
    return QString();
}

void QWindowsXpFileDialogHelper::selectFile(const QString &filename)
{
    if (QWindowsXpNativeFileDialog *nfd = nativeFileDialog())
        nfd->selectFile(filename);
}

QStringList QWindowsXpFileDialogHelper::selectedFiles() const
{
    if (const QWindowsXpNativeFileDialog *nfd = nativeFileDialog())
        return nfd->selectedFiles();
    return QStringList();
}

void QWindowsXpFileDialogHelper::setNameFilters(const QStringList &n)
{
    if (QWindowsXpNativeFileDialog *nfd = nativeFileDialog())
        nfd->setNameFilters(n);
}

void QWindowsXpFileDialogHelper::selectNameFilter(const QString &f)
{
    if (QWindowsXpNativeFileDialog *nfd = nativeFileDialog())
        nfd->selectNameFilter(f);
}

QString QWindowsXpFileDialogHelper::selectedNameFilter() const
{
    if (const QWindowsXpNativeFileDialog *nfd = nativeFileDialog())
        return nfd->selectedNameFilter();
    return QString();
}

#endif // Q_OS_WINCE

/*!
    \class QWindowsNativeColorDialog
    \brief Native Windows color dialog.

    Wrapper around Comdlg32's ChooseColor() function.
    Not currently in use as QColorDialog is equivalent.

    \sa QWindowsColorDialogHelper
    \sa #define USE_NATIVE_COLOR_DIALOG
    \internal
    \ingroup qt-lighthouse-win
*/

typedef QSharedPointer<QColor> SharedPointerColor;

#ifdef USE_NATIVE_COLOR_DIALOG
class QWindowsNativeColorDialog : public QWindowsNativeDialogBase
{
    Q_OBJECT
public:
    enum { CustomColorCount = 16 };

    explicit QWindowsNativeColorDialog(const SharedPointerColor &color);

    virtual void setWindowTitle(const QString &) {}
    virtual void exec(HWND owner = 0);
    virtual QPlatformDialogHelper::DialogCode result() const { return m_code; }

public slots:
    virtual void close() {}

private:
    COLORREF m_customColors[CustomColorCount];
    QPlatformDialogHelper::DialogCode m_code;
    SharedPointerColor m_color;
};

QWindowsNativeColorDialog::QWindowsNativeColorDialog(const SharedPointerColor &color) :
    m_code(QPlatformDialogHelper::Rejected), m_color(color)
{
    qFill(m_customColors, m_customColors + 16, COLORREF(0));
}

void QWindowsNativeColorDialog::exec(HWND owner)
{
    typedef BOOL (WINAPI *ChooseColorWType)(LPCHOOSECOLORW);

    CHOOSECOLOR chooseColor;
    if (QWindowsContext::verboseDialogs)
        qDebug() << '>' << __FUNCTION__ << " on " << owner;
    ZeroMemory(&chooseColor, sizeof(chooseColor));
    chooseColor.lStructSize = sizeof(chooseColor);
    chooseColor.hwndOwner = owner;
    chooseColor.lpCustColors = m_customColors;
    QRgb *qCustomColors = QColorDialogOptions::customColors();
    const int customColorCount = qMin(QColorDialogOptions::customColorCount(),
                                      int(CustomColorCount));
    for (int c= 0; c < customColorCount; ++c)
        m_customColors[c] = qColorToCOLORREF(QColor(qCustomColors[c]));
    chooseColor.rgbResult = qColorToCOLORREF(*m_color);
    chooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;
    static ChooseColorWType chooseColorW = 0;
    if (!chooseColorW) {
        QSystemLibrary library(QStringLiteral("Comdlg32"));
        chooseColorW = (ChooseColorWType)library.resolve("ChooseColorW");
    }
    if (chooseColorW) {
        m_code = chooseColorW(&chooseColor) ?
            QPlatformDialogHelper::Accepted : QPlatformDialogHelper::Rejected;
        QWindowsDialogs::eatMouseMove();
    } else {
        m_code = QPlatformDialogHelper::Rejected;
    }
    if (m_code == QPlatformDialogHelper::Accepted) {
        *m_color = COLORREFToQColor(chooseColor.rgbResult);
        for (int c= 0; c < customColorCount; ++c)
            qCustomColors[c] = COLORREFToQColor(m_customColors[c]).rgb();
        emit accepted();
        if (QWindowsContext::verboseDialogs)
            qDebug() << '<' << __FUNCTION__ << m_color;
    } else {
        emit rejected();
    }
}

/*!
    \class QWindowsColorDialogHelper
    \brief Helper for native Windows color dialogs

    Not currently in use as QColorDialog is equivalent.

    \sa #define USE_NATIVE_COLOR_DIALOG
    \sa QWindowsNativeColorDialog
    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsColorDialogHelper : public QWindowsDialogHelperBase<QPlatformColorDialogHelper>
{
public:
    QWindowsColorDialogHelper() {}

    virtual bool supportsNonModalDialog()
        { return false; }

    virtual QColor currentColor() const { return *m_currentColor; }
    virtual void setCurrentColor(const QColor &c) { *m_currentColor = c; }

private:
    inline QWindowsNativeColorDialog *nativeFileDialog() const
        { return static_cast<QWindowsNativeColorDialog *>(nativeDialog()); }
    virtual QWindowsNativeDialogBase *createNativeDialog();

    SharedPointerColor m_currentColor;
};

QWindowsNativeDialogBase *QWindowsColorDialogHelper::createNativeDialog()
{
    QWindowsNativeColorDialog *nativeDialog = new QWindowsNativeColorDialog(m_currentColor);
    nativeDialog->setWindowTitle(options()->windowTitle());
    return nativeDialog;
}
#endif // USE_NATIVE_COLOR_DIALOG

namespace QWindowsDialogs {

// QWindowsDialogHelperBase creation functions
bool useHelper(QPlatformTheme::DialogType type)
{
    if (QWindowsIntegration::instance()->options() & QWindowsIntegration::NoNativeDialogs)
        return false;
    switch (type) {
    case QPlatformTheme::FileDialog:
        return QSysInfo::windowsVersion() >= QSysInfo::WV_XP;
    case QPlatformTheme::ColorDialog:
#ifdef USE_NATIVE_COLOR_DIALOG
        return true;
#else
        break;
#endif
    case QPlatformTheme::FontDialog:
        break;
    }
    return false;
}

QPlatformDialogHelper *createHelper(QPlatformTheme::DialogType type)
{
    if (QWindowsIntegration::instance()->options() & QWindowsIntegration::NoNativeDialogs)
        return 0;
    switch (type) {
    case QPlatformTheme::FileDialog:
#ifndef Q_OS_WINCE
        if (QWindowsIntegration::instance()->options() & QWindowsIntegration::XpNativeDialogs
            || QSysInfo::windowsVersion() == QSysInfo::WV_XP) {
            return new QWindowsXpFileDialogHelper();
        }
        if (QSysInfo::windowsVersion() > QSysInfo::WV_XP)
            return new QWindowsFileDialogHelper();
#else
        return new QWindowsFileDialogHelper();
#endif // Q_OS_WINCE
    case QPlatformTheme::ColorDialog:
#ifdef USE_NATIVE_COLOR_DIALOG
        return new QWindowsColorDialogHelper();
#else
        break;
#endif
    case QPlatformTheme::FontDialog:
        break;
    }
    return 0;
}

} // namespace QWindowsDialogs
QT_END_NAMESPACE

#include "qwindowsdialoghelpers.moc"
