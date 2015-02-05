#!/usr/bin/env ruby

Vagrant.configure("2") do |config|
    config.vm.box = "titon/nexus"
    config.vm.hostname = "titon"

    # Increase memory size as HHVM requires at minimum 1GB
    config.vm.provider "virtualbox" do |v|
        v.name = "titon"
        v.memory = 2048
        v.cpus = 2
        v.customize ["modifyvm", :id, "--ostype", "Ubuntu_64"]
    end

    # Use a custom IP
    config.vm.network "private_network", ip: "192.168.73.31"
end
