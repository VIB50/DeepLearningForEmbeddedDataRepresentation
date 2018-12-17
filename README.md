# DeepLearningForEmbeddedDataRepresentation
This code is part of a bigger project : Deep Learning For embedded data representation.

In this repo, you will find :

one code, TrackAndFollow, meant to follow a user (or multiple, as best as it can) along one axis (can be easily extended to 2 axis or more). It uses a tracker, meant to be replaced if you want to go further and enable the code to recognize the user's activity (the tracking part will probably be included in the recognition part), and a motor command.

another code, FindFlatSurface, meant to find the biggest and best suited for projection surface available in the camera view. It is optimized to be fast.

Hardware :

- Jetson TK1 board
- camera Intel RealSense 200
- motor EPOS 2
