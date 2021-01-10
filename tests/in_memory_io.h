#ifndef AB_PROTO_IN_MEMORY_H
#define AB_PROTO_IN_MEMORY_H

#include <vector>

namespace aquabox { namespace proto
{

    class InMemory: public IO
    {
    public:
        InMemory(std::vector<byte_t>& in, std::vector<byte_t>& out)
            : m_in(in), m_out(out)
        {}
    
        VIRTUAL_FN buffer_length_t available() const
        {
            return m_in.size();
        }

        VIRTUAL_FN buffer_length_t read(buffer_length_t sz, byte_t* buffer)
        {
            if (sz > m_in.size()) {
                sz = m_in.size();
            }
            memcpy(buffer, m_in.data(), sz);
            m_in.erase(m_in.begin(), m_in.begin() + sz);
            return sz;
        }
        VIRTUAL_FN buffer_length_t write(const byte_t* buffer, buffer_length_t sz)
        {
            m_out.resize(m_out.size() + sz);
            memcpy(m_out.data() + m_out.size() - sz, buffer, sz);
            return sz;
        } 

    template<typename T>
    T min(const T a, const T b) { return b > a ? a : b; }



    private:
        std::vector<byte_t>& m_in;
        std::vector<byte_t>& m_out;
    };
} }


#endif // AB_PROTO_IN_MEMORY_H