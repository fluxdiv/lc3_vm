#!/usr/bin/env fish

# Takes the base name of the executable (output binary) as an argument

# Check if an argument is provided
if test (count $argv) -ne 1
    echo "Usage: $argv[0] <output binary name>"
    exit 1
end

# The name of the output binary
set BASE_NAME $argv[1]

# Debug mode
set DEBUG "-ggdb"

# Release mode
# set RELEASE "-O2 -DNDEBUG"

# Disable compiler extensions
set NOEXT "-pedantic-errors"

# Show all the warnings
set WARNINGS "-Wall" "-Weffc++" "-Wextra" "-Wconversion" "-Wsign-conversion"

# Treat warnings as errors
# set ERRONWARN "-Werror"

# What language standard to use
set STANDARD "-std=c++2a"

# Get all .cpp files in current dir
set CPP_FILES (ls *.cpp)

# Ensure bin directory exists
if not test -d bin
    mkdir bin
end

# Compile all .cpp files and link them into a single executable
# g++ $CPP_FILES -o "$BASE_NAME" $DEBUG $NOEXT $WARNINGS $ERRONWARN $STANDARD
g++ $CPP_FILES -o "bin/$BASE_NAME" $DEBUG $NOEXT $WARNINGS $STANDARD

# Check if the compilation was successful
if test $status -eq 0
    echo "Compilation successful. Output file is $BASE_NAME"
    echo "Running the program..."
    echo "---------------------------------------------------"
    # ./bin/"$BASE_NAME"
else
    echo "Compilation failed."
end
