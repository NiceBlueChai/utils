
require'lsp/nvim-cmp'
require'keybindings'
require('lspconfig').clangd.setup{cmd={"/home/nice/.local/share/nvim/lsp_servers/clangd/clangd/bin/clangd",};}
