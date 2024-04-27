#ifndef MCAP_WRAPPER_UTILS_H
#define MCAP_WRAPPER_UTILS_H

#include "json.hpp"

namespace mcap_wrapper{
    nlohmann::json infer_property_of_sample(nlohmann::json sample, bool recursive_call = false);
};

#endif