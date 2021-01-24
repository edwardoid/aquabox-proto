#ifndef AB_PROTO_MASTER_H
#define AB_PROTO_MASTER_H

#ifndef SLAVE_ONLY
#include "message_io.h"
#include "message.h"
#include "message_builder.h"

namespace aquabox
{
    namespace proto
    {
        class Master
        {
        public:
            Master(MessageIO *io)
                : m_io(io)
            {
                memset(m_serial, 0, SERIAL_LEN);
                memset(m_token, 0, TOKEN_LEN);
            }

            bool handleHandshake();

            VIRTUAL_FN bool acceptConnectionFrom(const byte_t *serial) const;

            VIRTUAL_FN bool authentificationPassed(const byte_t *token) const;

            bool isTemporaryToken(const byte_t *token) const;

            VIRTUAL_FN bool isBlocked(const byte_t *token) const;

            inline bool isEmptyToken(const byte_t *token) const;

            VIRTUAL_FN void updateToken(const byte_t *inital, byte_t *updated);

            template <typename T>
            bool setProperty(const char *property, const T &val);

            template <typename T>
            bool getProperty(const char *property, T &val);

            int getPropertiesCount();

            bool getPropertyName(uint8_t index, const char *name);

            bool hasIncomingMessage() const;
        protected:
            bool handle(Message *rcv, Message *rsp);
            bool performHandshake(Message *rcv, Message *rsp);

        protected:
            MessageIO *m_io;
            byte_t m_serial[SERIAL_LEN];
            byte_t m_token[TOKEN_LEN];
        };
    } // namespace proto
} // namespace aquabox

#endif // SLAVE_ONLY

#endif // AB_PROTO_MASTER_H