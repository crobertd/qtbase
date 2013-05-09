#include "qtptngtheme.h"

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
}

QVariant QTptNgTheme::themeHint(ThemeHint hint) const
{
    switch (hint) {
    case QPlatformTheme::StartDragDistance:
        return m_StartDragDistance;
    case QPlatformTheme::StartDragTime:
        return m_StartDragTime;
    case QPlatformTheme::StartDragVelocity:
        return m_StartDragVelocity;
    default:
        break;
    }
    return QPlatformTheme::themeHint(hint);
}

QT_END_NAMESPACE
