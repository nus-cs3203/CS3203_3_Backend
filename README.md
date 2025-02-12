# CS3203 Backend

# For Users (i.e. not Albert)

## Requirements:

- Docker

## How to Setup?
1. Install Docker and make sure that it is running
2. Build and run using Docker:
   ```bash
   docker compose build
   docker compose up
   ```

# For Developers (i.e. Albert)

## Requirements:

- Boost 1.8x
- CrowCpp v.1.0+
- Cmake 3.10+
- Mongo-cxx-driver
- mongodb-community

## Local Environment

### How to Setup?

1. Use `brew` to install `boost`, `cmake` and `mongo-cxx-driver`:
    ```bash
    brew install boost cmake mongo-cxx-driver mongodb-community
    ```
1. Change `set CMAKE_PREFIX_PATH` in `services/db_manager/src/CMakeLists.txt` to where mongo-cxx-driver is located. Use the command below to find where it is:
   ```bash
   brew --prefix mongo-cxx-driver
   ```
1. Create a directory for libraries:
   ```bash
   mkdir libs && cd libs
   ```
1. Clone the CrowCpp repository:
   ```bash
   git clone https://github.com/CrowCpp/Crow.git
   ```
1. Return to the root directory:
   ```bash
   cd ..
   ```
1. Add the following into mongod.conf:
    ```
    replication:
        replSetName: rs0
    ```
1. Run replica set:
    ```
    mongosh // connect using mongosh
    rs.initiate() // initiate replica set
    ```

### How to Build?

1. Create a build directory:
   ```bash
   mkdir build
   ```
2. Navigate to the `scripts` directory:
   ```bash
   cd scripts
   ```
3. Execute the build script:
   ```bash
   ./make.sh
   ```
4. Compile the project:
   ```bash
   ./compile.sh
   ```
5. Return to the root directory:
   ```bash
   cd ..
   ```

### How to Run?

1. Navigate to the executable directory:
   ```bash
   cd release/bin/
   ```
2. Execute the application:
   ```bash
   ./CS3203_BACKEND
   ```

# API Contract

## Service: db_manager

The API for db_manager closely follows MongoDB semantics (insert_one, insert_many, find_one, find, delete_one, delete_many, update_one, update_many). You can read this documentation to understand their specific behaviors further: [MongoDB C++ Driver Documentation](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/get-started/).

---

### **POST /insert_one**

**Request:**
```json
{
    "collection": "string",
    "document": "json"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "_id": "string" // internal id of MongoDB
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/insert_one \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "document": {
             "name": "Bob",
             "age": 30,
             "email": "bob@example.com"
         }
     }'
```

---

### **POST /insert_many**

**Request:**
```json
{
    "collection": "string",
    "documents": ["json"]
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "inserted_count": "int"
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/insert_many \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "documents": [
             {
                 "name": "Bob",
                 "age": 30,
                 "email": "bob@example.com"
             },
             {
                 "name": "Bob2",
                 "age": 30,
                 "email": "bob@example.com"
             }
         ]
     }'
```

---

### **POST /find_one**

**Request:**
```json
{
    "collection": "string",
    "filter": "json"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "document": "..."
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/find_one \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "filter": {
             "name": "Bob",
             "age": 30,
             "email": "bob@example.com"
         }
     }'
```

**How to use MongoDB ID:**
```sh
curl -X POST http://localhost:8081/find_one \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "filter": {
             "_id": {"oid": "67a2a5febaee6bb9b807d841"}
         }
     }'
```

---

### **POST /find**

**Request:**
```json
{
    "collection": "string",
    "filter": "json"
}
```

**Response:**
```json
{
    "success": "bool",
    "documents": [],
    "message": "string"
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/find \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "filter": {
             "name": "Bob",
             "age": 30,
             "email": "bob@example.com"
         }
     }'
```

---

### **POST /delete_one**

**Request:**
```json
{
    "collection": "string",
    "filter": "json"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "deleted_count": "int"
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/delete_one \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "filter": {
             "name": "Bob",
             "age": 30,
             "email": "bob@example.com"
         }
     }'
```

---

### **POST /delete_many**

**Request:**
```json
{
    "collection": "string",
    "filter": "json"
}
```

**Response:**
```json
{
    "success": "bool",
    "count": "int",
    "message": "string"
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/delete_many \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "filter": {
             "name": "Bob2"
         }
     }'
```

---

### **POST /update_one**

**Request:**
```json
{
    "collection": "string",
    "filter": "json",
    "update_document": "json"
}
```

**Response:**
```json
{
    "success": "bool",
    "matched_count": "int",
    "modified_count": "int",
    "upserted_count": "int",
    "message": "string"
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/update_one \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "filter": {
             "name": "Bob",
             "age": 30,
             "email": "bob@example.com"
         },
         "update_document": {
             "$set": {
                 "name": "Bob2",
                 "age": 30,
                 "email": "bob@example.com"
             }
         },
         "upsert": "bool"
     }'
```

---

### **POST /update_many**

**Request:**
```json
{
    "collection": "string",
    "filter": "json",
    "update_document": "json"
}
```

**Response:**
```json
{
    "success": "bool",
    "matched_count": "int",
    "modified_count": "int",
    "upserted_count": "int",
    "message": "string"
}
```

**Sample:**
```sh
curl -X POST http://localhost:8081/update_many \
     -H "Content-Type: application/json" \
     -d '{
         "collection": "new_users",
         "filter": {
             "name": "Bob"
         },
         "update_document": {
             "$set": {
                 "name": "Bob2",
                 "age": 30,
                 "email": "bob@example.com"
             }
         },
         "upsert": "bool"
     }'
```

