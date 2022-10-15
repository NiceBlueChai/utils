#!/bin/sh

mkdir -p ~/.config/nvim
cp ./init.vim ~/.config/nvim/init.vim
cp nvimrc ~/.vimrc
cp -r lua ~/.config/nvim/

# install packer as plugin manager
#git clone --depth 1 https://github.com/wbthomason/packer.nvim\
# ~/.local/share/nvim/site/pack/packer/start/packer.nvim
