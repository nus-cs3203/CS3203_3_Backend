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
    "date": "Datetime",
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
    "start_date": "string",   // format: %d-%m-%Y %H:%M:%S
    "end_date": "string",     // format: %d-%m-%Y %H:%M:%S
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
        "start_date": "1-1-2024 00:00:00",
        "end_date": "31-12-2024 23:59:59",
        "group_by_field": "category"
    }'
```

**Sample Response:**
```json
{
  "success": true,
  "message": "Analytics result retrieved.",
  "result": {
    "politics": {
      "count": 1,
      "avg_sentiment": -0.5
    },
    "technology": {
      "count": 3,
      "avg_sentiment": 0.733333
    },
    "environment": {
      "count": 1,
      "avg_sentiment": -0.3
    },
    "business": {
      "count": 1,
      "avg_sentiment": 0.7
    },
    "finance": {
      "count": 1,
      "avg_sentiment": 0.6
    },
    "entertainment": {
      "count": 1,
      "avg_sentiment": -0.2
    },
    "sports": {
      "count": 1,
      "avg_sentiment": 1.0
    },
    "health": {
      "count": 1,
      "avg_sentiment": 1.0
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
    "start_date": "string",       // format: %d-%m-%Y %H:%M:%S
    "end_date": "string",         // format: %d-%m-%Y %H:%M:%S
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
        "start_date": "1-1-2024 00:00:00",
        "end_date": "31-12-2024 23:59:59",
        "group_by_field": "category",
        "time_bucket_regex": "%m-%Y"
    }'
```

**Sample Response:**
```json
{
  "success": true,
  "message": "Analytics result retrieved.",
  "result": {
    "02-2024": {
      "health": {
        "count": 1,
        "avg_sentiment": 1
      },
      "politics": {
        "count": 1,
        "avg_sentiment": -0.5
      },
      "technology": {
        "count": 3,
        "avg_sentiment": 0.73333333333333339
      },
      "entertainment": {
        "count": 1,
        "avg_sentiment": -0.2
      },
      "finance": {
        "count": 1,
        "avg_sentiment": 0.6
      },
      "business": {
        "count": 1,
        "avg_sentiment": 0.7
      },
      "sports": {
        "count": 1,
        "avg_sentiment": 1
      },
      "environment": {
        "count": 1,
        "avg_sentiment": -0.3
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
    "start_date": "string",  // format: %d-%m-%Y %H:%M:%S
    "end_date": "string",    // format: %d-%m-%Y %H:%M:%S
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
        "start_date": "1-1-2024 00:00:00",
        "end_date": "31-12-2024 23:59:59",
        "bucket_size": 0.5
    }'
```

**Sample Response:**
```json
{
  "success": true,
  "message": "Analytics result retrieved.",
  "result": [
    {
      "count": 3,
      "right_bound_exclusive": 0,
      "left_bound_inclusive": -0.5
    },
    {
      "left_bound_inclusive": 0.5,
      "right_bound_exclusive": 1,
      "count": 5
    },
    {
      "count": 2,
      "right_bound_exclusive": 1.5,
      "left_bound_inclusive": 1.0
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
            "date": "DD-MM-YYYY",
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
  "success": true,
  "message": "Post(s) retrieved.",
  "posts": [
    {
      "sentiment": 1,
      "date": "12-02-2024 18:15:00",
      "category": "sports",
      "title": "Sports Team Wins Championship",
      "source": "SportsCenter",
      "_id": {
        "$oid": "67acd782bde3d73ac0147417"
      }
    },
    {
      "_id": {
        "$oid": "67acd782bde3d73ac0147419"
      },
      "source": "GreenTech",
      "title": "Advancements in Renewable Energy",
      "category": "technology",
      "date": "14-02-2024 11:20:00",
      "sentiment": 0.9
    },
    {
      "sentiment": 0.8,
      "date": "10-02-2024 12:00:00",
      "category": "technology",
      "title": "Tech Breakthrough in AI",
      "source": "TechNews",
      "_id": {
        "$oid": "67acd782bde3d73ac0147415"
      }
    },
    {
      "sentiment": 0.6,
      "date": "11-02-2024 14:30:00",
      "category": "finance",
      "title": "Stock Market Sees Record Highs",
      "source": "FinanceDaily",
      "_id": {
        "$oid": "67acd782bde3d73ac0147416"
      }
    },
    {
      "_id": {
        "$oid": "67acd782bde3d73ac0147418"
      },
      "source": "MovieCritic",
      "title": "New Movie Release Receives Mixed Reviews",
      "category": "entertainment",
      "date": "13-02-2024 09:45:00",
      "sentiment": -0.2
    }
  ]
}
```

---