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

#include "unix_serial_proto_io.h"

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

UNIXSerialIO::UNIXSerialIO(const char* dev, unsigned int baudrate)
{
    m_fd = open(dev, O_RDWR);

    // Create new termios struct, we call it 'tty' for convention
    // Read in existing settings, and handle any error
    if(tcgetattr(m_fd, &m_tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        m_good = false;
        return;
    }

    m_tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    m_tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    m_tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    m_tty.c_cflag |= CS8; // 8 bits per byte (most common)
    m_tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    m_tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    m_tty.c_lflag &= ~ICANON;
    m_tty.c_lflag &= ~ECHO; // Disable echo
    m_tty.c_lflag &= ~ECHOE; // Disable erasure
    m_tty.c_lflag &= ~ECHONL; // Disable new-line echo
    m_tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    m_tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    m_tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    m_tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    m_tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    m_tty.c_cc[VTIME] = 255;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    m_tty.c_cc[VMIN] = sizeof(Message);

    cfsetispeed(&m_tty, baudrate);
    cfsetospeed(&m_tty, baudrate);

    // Save tty settings, also checking for error
    if (tcsetattr(m_fd, TCSANOW, &m_tty) != 0) {
        m_good = false;
    }
    m_good = true;
}

void UNIXSerialIO::close()
{
    ::close(m_fd);
}

bool UNIXSerialIO::good() const
{
    return m_good;
}

buffer_length_t UNIXSerialIO::wait(buffer_length_t length) const
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

    while(remain)
    {
        int count = ::read(m_fd, m_buffer.data() + (m_buffer.size() - remain), remain);
        remain -= count;
    }
    

    return m_buffer.size();
}

buffer_length_t UNIXSerialIO::available() const
{
    int count;
    if (ioctl(m_fd, FIONREAD, &count) < 0) {
        return 0;
    }

    return static_cast<buffer_length_t>(count);
}

buffer_length_t UNIXSerialIO::write(const byte_t* buffer, buffer_length_t sz)
{
    int count = ::write(m_fd, buffer, (size_t) sz);
    return count > 0 ? count : 0;
}

buffer_length_t UNIXSerialIO::read(buffer_length_t sz, byte_t* buffer)
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

bool UNIXSerialIO::makeRequest(const proto::Message& req, proto::Message& rsp)
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
