# CS3203 Backend

## Requirements:

- Boost 1.8x
- CrowCpp v.1.0+
- Cmake 3.10+

## Local Environment

### How to Setup?

1. Use `brew` to install `boost` and `cmake`:
    ```bash
    brew install boost cmake
    ```
2. Create a directory for libraries:
   ```bash
   mkdir libs && cd libs
   ```
3. Clone the CrowCpp repository:
   ```bash
   git clone https://github.com/CrowCpp/Crow.git
   ```
4. Return to the root directory:
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