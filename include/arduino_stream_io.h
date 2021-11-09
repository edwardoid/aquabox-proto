#ifndef AB_PROTO_ARDUINO_STREAM_H
#define AB_PROTO_ARDUINO_STREAM_H

#include "io.h"
#include <Arduino.h>

namespace aquabox {
namespace proto {
    class ArduinoStreamIO : public IO {
    public:
        ArduinoStreamIO() = default;
        void init(Stream* stream)
        {
            m_stream = stream;
        }

        VIRTUAL_FN bool good() const
        {
            return true;
        }

        void setPollTimeout(int timeout)
        {
            m_pollTimeout = timeout;
        }

        VIRTUAL_FN buffer_length_t available() const
        {
            return (buffer_length_t)m_stream->available();
        }

        VIRTUAL_FN buffer_length_t write(const byte_t* buffer, buffer_length_t sz)
        {
            // virtual size_t write(const uint8_t *buf, size_t size) override;
            return (buffer_length_t)m_stream->write(reinterpret_cast<const uint8_t*>(buffer), static_cast<size_t>(sz));
        }

        VIRTUAL_FN buffer_length_t read(buffer_length_t sz, byte_t* buffer)
        {
            return (buffer_length_t)m_stream->readBytes(reinterpret_cast<uint8_t*>(buffer), static_cast<size_t>(sz));
        }

        VIRTUAL_FN buffer_length_t wait(buffer_length_t length) const
        {
            while (length > available()) {
                delay(m_pollTimeout);
            }
            return available();
        }

    private:
        Stream* m_stream;
        int m_pollTimeout = 100;
    };
}
}

#endif // AB_PROTO_ARDUINO_STREAM_H
