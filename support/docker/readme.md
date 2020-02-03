### Generate Docker image (for bitbucket pipelines)
 - write Dockerfile
 - install ```sudo dnf -y install dnf-plugins-core```
 - install ```sudo dnf config-manager \
                --add-repo \
                https://download.docker.com/linux/fedora/docker-ce.repo```
 - install ```sudo dnf install docker-ce docker-ce-cli containerd.io```
 - for F31 ```sudo grubby --update-kernel=ALL --args="systemd.unified_cgroup_hierarchy=0"```
 - for F31 ```sudo reboot```
 - call ```sudo systemctl start docker```
 - call ```sudo docker build -t leanid/fedora_latest .```
 - call ```sudo docker login
 - call ```sudo docker push leanid/fedora_latest```
