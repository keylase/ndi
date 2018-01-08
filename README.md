# ndi for ffmpeg 
- fixed version for compiling with ffmpeg

- add this repo in /opt/projects/:

git clone https://github.com/keylase/ndi

./configure --enable-libndi_newtek --extra-cflags=-I/opt/projects/ndi/linux/include --extra-ldflags=-L/opt/projects/ndi/linux/include --enable-ffplay

for use with multiple interfaces:
sudo apt-get install avahi-daemon avahi-discover avahi-utils libnss-mdns mdns-scan

