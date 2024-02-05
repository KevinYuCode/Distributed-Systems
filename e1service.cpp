#include "e1service.hpp"
#include "data.pb.h"
#include <gdbm.h>

#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

using namespace std;
using namespace string_literals;

#define close mclose

// Since you will have multiple independent
// servers, each must know the gdbm file that will contain the data. Add a method to your
// service class interface that will be used to set the name of the file, this method will be
// called by the equivalent method in your server class.
void E1ServiceServer::setGdbmFile(string gdbm_name)
{
    gdbm_file = gdbm_name;
}

void E1ServiceServer::stop()
{
    alive = false;
}

void E1ServiceServer::start()
{
    // std::lock_guard<std::mutex> guard(gdbm_mutex); //Did I set this up correctly?

    // ------------------------------ SETS THE NETWORK CONFIGURATION ------------------------------ //

    // get a socket to recieve messges
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // clear variables before initializing
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Port and Interface information for binding the socket
    servaddr.sin_family = AF_INET;         // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // all interfaces
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len;
    char clientStrBuffer[20];
    cout << "E1SERVICE: KEVIN: " << endl;

    cout << "E1SERVICE: Before---------------" << endl;

    gdbm_database = gdbm_open(gdbm_file.c_str(), 0, GDBM_WRCREAT, 0644, 0);

    cerr << "E1SERVICE: after opening db --------------" << endl;
    cerr << "E1SERVICE: alive status:" << alive << endl;

    // Seperate put routine

    // While inside the loop waiting for a call from the client
    while (alive)
    {
        cout << "E1SERVICE: INSIDE ALIVE LOOP waiting for call from client" << endl;

        len = sizeof(cliaddr); // len is value/result

        int n = recvfrom(sockfd, (uint8_t *)udpMessage, MAXMSG,
                         MSG_WAITALL, (struct sockaddr *)&cliaddr,
                         &len);

        const char *clientstr = inet_ntop(AF_INET, &(cliaddr.sin_addr), clientStrBuffer, 20);

        if (clientstr != nullptr)
        {
            cout << "E1SERVICE: from address " << clientstr << endl;
        }

        if (n < 0)
        {
            throw("SERVER RECEIVED ZERO BYTES OOF.");
        }

        // First byte is 1 - put, 2 - get
        // We parse the data from the udpMessage and put it into kv_message
        // The key-value data structure sent from the client
        Data::key_value_message kv_message;
        if (!kv_message.ParseFromArray(udpMessage, n))
        {
            // Handle error: Deserialization failed
            throw("Message failed to parse");
        }

        // Are we supposed to have some type conditional for just message dumping/hexdump???????????????????????????????????
        // ------------------------------------ Message 1: Server dumps the message from the client ------------------------------------ //
        cout << "E1SERVICE: server received " << n << " bytes" << endl;

        if (clientstr != nullptr)
        {
            cout << "E1SERVICE: from address " << clientstr << endl;
        }

        // ------------------------------------ Check Header ------------------------------------ //
        cout << "E1SERVICE: Before checkheader" << endl;

        checkHeader(&kv_message);

        cerr << "E1SERVICE: After check header " << endl;

        dispatch(&kv_message);
    }
    gdbm_close(gdbm_database);
    close(sockfd);
}

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

