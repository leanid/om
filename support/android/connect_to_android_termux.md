# How someone can easilly connect to Android phone Termux application with ssh

## Install Termux application
## Start Termux and do basic preparetions like:
```sh
pkg up 
pkg install openssh 
sshd
```
## Set password for your user in Termux application 
```sh
passwd 
```
## Find out your Termux user name: 
```sh
whoami
```
## Find out your Android device ip in local WiFi network 
```sh
ifconfig
```
## How you can connect to your Termux application with password like: 
```sh
ssh -p 8022 <user>@<ip>
```
## Better copy your ~/.ssh/id_rsa.pub to Termux ~/.ssh/authorized_keys 
```sh
ssh-copy-id -i ~/.ssh/id_rsa.pub -p 8022 u0_a393@192.168.100.13
```
## If you like to disable password authorization for sshd in Termux 
```sh
vim ../usr/etc/ssh/sshd_config 
```
and find line 'PasswordAuthontication yes' and rewrite it to 'no'
now
```sh
pkill sshd
sshd
```

