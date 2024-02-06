#include "e1service.hpp"
#include "data.pb.h"
#include <gdbm.h>

#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

using namespace std;
using namespace string_literals;

#define close mclose

/**
 * Set the gdbm file name for the server stub
 */
void E1ServiceServer::setGdbmFile(string gdbm_name)
{
    gdbm_file = gdbm_name;
}

/**
 * Stop the server stop
 */
void E1ServiceServer::stop()
{
    alive = false;
}

/**
 * Start the server stub to start listening to requests on the port
 */
void E1ServiceServer::start()
{

    // Create a socket endpoint
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Clear variables before initializing
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Port and Interface information for binding the socket
    servaddr.sin_family = AF_INET;         // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // all interfaces
    servaddr.sin_port = htons(PORT);       // Port number

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len;
    char clientStrBuffer[20];

    // Open the database
    gdbm_database = gdbm_open(gdbm_file.c_str(), 0, GDBM_WRCREAT, 0644, 0);

    // While inside the loop waiting for a call from the client
    while (alive)
    {

        len = sizeof(cliaddr); // Length of the client address

        // Recieve message from client
        int n = recvfrom(sockfd, (uint8_t *)udpMessage, MAXMSG,
                         MSG_WAITALL, (struct sockaddr *)&cliaddr,
                         &len);

        // Convert a internet address in binary network format
        const char *clientstr = inet_ntop(AF_INET, &(cliaddr.sin_addr), clientStrBuffer, 20);

        // Ensures server recieved data
        if (n < 0)
        {
            throw("SERVER RECEIVED ZERO BYTES OOF.");
        }

        cout << "E1SERVICE: server received " << n << " bytes" << endl;

        // Ensures the client address is not null
        if (clientstr != nullptr)
        {
            cout << "E1SERVICE: Client Address " << clientstr << endl;
        }

        Data::key_value_message kv_message;

        // Parse parse udp message from client
        if (!kv_message.ParseFromArray(udpMessage, n))
        {
            // Handle error: Deserialization failed
            throw("Message failed to parse");
        }

        // Check header and do any processing needed
        checkHeader(&kv_message);

        // Send the message to the dispatcher to determine what command is being requested from the client.
        dispatch(&kv_message);
    }

    // Close the database and socket when the server is closed.
    gdbm_close(gdbm_database);
    close(sockfd);
}

/**
 * Dispatcher used to determine what RPC method to call based on the client
 */
void E1ServiceServer::dispatch(Data::key_value_message *kv_message)
{
    if (kv_message->has_put_request())
    {
        put_request(kv_message);
    }
    else if (kv_message->has_get_request())
    {
        get_request(kv_message);
    }
}

/**
 * RPC put method implementation
 */
void E1ServiceServer::put_request(Data::key_value_message *kv_message)
{
    // Datum used to store the key and value to store in gdbm
    datum key;
    datum value;

    if (gdbm_database == nullptr)
    {
        throw("E1SERVICE: Could not open database.");
    }

    // Destructure put request key and value
    auto &request = kv_message->put_request();
    uint32_t key_value = request.key();
    const std::string &value_str = request.value();

    // Key
    key.dptr = reinterpret_cast<char *>(&key_value);
    key.dsize = sizeof(key_value);

    // Value
    value.dptr = const_cast<char *>(value_str.data());
    value.dsize = sizeof(value.dptr);

    // Store the datum values into the gdbm database
    int success = gdbm_store(gdbm_database, key, value, GDBM_REPLACE);

    // Define the key value response header and value
    Data::key_value_message kv_response;
    Data::message_header *header = kv_response.mutable_header();
    header->set_magic_number(1);
    header->set_version(1);
    header->set_message_id(kv_message->header().message_id());
    Data::put_response *response = kv_response.mutable_put_response();
    response->set_success(success);

    // Serialize the response
    std::string serializedResponse;
    if (!kv_response.SerializeToString(&serializedResponse))
    {
        throw("E1SERVICE: failed to serialize response");
    }

    // Send response back to the client
    ssize_t sentBytes = sendto(sockfd, serializedResponse.data(), serializedResponse.size(), 0,
                               (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    // Message failed to send back to client
    if (sentBytes == -1)
    {
        // Handle error in sending
        throw("Send response failed");
    }

    cerr << "--------------- E1SERVICE: Put Request Finished ---------------" << endl;
}

void E1ServiceServer::get_request(Data::key_value_message *kv_message)
{
    // Datum used to store the key and value to store in gdbm
    datum gdbm_key;

    // Check for null database
    if (gdbm_database == nullptr)
    {
        throw("E1SERVICE: Could not open database.");
    }

    // Destructure put request key and value
    auto &request = kv_message->get_request();
    uint32_t key_value1 = request.key();

    // Key
    gdbm_key.dptr = reinterpret_cast<char *>(&key_value1);
    gdbm_key.dsize = sizeof(key_value1);

    // Fetch the value from the gdbm database
    datum gdbm_value = gdbm_fetch(gdbm_database, gdbm_key);

    // Define the key value response header and value
    Data::key_value_message kv_msg;
    Data::message_header *header_get = kv_msg.mutable_header();
    header_get->set_magic_number(1);
    header_get->set_version(1);
    header_get->set_message_id(kv_message->header().message_id());
    Data::get_response *response_get = kv_msg.mutable_get_response();

    // Set the key value response's value
    if (gdbm_value.dptr != nullptr)
    {
        // Value found
        response_get->set_status(true);
        response_get->set_value(gdbm_value.dptr, gdbm_value.dsize);
        response_get->set_value_length(gdbm_value.dsize);
        free(gdbm_value.dptr); // Free memory allocated by gdbm_fetch
    }
    else
    {
        // Value not found
        response_get->set_status(false);   // Set the status field
        response_get->clear_value();       // Clear the value field
        response_get->set_value_length(0); // Set value_length to 0
    }

    // Serialize the response
    std::string serializedResponse;
    if (!kv_msg.SerializeToString(&serializedResponse))
    {
        throw("E1SERVICE: Could not open database.");
    }

    // Send response back to the client
    ssize_t sentBytes = sendto(sockfd, serializedResponse.data(), serializedResponse.size(), 0,
                               (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    // Message failed to send back to client
    if (sentBytes == -1)
    {
        // Handle error in sending
        throw("Send response failed");
    }

        cerr << "--------------- E1SERVICE: Get Request Finished ---------------" << endl;

}

/**
 * Here is where you would check for appropriate magic number, version number of the protocol, and message id sequence
 */
void E1ServiceServer::checkHeader(Data::key_value_message *kv_message)
{
    const Data::message_header &header = kv_message->header();
    uint32_t magicNumber = header.magic_number();
    uint32_t version = header.version();
    uint32_t messageId = header.message_id();
}
