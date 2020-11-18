#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QHostAddress>

#include "mqtts.h"

int main(int argc, char *argv[]) {
    QTextStream err(stderr);
    QTextStream out(stdout);

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("MqTTS");
    QCoreApplication::setApplicationVersion("0.2");

    QCommandLineParser clParser;
    clParser.setApplicationDescription("Text to Speak over MQTT-Broker");
    clParser.addHelpOption();
    clParser.addVersionOption();
    clParser.addOptions({
        {"host", "MQTT-Broker hostaddress", "ip", DEFAULT_HOST},
        {"port", "MQTT-Broker port", "port", QString(DEFAULT_PORT)},
        {"clientid", "Client-ID for MQTT-Broker", "name", DEFAULT_CLIENTID},
        {{"s", "sounds"}, "MP3-Ausgabe aktivieren"},
        {{"p", "print"}, "Daten auf stdout ausgeben"},
        {{"c", "config"}, "Konfigurationsdatei", "file", ""},
        {{"d", "debug"}, "Debug-Asugaben aktivieren"},
    });

    clParser.process(a);

    bool print = clParser.isSet("print");
    bool debug = clParser.isSet("debug");
    bool sounds = clParser.isSet("sounds");
    QHostAddress host = clParser.isSet("host") ? QHostAddress(clParser.value("host")) : QHostAddress::LocalHost;
    quint16 port = clParser.isSet("port") ? clParser.value("port").toInt() : 1883;
    QString clientid = clParser.isSet("clientid") ? clParser.value("clientid") : "";

    if (debug)
        out << "erzeuge MqTTS-Objekt" << endl;

    MqTTS *mqtts;
    if (clParser.isSet("config")) {
        mqtts = new MqTTS(clParser.value("config"), debug);
    } else {
        mqtts = new MqTTS(host, port, clientid, sounds, print, debug);
    }

    mqtts->setCleanSession(true);
    mqtts->setAutoReconnect(true);
    mqtts->setAutoReconnectInterval(5000);

    if (debug)
        out << "verbinde mit broker" << endl;

    mqtts->connectToHost();

    return a.exec();
}
