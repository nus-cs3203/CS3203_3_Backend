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

## Service: **analytics**

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

- **Purpose**: Retrieve complaints sorted by one or more specified fields, with an optional filter for narrowing down results.
- **Filter logic**: Accepts a `filter` field identical to the one in [`/get_complaints_statistics`](#post-get_complaints_statistics). All filter fields are optional.

**Request:**
```json
{
    "keys": ["string"],
    "ascending_orders": ["bool"],
    "limit": "int",
    "filter": {
        "keyword": "string",        // (optional) Title or selftext contains this keyword (case-insensitive)
        "source": "string",         // (optional) Source of complaint, e.g. "Reddit"
        "category": "string",       // (optional) Category of complaint, e.g. "Housing"
        "start_date": "string",     // (optional) format: dd-mm-YYYY HH:MM:SS
        "end_date": "string",       // (optional) format: dd-mm-YYYY HH:MM:SS
        "min_sentiment": "double",  // (optional)
        "max_sentiment": "double"   // (optional)
    }
}
```
- **`keys`**: An array of field names to sort by (e.g., `["sentiment", "date"]`).
- **`ascending_orders`**: A corresponding array of booleans indicating ascending (`true`) or descending (`false`) for each key.
- **`limit`**: The maximum number of complaints to return.
- **`filter`** (optional): Provides optional filtering criteria. If omitted, no filtering is applied.

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
            "_id": {
                "$oid": "string"
            }
        },
        ...
    ]
}
```

**Sample Request (no filter):**
```sh
curl -X POST "http://localhost:8082/get_complaints_sorted_by_fields" \
-H "Content-Type: application/json" \
-d '{
    "keys": ["sentiment"],
    "ascending_orders": [false],
    "limit": 5
}'
```

**Sample Request (with filter):**
```sh
curl -X POST "http://localhost:8082/get_complaints_sorted_by_fields" \
-H "Content-Type: application/json" \
-d '{
    "keys": ["sentiment"],
    "ascending_orders": [false],
    "limit": 5,
    "filter": {
        "category": "Housing",
        "start_date": "01-01-2023 00:00:00",
        "end_date": "01-02-2023 23:59:59"
    }
}'
```

---

### **POST /get_category_analytics_by_name**

- **Purpose**: Retrieve analytics for a given category name, returning various metrics such as current score, forecasted score, sentiment labels, key concerns, and more.

**Request:**
```json
{
    "name": "string"
}
```

**Response:**
```json
{
    "message": "string",
    "success": "bool",
    "document": {
        "_id": {
            "$oid": "string"
        },
        "name": "string",
        "suggestions": ["string", ...],
        "keywords_per_category": ["string", ...],
        "summary": "string",
        "forecasted_score": "float",
        "current_score": "float",
        "current_label": "string",
        "key_concerns": ["string", ...],
        "forecasted_label": "string"
    }
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8082/get_category_analytics_by_name" \
-H "Content-Type: application/json" \
-d '{
    "name": "Housing"
}'
```

**Sample Response:**
```json
{
    "message": "Retrieved analytics successfully",
    "success": true,
    "document": {
        "_id": {
            "$oid": "67c809fd56d1c1ad9a72939e"
        },
        "name": "Transportation",
        "suggestions": [
            "Reduce congestion",
            "Improve public transport",
            "Subsidize fares"
        ],
        "keywords_per_category": [
            "cars",
            "pollution",
            "MRT"
        ],
        "summary": "Concerns about transportation persist among citizens.",
        "forecasted_score": 0.5398,
        "current_score": 0.5714,
        "current_label": "positive",
        "key_concerns": [
            "Poor infrastructure"
        ],
        "forecasted_label": "positive"
    }
}
```

---

### **POST /get_complaints_statistics**

- **Purpose**: Retrieve the total count of complaints and average sentiment, based on optional filters.

**Request:**
```json
{
    "filter": {
        "keyword": "string",        // (optional) Title or selftext contains this keyword (case-insensitive)
        "source": "string",         // (optional) Source of complaint, e.g. "Reddit"
        "category": "string",       // (optional) Category of complaint, e.g. "Housing"
        "start_date": "string",     // (optional) format: dd-mm-YYYY HH:MM:SS
        "end_date": "string",       // (optional) format: dd-mm-YYYY HH:MM:SS
        "min_sentiment": "double",  // (optional)
        "max_sentiment": "double"   // (optional)
    }
}
```
> **Note**: The `filter` field itself is optional, and any field inside `filter` is also optional. For example, you can provide an empty filter like `"filter": {}`.

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "result": {
        "count": "int",
        "avg_sentiment": "float"
    }
}
```

**Sample Requests:**
```sh
curl -X POST "http://localhost:8082/get_complaints_statistics" \
-H "Content-Type: application/json" \
-d '{}'
```
```sh
curl -X POST "http://localhost:8082/get_complaints_statistics" \
-H "Content-Type: application/json" \
-d '{
    "filter": {
        "category": "Housing"
    }
}'
```
```sh
curl -X POST "http://localhost:8082/get_complaints_statistics" \
-H "Content-Type: application/json" \
-d '{
    "filter": {
        "category": "Housing",
        "start_date": "01-01-2023 00:00:00",
        "end_date":  "02-01-2023 00:00:00"
    }
}'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Analytics result retrieved.",
    "result": {
        "count": 1,
        "avg_sentiment": -0.815503
    }
}
```

