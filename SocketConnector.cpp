#include "pch.h"
#include "SocketConnector.h"
#include "Configuration.h"
#include "SocketUtil.h"
#include <chrono>
#include <Ws2tcpip.h>
#include <future>
#include <optional>

using namespace std::chrono_literals;

namespace connectionLogic
{

	std::queue<std::pair<std::string, std::optional<std::vector<SOCKADDR_IN>>>> msgsToSendQueue;

	SocketConnector::SocketConnector() :
		m_socketServDescriptor(0),
		m_clientSocket(0),
		m_pReceiveThread(nullptr),
		m_pSenderThread(nullptr),
		m_ListenError(false),
		m_SendError(false),
		m_otherSocketAddress(),
		m_commonServerAddress(),
		m_pSocketDataListener(nullptr),
		m_wsaLastError(0),
		m_socketType(SocketConType::Undetermined),
		m_sendingMutex{},
		m_sendingCondtion{},
		m_clientAddrMutex{},
		m_clientAddrCollection{}
	{
	}

	SocketConnector::~SocketConnector()
	{
		if (isConnectionAlive()) stop();
	}

	bool SocketConnector::init()
	{
		// set common server address
		m_commonServerAddress.sin_family = AF_INET;
		m_commonServerAddress.sin_port = htons(Config::ConnectionPort);
		InetPton(AF_INET, _T("127.0.0.1"), &m_commonServerAddress.sin_addr.s_addr);

		// other socket address to be set when first client connects and does a handshake
		m_otherSocketAddress = m_commonServerAddress;

		m_clientAddrCollection.push_back(m_commonServerAddress);
		
		return initWinsock();
	}

	bool SocketConnector::start()
	{
		bool startSuccessfull = true;

		if (isConnectionAlive() == false)
		{
			auto handshakeSuccesfull = doHandshake();
			m_socketType = handshakeSuccesfull ? SocketConType::Client : SocketConType::Server;

			if (createSocket())
			{

				if (m_socketType == SocketConType::Server)
				{
					auto bindSucceded = attachSocketToPort();
					if (bindSucceded == false)
					{
						return false;
					}
					else
					{
						// serever does not needed to send messages to the himself
						m_clientAddrCollection.clear();
					}
				}

				// start receive thread
				
				m_pReceiveThread = new std::thread(&SocketConnector::continuesListen, this);

				std::this_thread::sleep_for(10ms); // wait 10ms for any immediate server errors

				if (m_ListenError)
				{
					closesocket(m_socketServDescriptor);
					m_pReceiveThread->join();
					cleanUp();
					startSuccessfull = false;
				}

				// start the sending thread			
				else
				{		
					m_pSenderThread = new std::thread(&SocketConnector::continuesSend, this);

					std::this_thread::sleep_for(10ms); // wait 10ms for any immediate server errors

					if (m_SendError)
					{
						// this means a send error so soccet connector start failed 
						closesocket(m_socketServDescriptor);
						m_pReceiveThread->join();
						cleanUp();
						startSuccessfull = false;
					}
				}	
			}
			else
			{
				//failed at socket creation or bind
				startSuccessfull = false;
			}
		}

		return startSuccessfull;
	}

	void SocketConnector::stop()
	{
		if (m_pReceiveThread)
		{
			closesocket(m_socketServDescriptor);

			m_pReceiveThread->join();

			cleanUp();
		}
	}

	void SocketConnector::sendOnSocket(std::string& text, const std::vector<SOCKADDR_IN>& recepients)
	{
		std::scoped_lock lock(m_sendingMutex);

		msgsToSendQueue.emplace(std::make_pair(text, recepients));

		m_sendingCondtion.notify_one();
	}

	void SocketConnector::sendOnSocketToAll(std::string& text)
	{
		std::scoped_lock lock(m_sendingMutex);

		msgsToSendQueue.emplace(std::make_pair(text, std::nullopt));

		m_sendingCondtion.notify_one();
	}

	SocketConType SocketConnector::getConnectionType() const
	{
		return m_socketType;
	}

	bool SocketConnector::isConnectionAlive() const
	{
		return (m_pReceiveThread != nullptr) && (m_ListenError == false);
	}

	void SocketConnector::subscribeForNotifications(ISocketDataListener* listener)
	{
		m_pSocketDataListener = listener;
	}

