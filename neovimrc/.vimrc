
syntax enable
filetype plugin indent on
let mapleader = ","
let maplocalleader = "\\"
set number
set relativenumber
set shiftround
set shiftwidth=4
set exrc

iabbrev ssig -- <cr>NiceBlueChai<cr>bluechai@qq.com


nnoremap <Leader>k :resize +10<CR>
nnoremap <Leader>j :resize -10<CR>
nnoremap <Leader>l :vert resize +10<CR>
nnoremap <Leader>h :vert resize -10<CR>

nnoremap <Up> :resize +5<CR>
nnoremap <Down> :resize -5<CR>
nnoremap <Left> :vert resize -5<CR>
nnoremap <Right> :vert resize +5<CR>

inoremap <ESC> <NOP>
inoremap jk <ESC>

noremap <C-p> :GFiles<CR>

nnoremap <Leader>ev :vsplit ~/.vimrc<CR> 
nnoremap <Leader>sv :source $MYVIMRC<CR>

let g:python3_host_prog='C:/Users/NiceBlueChai\AppData\Local\Programs\Python\Python39\python.exe'
let g:python_host_prog='C:/Users/NiceBlueChai\AppData\Local\Programs\Python\Python39\python.exe'


" Vimscript file settings ---------------------- {{{
augroup filetype_vim
    autocmd!
    autocmd FileType vim setlocal foldmethod=marker
augroup END
" }}}



" vim -b : edit binary using xxd-format!
augroup Binary
  au!
  au BufReadPre  *.bin let &bin=1
  au BufReadPost *.bin if &bin | %!xxd
  au BufReadPost *.bin set ft=xxd | endif
  au BufWritePre *.bin if &bin | %!xxd -r
  au BufWritePre *.bin endif
  au BufWritePost *.bin if &bin | %!xxd
  au BufWritePost *.bin set nomod | endif
augroup END


function! GetCurBufPath()
  return expand('%:h')
endfunction

function! NasmBuild()
    if tolower(expand("%:e")) == "asm"
        execute "cd ".GetCurBufPath()
        let args = " ".expand('%:t') ." -o " .expand('%:r').".bin -l " .expand('%:r'). ".lst"
        execute "!nasm ".args
    else
        echom "Faild because this file not asm file"
    endif
endfunction    

command NasmBuild :call NasmBuild()
if has('win32')
    command Ex :execute "!explorer ".GetCurBufPath()
else
    command Ex :execute "!xdg-open ".GetCurBufPath()
endif
command CD :execute "cd ".GetCurBufPath()
