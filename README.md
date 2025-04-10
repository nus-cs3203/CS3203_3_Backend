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

### **POST /complaints/get_statistics**

- **Purpose**: Retrieve the total count of complaints and average sentiment, based on filters.

**Request:**
```json
{
    "filter": {
        "$text": {                      // (optional) Title or selftext contains this keyword (case-insensitive)
            "$search": "string"
        },        
        "source": "string",             // (optional) Source of complaint, e.g. "Reddit"
        "category": "string",           // (optional) Category of complaint, e.g. "Housing"
        "_from_date": "string",         // (optional) format: dd-mm-YYYY HH:MM:SS
        "_to_date": "string",           // (optional) format: dd-mm-YYYY HH:MM:SS
        "_from_sentiment": "double",    // (optional)
        "_to_sentiment": "double"       // (optional)
    }
}
```
> **Note**: The `filter` field IS NO LONGER optional. Any field inside `filter` is optional for this API.

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "statistics": {
        "count": "int",
        "avg_sentiment": "float"
    }
}
```

**Sample Requests:**
```sh
curl -X POST "http://localhost:8082/complaints/get_statistics" \
-H "Content-Type: application/json" \
-d '{
    "filter": {
    }
}'
```
```sh
curl -X POST "http://localhost:8082/complaints/get_statistics" \
-H "Content-Type: application/json" \
-d '{
    "filter": {
        "category": "Housing"
    }
}'
```
```sh
curl -X POST "http://localhost:8082/complaints/get_statistics" \
-H "Content-Type: application/json" \
-d '{
    "filter": {
        "category": "Housing",
        "_from_date": "01-01-2023 00:00:00",
        "_to_date":  "02-01-2023 00:00:00"
    }
}'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Server processed aggregate request successfully.",
    "statistics": {
        "count": 1,
        "avg_sentiment": -0.815503
    }
}
```

---

### **POST /complaints/get_statistics_over_time**

- **Purpose**: Retrieve count and average sentiment for complaints over a time range based on specified filter. Results are grouped by `"%m-%Y"` (month-year) buckets.

**Request:**
```json
{
    "filter": {
        "$text": {                      // (optional) Title or selftext contains this keyword (case-insensitive)
            "$search": "string"
        },        
        "source": "string",             // (optional) Source of complaint, e.g. "Reddit"
        "category": "string",           // (optional) Category of complaint, e.g. "Housing"
        "_from_date": "string",         // (REQUIRED) format: dd-mm-YYYY HH:MM:SS
        "_to_date": "string",           // (REQUIRED) format: dd-mm-YYYY HH:MM:SS
        "_from_sentiment": "double",    // (optional)
        "_to_sentiment": "double"       // (optional)
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "statistics": [
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
curl -X POST "http://localhost:8082/complaints/get_statistics_over_time" \
-H "Content-Type: application/json" \
-d '{
    "filter": {
        "_from_date": "01-01-2023 00:00:00",
        "_to_date":  "02-01-2023 00:00:00"
    }
}'
```
```sh
curl -X POST "http://localhost:8082/complaints/get_statistics_over_time" \
-H "Content-Type: application/json" \
-d '{
    "filter": {
        "category": "Housing",
        "_from_date": "01-01-2023 00:00:00",
        "_to_date":  "02-01-2025 00:00:00"
    }
}'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Server processed aggregate request successfully.",
    "statistics": [
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

### **POST /complaints/get_statistics_grouped**

- **Purpose**: Group complaints based on a specified field (e.g., `category`), returning `count` and `avg_sentiment` of that group.
- **`group_by_field` explanation**: This is the field in the `complaints` collection used for grouping e.g.  `"category"`, `"source"`.

**Request:**
```json
{
    "group_by_field": "string",
    "filter": {
        "$text": {                      // (optional) Title or selftext contains this keyword (case-insensitive)
            "$search": "string"
        },        
        "source": "string",             // (optional) Source of complaint, e.g. "Reddit"
        "category": "string",           // (optional) Category of complaint, e.g. "Housing"
        "_from_date": "string",         // (optional) format: dd-mm-YYYY HH:MM:SS
        "_to_date": "string",           // (optional) format: dd-mm-YYYY HH:MM:SS
        "_from_sentiment": "double",    // (optional)
        "_to_sentiment": "double"       // (optional)
    }
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
    curl -X POST "http://localhost:8082/complaints/get_statistics_grouped" \
    -H "Content-Type: application/json" \
    -d '{
        "group_by_field": "category",
        "filter": {
        }
    }'
```
```sh
    curl -X POST "http://localhost:8082/complaints/get_statistics_grouped" \
    -H "Content-Type: application/json" \
    -d '{
        "group_by_field": "category",
        "filter": {
            "_from_date": "20-03-2024 00:00:00",
            "_to_date": "02-01-2025 23:59:59"
        }
    }'
```

**Sample Response:**
```json
{
    "message": "Analytics result retrieved.",
    "success": true,
    "statistics": {
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

### **POST /complaints/get_statistics_grouped_over_time**

- **Purpose**: Similar to `/complaints/get_statistics_grouped`, but also groups the results by month.
- Note: for time interval with no data for that category, default value of 0 is used for count and avg_sentiment.

**Request:**
```json
{
    "group_by_field": "string",
    "filter": {
        "$text": {                      // (optional) Title or selftext contains this keyword (case-insensitive)
            "$search": "string"
        },        
        "source": "string",             // (optional) Source of complaint, e.g. "Reddit"
        "category": "string",           // (optional) Category of complaint, e.g. "Housing"
        "_from_date": "string",         // (REQUIRED) format: dd-mm-YYYY HH:MM:SS
        "_to_date": "string",           // (REQUIRED) format: dd-mm-YYYY HH:MM:SS
        "_from_sentiment": "double",    // (optional)
        "_to_sentiment": "double"       // (optional)
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "statistics": [
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
    curl -X POST "http://localhost:8082/complaints/get_statistics_grouped_over_time" \
    -H "Content-Type: application/json" \
    -d '{
        "group_by_field": "category",
        "filter": {
            "_from_date": "20-03-2024 00:00:00",
            "_to_date": "02-01-2025 23:59:59"
        }
    }'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Server processed aggregate request successfully.",
    "statistics": [
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

### **POST /complaints/get_statistics_grouped_by_sentiment_value**

- **Purpose**: Group complaints into "buckets" based on their sentiment value. For example, if the `bucket_size` is 0.5, it will group sentiments in the following ranges: `[-1, -0.5), [-0.5, 0), [0, 0.5), [0.5, 1)`.
- **`bucket_size` explanation**: A numerical interval for grouping sentiment values, e.g. `0.5` creates buckets of width 0.5 each.

**Request:**
```json
{
    "bucket_size": "float",
    "filter": {
        "$text": {                      // (optional) Title or selftext contains this keyword (case-insensitive)
            "$search": "string"
        },        
        "source": "string",             // (optional) Source of complaint, e.g. "Reddit"
        "category": "string",           // (optional) Category of complaint, e.g. "Housing"
        "_from_date": "string",         // (optional) format: dd-mm-YYYY HH:MM:SS
        "_to_date": "string",           // (optional) format: dd-mm-YYYY HH:MM:SS
        "_from_sentiment": "double",    // (optional)
        "_to_sentiment": "double"       // (optional)
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "statistics": [
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
    curl -X POST "http://localhost:8082/complaints/get_statistics_grouped_by_sentiment_value" \
    -H "Content-Type: application/json" \
    -d '{
        "bucket_size": 0.5,
        "filter": {
            "_from_date": "01-01-2010 00:00:00",
            "_to_date": "31-12-2010 23:59:59"
        }
    }'
```

**Sample Response:**
```json
{
    "message": "Server processed aggregate request successfully.",
    "success": true,
    "statistics": [
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

### **POST /category_analytics/get_by_name**

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
        "keywords": ["string", ...],
        "absa_result": [{"theme": "string", "sentiment": "string"}, ...],
        "summary": "string",
        "forecasted_sentiment": "float",
        "sentiment": "float",
        "concerns": ["string", ...]
    }
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8082/category_analytics/get_by_name" \
-H "Content-Type: application/json" \
-d '{
    "name": "Housing"
}'
```

**Sample Response:**
```json
{
    "message": "Server processed get request successfully.",
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
        "keywords": [
            "cars",
            "pollution",
            "MRT"
        ],
        "summary": "Concerns about transportation persist among citizens.",
        "forecasted_sentiment": 0.5398,
        "sentiment": 0.5714,
        "concerns": [
            "Poor infrastructure"
        ]
    }
}
```

---

## Service: **management**

This service provides various endpoints to manage categories and complaints for administrative purposes.

---

### **Collection: `categories`**

Refer to Schema Document for collection definition.

---

#### **POST /categories/get_count**

**Request:**
```json
{
    "filter": {
        // any valid field filters (e.g. {"name": "Techonology"} ), this API generally only makes sense for empty filter to get total number categories we have
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "count": "int"
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/categories/get_count \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {}
     }'
```

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
         "oid": "67cc41c23b05707c187145ec"
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
    "oid": "string"  // internal id of MongoDB
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

#### **POST /complaints/get_count**

**Request:**
```json
{
    "filter": {
        // any valid field filters (e.g. {"category": "Technology"} )
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "count": "int"
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/complaints/get_count \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {}
     }'
```

```sh
curl -X POST http://localhost:8083/complaints/get_count \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {"category": "Technology"} 
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
         "oid": "67cc41c73b05707c1872c22d"
     }'
```

---

#### **POST /complaints/get_many**

**Supports: Keyword Search**:  
Text field explanation: Searches for the **existence** of a word in the `title` or `description` of a complaint (case-insensitive). For example, `"this"` will match the word `"this"` but not just the letter `"t"`.

**Pagination explanation**:  
- Filtered results are sorted by time (decreasing order).  
- This endpoint will return the slice of documents at index `[page_size * page_number, page_size * (page_number + 1) - 1]` (1-based indexing).

**Sort Field**:
- Optionally, takes sort field.

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
    "page_number": "int",
    "sort": { // optional
        "field1": "int", // 1 for ascending, -1 for descending
        "field2": "int",
        ...
    }
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

**Sample Request:**
```sh
curl -X POST http://localhost:8083/complaints/get_many \
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

```sh
curl -X POST http://localhost:8083/complaints/get_many \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {
             "$text": {
                 "$search": "new"
             },
             "category": "Technology"
         },
         "page_size": 25,
         "page_number": 1,
         "sort": {
            "date": -1,
            "category": 1
         }
     }'
```

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
        "end_date": "01-06-2023 23:59:59"
     }'
```

---

### **Collection: `posts`**

Refer to Schema Document for collection definition.

---

#### **POST /posts/get_count**

**Request:**
```json
{
    "filter": {
        // any valid field filters (e.g. {"source": "Reddit"} )
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "count": "int"
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/posts/get_count \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {}
     }'
```

```sh
curl -X POST http://localhost:8083/posts/get_count \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {"source": "Reddit"}
     }'
```

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

### **Collection: `polls`**

Refer to Schema Document for collection definition.

---

#### **POST /polls/insert_one**

**Request:**
```json
{
    "document": {
        "question": "string",
        "category": "string",
        "question_type": "string",
        "options": [],
        "date_created": "string", // format: dd-mm-YYYY HH:MM:SS
        "date_published": "null",
        "date_closed": "null",
        "status": "string"
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "oid": "string"
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/polls/insert_one \
     -H "Content-Type: application/json" \
     -d '{
         "document": {
             "question": "Which hawker centre is the dirtiest?",
             "category": "Housing",
             "question_type": "MCQ",
             "options": ["Maxwell Food Centre", "Chinatown Complex", "Old Airport Road", "Newton Food Centre"],
             "date_created": "01-01-2022 00:00:00",
             "date_published": null,
             "date_closed": null,
             "status": "Unpublished"
         }
     }'
```

---

#### **POST /polls/get_by_oid**

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
    "document": {} // Poll document or null
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/polls/get_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67d6d68877831f230e0b0113"
     }'
```

---

#### **POST /polls/get_many**

**Pagination explanation**:  
- Filtered results are sorted in descending order (implementation-dependent).
- This endpoint returns documents at index `[page_size * page_number, page_size * (page_number + 1) - 1]` (1-based indexing).

**Sort Field**:
- Optionally, takes sort field.

**Request:**
```json
{
    "filter": {
        "field1": "string",
        ...
    },
    "page_size": "int",
    "page_number": "int",
    "sort": { // optional
        "field1": "int", // 1 for ascending, -1 for descending
        "field2": "int",
        ...
    }
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
curl -X POST "http://localhost:8083/polls/get_many" \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {
             "status": "Unpublished"
         },
         "page_size": 2,
         "page_number": 1
     }'
```

---

#### **POST /polls/get_count**

**Request:**
```json
{
    "filter": {
        // any valid field filters (e.g. {"status": "Unpublished"} )
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "count": "int"
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/polls/get_count \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {}
     }'
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/polls/get_count \
     -H "Content-Type: application/json" \
     -d '{
        "filter": {
            "status": "Unpublished"
        }
     }'
```

---

#### **POST /polls/delete_by_oid**

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
    "deleted_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/polls/delete_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67d6d68877831f230e0b0112"
     }'
```

---

#### **POST /polls/delete_many_by_oids**

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
    "deleted_count": "int"
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/polls/delete_many_by_oids" \
     -H "Content-Type: application/json" \
     -d '{
         "oids": ["67b458405b0f29f2e7c47ce9", "67b458405b0f29f2e7c47ce7"]
     }'
```

---

#### **POST /polls/update_by_oid**

**Request:**
```json
{
    "oid": "string",
    "update_document": {
        "$set": {
            // fields to be updated
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
curl -X POST "http://localhost:8083/polls/update_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67d6d68877831f230e0b0113",
         "update_document": {
             "$set": {
                 "status": "Published"
             }
         }
     }'
```

---

### **Collection: `poll_templates`**

Refer to Schema Document for collection definition.

---

#### **POST /poll_templates/get_all**

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
curl -X POST "http://localhost:8083/poll_templates/get_all" \
     -H "Content-Type: application/json" \
     -d '{}'
```

---

#### **POST /poll_templates/get_by_oid**

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
    "document": {} // PollTemplate document or null
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8083/poll_templates/get_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67d70b68b4119a65bca73640"
     }'
```

### **Collection: `poll_responses`**

Refer to Schema Document for collection definition.

---

#### **POST /poll_responses/insert_one**

- **Purpose**: Insert a new poll response document.

**Request:**
```json
{
    "document": {
        "poll_id": "string",         // MongoDB OID (as string) referencing the poll
        "user_id": "string",         // MongoDB OID (as string) referencing the user
        "response": "string",        // Response data (e.g., a text answer or selected option)
        "date_submitted": "string"   // format: dd-mm-YYYY HH:MM:SS
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "oid": "string"  // The inserted document's MongoDB OID
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/poll_responses/insert_one \
     -H "Content-Type: application/json" \
     -d '{
         "document": {
             "poll_id": "67da871c1447ef5cec00d5f1",
             "user_id": "67d93e8c3c0bfe14510691b1",
             "response": "Maxwell Food Centre",
             "date_submitted": "01-01-2022 00:00:00"
         }
     }'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Server processed insert request successfully.",
    "oid": "67dad311a63875aa4603d5d1"
}
```

---

#### **POST /poll_responses/get_one**

- **Purpose**: Retrieve a single poll response matching the specified filter criteria.

**Request:**
```json
{
    "filter": {
        "poll_id": "string",
        "user_id": "string"
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "document": {
        "_id": {
            "$oid": "string"
        },
        "user_id": "string",
        "poll_id": "string",
        "response": "string",
        "date_submitted": "string"
    }
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/poll_responses/get_one \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {
             "poll_id": "67da871c1447ef5cec00d5f1",
             "user_id": "67d93e8c3c0bfe14510691b1"
         }
     }'
```

**Sample Response:**
```json
{
    "message": "Server processed get request successfully.",
    "success": true,
    "document": {
        "_id": {
            "$oid": "67da891dbb0d0f2b82082521"
        },
        "user_id": "67d93e8c3c0bfe14510691b1",
        "poll_id": "67da871c1447ef5cec00d5f1",
        "response": "Maxwell Food Centre",
        "date_submitted": "01-01-2022 00:00:00"
    }
}
```

---

#### **POST /poll_responses/get_many**

- **Purpose**: Retrieve multiple poll responses that match the specified filter, supporting pagination and optional sorting.

**Pagination Explanation**:  
- Results can be paginated using `page_size` and `page_number`.  

**Request:**
```json
{
    "filter": {
        "field1": "string",
        "field2": "string",
        ...
    },
    "page_size": "int",
    "page_number": "int",
    "sort": { // optional
        "field1": "int", // 1 for ascending, -1 for descending
        "field2": "int",
        ...
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "documents": [
        {
            "_id": {
                "$oid": "string"
            },
            "user_id": "string",
            "poll_id": "string",
            "response": "string",
            "date_submitted": "string"
        },
        ...
    ]
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/poll_responses/get_many \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {
            "poll_id": "67da871c1447ef5cec00d5f1"
         },
         "page_size": 1,
         "page_number": 1,
         "sort": {
            "date": 1
         }
     }'
```

**Sample Response:**
```json
{
    "message": "Server processed get request successfully.",
    "success": true,
    "documents": [
        {
            "_id": {
                "$oid": "67da891dbb0d0f2b82082521"
            },
            "user_id": "67d93e8c3c0bfe14510691b1",
            "poll_id": "67da871c1447ef5cec00d5f1",
            "response": "Maxwell Food Centre",
            "date_submitted": "01-01-2022 00:00:00"
        }
    ]
}
```

---

#### **POST /poll_responses/get_statistics**

- **Purpose**: Retrieve aggregated statistics (count of each unique response) for a given poll.

**Request:**
```json
{
    "filter": {
        "poll_id": "string"
        // other potential fields (if exist in schema)
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "statistics": {
        "response_value_1": "int", // count for response_value_1
        "response_value_2": "int",
        ...
    }
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/poll_responses/get_statistics \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {
             "poll_id": "67da871c1447ef5cec00d5f1"
         }
     }'
```

**Sample Response:**
```json
{
    "success": true,
    "message": "Server processed get request successfully.",
    "statistics": {
        "Maxwell Food Centre": 2
    }
}
```

---

#### **POST /poll_responses/get_count**

- **Purpose**: Retrieve the total number of poll response documents matching the specified filter.

**Request:**
```json
{
    "filter": {
        // If empty, counts all documents
    }
}
```

**Response:**
```json
{
    "success": "bool",
    "message": "string",
    "count": "int"
}
```

**Sample Request:**
```sh
curl -X POST http://localhost:8083/poll_responses/get_count \
     -H "Content-Type: application/json" \
     -d '{
         "filter": {}
     }'
```

**Sample Response:**
```json
{
    "message": "Server processed count_documents request successfully.",
    "success": true,
    "count": 2
}
```

---

## Service: **user**

This service provides user management endpoints, including account creation (`signup`), authentication (`login`), and profile retrieval.

---

### **POST /signup**

- **Purpose**: Create a new account with role `Citizen`.

**Request:**
```json
{
    "document": {
        "name": "string",
        "email": "string",
        "password": "string",
        "collectibles": "[]string"
    }
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
        "document": {
            "name": "test",
            "email": "test",
            "password": "test",
            "collectibles": []
        }
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

### **POST /create_admin_account**

- **Purpose**: Create a new account with role `Admin`.

**Request:**
```json
{
    "document": {
        "name": "string",
        "email": "string",
        "password": "string",
        "collectibles": "[]string"
    }
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
curl -X POST "http://localhost:8085/create_admin_account" \
     -H "Content-Type: application/json" \
     -d '{
        "document": {
            "name": "admin",
            "email": "admin",
            "password": "admin",
            "collectibles": []
        }
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
        "email": "string",
        "collectibles": "[]string"
    }
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8085/get_profile_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67d93e8c3c0bfe14510691b1"
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
        "role": "Citizen",
        "email": "test",
        "collecitbles": []
    }
}
```

---

### **POST /update_profile_by_oid**

- **Purpose**: Update a user's profile based on their unique ID (`oid`).

**Request:**
```json
{
    "oid": "string",
    "update_document": {
        "$set": {
            "field_name": "updated_value"
        }
    }
}
```

**Sample Request:**
```sh
curl -X POST "http://localhost:8085/update_profile_by_oid" \
     -H "Content-Type: application/json" \
     -d '{
         "oid": "67d93e8c3c0bfe14510691b1",
         "update_document": {
             "$set": {
                 "name": "test_2"
             }
         }
     }'
```

**Sample Response:**
```json
{
    "message": "Server processed update request successfully.",
    "matched_count": 1,
    "success": true,
    "modified_count": 0,
    "upserted_count": 0
}
```
---

## JWT Protection Logic

We use JWT to protect our API endpoints. JWT can be obtained through `/login` API in **user** service. JWT token should be attached as a Bearer token to access JWT-protected API endpoints.

### JWT Access Levels

| Level      | Description                                                                                                      |
|------------|------------------------------------------------------------------------------------------------------------------|
| `None`     | No bearer token required.                                                                                        |
| `Citizen`  | Requires a valid bearer token of account with `role=Citizen`. Used for general authenticated user access.        |
| `Admin`    | Requires a valid bearer token of account with `role=Admin`. Reserved for privileged administrative operations.   |
| `Personal` | Only used for modification of personal profile. Requires bearer token of profile owner.                          |

**Note:** Some API paths have access level `None`. This does not mean they are publicly available since we whitelist clients who are able to call our API endpoints. This just means that it does not necessarily make sense to `login` in order to access these API endpoints.

---

### Management Service

| Endpoint                            | JWT Protection |
|-------------------------------------|----------------|
| `/categories/get_count`             | None           |
| `/categories/get_all`               | None           |
| `/categories/get_by_oid`            | None           |
| `/categories/insert_one`            | Admin          |
| `/categories/delete_by_oid`         | Admin          |
| `/categories/update_by_oid`         | Admin          |
| `/posts/get_count`                  | None           |
| `/posts/get_by_daterange`           | None           |
| `/complaints/get_count`             | None           |
| `/complaints/get_by_oid`            | None           |
| `/complaints/get_by_daterange`      | None           |
| `/complaints/get_many`              | None           |
| `/complaints/delete_by_oid`         | Admin          |
| `/complaints/delete_many_by_oids`   | Admin          |
| `/complaints/update_by_oid`         | Admin          |
| `/polls/insert_one`                 | Admin          |
| `/polls/get_by_oid`                 | None           |
| `/polls/get_many`                   | None           |
| `/polls/get_count`                  | None           |
| `/polls/delete_by_oid`              | Admin          |
| `/polls/delete_many_by_oids`        | Admin          |
| `/polls/update_by_oid`              | Admin          |
| `/poll_templates/get_all`           | None           |
| `/poll_templates/get_by_oid`        | None           |
| `/poll_responses/get_count`         | None           |
| `/poll_responses/get_one`           | None           |
| `/poll_responses/get_many`          | None           |
| `/poll_responses/insert_one`        | Citizen        |
| `/poll_responses/get_statistics`    | None           |

---

### Analytics Service

| Endpoint                                                 | JWT Protection |
|----------------------------------------------------------|----------------|
| `/category_analytics/get_by_name`                        | None           |
| `/complaints/get_statistics`                             | None           |
| `/complaints/get_statistics_over_time`                   | None           |
| `/complaints/get_statistics_grouped`                     | None           |
| `/complaints/get_statistics_grouped_over_time`           | None           |
| `/complaints/get_statistics_grouped_by_sentiment_value`  | None           |

---

### User Service

| Endpoint                   | JWT Protection |
|----------------------------|----------------|
| `/signup`                  | None           |
| `/create_admin_account`    | Admin          |
| `/login`                   | None           |
| `/get_profile_by_oid`      | None           |
| `/update_profile_by_oid`   | Personal       |

---