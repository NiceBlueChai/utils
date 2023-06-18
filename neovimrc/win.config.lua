print("Begin config lua")

local confPath = vim.fn.stdpath('config')
local dataPath = vim.fn.stdpath('data')
local homePath = vim.fn.expand('~')
local lazypath = vim.fn.stdpath("data") .. "\\lazy\\lazy.nvim"


os.execute("mkdir " .. confPath)
os.execute("mkdir " .. confPath .. "\\lua")
os.execute("mkdir " .. lazypath) 

local f = io.popen("copy /y " .. "init.vim " .. confPath, "r")
os.execute("xcopy /y /E " .. " lua " .. confPath .. "\\lua")
os.execute("xcopy /y /E " .. " lazy.nvim " .. lazypath)
os.execute("copy /y " .. ".vimrc " .. homePath)

print("End config lua")
