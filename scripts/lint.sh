#!/bin/bash
cd ..

echo -e "Running clang-tidy...}"

find common services tests -type f \( -name '*.cpp' -o -name '*.hpp' \) | while read -r file; do
  echo -e "Linting ${file}"
  clang-tidy "$file" -- -Icommon -Iservices
done

echo -e "Running clang-format..."

find common services tests -type f \( -name '*.cpp' -o -name '*.hpp' \) | while read -r file; do
  echo "Formatting $file"
  clang-format -i "$file"
done

echo -e "Done!"