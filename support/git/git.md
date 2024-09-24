# команды для git которые я постоянно забываю
## Windows
1. нужно сразу включить OpenSSH службу (PowerShell admin): 
Get-Service ssh-agent | Set-Service -StartupType Automatic
Start-Service ssh-agent
Get-Service ssh-agent
ssh-add
2. restart and check everything is working in new Terminal:
ssh-add -l (should print list of all your added keys before)
3. now add Windows ssh programm to git config like:
git config --global core.sshCommand "C:/Windows/System32/OpenSSH/ssh.exe"
git config core.sshCommand
