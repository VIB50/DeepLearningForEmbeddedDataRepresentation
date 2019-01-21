This is the camera I used in the end.

Once the libraries are installed, just run the Makefile, but it is very tricky to get it installed.

Here are some links that were very helpful to make me install it :

https://www.jetsonhacks.com/2016/06/20/intel-realsense-camera-installation-nvidia-jetson-tk1/

https://www.jetsonhacks.com/2016/06/23/realsense-camera-ros-install-jetson-tk1/

https://github.com/jetsonhacks/installLibrealsense/commit/25f0b1b700c0f9991d768ace4ffb15b589694d38


Installation instructions in a nutshell :

$ git clone https://github.com/jetsonhacks/installLibrealsense.git

$ cd installLibrealsense/UVCKernelPatches

$ ./getKernelSources.sh

do exactly what is explained in the video in the editor that opens at the end of the process

$ ./applyUVCPatch.sh

go to scripts and replace in makeKernel.sh every “make” by “make -i “

./buildKernel.sh

$ ./copyzImage.sh

$ cd ..

$ ./setupTK1.sh

reboot

modify files according to above link :

installLibrealsense.sh -> replace “git checkout version” with “git checkout _hugenumber”

arm.patch -> add everything at the end of already existing file

open installLibrealsense.sh in gedit and run commands one by one. Do the same with install_glfw3.sh (called in previous .sh)

It should be good to go. 

Try

$ cd ~/librealsense/bin

$ ./cpp-config-ui



I hope this works for you but I can't guarantee.
