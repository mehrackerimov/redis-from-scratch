# Redis From Scratch

A Redis-inspired in-memory key-value store built from scratch in C++.

This project is primarily focused on learning how databases, command handling, authentication, expiration, and server-side architecture work internally.

> This is an educational project and is not intended to be a production-ready Redis replacement.

---

## Features

### Key-Value Operations

- `SET` - Store a value
- `GET` - Retrieve a value
- `DEL` - Delete a key
- `EXISTS` - Check whether a key exists

### Key Expiration

- `SET ... EX <seconds>` - Set a key with an expiration time
- `EXPIRE` - Set an expiration time on an existing key
- `TTL` - Get the remaining lifetime of a key
- Automatic expiration checking

### Authentication

- User management
- Username and password authentication
- Session-based authentication
- Authentication required before protected commands

### Architecture

```text
Client
  │
  ▼
CommandHandler
  │
  ├── UserManager
  │
  └── Database
```

Main components:

- **Database** - Handles key-value storage and expiration
- **CommandHandler** - Parses and handles commands
- **UserManager** - Manages users and authentication
- **Session** - Stores authentication state for a connection

---

## Supported Commands

### SET

Store a value:

```text
SET name Mehrac
```

Response:

```text
OK
```

Set a value with expiration:

```text
SET name Mehrac EX 60
```

The key will expire after 60 seconds.

---

### GET

Retrieve a value:

```text
GET name
```

Response:

```text
Mehrac
```

---

### DEL

Delete a key:

```text
DEL name
```

Response:

```text
1
```

If the key does not exist:

```text
0
```

---

### EXISTS

Check whether a key exists:

```text
EXISTS name
```

Response:

```text
1
```

or:

```text
0
```

---

### EXPIRE

Set an expiration time:

```text
EXPIRE name 60
```

The key will expire after 60 seconds.

---

### TTL

Check the remaining lifetime:

```text
TTL name
```

Example:

```text
59
```

Possible return values:

```text
-1    Key exists but has no expiration
-2    Key does not exist
```

---

### LOGIN

Authenticate a user:

```text
LOGIN mehrac 12345
```

Successful authentication:

```text
OK
```

Invalid credentials:

```text
ERR: Invalid username or password
```

Authentication is stored inside the current session.

---

## Example

```text
> LOGIN mehrac 12345
OK

> SET name Mehrac
OK

> GET name
Mehrac

> EXISTS name
1

> EXPIRE name 10
1

> TTL name
9

> DEL name
1

> GET name
ERR: Key not found
```

---

## Project Structure

```text
redis-from-scratch/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
│
├── include/
│   ├── database.h
│   ├── command_handler.h
│   ├── user_manager.h
│   └── session.h
│
├── src/
│   ├── main.cpp
│   ├── database.cpp
│   ├── command_handler.cpp
│   └── user_manager.cpp
│
└── build/
```

### Database

Responsible for:

- Storing key-value pairs
- Retrieving values
- Deleting keys
- Checking key existence
- Managing expiration times
- Calculating TTL

The data is currently stored in memory using:

```cpp
std::unordered_map
```

### CommandHandler

Responsible for:

- Receiving commands
- Parsing command arguments
- Validating commands
- Calling the appropriate database operation
- Handling authentication-related commands

Example:

```text
SET name Mehrac
       │
       ▼
CommandHandler
       │
       ▼
Database::set()
```

### UserManager

Responsible for:

- Creating users
- Checking whether users exist
- Authenticating usernames and passwords

### Session

Stores the authentication state of the current connection.

Example:

```cpp
struct Session
{
    bool authenticated = false;
    std::string username;
};
```

---

## Technologies

- **C++20**
- **CMake**
- **STL**
- `std::unordered_map`
- `std::optional`
- `std::chrono`

---

## Building

### Requirements

- C++20 compatible compiler
- CMake
- Windows / Visual Studio Build Tools or another compatible compiler

### Configure

```bash
cmake -S . -B build
```

### Build

```bash
cmake --build build
```

The executable will be generated inside the build directory.

---

## Running

On Windows:

```bash
build\Debug\redis_from_scratch.exe
```

You can also use the included build script if available:

```bash
build-and-run.bat
```

---

## Design Goals

The main goal of this project is to understand how a Redis-like system can be built from the ground up.

The project focuses on learning:

- Data structures
- Command parsing
- Authentication
- Session management
- Key expiration
- C++ class design
- Separation of responsibilities
- CMake project structure
- Networking
- Concurrency

---

## Roadmap

### Completed

- [x] CMake project setup
- [x] In-memory key-value storage
- [x] `SET`
- [x] `GET`
- [x] `DEL`
- [x] `EXISTS`
- [x] `EXPIRE`
- [x] `TTL`
- [x] Key expiration with `EX`
- [x] User management
- [x] Password authentication
- [x] Session authentication
- [x] CommandHandler
- [x] Separate database logic from `main.cpp`
- [x] TCP server
- [x] Multiple client connections
- [x] Concurrent clients
- [x] Thread-safe database

### Planned

- [ ] Better command parser
- [ ] More Redis commands
- [ ] Persistence
- [ ] Automated tests
- [ ] Logging
- [ ] Configuration system
- [ ] Performance benchmarks

---

## Disclaimer

This project is created for educational purposes and is inspired by Redis.

It is not intended to be compatible with Redis or to replace Redis in production environments.

---

## License

This project is licensed under the MIT License.
