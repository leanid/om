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

1. do:
``` bash
sudo vim /etc/nginx/conf.d/default.conf
```
end change `listen 80;` to `listen 81` (in case you can't use default 80 port)
2. restart nginx
``` bash
sudo services nginx restart
```
3. go to you browser and open `localhost:81`. You should see default welcome nginx page.


# nginx problems
1. check 80 port before start listening it (if using WSL check on windows too)
   on Windows you can run Resource Monitor as Admin and tap Listening ports
2. on Linux to see all listening ports use
   `sudo netstat -tunlp`

# minimal FastCGI nginx configuration

``` nginx
server { 
    listen 82;
    server_name localhost; 

    location / { 
        fastcgi_pass 127.0.0.1:9000; 
        #fastcgi_pass  unix:/tmp/fastcgi/mysocket; 
        #fastcgi_pass localhost:9000; 
         
        include fastcgi_params; 
    } 
}
```
