set runtimepath^=~/.vim runtimepath+=~/.vim/after
let &packpath = &runtimepath
source ~/.vimrc

lua require('basic')
lua require('plugins')

lua require("nvim-lsp-installer").setup{}

echo ">^.^<"
