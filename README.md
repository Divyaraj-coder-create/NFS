# Network File System (NFS) Implementation

## Overview

This project implements a Network File System (NFS) consisting of Naming Server (NM), Storage Servers (SS), and Clients. The system facilitates file operations, replication, and efficient searching using Tries.

## Components

### Naming Server (NM)

#### Server Setup

- `main.c` in the `nm_server_client` folder handles NM functionality.
- Two threads, `listen_nm_thread` and `listen_client`, continuously listen for servers and clients.
- NM stores server entries in a global struct array.

#### File System Tree (TRIE)

- SS sends accessible files and folders list to NM, stored in a TRIE data structure.
- TRIE nodes store path characters and an `end_of_file` value.

#### Handling Server Shutdown

- "CLOSE" request from SS leads to NM closing and deleting entries from TRIE and cache.

### Storage Server (SS)

#### Server Initialization

- SS sends its accessible files and folders list to NM upon connection.
- Threads are created for listening to client and NM server requests.

#### File Operations

- Threads handle READ/WRITE operations from clients.
- Additional threads handle server shutdown.

### Clients

- Clients connect to NM and input operations continuously.

## File Operations

### Reading, Writing, and File Details

- `handle_read_write` in `main.c` executes read/write operations.
- Paths found in TRIE trigger read/write operations on the corresponding SS.

### Creating, Deleting Files/Folders

- NM and SS communicate for creation and deletion operations.
- Correct SS determined by `search_trie`, followed by necessary operations.

### Copying Files/Directories between SS

- File copy involves sending data byte by byte between SS.
- Folder copy uses recursive data transfer.
- Same SS copy uses TAR.

## Additional Features

### Multiple Clients Handling

- "f_lock" ensures exclusive write access.

### Efficient Search in NM

- Tries provide O(1) time complexity for path searches.
- LRU Caching implemented with a Linked List.

### Redundancy/Replication

- Copies of files and folders created for READ/WRITE operations.
- SS redundancy implemented.

## Assumptions

- Paths are relative to SS folder.
- Executable for SS folder is created by "gcc" in the relevant folder.
- Folder specified by `./FOLDER_NAME/` path.
- Input format of Copy differs from the specified doc.
- Redundancy is implemented only for READ/WRITE operations.

