imap jk <Esc>:w<CR>
cnoreabbrev nt tabnew
nmap gr gT
set laststatus=2
set showtabline=2
set autoindent
syntax on
set number
set incsearch
set ignorecase
set smartcase
set pastetoggle=<F2>
imap <S-Tab> <Esc><<i
nmap <S-Tab> <<
nmap <Tab> >>
 
set expandtab
set tabstop=4
set softtabstop=4
set shiftwidth=4
set autoindent
 
function! Tab_Or_Complete()
    if col('.')>1 && strpart( getline('.'), col('.')-2, 3 ) =~ '^\w'
        return "\<C-N>"
    else
        return "\<Tab>"
    endif
endfunction
inoremap <Tab> <C-R>=Tab_Or_Complete()<CR>
set complete=""
set complete+=.
set complete+=k
set complete+=b
set complete+=t

colorscheme desert

