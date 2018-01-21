# Home automation with HAP-NodeJS and ESP8266

# en 🇬🇧


### This project is currently finished but some new commits are possible.

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

In theory you can update chip over the air. There is a library for it named *Arduino OTA*. And ESPHelper library [supports OTA updates](https://github.com/ItKindaWorks/ESPHelper/blob/master/examples/GettingStarted/OTA/OTA.ino) but on my computer Arduino IDE cannot *see* my chip. And I tried to update it using [web update](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html#web-browser) but had no success at that too.

___

## Prepared files for installation 4 RGB strips in one room

For easy installation [4 new accessorry files](https://github.com/surik00/ESP8266/tree/master/Bedroom%20\(2%20chips%2C%204%20strips\)) were added. Don't forget to set a unique mac adress in every .js file on line 43. Then save these files in the `HAP-NodeJS/accessories/` folder.

To use the alarm you need to edit file [alarm.py](https://github.com/surik00/ESP8266/blob/master/Bedroom%20\(2%20chips%2C%204%20strips\)/alarm.py) and set following variables: desired wake hour, wake minute, step timeout (light becomes brighter every `TIMEOUT_STEP` seconds from 1% to 100%) and minutes before wake for light to be 100% bright (can be 0). If you set timeout equal 21 seconds (as default) light will be growing from 1% to 100% during 35 minutes before desired wake time. So if you want to wake up at 8:30 and set `MINUTES_BEFORE = 25`, then 1% brightness will be at 7:30 and 100% will be at 8:05. If during this "lightrise" you make any changes from your iOS device, it'll stop current execution, but the next alarm will work properly. For example you have a day off and don't need to wake up so early but you forgot to disable alarm. It begins raising brightness, you switch lights off from your iOS device and it stops. The next day it'll run as usual.

When flashing ESPs don't forget to change file [dual\_rgb\_strip.ino](https://github.com/surik00/ESP8266/blob/master/2%20strips%201%20chip/dual_rgb_strip.ino) on the line 3. You have to make it `"/home/RGBLight1"` for the first chip and `"/home/RGBLight2"` for the second.

Assemly doesn't differ from the one shown in [2 strips 1 chip](https://github.com/surik00/ESP8266/tree/master/2%20strips%201%20chip).

___

# ru 🇷🇺


### Данный проект завершен, однако возможны некоторые обновления.

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

Ещё в теории есть возможность обновлять чип по воздуху. Это возможно благодаря Arduino OTA, в библиотеке ESPHelper есть её поддержка, [вот пример](https://github.com/ItKindaWorks/ESPHelper/blob/master/examples/GettingStarted/OTA/OTA.ino), однако у меня чип не обнаруживается через Arduino IDE, так же не получается обновиться при помощи [веб-сервера](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html#web-browser).

___

## Готовые файлы для установки четырех лент в одну комнату
Для удобства быстрого запуска системы [добавил 4 готовых файла аксессуаров](https://github.com/surik00/ESP8266/tree/master/Bedroom%20\(2%20chips%2C%204%20strips\)). Чтобы использовать их у себя, необходимо в каждом .js файле на строке 43 изменить мак адрес на уникальный. Эти файлы необходимо поместить в папку `HAP-NodeJS/accessories/`.

Для использования будильника необходимо в файле [alarm.py](https://github.com/surik00/ESP8266/blob/master/Bedroom%20\(2%20chips%2C%204%20strips\)/alarm.py) выставить следующие параметры: час и минута желаемого пробуждения, таймаут между шагами яркости (яркость поднимается каждые `TIMEOUT_STEP` секунд от 1% до 100%), количество минут, за сколько до пробуждения свет уже должен быть ярким на 100% (можно выставить 0). При таймауте в 21 секунду (по умолчанию) свет будет плавно разгораться в течение 35 минут до времени выбранного пробуждения. Поэтому если вы планируете вставать в 8:30 и установили значение 25 в `MINUTES_BEFORE`, то 1% яркости будет уже в 7:30, а 100% будет в 8:05. При этом если вы внесете изменения в настройки света с iOS усройства, то исполнение текущего повышения яркости прервется (например, в этот день вам не надо рано вставать, а свет начал загораться. Просто выключаете свет, он перестает наращивать яркость. При этом на следующий день будильник сработает корректно.).

При прошивке чипов необходимо не забыть для второго чипа в файле [dual\_rgb\_strip.ino](https://github.com/surik00/ESP8266/blob/master/2%20strips%201%20chip/dual_rgb_strip.ino) на строке 3 изменить `"/home/RGBLight1"` на `"/home/RGBLight2"`, чтобы всё работало корректно.

Схема сборки также не отличается от той, что лежит в [2 strips 1 chip](https://github.com/surik00/ESP8266/tree/master/2%20strips%201%20chip).
