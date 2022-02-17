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

#pragma once

#include <message_io.h>
#include <mutex>
#include <vector>

namespace aquabox
{
    namespace proto
    {
        class UNIXSocketIO : public MessageIO
        {
        public:
            UNIXSocketIO(int sc, int timeout = 10);
            virtual proto::buffer_length_t wait(proto::buffer_length_t length) const override;
            virtual proto::buffer_length_t available() const override;
            virtual proto::buffer_length_t write(const proto::byte_t* buffer, proto::buffer_length_t sz) override;
            virtual proto::buffer_length_t read(proto::buffer_length_t sz, proto::byte_t* buffer) override;
            virtual bool good() const override;
            virtual void close() override;

            virtual bool makeRequest(const proto::Message& req, proto::Message& rsp) override;

        private:
            int m_sfd;
            std::mutex m_ioMx;
            mutable std::vector<proto::byte_t> m_buffer;
            mutable bool m_good;
        };
    } // namespace proto
} // namespace aquabox