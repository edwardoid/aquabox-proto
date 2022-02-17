#ifndef AB_PROTO_MESSAGE_IO_H
#define AB_PROTO_MESSAGE_IO_H

#include <io.h>
#include <message.h>

namespace aquabox
{
    namespace proto
    {
        class MessageIO : public IO
        {
        public:
            MessageIO();
            virtual bool makeRequest(const Message& req, Message& rsp);
        };
    } // namespace proto
} // namespace aquabox

#endif // AB_PROTO_MESSAGE_IO_H