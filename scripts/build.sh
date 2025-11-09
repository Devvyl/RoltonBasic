#!/bin/bash

mkdir -p build bin

echo "Компиляция RoltonDevvyl..."

g++ -c src/lexer.cpp -Iinclude -o build/lexer.o
g++ -c src/parser.cpp -Iinclude -o build/parser.o
g++ -c src/compiler.cpp -Iinclude -o build/compiler.o
g++ -c src/types.cpp -Iinclude -o build/types.o

g++ src/rolton-devvyl.cpp build/lexer.o build/parser.o build/compiler.o build/types.o -Iinclude -o bin/roltondevvyl

if [ $? -eq 0 ]; then
    echo "Компиляция завершена успешно!"
    echo "Исполняемый файл: bin/roltondevvyl"
else
    echo "Ошибка компиляции!"
    exit 1
fi
