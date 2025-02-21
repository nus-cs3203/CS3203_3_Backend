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
3. Initialize the database (run /services/db/initializer/script.ipynb)

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

## Service: **initializer**

How to initialize the database?
1. Run docker services
```bash
    docker compose up
```
2. Run the script in services/db/initializer/script.ipynb

---

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

## Service: **pages_admin_analytics**

This service provides analytics for the `complaints` collection.

Note: Sample responses are obtained by using dummy data defined in [here](./services/pages/admin/analytics/dummy_data/README.MD) (with the assumption of no other data under `complaints` collection).

---

### **Collection: `complaints`**

Refer to Schema Document for collection definition.

---

### **POST /get_complaints_grouped_by_field**

- **Purpose**: Group complaints based on a specified field (e.g., `category`), returning `count` and `avg_sentiment` that group.
- **`group_by_field` explanation**: This is the field in the `complaints` collection used for grouping e.g.  `"category"`, `"source"`.

**Request:**
```json
{
    "start_date": "string",   // format: dd-mm-YYYY HH:MM:SS
    "end_date": "string",     // format: dd-mm-YYYY HH:MM:SS
    "group_by_field": "string"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "result": {
        "group_value_1": {
            "count": "int",
            "avg_sentiment": "float"
        },
        "group_value_2": {
            "count": "int",
            "avg_sentiment": "float"
        },
        ...
    }
}
```

**Sample Request:**
```sh
    curl -X POST "http://localhost:8082/get_complaints_grouped_by_field" \
    -H "Content-Type: application/json" \
    -d '{
        "start_date": "01-01-2010 00:00:00",
        "end_date": "31-12-2010 23:59:59",
        "group_by_field": "category"
    }'
```

**Sample Response:**
```json
{
    "message": "Analytics result retrieved.",
    "success": true,
    "result": {
        "Politics": {
            "count": 1,
            "avg_sentiment": -0.198254
        },
        "Recreation": {
            "count": 2,
            "avg_sentiment": -0.077424
        },
        "Financial": {
            "count": 1,
            "avg_sentiment": -0.737511
        },
        "Healthcare": {
            "count": 2,
            "avg_sentiment": 0.380603
        },
        "Retail": {
            "count": 1,
            "avg_sentiment": -0.080992
        },
        "Social Services": {
            "count": 1,
            "avg_sentiment": -0.436532
        },
        "Public Safety": {
            "count": 1,
            "avg_sentiment": 0.26788
        },
        "Environmental": {
            "count": 2,
            "avg_sentiment": 0.373306
        },
        "Transportation": {
            "count": 1,
            "avg_sentiment": 0.270747
        },
        "Employment": {
            "count": 1,
            "avg_sentiment": 0.696234
        },
        "Housing": {
            "count": 3,
            "avg_sentiment": -0.042444
        }
    }
}
```

---

### **POST /get_complaints_grouped_by_field_over_time**

- **Purpose**: Similar to `get_complaints_grouped_by_field`, but also groups the results by a time interval. Time interval used is based on `"%m-%Y"` regex of date field.
- Note: for time interval with no data for that category, default value of 0 is used for count and avg_sentiment.

**Request:**
```json
{
    "start_date": "string",       // format: dd-mm-YYYY HH:MM:SS
    "end_date": "string",         // format: dd-mm-YYYY HH:MM:SS
    "group_by_field": "string"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "result": [
        {
            "date": "%m-%Y",
            "data": {
                "field1": {
                    "count": "int",
                    "avg_sentiment": "double"
                },
                "field2": {
                    "count": "int",
                    "avg_sentiment": "double"
                },
                ...
            }
        },
        {
            "date": "%m-%Y",
            "data": {
                "field1": {
                    "count": "int",
                    "avg_sentiment": "double"
                },
                "field2": {
                    "count": "int",
                    "avg_sentiment": "double"
                },
                ...
            }
        }
        ...
    ]
}
```

**Sample Request:**
```sh
    curl -X POST "http://localhost:8082/get_complaints_grouped_by_field_over_time" \
    -H "Content-Type: application/json" \
    -d '{
        "start_date": "01-01-2010 00:00:00",
        "end_date": "31-03-2010 23:59:59",
        "group_by_field": "category"
    }'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Analytics result retrieved.",
    "result": [
        {
            "date": "01-2010",
            "data": {
                "Financial": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Infrastructure": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Healthcare": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Food Services": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Social Services": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Retail": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Housing": {
                    "count": 1,
                    "avg_sentiment": -0.17583705399999999
                },
                "Technology": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Education": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Public Safety": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Transportation": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Environmental": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Employment": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Noise": {
                    "count": 0,
                    "avg_sentiment": 0
                },
                "Others": {
                    "count": 0,
                    "avg_sentiment": 0
                }
            }
        },
        ...
    ]
}
```

