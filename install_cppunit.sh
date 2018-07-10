git clone git://anongit.freedesktop.org/git/libreoffice/cppunit/
cd cppunit/
sudo apt-get install libtool m4 automake
./autogen.sh
./configure
make
make check
sudo make install

