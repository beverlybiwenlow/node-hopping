Configuring Reverse SSH between RPi and AWS Instance
====================================================

Big picture:
- The Pis are gateways that are set up all over the world, and are sitting behind firewalls that prevent remote servers from directly SSH-ing into them.
- Because of this, we need a AWS Instance (EC2) with a public address that has access to these Pis and that we can SSH into.
- For the Pis to give access to EC2, they have to set up a reverse SSH tunnel from within the firewall. This tunnel opens a local port on EC2 (eg. 2222) and listens on it, and any forwards access to the Pi on this port (eg. 2222). To access the Pi, we just need to access the port (eg. 2222) on EC2.

Steps:
1. Creation of the EC2 instance will give a ec2-public-ip and a private-key (PEM file).
2. On a terminal, SSH into EC2 using:
    ```
    ssh -i path-to-private-key ec2-user@ec2-public-ip
    ```
    where you replace _path-to-private-key_, _ec2-user_ and _ec2-public-ip_ with your own values. 
    (Default for _ec2-user_ is "ubuntu")
3. Within EC2 instance, run
    ```
    sudo nano /etc/ssh/sshd_config
    ```
    Then add/edit the line
    ```
    GatewayPorts yes
    ```
4. Reload ssh configuration with
    ```
    service ssh reload
    ```
5. On the Pi, ensure you have a copy of the private key.
6. Then edit SSH configuration with
    ```
    sudo nano ~/.ssh/config
    ```

    and insert:
    ```
    host *.amazonaws.com ec2-public-ip
    user ec2-user
    StrictHostKeyChecking no
    UserKnownHostsFile /dev/null
    CheckHostIP no
    ServerAliveInterval 120
    IdentityFile path-to-private-key-on-pi
    ```
    where you replace _ec2-public-ip_, _ec2-user_, _path-to-private-key-on-pi_ with your own values.
7. To auto start AutoSSH at reboot, run
    ```
    sudo nano /etc/rc.local
    ```
    Before exit 0, add the line:
    ```
    su pi-username -c '/usr/bin/autossh -M monitor-port:7 -fNR ec2-port:localhost:22 ec2-public-ip'
    ```
    where you replace _pi-username_, _monitor-port_, _ec2-port_ and _ec2-public-ip_ with your own values.
    (Default for _pi-username_ is "pi")
8. To use the reverse SSH tunnel, on EC2, run:
    ```
    ssh pi-username@localhost -p ec2-port
    ```
    where you replace _pi-username_ with your own value and _ec2-port_ with the port number you set in step 7.

Things to note:
- _ec2-port_ and _monitor-port_ have to be free on EC2 and cannot be used by any other Pi
- The -M tag in the autossh command is to set monitor-port. This port is separate from the ec2-port, and will monitor the status of the SSH and restart AutoSSH if things happen (connection gets cut/lost, etc). For example, if you specify "-M 20000", AutoSSH will set up so that it can send data on port 20000 and receive it back on the port above it 20001. Thus, both the monitor-port and the one above it have to be free and cannot be used for another Pi. To reduce the number of ports used, ":7" makes use of the echo port such that only monitor-port is used, and carries the message in both directions so that the one above it can be free.

Useful commands to debug:
- To check if reverse tunnel is set up:
    ```
    ps aux | grep ssh
    ```
    You will see a list of ssh connections. If the AutoSSH command is run in rc.local, then there will be 2 entries showing _ec-public-ip_.
- To kill the ssh tunnel, run
    ```
    kill -9 process-id
    ```
    where _process-id_ can be obtained from the list of ssh connections (2nd column). Run this command twice to kill both entries.