	bool SocketConnector::initWinsock()
	{
		WSADATA wsaData;

		auto res = WSAStartup(MAKEWORD(2, 2), &wsaData);

		return (res == 0);
	}

	bool SocketConnector::createSocket()
	{
		m_socketServDescriptor = 0;

		m_socketServDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		
		return (m_socketServDescriptor != 0);
	}

	//bool SocketConnector::setSocketHandshakeReceiveTimeout()
	//{
	//	auto res = setsockopt(m_socketServDescriptor, SOL_SOCKET, SO_RCVTIMEO, Config::HandsakeRecvTimeoutMs, sizeof(Config::HandsakeRecvTimeoutMs));
	//	return (res == 0);
	//}

	bool SocketConnector::attachSocketToPort()
	{
		SOCKADDR_IN socketIn;
		socketIn.sin_family = AF_INET;
		socketIn.sin_addr.s_addr = INADDR_ANY;
		socketIn.sin_port = htons(Config::ConnectionPort);
		//InetPton(AF_INET, _T("127.0.0.1"), &m_socketAddress.sin_addr.s_addr);

		auto res = bind(m_socketServDescriptor, (struct sockaddr*)&socketIn, sizeof(socketIn));

		return (res == 0);
	}

	void SocketConnector::cleanUp()
	{
		WSACleanup();

		if (m_pReceiveThread)
		{
			delete m_pReceiveThread;
			m_pReceiveThread = nullptr;
		}

		if (m_pSenderThread)
		{
			delete m_pSenderThread;
			m_pSenderThread = nullptr;
		}
		m_socketServDescriptor = 0;
		m_ListenError = false;
		m_SendError = false;
	}

	void SocketConnector::continuesListen()
	{
		char buffer[Config::IncomingDataBufSize]{'\0'};
		char ackBuffer[Config::HandshakeAckDataBufSize]{'\0'};
		char sendBuffer[Config::HandshakeDataBufSize]{ '\0' };

		SOCKADDR_IN fromSocket {};
		int fromSocketLen = sizeof(fromSocket);

		if (m_socketType == SocketConType::Client)
		{
			fromSocket = m_commonServerAddress;
		}

		while (true)
		{		
			// receive data

			if (m_socketType == SocketConType::Client)
			{	// send something...
				auto resp = sendto(m_socketServDescriptor, sendBuffer, strlen(sendBuffer), 0, (struct sockaddr*)&fromSocket, fromSocketLen);
				if (resp == SOCKET_ERROR)
				{
					m_wsaLastError = WSAGetLastError();
					m_ListenError = true;
					break;
				}
			}

			auto dataReceivedAmount = recvfrom(m_socketServDescriptor, buffer, Config::IncomingDataBufSize, 0, (struct sockaddr*)&fromSocket, &fromSocketLen);
			if (dataReceivedAmount == SOCKET_ERROR)
			{
				m_wsaLastError = WSAGetLastError();
				m_ListenError = true;
				break;
			}


			//store the adress to client in case this is a server
			if (m_socketType == SocketConType::Server)
			{
				std::lock_guard lock(m_clientAddrMutex);
	
				auto foundIter = std::find(m_clientAddrCollection.begin(), m_clientAddrCollection.end(), fromSocket);
				if (foundIter == m_clientAddrCollection.end())
				{
					// client port unkown add adress to collection
					m_clientAddrCollection.push_back(fromSocket);
				}
				
				// TODO m_otherSocketAddress to be removed after tests
				m_otherSocketAddress = fromSocket;
			}

			// handshake to be processed only by the server
			if ((m_socketType == SocketConType::Server) && isHandshake(buffer))
			{
				// the handshake address is not to be stored as the connection will be anyway closed by client.
				m_clientAddrCollection.pop_back();
			
				// send hanshake acknowledhment
				memcpy(ackBuffer, Config::InitHandshakeAck, strlen(Config::InitHandshakeAck) / sizeof(char));

				// only respond to a handshake
				auto resp = sendto(m_socketServDescriptor, ackBuffer, strlen(ackBuffer), 0, (struct sockaddr*)&fromSocket, fromSocketLen);
				if (resp == SOCKET_ERROR)
				{
					m_wsaLastError = WSAGetLastError();
					m_ListenError = true;
					break;
				}
			}
			else
			{
				if (m_socketType == SocketConType::Server)
				{
					// TODO notify upper layer so that:
					// 1) it will send the chat history to client that has joined
					// 2) it can display a joined info or at least increment connected clients counter
					m_pSocketDataListener->OnClientConnected(fromSocket);
				}
				// is not a handshake process a chat message
				m_pSocketDataListener->OnMessageReceived(fromSocket, buffer, dataReceivedAmount);
			}

			// clear data buffers
			memset(buffer, '\0', Config::IncomingDataBufSize);
			memset(ackBuffer, '\0', Config::HandshakeAckDataBufSize);
		}
	}


