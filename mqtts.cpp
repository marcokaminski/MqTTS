#include <qmqtt_client.h>
#include <qmqtt_message.h>
#include <QProcess>

#include "mqtts.h"

MqTTS::MqTTS(const QHostAddress& host, const quint16 port, QString clientid, bool sounds, bool print, bool debug, QObject *parent) : QMQTT::Client(host, port, parent), out(stdout), err(stderr) {
    connect(this, SIGNAL(connected()), this, SLOT(startSubscription()));
    connect(this, SIGNAL(received(const QMQTT::Message&)), this, SLOT(parseMessage(const QMQTT::Message&)));
    connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(gotSubscription(const QString&, const quint8)));
    connect(this, SIGNAL(published(const QMQTT::Message&, quint16)), this, SLOT(isPublished(const QMQTT::Message&, quint16)));
    connect(this, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(errorHandler(const QMQTT::ClientError)));

    debugging = debug;
    printing = print;
    soundEnabled = sounds;

    setClientId(clientid);

    if (debugging) {
        out << "Host: " << this->host().toString() << endl;
        out << "Port: " << this->port() << endl;
        out << "Client ID: " << this->clientId() << endl;
        out << "Ausgabe der Nachrichten: " << printing << endl;
        out << "Topic Subscription: " << topic_sub << endl;
        out << "Topic Publish: " << topic_pub << endl;
        if (soundEnabled) {
            out << "Topic Sound (play): " << topic_sound << "/*/play" << endl;
            out << "Topic Sound (file): " << topic_sound << "/*/file" << endl;
        }
    }
}

MqTTS::MqTTS(QString configFile, bool debug, QObject*) : out(stdout), err(stderr) {
    connect(this, SIGNAL(connected()), this, SLOT(startSubscription()));
    connect(this, SIGNAL(received(const QMQTT::Message&)), this, SLOT(parseMessage(const QMQTT::Message&)));
    connect(this, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(gotSubscription(const QString&, const quint8)));
    connect(this, SIGNAL(published(const QMQTT::Message&, quint16)), this, SLOT(isPublished(const QMQTT::Message&, quint16)));
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
    topic_sound = config->value("sound", "").toString();

    if (!topic_sound.isEmpty()) {
        soundEnabled = true;
    }

    config->endGroup();

    if (debugging) {
        out << "Topic Subscription: " << topic_sub << endl;
        out << "Topic Publish: " << topic_pub << endl;
        if (soundEnabled) {
            out << "Topic Sound (play): " << topic_sound << "/*/play" << endl;
            out << "Topic Sound (file): " << topic_sound << "/*/file" << endl;
        }
    }
}

void MqTTS::startSubscription() {
    if (debugging) {
        out << "Verbindung zu Broker hergestellt" << endl;
    }

    if(isConnectedToHost()) {
        unsubscribe(topic_sub);
        unsubscribe(topic_pub);
        if(soundEnabled) {
            unsubscribe(topic_sound);
        }

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
        if(soundEnabled) {
            subscribe(topic_sound + "/*/play", 0);
            subscribe(topic_sound + "/*/file", 0);
        }
    }
}

void MqTTS::parseMessage(const QMQTT::Message& message) {
    QString payload(message.payload());

    if (message.topic() == topic_sub) {
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

        QProcess::execute(program, arguments);

        if (printing || debugging) {
            out << "Sprachausgabe beendet" << endl;
        }
    } else if (soundEnabled && message.topic().contains("play")) {
        if (printing || debugging) {
            out << "Topic: " << message.topic() << endl;
            out << "Message: " << payload << endl;
        }

        if (payload == "true") {
            QStringList topicParts = message.topic().split('/');
            QString sound = topicParts[topicParts.size() - 2];
            QString file = filelist.value(sound, "");

            if (file.isEmpty()) {
                out << "Keine Datei zum abspielen im MqTT-Broker hinterlegt." << endl;
            } else {
                if (printing || debugging) {
                    out << "Starte Soundausgabe"  << endl;
                }

                QString program = "mpg123";
                QStringList arguments;
                arguments << QString("%1").arg(file);

                QProcess::execute(program, arguments);

                if (printing || debugging) {
                    out << "Soundausgabe beendet" << endl;
                }
            }

            quint16 msgid = publish(QMQTT::Message(1, message.topic(), "false"));

            if (debugging) {
                out << "publish Nachricht mit ID: " << msgid << " on topic: " << message.topic() << endl;
            }
        }
    } else if (soundEnabled && message.topic().contains("file")) {
        if (printing || debugging) {
            out << "Topic: " << message.topic() << endl;
            out << "Message: " << payload << endl;
        }

        QStringList topicParts = message.topic().split('/');
        QString sound = topicParts[topicParts.size() - 2];
        QString file = payload;

        if (printing || debugging) {
            out << "Sound: " << sound << endl;
            out << "File: " << file << endl;
        }

        filelist.insert(sound, file);
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
