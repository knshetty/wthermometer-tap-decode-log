com = require('serialport')
serialPort = new com.SerialPort("/dev/ttyUSB0", {
  baudrate: 9600
	parser: com.parsers.readline('\n')
})

serialPort.on('open', ->
	console.log 'Serial Port Opened...'
	serialPort.on('data', (data) ->
		console.log data
	)
)
