execute pathogen#infect()
filetype plugin indent on
set nonu
set tabstop=4       "tab键长度
set softtabstop=4   "删除时长度
set shiftwidth=4    "自动tab长度
set expandtab       "禁用tab 用空格代替
set cindent         "C语言专用tab
colorscheme darkblue

"nnoremap i k
"nnoremap k j
"nnoremap j h
"nnoremap h i
"nnoremap H I

nnoremap mm )
nnoremap mn (
nnoremap mh 0
nnoremap mk %
nnoremap me $

inoremap <C-d> <del>
nnoremap <C-c> <ESC>:w
inoremap <C-b> <Left>
inoremap <C-j> <Down>
inoremap <C-k> <Up>
inoremap <C-l> <Right>
hi MatchParen ctermbg=blue ctermfg=white

let g:user_emmet_leader_key='<C-Z>'
filetype indent on

