# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/trusty64"

  # Uncomment this once the provision script has been fully tested
  config.vm.provision :shell, path: "bin/provision-vagrant.sh"

  # Increase memory size as HHVM requires at minimum 1GB
  config.vm.provider "virtualbox" do |v|
    v.memory = 2048
    v.cpus = 2
  end
end
