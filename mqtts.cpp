#include <qmqtt_client.h>
#include <qmqtt_message.h>
#include <QProcess>

#include "mqtts.h"

MqTTS::MqTTS(const QHostAddress& host, const quint16 port, QString clientid, bool enSounds, bool enPrinting, bool enDebuging, QObject *parent) : QMQTT::Client(host, port, parent), out(stdout), err(stderr) {
    connect(this, SIGNAL(connected()), this, SLOT(startSubscription()));
    connect(this, SIGNAL(received(const QMQTT::Message&)), this, SLOT(parseMessage(const QMQTT::Message&)));
    connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(gotSubscription(const QString&, const quint8)));
    connect(this, SIGNAL(published(const QMQTT::Message&, quint16)), this, SLOT(isPublished(const QMQTT::Message&, quint16)));
    connect(this, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(errorHandler(const QMQTT::ClientError)));

    debuging = enDebuging;
    printing = enPrinting;
    soundEnabled = enSounds;

    setClientId(clientid);

    if (debuging) {
        out << "Host: " << this->host().toString() << endl;
        out << "Port: " << this->port() << endl;
        out << "Client ID: " << this->clientId() << endl;
        out << "Ausgabe der Nachrichten: " << printing << endl;
        out << "Topic Subscription: " << topic_sub << endl;
        out << "Topic Publish: " << topic_pub << endl;
        if (soundEnabled) {
            out << "Topic Sound: " << topic_sound << endl;
        }
    }
}

MqTTS::MqTTS(QString configFile, bool enDebuging, QObject*) : out(stdout), err(stderr) {
    connect(this, SIGNAL(connected()), this, SLOT(startSubscription()));
    connect(this, SIGNAL(received(const QMQTT::Message&)), this, SLOT(parseMessage(const QMQTT::Message&)));
    connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(gotSubscription(const QString&, const quint8)));
    connect(this, SIGNAL(published(const QMQTT::Message&, quint16)), this, SLOT(isPublished(const QMQTT::Message&, quint16)));
    connect(this, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(errorHandler(const QMQTT::ClientError)));

    config = new QSettings(configFile, QSettings::IniFormat);
    debuging = enDebuging;

    if (debuging) {
        out << "Lese Konfigurationsdatei: " << configFile << endl;
    }

    config->beginGroup("MQTT-Broker");
    setHost(QHostAddress(config->value("host", DEFAULT_HOST).toString()));
    setPort(config->value("port", DEFAULT_PORT).toInt());
    setClientId(config->value("clientid", DEFAULT_CLIENTID).toString());
    config->endGroup();

    if (debuging) {
        out << "Host: " << this->host().toString() << endl;
        out << "Port: " <<this->port() << endl;
        out << "Clienz ID: " << this->clientId() << endl;
    }

    config->beginGroup("QPiUPSMon");
    printing = config->value("printing", false).toBool();
    config->endGroup();

    if (debuging) {
        out << "Ausgabe der Nachrichten: " << printing << endl;
    }

    config->beginGroup("MQTT-Topics");
    topic_sub = config->value("sub", DEFAULT_PUBTOPIC).toString();
    topic_pub = config->value("pub", DEFAULT_SUBTOPIC).toString();
    topic_sound = config->value("sound", "").toString();

    if (!topic_sound.isEmpty()) {
        soundEnabled = true;
    }

    config->endGroup();

    if (debuging) {
        out << "Topic Subscription: " << topic_sub << endl;
        out << "Topic Publish: " << topic_pub << endl;
        if (soundEnabled) {
            out << "Topic Sound: " << topic_sound << endl;
        }
    }
}

void MqTTS::startSubscription() {
    if (debuging) {
        out << "Verbindung zu Broker hergestellt" << endl;
    }

    if(isConnectedToHost()) {
        unsubscribe(topic_sub);
        unsubscribe(topic_pub);
        if(soundEnabled) {
            unsubscribe(topic_sound);
        }

        if (debuging) {
            out << "Bestätige Verbindung." << endl;
        }

        QMQTT::Message message(0, topic_pub, "MqTTS verbunden");
        quint16 msgid = publish(message);

        if (debuging) {
            out << "publish Nachricht mit ID: " << msgid << endl;
        }

        if (debuging) {
            out << "starte subscription" << endl;
        }

        subscribe(topic_sub, 0);
        if(soundEnabled) {
            subscribe(topic_sound, 0);
        }
    }
}

void MqTTS::parseMessage(const QMQTT::Message& message) {
    QString payload(message.payload());

    if (message.topic() == topic_sub) {
        if (printing || debuging) {
            out << "Message: " << payload << endl;
        }

        // espeak-ng -vmb-de7 -k1 -s120 "Ich bin die neue Stimme von ProjektionTV"
        QString program = "espeak-ng";
        QStringList arguments;
        arguments << "-v" << "mb-de7";
        arguments << "-k" << "1";
        arguments << "-s" << "120";
        arguments << QString("\"%1\"").arg(payload);

        if (printing || debuging) {
            out << "Starte Sprachausgabe"  << endl;
        }

        QProcess::execute(program, arguments);

        if (printing || debuging) {
            out << "Sprachausgabe beendet" << endl;
        }
    } else if (soundEnabled && message.topic() == topic_sound) {
        if (printing || debuging) {
            out << "Message: " << payload << endl;
        }

        if (printing || debuging) {
            out << "Starte Soundausgabe"  << endl;
        }

        QString program = "omxplayer";
        QStringList arguments;
        arguments << "-o" << "local";
        arguments << QString("%1").arg(payload);

        QProcess::execute(program, arguments);

        if (printing || debuging) {
            out << "Soundausgabe beendet" << endl;
        }
    }
}

void MqTTS::gotSubscription(const QString& topic, const quint8) {
    if (debuging) {
        out << topic << " subscribed" << endl;
    }
}

void MqTTS::isPublished(const QMQTT::Message& message, quint16) {
    if (debuging) {
        out << "Message \"" << message.payload() << "\" published." << endl;
    }
}

void MqTTS::errorHandler(const QMQTT::ClientError error) {
    if (debuging) {
        err << "MQTT Error: " << error << endl;
    }
}

MqTTS::~MqTTS() {
    if(isConnectedToHost()) {
        if (debuging) {
            out << "Unsubscribe topic" << endl;
        }

        unsubscribe(topic_sub);

        if (debuging) {
            out << "Verbindung zum Broker trennen" << endl;
        }

        disconnectFromHost();
    }
}
