cd ../build
make -j3
make install
cp ../.env ../release/bin/
ctest --test-dir ../build