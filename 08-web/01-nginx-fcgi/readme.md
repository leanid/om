# install nginx
## on Ubuntu
Go to https://nginx.org/ru/linux_packages.html#Ubuntu and do it
# start stop nginx like:

```bash
# what is the status
sudo systemctl status nginx
# start
sudo systemctl start nginx
# stop
sudo systemctl stop nginx
# reload (reload configuration files)
sudo systemctl reload nginx
```
# How to start with nginx (FastCGI)
https://nginx.org/ru/docs/beginners_guide.html

# nginx problems
1. check 80 port before start listening it (if using WSL check on windows too)
   on Windows you can run Resource Monigor as Admin and tap Listening ports
2. on Linux to see all listening ports use
   sudo netstat -tunlp
