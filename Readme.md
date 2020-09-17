### Description
There are two programs: myController and myDevice. The programs will run independently but synchronize by having the myDevice program install a resource manager (aka device driver) within QNX Neutrino.
The device status will then be visible to the myController by actively reading directly from the device, or by receiving alert pulses from the device.

### Running the Programs...

Startup the device: # myDevice &
Startup the controller: # myController &

### Then you will be able to test your programs using a command scripts containing commands such as
echo status value > /dev/local/mydevice
echo alert 1 > /dev/local/mydevice
echo status closed > /dev/local/mydevice
echo alert 2 > /dev/local/mydevice

### The device should operate as follow
1. update its internal status buffer with: value. Where value is a string of characters following the status <space> prefix. Please note a blank space separates status and value. Allow for up to 255 characters in the value. This value will then be available to the controller if the device is read (e.g., using fscanf(….)).
2. send an “alert”, in the form of a pulse to the controller, to notify that the alert <small_int> event has been written to the device. From above, I use the echo command to write to the device, /dev/local/mydevice (FQN – fully qualified name). The value of <small_int> must be in the range 1 to 99, inclusive. You can assume integer values are sent; you cannot assume the integer value is in the range of 1 – 99 (inclusive). If the integer value is outside this range, print a warning message (“Integer value is not between 1 and 99 (inclusive)”), but do not terminate --- the device program is to continue running (i.e. its fault tolerant, well a little at least).
Upon startup, the myController program should read the status of the device, and then whenever a pulse is received, the myController program should output the value of the integer sent with the pulse, read the status of the device again, and output a message to the console with the current device status.