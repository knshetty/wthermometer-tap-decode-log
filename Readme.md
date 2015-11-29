# Arduino + Nodejs App: Wireless Thermometer - Tapping, Decoding, & Logging To IoT Cloud

This project attempts the following:
* Hardware tapping of the [Biltema wireless cooking thermometer's](http://www.biltema.fi/BiltemaDocuments/Manuals/84-0867_man.pdf) receiver-side signal. Here the signal carries temperature measurement every 5 seconds.
* Using Arduino Uno single-board microcontroller, we decode the above signal and make it available on this Arduino's serial port. Here decoding refers to interpreting the above signal, which are pulse intervals into binary data packets.
* On a separate Linux box, we use Nodejs & read Arduino's serial port. Then log the read temperature measurement to IoT cloud (i.e. to [ThingSpeak](https://thingspeak.com) channel)

## Prerequisites

You will need the following things properly installed on your computer.

* [Git](http://git-scm.com/)
* [Node.js](http://nodejs.org/) (with NPM)

## Installation

1. `$ git clone https://github.com/knshetty/wthermometer-tap-decode-log.git`
2. Change into the newly created directory
3. `$ npm install`
4. `$ npm install -g coffee-script`
5. `$ coffee app.coffee`


## Further Reading / Useful Links
* https://github.com/DanialK/Arduino-NodeJs-Serialport
* http://danialk.github.com/blog/2014/04/12/arduino-and-nodejs-communication-with-serial-ports/
* Other similar projects:
  * https://github.com/GianlucaGuarini/nodejs-arduino-example
  * https://github.com/pimatic/rfcontroljs
