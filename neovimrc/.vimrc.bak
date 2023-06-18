"==============================================================================
" vim 内置配置 
"==============================================================================

" 设置 vimrc 修改保存后立刻生效，不用在重新打开
" 建议配置完成后将这个关闭
autocmd BufWritePost $MYVIMRC source $MYVIMRC
" 关闭兼容模式
set nocompatible


" Specify a directory for plugins
" - For Neovim: stdpath('data') . '/plugged'
" - Avoid using standard Vim directory names like 'plugin'
call plug#begin('~/.vim/plugged')

" Make sure you use single quotes


" Multiple Plug commands can be written in a single line using | separators
" Plug 'SirVer/ultisnips' | Plug 'honza/vim-snippets'
" Plug 'https://gitee.com/yaozhijin/ultisnips.git' 

" On-demand loading
" Plug 'scrooloose/nerdtree', { 'on':  'NERDTreeToggle' }
Plug 'https://gitee.com/fj-awei/nerdtree.git', { 'on': 'NERDTreeToggle' }
" Plug 'tpope/vim-fireplace', { 'for': 'clojure' }


" Using a non-default branch
" Plug 'rdnetto/YCM-Generator', { 'branch': 'stable' }
Plug 'https://gitee.com/ithewei/YCM-Generator.git', { 'branch': 'stable' }

" Using a tagged release; wildcard allowed (requires git 1.9.2 or above)
" Plug 'fatih/vim-go', { 'tag': '*' }

" Plugin options
" Plug 'nsf/gocode', { 'tag': 'v.20150303', 'rtp': 'vim' }

" Plugin outside ~/.vim/plugged with post-update hook
" Plug 'junegunn/fzf', { 'dir': '~/.fzf', 'do': './install --all' }
Plug 'https://gitee.com/yaozhijin/fzf.git', { 'dir': '~/.fzf', 'do': './install --all' }

Plug 'https://gitee.com/itl/vim-airline.git'
Plug 'https://gitee.com/itl/vim-airline-themes.git'
Plug 'rust-lang/rust.vim'
Plug 'https://github.com/mattn/webapi-vim'
" 有道词典
Plug 'https://github.com/ianva/vim-youdao-translater.git'


" go 主要插件
" Plug 'fatih/vim-go', { 'tag': '*' }
Plug 'https://gitee.com/misakasuna/vim-go.git', { 'do': ':GoUpdateBinaries' }
" go 中的代码追踪，输入 gd 就可以自动跳转
Plug 'dgryski/vim-godef'

" markdown 插件
Plug 'iamcco/mathjax-support-for-mkdp'
Plug 'iamcco/markdown-preview.vim'
" Unmanaged plugin (manually installed and updated)
" Plug '~/my-prototype-plugin'

call plug#end()

set backspace=indent,eol,start

" 有道词典
vnoremap <silent> <C-T> :<C-u>Ydv<CR>
nnoremap <silent> <C-T> :<C-u>Ydc<CR>
noremap <leader>yd :<C-u>Yde<CR>


" airline
set laststatus=2 "永远显示状态栏
let g:airline#extensions#tabline#enable=1 " 显示窗口tab和buffer

" rust.vim
" :RustPlay
" :RustTest 
let g:rust_clip_command = 'xclip -selection clipboard'
let g:rustfmt_autosave = 1

" NERDTreeToggle config
let mapleader = ","
map <F3> :NERDTreeToggle<CR>
map <Leader>n :NERDTreeFocus<CR>
map <Leader>S :source ~/.vimrc<CR>
 colorscheme desert
" colorscheme industry
" colorscheme koehler
" colorscheme slate
" colorscheme murphy


map <up> :resize +5<CR>
map <Down> :resize -5<CR>
map <Left> :vertial resize -5<CR>
map <Right> :vertial resize +5<CR>

syntax enable
filetype plugin indent on
set number
set relativenumber

set cursorline "突出显示当前行
" set cursorcolumn " 突出显示当前列
set showmatch " 显示括号匹配

" tab 缩进
set tabstop=4 " 设置Tab长度为4空格
set shiftwidth=4 " 设置自动缩进长度为4空格
set autoindent " 继承前一行的缩进方式，适用于多行注释

" ==== 系统剪切板复制粘贴 ====
" v 模式下复制内容到系统剪切板
vmap <Leader>c "+yy
" n 模式下复制一行到系统剪切板
nmap <Leader>c "+yy
" n 模式下粘贴系统剪切板的内容
nmap <Leader>v "+p

" 开启实时搜索
set incsearch
