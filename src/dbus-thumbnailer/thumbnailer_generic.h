/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -c ThumbnailerGenericProxy -p dbus-thumbnailer/thumbnailer_generic.h:dbus-thumbnailer/thumbnailer_generic.cpp dbus-thumbnailer/tumbler-service-dbus.xml org.freedesktop.thumbnails.Thumbnailer1
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef THUMBNAILER_GENERIC_H_1267616651
#define THUMBNAILER_GENERIC_H_1267616651

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.freedesktop.thumbnails.Thumbnailer1
 */
class ThumbnailerGenericProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.thumbnails.Thumbnailer1"; }

public:
    ThumbnailerGenericProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ThumbnailerGenericProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Dequeue(uint handle)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(handle);
        return asyncCallWithArgumentList(QLatin1String("Dequeue"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetFlavors()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("GetFlavors"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetSchedulers()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("GetSchedulers"), argumentList);
    }

    inline QDBusPendingReply<QStringList, QStringList> GetSupported()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("GetSupported"), argumentList);
    }
    inline QDBusReply<QStringList> GetSupported(QStringList &mime_types)
    {
        QList<QVariant> argumentList;
        QDBusMessage reply = callWithArgumentList(QDBus::Block, QLatin1String("GetSupported"), argumentList);
        if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
            mime_types = qdbus_cast<QStringList>(reply.arguments().at(1));
        }
        return reply;
    }

    inline QDBusPendingReply<uint> Queue(const QStringList &uris, const QStringList &mime_types, const QString &flavor, const QString &scheduler, uint handle_to_unqueue)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(uris) << qVariantFromValue(mime_types) << qVariantFromValue(flavor) << qVariantFromValue(scheduler) << qVariantFromValue(handle_to_unqueue);
        return asyncCallWithArgumentList(QLatin1String("Queue"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void Error(uint handle, const QStringList &failed_uris, int error_code, const QString &message);
    void Finished(uint handle);
    void Ready(uint handle, const QStringList &uris);
    void Started(uint handle);
};

namespace org {
  namespace freedesktop {
    namespace thumbnails {
      typedef ::ThumbnailerGenericProxy Thumbnailer1;
    }
  }
}
#endif
