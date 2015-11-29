com = require('serialport')
serialPort = new com.SerialPort("/dev/ttyUSB0", {
  baudrate: 9600
	parser: com.parsers.readline('\n')
})

serialPort.on('open', ->
  console.log 'Serial Port Opened...'
  serialPort.on('data', (data) ->
    #console.log data
    if((data[0..5] isnt 'ERROR:') or (data[0..5] isnt 'Fault:'))
      temperatureFahrenheit = "#{parseInt(data[0..3], 2)}#{parseInt(data[4..7], 2)}#{parseInt(data[8..11], 2)}"
      temperatureCelsius = "#{(temperatureFahrenheit - 32) * 0.56}"
      tcParts = temperatureCelsius.split('.')
      console.log tcParts[0]
	)
)
