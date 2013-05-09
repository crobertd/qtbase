#ifndef __QTPTNGTHEME_H__
#define __QTPTNGTHEME_H__

#include <qpa/qplatformtheme.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

class QTptNgTheme : public QPlatformTheme
{
public:
    QTptNgTheme();

    QVariant themeHint(ThemeHint hint) const;

    static const char *name;

private:
    QVariant m_StartDragDistance;
    QVariant m_StartDragTime;
    QVariant m_StartDragVelocity;
};

QT_END_NAMESPACE

#endif/*__QTPTNGTHEME_H__*/
