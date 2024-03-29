/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <qpa/qplatformopenglcontext.h>
#include <qpa/qplatformintegration.h>
#include "qopenglcontext.h"
#include "qopenglcontext_p.h"
#include "qwindow.h"

#include <QtCore/QThreadStorage>
#include <QtCore/QThread>

#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/private/qopengl_p.h>
#include <QtGui/private/qwindow_p.h>
#include <QtGui/QScreen>

#include <private/qopenglextensions_p.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

class QGuiGLThreadContext
{
public:
    QGuiGLThreadContext()
        : context(0)
    {
    }
    ~QGuiGLThreadContext() {
        if (context)
            context->doneCurrent();
    }
    QOpenGLContext *context;
};

static QThreadStorage<QGuiGLThreadContext *> qwindow_context_storage;

#ifndef QT_NO_DEBUG
QHash<QOpenGLContext *, bool> QOpenGLContextPrivate::makeCurrentTracker;
QMutex QOpenGLContextPrivate::makeCurrentTrackerMutex;
#endif

/*!
    \class QOpenGLContext
    \inmodule QtGui
    \since 5.0
    \brief The QOpenGLContext class represents a native OpenGL context, enabling
           OpenGL rendering on a QSurface.

    QOpenGLContext represents the OpenGL state of an underlying OpenGL context.
    To set up a context, set its screen and format such that they match those
    of the surface or surfaces with which the context is meant to be used, if
    necessary make it share resources with other contexts with
    setShareContext(), and finally call create(). Use isValid() to check if the
    context was successfully initialized.

    A context can be made current against a given surface by calling
    makeCurrent(). When OpenGL rendering is done, call swapBuffers() to swap
    the front and back buffers of the surface, so that the newly rendered
    content becomes visible. To be able to support certain platforms,
    QOpenGLContext requires that you call makeCurrent() again before starting
    rendering a new frame, after calling swapBuffers().

    If the context is temporarily not needed, such as when the application is
    not rendering, it can be useful to call destroy() to free resources.
    However, if you do so you will need to call create() again before the
    context can be used, and you might need to recreate any OpenGL resources
    and reinitialize the OpenGL state. You can connect to the
    aboutToBeDestroyed() signal to clean up any resources that have been
    allocated with different ownership from the QOpenGLContext itself.

    Once a QOpenGLContext has been made current, you can render to it in a
    platform independent way by using Qt's OpenGL enablers such as
    QOpenGLFunctions, QOpenGLBuffer, QOpenGLShaderProgram, and
    QOpenGLFramebufferObject. It is also possible to use the platform's OpenGL
    API directly, without using the Qt enablers, although potentially at the
    cost of portability. The latter is necessary when wanting to use OpenGL 1.x
    or OpenGL ES 1.x.

    For more information about the OpenGL API, refer to the official
    \l{OpenGL documentation}.

    For an example of how to use QOpenGLContext see the
    \l{OpenGL Window Example}{OpenGL Window} example.

    \section1 Thread affinity

    QOpenGLContext can be moved to a different thread with moveToThread(). Do
    not call makeCurrent() from a different thread than the one to which the
    QOpenGLContext object belongs. A context can only be current in one thread
    and against one surface at a time, and a thread only has one context
    current at a time.

    \section1 Context resource sharing

    Resources, such as framebuffer objects, textures, and vertex buffer objects
    can be shared between contexts.  Use setShareContext() before calling
    create() to specify that the contexts should share these resources.
    QOpenGLContext internally keeps track of a QOpenGLContextGroup object which
    can be accessed with shareGroup(), and which can be used to find all the
    contexts in a given share group. A share group consists of all contexts that
    have been successfully initialized and are sharing with an existing context in
    the share group. A non-sharing context has a share group consisting of a
    single context.

    \section1 Default framebuffer

    On certain platforms, a framebuffer other than 0 might be the default frame
    buffer depending on the current surface. Instead of calling
    glBindFramebuffer(0), it is recommended that you use
    glBindFramebuffer(ctx->defaultFramebufferObject()), to ensure that your
    application is portable between different platforms. However, if you use
    QOpenGLFunctions::glBindFramebuffer(), this is done automatically for you.

    \sa QOpenGLFunctions, QOpenGLBuffer, QOpenGLShaderProgram, QOpenGLFramebufferObject
*/

