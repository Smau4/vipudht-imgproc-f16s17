# PREPARATION
# The Makefile was created on Windows so it needs to 
# be reformatted for Unix
apt-get install dos2unix
dos2unix Makefile

# INSTALL (sudo, make, pkg-config) 
apt-get install sudo
apt-get install build-essential
apt-get install pkg-config

# Install libgphoto2
sudo apt-get install libltdl-dev libusb-dev libusb-1.0 libexif-dev libpopt-dev
wget http://downloads.sourceforge.net/project/gphoto/libgphoto/2.5.12/libgphoto2-2.5.12.tar.gz
tar -xvzf libgphoto2-2.5.12.tar.gz
cd libgphoto2-2.5.12
./configure
make
sudo make install

# Install OpenCV
sudo apt-get install libopencv-dev python-opencv