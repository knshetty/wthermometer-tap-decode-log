Setting up your ArchLinux Box to work with Arduino Uno single-board microcontroller
===================

Useful resources:
	- https://wiki.archlinux.org/index.php/Arduino#Configuration

1. Install following software from AUR repository
-------------
	a. Install Arduino IDE

		- arduino  1:1.6.5.r5-2 (Arduino prototyping platform SDK)

	b. Install "ch341" USB-2-I2C driver (Note! I have the Arduino hardware clone >> "DCcduino R3 Uno", hence the need for this USB-2-I2C driver)

		i. Preparing for ch341 driver installation

			Installing linux41-headers:

			Note! I have the following linux kernal >> "Linux helium 4.1.12-1-MANJARO #1 SMP PREEMPT Tue Oct 27 22:11:47 UTC 2015 x86_64 GNU/Linux". Hence, I install this package from the archlinux's core repository:

			- linux41-headers  4.1.12-1 (Header files and scripts for building modules for Linux41 kernel)

		ii. Install ch341 driver software:

			- ch341prog-git  r29.379ef13-1 (A simple command line tool (programmer) interfacing with ch341a)

2. Setup groups rights for your user on ArchLinux box
-------------
	Add your user name into uucp, lock, tty groups:
	$ sudo gpasswd -a <USERNAME> uucp
	$ sudo gpasswd -a <USERNAME> lock
	$ sudo gpasswd -a <USERNAME> tty

Note! It's recommed at this stage to restart your ArchLinux box or at least restart your commandline-terminal.

3. Arduino IDE Settings i.e. setup serial port, board, and programmer
-------------

	a. Steps to identify the right serial port name
		Now, plug your Arduino UNO serial USB to your Archlinux box and identify the serial port name
		$ cd /dev/
		$ ls ttyU*
In this case, if you notice "ttyUSB0". Then this is your gateway to Arduino UNO via the serial USB.

	b. Arduino IDE: Setup serial port, board, and programmer from the Tools menu
		In my case Arduino IDE 1.6.5 is installed with following settings:
		- Board: 'Arduino/Genuino Uno'
		- Port: 'ttyUSB0'
		- Programmer: 'AVRISP mkll'
