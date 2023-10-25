#!/bin/bash
: '
""" Configure local python2
    Version 1.0    30/10/2023
    Authors: Geancarlo Abich - abich@ieee.org
"""
'

wget http://www.python.org/ftp/python/2.7.18/Python-2.7.18.tgz
tar -xvf Python-2.7.18.tgz
cd Python-2.7.18
mkdir ~/.localpython
./configure --prefix=/home/${USER}/.localpython
make
make install
cd ..
rm -rf Python-2.7.18
wget https://pypi.python.org/packages/source/v/virtualenv/virtualenv-1.9.tar.gz
tar -xvf virtualenv-1.9.tar.gz /home/${USER}/virtualenv-1.9
cd /home/${USER}/virtualenv-1.9
~/.localpython/bin/python setup.py install
#This is to activate virtualenv
#~/.localpython/bin/python2 virtualenv.py virtualenv_py2 -p /home/${USER}/.localpython/bin/python2.7
#source ~/virtualenv-1.9/virtualenv_py2/bin/activate
cd -
echo "export PATH=/home/${USER}/.localpython/bin:${PATH}" >> ~/.bashrc
source ~/.bashrc
#pip2 install --user pp
#wget https://www.parallelpython.com/downloads/pp/pp-1.6.6.tar.gz
#tar -xvf pp-1.6.6.tar.gz
#rm pp-1.6.6.tar.gz
cd pp-1.6.6
python2 setup.py install
cd ..
