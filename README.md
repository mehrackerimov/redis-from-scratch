# Redis From Scratch

A Redis-inspired in-memory key-value database written in C++ from scratch.

> **Status:** Work in Progress 🚧

This project is built for learning and focuses on TCP networking, command parsing, authentication, sessions, expiration, concurrency, thread safety, and database design.

## Features

### Database

- [x] In-memory key-value storage
- [x] `SET`
- [x] `GET`
- [x] `DEL`
- [x] `EXISTS`
- [x] Key expiration
- [x] `EX`
- [x] `TTL`
- [x] `EXPIRE`
- [x] Thread-safe database access
- [ ] Persistence
- [ ] RDB snapshots
- [ ] AOF

### Networking

- [x] TCP server
- [x] IPv4 / Winsock support
- [x] Multiple client connections
- [x] Concurrent client handling
- [x] Per-client sessions
- [ ] Connection limits
- [ ] Graceful shutdown

### Authentication

- [x] User management
- [x] User creation
- [x] Username/password authentication
- [x] Session authentication state
- [x] Protected commands
- [ ] ACL system
- [ ] Password hashing

### Command System

- [x] Command handler
- [x] Command parser
- [x] Tokenizer
- [x] Structured `Command` representation
- [ ] Better argument validation
- [ ] Quoted arguments
- [ ] Escape character support
- [ ] RESP protocol parser

## Supported Commands

Currently implemented:

```text
PING
SET
GET
DEL
EXISTS
EXPIRE
TTL
LOGIN
```

More Redis-compatible commands will be added over time.

## Architecture

```text
Client
  │
  │ TCP
  ▼
Server
  │
  ▼
Session
  │
  ▼
CommandParser
  │
  ▼
Command
  │
  ▼
CommandHandler
  │
  ├──────────────► Database
  │
  └──────────────► UserManager
```

A command such as:

```text
SET name Mehrac EX 60
```

flows through:

```text
Raw input
   ↓
Tokenizer
   ↓
["SET", "name", "Mehrac", "EX", "60"]
   ↓
CommandParser
   ↓
Command { name, args }
   ↓
CommandHandler
   ↓
Database
```

## Project Structure

```text
redis-from-scratch/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── build-and-run.bat
│
├── include/
│   ├── database.h
│   ├── command.h
│   ├── command_parser.h
│   ├── command_handler.h
│   ├── tokenizer.h
│   ├── user_manager.h
│   ├── session.h
│   └── server.h
│
├── src/
│   ├── main.cpp
│   ├── database.cpp
│   ├── command_parser.cpp
│   ├── command_handler.cpp
│   ├── tokenizer.cpp
│   ├── user_manager.cpp
│   └── server.cpp
│
├── tests/
│   └── ...
│
└── benchmarks/
    └── ...
```

## Database

The database is currently an in-memory hash-table-based store, conceptually similar to:

```cpp
std::unordered_map<std::string, Entry>
```

An entry contains a value and optional expiration information:

```text
Key
 ├── Value
 └── Expiration
```

Expiration is tracked with:

```cpp
std::chrono::steady_clock
```

## Thread Safety

The server supports concurrent clients. Each client is handled independently, while shared database operations are protected with a mutex.

RAII-based locking is used:

```cpp
std::lock_guard<std::mutex>
```

This is preferred over manual:

```cpp
mutex.lock();
mutex.unlock();
```

because the lock is automatically released when the guard leaves scope.

## TCP Server

The server uses Windows Winsock.

Basic lifecycle:

```text
WSAStartup
   ↓
socket()
   ↓
bind()
   ↓
listen()
   ↓
accept()
   ↓
recv()
   ↓
CommandParser
   ↓
CommandHandler
   ↓
send()
```

The default port is `6379`.

Multiple clients can connect concurrently, with each client handled in its own thread.

## Sessions

Each connected client receives its own session.

A session tracks authentication state and the logged-in username:

```cpp
struct Session
{
    bool authenticated;
    std::string username;
};
```

Authentication therefore belongs to a connection rather than being global state inside the command handler.

## Authentication

The project includes a basic in-memory `UserManager`.

Example flow:

```text
LOGIN user1 password
        ↓
   UserManager
        ↓
   authenticate()
        ↓
     Session
```

Unauthenticated clients are prevented from executing protected commands.

The authentication system is intentionally simple and is planned to evolve into an ACL-style permission system.

## Local Console

The same command pipeline can be used from the local console during development:

```text
Local console ready.

> PING
PONG

> SET name Mehrac
OK

> GET name
Mehrac
```

