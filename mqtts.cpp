#include <qmqtt_client.h>
#include <qmqtt_message.h>
#include <QProcess>

#include "mqtts.h"

MqTTS::MqTTS(const QHostAddress& host, const quint16 port, QString clientid, bool print, bool debug, QObject *parent) : QMQTT::Client(host, port, parent), out(stdout), err(stderr) {
    connect(this, SIGNAL(connected()), this, SLOT(startSubscription()));
    connect(this, SIGNAL(received(const QMQTT::Message&)), this, SLOT(parseMessage(const QMQTT::Message&)));
    connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(gotSubscription(const QString&, const quint8)));
    connect(this, SIGNAL(published(const QMQTT::Message&, quint16)), this, SLOT(isPublished(const QMQTT::Message&, quint16)));
    connect(this, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(errorHandler(const QMQTT::ClientError)));

    debugging = debug;
    printing = print;

    setClientId(clientid);

    if (debugging) {
        out << "Host: " << this->host().toString() << endl;
        out << "Port: " << this->port() << endl;
        out << "Client ID: " << this->clientId() << endl;
        out << "Ausgabe der Nachrichten: " << printing << endl;
        out << "Topic Subscription: " << topic_sub << endl;
        out << "Topic Publish: " << topic_pub << endl;
    }
}

MqTTS::MqTTS(QString configFile, bool debug, QObject*) : out(stdout), err(stderr) {
    connect(this, SIGNAL(connected()), this, SLOT(startSubscription()));
    connect(this, SIGNAL(received(const QMQTT::Message& message)), this, SLOT(parseMessage(const QMQTT::Message& message)));
    connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(gotSubscription(const QString&, const quint8)));
    connect(this, SIGNAL(published(const QMQTT::Message&, const quint8)), this, SLOT(isPublished(const QMQTT::Message&, const quint8)));
    connect(this, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(errorHandler(const QMQTT::ClientError)));

    config = new QSettings(configFile, QSettings::IniFormat);
    debugging = debug;

    if (debugging) {
        out << "Lese Konfigurationsdatei: " << configFile << endl;
    }

    config->beginGroup("MQTT-Broker");
    setHost(QHostAddress(config->value("host", DEFAULT_HOST).toString()));
    setPort(config->value("port", DEFAULT_PORT).toInt());
    setClientId(config->value("clientid", DEFAULT_CLIENTID).toString());
    config->endGroup();

    if (debugging) {
        out << "Host: " << this->host().toString() << endl;
        out << "Port: " <<this->port() << endl;
        out << "Clienz ID: " << this->clientId() << endl;
    }

    config->beginGroup("QPiUPSMon");
    printing = config->value("printing", false).toBool();
    config->endGroup();

    if (debugging) {
        out << "Ausgabe der Nachrichten: " << printing << endl;
    }

    config->beginGroup("MQTT-Topics");
    topic_sub = config->value("sub", DEFAULT_PUBTOPIC).toString();
    topic_pub = config->value("pub", DEFAULT_SUBTOPIC).toString();
    config->endGroup();

    if (debugging) {
        out << "Topic Subscription: " << topic_sub << endl;
        out << "Topic Publish: " << topic_pub << endl;
    }
}

void MqTTS::startSubscription() {
    if (debugging) {
        out << "Verbindung zu Broker hergestellt" << endl;
    }

    if(isConnectedToHost()) {
        unsubscribe(topic_sub);
        unsubscribe(topic_pub);

        if (debugging) {
            out << "Bestätige Verbindung." << endl;
        }

        QMQTT::Message message(0, topic_pub, "MqTTS verbunden");
        quint16 msgid = publish(message);

        if (debugging) {
            out << "publish Nachricht mit ID: " << msgid << endl;
        }

        if (debugging) {
            out << "starte subscription" << endl;
        }

        subscribe(topic_sub, 0);
    }
}

void MqTTS::parseMessage(const QMQTT::Message& message) {
    QString payload(message.payload());

    if (printing || debugging) {
        out << "Message: " << payload << endl;
    }

    // espeak-ng -vmb-de7 -k1 -s120 "Ich bin die neue Stimme von ProjektionTV"
    QString program = "espeak-ng";
    QStringList arguments;
    arguments << "-v" << "mb-de7";
    arguments << "-k" << "1";
    arguments << "-s" << "120";
    arguments << QString("\"%1\"").arg(payload);

    if (printing || debugging) {
        out << "Starte Sprachausgabe"  << endl;
    }

    QProcess *espeak = new QProcess();
    espeak->start(program, arguments);

    espeak->waitForFinished(10000);

    if (printing || debugging) {
        out << "Sprachausgabe beendet" << endl;
    }
}

void MqTTS::gotSubscription(const QString& topic, const quint8) {
    if (debugging) {
        out << topic << " subscribed" << endl;
    }
}

void MqTTS::isPublished(const QMQTT::Message& message, quint16) {
    if (debugging) {
        out << "Message \"" << message.payload() << "\" published." << endl;
    }
}

void MqTTS::errorHandler(const QMQTT::ClientError error) {
    if (debugging) {
        err << "MQTT Error: " << error << endl;
    }
}

MqTTS::~MqTTS() {
    if(isConnectedToHost()) {
        if (debugging) {
            out << "Unsubscribe topic" << endl;
        }

        unsubscribe(topic_sub);

        if (debugging) {
            out << "Verbindung zum Broker trennen" << endl;
        }

        disconnectFromHost();
    }
}
