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

# API Contract

## Service: **pages_admin_analytics**

This service provides analytics for the `posts` collection.

Note: Sample responses are obtained by using dummy data defined in [here](./services/pages/admin/analytics/dummy_data/README.MD) (with the assumption of no other data under `posts` collection).

---

### **Collection: `posts`**

Each document in the `posts` collection has the following structure:

```json
{
    "_id": {
        "oid": "string", // mongodb internal id
    },
    "title": "string",
    "source": "string",
    "category": "string",
    "date": "mongodb datetime",
    "sentiment": "float"
}
```

---

### **POST /get_posts_grouped_by_field**

- **Purpose**: Group posts based on a specified field (e.g., `category`), returning `count` and `avg_sentiment` that group.
- **`group_by_field` explanation**: This is the field in the `posts` collection used for grouping e.g.  `"category"`, `"source"`.

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
    curl -X POST "http://localhost:8082/get_posts_grouped_by_field" \
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

### **POST /get_posts_grouped_by_field_over_time**

- **Purpose**: Similar to `get_posts_grouped_by_field`, but also groups the results by a time interval (based on the `time_bucket_regex`).
- **`time_bucket_regex` explanation**: This is a date format string specifying how to bucket or group posts by their date. For example, `"%m-%Y"` groups posts by month-year (e.g., `02-2024`) and `"%Y"` groups posts by year (e.g., `2024`).

**Request:**
```json
{
    "start_date": "string",       // format: dd-mm-YYYY HH:MM:SS
    "end_date": "string",         // format: dd-mm-YYYY HH:MM:SS
    "time_bucket_regex": "string", // e.g. "%m-%Y"
    "group_by_field": "string"
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "result": {
        "time_bucket_value_1": {
            "group_value_1": {
                "count": "int",
                "avg_sentiment": "float"
            },
            "group_value_2": {
                "count": "int",
                "avg_sentiment": "float"
            },
            ...
        },
        "time_bucket_value_2": {
            ...
        }
    }
}
```

**Sample Request:**
```sh
    curl -X POST "http://localhost:8082/get_posts_grouped_by_field_over_time" \
    -H "Content-Type: application/json" \
    -d '{
        "start_date": "01-01-2010 00:00:00",
        "end_date": "31-12-2010 23:59:59",
        "group_by_field": "category",
        "time_bucket_regex": "%m-%Y"
    }'
```

**Sample Response:**
```json
{
    "message": "Analytics result retrieved.",
    "success": true,
    "result": {
        "05-2010": {
            "Retail": {
                "count": 1,
                "avg_sentiment": -0.0809918000000000026794566565513378009
            },
            "Healthcare": {
                "count": 2,
                "avg_sentiment": 0.38060264799999998786006472073495388
            },
            "Environmental": {
                "count": 1,
                "avg_sentiment": 0.500866342999999991647541719430591911
            },
            "Housing": {
                "count": 1,
                "avg_sentiment": 0.264428435999999988936792760796379298
            }
        },
        "03-2010": {
            "Public Safety": {
                "count": 1,
                "avg_sentiment": 0.267879894999999978999483118968782946
            }
        },
        "06-2010": {
            "Social Services": {
                "count": 1,
                "avg_sentiment": -0.436532213999999973985666201770072803
            },
            "Environmental": {
                "count": 1,
                "avg_sentiment": 0.245745574999999993881516502369777299
            },
            "Transportation": {
                "count": 1,
                "avg_sentiment": 0.270746941999999990446923447962035425
            }
        },
        "08-2010": {
            "Financial": {
                "count": 1,
                "avg_sentiment": -0.737510960000000048708557187637779862
            },
            "Politics": {
                "count": 1,
                "avg_sentiment": -0.198254379000000008348436608685005922
            },
            "Housing": {
                "count": 1,
                "avg_sentiment": -0.215923077999999990517210335383424535
            }
        },
        "07-2010": {
            "Recreation": {
                "count": 2,
                "avg_sentiment": -0.0774242129999999639800023487623548135
            }
        },
        "09-2010": {
            "Employment": {
                "count": 1,
                "avg_sentiment": 0.696234156000000048436504584969952703
            }
        },
        "01-2010": {
            "Housing": {
                "count": 1,
                "avg_sentiment": -0.175837053999999992637270906925550662
            }
        }
    }
}
```

---

### **POST /get_posts_grouped_by_sentiment_value**

- **Purpose**: Group posts into "buckets" based on their sentiment value. For example, if the `bucket_size` is 0.5, it will group sentiments in the following ranges: `[-1, -0.5), [-0.5, 0), [0, 0.5), [0.5, 1)`.
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
    curl -X POST "http://localhost:8082/get_posts_grouped_by_sentiment_value" \
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

### **POST /get_posts_sorted_by_fields**

- **Purpose**: Retrieve posts sorted by one or more specified fields.

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
- **`limit`**: The maximum number of posts to return.

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "posts": [
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
    curl -X POST "http://localhost:8082/get_posts_sorted_by_fields" \
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
    "message": "Post(s) retrieved.",
    "success": true,
    "posts": [
        {
            "date": "17-11-2009 00:00:00",
            "source": "reddit",
            "category": "Financial",
            "title": "The Harold Lloyd Method of Mass Transit Advertising",
            "sentiment": 0.837126,
            "_id": {
                "$oid": "67ae0af686ec6a1996a96074"
            }
        },
        {
            "_id": {
                "$oid": "67ae0af686ec6a1996a96075"
            },
            "sentiment": -0.175837,
            "title": "Hello la, I'm in Singapore for the week and want to go dancing, was wondering Zirca or Zouk, and which days?",
            "category": "Housing",
            "source": "reddit",
            "date": "12-01-2010 00:00:00"
        },
        {
            "date": "22-10-2008 00:00:00",
            "source": "reddit",
            "category": "Transportation",
            "title": "Good article on the Singapore economy: 'Review strategy, take crisis as opportunity '",
            "sentiment": -0.287196,
            "_id": {
                "$oid": "67ae0af686ec6a1996a96072"
            }
        },
        {
            "date": "30-09-2008 00:00:00",
            "source": "reddit",
            "category": "Politics",
            "title": "RIP JB Jeyaretnam. Possibly Singapore's greatest citizen.",
            "sentiment": 0.477568,
            "_id": {
                "$oid": "67ae0af686ec6a1996a96071"
            }
        },
        {
            "_id": {
                "$oid": "67ae0af686ec6a1996a96073"
            },
            "sentiment": -0.475928,
            "title": "High-living Singaporean monk faces jail for fraud.",
            "category": "Infrastructure",
            "source": "reddit",
            "date": "09-10-2009 00:00:00"
        }
    ]
}
```

---