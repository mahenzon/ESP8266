var Accessory = require('../').Accessory;
var Service = require('../').Service;
var Characteristic = require('../').Characteristic;
var uuid = require('../').uuid;
var color = require('onecolor');
var mqtt = require('mqtt');

var MQTT_IP = 'localhost';  // your MQTT broker server IP. In my case it's hosted on the same machine
var lightTopic = '/home/RGBLight1';  // MQTT topic to publish updates

var options = {
  port: 1883,
  host: MQTT_IP,
  clientId: 'RGB_Strip_2'  // name to be introduced for MQTT broker
};
var client = mqtt.connect(options);
client.on('message', function(topic, message){  //not sure if we need if here.

});


// function to format numbers with fixed length (3) to make it easy to parse on ESP8266
function formatNumberLength(num) {
    var l = "" + num;
    while (l.length < 3) {
        l = "0" + l;
    };
    return l;
};

var LightController = {
  name: "RGB Strip Right", //name of accessory
  pincode: "111-11-111", // password to add HomeKit device. You can change numbers keeping '-' symbols
  username: "AB:AA:AA:AA:AA:AA", // MAC like address used by HomeKit to differentiate accessories.
  manufacturer: "Suren Khorenyan", //manufacturer (optional)
  model: "v1.1", //model (optional)
  serialNumber: "RGB_S_003", //serial number (optional)


  currentPower: 0,
  lastPower: 0,

  currentHue: 0,
  currentSaturation: 0,
  currentBrightness: 0,

  lastHue: 0,
  lastSaturation: 0,
  lastBrightness: 0,


  outputLogs: false,  // change to true for debug purposes

  setPower: function(status) {
    if(this.outputLogs) console.log("Setting the '%s' %s", this.name, status ? "on" : "off");
    this.currentPower = status;
    this.updatePower();
  },

  getPower: function() {
    if(this.outputLogs) console.log("'%s' is %s!", this.name, this.currentPower ? "on" : "off");
    return this.currentPower ? true : false;
  },

  setBrightness: function(brightness) {
    if(this.outputLogs) console.log("Setting '%s' brightness to %s", this.name, brightness);
    this.currentBrightness = brightness;
    this.updateLight();
  },

  getBrightness: function() {
    if(this.outputLogs) console.log("'%s' brightness is %s", this.name, this.currentBrightness);
    return this.currentBrightness;
  },

  setSaturation: function(saturation) {
    if(this.outputLogs) console.log("Setting '%s' saturation to %s", this.name, saturation);
    this.currentSaturation = saturation;
    this.updateLight();
  },

  getSaturation: function() {
    if(this.outputLogs) console.log("'%s' saturation is %s", this.name, this.currentSaturation);
    return this.currentSaturation;
  },

  setHue: function(hue) {
    if(this.outputLogs) console.log("Setting '%s' hue to %s", this.name, hue);
    this.currentHue = hue;
    this.updateLight();
  },

  getHue: function() {
    if(this.outputLogs) console.log("'%s' hue is %s", this.name, this.currentHue);
    return this.currentHue;
  },


  identify: function() {
    if(this.outputLogs) console.log("Identify the '%s'", this.name);
  },

// this function checks if any changes about power were made and publishes a new MQTT message if true
  updatePower: function() {
    if(this.lastPower != this.currentPower){
      if(this.outputLogs) console.log("Turning '%s' %s", this.name, this.currentPower ? "on" : "off");
      if(this.currentPower){
        client.publish(lightTopic, 'pr1');
      }else{
        client.publish(lightTopic, 'pr0');
      }
      this.lastPower = this.currentPower;
    }
  },

// this function checks if any changes about color were made and publishes a new MQTT message if true
  updateLight: function() {
    if(this.lastSaturation != this.currentSaturation || this.lastHue != this.currentHue || this.lastBrightness != this.currentBrightness){
      if(this.outputLogs) console.log("Hue %s, Sat %s, Bri %s", this.currentHue, this.currentSaturation, this.currentBrightness);

// As we need to control RGB light and iOS sends color in HSB (HSV) format, we need to convert it.
      myColor = new color.HSV(this.currentHue/360, this.currentSaturation/100, this.currentBrightness/100).rgb();  //pass values in 0..1 range
      // creating new RGB values. Multiply by 255 bc it returnes value in 0..1 range
      red = Math.round(myColor.red()*255);
      green = Math.round(myColor.green()*255);
      blue = Math.round(myColor.blue()*255);

      // formatting string to publish
      // ex string 'c090,000,157' where 'c' is for our ESP to recognize new color command and 3 numbers for red, green and blue in fixed places
      toPublish = 'cr' + formatNumberLength(red) + ',' + formatNumberLength(green) + ',' +  formatNumberLength(blue);
      client.publish(lightTopic, toPublish);

      this.lastHue = this.currentHue;
      this.lastSaturation = this.currentSaturation;
      this.lastBrightness = this.currentBrightness;
    }
  }
}

// Generate a consistent UUID for our light Accessory that will remain the same even when
// restarting our server. We use the `uuid.generate` helper function to create a deterministic
// UUID based on an arbitrary "namespace" and the word "light".
var lightUUID = uuid.generate('hap-nodejs:accessories:light' + LightController.name);

// This is the Accessory that we'll return to HAP-NodeJS that represents our light.
var lightAccessory = exports.accessory = new Accessory(LightController.name, lightUUID);

// Add properties for publishing (in case we're using Core.js and not BridgedCore.js)
lightAccessory.username = LightController.username;
lightAccessory.pincode = LightController.pincode;

// set some basic properties (these values are arbitrary and setting them is optional)
lightAccessory
  .getService(Service.AccessoryInformation)
    .setCharacteristic(Characteristic.Manufacturer, LightController.manufacturer)
    .setCharacteristic(Characteristic.Model, LightController.model)
    .setCharacteristic(Characteristic.SerialNumber, LightController.serialNumber);

// listen for the "identify" event for this Accessory
lightAccessory.on('identify', function(paired, callback) {
  LightController.identify();
  callback();
});

// Add the actual Lightbulb Service and listen for change events from iOS.
// We can see the complete list of Services and Characteristics in `lib/gen/HomeKitTypes.js`
lightAccessory
  .addService(Service.Lightbulb, LightController.name) // services exposed to the user should have "names" like "Light" for this case
  .getCharacteristic(Characteristic.On)
  .on('set', function(value, callback) {
    LightController.setPower(value);
    callback();
  })
  // We want to intercept requests for our current power state so we can query the hardware itself instead of
  // allowing HAP-NodeJS to return the cached Characteristic.value.
  .on('get', function(callback) {
    callback(null, LightController.getPower());
  });


// also add an "optional" Characteristic for Brightness
lightAccessory
  .getService(Service.Lightbulb)
  .addCharacteristic(Characteristic.Brightness)
  .on('set', function(value, callback) {
    LightController.setBrightness(value);
    callback();
  })
  .on('get', function(callback) {
    callback(null, LightController.getBrightness());
  });

// also add an "optional" Characteristic for Saturation
lightAccessory
  .getService(Service.Lightbulb)
  .addCharacteristic(Characteristic.Saturation)
  .on('set', function(value, callback) {
    LightController.setSaturation(value);
    callback();
  })
  .on('get', function(callback) {
    callback(null, LightController.getSaturation());
  });

// also add an "optional" Characteristic for Hue
lightAccessory
  .getService(Service.Lightbulb)
  .addCharacteristic(Characteristic.Hue)
  .on('set', function(value, callback) {
    LightController.setHue(value);
    callback();
  })
  .on('get', function(callback) {
    callback(null, LightController.getHue());
  });
