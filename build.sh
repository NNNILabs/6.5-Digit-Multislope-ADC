VAR=$(realpath ./build)
echo "Found absolute build path: ${VAR}"

cmake -B $VAR -S ./src/Multislope/
cmake --build $VAR --config Debug --target all
