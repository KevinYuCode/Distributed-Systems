#include "clientStub.hpp"
#include "data.pb.h"

using namespace std;
using namespace std::string_literals;

/**
 * Initialize the currentSerialNumber to be attached the key value message header
 */
ClientStub::ClientStub()
{
    currentSerialNumber = 0;
}

/**
 * Set the server address the client stub will send data to
 */
void ClientStub::setServerAddress(char *server_addr)
{
    this->serverAddress = server_addr;
}

/**
 * Initialize the socket the client stub will use
 */
void ClientStub::createSocket()
{
    memset(&servaddr, 0, sizeof(servaddr)); // Setting the initial values
    servaddr.sin_family = AF_INET;          // Protocol family
    servaddr.sin_port = htons(PORT);        // Set the port number

    // Converts the string address to the appropriate format
    inet_aton(serverAddress, &servaddr.sin_addr);

    // Open a socket to send the info.Checks to see if the socket file descriptor creation failed
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct timeval tv; // Set up the timeout value
    tv.tv_sec = 5;     // Set timeout in seconds
    tv.tv_usec = 0;    // Not using microseconds

    // Set socket options eg. timeout values
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error setting the socket timeout");
    }
}

/**
 * Put RPC call
 */
Data::put_response ClientStub::put(uint32_t key, const uint8_t *value, uint16_t vlen)
{

    // Initialize the socket
    createSocket();

    // Setting the magic number, version number, and message id for the header of the key value message
    Data::key_value_message kv_message;
    Data::message_header *header = kv_message.mutable_header();
    header->set_magic_number(1);
    header->set_version(1);
    header->set_message_id(++currentSerialNumber);

    // Initialize the put request struct data
    Data::put_request *put_request = kv_message.mutable_put_request();
    put_request->set_key(key);
    put_request->set_value(value, vlen);

    // Serialize the key value message and store it into a string
    std::string serializedData;
    if (!kv_message.SerializeToString(&serializedData))
    {
        // Handle serialization error
        throw("Failed to serialize the data...");
    }

    // Ensure the serialized data fits within your buffer size
    if (serializedData.size() > MAXMSG)
    {
        // Handle error: Message too large for buffer
        throw("This message is way to big...");
    }

    // Send the serialized data and return back the number of bytes sent
    int n = sendto(sockfd, serializedData.data(), serializedData.size(),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    // No message set to server
    if (n < 0)
    {
        // Handle error in sending
        throw("No message sent to server");
    }

    bool validResponse = false;          // Used to check if the serial number for the message is correct
    Data::key_value_message kv_response; // Key value message response
    int attempt = 0;                     // Tracks the number of attempts each time the socket times out
    const int MAX_ATTEMPTS = 3;          // Number of attempts for each timeout

    // Waits for response from server until conditions aren't met.
    while (!validResponse && attempt < MAX_ATTEMPTS)
    {
        char responseBuffer[MAXMSG]; // Buffer holds the response binary data

        // Recieve message from server
        len = sizeof(struct sockaddr_in);
        int responseLength = recvfrom(sockfd, (char *)buffer, MAXMSG,
                                      MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

        // Checks if the received data is empty
        if (responseLength < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                std::cout << "Timeout occurred, no response received." << std::endl;
                // You might want to resend the request, increase attempt count, or handle error
                attempt++;
                continue; // Skip to the next iteration of the loop to retry or handle the error
            }
            else
            {
                // An error other than timeout occurred
                perror("recvfrom");
            }
        }

        // Deserialize the binary data in the buffer received from the response
        if (!kv_response.ParseFromArray(buffer, responseLength))
        {
            throw std::runtime_error("Failed to deserialize response");
        }

        // Verify serial number
        if (kv_response.header().message_id() == currentSerialNumber)
        {
            validResponse = true;
        }
        else
        {
            std::cout << "CLIENT STUB: Received message with incorrect serial number. Waiting for the correct message..." << std::endl;
        }
    }

    // Max attempts achieved throw and error after attempting 3 times
    if (attempt == MAX_ATTEMPTS)
    {
        throw std::runtime_error("Maximum number of attempts reached, no valid response received");
    }

    // Check if the response contains a put_response
    if (kv_response.has_put_response())
    {
        // Retrieve the response status
        const Data::put_response &response = kv_response.put_response();
        bool success = response.success();

        std::cout << "CLIENT STUB: Success status of the put request:" << success << std::endl;
        return response;
    }
    else
    {
        throw std::runtime_error("Invalid response type received");
    }
    closeSocket();
}

/**
 * Put RPC call
 */
Data::get_response ClientStub::get(uint32_t key)
{
    // Initialize the socket
    createSocket();

    // Setting the magic number, version number, and message id for the header of the key value message
    Data::key_value_message kv_message;
    Data::message_header *header = kv_message.mutable_header();
    header->set_magic_number(1);
    header->set_version(1);
    header->set_message_id(currentSerialNumber);

    // Initialize the get request struct data
    Data::get_request *get_request = kv_message.mutable_get_request();
    get_request->set_key(key);

    // Serialize the key value message and store it into a string
    std::string serializedData;
    if (!kv_message.SerializeToString(&serializedData))
    {
        // Handle serialization error
        throw("Failed to serialize the data...");
    }

    // Ensure the serialized data fits within your buffer size
    if (serializedData.size() > MAXMSG)
    {
        // Handle error: Message too large for buffer
        throw("This message is way to big...");
    }

    // Send the serialized data and return back the number of bytes sent
    int n = sendto(sockfd, serializedData.data(), serializedData.size(),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    // No message set to server
    if (n < 0)
    {
        // Handle error in sending
        throw("No message sent to server");
    }

    bool validResponse = false;          // Used to check if the serial number for the message is correct
    Data::key_value_message kv_response; // Key value message response
    int attempt = 0;                     // Tracks the number of attempts each time the socket times out
    const int MAX_ATTEMPTS = 3;          // Number of attempts for each timeout

    // Waits for response from server until conditions aren't met.
    while (!validResponse && attempt < MAX_ATTEMPTS)
    {
        char responseBuffer[MAXMSG]; // Buffer holds the response binary data

        // Recieve message from server
        len = sizeof(struct sockaddr_in);
        int responseLength = recvfrom(sockfd, (char *)buffer, MAXMSG,
                                      MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

        // Checks if the received data is empty
        if (responseLength < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                std::cout << "Timeout occurred, no response received." << std::endl;
                // You might want to resend the request, increase attempt count, or handle error
                attempt++;
                continue; // Skip to the next iteration of the loop to retry or handle the error
            }
            else
            {
                // An error other than timeout occurred
                perror("recvfrom");
            }
        }

        // Deserialize the binary data in the buffer received from the response
        if (!kv_response.ParseFromArray(buffer, responseLength))
        {
            throw std::runtime_error("Failed to deserialize response");
        }

        // Verify serial number
        if (kv_response.header().message_id() == currentSerialNumber)
        {
            validResponse = true;
        }
        else
        {
            std::cout << "CLIENT STUB: Received message with incorrect serial number. Waiting for the correct message..." << std::endl;
        }
    }

    // Max attempts achieved throw and error after attempting 3 times
    if (attempt == MAX_ATTEMPTS)
    {
        throw std::runtime_error("Maximum number of attempts reached, no valid response received");
    }

    // Check if the response contains a get_response
    if (kv_response.has_get_response())
    {
        // Retrieve the response status
        const Data::get_response &response = kv_response.get_response();
        return response;
    }
    else
    {
        throw std::runtime_error("Invalid response type received");
    }
    closeSocket();
}

/**
 * Close the socket end point
 */
void ClientStub::closeSocket()
{
    close(sockfd);
}