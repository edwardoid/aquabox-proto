#include "master.h"

#ifndef SLAVE_ONLY

#include <string.h>
#include <memory.h>

using namespace aquabox::proto;

bool Master::hasIncomingMessage() const
{
    return m_io->available() >= sizeof(Message);
}

bool Master::handleHandshake()
{
    if (m_io->available() >= sizeof(Message))
    {
        Message rcv;
        m_io->get(rcv);
        Message rpl;
        if (handle(&rcv, &rpl))
        {
            return m_io->send(rpl);
        }
        return true;
    }

    return false;
}

bool Master::acceptConnectionFrom(const byte_t *serial) const
{
    static const char *TST_RELAY = "tstrelay";
    return memcmp(serial, TST_RELAY, SERIAL_LEN) == 0;
}

bool Master::authentificationPassed(const byte_t *token) const
{
    static const char *ALLOWED = "allow me";
    static const char *DENIED = "deny  me";
    return memcmp(token, ALLOWED, TOKEN_LEN) == 0;
}

bool Master::isTemporaryToken(const byte_t *token) const
{
    return token[0] == 'T';
}

bool Master::isBlocked(const byte_t *token) const
{
    static const char *DENIED = "deny  me";
    return memcmp(token, DENIED, TOKEN_LEN) == 0;
}

bool Master::isEmptyToken(const byte_t *token) const
{
    for (int i = 0; i < TOKEN_LEN; ++i)
    {
        if (token[i] != '0')
            return false;
    }

    return true;
}

void Master::updateToken(const byte_t *inital, byte_t *updated)
{
    if (isEmptyToken(inital))
    {
        const char *ALLOWED = "allow me";
        memcpy(updated, ALLOWED, TOKEN_LEN);
        updated[0] = 'T';
    }
    else
    {
        memcpy(updated, inital, TOKEN_LEN);
    }
}

int Master::getPropertiesCount()
{
    Message msg;
    MessageBuilder::setSerial(msg, m_serial);
    MessageBuilder::setToken(msg, m_token);
    msg.type = MessageType::Command;
    msg.payload.command.command = Command::GetPropertiesCount;

    Message rsp;

    if(!m_io->makeRequest(msg, rsp)) {
        return -1;
    }

    if (rsp.type != MessageType::Command || rsp.payload.command.command != Command::GetPropertiesCount)
    {
        return -1;
    }

    return (int)rsp.payload.command.data.count;
}

bool Master::getPropertyName(uint8_t index, const char *name)
{
    Message msg;
    memcpy(msg.serial, m_serial, SERIAL_LEN);
    msg.type = MessageType::Command;
    msg.payload.command.command = Command::GetPropertyName;
    msg.payload.command.data.index = index;


    Message rsp;
    if(!m_io->makeRequest(msg, rsp)) {
        return false;
    }
    
if (rsp.type != MessageType::Command || rsp.payload.command.command != Command::GetPropertyName)
    {
        return false;
    }

    strcpy(const_cast<char *>(name), rsp.payload.command.data.string);
    return true;
}

bool Master::handle(Message *rcv, Message *rsp)
{
    switch (rcv->type)
    {
    case MessageType::Handshake:
    {
        return performHandshake(rcv, rsp);
    }
    default:
    {
        MessageBuilder::setError(*rsp, ErrorType::Unsupported, "Command is not supported by server");
        return true;
    }
    }

    return false;
}

bool Master::performHandshake(Message *rcv, Message *rsp)
{
    rsp->type = MessageType::Handshake;

    rsp->payload.handshake.result = HandshakeResult::Ok;

    if (isEmptyToken(rcv->token))
    {
        updateToken(rcv->token,
                    rsp->token);
    }
    else if (isBlocked(rcv->token))
    {
        MessageBuilder::setError(*rsp, ErrorType::AccessDenied, "Authentification failed");
    }
    else if (acceptConnectionFrom(rcv->serial))
    {
        updateToken(rcv->token,
                    rsp->token);
        mempcpy(m_serial, rcv->serial, SERIAL_LEN);
        mempcpy(m_token, rcv->token, TOKEN_LEN);
    }
    else
    {
        rsp->payload.handshake.result = HandshakeResult::Fail;
    }

    return true;
}

