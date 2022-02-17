#include "message_io.h"

using namespace aquabox::proto;

MessageIO::MessageIO() {}

bool MessageIO::makeRequest(const Message& req, Message& rsp)
{
    return send(req) && get(rsp);
}