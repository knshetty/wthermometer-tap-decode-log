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
    if((data[0..5] isnt 'ERROR:') or (data[0..5] isnt 'Fault:'))
      temperatureFahrenheit = "#{parseInt(data[0..3], 2)}#{parseInt(data[4..7], 2)}#{parseInt(data[8..11], 2)}"
      temperatureCelsius = "#{(temperatureFahrenheit - 32) * 0.56}"
      tcParts = temperatureCelsius.split('.')
      temperature = tcParts[0]
      console.log temperature
      if temperature[0..2] isnt 'NaN'
        client.updateChannel(channelId, {field1: temperature})
	)
)
