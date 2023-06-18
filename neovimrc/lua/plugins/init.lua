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
	    { "<leader>ft", "<cmd>NeoTree toggle<cr>", desc = "NeoTree" },
	},
	config = function() 
	    require("neo-tree").setup()
	end,
	dependencies = {
	    "nvim-lua/plenary.nvim",
	    "nvim-tree/nvim-web-devicons", -- not strictly required, but recommended
	    "MunifTanjim/nui.nvim",
	},
    },
}