/*!
    \internal

    Set the current context. Returns the previously current context.
*/
QOpenGLContext *QOpenGLContextPrivate::setCurrentContext(QOpenGLContext *context)
{
    QGuiGLThreadContext *threadContext = qwindow_context_storage.localData();
    if (!threadContext) {
        if (!QThread::currentThread()) {
            qWarning("No QTLS available. currentContext won't work");
            return 0;
        }
        threadContext = new QGuiGLThreadContext;
        qwindow_context_storage.setLocalData(threadContext);
    }
    QOpenGLContext *previous = threadContext->context;
    threadContext->context = context;
    return previous;
}

int QOpenGLContextPrivate::maxTextureSize()
{
    if (max_texture_size != -1)
        return max_texture_size;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

#if defined(QT_OPENGL_ES)
    return max_texture_size;
#else
    GLenum proxy = GL_PROXY_TEXTURE_2D;

    GLint size;
    GLint next = 64;
    glTexImage2D(proxy, 0, GL_RGBA, next, next, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glGetTexLevelParameteriv(proxy, 0, GL_TEXTURE_WIDTH, &size);
    if (size == 0) {
        return max_texture_size;
    }
    do {
        size = next;
        next = size * 2;

        if (next > max_texture_size)
            break;
        glTexImage2D(proxy, 0, GL_RGBA, next, next, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glGetTexLevelParameteriv(proxy, 0, GL_TEXTURE_WIDTH, &next);
    } while (next > size);

    max_texture_size = size;
    return max_texture_size;
#endif
}

/*!
    Returns the last context which called makeCurrent in the current thread,
    or 0, if no context is current.
*/
QOpenGLContext* QOpenGLContext::currentContext()
{
    QGuiGLThreadContext *threadContext = qwindow_context_storage.localData();
    if(threadContext) {
        return threadContext->context;
    }
    return 0;
}

/*!
    Returns true if the \a first and \a second contexts are sharing OpenGL resources.
*/
bool QOpenGLContext::areSharing(QOpenGLContext *first, QOpenGLContext *second)
{
    return first->shareGroup() == second->shareGroup();
}

/*!
    Returns the underlying platform context.

    \internal
*/
QPlatformOpenGLContext *QOpenGLContext::handle() const
{
    Q_D(const QOpenGLContext);
    return d->platformGLContext;
}

/*!
    Returns the underlying platform context with which this context is sharing.

    \internal
*/

QPlatformOpenGLContext *QOpenGLContext::shareHandle() const
{
    Q_D(const QOpenGLContext);
    if (d->shareContext)
        return d->shareContext->handle();
    return 0;
}

/*!
    Creates a new OpenGL context instance with parent object \a parent.

    Before it can be used you need to set the proper format and call create().

    \sa create(), makeCurrent()
*/
QOpenGLContext::QOpenGLContext(QObject *parent)
    : QObject(*new QOpenGLContextPrivate(), parent)
{
    Q_D(QOpenGLContext);
    d->screen = QGuiApplication::primaryScreen();
}

/*!
    Sets the \a format the OpenGL context should be compatible with. You need
    to call create() before it takes effect.
*/
void QOpenGLContext::setFormat(const QSurfaceFormat &format)
{
    Q_D(QOpenGLContext);
    d->requestedFormat = format;
}

/*!
    Makes this context share textures, shaders, and other OpenGL resources
    with \a shareContext. You need to call create() before it takes effect.
*/
void QOpenGLContext::setShareContext(QOpenGLContext *shareContext)
{
    Q_D(QOpenGLContext);
    d->shareContext = shareContext;
}

/*!
    Sets the \a screen the OpenGL context should be valid for. You need to call
    create() before it takes effect.
*/
void QOpenGLContext::setScreen(QScreen *screen)
{
    Q_D(QOpenGLContext);
    d->screen = screen;
    if (!d->screen)
        d->screen = QGuiApplication::primaryScreen();
}

/*!
    Attempts to create the OpenGL context with the current configuration.

    The current configuration includes the format, the share context, and the
    screen.

    If the OpenGL implementation on your system does not support the requested
    version of OpenGL context, then QOpenGLContext will try to create the closest
    matching version. The actual created context properties can be queried
    using the QSurfaceFormat returned by the format() function. For example, if
    you request a context that supports OpenGL 4.3 Core profile but the driver
    and/or hardware only supports version 3.2 Core profile contexts then you will
    get a 3.2 Core profile context.

    Returns true if the native context was successfully created and is ready to
    be used with makeCurrent(), swapBuffers(), etc.

    \sa makeCurrent(), destroy(), format()
*/
bool QOpenGLContext::create()
{
    destroy();

    Q_D(QOpenGLContext);
    d->platformGLContext = QGuiApplicationPrivate::platformIntegration()->createPlatformOpenGLContext(this);
    if (!d->platformGLContext)
        return false;
    d->platformGLContext->setContext(this);
    if (!d->platformGLContext->isSharing())
        d->shareContext = 0;
    d->shareGroup = d->shareContext ? d->shareContext->shareGroup() : new QOpenGLContextGroup;
    d->shareGroup->d_func()->addContext(this);
    return d->platformGLContext;
}

/*!
    Destroy the underlying platform context associated with this context.

    If any other context is directly or indirectly sharing resources with this
    context, the shared resources, which includes vertex buffer objects, shader
    objects, textures, and framebuffer objects, are not freed. However,
    destroying the underlying platform context frees any state associated with
    the context.

    After destroy() has been called, you must call create() if you wish to
    use the context again.

    \note This implicitly calls doneCurrent() if the context is current.

    \sa create()
*/
void QOpenGLContext::destroy()
{
    Q_D(QOpenGLContext);
    if (d->platformGLContext)
        emit aboutToBeDestroyed();
    if (QOpenGLContext::currentContext() == this)
        doneCurrent();
    if (d->shareGroup)
        d->shareGroup->d_func()->removeContext(this);
    d->shareGroup = 0;
    delete d->platformGLContext;
    d->platformGLContext = 0;
    delete d->functions;
    d->functions = 0;
}

/*!
    \fn void QOpenGLContext::aboutToBeDestroyed()

    This signal is emitted before the underlying native OpenGL context is
    destroyed, such that users may clean up OpenGL resources that might
    otherwise be left dangling in the case of shared OpenGL contexts.

    If you wish to make the context current in order to do clean-up, make sure
    to only connect to the signal using a direct connection.
*/

/*!
    Destroys the QOpenGLContext object.

    This implicitly calls destroy(), so if this is the current context for the
    thread, doneCurrent() is also called.

    \sa destroy()
*/
QOpenGLContext::~QOpenGLContext()
{
    destroy();

#ifndef QT_NO_DEBUG
    QOpenGLContextPrivate::cleanMakeCurrentTracker(this);
#endif
}

/*!
    Returns if this context is valid, i.e. has been successfully created.

    \sa create()
*/
bool QOpenGLContext::isValid() const
{
    Q_D(const QOpenGLContext);
    return d->platformGLContext && d->platformGLContext->isValid();
}

/*!
    Get the QOpenGLFunctions instance for this context.

    QOpenGLContext offers this as a convenient way to access QOpenGLFunctions
    without having to manage it manually.

    The context or a sharing context must be current.
*/
QOpenGLFunctions *QOpenGLContext::functions() const
{
    Q_D(const QOpenGLContext);
    if (!d->functions)
        const_cast<QOpenGLFunctions *&>(d->functions) = new QOpenGLExtensions(QOpenGLContext::currentContext());
    return d->functions;
}

/*!
    Returns the set of OpenGL extensions supported by this context.

    The context or a sharing context must be current.

    \sa hasExtension()
*/
QSet<QByteArray> QOpenGLContext::extensions() const
{
    Q_D(const QOpenGLContext);
    if (d->extensionNames.isEmpty()) {
        QOpenGLExtensionMatcher matcher;
        d->extensionNames = matcher.extensions();
    }

    return d->extensionNames;
}

/*!
    Returns true if this OpenGL context supports the specified OpenGL
    \a extension, false otherwise.

    The context or a sharing context must be current.

    \sa extensions()
*/
bool QOpenGLContext::hasExtension(const QByteArray &extension) const
{
    return extensions().contains(extension);
}

/*!
    Call this to get the default framebuffer object for the current surface.

    On some platforms the default framebuffer object depends on the surface
    being rendered to, and might be different from 0. Thus, instead of calling
    glBindFramebuffer(0), you should call
    glBindFramebuffer(ctx->defaultFramebufferObject()) if you want your
    application to work across different Qt platforms.

    If you use the glBindFramebuffer() in QOpenGLFunctions you do not have to
    worry about this, as it automatically binds the current context's
    defaultFramebufferObject() when 0 is passed.
*/
GLuint QOpenGLContext::defaultFramebufferObject() const
{
    if (!isValid())
        return 0;

    Q_D(const QOpenGLContext);
    if (!d->surface || !d->surface->surfaceHandle())
        return 0;

    return d->platformGLContext->defaultFramebufferObject(d->surface->surfaceHandle());
}

/*!
    Makes the context current in the current thread, against the given
    \a surface. Returns true if successful.

    If \a surface is 0 this is equivalent to calling doneCurrent().

    Do not call this function from a different thread than the one the
    QOpenGLContext instance lives in. If you wish to use QOpenGLContext from a
    different thread you should first call make sure it's not current in the
    current thread, by calling doneCurrent() if necessary. Then call
    moveToThread(otherThread) before using it in the other thread.

    \sa functions(), doneCurrent()
*/
bool QOpenGLContext::makeCurrent(QSurface *surface)
{
    Q_D(QOpenGLContext);
    if (!isValid())
        return false;

    if (thread() != QThread::currentThread())
        qFatal("Cannot make QOpenGLContext current in a different thread");

    if (!surface) {
        doneCurrent();
        return true;
    }

    if (!surface->surfaceHandle())
        return false;

    if (surface->surfaceType() != QSurface::OpenGLSurface) {
        qWarning() << "QOpenGLContext::makeCurrent() called with non-opengl surface";
        return false;
    }

    QOpenGLContext *previous = QOpenGLContextPrivate::setCurrentContext(this);

    if (d->platformGLContext->makeCurrent(surface->surfaceHandle())) {
        d->surface = surface;

        d->shareGroup->d_func()->deletePendingResources(this);

#ifndef QT_NO_DEBUG
        QOpenGLContextPrivate::toggleMakeCurrentTracker(this, true);
#endif

        return true;
    }

    QOpenGLContextPrivate::setCurrentContext(previous);

    return false;
}

/*!
    Convenience function for calling makeCurrent with a 0 surface.

    This results in no context being current in the current thread.

    \sa makeCurrent(), currentContext()
*/
void QOpenGLContext::doneCurrent()
{
    Q_D(QOpenGLContext);
    if (!isValid())
        return;

    if (QOpenGLContext::currentContext() == this)
        d->shareGroup->d_func()->deletePendingResources(this);

    d->platformGLContext->doneCurrent();
    QOpenGLContextPrivate::setCurrentContext(0);

    d->surface = 0;
}

/*!
    Returns the surface the context has been made current with.

    This is the surface passed as an argument to makeCurrent().
*/
QSurface *QOpenGLContext::surface() const
{
    Q_D(const QOpenGLContext);
    return d->surface;
}


/*!
    Swap the back and front buffers of \a surface.

    Call this to finish a frame of OpenGL rendering, and make sure to
    call makeCurrent() again before you begin a new frame.
*/
void QOpenGLContext::swapBuffers(QSurface *surface)
{
    Q_D(QOpenGLContext);
    if (!isValid())
        return;

    if (!surface) {
        qWarning() << "QOpenGLContext::swapBuffers() called with null argument";
        return;
    }

    if (surface->surfaceType() != QSurface::OpenGLSurface) {
         qWarning() << "QOpenGLContext::swapBuffers() called with non-opengl surface";
         return;
    }

    if (surface->surfaceClass() == QSurface::Window
        && !qt_window_private(static_cast<QWindow *>(surface))->receivedExpose)
    {
        qWarning() << "QOpenGLContext::swapBuffers() called with non-exposed window, behavior is undefined";
    }

    QPlatformSurface *surfaceHandle = surface->surfaceHandle();
    if (!surfaceHandle)
        return;

#if !defined(QT_NO_DEBUG)
    if (!QOpenGLContextPrivate::toggleMakeCurrentTracker(this, false))
        qWarning() << "QOpenGLContext::swapBuffers() called without corresponding makeCurrent()";
#endif
    if (surface->format().swapBehavior() == QSurfaceFormat::SingleBuffer)
        glFlush();
    d->platformGLContext->swapBuffers(surfaceHandle);
}

/*!
    Resolves the function pointer to an OpenGL extension function, identified by \a procName

    Returns 0 if no such function can be found.
*/
QFunctionPointer QOpenGLContext::getProcAddress(const QByteArray &procName) const
{
    Q_D(const QOpenGLContext);
    if (!d->platformGLContext)
        return 0;
    return d->platformGLContext->getProcAddress(procName);
}

/*!
    Returns the format of the underlying platform context, if create() has been called.

    Otherwise, returns the requested format.
*/
QSurfaceFormat QOpenGLContext::format() const
{
    Q_D(const QOpenGLContext);
    if (!d->platformGLContext)
        return d->requestedFormat;
    return d->platformGLContext->format();
}

/*!
    Returns the share group this context belongs to.
*/
QOpenGLContextGroup *QOpenGLContext::shareGroup() const
{
    Q_D(const QOpenGLContext);
    return d->shareGroup;
}

/*!
    Returns the share context this context was created with.

    If the underlying platform was not able to support the requested
    sharing, this will return 0.
*/
QOpenGLContext *QOpenGLContext::shareContext() const
{
    Q_D(const QOpenGLContext);
    return d->shareContext;
}

/*!
    Returns the screen the context was created for.
*/
QScreen *QOpenGLContext::screen() const
{
    Q_D(const QOpenGLContext);
    return d->screen;
}

/*!
    internal: Needs to have a pointer to qGLContext. But since this is in Qt GUI we can't
    have any type information.

    \internal
*/
void *QOpenGLContext::qGLContextHandle() const
{
    Q_D(const QOpenGLContext);
    return d->qGLContextHandle;
}

/*!
    \internal
*/
void QOpenGLContext::setQGLContextHandle(void *handle,void (*qGLContextDeleteFunction)(void *))
{
    Q_D(QOpenGLContext);
    d->qGLContextHandle = handle;
    d->qGLContextDeleteFunction = qGLContextDeleteFunction;
}

/*!
    \internal
*/
void QOpenGLContext::deleteQGLContext()
{
    Q_D(QOpenGLContext);
    if (d->qGLContextDeleteFunction && d->qGLContextHandle) {
        d->qGLContextDeleteFunction(d->qGLContextHandle);
        d->qGLContextDeleteFunction = 0;
        d->qGLContextHandle = 0;
    }
}

/*!
    \class QOpenGLContextGroup
    \since 5.0
    \brief The QOpenGLContextGroup class represents a group of contexts sharing
    OpenGL resources.
    \inmodule QtGui

    QOpenGLContextGroup is automatically created and managed by QOpenGLContext
    instances.  Its purpose is to identify all the contexts that are sharing
    resources.

    \sa QOpenGLContext::shareGroup()
*/
QOpenGLContextGroup::QOpenGLContextGroup()
    : QObject(*new QOpenGLContextGroupPrivate())
{
}

/*!
    \internal
*/
QOpenGLContextGroup::~QOpenGLContextGroup()
{
    Q_D(QOpenGLContextGroup);
    d->cleanup();
}

/*!
    Returns all the QOpenGLContext objects in this share group.
*/
QList<QOpenGLContext *> QOpenGLContextGroup::shares() const
{
    Q_D(const QOpenGLContextGroup);
    return d->m_shares;
}

/*!
    Returns the QOpenGLContextGroup corresponding to the current context.

    \sa QOpenGLContext::currentContext()
*/
QOpenGLContextGroup *QOpenGLContextGroup::currentContextGroup()
{
    QOpenGLContext *current = QOpenGLContext::currentContext();
    return current ? current->shareGroup() : 0;
}

void QOpenGLContextGroupPrivate::addContext(QOpenGLContext *ctx)
{
    QMutexLocker locker(&m_mutex);
    m_refs.ref();
    m_shares << ctx;
}

void QOpenGLContextGroupPrivate::removeContext(QOpenGLContext *ctx)
{
    Q_Q(QOpenGLContextGroup);

    bool deleteObject = false;

    {
        QMutexLocker locker(&m_mutex);
        m_shares.removeOne(ctx);

        if (ctx == m_context && !m_shares.isEmpty())
            m_context = m_shares.first();

        if (!m_refs.deref()) {
            cleanup();
            deleteObject = true;
        }
    }

    if (deleteObject) {
        if (q->thread() == QThread::currentThread())
            delete q; // Delete directly to prevent leak, refer to QTBUG-29056
        else
            q->deleteLater();
    }
}

void QOpenGLContextGroupPrivate::cleanup()
{
    Q_Q(QOpenGLContextGroup);
    {
        QHash<QOpenGLMultiGroupSharedResource *, QOpenGLSharedResource *>::const_iterator it, end;
        end = m_resources.constEnd();
        for (it = m_resources.constBegin(); it != end; ++it)
            it.key()->cleanup(q, it.value());
        m_resources.clear();
    }

    QList<QOpenGLSharedResource *>::iterator it = m_sharedResources.begin();
    QList<QOpenGLSharedResource *>::iterator end = m_sharedResources.end();

    while (it != end) {
        (*it)->invalidateResource();
        (*it)->m_group = 0;
        ++it;
    }

    m_sharedResources.clear();

    qDeleteAll(m_pendingDeletion.begin(), m_pendingDeletion.end());
    m_pendingDeletion.clear();
}

void QOpenGLContextGroupPrivate::deletePendingResources(QOpenGLContext *ctx)
{
    QMutexLocker locker(&m_mutex);

    QList<QOpenGLSharedResource *> pending = m_pendingDeletion;
    m_pendingDeletion.clear();

    QList<QOpenGLSharedResource *>::iterator it = pending.begin();
    QList<QOpenGLSharedResource *>::iterator end = pending.end();
    while (it != end) {
        (*it)->freeResource(ctx);
        delete *it;
        ++it;
    }
}

/*!
    \class QOpenGLSharedResource
    \internal
    \since 5.0
    \brief The QOpenGLSharedResource class is used to keep track of resources
    that are shared between OpenGL contexts (like textures, framebuffer
    objects, shader programs, etc), and clean them up in a safe way when
    they're no longer needed.
    \inmodule QtGui

    The QOpenGLSharedResource instance should never be deleted, instead free()
    should be called when it's no longer needed. Thus it will be put on a queue
    and freed at an appropriate time (when a context in the share group becomes
    current).

    The sub-class needs to implement two pure virtual functions. The first,
    freeResource() must be implemented to actually do the freeing, for example
    call glDeleteTextures() on a texture id. Qt makes sure a valid context in
    the resource's share group is current at the time. The other,
    invalidateResource(), is called by Qt in the circumstance when the last
    context in the share group is destroyed before free() has been called. The
    implementation of invalidateResource() should set any identifiers to 0 or
    set a flag to prevent them from being used later on.
*/
QOpenGLSharedResource::QOpenGLSharedResource(QOpenGLContextGroup *group)
    : m_group(group)
{
    QMutexLocker locker(&m_group->d_func()->m_mutex);
    m_group->d_func()->m_sharedResources << this;
}

QOpenGLSharedResource::~QOpenGLSharedResource()
{
}

// schedule the resource for deletion at an appropriate time
void QOpenGLSharedResource::free()
{
    if (!m_group) {
        delete this;
        return;
    }

    QMutexLocker locker(&m_group->d_func()->m_mutex);
    m_group->d_func()->m_sharedResources.removeOne(this);
    m_group->d_func()->m_pendingDeletion << this;

    // can we delete right away?
    QOpenGLContext *current = QOpenGLContext::currentContext();
    if (current && current->shareGroup() == m_group) {
        m_group->d_func()->deletePendingResources(current);
    }
}

/*!
    \class QOpenGLSharedResourceGuard
    \internal
    \since 5.0
    \brief The QOpenGLSharedResourceGuard class is a convenience sub-class of
    QOpenGLSharedResource to be used to track a single OpenGL object with a
    GLuint identifier. The constructor takes a function pointer to a function
    that will be used to free the resource if and when necessary.
    \inmodule QtGui

*/
void QOpenGLSharedResourceGuard::freeResource(QOpenGLContext *context)
{
    if (m_id) {
        QOpenGLFunctions functions(context);
        m_func(&functions, m_id);
        m_id = 0;
    }
}

/*!
    \class QOpenGLMultiGroupSharedResource
    \internal
    \since 5.0
    \brief The QOpenGLMultiGroupSharedResource keeps track of a shared resource
    that might be needed from multiple contexts, like a glyph cache or gradient
    cache. One instance of the object is created for each group when necessary.
    The shared resource instance should have a constructor that takes a
    QOpenGLContext *. To get an instance for a given context one calls
    T *QOpenGLMultiGroupSharedResource::value<T>(context), where T is a sub-class
    of QOpenGLSharedResource.
    \inmodule QtGui

    You should not call free() on QOpenGLSharedResources owned by a
    QOpenGLMultiGroupSharedResource instance.
*/
QOpenGLMultiGroupSharedResource::QOpenGLMultiGroupSharedResource()
    : active(0)
{
#ifdef QT_GL_CONTEXT_RESOURCE_DEBUG
    qDebug("Creating context group resource object %p.", this);
#endif
}

QOpenGLMultiGroupSharedResource::~QOpenGLMultiGroupSharedResource()
{
#ifdef QT_GL_CONTEXT_RESOURCE_DEBUG
    qDebug("Deleting context group resource %p. Group size: %d.", this, m_groups.size());
#endif
    for (int i = 0; i < m_groups.size(); ++i) {
        if (!m_groups.at(i)->shares().isEmpty()) {
            QOpenGLContext *context = m_groups.at(i)->shares().first();
            QOpenGLSharedResource *resource = value(context);
            if (resource)
                resource->free();
        }
        m_groups.at(i)->d_func()->m_resources.remove(this);
        active.deref();
    }
#ifndef QT_NO_DEBUG
    if (active.load() != 0) {
        qWarning("QtGui: Resources are still available at program shutdown.\n"
                 "          This is possibly caused by a leaked QOpenGLWidget, \n"
                 "          QOpenGLFramebufferObject or QOpenGLPixelBuffer.");
    }
#endif
}

void QOpenGLMultiGroupSharedResource::insert(QOpenGLContext *context, QOpenGLSharedResource *value)
{
#ifdef QT_GL_CONTEXT_RESOURCE_DEBUG
    qDebug("Inserting context group resource %p for context %p, managed by %p.", value, context, this);
#endif
    QOpenGLContextGroup *group = context->shareGroup();
    Q_ASSERT(!group->d_func()->m_resources.contains(this));
    group->d_func()->m_resources.insert(this, value);
    m_groups.append(group);
    active.ref();
}

QOpenGLSharedResource *QOpenGLMultiGroupSharedResource::value(QOpenGLContext *context)
{
    QOpenGLContextGroup *group = context->shareGroup();
    return group->d_func()->m_resources.value(this, 0);
}

QList<QOpenGLSharedResource *> QOpenGLMultiGroupSharedResource::resources() const
{
    QList<QOpenGLSharedResource *> result;
    for (QList<QOpenGLContextGroup *>::const_iterator it = m_groups.constBegin(); it != m_groups.constEnd(); ++it) {
        QOpenGLSharedResource *resource = (*it)->d_func()->m_resources.value(const_cast<QOpenGLMultiGroupSharedResource *>(this), 0);
        if (resource)
            result << resource;
    }
    return result;
}

void QOpenGLMultiGroupSharedResource::cleanup(QOpenGLContextGroup *group, QOpenGLSharedResource *value)
{
#ifdef QT_GL_CONTEXT_RESOURCE_DEBUG
    qDebug("Cleaning up context group resource %p, for group %p in thread %p.", this, group, QThread::currentThread());
#endif
    value->invalidateResource();
    value->free();
    active.deref();

    Q_ASSERT(m_groups.contains(group));
    m_groups.removeOne(group);
}

QT_END_NAMESPACE
