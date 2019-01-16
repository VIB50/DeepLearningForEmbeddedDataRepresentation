Here is my code to find adequate surfaces for video projection.

It only uses an RGB camera.

It first detours the image, with the Canny() function. Then it finds the biggest cross that can fit between those contours. It eventually finds the biggest rectangle that can fit, where the cross was found.

The apparent simplicity of the procedure makes it very fast for the program to find the biggest surface for video projection. It has proven very accurate.

For more information about those choices, refer to my research report :
link
