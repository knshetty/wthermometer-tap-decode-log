Com = require 'serialport'
ThingSpeakClient = require 'thingspeakclient'

serialPort = new Com.SerialPort("/dev/ttyUSB0", {
  baudrate: 9600
	parser: Com.parsers.readline('\n')
})

# Instantiate a ThinkSpeak IoT-cloud client
client = new ThingSpeakClient({ server:'https://api.thingspeak.com', useTimeoutMode: false})
# Attach a channel with only a writekey
channelId = 68802
client.attachChannel(channelId, { writeKey:'X0H0DM567K9B067G'})

serialPort.on('open', ->
  console.log 'Serial Port Opened...'
  serialPort.on('data', (data) ->
    #console.log data
    if((data[0..5] isnt 'ERROR:') or (data[0..5] isnt 'Fault:') or (data[0..2] isnt 'NaN'))
      digit1 = parseInt(data[0..3], 2)
      digit2 = parseInt(data[4..7], 2)
      digit3 = parseInt(data[8..11], 2)
      # Note! If the digit2 is greater than 9 i.e. for e.g. 12 then the
      # transmitter batteries are low on power.
      if (digit2 < 10)
        temperatureFahrenheit = "#{digit1}#{digit2}#{digit3}"
        #console.log temperatureFahrenheit
        # Note! Min-Max temperature reading capability of this wireless
        # thermometer 32F = 0C & 410F = 210C
        if ((temperatureFahrenheit >= 32) or (temperatureFahrenheit <= 410))
          temperatureCelsius = "#{(temperatureFahrenheit - 32) * 0.56}"
          tcParts = temperatureCelsius.split('.')
          temperature = tcParts[0]
          console.log temperature
          client.updateChannel(channelId, {field1: temperature})
	)
)
