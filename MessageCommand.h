#pragma once
#include "BaseChatCommand.h"

namespace chatManagement
{
    class MessageCommand : public BaseChatCommand
    {
    public:
        explicit MessageCommand(std::string userId, std::string message);

		explicit MessageCommand();

		friend bool operator==(const MessageCommand& a, const MessageCommand& b)
		{
			return ((a.m_userId == b.m_userId) && (a.m_message == b.m_message));
		}

		friend std::istream& operator>>(std::istream& inputStream, chatManagement::MessageCommand& msgCmd)
		{
			std::string inputStr;
			getline(inputStream, inputStr);

			auto posUserSectionBegin = inputStr.find(Config::RawDataUserIdSectionBegin);
			auto posUserSectionEnd = inputStr.find(Config::RawDataUserIdSectionEnd);

			auto posMsgSectionBegin = inputStr.find(Config::RawDataMsgSectionBegin);
			auto posMsgSectionEnd = inputStr.find(Config::RawDataMsgSectionEnd);

			if ((posUserSectionBegin == std::string::npos) ||
				(posUserSectionEnd == std::string::npos) ||
				(posMsgSectionBegin == std::string::npos) ||
				(posMsgSectionEnd == std::string::npos))
			{
				throw std::runtime_error("IntroCmdParsing: Problem with the <TAGS>!");
			}

			auto offsetUserIdStart = posUserSectionBegin + strlen(Config::RawDataUserIdSectionBegin);
			std::string userId(inputStr, offsetUserIdStart, posUserSectionEnd - offsetUserIdStart);

			auto offsetMsgStart = posMsgSectionBegin + strlen(Config::RawDataMsgSectionBegin);
			std::string msgStr(inputStr, offsetMsgStart, posMsgSectionEnd - offsetMsgStart);

			MessageCommand parsedMsgCmd(userId, msgStr);
			msgCmd = parsedMsgCmd;

			return inputStream;
		}

		friend std::ostream& operator<<(std::ostream& ouptutStr, chatManagement::MessageCommand& msgCmd)
		{
			std::string cmd = Config::RawDataUserIdSectionBegin + msgCmd.m_userId + Config::RawDataUserIdSectionEnd
							+ Config::RawDataMsgSectionBegin + msgCmd.m_message + Config::RawDataMsgSectionEnd;

			ouptutStr << cmd;

			return ouptutStr;
		}

		virtual std::map<sysTime, userAndMsgPair> getChatData() const override;	

    private:
        std::string m_userId;
        std::string m_message;
    };

} // chatManagement
