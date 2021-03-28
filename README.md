# sneaker (alpha)

1. compile:
g++ *.cpp -std=c++11 -o sneaker -no-pie -Wall \`pkg-config gtkmm-3.0 --cflags --libs\` -lcap

2. set right to open raw socket as non root:
setcap cap_net_raw=eip ./sneaker

![alt tag](https://github.com/bitspalter/sneaker/blob/main/sneaker.png "sneaker")
