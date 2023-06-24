# RoboVision_Client_Bridge_Server
A modified version of the [Shared_Memory_in_Robo_Vision_Simulator](https://github.com/ParinazRmp/Shared_Memory_in_Robo_Vision_Simulator.git) project includes client/server capabilities.

## ARP-Assignment3

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#Introduction">Introduction</a></li>
    <li><a href="#Installation_and_Running">Installation_and_Running</a></li>
    <li><a href="#Fauctionality">Fauctionality</a></li>
    <li><a href="#How_it_works">How_it_works</a></li>
    <li><a href="#PseudoCode">PseudoCode</a></li>
    <li><a href="#Simulation_and_Results">Simulation_and_Results</a></li>
    <li><a href="#Members">Members</a></li>
  </ol>
</details>

## Introduction
It is a modified version of this project [Shared_Memory_in_Robo_Vision_Simulator](https://github.com/ParinazRmp/Shared_Memory_in_Robo_Vision_Simulator.git)  that is used to design, develop, test, and deploy an interactive simulator of a (simplified) typical vision system. 

The updated version has client/server features and is called an "application". 

The modified application has some unchanged features, such as process B, shared memory, and the second Ncurses window. 

This version of Process A now includes two new features. The first one is a client connection to a similar app.

The structure of the code is as follows:

![image](https://github.com/ParinazRmp/RoboVision_Client_Bridge_Server/assets/94115975/70db3a15-a3b3-4c4d-be8f-ad741e862222)

<img src="https://user-images.githubusercontent.com/80394968/225428258-e8b9859d-d05b-42ed-b783-29340073b10f.png" width="60" />

## Installation_and_Running
### 1. ncurses installation
To install the ncurses library, simply open a terminal and type the following command:
```console
sudo apt-get install libncurses-dev
```
### 2. *libbitmap* installation and usage
To work with the bitmap library, you need to follow these steps:
1. Download the source code from [this GitHub repo](https://github.com/draekko/libbitmap.git) in your file system.
2. Navigate to the root directory of the downloaded repo and run the configuration through command ```./configure```. Configuration might take a while.  While running, it prints some messages telling which features it is checking for.
3. Type ```make``` to compile the package.
4. Run ```make install``` to install the programs and any data files and documentation.
5. Upon completing the installation, check that the files have been properly installed by navigating to ```/usr/local/lib```, where you should find the ```libbmp.so``` shared library ready for use.
6. In order to properly compile programs which use the *libbitmap* library, you first need to notify the **linker** about the location of the shared library. To do that, you can simply add the following line at the end of your ```.bashrc``` file:      
```export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"```

### 3. Compiling and running Process A and Process B
Once the installation is done, clone this repository to your desired location:
<pre><code>git clone https://github.com/ParinazRmp/RoboVision_Client_Bridge_Server.git </code></pre>
After you clone the repository, run this command:
<pre><code> sh run.sh </code></pre>

<img src="https://user-images.githubusercontent.com/80394968/225428503-ce0991e7-10c8-45f5-bb52-ab4de821c27d.png" width="60" /> 

## Members
| Github-page | Email |
|------------------|------------------|
| [Danial Sabzevari](https://github.com/dssdanial)  | s5217544@studenti.unige.it |
| [Parinaz Ramezanpour](https://github.com/ParinazRmp)  | s5214640@studenti.unige.it |
