# Simple C Web Server (Windows)

This is a simple C web server designed specifically for Windows systems. It serves static HTML files to clients over HTTP and utilizes Windows-specific APIs.

## Prerequisites

Before you begin, ensure you have met the following requirements:

- You have a basic understanding of C programming language.
- You have `gcc` installed on your Windows system.
- You are running this on a Windows system.

## Installation

To install and run the server, follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/AdaiasMagdiel/windows-c-http-server.git
   ```

2. Navigate to the project directory:

   ```bash
   cd windows-c-http-server
   ```

3. Build the project using `Makefile`:

   ```bash
   make
   ```

4. Run the server:

   ```bash
   ./main.exe
   ```

## Usage

Once the server is running, it will listen for incoming connections on `http://0.0.0.0:3000/`. You can access the server using a web browser or tools like `curl`.

## Project Structure

The project consists of the following files:

- `main.c`: Contains the main function where the server is initialized and client connections are handled.
- `src/server.c`: Implements functions for initializing the server socket, accepting connections, and sending data.
- `src/server.h`: Header file for server-related functions.
- `src/content.c`: Implements functions for reading HTML files and generating appropriate HTTP responses.
- `src/content.h`: Header file for content-related functions.
- `Makefile`: Defines rules for building the project.

## Running on Linux

This project is specifically designed for Windows systems and utilizes Windows-specific APIs. To run it on Linux, you may need to make adjustments to the code to replace Windows APIs with their Linux equivalents.

## Contributing

Contributions are welcome! Feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [`LICENSE`](LICENSE) file for details.
