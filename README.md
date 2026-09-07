# ECE 5532 Examples Repository - Fall 2026

This repository contains the instructions for setting up a PC with Ubuntu 24.04 and ROS 2 Jazzy to follow along with the ECE 5532 lecture videos and implement the corresponding programming homework assignments.

## Computer Setup Instructions

There are three viable options to get a computer set up properly for the course.
- Natively install Ubuntu 24.04 on either an empty hard drive, or on a separate partition of a hard drive (**RECOMMENDED**).
- Install VMWare Player in an existing Windows installation and then set up a virtual machine (VM) with Ubuntu 24.04 installed on it.
- Use Docker in another existing Ubuntu installation. It is not recommended to use Docker with Windows.

To install Docker and build a development image for the course, follow these instructions:

https://github.com/robustify/ece5532_ros2_docker

and disregard the instructions below.

### Operating System Setup

Regardless of whether a native installation or virtual machine is selected, the Ubuntu installation disk image needs to be downloaded.
To do so, download the latest desktop image of Ubuntu version 24.04 LTS from Ubuntu's website: [https://releases.ubuntu.com/24.04/](https://releases.ubuntu.com/24.04/).
After the download is complete, then either use it to create a VM or create a USB installation drive to install it natively.

To install natively:
- Create a bootable USB flash drive to install Ubuntu by following the instructions found here: [https://ubuntu.com/tutorials/create-a-usb-stick-on-windows](https://ubuntu.com/tutorials/create-a-usb-stick-on-windows).
- Restart the computer and boot from the flash drive.
- Install Ubuntu! This step depends on whether you're installing on a separate partition or on the entire hard drive. If you want to install Ubuntu next to Windows, you can use the Windows disk management utility to resize your existing Windows installation and create a new, second partition. Ubuntu can then be installed on this new partition. However, be careful when doing this, and back up any important files just in case something happens to your original Windows installation.

To set up a virtual machine:
- Download and install VMWare player for Windows from [here](https://1drv.ms/u/s!Ar6id-4c-fy31Qs6W53MPME0j493).
- Create a VM by following along with this walkthrough video: [https://youtu.be/f9v6WJ6tTkI](https://youtu.be/f9v6WJ6tTkI).

### Software Setup

Once the operating system is set up properly, it is time to set up the new OS installation for the course. To do this, first download the script that automates the software installation, and save it somewhere on your Ubuntu filesystem:
[ece5532_software_setup.bash](https://1drv.ms/u/c/b7fcf91cee77a2be/IQBZNGY8xeeNR4HZF7ojf3i5AbU7Q1NkFlgOsRfybN2p43s?e=gXTfPZ)

Then, open a terminal with `Ctrl-Alt-T`, `cd` to the location where `software_setup_ece5532.bash` is saved, and enter the following commands to first make the script executable, and then actually run it:

```
chmod +x ece5532_software_setup.bash
./ece5532_software_setup.bash
```
This script automatically clones the following repositories that support the simulations used in the course:

- `ece5532_gazebo` (https://github.com/robustify/ece5532_gazebo.git)
- `audibot` (https://github.com/robustify/audibot.git)

In case you need to recreate the ROS workspace manually, clone these two repositories in addition to this one.

### Install Visual Studio Code

VS Code is recommended to write the code for this class.
To install it on your PC:

```
sudo snap install --classic code
```
To help with writing code for ROS 2, it is recommended to install the "Robot Developer Extensions for ROS 2" VS Code extension.
To install this, open VS Code, click Extensions on the left side panel, and search for "ROS".

### Set Up GitHub CLI

The homework submission process uses a third-party tool connected to GitHub called Classroom50, which uses GitHub CLI to automate some of the submission procedure.
The base GitHub CLI tool `gh` is installed by the `ece5532_software_setup.bash` script, but further setup steps are needed to use it properly for the course.

1. Get a token with your GitHub credentials through browser OAuth

```
gh auth login --hostname github.com --git-protocol https --web --scopes "admin:org,read:org,repo,workflow"
```
After following the prompts to open a web browser, enter the one-time-code, and then authorize access, the terminal should show that you are logged in with your GitHub username.


2. Install the Classroom50 student extension

```
gh extension install foundation50/gh-student
```