	void SocketConnector::continuesSend()
	{
		char buffer[Config::OutgoingDataBufSize]{ '\0' };


		while (true)
		{
			// 1. safely acquire the data for sending
			std::unique_lock lock(m_sendingMutex);

			m_sendingCondtion.wait(lock, []() {return !msgsToSendQueue.empty(); });
			auto [msgToSend, specifiedRecepients] = msgsToSendQueue.front();
			msgsToSendQueue.pop();

			lock.unlock();

			// 2. get the recepients collection in case not specified
			std::vector<SOCKADDR_IN> recepients{};

			if (specifiedRecepients)
			{
				recepients = specifiedRecepients.value();
			}
			else
			{
				std::scoped_lock lock(m_clientAddrMutex);
				recepients = m_clientAddrCollection;
			}

			// 3. send out the data to connected recepients
			for (auto& recepiantSocket : recepients)
			{
				m_otherSocketAddress = recepiantSocket;

				memcpy(buffer, msgToSend.c_str(), msgToSend.size() / sizeof(char));

				InetPton(AF_INET, _T("127.0.0.1"), &m_otherSocketAddress.sin_addr.s_addr);

				auto resp = sendto(m_socketServDescriptor, buffer, strlen(buffer), 0, (struct sockaddr*)&m_otherSocketAddress, sizeof(m_otherSocketAddress));
				if (resp == SOCKET_ERROR)
				{
					m_wsaLastError = WSAGetLastError();
					m_SendError = true;
					break;
				}

				memset(buffer, '\0', Config::OutgoingDataBufSize);
			}
		}	
	}

	bool SocketConnector::doHandshake()
	{
		auto handshakeSuccesfull = false;

		if (createSocket())
		{
			auto handshakeFuture = std::async(std::launch::async, &SocketConnector::doHandshakeSendRecv, this);

			auto status = handshakeFuture.wait_for(100ms);

			if (status == std::future_status::ready)
			{
				handshakeSuccesfull = handshakeFuture.get();
			}

			// abrupt stop to the doHandshake task in case was blocked at recv..
			closesocket(m_socketServDescriptor);
		}

		return handshakeSuccesfull;
	}

	bool SocketConnector::doHandshakeSendRecv()
	{
		bool handshakeSuccesfull = true;

		char sendBuffer[Config::HandshakeDataBufSize]{ '\0' };
		char recvBuffer[Config::HandshakeAckDataBufSize]{ '\0' };

		// server adress:
		struct sockaddr_in siOther = m_commonServerAddress;
		int siOtherSize = sizeof(siOther);

		memcpy(sendBuffer, Config::InitHandshake, strlen(Config::InitHandshake) / sizeof(char));

		auto resp = sendto(m_socketServDescriptor, sendBuffer, strlen(sendBuffer), 0, (struct sockaddr*)&siOther, sizeof(siOther));

		auto dataReceivedAmount = recvfrom(m_socketServDescriptor, recvBuffer, Config::HandshakeAckDataBufSize, 0, (struct sockaddr*)&siOther, &siOtherSize);

		// Socket to repond with a error or wrong answer
		if ((dataReceivedAmount == SOCKET_ERROR) || (isHandshakeAck(recvBuffer) == false))
		{
			m_wsaLastError = WSAGetLastError();
			handshakeSuccesfull = false;
		}

		memset(recvBuffer, '\0', Config::HandshakeAckDataBufSize);

		return handshakeSuccesfull;
	}

	bool SocketConnector::isHandshake(std::string potencialHandshake)
	{
		std::string initHandshakeStr(Config::InitHandshake);
		
		auto res = initHandshakeStr.compare(potencialHandshake);

		return (res == 0);
	}

	bool SocketConnector::isHandshakeAck(std::string potencialHandshakeAck)
	{
		std::string initHandshakeAckStr(Config::InitHandshakeAck);

		auto res = initHandshakeAckStr.compare(potencialHandshakeAck);

		return (res == 0);
	}
}
