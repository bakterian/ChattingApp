#pragma once
#include <string>

namespace chatManagement
{
	class IUserDetailUpdatesListener
	{
	public:
		virtual void processUserDetailChange(const std::string& newNick) = 0;
	};
}