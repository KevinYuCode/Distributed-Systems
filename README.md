# ELEC 477 - Distributed Systems

This repository contains the lab assignments assigned during ELEC 477 and my corresponding solutions to these assignments. 

## Assignment 1
The lab requires you to implement a **Remote Procedure Call (RPC)** system using the User Datagram Protocol (UDP) in C++. The primary objective is to create a **key-value server** with two main functionalities:

1. **`kvPut`**: Store a value (byte array) using an integer key on the server.
2. **`kvGet`**: Retrieve a value associated with a given key.

The lab also introduces **Google Protobuf** for data serialization and requires handling network failures and delays through a simulated network environment. Students must:

- Design client and server programs.
- Implement marshalling/unmarshalling of data.
- Handle UDP message constraints like maximum packet size and reliability.
- Incorporate a persistent storage mechanism for the server (using `gdbm` or similar libraries).
- Perform extensive testing for scenarios like data consistency, multi-client/server setups, and error handling.

The deliverables include:
- A **design document** explaining the solution.
- Test cases with outputs demonstrating successful implementation and robustness.

## Assignment 2
The lab focuses on enhancing the **Remote Procedure Call (RPC)** system by introducing a **name service** and improving error recovery mechanisms. Students are tasked with the following:

1. **Name Service Implementation**:
   - Develop a **service directory server** that allows registering, searching, and deleting services.
   - Implement a flat namespace where multiple key-value (KV) servers can register with human-readable names (e.g., "apartments" or "stocks").
   - Use a C++ `map` to associate service names with server names and ports.
   - Create a client stub to interact with the service directory for registering, searching, and deleting entries.

2. **Integrating Name Service into KV RPC**:
   - Modify the KV servers to register their service name and port with the directory service during startup and deregister when shutting down.
   - Update the KV client stub to query the directory service for the server's address and port before connecting.

3. **Improving RPC Semantics**:
   - Change the `put` method to use **at-least-once semantics**, ensuring the operation is retried until it succeeds or reaches a maximum retry limit.
   - Apply the same semantics to the search and delete methods of the name service.

4. **Testing**:
   - Test the service directory independently by storing and retrieving service information.
   - Test the integration by running multiple KV servers with different service names and verifying that clients can locate and interact with the correct server.
   - Confirm that deregistered services are no longer accessible.

Deliverables include:
- A design document explaining the solution and design decisions.
- A test document detailing the test cases and their results.

## Assignment 3
The lab focuses on implementing a simplified version of the **Primary-Backup consistency model** for the key-value (KV) server system. Students are tasked with extending the KV server from Assignment 2 to handle primary-replica synchronization for improved fault tolerance and consistency.

### Key Objectives:
1. **Primary-Backup Architecture**:
   - Only the primary KV server registers with the service directory.
   - All client requests are routed through the primary server.
   - For `put` requests, the primary updates itself first and then forwards the changes to all replica servers.
   - A response is sent back to the client only after all replicas confirm the update.

2. **Server Role Configuration**:
   - The primary and replicas are configured at runtime, with distinct GDBM files for each server.
   - Replica servers receive the primary's name and port, while the primary knows the names and ports of all replicas.

3. **Message Handling**:
   - Primary servers forward `put` messages to replicas.
   - Replicas verify that messages come from the correct primary server.
   - Error handling and forwarding unprocessed messages are simplified for this assignment (no handling of delays or missing messages).

4. **Testing**:
   - Verify that `put` messages are correctly forwarded to replica servers and that values are updated.
   - Use trace methods to confirm consistency across servers.

### Deliverables:
- A **design document** explaining the solution and implementation details.
- A **test document** describing test cases, including scenarios validating primary-replica synchronization.

## Assignment 4
The lab focuses on developing a **Data Distribution System (DDS)** for air traffic control using Cyclone DDS. The goal is to simulate real-time airspace management and data exchange between zones. Students are tasked with creating programs to manage and analyze flight data.

### Key Objectives:
1. **Cyclone DDS Setup**:
   - Use Cyclone DDS to distribute and subscribe to air traffic data, leveraging the provided installation on the university's server or locally.

2. **Program Development**:
   - **Program 1: Toronto Approach/Departure (Toronto AD)**:
     - Simulates air traffic control for the area within 8 nautical miles of the airport and below 3000 feet.
   - **Program 2: Toronto Centre (Enroute Airspace)**:
     - Manages air traffic in the surrounding enroute airspace beyond 8 nautical miles or above 3000 feet.
   - **Program 3: Query Program**:
     - Retrieves the latest state information for a given flight number using the DDS DataReader.

3. **Control Transfer Simulation**:
   - Implement a new message type to represent the handoff of control between airspace zones (e.g., Toronto AD to Toronto Centre).
   - Use the **Haversine formula** to calculate distances between latitude/longitude coordinates.
   - Simulate handoff messages when aircraft:
     - Cross the 8 nautical mile boundary.
     - Ascend or descend through 3000 feet.
   - Apply content filtering to ensure zones only process relevant messages.

4. **Data and State Handling**:
   - Work with real-world Toronto air traffic data provided in a CSV file.
   - Extend the `state.idl` file to define message types and structures for flight data and control transfer.
   - Avoid creating custom data structures—rely on DDS features like DataReaders for querying and state storage.

5. **Testing Scenarios**:
   - Verify system behavior for various conditions:
     1. Aircraft leaving or entering Toronto AD by crossing the 8nm boundary.
     2. Aircraft ascending or descending through 3000 feet.
   - Use the provided Toronto air traffic data and additional test files to validate functionality.

### Deliverables:
- A **design document** explaining the three programs and the overall system architecture.
- A **testing document** detailing the test cases, inputs, outputs, and how the system handles the required scenarios.

