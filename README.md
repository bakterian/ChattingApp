## Chatting App ##

This is a socket based (UDP) windows chatting application.<br>
The first opened application instance tries perofrms a handshake with a different application instance on the same Localhost/LAN.<br>
In case the handshake is not concluded the application instance becomes the server.<br>
The source code is C++ using language features up to the 2017 standard.<br>
Presentation layer is created using the microsoft Windows MFC.<br>
The application is developed, maintained and constantly improved using the Visual Studio IDE.<br>
<br><br>

### App Features

- Multiple chat particpants are supported
- The application is multithreaded and dead-lock protected
- Application has two pop-up Windows one with the Chat User Settings other with the actual chat
- The chat user can choose his nickname in the Chat User Settings window
- The chat window has two controls message box and the main chat dialog control and a send button
- The main chat dialog displays the nick and the time when the message was sent
- When a new chat user is joining he will receive the whole chat history

<br>

### App Features TODOs
- Application to inform about a socket connection blocking firewall problem
- Inspect if Application is not leaking resources
- The user should join the chat when choose to do so a button to be added to the Chat User Settings Window
- The user should have the possability to choose a Avatar (that would then be displayed in the chat window)
- The avatar of the user is to be displayed the Avatar than the name of the chat user
- To easy the runtime testing it would be good to introduce a random user name generator i.e user1274930439
- A debug control would be helpful to see if the application has joined the chat
- A debug control would be helpful to see if the running instance is a server or client
- A usefull feature would be a counter in the chat window which woudl display the number of chat users
- The application should be immune to network disconnects
