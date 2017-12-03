# Home automation with HAP-NodeJS and ESP8266

`en`<br>
This project is currently finished but some new commits are possible.

To make iOS devices discovery HomeKit devices [HAP-NodeJS](https://github.com/KhaosT/HAP-NodeJS) is required.<br>
Following libraries needed to be installed (for installation use `npm install %packetname%`):

* `onecolor`
* `mqtt`

For ESP8266 flashing you need Arduino IDE and the following libraries:

* [ESPHelper](https://github.com/ItKindaWorks/ESPHelper)
* [Metro](https://www.pjrc.com/teensy/td_libs_Metro.html)
* [pubsubclient](https://github.com/knolleary/pubsubclient)

Folder *2 strips 1 chip* contains two .js files for HAP-NodeJS server and one .ino file for flashing ESP8266 using Arduino IDE. Together they allow to control two independent RGB strips using one ESP8266 chip.

Live tests were made, chip controls two RGB strips without errors and powers on / off power supply unit when needed.

### FLASHING
As a result I used all GPIO ports on ESP8266-12. **GPIO15 always** has to be connected to ground to make chip "boot" from internal storage. Built-in LED is under control of **GPIO2**. I don't want to use **GPIO0** because if it is connected to ground while powering on esp12 it will boot in flashing mode. Unfortunately we also cannot use GPIO9 because when trying to write (analogWrite / digitalWrite) anything on this port causes immediate reboot of the whole chip.

Speaking of flashing ESP8266-12. I reccomend to disconnect everything from chip leaving only required for flashing pins. There's a pic of wiring the esp12 for flasing in the [stuff](https://github.com/surik00/ESP8266/tree/master/stuff) folder. Here's how it looks like as text:

* TX to RX of a UART converter
* RX to TX of a UART converter
* GPIO to GND (while powering on)
* EN/CH_PD to VCC (always)
* GPIO15 to GND (always)

Transistors used in this project: **IRLML6244TRPBF**

Notice that your UART converter can output two currencies: 5v and 3.3v. Check if it is in 3.3v mode because 5v will burn the chip.

In theory you can update chip over the air. There is a library for it named *Arduino OTA*. And ESPHelper library [supports OTA updates](https://github.com/ItKindaWorks/ESPHelper/blob/master/examples/OTA/OTA.ino) but on my computer Arduino IDE cannot *see* my chip. And I tried to update it using [web update](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html#web-browser) but had no success at that too.

<hr>

`ru`
<br>
Данный проект завершен, однако возможны некоторые обновления.

Для обнаружения iOS девайсом устройств HomeKit требуется [HAP-NodeJS](https://github.com/KhaosT/HAP-NodeJS).<br>
Для работы представленных модулей требуется установка следующих библиотек для node-JS (для установки используйте `npm install %имяпакета%`):

* `onecolor`
* `mqtt`


Для прошивки чипа ESP8266 требуется Arduino IDE и установка следующих библиотек:

* [ESPHelper](https://github.com/ItKindaWorks/ESPHelper)
* [Metro](https://www.pjrc.com/teensy/td_libs_Metro.html)
* [pubsubclient](https://github.com/knolleary/pubsubclient)

В папке *2 strips 1 chip* лежит два файла для HAP-NodeJS, представляющие два независимых HomeKit устройства, и один .ino файл, которым надо прошить ESP8266. Вместе они позволяют управлять двумя независимыми RGB лентами.

Тесты проведены, чип корректно управляет двумя независимыми лентами и своевременно включает и выключает компьютерный блок питания.

### ПРОШИВКА
В итоге я занял все доступные GPIO на чипе. **GPIO15 всегда** должен быть замкнут на землю, чтобы чип загружался со встроенной памяти. На **GPIO2** *сидит* встроенный светодиод. **GPIO0** я не хочу использовать, так как если при подаче питания он замкнут на землю (GND), то ESP-12 загружается в режим прошивки. По неизвестной мне причине при попытке что-то писать (analogWrite / digitalWrite) на GPIO9 происходит какая-то ошибка, и чип полностью перезагружается, так что об этом пине мы тоже забываем.

К слову о прошивке. Рекомендую отключать всё от пинов, кроме необходимых для прошивки. Картинку оставляю в папке [stuff](https://github.com/surik00/ESP8266/tree/master/stuff). Текстом это выглядит так:

* TX к RX UART конвертера
* RX к TX UART конвертера
* GPIO0 на GND (при подаче питания)
* EN/CH_PD на VCC (всегда)
* GPIO15 на GND (всегда)

Модель используемых транзисторов: **IRLML6244TRPBF**

Стоит отметить, что UART конвертер зачастую может работатать в двух режимах: 5в и 3.3в. Перед подключением убедитесь, что сейчас режим 3.3в, так как 5в просто сожжет чип.

Ещё в теории есть возможность обновлять чип по воздуху. Это возможно благодаря Arduino OTA, в библиотере ESPHelper есть её поддержка, [вот пример](https://github.com/ItKindaWorks/ESPHelper/blob/master/examples/OTA/OTA.ino), однако у меня чип не обнаруживается через Arduino IDE, так же не получается обновиться при помощи [веб-сервера](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html#web-browser).