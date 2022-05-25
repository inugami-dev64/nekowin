# Nekowin

![](avatar.png)

## What is it?
Nekowin is a simple OpenGL and Vulkan compatible context window / surface library designed to work on GNU/Linux and Windows operating systems. 
It also has an ability to manage input devices and events.

## Building (Debian)
Install prerequisites:  
```
$ sudo apt install libx11-dev libxcursor-dev
```

Build using cmake and make:  
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=<Debug|Release> -DVULKAN_SDK_PATH=<path-to-vulkan-sdk> ..
$ make
```

## Usage
Read [USAGE](USAGE.md) file for more information and references about using nekowin in your project
