#ifndef AQUABOX_EASY_SLAVE_PROPERTY_T
#define AQUABOX_EASY_SLAVE_PROPERTY_T

#include "value_data.h"

namespace aquabox
{
    namespace proto
    {
        struct SlaveProperty
        {
            const char* name;
            virtual ValueData type() const = 0;
            bool set(const ValueData& in) virtual = 0;
            bool get(ValueData& out) const virtual = 0;
        };

        template<typename BasicT>
        struct BasicType
        {
            static const ValueType type;
        };

        template<>
        const ValueType BasicType<bool>::type = ValueType::Boolean;

        template<>
        const ValueType BasicType<int32_t>::type = ValueType::Integer;

        template<>
        const ValueType BasicType<char*>::type = ValueType::String;

        template<>
        const ValueType BasicType<float>::type = ValueType::Float;

        template<typename CachedT>
        struct CachedSlaveProperty : public SlaveProperty
        {
            CachedSlaveProperty(CachedT v = CachedT())
                : value(v)
            {}
            CachedT value;

            virtual ValueData type() const { return BasicType<CachedT>::type; }

            virtual bool set(const ValueData& in)
            {
                if (in.type != type())
                    return false;
            }
            virtual bool get(ValueData& out) const
            {
                out.type = type();

                return true;
            }
        };

        template<>
        bool CachedSlaveProperty<bool>::set(const ValueData& in)
        {
            if (in.type != type())
                return false;

            value = in.value.B != 0;
        }

        template<>
        bool CachedSlaveProperty<bool>::get(ValueData& out) const
        {
            out.value.B = value ? 1 : 0;
        }

        template<>
        bool CachedSlaveProperty<int32_t>::set(const ValueData& in)
        {
            if (in.type != type())
                return false;

            value = in.value.I;
        }

        template<>
        bool CachedSlaveProperty<int32_t>::get(ValueData& out) const
        {
            out.value.I = value;
        }

        template<>
        bool CachedSlaveProperty<char*>::set(const ValueData& in)
        {
            if (in.type != type())
                return false;

            strcpy(value, in.value.S);
            return 0;
        }

        template<>
        bool CachedSlaveProperty<char*>::get(ValueData& out) const
        {
            strcpy(out.value.S, value);
        }

        template<>
        bool CachedSlaveProperty<float>::set(const ValueData& in)
        {
            if (in.type != type())
                return false;

            value = (float) in.value.F.V;
            value /= (float) in.value.F.Precision;

            return 0;
        }

        template<>
        bool CachedSlaveProperty<char*>::get(ValueData& out) const
        {
            in.value.F.Precision = 100;
            in.value.F.V = (int32_t)(value * 100);
        }
    } // namespace proto
} // namespace aquabox

#endif // AQUABOX_EASY_SLAVE_PROPERTY_T