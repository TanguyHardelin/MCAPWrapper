#include "MCAPReader.h"
#include "internal/MCAPReaderImpl.h"

namespace mcap_wrapper
{
    MCAPReader::MCAPReader(std::string file_path)
    {
        _impl = std::make_shared<MCAPReaderImpl>(file_path);
    }

    MCAPReader::~MCAPReader()
    {
        
    }

    std::map<std::string, MCAPReaderChannelType> MCAPReader::get_channels()
    {
        return _impl->get_channels();
    }

    bool MCAPReader::get_next_message(std::string channel_name, std::string &out_message)
    {
        return _impl->get_next_message(channel_name, out_message);
    }

    bool MCAPReader::get_next_image(std::string channel_name, cv::Mat &out_image)
    {
        return _impl->get_next_image(channel_name, out_image);
    }

    bool MCAPReader::get_next_logs(std::string channel_name, std::string & out_log)
    {
        return _impl->get_next_logs(channel_name, out_log);
    }
};