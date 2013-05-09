#include "qtptngtheme.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

static QVariant GetStyle(const char *styleName, QMetaType::Type type = QMetaType::Int)
{
    QByteArray value = qgetenv(styleName);

    if (value.isEmpty() == false) {
        QVariant style(value);
        if (style.convert(type) == true)
            return style;
    }

    return QVariant();
}

const char *QTptNgTheme::name = "equinox";

QTptNgTheme::QTptNgTheme()
{
    // Initialize our style values
    m_StartDragDistance = GetStyle("EQ_STARTDRAGDISTANCE");
    m_StartDragTime = GetStyle("EQ_STARTDRAGTIME");
    m_StartDragVelocity = GetStyle("EQ_STARTDRAGVELOCITY");
    qDebug() << "EQ_STARTDRAGDISTANCE:" << m_StartDragDistance;
    qDebug() << "EQ_STARTDRAGTIME:" << m_StartDragTime;
    qDebug() << "EQ_STARTDRAGVELOCITY:" << m_StartDragVelocity;
}

QVariant QTptNgTheme::themeHint(ThemeHint hint) const
{
qDebug() << "styleHint:" << hint;
qDebug() << __FUNCTION__ << __LINE__;
    switch (hint) {
    case QPlatformTheme::StartDragDistance:
qDebug() << __FUNCTION__ << __LINE__;
        qDebug() << "StartDragDistance:" << m_StartDragDistance;
        return m_StartDragDistance;
    case QPlatformTheme::StartDragTime:
qDebug() << __FUNCTION__ << __LINE__;
        qDebug() << "StartDragTime:" << m_StartDragTime;
        return m_StartDragTime;
    case QPlatformTheme::StartDragVelocity:
qDebug() << __FUNCTION__ << __LINE__;
        qDebug() << "StartDragVelocity:" << m_StartDragVelocity;
        return m_StartDragVelocity;
    default:
        break;
    }
    return QPlatformTheme::themeHint(hint);
}

QT_END_NAMESPACE
