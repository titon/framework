#!/usr/bin/env ruby

Vagrant.configure("2") do |config|
    config.vm.box = "titon/nexus"
    config.vm.hostname = "titon"

    # Increase memory size as HHVM requires at minimum 1GB
    config.vm.provider "virtualbox" do |v|
        v.memory = 2048
        v.cpus = 2
    end
end
