# Dockerfile to build CSE411 developer image
FROM ubuntu
MAINTAINER Younger

# Initialize repos and update the base system
RUN apt-get update -y
RUN apt-get upgrade -y

# Install additional software needed for development (you can add to this list)
RUN apt-get install -y git man curl build-essential screen gdb libssl-dev wget cmake vim libtbb-dev libv8-dev libboost-all-dev

# To make a container from this Dockerfile
# - Pick a name for the image.  We'll go with "cse411"
# - Go to the folder where this Dockerfile exists
# - Type the following: docker build -t cse411 .
#   (note that the last period is part of the command)
#   (note that on Mac and Linux, you might need to prefix the command with 'sudo')

# To launch an instance of the cse411 container
# - You will need to be in PowerShell on Windows, or a terminal on MacOS/Linux
# - Determine the full path to the existing place on your hard disk where you will save your work
#   (for example /c/Users/yourname/Desktop/cse411 or /home/yourname/cse411)
#   (we will use /home/sue/cse411 for our example)
# - Type the following: docker run --privileged -v /home/sue/cse411:/root --name cse411dev -it cse411

# When your terminal starts up:
# - You will be logged in as root
# - Type 'cd /root' (or just 'cd') to switch to the root user's home folder
# - You should see your cse411 folder's contents in there

# When you are done:
# - Type docker rm cse411dev to clean up from your container, so you can start a new one with the same name.

# docker build -t mydocker .
# docker run --privileged -v ~/Desktop/cse411/docker/upload/test:/root --name mydocker_dev -it mydocker