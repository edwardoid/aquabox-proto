#include "slave.h"
#include "io.h"

#include <iostream>

using namespace aquabox::proto;

Slave::Slave(IO* io, const byte_t* serial, const byte_t* token)
    : m_io(io)
    , m_serial { 0, 0, 0, 0, 0, 0, 0 }
{
    memcpy(const_cast<byte_t*>(m_serial), serial, SERIAL_LEN);
    memcpy(m_token, token, TOKEN_LEN);
}

bool Slave::handshake()
{
    Message msg = MessageBuilder::handshake(m_serial, 1, 0, 0, &m_token[0]);
    if (!m_io->send(msg)) {
        return false;
    }

    Message response;
    if (!m_io->get(response)) {
        return false;
    }

    if (response.type == MessageType::Handshake) {
        memcpy(m_token, response.token, TOKEN_LEN);
        return true;
    }

    return false;
}

void Slave::loop()
{
    bool getResult = false;
    do {
        getResult = processNextMessage();
    } while (getResult);
}

bool Slave::processNextMessage()
{
    bool getResult = false;
    Message rcv;
    getResult = m_io->get(rcv);
    Message rpl;
    if (dispatch(&rcv, &rpl)) {
        m_io->send(rpl);
    }
    return getResult;
}

bool Slave::dispatch(Message* message, Message* response)
{
    switch (message->type) {
    case MessageType::Command: {
        return handleCommandRequest(message, response);
    }
    }

    return false;
}

bool Slave::handleCommandRequest(Message* msg, Message* response)
{
    bool handled = true;
    MessageBuilder::setSerial(*response, m_serial);
    MessageBuilder::setToken(*response, m_token);

    switch (msg->payload.command.command) {
    case Command::GetPropertiesCount: {
        response->type = MessageType::Command;
        response->payload.command.command = Command::GetPropertiesCount;
        response->payload.command.data.count = propertiesCount();
        break;
    }

    case Command::GetPropertyName: {
        response->type = MessageType::Command;
        response->payload.command.command = Command::GetPropertyName;

        const char* pname = propertyName(msg->payload.command.data.index);
        if (pname != NULL) {
            size_t len = strlen(pname);
            memcpy(response->payload.command.data.string, pname, len);
            response->payload.command.data.string[len] = '\0';
        } else {
            MessageBuilder::setError(*response, ErrorType::Unsupported,
                "Bad property index");
        }
        break;
    }

    case Command::Get: {
        response->type = MessageType::Command;
        response->payload.command.command = Command::Get;

        const int8_t idx = propertyIndex(msg->payload.command.data.get.name);
        bool ok = idx < propertiesCount() && idx >= 0;
        if (ok) {
            memcpy(response->payload.command.data.value.name,
                msg->payload.command.data.get.name,
                strlen(msg->payload.command.data.get.name));
            response->payload.command.data.value
                .name[strlen(msg->payload.command.data.get.name)]
                = '\0';
            response->payload.command.data.value.type = propertyType(idx);
            ok = get(idx, response->payload.command.data.get);
        } else if (!ok) {
            MessageBuilder::setError(*response, ErrorType::Unsupported,
                "Property does not exists");
        }
        break;
    }

    case Command::Set: {
        const int8_t idx = propertyIndex(msg->payload.command.data.get.name);
        bool ok = idx < propertiesCount() && idx >= 0;
        if (ok) {
            memcpy(response->payload.command.data.value.name,
                msg->payload.command.data.get.name,
                strlen(msg->payload.command.data.get.name));
            response->payload.command.data.value
                .name[strlen(msg->payload.command.data.get.name)]
                = '\0';
            response->payload.command.data.value.type = propertyType(idx);
            ok = set(idx, msg->payload.command.data.set) && get(idx, response->payload.command.data.set);
        } else if (!ok) {
            MessageBuilder::setError(*response, ErrorType::Unsupported,
                "Property does not exists");
        }
        break;
    }

    default: {
        return false;
    }
    }

    return true;
}
