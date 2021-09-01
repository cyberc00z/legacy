"remembering cursor position between session
set viminfo='10,\"100,:20,%,n~/.viminfo

function! ResCur()
   if line("'\"") <= line("$")
      normal! g`"
      return 1
   endif
endfunction

augroup resCur
    autocmd!
    autocmd BufWinEnter * call ResCur()
augroup END

"addressing vim scheme

syntax on
set number
set nowrapscan
set autoindent
set tabstop=4
set tags=./tags 
set expandtab
set shiftwidth=4
set smartcase
set ruler
set noswapfile
map <tab> <C-w><C-w>
set wildmode=longest,list,full
set wildmenu
colorscheme murphy

"underline spell check
hi clear SpellBad
hi SpellBad cterm=underline

"use tabs in linux kernal code
au BufNewFile,BufRead /*kernal* set tabstop=8
au BufNewFile,BufRead /*kernal* set shiftwidth=8
au BufNewFile,BufRead /*kernal* set noexpandtab