## Building

### Requirements

- Windows
- C++17 or newer
- CMake
- Visual Studio / MSVC
- Winsock2

### Build

```bash
cmake -S . -B build
cmake --build build
```

The Visual Studio executable will typically be:

```text
build/Debug/redis_from_scratch.exe
```

## Running

Run:

```text
build-and-run.bat
```

or:

```text
build\Debug\redis_from_scratch.exe
```

The server starts on port `6379` by default.

## Testing

Tests are kept separate from the main application.

Example:

```text
tests/
├── database_test.cpp
├── command_handler_test.cpp
└── user_manager_test.cpp
```

A simple database test can look like:

```cpp
db.set("name", "Mehrac", std::nullopt);

auto value = db.get("name");

assert(value.has_value());
assert(value.value() == "Mehrac");
```

Planned testing improvements:

- Unit tests
- Integration tests
- Concurrent access tests
- CTest integration
- Dedicated testing framework

## Performance Benchmarks

Benchmarks will measure:

- `SET` operations per second
- `GET` operations per second
- Database throughput
- Concurrent client performance
- Mutex overhead
- Request latency
- Stress-test behavior

Example future output:

```text
Operation    Operations/sec
---------------------------
SET          150,000
GET          200,000
DEL          140,000
```

## Configuration

A configuration system is planned so server settings can be changed without recompiling.

Potential configuration:

```ini
port=6379
max_clients=100
log_level=info
```

Planned settings include:

- Server port
- Maximum clients
- Log level
- Persistence settings
- Database settings
- Authentication settings

## Logging

A dedicated logging system is planned for diagnostics.

Example:

```text
[INFO] Server started on port 6379
[INFO] Client connected
[INFO] Command received: SET
[INFO] User logged in
[ERROR] Failed to accept client
```

Planned features:

- `DEBUG`
- `INFO`
- `WARN`
- `ERROR`
- Timestamps
- File output
- Thread-safe logging

## Roadmap

### Commands

- [ ] `MGET`
- [ ] `MSET`
- [ ] `SETNX`
- [ ] `GETSET`
- [ ] `GETDEL`
- [ ] `GETEX`
- [ ] `APPEND`
- [ ] `STRLEN`
- [ ] `INCR`
- [ ] `INCRBY`
- [ ] `INCRBYFLOAT`
- [ ] `DECR`
- [ ] `DECRBY`
- [ ] `SETEX`
- [ ] `PSETEX`

### Data Structures

- [ ] Lists
- [ ] Sets
- [ ] Hashes
- [ ] Sorted Sets
- [ ] Streams

### Protocol

- [ ] RESP2
- [ ] RESP3
- [ ] Proper request framing
- [ ] Bulk strings
- [ ] Arrays
- [ ] Error responses

### Persistence

- [ ] RDB-style snapshots
- [ ] AOF
- [ ] Automatic snapshots
- [ ] Data recovery on startup

### Security

- [ ] ACL
- [ ] Multiple users
- [ ] User permissions
- [ ] Password hashing

### Server

- [x] TCP server
- [x] Concurrent clients
- [x] Sessions
- [ ] Connection limits
- [ ] Graceful shutdown
- [ ] Configuration system
- [ ] Logging system

### Testing & Performance

- [ ] Full unit test suite
- [ ] Integration tests
- [ ] Concurrent tests
- [ ] Performance benchmarks
- [ ] Stress testing

## Learning Goals

This is primarily an educational implementation.

The project is intended to build practical understanding of:

- TCP and socket programming
- Concurrent programming
- Mutexes and thread safety
- In-memory databases
- Hash tables
- Command parsing
- Authentication
- Session management
- Key expiration
- Persistence
- Database protocols
- Performance optimization
- CMake project organization

Features are added incrementally rather than attempting to reproduce the entire Redis implementation at once.

## Development Roadmap

```text
Basic Database
      ↓
Command System
      ↓
Authentication
      ↓
TCP Server
      ↓
Concurrent Clients
      ↓
Thread Safety
      ↓
Better Parser
      ↓
More Commands
      ↓
Testing
      ↓
Logging
      ↓
Configuration
      ↓
Persistence
      ↓
RESP Protocol
      ↓
More Redis Data Structures
      ↓
Performance Optimization
```

## Disclaimer

This project is **not Redis** and is not intended to be a production-ready replacement for Redis.

It is an independent implementation inspired by Redis, created for educational and experimental purposes.

## License

See the `LICENSE` file included in the repository.
