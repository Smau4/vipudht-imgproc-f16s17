# 1 Install (sudo, make, pkg-config) 
apt-get install sudo
apt-get install build-essential
apt-get install pkg-config

# 2 Uninstall gphoto2
sudo apt-get remove gphoto2

# 3 Install libgphoto2-2.5.12
sudo apt-get install libltdl-dev libusb-dev libusb-1.0 libexif-dev libpopt-dev
wget http://downloads.sourceforge.net/project/gphoto/libgphoto/2.5.12/libgphoto2-2.5.12.tar.gz
tar -xvzf libgphoto2-2.5.12.tar.gz
cd libgphoto2-2.5.12
./configure
make
sudo make install

# 4 Install OpenCV
sudo apt-get install libopencv-dev python-opencv