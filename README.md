**Project Description:**

This project implements a simple client-server system that allows users to perform basic file and directory operations on a remote server. The client connects to the server and can send instructions to list files, create folders, delete files/folders, change directories, upload files to the server, and download files from the server.

**Project Structure:**

The project is organized into two main directories:

- **client:** Contains the source code of the client, which runs on the user's local machine.
- **server:** Contains the source code of the server, which runs on a remote machine.

**Additional Scripts:**

Two Bash scripts are included to facilitate project compilation and execution:

- **`compile_and_run_server.sh`:** This script compiles the server and client programs and then executes the server.
- **`run_client.sh`:** This script runs the client program.

**Prerequisites:**

- C Compiler

**Compilation and Installation:**

**1. Without Using the Scripts:**

- **Client:**
  - Navigate to the `client` directory.
  - Execute the following command to compile the client executable: `gcc client.c -o client`
- **Server:**
  - Navigate to the `server` directory.
  - Execute the following command to compile the server executable: `gcc server.c -o server`

**2. Using the Scripts:**

- Open a terminal and navigate to the root directory of the project.
- Execute the following command to compile and run the server: `./compile_and_run_server.sh`

**Execution:**

**1. Without Using the Scripts:**

- **Server:**
  - Open a terminal and execute the following command to start the server: `./server`
- **Client:**
  - Open another terminal and execute the following command to start the client: `./client`
  - Follow the on-screen instructions to interact with the server.

**2. Using the Scripts:**

- The `compile_and_run_server.sh` script automatically starts the server after compilation.
- To start the client, execute the following command: `./run_client.sh`
- Follow the on-screen instructions to interact with the server.

**Code Explanation:**

- **Client:**
  - The `client.c` file contains the client implementation.
  - The client connects to the server using the `socket()` function and the `connect()` function.
  - The client sends instructions to the server using the `send()` function.
  - The client receives responses from the server using the `recv()` function.
  - The client processes server responses and displays results to the user.
- **Server:**
  - The `server.c` file contains the server implementation.
  - The server creates a socket using the `socket()` function and the `bind()` function.
  - The server listens for client connections using the `listen()` function.
  - The server accepts client connections using the `accept()` function.
  - The server receives instructions from clients using the `recv()` function.
  - The server processes client instructions and sends responses using the `send()` function.

**Notes:**

- This project is a basic example and can be expanded to include more functionalities.
- It is recommended to review the code to better understand its operation.
- The `compile_and_run_server.sh` and `run_client.sh` scripts are optional and can be used to facilitate project compilation and execution.

**Acknowledgments:**

To you, for using this project and for your interest in software development.

**Contributing:**

I appreciate contributions to enhance and improve this project. If you'd like to contribute, please follow these steps:

1. Fork the repository to your own GitHub account.
2. Clone the forked repository to your local machine.
3. Make your desired changes or additions to the codebase.
4. Test your changes thoroughly to ensure they work as expected.
5. Commit your changes with clear and descriptive commit messages.
6. Push your changes to your forked repository.
7. Create a pull request from your forked repository to the main repository.
8. Provide a detailed description of your changes in the pull request.

Thank you for considering contributing to this project. Your efforts are highly appreciated!
