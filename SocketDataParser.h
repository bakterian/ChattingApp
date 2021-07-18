#pragma once
#include "ILiveDataListener.h"
#include <string>
#include <optional>
#include "NonCopyableMovable.h"

namespace chatManagement
{
	class SocketDataParser : private utils::NonCopyableMovable
	{
	public:
		// if a valid chat message was identified a [userName, Message] pair is returned.
		std::optional<std::pair<std::string, std::string>> parseSocketData(const char* data, int amountOfData);
	};
}