---

### **POST /get_complaints_grouped_by_sentiment_value**

- **Purpose**: Group complaints into "buckets" based on their sentiment value. For example, if the `bucket_size` is 0.5, it will group sentiments in the following ranges: `[-1, -0.5), [-0.5, 0), [0, 0.5), [0.5, 1)`.
- **`bucket_size` explanation**: A numerical interval for grouping sentiment values, e.g. `0.5` creates buckets of width 0.5 each.

**Request:**
```json
{
    "start_date": "string",  // format: dd-mm-YYYY HH:MM:SS
    "end_date": "string",    // format: dd-mm-YYYY HH:MM:SS
    "bucket_size": "float"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "result": [
        {
            "left_bound_inclusive": "float",
            "right_bound_exclusive": "float",
            "count": "int"
        },
        ...
    ]
}
```

**Sample Request:**
```sh
    curl -X POST "http://localhost:8082/get_complaints_grouped_by_sentiment_value" \
    -H "Content-Type: application/json" \
    -d '{
        "start_date": "01-01-2010 00:00:00",
        "end_date": "31-12-2010 23:59:59",
        "bucket_size": 0.5
    }'
```

**Sample Response:**
```json
{
    "message": "Analytics result retrieved.",
    "success": true,
    "result": [
        {
            "right_bound_exclusive": -0.5,
            "count": 2,
            "left_bound_inclusive": -1.0
        },
        {
            "left_bound_inclusive": -0.5,
            "count": 6,
            "right_bound_exclusive": 0
        },
        {
            "right_bound_exclusive": 0.5,
            "count": 4,
            "left_bound_inclusive": 0.0
        },
        {
            "right_bound_exclusive": 1,
            "count": 4,
            "left_bound_inclusive": 0.5
        }
    ]
}
```

---

### **POST /get_complaints_sorted_by_fields**

- **Purpose**: Retrieve complaints sorted by one or more specified fields.

**Request:**
```json
{
    "keys": ["string"],
    "ascending_orders": ["bool"],
    "limit": "int"
}
```
- **`keys`**: An array of field names to sort by (e.g., `["sentiment", "date"]`).
- **`ascending_orders`**: A corresponding array of booleans indicating ascending (`true`) or descending (`false`) for each key.
- **`limit`**: The maximum number of complaints to return.

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "complaints": [
        {
            "title": "string",
            "source": "string",
            "category": "string",
            "date": "dd-mm-YYYY HH:MM:SS",
            "sentiment": "float",
            "_id": {"$oid": "string"}
        },
        ...
    ]
}
```

**Sample Request:**
```sh
    curl -X POST "http://localhost:8082/get_complaints_sorted_by_fields" \
    -H "Content-Type: application/json" \
    -d '{
        "keys": ["sentiment"],
        "ascending_orders": [false],
        "limit": 5
    }'
