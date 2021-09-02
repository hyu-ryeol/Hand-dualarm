# Poco_install
```bash
sudo apt-get install openssl libssl-dev
sudo apt-get install libiodbc2 libiodbc2-dev
wget http://pocoproject.org/releases/poco-1.7.4/poco-1.7.4.tar.gz
./configure --omit=Data/ODBC,Data/MySQL
make -j16
make install
ldconfig
```

```bash
g++ -I/usr/local/include -L/usr/local/lib -o poco_test test.cpp -lPocoFoundation -lPocoUtil
g++  -I/usr/local/include -L/usr/local/lib -o server TCPServer.cpp -lPocoFoundation -lPocoUtil -lPocoNet
g++  -I/usr/local/include -L/usr/local/lib -o client TCPClient.cpp -lPocoFoundation -lPocoUtil -lPocoNet
```

```bash
make
```

```bash
 ./server
```

```bash
./client
```
