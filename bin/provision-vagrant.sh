#!/usr/bin/env bash

# Update box
sudo apt-get update
sudo apt-get -y upgrade

# Install packages
sudo apt-get -y install git curl vim

# Install HHVM
wget -O - http://dl.hhvm.com/conf/hhvm.gpg.key | sudo apt-key add -
echo deb http://dl.hhvm.com/ubuntu trusty main | sudo tee /etc/apt/sources.list.d/hhvm.list
sudo apt-get update
sudo apt-get -y install hhvm-nightly
sudo /usr/bin/update-alternatives --install /usr/bin/php php /usr/bin/hhvm 60

# Install Composer
wget -O /vagrant/composer.phar https://getcomposer.org/composer.phar
echo "alias composer='hhvm -v ResourceLimit.SocketDefaultTimeout=30 -v Http.SlowQueryThreshold=30000 /vagrant/composer.phar'" >> ~/.bashrc
source ~/.bashrc

# Install MySQL & PGSQL
#DEBIAN_FRONTEND=noninteractive sudo apt-get -y install mysql-server mysql-client sqlite3 postgresql

#mysql -u root -e "CREATE DATABASE titon_test;"
#psql -U postgres -c "CREATE DATABASE titon_test;"

# Install Redis & Memcache
#sudo apt-get -y install redis-server memcached