# gr-ism
GNURadio block for decoding iSmartAlarm radio device packets.

Build with:

> mkdir build && cd build

> cmake -DCMAKE_INSTALL_PREFIX=/usr ../

> make

> sudo make install

Search for "iSmartAlarm" within gnuradio-companion block list.

Block Requirements:

> target message queue variable: myqueue

Block Options:

> CRC16: Only print packets that pass CRC16 check.

> Verbose: Print raw packets from C file stdout to console.
