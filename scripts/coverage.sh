#!/bin/bash
cd ..

set -e

BUILD_DIR="build"
COVERAGE_DIR="coverage"

rm -rf "$COVERAGE_DIR"
mkdir -p "$COVERAGE_DIR"

# Run tests to generate .gcda files
echo "Running tests..."
cd "$BUILD_DIR"
ctest --output-on-failure   # or run your test binary directly
cd ..

# Step 1: Capture coverage and ignore common errors.
lcov --rc derive_function_end_line=0 \
     --rc branch_coverage=0 \
     --rc check_data_consistency=0 \
     --ignore-errors all,unused,empty,format,inconsistent,deprecated \
     --directory "$BUILD_DIR" \
     --capture \
     --output-file "$COVERAGE_DIR/coverage.info"

# Step 2: Remove external dependencies from the coverage data.
lcov --remove "$COVERAGE_DIR/coverage.info" \
     "/Users/albertarielwidiaatmaja/Documents/NUS/Y4S2/CS3203/Project/CS3203_3_Backend/build/_deps/*" \
     "/Applications/Xcode.app/*" \
     --output-file "$COVERAGE_DIR/coverage_clean.info" \
     --ignore-errors unused,empty,format,inconsistent,deprecated

# Step 3: Extract coverage only for your own code.
lcov --rc check_data_consistency=0 \
     --extract "$COVERAGE_DIR/coverage_clean.info" \
     "$(pwd)/common/*.cpp" \
     "$(pwd)/services/*.cpp" \
     --output-file "$COVERAGE_DIR/coverage_filtered.info" \
     --ignore-errors unused,empty,format

# Step 4: Generate HTML report.
genhtml "$COVERAGE_DIR/coverage_filtered.info" \
        --output-directory "$COVERAGE_DIR/html" \
        --ignore-errors unused,empty,format,inconsistent,deprecated

echo "✅ Coverage report available at: $COVERAGE_DIR/html/index.html"
