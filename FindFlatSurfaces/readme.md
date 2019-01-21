Here is my code to find adequate surfaces for video projection.

It only uses an RGB camera. The one I had was an Intel RealSense R200. It was very tricky and long to install the libraries. You may just wish to switch to another camera or even just a file. For this, just give a mat image to the code and erase the parts linked with the camera (RS_something and so on).

You will find installation instructions for the camera in the OfficialTestCodes folder.

Once everything installed, just run the Makefile. It will show you multiple windows including the RGB and depth stream, the detouring and the biggest rectangle found.

The code first detours the image, with the Canny() function from openCV. Then it finds the biggest cross that can fit between those contours. It eventually finds the biggest rectangle that can fit, where the cross was found.

The apparent simplicity of the procedure makes it very fast for the program to find the biggest surface for video projection. It has proven very accurate.

For more information about those choices, refer to my research report :
link
