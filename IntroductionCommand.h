#pragma once
#include "BaseChatCommand.h"

namespace chatManagement
{
    class IntroductionCommand : public BaseChatCommand
    {
    public:
        explicit IntroductionCommand(std::string userId);

		explicit IntroductionCommand();

		friend bool operator==(const IntroductionCommand& a, const IntroductionCommand& b)
		{
			return (a.m_userId == b.m_userId);
		}

		friend std::istream& operator>>(std::istream& inputStream, chatManagement::IntroductionCommand& introCmd)
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
			std::string introductionStr(inputStr, offsetMsgStart, posMsgSectionEnd - offsetMsgStart);

			if (introductionStr.compare(Config::IntroStr) != 0)
			{
				throw std::runtime_error("IntroCmdParsing: The IntroStr therefore this is not a valid IntroductionCommand!");
			}

			IntroductionCommand parsedIntroCmd(userId);
			introCmd = parsedIntroCmd;

			return inputStream;
		}

		friend std::ostream& operator<<(std::ostream& ouptutStr, chatManagement::IntroductionCommand& introCmd)
		{
			std::string cmd = Config::RawDataUserIdSectionBegin + introCmd.m_userId + Config::RawDataUserIdSectionEnd
				+ Config::RawDataMsgSectionBegin + Config::IntroStr + Config::RawDataMsgSectionEnd;

			ouptutStr << cmd;

			return ouptutStr;
		}

		virtual std::map<sysTime, userAndMsgPair> getChatData() const override;

    private:
        std::string m_userId;
    };
}
