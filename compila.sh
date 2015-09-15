clear
rm main
clear
g++ `pkg-config --cflags opencv` -o main main.cpp `pkg-config --libs opencv` -I/usr/include/pqxx -lpqxx -lpq
./main video.mp4
