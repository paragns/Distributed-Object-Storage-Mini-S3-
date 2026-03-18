# Mini S3 — Distributed Object Storage in C++

A ground-up implementation of an S3-style object storage system, built in C++ as a learning project focused on **system design**, **storage internals**, and **distributed systems concepts**.

---

## What This Project Is

Amazon S3 is one of the most widely used storage services in the world. At its core it solves a deceptively simple problem: store a blob of bytes, give it a name, retrieve it later. This project rebuilds that idea from scratch — starting with a single-node HTTP server and growing it into a distributed storage cluster with replication.

The goal is not to replace S3. The goal is to understand how it works.

---

## Architecture

The system is built around a clean separation between **metadata** and **data**.

```
Client
  ↓
HTTP API
  ↓
Object Controller
  ↓
Metadata Manager ──→ storage/metadata/objects.json
  ↓
Storage Engine   ──→ storage/data/<bucket>/<object>
  ↓
Filesystem
```

**Metadata** tracks everything _about_ an object: its name, bucket, size, timestamp, and chunk locations.
**Data** is the raw bytes of the object, written directly to the filesystem.

This separation is what makes features like chunking, replication, and distributed routing possible.

---

## Project Structure

```
mini_s3/
├── src/                  # .cpp implementation files
├── include/              # .h header files
├── storage/
│   ├── data/             # Raw object data (per bucket/object)
│   └── metadata/         # JSON metadata index
├── third_party/          # vendored dependencies (cpp-httplib)
├── CMakeLists.txt
├── README.md
└── roadmap.md
```

---

## Build

**Requirements:** CMake 3.14+, a C++17 compiler (GCC / Clang / MSVC)

```bash
mkdir build && cd build
cmake ..
cmake --build .
./mini_s3
```

---

## Roadmap

The project is built in two phases across 10 chapters.

### Phase 1 — Mini S3 (Single Node)

| Chapter | Topic | Key Concepts |
|---------|-------|--------------|
| 1 | HTTP Server Setup | REST API, routing, request/response cycle |
| 2 | Bucket Management | Namespace design, directory mapping |
| 3 | Object Upload (PUT) | Object storage model, filesystem writes |
| 4 | Object Retrieval (GET) | Request handling, response streaming |
| 5 | Object Deletion | Storage lifecycle, filesystem cleanup |
| 6 | Metadata Manager | Metadata indexing, data/metadata separation |
| 7 | Large Object Chunking | Chunk-based storage, reassembly |

### Phase 2 — Distributed Storage

| Chapter | Topic | Key Concepts |
|---------|-------|--------------|
| 8 | Storage Node Abstraction | Distributed architecture, node design |
| 9 | Replication | Durability, replication factor, consistency |
| 10 | Request Routing | Coordinator pattern, cluster coordination |

See [roadmap.md](roadmap.md) for full details on each chapter.

---

## API Reference

### Buckets
| Method | Endpoint | Description |
|--------|----------|-------------|
| `PUT` | `/bucket/{name}` | Create a bucket |
| `DELETE` | `/bucket/{name}` | Delete a bucket |
| `GET` | `/buckets` | List all buckets |

### Objects
| Method | Endpoint | Description |
|--------|----------|-------------|
| `PUT` | `/{bucket}/{object}` | Upload an object |
| `GET` | `/{bucket}/{object}` | Download an object |
| `DELETE` | `/{bucket}/{object}` | Delete an object |

### Health
| Method | Endpoint | Description |
|--------|----------|-------------|
| `GET` | `/health` | Server health check |

---

## Learning Outcomes

By the end of this project you will understand:

- How REST API servers are structured and implemented in C++
- How object storage systems separate metadata from raw data
- How large objects are split into chunks and reassembled
- How distributed storage nodes communicate and coordinate
- How replication improves durability and what trade-offs it introduces

---

## Dependencies

- [cpp-httplib](https://github.com/yhirose/cpp-httplib) — single-header HTTP server/client library
- [nlohmann/json](https://github.com/nlohmann/json) — single-header JSON library (added in Chapter 6)
