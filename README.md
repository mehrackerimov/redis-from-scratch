# Redis From Scratch

A simple Redis-inspired key-value store written from scratch in C++.

## Current Features

- `SET` command
- `GET` command
- Key expiration with `EX`
- C++20
- CMake

## Example

```text
> SET name Mehrac
OK

> GET name
Mehrac

> SET age 18 EX 60
OK

> GET age
18