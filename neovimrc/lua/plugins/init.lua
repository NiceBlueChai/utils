
local lsp = require("plugins.lsp")

return {
    {
    "folke/which-key.nvim",
    event = "VeryLazy",
    init = function()
	vim.o.timeout = true
	vim.o.timeoutlen = 300
    end,
    opts = {
	-- your configuration comes here
	-- or leave it empty to use the default settings
	-- refer to the configuration section below
	}
    },
    {
	"nvim-neo-tree/neo-tree.nvim",
	branch = "v2.x",
	keys ={
	    { "<leader>ft", "<cmd>NeoTreeShowToggle<cr>", desc = "NeoTree" },
	    { "<F3>", "<cmd>NeoTreeFocusToggle<cr>", desc = "NeoTree" },
	},
	config = function() 
	    require("neo-tree").setup({
		close_if_last_window = true,
		enable_git_status = true,
		default_component_configs = {
		    git_status = {
		    symbols = {
			-- Change type
			added     = "", -- or "✚", but this is redundant info if you use git_status_colors on the name
			modified  = "", -- or "", but this is redundant info if you use git_status_colors on the name
			deleted   = "×",-- this can only be used in the git_status source
			renamed   = "",-- this can only be used in the git_status source
			-- Status type
			untracked = "",
			ignored   = "",
			unstaged  = "",
			staged    = "",
			conflict  = "*",
			},
		    },
		},
	    })
	end,
	dependencies = {
	    "nvim-lua/plenary.nvim",
	    "nvim-tree/nvim-web-devicons", -- not strictly required, but recommended
	    "MunifTanjim/nui.nvim",
	},
    },
    lsp
}