---

### **POST /get_complaints_statistics_over_time**

- **Purpose**: Retrieve count and average sentiment for complaints over a time range, optionally filtered. Results are grouped by `"%m-%Y"` (month-year) buckets.

**Request:**
```json
{
    "start_date": "string",   // format: dd-mm-YYYY HH:MM:SS
    "end_date": "string",     // format: dd-mm-YYYY HH:MM:SS
    "filter": {
        "keyword": "string",        // (optional) 
        "source": "string",         // (optional)
        "category": "string",       // (optional)
        "start_date": "string",     // (optional) format: dd-mm-YYYY HH:MM:SS
        "end_date": "string",       // (optional) format: dd-mm-YYYY HH:MM:SS
        "min_sentiment": "double",  // (optional)
        "max_sentiment": "double"   // (optional)
    }
}
```
> **Note**: Fields in `filter` are optional. If omitted, no filtering is applied to that field.

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "result": [
        {
            "date": "string",  // month-year, e.g. "1-2023"
            "data": {
                "count": "int",
                "avg_sentiment": "float"
            }
        },
        ...
    ]
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8082/get_complaints_statistics_over_time" \
-H "Content-Type: application/json" \
-d '{
    "start_date": "01-01-2023 00:00:00",
    "end_date": "01-03-2023 00:00:00"
}'
```
```sh
curl -X POST "http://localhost:8082/get_complaints_statistics_over_time" \
-H "Content-Type: application/json" \
-d '{
    "start_date": "01-01-2023 00:00:00",
    "end_date": "01-03-2023 00:00:00",
    "filter": {
        "category": "Housing"
    }
}'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Analytics result retrieved.",
    "result": [
        {
            "data": {
                "avg_sentiment": 0,
                "count": 0
            },
            "date": "1-2023"
        },
        {
            "date": "2-2023",
            "data": {
                "count": 0,
                "avg_sentiment": 0
            }
        },
        {
            "data": {
                "avg_sentiment": 0,
                "count": 0
            },
            "date": "3-2023"
        }
    ]
}
```

---

## Service: **management**

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

#### **POST /complaints/get_by_daterange**

**Request:**
```json
{
    "start_date": "string", // dd-mm-YYYY HH:MM:SS
    "end_date": "string" // dd-mm-YYYY HH:MM:SS
}
```

**Response:**
```json
{
    "posts": "[]post" // Check collection: posts schema for field details
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/complaints/get_by_daterange" \
     -H "Content-Type: application/json" \
     -d '{
        "start_date": "01-01-2023 00:00:00",
        "end_date": "01-01-2023 23:59:59"
     }'
```

---

### **Collection: `posts`**

Refer to Schema Document for collection definition.

#### **POST /posts/get_by_daterange**

**Request:**
```json
{
    "start_date": "string", // dd-mm-YYYY HH:MM:SS
    "end_date": "string" // dd-mm-YYYY HH:MM:SS
}
```

**Response:**
```json
{
    "complaints": "[]complaint" // Check collection: complaint schema for field details
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/posts/get_by_daterange" \
     -H "Content-Type: application/json" \
     -d '{
        "start_date": "01-01-2022 00:00:00",
        "end_date": "01-01-2022 23:59:59"
     }'
```

---

## Service: **user**

This service provides user management endpoints, including account creation (`signup`), authentication (`login`), and profile retrieval.

---

### **POST /signup**

- **Purpose**: Create a new user account.

**Request:**
```json
{
    "name": "string",
    "email": "string",
    "password": "string"
}
```

**Response (Success):**
```json
{
    "message": "Account created successfully.",
    "success": true
}
```

**Response (Failure, HTTP Code 409)**:
```json
{
    "message": "Email already in use.",
    "success": false
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8085/signup" \
     -H "Content-Type: application/json" \
     -d '{
         "name": "test",
         "email": "test",
         "password": "test"
     }'
```

**Sample Response (Success):**
```json
{
    "message": "Account created successfully.",
    "success": true
}
```

---

### **POST /login**

- **Purpose**: Authenticate an existing user with `email` and `password`.

**Request:**
```json
{
    "email": "string",
    "password": "string"
}
```

**Response:**
```json
{
    "message": "Login successful",
    "success": true,
    "jwt": "string",
    "oid": "string"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8085/login" \
     -H "Content-Type: application/json" \
     -d '{
         "email": "test",
         "password": "test"
     }'
```

**Sample Response:**
```json
{
    "message": "Login successful",
    "success": true,
    "jwt": "eyJhbGciOiJIUzI1Ni...",
    "oid": "67cc41c912d9edad01064ac4"
}
```

---

### **POST /get_profile_by_oid**

- **Purpose**: Retrieve a user's profile by their unique ID (`oid`).

**Request:**
```json
{
    "oid": "string"
}
```

**Response:**
```json
{
    "success": true,
    "message": "Account retrieved successfully",
    "profile": {
        "_id": {
            "$oid": "string"
        },
        "name": "string",
        "role": "string",
        "email": "string"
    }
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8085/get_profile_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67cc161e272d605a7902a4e2"
     }'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Account retrieved successfully",
    "profile": {
        "_id": {
            "$oid": "67cc161e272d605a7902a4e2"
        },
        "name": "test",
        "role": "CITIZEN",
        "email": "test"
    }
}
```