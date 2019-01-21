This is the official code from Epson, I have no right on it.
I added a few little functions to make it easy to set a position or a velocity, and stop the motor.
You can take inspiration from this code to run an EPOS motor easily.

You may want to change the default "EPOS4" in settings for the right one, "EPOS2" for me.

Wiring :
connect a source of power to the controller
connect the motor to the controller
connect the controller to your computer via usb

Installation of the libraries :
install EPOS Linux Library (I uploaded the file. If you want to find it yourself, go to 
https://www.maxonmotor.com/maxon/view/catalog?etcc_med=ID+Teaser&etcc_cmp=ID-Teaser-Homepage-Drivesystem&etcc_cu=onsite&etcc_var=%5bcom%5d%23en%23_d_&etcc_plc=home
controller ->anyOne -> Downloads -> Software -> EPOS Linux library) on your machine. Make the location match with the Makefile or just modify the Makefile for the right path.

Run make

The motor should move to 2 or 3 positions, then rotate for 1 second and then stop.

If not, make sure the light on the controller is not red. It can be due to the placement of the cables, I don't know why.
