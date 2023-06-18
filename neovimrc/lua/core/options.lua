vim.g.mapleader = ","
vim.g.maplocalleader = ","

local opt = vim.opt

opt.autowrite = true
opt.cursorline = true

-- use mouse
opt.mouse = "a"

-- system clipboard
opt.clipboard = "unnamedplus"

-- 默认新窗口右和下
opt.splitright = true
opt.splitbelow = true

-- 搜索
opt.ignorecase = true
opt.smartcase = true

-- 外观
opt.termguicolors =true
opt.signcolumn = "yes"


opt.wrap = false -- 禁用换行
