#include "internal/utils.hpp"

namespace mcap_wrapper{

    nlohmann::json infer_property_of_sample(nlohmann::json sample, bool recursive_call){
        nlohmann::json out;
        for (auto &kv : sample.items())
        {
            // Handle string case
            if (kv.value().is_string())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "string";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle boolean case
            else if (kv.value().is_boolean())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "boolean";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle bytes case
            else if (kv.value().is_binary())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "bytes";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle float case
            else if (kv.value().is_number_float())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle uint32 case
            else if (kv.value().is_number_unsigned())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle int32 case
            else if (kv.value().is_number_integer())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle other number types:
            else if (kv.value().is_number())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle recursive schema:
            else if (kv.value().is_object())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "object";
                out[kv.key()]["comment"] = "Generated by wrapper";
                out[kv.key()]["properties"] = infer_property_of_sample(kv.value(), true);
            }
        }

        return out;
    }
};