template <>
bool Master::getProperty<GetValueData>(const char *property, GetValueData &value)
{
    Message msg;
    MessageBuilder::setSerial(msg, m_serial);
    MessageBuilder::setToken(msg, m_token);
    msg.type = MessageType::Command;
    msg.payload.command.command = Command::Get;
    strcpy(msg.payload.command.data.get.name, property);
    msg.payload.command.data.get.type = value.type;


    Message rpl;
    if(!m_io->makeRequest(msg, rpl)) {
        return false;
    }

    if (rpl.type == MessageType::Command && rpl.payload.command.command == Command::Get)
    {
        if (rpl.payload.command.data.get.type != value.type)
        {
            return false;
        }

        if (strncmp(property, rpl.payload.command.data.get.name, strlen(property) != 0))
        {
            return false;
        }

        if (rpl.payload.command.data.set.type != value.type)
        {
            return false;
        }

        memcpy(&value, &rpl.payload.command.data.get, sizeof(GetValueData));

        return true;
    }
    return false;
}

template <>
bool Master::getProperty<bool>(const char *property, bool &value)
{
    GetValueData vd;
    vd.type = ValueType::Boolean;
    bool ok = getProperty(property, vd);
    value = vd.value.B != 0;
    return ok;
}

template <>
bool Master::getProperty<int32_t>(const char *property, int32_t &value)
{
    GetValueData vd;
    vd.type = ValueType::Integer;
    bool ok = getProperty(property, vd);
    value = vd.value.I;
    return ok;
}

template <>
bool Master::getProperty<uint32_t>(const char *property, uint32_t &value)
{
    GetValueData vd;
    vd.type = ValueType::UnsignedInteger;
    bool ok = getProperty(property, vd);
    value = vd.value.U;
    return ok;
}

template <>
bool Master::getProperty<const char *>(const char *property, const char *&value)
{
    GetValueData vd;
    vd.type = ValueType::String;
    bool ok = getProperty(property, vd);
    if (ok)
    {
        strcpy(const_cast<char *>(value), vd.value.S);
    }
    return ok;
}

template <>
bool Master::getProperty<float>(const char *property, float &value)
{
    GetValueData vd;
    vd.type = ValueType::Float;
    bool ok = getProperty(property, vd);
    value = (double)(vd.value.F.V) / (double)(vd.value.F.Precision);
    return ok;
}

template <>
bool Master::setProperty<SetValueData>(const char *property, const SetValueData &value)
{
    Message msg;
    MessageBuilder::setSerial(msg, m_serial);
    MessageBuilder::setToken(msg, m_token);
    msg.type = MessageType::Command;
    msg.payload.command.command = Command::Set;


    strcpy(msg.payload.command.data.set.name,
           property);
    memcpy(&msg.payload.command.data.set, &value, sizeof(SetValueData));

    Message rpl;
    if(!m_io->makeRequest(msg, rpl)) {
        return false;
    }

    if (rpl.type == MessageType::Command && rpl.payload.command.command == Command::Set)
    {
        if (rpl.payload.command.data.set.type != value.type)
        {
            return false;
        }

        if (strncmp(property, rpl.payload.command.data.set.name, strlen(property) != 0))
        {
            return false;
        }

        return true;
    }

    return false;
}

template <>
bool Master::setProperty<bool>(const char *property, const bool &value)
{
    SetValueData vd;
    strcpy(vd.name, property);
    vd.type = ValueType::Boolean;
    vd.value.B = value ? 1 : 0;
    return setProperty<SetValueData>(property, vd);
}

template <>
bool Master::setProperty<int32_t>(const char *property, const int32_t &value)
{
    SetValueData vd;
    strcpy(vd.name, property);
    vd.type = ValueType::Integer;
    vd.value.I = value;
    return setProperty<SetValueData>(property, vd);
}

template <>
bool Master::setProperty<uint32_t>(const char *property, const uint32_t &value)
{
    SetValueData vd;
    strcpy(vd.name, property);
    vd.type = ValueType::UnsignedInteger;
    vd.value.U = value;
    return setProperty<SetValueData>(property, vd);
}

template <>
bool Master::setProperty<const char *>(const char *property, const char *const &value)
{
    SetValueData vd;
    strcpy(vd.name, property);
    vd.type = ValueType::String;
    strcpy(vd.value.S, value);
    return setProperty<SetValueData>(property, vd);
}

template <>
bool Master::setProperty<float>(const char *property, const float &value)
{
    SetValueData vd;
    strcpy(vd.name, property);
    vd.type = ValueType::Float;
    vd.value.F.Precision = 1000;
    vd.value.F.V = value * vd.value.F.Precision;
    return setProperty<SetValueData>(property, vd);
}

#endif // SLAVE_ONLY