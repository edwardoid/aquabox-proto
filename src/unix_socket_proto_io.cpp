/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "unix_socket_proto_io.h"

#include <arpa/inet.h>
#include <chrono>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace aquabox;
using namespace aquabox::proto;

UNIXSocketIO::UNIXSocketIO(int sc, int timeoutSeconds)
{
    m_sfd = sc;

    // int flags = fcntl(m_sfd, F_GETFL, 0);
    // fcntl(m_sfd, F_SETFL, flags & ~O_NONBLOCK);

    struct timeval timeout;
    timeout.tv_sec = timeoutSeconds;
    timeout.tv_usec = 500;

    m_good = setsockopt(m_sfd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout)) >= 0
             && setsockopt(m_sfd, SOL_SOCKET, SO_SNDTIMEO, (char*) &timeout, sizeof(timeout)) >= 0;
    m_good = true;
}

void UNIXSocketIO::close()
{
    ::close(m_sfd);
}

bool UNIXSocketIO::good() const
{
    return m_good;
}

buffer_length_t UNIXSocketIO::wait(buffer_length_t length) const
{
    if (!m_good) {
        return 0;
    }
    if (m_buffer.size() >= length) {
        return length;
    }
    buffer_length_t remain = length - m_buffer.size();
    buffer_length_t avail = available();
    if (avail > remain) {
        remain = avail;
    }
    m_buffer.resize(m_buffer.size() + remain);

    int count = ::read(m_sfd, m_buffer.data() + (m_buffer.size() - remain), remain);

    if (count < 0) {
        m_good = false;
    }

    return m_buffer.size();
}

buffer_length_t UNIXSocketIO::available() const
{
    int count;
    if (ioctl(m_sfd, FIONREAD, &count) < 0) {
        // m_good = false;
        return 0;
    }

    return static_cast<buffer_length_t>(count);
}

buffer_length_t UNIXSocketIO::write(const byte_t* buffer, buffer_length_t sz)
{
    int count = ::write(m_sfd, buffer, (size_t) sz);
    return count > 0 ? count : 0;
}

buffer_length_t UNIXSocketIO::read(buffer_length_t sz, byte_t* buffer)
{
    if (m_buffer.size() < sz) {
        buffer_length_t remain = sz - m_buffer.size();
        wait(remain);
    }

    buffer_length_t len = sz > m_buffer.size() ? m_buffer.size() : sz;
    if (len > 0) {
        memcpy(buffer, m_buffer.data(), len);
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + len);
    }
    return len;
}

bool UNIXSocketIO::makeRequest(const proto::Message& req, proto::Message& rsp)
{
    if (m_good) {
        std::lock_guard<std::mutex> lock(m_ioMx);
        if (m_good) {
            m_good = send(req);
            if (!m_good) {
            }
            else {
                m_good = get(rsp);
            }
        }
    }

    return m_good;
}
