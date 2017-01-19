## imgproc.r1
Successfully tested integration of libgphoto2 and OpenCV.

## Setup
* Connect RasberryPi to the internet
* Install libgphoto2 and opencv by running: ``` sudo .\setup.sh ```
* Change into this directory and run: ``` make ```
* Execute the newly compiled program: ``` ./imgproc ```

## Troubleshooting
* 	Error (-53: "Could not claim the USB device")
	Why: Another process is using the USB device. If gphoto2 is installed, a gvfs-gphoto2 related process is most likely the cause of it.
	Solution: Find and kill the process then disable it. If it is gphoto2 related, uninstall gphoto2 using ``` sudo apt-get remove gphoto2 ```