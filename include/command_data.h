#ifndef AB_PROTO_COMMAND_DATA_H
#define AB_PROTO_COMMAND_DATA_H

#include "value_data.h"

namespace aquabox {
namespace proto {

    using GetValueData = ValueData;
    using SetValueData = ValueData;
    using CountData = uint8_t;
    using IndexData = uint8_t;
    using StringData = char[32];

    union CommandData {
        ValueData value;
        SetValueData set;
        GetValueData get;
        CountData count;
        IndexData index;
        StringData string;
    };

} // namespace proto
} // namespace aquabox

#endif // AB_PROTO_COMMAND_DATA_H