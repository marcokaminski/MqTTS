# MqTTS

Reads text from an MQTT broker and uses espaek-ng as a speech synthesizer to read the text out loud.

## Install easpeak-ng on Raspberry Pi ##
    sudo apt install espeak-ng

### Testing espeak-ng ###
    espeak-ng -vde -k1 -s120 "Ich bin die neue Stimme von ProjektionTV"

### Install MBROLA on Raspbian ###
    wget http://ftp.de.debian.org/debian/pool/contrib/m/mbrola/mbrola_3.02b+dfsg-4_armhf.deb
    sudo dpkg -i mbrola_3.02b+dfsg-4_armhf.deb

### Install german MBROLA Voices ###
    sudo apt install mbrola-de1
    sudo apt install mbrola-de2
    sudo apt install mbrola-de3
    sudo apt install mbrola-de4
    sudo apt install mbrola-de5
    sudo apt install mbrola-de6
    sudo apt install mbrola-de7
    sudo apt install mbrola-de8

### Testing MBROLA Voices ###
    espeak-ng -vmb-de7 -k1 -s120 "Ich bin die neue Stimme von ProjektionTV"

### Install Qt5 ###
	sudo apt install qt5-default
    
### Install MQTT-Libs ###
    sudo apt install libqt5websockets5-dev
    git clone https://github.com/emqx/qmqtt.git
    cd qmqtt
    qmake
    make
    sudo make install
    sudo ldconfig

## Build ##
    git clone https://github.com/marcokaminski/MqTTS.git
    cd MqTTS
    qmake
    make

## Usage ##
Usage: ./MqTTS [options]  
Text to Speak over MQTT-Broker  

Options:  
  -h, --help           Displays this help.  
  -v, --version        Displays version information.  
  --host <ip>          MQTT-Broker hostaddress  
  --port <port>        MQTT-Broker port  
  --clientid <name>    Client-ID for MQTT-Broker  
  -s, --sounds         MP3-Ausgabe aktivieren  
  -p, --print          Daten auf stdout ausgeben  
  -c, --config <file>  Konfigurationsdatei  
  -d, --debug          Debug-Asugaben aktivieren  

## Dependencies ##

### QMQTT ###
https://github.com/emqx/qmqtt
commit: 1fc3e1cbb5d14ff60da8ab46549fc846e8f9f6b5
