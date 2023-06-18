print("Begin config lua")

local confPath = vim.fn.stdpath('config')
local dataPath = vim.fn.stdpath('data')
local homePath = vim.fn.expand('~')
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"


os.execute("mkdir -p " .. confPath)
os.execute("mkdir -p " .. confPath .. "/lua")
os.execute("mkdir -p " .. lazypath) 

local f = io.popen("cp -rf " .. "init.vim " .. confPath, "r")
os.execute("cp -rf " .. " lua/* " .. confPath .. "/lua")
os.execute("cp -rf " .. " lazy.nvim/* " .. lazypath)
os.execute("cp -rf " .. ".vimrc " .. homePath)

print("End config lua")
