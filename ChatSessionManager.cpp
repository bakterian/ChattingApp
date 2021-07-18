#include "pch.h"
#include "ChatSessionManager.h"
#include "SocketUtil.h"

namespace chatManagement
{
	ChatSessionManager::ChatSessionManager() : 
		m_ClientsCollection{}
	{
	}

	bool ChatSessionManager::clientConnected(const SOCKADDR_IN& socketAddr)
	{
		auto foundIndex = std::find(m_ClientsCollection.begin(), m_ClientsCollection.end(), socketAddr);

		bool isNewClient = (foundIndex == m_ClientsCollection.end());

		if(isNewClient) m_ClientsCollection.push_back(socketAddr);

		return isNewClient;
	}

	std::optional<std::vector<SOCKADDR_IN>> ChatSessionManager::getClientsForMsgForward(const SOCKADDR_IN& receivedFromAddr)
	{
		std::optional<std::vector<SOCKADDR_IN>> res = std::nullopt;

		if (m_ClientsCollection.size() > 1)
		{
			auto resultingColection = m_ClientsCollection;

			auto clientToExclude = std::find(resultingColection.begin(), resultingColection.end(), receivedFromAddr);

			// the client from which data is evaluated should be listed
			if(clientToExclude != resultingColection.end());
			{
				resultingColection.erase(clientToExclude);

				res = resultingColection;
			}
		}
		
		return res;
	}

	bool ChatSessionManager::areThereMutlipleClients() const
	{
		return (m_ClientsCollection.size() > 1);
	}

}
