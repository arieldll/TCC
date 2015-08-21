clear
rm main
clear
g++ `pkg-config --cflags opencv` -o main main.cpp `pkg-config --libs opencv`
./main video.mp4
