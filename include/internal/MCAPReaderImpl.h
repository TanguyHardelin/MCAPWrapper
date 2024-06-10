#ifndef MCAP_READER_IMPLEMENTATION_HPP
#define MCAP_READER_IMPLEMENTATION_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "internal/Base64.hpp"
#include "internal/json.hpp"
#include "mcap/reader.hpp"
#include "define.h"


namespace mcap_wrapper
{
    class MCAPReaderImpl
    {
    public:
        /**
         * @brief Construct a new mcap reader object
         *
         * @param file_path path to mcap file
         */
        MCAPReaderImpl(std::string file_path);
        ~MCAPReaderImpl();
        /**
         * @brief Get all channels presents in MCAP with it type
         * 
         * @return std::map<std::string, MCAPReaderChannelType> Dictionnary of <channel_name, channel_type>
         */
        std::map<std::string, MCAPReaderChannelType> get_channels();
        /**
         * @brief Get the next message (raw) message on this channel present into MCAP file. The message is outputted as std::string containing serialized message
         * 
         * @param channel_name channel to look
         * @param out_message output message
         * @return true Everything goes well.
         * @return false Everythin goes bad.
         */
        bool get_next_message(std::string channel_name, std::string &out_message);
        /**
         * @brief Get the next image present on this channel into MCAP file
         * 
         * @param channel_name channel to look
         * @param out_image output image
         * @return true Everything goes well.
         * @return false Everything goes bad.
         */
        bool get_next_image(std::string channel_name, cv::Mat &out_image);
        /**
         * @brief Get the next log message present on this channel into MCAP file
         * 
         * @param channel_name channel to look
         * @param out_log output log
         * @return true Everything goes well.
         * @return false Everything goes bad
         */
        bool get_next_logs(std::string channel_name, std::string & out_log);

    protected:
        std::map<std::string, MCAPReaderChannelType> _channels_description;
        mcap::McapReader _file_reader;
        bool _is_file_open = false;
        std::map<std::string, std::shared_ptr<mcap::LinearMessageView>> _channel_message_view;
        std::map<std::string, std::shared_ptr<mcap::LinearMessageView::Iterator>> _channel_iterator;
    };

}; // namespace mcap_wrapper

#endif