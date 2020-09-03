# fork-xinu

This a xinu implementation that works with virtual box. Visit [ftp://ftp.cs.purdue.edu/pub/comer/private/Xinu/xinuvbox-appliances.tar.gz] to download the dev machine 
and the backend machine. This repository adds a unix based fork to xinu where, the parent process can fork off and create a child process. The parent process is retured with process id 
of the child process and the child returns from fork with the value NPROC where NPROC is the maximum number of avaiable processes in XINU and a value none of the
process would ever take. 

After installing xinu, using the link mentioned earlier, replace the files in the xinu directory with the files in this repository. (Note: Not all files are modified)
Run make in the compile folder, and then run sudo minicom to establish a connection between the dev machine and the backup machine. Run the backend machine and based on 
the main function, certain functions would be called or shell would be created. The current main.c file inside the system folder has test cases for fork. Rename main.1 to main.c 
to get the shell and run shell commands. 

Please reach out to me incase you want to get more details on the installation process. 

To know more about xinu, visit [https://xinu.cs.purdue.edu/]
