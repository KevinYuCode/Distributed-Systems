#include "clientStub.hpp"
#include "data.pb.h"

using namespace std;
using namespace std::string_literals;

ClientStub::ClientStub()
{
    currentSerialNumber = 0;
}

void ClientStub::setServerAddress(char *server_addr)
{
    serverAddress = server_addr;
}

/**
 * Establishes a network connection by creating the socket
 */
void ClientStub::createSocket()
{
    // ------------------------------------------ CONFIGURE SERVER ADDRESS------------------------------------ //
    memset(&servaddr, 0, sizeof(servaddr)); // Setting the initial values
    servaddr.sin_family = AF_INET;          // Protocol family
    servaddr.sin_port = htons(PORT);

    // client port
    inet_aton("10.0.0.2", &servaddr.sin_addr);

    // Open a socket to send the info.Checks to see if the socket file descriptor creation failed
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    tv.tv_sec = 5;  // Set timeout in seconds
    tv.tv_usec = 0; // Not using microseconds

    // Set socket options here:
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error setting the socket timeout");
    }
}

Data::put_response ClientStub::put(uint32_t key, const uint8_t *value, uint16_t vlen)
{

    createSocket();

    Data::key_value_message kv_message;
    Data::message_header *header = kv_message.mutable_header();
    header->set_magic_number(1);
    header->set_version(1);
    header->set_message_id(++currentSerialNumber);

    // Create a PUT request
    Data::put_request *put_request = kv_message.mutable_put_request();
    put_request->set_key(key);
    put_request->set_value(value, vlen);

    // Serialize the message
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

    // Send the serialized data
    int n = sendto(sockfd, serializedData.data(), serializedData.size(),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    cout << "CLIENT STUB: NUMBER OF BYTES SENT: " << n << endl;

    if (n < 0)
    {
        // Handle error in sending
        throw("No message sent to server");
    }

    bool validResponse = false;
    Data::key_value_message kv_response;

    int attempt = 0;
    const int MAX_ATTEMPTS = 3; // for example, retry 3 times
    while (!validResponse && attempt < MAX_ATTEMPTS)
    {
        char responseBuffer[MAXMSG];
        len = sizeof(struct sockaddr_in);
        int responseLength = recvfrom(sockfd, (char *)buffer, MAXMSG,
                                      MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

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

        // Deserialize the response
        if (!kv_response.ParseFromArray(buffer, responseLength))
        {
            throw std::runtime_error("Failed to deserialize response");
        }

        // Verify serial number
        if (kv_response.header().message_id() == currentSerialNumber)
        {
            validResponse = true;
            std::cout << "CLIENT STUB: Received Magic Number: " << kv_response.header().magic_number() << std::endl;
        }
        else
        {
            std::cout << "CLIENT STUB: Received message with incorrect serial number. Waiting for the correct message..." << std::endl;
            // Optionally, you could add a timeout or a maximum number of attempts to avoid an infinite loop
        }
    }

    // Max attempts achieved throw and error after attempting 3 times
    if (attempt == MAX_ATTEMPTS)
    {
        // throw std::runtime_error("Maximum number of attempts reached, no valid response received");
        std::cout << "Maximum number of attempts reached, no valid response received" << std::endl;
    }

    // Check if the response contains a put_response
    if (kv_response.has_put_response())
    {
        const Data::put_response &response = kv_response.put_response();

        // Handle the response
        bool success = response.success();

        std::cout << "CLIENT STUB: this is the response from the server: " << success << std::endl;
        return response;
    }
    else
    {
        throw std::runtime_error("Invalid response type received");
    }
}

// -------------------------------------- GET -------------------------------------- //
Data::get_response ClientStub::get(uint32_t key)
{
    createSocket();

    Data::key_value_message kv_message;
    Data::message_header *header = kv_message.mutable_header();
    header->set_magic_number(1);
    header->set_version(1);
    header->set_message_id(currentSerialNumber);

    // Create a PUT request
    Data::get_request *get_request = kv_message.mutable_get_request();
    get_request->set_key(key);

    // Serialize the message
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

    // Send the serialized data
    int n = sendto(sockfd, serializedData.data(), serializedData.size(),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (n < 0)
    {
        // Handle error in sending
        throw("No message sent to server");
    }

    bool validResponse = false;
    Data::key_value_message kv_response;
    int attempt = 0;
    const int MAX_ATTEMPTS = 3; // for example, retry 3 times
    while (!validResponse && attempt < MAX_ATTEMPTS)
    {
        char responseBuffer[MAXMSG];
        len = sizeof(struct sockaddr_in);
        int responseLength = recvfrom(sockfd, (char *)buffer, MAXMSG,
                                      MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

        // std::cerr << HexDump{buffer, (uint32_t)n} << endl;

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

        // Deserialize the response
        if (!kv_response.ParseFromArray(buffer, responseLength))
        {
            throw std::runtime_error("Failed to deserialize response");
        }

        // Verify serial number
        if (kv_response.header().message_id() == currentSerialNumber)
        {
            validResponse = true;
            std::cout << "CLIENT STUB: Received Magic Number: " << kv_response.header().magic_number() << std::endl;
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
        const Data::get_response &response = kv_response.get_response();

        return response;
    }
    else
    {
        throw std::runtime_error("Invalid response type received");
    }
}

void ClientStub::closeSocket()
{
    close(sockfd);
}