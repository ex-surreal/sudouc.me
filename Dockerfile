FROM gcc
ADD . .
RUN gcc main.c -o main
