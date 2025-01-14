#include <cstring>
#include <unistd.h>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

class CalendarServer {
public:
    CalendarServer(int port) : port(port) {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << "Error in opening socket" << std::endl;
            exit(1);
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Error in binding to port" << std::endl;
            exit(2);
        }
    }

    void start() {
        if (listen(serverSocket, 10) < 0) {
            std::cerr << "Error in listening" << std::endl;
            exit(3);
        }
        std::cout << "Server listening on port " << port << std::endl;

        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

            if (clientSocket < 0) {
                std::cerr << "Error in accepting client connection" << std::endl;
                continue;
            }

            pthread_t threadId;
            if (pthread_create(&threadId, nullptr, handleClient, new int(clientSocket)) != 0) {
                std::cerr << "Failed to create thread" << std::endl;
            }
        }
    }

private:
    int serverSocket;
    int port;
    static std::map<std::string, std::map<std::string, std::vector<std::string>>> userCalendars;
    static pthread_mutex_t calendarMutex;

    static void* handleClient(void* arg) {
        int clientSocket = *(int*)arg;
        char buffer[1024] = {0};

        while (true) {
            ssize_t bytesRead = read(clientSocket, buffer, 1024);
            if (bytesRead <= 0) {
                break;
            }
            std::string command(buffer);
            std::memset(buffer, 0, sizeof(buffer));
            processCommand(command, clientSocket);
        }

        close(clientSocket);
        delete (int*)arg;
        return nullptr;
    }

    static void processCommand(const std::string& command, int clientSocket) {
        std::istringstream iss(command);
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>{}};

        if (tokens.empty()) return;

        if (tokens[0] == "create_user" && tokens.size() == 2) {
            createUser(tokens[1], clientSocket);
        } else if (tokens[0] == "add_event" && tokens.size() >= 4) {
            std::string username = tokens[1];
            std::string day = tokens[2];
            std::string event = combine(tokens.begin() + 3, tokens.end(), " ");
            addEvent(username, day, event, clientSocket);
        } else if (tokens[0] == "list_events" && tokens.size() == 2) {
            listEvents(tokens[1], clientSocket);
        } else {
            std::string message = "Error: Unknown command";
            send(clientSocket, message.c_str(), message.length(), 0);
        }
    }

    static void createUser(const std::string& username, int clientSocket) {
        pthread_mutex_lock(&calendarMutex);
        if (userCalendars.find(username) == userCalendars.end()) {
            std::map<std::string, std::vector<std::string>> calendar;
            const std::vector<std::string> daysOfWeek = {
                "Monday", "Tuesday", "Wednesday", "Thursday",
                "Friday", "Saturday", "Sunday"
            };
            for (const auto& day : daysOfWeek) {
                calendar[day] = {};
            }
            userCalendars[username] = calendar;
            std::string message = "User created successfully";
            send(clientSocket, message.c_str(), message.length(), 0);
        } else {
            std::string message = "Error: User already exists";
            send(clientSocket, message.c_str(), message.length(), 0);
        }
        pthread_mutex_unlock(&calendarMutex);
    }


    static void addEvent(const std::string& username, const std::string& day, const std::string& event, int clientSocket) {
        pthread_mutex_lock(&calendarMutex);
        if (userCalendars.find(username) != userCalendars.end()) {
            std::string dayLower = day;
            std::transform(dayLower.begin(), dayLower.end(), dayLower.begin(), ::tolower);

            const std::map<std::string, std::string> dayMapping = {
                {"monday", "Monday"}, {"tuesday", "Tuesday"}, {"wednesday", "Wednesday"},
                {"thursday", "Thursday"}, {"friday", "Friday"}, {"saturday", "Saturday"},
                {"sunday", "Sunday"}
            };

            if (dayMapping.find(dayLower) != dayMapping.end()) {
                std::string normalizedDay = dayMapping.at(dayLower);
                userCalendars[username][normalizedDay].push_back(event);
                std::string message = "Event added successfully";
                send(clientSocket, message.c_str(), message.length(), 0);
            } else {
                std::string message = "Error: Invalid day of the week";
                send(clientSocket, message.c_str(), message.length(), 0);
            }
        } else {
            std::string message = "Error: User does not exist";
            send(clientSocket, message.c_str(), message.length(), 0);
        }
        pthread_mutex_unlock(&calendarMutex);
    }


    static void listEvents(const std::string& username, int clientSocket) {
        pthread_mutex_lock(&calendarMutex);
        if (userCalendars.find(username) != userCalendars.end()) {
            const std::vector<std::string> daysOfWeek = {
                "Monday", "Tuesday", "Wednesday", "Thursday",
                "Friday", "Saturday", "Sunday"
            };

            std::string events = "Events:\n";
            for (const auto& day : daysOfWeek) {
                events += day + ":\n";
                for (const auto& event : userCalendars[username][day]) {
                    events += "  " + event + "\n";
                }
            }
            send(clientSocket, events.c_str(), events.length(), 0);
        } else {
            std::string message = "Error: User does not exist";
            send(clientSocket, message.c_str(), message.length(), 0);
        }
        pthread_mutex_unlock(&calendarMutex);
    }

    template <typename Iter>
    static std::string combine(Iter begin, Iter end, const std::string& separator) {
        std::ostringstream result;
        if (begin != end) {
            result << *begin++;
        }
        while (begin != end) {
            result << separator << *begin++;
        }
        return result.str();
    }

    static std::string toLower(const std::string& str) {
        std::string result;
        for (char c : str) {
            result += std::tolower(c);
        }
        return result;
    }
};

std::map<std::string, std::map<std::string, std::vector<std::string>>> CalendarServer::userCalendars;
pthread_mutex_t CalendarServer::calendarMutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    int port = 1337;
    CalendarServer server(port);
    server.start();
    return 0;
}
