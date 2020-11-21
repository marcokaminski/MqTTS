#ifndef MQTTS_H
#define MQTTS_H

#include <QObject>

#include <QObject>
#include <QTimer>
#include <QTextStream>
#include <QSettings>

#include <qmqtt_client.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 1883
#define DEFAULT_CLIENTID "MqTTS"
#define DEFAULT_PUBTOPIC "mqtts/0/pub"
#define DEFAULT_SUBTOPIC "mqtts/0/sub"
#define DEFAULT_SOUNDTOPIC "mqtts/0/sounds"

class MqTTS : public QMQTT::Client {
    Q_OBJECT

public:
    explicit MqTTS(const QHostAddress& host, const quint16 port, const QString clientid, bool sounds, bool print, bool debug = false, QObject *parent = nullptr);
    MqTTS(QString configFile, bool debug = false, QObject *parent = nullptr);

    ~MqTTS();

public slots:
    void startSubscription();
    void parseMessage(const QMQTT::Message& message);
    void gotSubscription(const QString& topic, const quint8 qos);
    void isPublished(const QMQTT::Message& message, quint16 msgid);
    void errorHandler(const QMQTT::ClientError error);

signals:

private:
    QTextStream out, err;

    bool printing;
    bool debuging;
    bool soundEnabled = false;

    QSettings *config;

    QString clientid = DEFAULT_CLIENTID;
    QString topic_sub = DEFAULT_SUBTOPIC;
    QString topic_pub = DEFAULT_PUBTOPIC;
    QString topic_sound = DEFAULT_SOUNDTOPIC;
};

#endif // MQTTS_H
