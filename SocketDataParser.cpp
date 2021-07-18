#include "pch.h"
#include "SocketDataParser.h"
#include "Configuration.h"

namespace chatManagement
{

	std::optional<std::pair<std::string, std::string>> SocketDataParser::parseSocketData(const char* data, int amountOfData)
	{
		std::string rawData(data, amountOfData);

		auto posUserSectionBegin = rawData.find(Config::RawDataUserIdSectionBegin);
		auto posUserSectionEnd = rawData.find(Config::RawDataUserIdSectionEnd);

		auto posMsgSectionBegin = rawData.find(Config::RawDataMsgSectionBegin);
		auto posMsgSectionEnd = rawData.find(Config::RawDataMsgSectionEnd);

		if ( (posUserSectionBegin == std::string::npos) || 
			 (posUserSectionEnd == std::string::npos)   ||
			 (posMsgSectionBegin == std::string::npos)  ||
			 (posMsgSectionEnd == std::string::npos))
		{
			// not a valid data stream
			// TODO add CRC checksume verification

			return std::nullopt;
		}

		auto offsetUserIdStart = posUserSectionBegin + strlen(Config::RawDataUserIdSectionBegin);

		std::string userId(data, offsetUserIdStart, posUserSectionEnd - offsetUserIdStart);


		auto offsetMsgStart = posMsgSectionBegin + strlen(Config::RawDataMsgSectionBegin);

		std::string msg(data, offsetMsgStart, posMsgSectionEnd - offsetMsgStart);

		
		auto parsedData = std::pair<std::string, std::string>{ userId, msg };

		return std::optional<std::pair<std::string, std::string>> {parsedData};
	}
	
}