# Home automation with HAP-NodeJS and ESP8266

`en`<br>
Current project is in development state. It will be updating weekly or so.

To make iOS devices discovery HomeKit devices [HAP-NodeJS](https://github.com/KhaosT/HAP-NodeJS) is required.<br>
Following libraries needed to be installed (for installation use `npm install %packetname%`):

* `onecolor`
* `mqtt`

For ESP8266 flashing you need Arduino IDE and the following libraries:

* [ESPHelper](https://github.com/ItKindaWorks/ESPHelper)
* [Metro](https://www.pjrc.com/teensy/td_libs_Metro.html)
* [pubsubclient](https://github.com/knolleary/pubsubclient)

Folder *2 strips 1 chip* contains 2 .js files for HAP-NodeJS server and 1 .ino file for flashing ESP8266 using Arduino IDE. Together they allow to control 2 independent RGB strips using 1 ESP8266 chip.


P.S.:

1. Live tests didn't happen yet.
2. Breadboard layouts are in development state.


<hr>
`ru`<br>
Данный проект ещё в разработке. Обновления будут появляться примерно раз в неделю.

Для обнаружения iOS девайсом устройств HomeKit требуется [HAP-NodeJS](https://github.com/KhaosT/HAP-NodeJS).<br>
Для работы представленных модулей требуется установка следующих библиотек для node-JS (для установки используйте `npm install %имяпакета%`):

* `onecolor`
* `mqtt`


Для прошивки чипа ESP8266 требуется Arduino IDE и установка следующих библиотек:

* [ESPHelper](https://github.com/ItKindaWorks/ESPHelper)
* [Metro](https://www.pjrc.com/teensy/td_libs_Metro.html)
* [pubsubclient](https://github.com/knolleary/pubsubclient)

В папке *2 strips 1 chip* лежит 2 файла для HAP-NodeJS, представляющие 2 независимых HomeKit устройства, и .ino файл, которым надо прошить ESP8266. Вместе они позволяют управлять двумя независимыми RGB лентами.


P.S.:

1. Тесты на чипе ещё не проводились, только грядут.
2. Схемы макетной платы в процессе.