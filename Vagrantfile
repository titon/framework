#!/usr/bin/env ruby
# https://stefanwrobel.com/how-to-make-vagrant-performance-not-suck

Vagrant.configure("2") do |config|
    config.vm.box = "titon/nexus"
    config.vm.hostname = "titon"

    # Determine the best memory and CPU options
    host = RbConfig::CONFIG['host_os']

    if host =~ /darwin/
        cpus = `sysctl -n hw.ncpu`.to_i
        memory = `sysctl -n hw.memsize`.to_i / 1024 / 1024 / 4
    elsif host =~ /linux/
        cpus = `nproc`.to_i
        memory = `grep 'MemTotal' /proc/meminfo | sed -e 's/MemTotal://' -e 's/ kB//'`.to_i / 1024 / 4
    else
        cpus = 4
        memory = 4096
    end

    # Increase memory size as HHVM requires at minimum 1GB
    config.vm.provider "virtualbox" do |vb|
        vb.name = "titon"
        vb.customize ["modifyvm", :id, "--ostype", "Ubuntu_64"]
        vb.customize ["modifyvm", :id, "--ioapic", "on"]
        vb.customize ["modifyvm", :id, "--cpus", cpus]
        vb.customize ["modifyvm", :id, "--memory", memory]
    end

    # Use a custom IP
    config.vm.network "private_network", ip: "192.168.73.31"

    # Use NFS for better performance
    config.vm.synced_folder ".", "/vagrant", nfs: true
end
