#!/bin/bash


SRC=$1
DST=$HOME/Arduino/libraries/Aquabox
PROTO_FILE=$DST/AquaboxProto.h

make_link() {
    ln -sf $SRC/$1 $DST/$1
}

mkdir -p $DST
echo '#ifndef AQUABOX_PROTO_H' > $PROTO_FILE
echo '#define AQUABOX_PROTO_H' >> $PROTO_FILE

make_link arduino_stream_io.h
make_link command_data.h
make_link command_payload.h
make_link easy_slave.h
make_link easy_slave_property.h
make_link easy_slave_variable.h
make_link error_payload.h
make_link handshake_payload.h
make_link io.h
make_link message_builder.h
make_link message.h
make_link message_io.cpp
make_link message_io.h
make_link slave.cpp
make_link slave.h
make_link types.h
make_link value_data.h


echo "#include <arduino_stream_io.h>" >> $PROTO_FILE
echo "#include <easy_slave.h>" >> $PROTO_FILE
echo '#endif // AQUABOX_PROTO_H' >> $PROTO_FILE
