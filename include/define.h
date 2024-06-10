#ifndef MCAP_WRAPPER_DEFINE_H
#define MCAP_WRAPPER_DEFINE_H

namespace mcap_wrapper
{
    enum class MCAPReaderChannelType
    {
        RAW_JSON = 0,
        IMAGE = 1,
        OBJECT_3D = 2,
        LOG = 3,
        TRANSFORM = 4
    };
};

#endif