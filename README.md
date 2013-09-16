trillek-client
==============

Installing dependancies on Ubuntu
---------------------------------
```bash
sudo apt-add-repository ppa:sonkun/sfml-development
sudo apt-add-repository ppa:openrave/release
sudo apt-get update
sudo apt-get install build-essential libsfml-dev libassimp-dev libglew-dev
```

Build and Run
-------------
```bash
cmake .
make
./trillek-client-core
```
