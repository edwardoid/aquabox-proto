#ifndef AB_PROTO_MESH_IO_H
#define AB_PROTO_MESH_IO_H

#include "RF24.h"
#include "RF24Mesh.h"
#include "io.h"
#include "message.h"

#define BUFFER_LEN 3

namespace aquabox {
namespace proto {
    class MeshIO : public IO {
    public:
        MeshIO(RF24Mesh& mesh, RF24Network& network)
            : m_mesh(mesh)
            , m_network(network)
        {
        }
        VIRTUAL_FN bool good() const
        {
            return m_mesh.checkConnection();
        }
        VIRTUAL_FN buffer_length_t available() const
        {
            return m_count * sizeof(Message);
        }

        VIRTUAL_FN buffer_length_t wait(buffer_length_t length) const
        {
            if (available() < length) {
                while (!poll())
                    ;
            }
            return available();
        }
        VIRTUAL_FN buffer_length_t read(buffer_length_t sz, byte_t* buffer)
        {
            if (m_count == 0)
                return 0;

            Message& msg = m_buffer[m_front];

            memcpy(buffer, &(m_buffer[m_front]), sizeof(Message));

            m_front = (m_front + 1) % BUFFER_LEN;
            m_count--;
            return sizeof(Message);
        }
        VIRTUAL_FN buffer_length_t write(const byte_t* buffer, buffer_length_t sz)
        {
            m_mesh.update();
            int i = 0;
            while (!m_mesh.write(buffer, 'M', sz)) {
                delay(2);
                ++i;
                if (i == 500) {
                    return 0;
                }
            }
            return sz;
        }

        bool poll() const
        {
            waitTillAvailable();

            return processAvailableMessages();
        }

        void waitTillAvailable() const
        {
            while (!m_network.available()) {
                m_mesh.update();
                delay(1);
            }
        }

        bool processAvailableMessages() const
        {
            bool hasAnyData = false;
            while (m_network.available()) {
                RF24NetworkHeader header;

                m_network.peek(header);

                if (header.type == 'M' && header.from_node == 0) {
                    if (m_count == BUFFER_LEN) {
                        m_front = (++m_front) % BUFFER_LEN;
                        --m_count;
                    }

                    int res = m_network.read(header, &(m_buffer[m_rear]), sizeof(Message));
                    hasAnyData = true;

                    Message msg = m_buffer[m_rear];

                    ++m_count;
                    m_rear = (m_rear + 1) % BUFFER_LEN;
                } else {
                    int p;
                    m_network.read(header, 0, 0);
                    ++p;
                }
            }
            return hasAnyData;
        }

    protected:
        RF24Mesh& m_mesh;
        RF24Network& m_network;
        mutable Message m_buffer[BUFFER_LEN];
        mutable int8_t m_front = 0;
        mutable int8_t m_rear = 0;
        mutable int8_t m_count = 0;
    };
} // namespace proto
} // namespace aquabox

#endif // AB_PROTO_MESH_IO_H
