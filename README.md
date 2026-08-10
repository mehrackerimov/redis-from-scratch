# Redis From Scratch

A Redis-inspired in-memory key-value database written in C++ from scratch.

> **Status:** Work in Progress 🚧

This project is mainly built for learning and experimenting with:
- C++ networking
- TCP sockets
- Concurrency
- Thread safety
- Command parsing
- Authentication
- In-memory databases
- Key expiration

## Current Features

- In-memory key-value storage
- Thread-safe database access
- TCP server on port `6379`
- Multiple concurrent clients
- Per-client sessions
- Basic username/password authentication
- Command handler
- Command parser and tokenizer
- Key expiration
- Local console

## Supported Commands

```text
PING
SET
GET
DEL
EXISTS
EXPIRE
TTL
LOGIN
MSET
MGET
```

## Architecture

```text
Client
  ↓
TCP Server
  ↓
Session
  ↓
Command Parser
  ↓
Command Handler
  ↓
Database / UserManager
```

A command such as:

```text
SET name Mehrac EX 60
```

is tokenized, parsed into a structured command, and passed to the command handler.

## Database

Data is currently stored in memory using a hash-table-based structure.

Conceptually:

```text
key → value + optional expiration
```

The database is protected with mutexes so multiple client threads can safely access shared data.

## Authentication

Clients have their own `Session`.

A session keeps track of whether the client is authenticated and which user is logged in.

Example:

```text
LOGIN user1 password
→ OK
```

Protected commands are rejected when the client is not authenticated.

## TCP Server

The server uses Windows Winsock.

Basic flow:

```text
WSAStartup
→ socket
→ bind
→ listen
→ accept
→ recv
→ command handling
→ send
```

Each connected client is handled independently, allowing multiple clients to communicate with the server at the same time.

## Running

Build with CMake:

```bash
cmake -S . -B build
cmake --build build
```

Then run:

```text
build-and-run.bat
```

The server listens on port `6379` by default.

## Example

```text
> PING
PONG

> SET name Mehrac
OK

> GET name
Mehrac

> EXISTS name
1
```

## Goals

The main goal is to understand how a Redis-like database works internally rather than simply using an existing implementation.

The project is developed incrementally, with new features added as the underlying systems become more mature.

## License

See the `LICENSE` file.
