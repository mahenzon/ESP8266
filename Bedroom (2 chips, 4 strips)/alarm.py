import time
import logging
import datetime
import paho.mqtt.client as mqtt
from apscheduler.schedulers.background import BlockingScheduler


logging.basicConfig(format=u'%(filename)s [LINE:%(lineno)d]#%(levelname)-8s [%(asctime)s]  %(message)s',
                    level=logging.WARNING, filename='./alarm.log')

BROKER_ADDRESS = "localhost"

WAKE_HOUR = 8
WAKE_MINUTE = 30

TIMEOUT_STEP = 21
MINUTES_BEFORE = 35


def mkthree(color):
    line = str(color)
    while len(line) < 3:
        line = "0" + line
    return line


class AlarmClock(object):

    def __init__(self):
        super(AlarmClock, self).__init__()
        self.is_running = True
        logging.info('Inited AlarmClock')

    def lightit(self):
        logging.info('Task started')
        self.is_running = True

        client = mqtt.Client("AlarmClock")
        client.on_message = self._on_message
        client.connect(BROKER_ADDRESS)
        client.subscribe([("/home/RGBLight1", 0), ("/home/RGBLight2", 0)])
        client.loop_start()

        client.publish("/home/RGBLight1", "pl1S")
        client.publish("/home/RGBLight1", "pr1S")
        client.publish("/home/RGBLight2", "pl1S")
        client.publish("/home/RGBLight2", "pr1S")
        time.sleep(.1)

        client.publish("/home/RGBLight1", "cl001,001,001S")
        client.publish("/home/RGBLight1", "cr001,001,001S")
        client.publish("/home/RGBLight2", "cl001,001,001S")
        client.publish("/home/RGBLight2", "cr001,001,001S")
        time.sleep(.1)

        for i in range(1, 101):
            if self.is_running:
                color = mkthree(round(255 * (i / 100)))
                client.publish("/home/RGBLight1", "cl{},{},{}S".format(color, color, color))
                client.publish("/home/RGBLight1", "cr{},{},{}S".format(color, color, color))
                client.publish("/home/RGBLight2", "cl{},{},{}S".format(color, color, color))
                client.publish("/home/RGBLight2", "cr{},{},{}S".format(color, color, color))
                time.sleep(TIMEOUT_STEP)

        logging.info('Task finished')

    def _on_message(self, client, userdata, message):
        logging.debug('Message: {}'.format(message.payload))
        msg = message.payload.decode()
        if not msg.endswith('S'):
            client.loop_stop()
            self.is_running = False
            logging.info('Stopped execution in case of new internal updates')


if __name__ == '__main__':
    logging.debug('Started main')
    scheduler = BlockingScheduler()
    alarm_start_time = datetime.datetime(1970, 1, 1, WAKE_HOUR, WAKE_MINUTE) - datetime.timedelta(seconds=(TIMEOUT_STEP * 100) + (MINUTES_BEFORE * 60))
    logging.info('Counted datetime: {}'.format(str(alarm_start_time)))
    alarm = AlarmClock()
    scheduler.add_job(alarm.lightit, 'cron', day_of_week='mon-fri', hour=alarm_start_time.hour, minute=alarm_start_time.minute)
    scheduler.start()