void E1ServiceServer::put_request(Data::key_value_message *kv_message)
{
    datum key;
    datum value;
    int result;
    cerr << "E1SERVICE: In put request, before" << endl;
    if (gdbm_file == "")
    {
        cerr << "E1SERVICE: L bozo" << endl;
    }
    else
    {
        cerr << gdbm_file << endl;
    }

    cerr << "E1SERVICE: testing" << endl;
    if (gdbm_database == nullptr)
    {
        std::cerr << "E1SERVICE: Could not open database." << std::endl;
    }

    cerr << "E1SERVICE: before request " << endl;
    auto &request = kv_message->put_request();
    uint32_t key_value = request.key();
    const std::string &value_str = request.value();

    cerr << "E1SERVICE: After request" << endl;

    // const Data::put_request &put2_request = kv_message.put_request();

    std::cout << "E1SERVICE: key from e1service setting it is:" << key_value << endl;
    std::cout << "E1SERVICE: value from e1service setting it is:" << value_str << endl;

    // Key
    key.dptr = reinterpret_cast<char *>(&key_value);
    key.dsize = sizeof(key_value);
    cerr << "E1SERVICE: after setting key" << endl;

    // Value
    value.dptr = const_cast<char *>(value_str.data());
    value.dsize = sizeof(value.dptr);

    cerr << "E1SERVICE: after setting key and value" << endl;

    // result = gdbm_store(gdbm_database, key, value, GDBM_REPLACE);
    result = gdbm_store(gdbm_database, key, value, GDBM_REPLACE);
    bool success = (result == 0);
    cerr << "E1SERVICE: After get result " << endl;

    // TESTING PURPOSES
    // std::cout << HexDump{buffer, (uint32_t)n} << endl;

    // datum gdbm_value = gdbm_fetch(gdbm_database, key);
    cerr << "E1SERVICE: CHECKING----------- " << sizeof(key) << endl;

    // gdbm_close(gdbm_database);
    cerr << "E1SERVICE: After gdbm close " << endl;

    // Response back to the client

    cerr << "E1SERVICE: success: " << success << endl;
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
        // Handle serialization error
    }

    // Send the response (assuming sockfd is your UDP socket and cliaddr is the client's address)
    ssize_t sentBytes = sendto(sockfd, serializedResponse.data(), serializedResponse.size(), 0,
                               (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    if (sentBytes == -1)
    {
        // Handle error in sending
        throw("Send response failed");
    }

    cerr << "--------------------- E1SERVICE: PUT request finished ---------------------" << endl;
}

void E1ServiceServer::get_request(Data::key_value_message *kv_message)
{
    datum gdbm_key;
    cerr << "E1SERVICE: In get request, before" << endl;
    // It's a GET request
    // Process the GET request

    cerr << "E1SERVICE: In get request, before fetching" << endl;

    if (gdbm_file == "")
    {
        cerr << "E1SERVICE: L bozo" << endl;
    }
    else
    {
        cerr << gdbm_file << endl;
    }
    if (gdbm_database == nullptr)
    {
        std::cerr << "E1SERVICE: Could not open database." << std::endl;
    }

    // Prepare the key for GDBM
    // datum gdbm_key;
    auto &request = kv_message->get_request();
    uint32_t key_value1 = request.key();
    gdbm_key.dptr = reinterpret_cast<char *>(&key_value1);
    gdbm_key.dsize = sizeof(key_value1);
    cerr << "E1SERVICE: after setting key" << endl;

    cerr << "E1SERVICE (get): key " << key_value1 << endl;
    cerr << "E1SERVICE (get): SIZE" << sizeof(gdbm_key) << endl;

    datum gdbm_value = gdbm_fetch(gdbm_database, gdbm_key);

    cerr << "E1SERVICE: value:fukking this hsit up" << endl;

    // Prepare and send the get_response
    // Data::get_response response;
    cerr << "E1SERVICE: good????" << endl;

    Data::key_value_message kv_msg;
    Data::message_header *header_get = kv_msg.mutable_header();
    header_get->set_magic_number(1);
    header_get->set_version(1);
    header_get->set_message_id(kv_message->header().message_id());
    Data::get_response *response_get = kv_msg.mutable_get_response();

    cerr << "E1SERVICE: before setting status and value into response" << endl;
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
        response_get->set_status(false);
    }

    cerr << "E1SERVICE: after setting status, the status:" << response_get->status() << endl;
    cerr << "E1SERVICE: after setting status, the value:" << response_get->value() << endl;

    // Serialize the response

    // Serialize the response
    std::string serializedResponse;
    if (!kv_msg.SerializeToString(&serializedResponse))
    {
        // Handle serialization error
    }

    // Send the response (assuming sockfd is your UDP socket and cliaddr is the client's address)
    ssize_t sentBytes = sendto(sockfd, serializedResponse.data(), serializedResponse.size(), 0,
                               (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    if (sentBytes == -1)
    {
        // Handle error in sending
        throw("Send response failed");
    }

    // Close the database
}

void E1ServiceServer::checkHeader(Data::key_value_message *kv_message)
{
    std::cout << "E1SERVICE: in check header - service server" << std::endl;
    const Data::message_header &header = kv_message->header();
    uint32_t magicNumber = header.magic_number();
    uint32_t version = header.version();
    uint32_t messageId = header.message_id();
}
