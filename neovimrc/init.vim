set runtimepath^=~/.vim runtimepath+=~/.vim/after
let &packpath = &runtimepath
source ~/.vimrc


" lua require('basic')
" lua require('plugins')

" lua require("nvim-lsp-installer").setup{}

lua << EOF
    require("core.options")
    require("plugins-setup")

--[[    local capabilities = require('cmp_nvim_lsp').default_capabilities()

require("lspconfig").clangd.setup {
  capabilities = capabilities,
}
--]]
EOF

echo ">^.^<"