```

**Sample Response:**
```json
{
    "message": "Complaint(s) retrieved.",
    "success": true,
    "complaints": [
        {
            "date": "17-11-2009 00:00:00",
            "source": "Reddit",
            "category": "Financial",
            "id": "de456",
            "url": "https://example.com/",
            "description": "This is a description for complaint #4",
            "title": "The Harold Lloyd Method of Mass Transit Advertising",
            "sentiment": 0.837126,
            "_id": {
                "$oid": "67aeded9b5bc2d9932255caa"
            }
        },
        {
            "_id": {
                "$oid": "67aeded9b5bc2d9932255cab"
            },
            "sentiment": -0.175837,
            "title": "Hello la, I'm in Singapore for the week and want to go dancing, was wondering Zirca or Zouk, and which days?",
            "description": "This is a description for complaint #5",
            "url": "https://example.com/",
            "id": "ef567",
            "category": "Housing",
            "source": "Reddit",
            "date": "12-01-2010 00:00:00"
        },
        {
            "date": "22-10-2008 00:00:00",
            "source": "Reddit",
            "category": "Transportation",
            "id": "bc234",
            "url": "https://example.com/",
            "description": "This is a description for complaint #2",
            "title": "Good article on the Singapore economy: 'Review strategy, take crisis as opportunity '",
            "sentiment": -0.287196,
            "_id": {
                "$oid": "67aeded9b5bc2d9932255ca8"
            }
        },
        {
            "date": "30-09-2008 00:00:00",
            "source": "Reddit",
            "category": "Politics",
            "id": "ab123",
            "url": "https://example.com/",
            "description": "This is a description for complaint #1",
            "title": "RIP JB Jeyaretnam. Possibly Singapore's greatest citizen.",
            "sentiment": 0.477568,
            "_id": {
                "$oid": "67aeded9b5bc2d9932255ca7"
            }
        },
        {
            "_id": {
                "$oid": "67aeded9b5bc2d9932255ca9"
            },
            "sentiment": -0.475928,
            "title": "High-living Singaporean monk faces jail for fraud.",
            "description": "This is a description for complaint #3",
            "url": "https://example.com/",
            "id": "cd345",
            "category": "Infrastructure",
            "source": "Reddit",
            "date": "09-10-2009 00:00:00"
        }
    ]
}
```

---

## Service: **pages_admin_management**

This service provides various endpoints to manage categories and complaints for administrative purposes.

---

### **Collection: `categories`**

Refer to Schema Document for collection definition.

#### **POST /categories/get_all**

**Request:**
```json
{
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "documents": []
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/categories/get_all" \
     -H "Content-Type: application/json"
```

---

#### **POST /categories/get_by_oid**

**Request:**
```json
{
    "oid": "string"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "document": {} // Category document or null
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/categories/get_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67b458405b0f29f2e7c47ce8"
     }'
```

---

#### **POST /categories/insert_one**

**Request:**
```json
{
    "document": {}
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "_id": "string"  // internal id of MongoDB
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/categories/insert_one \
     -H "Content-Type: application/json" \
     -d '{
         "document": {
             "name": "Others",
             "color": "#FFFFFF"
         }
     }'
```

---

#### **POST /categories/delete_by_oid**

**Request:**
```json
{
    "oid": "string"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "delete_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/categories/delete_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67b458405b0f29f2e7c47ce8"
     }'
```

---

#### **POST /categories/update_by_oid**

**Request:**
```json
{
    "oid": "string",
    "update_document": {
        "$set": {
            "field1": "...",
            "field2": "...",
            ...
        }
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "matched_count": "int",
    "modified_count": "int",
    "upserted_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/categories/update_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67b735dfc9b96625a2f1385c",
         "update_document": {
             "$set": {
                 "color": "#34495A"
             }
         }
     }'
```

---

### **Collection: `complaints`**

Refer to Schema Document for collection definition.

#### **POST /complaints/get_by_oid**

**Request:**
```json
{
    "oid": "string"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "document": {} // Complaint document or null
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/complaints/get_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67b458405b0f29f2e7c47ce8"
     }'
```

---

#### **POST /complaints/delete_by_oid**

**Request:**
```json
{
    "oid": "string"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "delete_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/complaints/delete_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67b458405b0f29f2e7c47ce8"
     }'
```

---

#### **POST /complaints/search**

**Text field explanation**:  
Searches for the **existence** of a word in the `title` or `description` of a complaint (case-insensitive). For example, `"this"` will match the word `"this"` but not just the letter `"t"`.

**Pagination explanation**:  
- Filtered results are sorted by time (decreasing order).  
- `total_count` of filtered results is returned.  
- This endpoint will return the slice of documents at index `[page_size * page_number, page_size * (page_number + 1) - 1]` (1-based indexing).

**Request:**
```json
{
    "collection": "string",
    "filter": {
        "$text": {
            "$search": "keyword"
        },
        "field1": "field1 value",
        "field2": "field2 value"
    },
    "page_size": "int",
    "page_number": "int"
}
```

**Response:**
```json
{
    "success": "bool",
    "documents": [],
    "message": "string",
    "total_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/complaints/search \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {
             "$text": {
                 "$search": "new"
             },
             "category": "Technology"
         },
         "page_size": 25,
         "page_number": 1
     }'
```

---

#### **POST /complaints/delete_many_by_oids**

**Request:**
```json
{
    "oids": ["string", "string", ...]
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "delete_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/complaints/delete_many_by_oids" \
     -H "Content-Type: application/json" \
     -d '{
         "oids": ["67b458405b0f29f2e7c47ce9", "67b458405b0f29f2e7c47ce7"]
     }'
```

---

#### **POST /complaints/update_by_oid**

> **Note**: For the field `date`, it must be a string of format `dd-mm-YYYY HH:MM:SS`.

**Request:**
```json
{
    "oid": "string",
    "update_document": {
        "$set": {
            "field1": "...",
            "field2": "...",
            ...
        }
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "matched_count": "int",
    "modified_count": "int",
    "upserted_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/complaints/update_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67b744f98c61c92b7195e96d",
         "update_document": {
             "$set": {
                 "title": "new title",
                 "date": "01-01-2025 00:00:00"
             }
         }
     }'
```

---