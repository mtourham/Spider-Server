# Spider-Server

## Overview

This project is a web server implementation developed as part of the curriculum at 42 School. The web server is designed to handle HTTP requests and serve web content using the C programming language.

## Features

- **HTTP Protocol Support:** Implements a basic HTTP server capable of handling GET and POST requests.
- **Static File Serving:** Serves static HTML, CSS, JavaScript, and image files to clients.
- **Dynamic Content Generation:** Supports dynamic content generation through CGI scripts.
- **Configuration Options:** Allows configuration of server settings through configuration files.

## Getting Started

### Prerequisites

- Make sure you have `gcc` compiler installed on your system.

### Installation

1. Clone the repository:

`git clone <repository-url>`

2. Navigate to the project directory:

`cd Spider-Server`

3. Build the webserver:

`make`

### Usage

1. Run the webserver:

`./webserver config.cfg`


2. Access the webserver:
Open your web browser and navigate to `http://localhost:<port>` where `<port>` is the port number specified in the `config.cfg` file.

### Configuration

You can customize server settings by modifying the `config.cfg` file. Available configuration options include:
- `port`: The port number on which the server listens for incoming connections.
- `document_root`: The directory from which the server serves static files.
- `cgi_bin`: The directory where CGI scripts are located.
