let SessionLoad = 1
if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
inoremap <silent> <expr> <Plug>(neocomplcache_start_unite_complete) unite#sources#neocomplcache#start_complete()
inoremap <silent> <expr> <Plug>(neocomplcache_start_unite_quick_match) unite#sources#neocomplcache#start_quick_match()
inoremap <silent> <Plug>(neocomplcache_start_auto_complete) =neocomplcache#mappings#popup_post()
inoremap <silent> <Plug>(neocomplcache_start_auto_complete_no_select) 
inoremap <silent> <Plug>(neocomplcache_start_omni_complete) 
inoremap <silent> <S-Tab> =BackwardsSnippet()
inoremap <silent> <Plug>NERDCommenterInsert  <BS>:call NERDComment('i', "insert")
inoremap <Plug>(emmet-anchorize-summary) =emmet#util#closePopup()=emmet#anchorizeURL(1)
inoremap <Plug>(emmet-anchorize-url) =emmet#util#closePopup()=emmet#anchorizeURL(0)
inoremap <Plug>(emmet-remove-tag) =emmet#util#closePopup()=emmet#removeTag()
inoremap <Plug>(emmet-split-join-tag) :call emmet#splitJoinTag()
inoremap <Plug>(emmet-toogle-comment) =emmet#util#closePopup()=emmet#toggleComment()
inoremap <Plug>(emmet-image-size) =emmet#util#closePopup()=emmet#imageSize()
inoremap <Plug>(emmet-move-prev) :call emmet#moveNextPrev(1)
inoremap <Plug>(emmet-move-next) :call emmet#moveNextPrev(0)
inoremap <Plug>(emmet-balance-tag-outword) :call emmet#balanceTag(-1)
inoremap <Plug>(emmet-balance-tag-inward) :call emmet#balanceTag(1)
inoremap <Plug>(emmet-update-tag) =emmet#util#closePopup()=emmet#updateTag()
inoremap <Plug>(emmet-expand-word) =emmet#util#closePopup()=emmet#expandAbbr(1,"")
inoremap <Plug>(emmet-expand-abbr) =emmet#util#closePopup()=emmet#expandAbbr(0,"")
imap <C-F10> :call Link()
imap <C-F9> :call Compile()
imap <F9> :call Run()
noremap  h
snoremap <silent> 	 i<Right>=TriggerSnippet()
noremap <NL> j
noremap  k
noremap  l
nnoremap <silent>  :CtrlP
nnoremap <silent>  :call repeat#wrap("\<C-R>",v:count)
nmap o <Plug>ZoomWin
snoremap  b<BS>
vmap c <Plug>(emmet-code-pretty)
vmap m <Plug>(emmet-merge-lines)
nmap A <Plug>(emmet-anchorize-summary)
nmap a <Plug>(emmet-anchorize-url)
nmap k <Plug>(emmet-remove-tag)
nmap j <Plug>(emmet-split-join-tag)
nmap / <Plug>(emmet-toogle-comment)
nmap i <Plug>(emmet-image-size)
nmap N <Plug>(emmet-move-prev)
nmap n <Plug>(emmet-move-next)
vmap D <Plug>(emmet-balance-tag-outword)
nmap D <Plug>(emmet-balance-tag-outword)
vmap d <Plug>(emmet-balance-tag-inward)
nmap d <Plug>(emmet-balance-tag-inward)
nmap u <Plug>(emmet-update-tag)
nmap ; <Plug>(emmet-expand-word)
vmap , <Plug>(emmet-expand-abbr)
nmap , <Plug>(emmet-expand-abbr)
nmap d :cs find d =expand("<cword>")
nmap i :cs find i ^=expand("<cfile>")$
nmap f :cs find f =expand("<cfile>")
nmap e :cs find e =expand("<cword>")
nmap t :cs find t =expand("<cword>")
nmap c :cs find c =expand("<cword>")
nmap g :cs find g =expand("<cword>")
nmap s :cs find s =expand("<cword>")
nnoremap   @=((foldclosed(line('.')) < 0) ? 'zc' : 'zo')
vnoremap <silent> # y?=substitute(escape(@", '.*\\/[]'), "\n", '\\n', 'g')
nmap # <Plug>MarkSearchPrev
snoremap % b<BS>%
snoremap ' b<BS>'
vnoremap <silent> * y/=substitute(escape(@", '.*\\/[]'), "\n", '\\n', 'g')
nmap * <Plug>MarkSearchNext
nnoremap <silent> . :call repeat#run(v:count)
nmap ;cw :cw 10
xmap ;cn <Plug>NERDCommenterNested
nmap ;ca <Plug>NERDCommenterAltDelims
xmap ;cu <Plug>NERDCommenterUncomment
nmap ;cu <Plug>NERDCommenterUncomment
xmap ;cb <Plug>NERDCommenterAlignBoth
nmap ;cb <Plug>NERDCommenterAlignBoth
xmap ;cl <Plug>NERDCommenterAlignLeft
nmap ;cA <Plug>NERDCommenterAppend
xmap ;cy <Plug>NERDCommenterYank
nmap ;cy <Plug>NERDCommenterYank
xmap ;cs <Plug>NERDCommenterSexy
nmap ;cs <Plug>NERDCommenterSexy
xmap ;ci <Plug>NERDCommenterInvert
nmap ;ci <Plug>NERDCommenterInvert
nmap ;c$ <Plug>NERDCommenterToEOL
xmap ;cm <Plug>NERDCommenterMinimal
nmap ;cm <Plug>NERDCommenterMinimal
xmap ;c  <Plug>NERDCommenterToggle
nmap ;c  <Plug>NERDCommenterToggle
xmap ;cc <Plug>NERDCommenterComment
nmap ;cc <Plug>NERDCommenterComment
nmap ;? <Plug>MarkSearchAnyPrev
nmap ;/ <Plug>MarkSearchAnyNext
nmap ;# <Plug>MarkSearchCurrentPrev
nmap ;* <Plug>MarkSearchCurrentNext
nmap ;n <Plug>MarkClear
xmap ;r <Plug>MarkRegex
nmap ;r <Plug>MarkRegex
xmap ;m <Plug>MarkSet
nmap ;m <Plug>MarkSet
map ;sc :call ConfigSymbs()  
map ;sy :call SyncSource()   
map ;rwp <Plug>RestoreWinPosn
map ;swp <Plug>SaveWinPosn
vmap <silent> ;tt :call AlignMaps#Vis("tt")
nmap ;tt <Plug>AM_tt
vmap <silent> ;tsq :call AlignMaps#Vis("tsq")
nmap ;tsq <Plug>AM_tsq
vmap <silent> ;tsp :call AlignMaps#Vis("tsp")
nmap ;tsp <Plug>AM_tsp
vmap <silent> ;tml :call AlignMaps#Vis("tml")
nmap ;tml <Plug>AM_tml
vmap <silent> ;tab :call AlignMaps#Vis("tab")
nmap ;tab <Plug>AM_tab
vmap <silent> ;m= :call AlignMaps#Vis("m=")
nmap ;m= <Plug>AM_m=
vmap <silent> ;tW@ :call AlignMaps#Vis("tW@")
nmap ;tW@ <Plug>AM_tW@
vmap <silent> ;t@ :call AlignMaps#Vis("t@")
nmap ;t@ <Plug>AM_t@
vmap <silent> ;t~ :call AlignMaps#Vis("t~")
nmap ;t~ <Plug>AM_t~
vmap <silent> ;t? :call AlignMaps#Vis("t?")
nmap ;t? <Plug>AM_t?
vmap <silent> ;w= :call AlignMaps#Vis("w=")
nmap ;w= <Plug>AM_w=
vmap <silent> ;ts= :call AlignMaps#Vis("ts=")
nmap ;ts= <Plug>AM_ts=
vmap <silent> ;ts< :call AlignMaps#Vis("ts<")
nmap ;ts< <Plug>AM_ts<
vmap <silent> ;ts; :call AlignMaps#Vis("ts;")
nmap ;ts; <Plug>AM_ts;
vmap <silent> ;ts: :call AlignMaps#Vis("ts:")
nmap ;ts: <Plug>AM_ts:
vmap <silent> ;ts, :call AlignMaps#Vis("ts,")
nmap ;ts, <Plug>AM_ts,
vmap <silent> ;t= :call AlignMaps#Vis("t=")
nmap ;t= <Plug>AM_t=
vmap <silent> ;t< :call AlignMaps#Vis("t<")
nmap ;t< <Plug>AM_t<
vmap <silent> ;t; :call AlignMaps#Vis("t;")
nmap ;t; <Plug>AM_t;
vmap <silent> ;t: :call AlignMaps#Vis("t:")
nmap ;t: <Plug>AM_t:
vmap <silent> ;t, :call AlignMaps#Vis("t,")
nmap ;t, <Plug>AM_t,
vmap <silent> ;t# :call AlignMaps#Vis("t#")
nmap ;t# <Plug>AM_t#
vmap <silent> ;t| :call AlignMaps#Vis("t|")
nmap ;t| <Plug>AM_t|
vmap <silent> ;T~ :call AlignMaps#Vis("T~")
nmap ;T~ <Plug>AM_T~
vmap <silent> ;Tsp :call AlignMaps#Vis("Tsp")
nmap ;Tsp <Plug>AM_Tsp
vmap <silent> ;Tab :call AlignMaps#Vis("Tab")
nmap ;Tab <Plug>AM_Tab
vmap <silent> ;TW@ :call AlignMaps#Vis("TW@")
nmap ;TW@ <Plug>AM_TW@
vmap <silent> ;T@ :call AlignMaps#Vis("T@")
nmap ;T@ <Plug>AM_T@
vmap <silent> ;T? :call AlignMaps#Vis("T?")
nmap ;T? <Plug>AM_T?
vmap <silent> ;T= :call AlignMaps#Vis("T=")
nmap ;T= <Plug>AM_T=
vmap <silent> ;T< :call AlignMaps#Vis("T<")
nmap ;T< <Plug>AM_T<
vmap <silent> ;T; :call AlignMaps#Vis("T;")
nmap ;T; <Plug>AM_T;
vmap <silent> ;T: :call AlignMaps#Vis("T:")
nmap ;T: <Plug>AM_T:
vmap <silent> ;Ts, :call AlignMaps#Vis("Ts,")
nmap ;Ts, <Plug>AM_Ts,
vmap <silent> ;T, :call AlignMaps#Vis("T,")
nmap ;T, <Plug>AM_T,
vmap <silent> ;T# :call AlignMaps#Vis("T#")
nmap ;T# <Plug>AM_T#
vmap <silent> ;T| :call AlignMaps#Vis("T|")
nmap ;T| <Plug>AM_T|
map ;Htd <Plug>AM_Htd
vmap <silent> ;anum :call AlignMaps#Vis("anum")
nmap ;anum <Plug>AM_anum
vmap <silent> ;aenum :call AlignMaps#Vis("aenum")
nmap ;aenum <Plug>AM_aenum
vmap <silent> ;aunum :call AlignMaps#Vis("aunum")
nmap ;aunum <Plug>AM_aunum
vmap <silent> ;afnc :call AlignMaps#Vis("afnc")
nmap ;afnc <Plug>AM_afnc
vmap <silent> ;adef :call AlignMaps#Vis("adef")
nmap ;adef <Plug>AM_adef
vmap <silent> ;adec :call AlignMaps#Vis("adec")
nmap ;adec <Plug>AM_adec
vmap <silent> ;ascom :call AlignMaps#Vis("ascom")
nmap ;ascom <Plug>AM_ascom
vmap <silent> ;aocom :call AlignMaps#Vis("aocom")
nmap ;aocom <Plug>AM_aocom
vmap <silent> ;adcom :call AlignMaps#Vis("adcom")
nmap ;adcom <Plug>AM_adcom
vmap <silent> ;acom :call AlignMaps#Vis("acom")
nmap ;acom <Plug>AM_acom
vmap <silent> ;abox :call AlignMaps#Vis("abox")
nmap ;abox <Plug>AM_abox
vmap <silent> ;a( :call AlignMaps#Vis("a(")
nmap ;a( <Plug>AM_a(
vmap <silent> ;a= :call AlignMaps#Vis("a=")
nmap ;a= <Plug>AM_a=
vmap <silent> ;a< :call AlignMaps#Vis("a<")
nmap ;a< <Plug>AM_a<
vmap <silent> ;a, :call AlignMaps#Vis("a,")
nmap ;a, <Plug>AM_a,
vmap <silent> ;a? :call AlignMaps#Vis("a?")
nmap ;a? <Plug>AM_a?
nmap ;ihn :IHN
nmap ;is :IHS:A
nmap ;ih :IHS
nmap ;il :IndentLinesToggle
nmap ;sch :AS
nmap ;ch :A
nmap <silent> ;wm :WMToggle 
nmap <silent> ;wd :BottomExplorerWindow
nmap <silent> ;wb :FirstExplorerWindow
nmap ;cl <Plug>NERDCommenterAlignLeft
nmap ;co :copen
nmap ;cp :cp
nmap ;cn :cn
map <silent> ;ee :e ~/.vimrc
map <silent> ;ss :source ~/.vimrc
xmap S <Plug>VSurround
nnoremap <silent> U :call repeat#wrap('U',v:count)
snoremap U b<BS>U
snoremap \ b<BS>\
snoremap ^ b<BS>^
snoremap ` b<BS>`
nmap cs <Plug>Csurround
nmap cM :%s/\r$//g:noh
nmap cS :%s/\s\+$//g:noh
nmap ds <Plug>Dsurround
nmap gx <Plug>NetrwBrowseX
xmap gS <Plug>VgSurround
nmap tl :TagbarClose:Tlist
nmap tb :TlistClose:TagbarToggle
nnoremap <silent> u :call repeat#wrap('u',v:count)
nmap ySS <Plug>YSsurround
nmap ySs <Plug>YSsurround
nmap yss <Plug>Yssurround
nmap yS <Plug>YSurround
nmap ys <Plug>Ysurround
nnoremap <silent> <Plug>NERDCommenterNested :call NERDComment("n", "Nested")
xnoremap <silent> <Plug>NERDCommenterNested :call NERDComment("x", "Nested")
snoremap <Left> bi
snoremap <Right> a
snoremap <BS> b<BS>
snoremap <silent> <S-Tab> i<Right>=BackwardsSnippet()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
nnoremap <silent> <Plug>SurroundRepeat .
xnoremap <silent> <Plug>NERDCommenterUncomment :call NERDComment("x", "Uncomment")
nnoremap <silent> <Plug>NERDCommenterUncomment :call NERDComment("n", "Uncomment")
xnoremap <silent> <Plug>NERDCommenterAlignBoth :call NERDComment("x", "AlignBoth")
nnoremap <silent> <Plug>NERDCommenterAlignBoth :call NERDComment("n", "AlignBoth")
xnoremap <silent> <Plug>NERDCommenterAlignLeft :call NERDComment("x", "AlignLeft")
nnoremap <silent> <Plug>NERDCommenterAlignLeft :call NERDComment("n", "AlignLeft")
nnoremap <silent> <Plug>NERDCommenterAppend :call NERDComment("n", "Append")
xnoremap <silent> <Plug>NERDCommenterYank :call NERDComment("x", "Yank")
nnoremap <silent> <Plug>NERDCommenterYank :call NERDComment("n", "Yank")
xnoremap <silent> <Plug>NERDCommenterSexy :call NERDComment("x", "Sexy")
nnoremap <silent> <Plug>NERDCommenterSexy :call NERDComment("n", "Sexy")
xnoremap <silent> <Plug>NERDCommenterInvert :call NERDComment("x", "Invert")
nnoremap <silent> <Plug>NERDCommenterInvert :call NERDComment("n", "Invert")
nnoremap <silent> <Plug>NERDCommenterToEOL :call NERDComment("n", "ToEOL")
xnoremap <silent> <Plug>NERDCommenterMinimal :call NERDComment("x", "Minimal")
nnoremap <silent> <Plug>NERDCommenterMinimal :call NERDComment("n", "Minimal")
xnoremap <silent> <Plug>NERDCommenterToggle :call NERDComment("x", "Toggle")
nnoremap <silent> <Plug>NERDCommenterToggle :call NERDComment("n", "Toggle")
xnoremap <silent> <Plug>NERDCommenterComment :call NERDComment("x", "Comment")
nnoremap <silent> <Plug>NERDCommenterComment :call NERDComment("n", "Comment")
nmap <C-k9> <Plug>MarkSearchGroup9Prev
nnoremap <silent> <Plug>MarkSearchGroup9Prev :call mark#SearchGroupMark(9, v:count1, 1, 1)
nmap <k9> <Plug>MarkSearchGroup9Next
nnoremap <silent> <Plug>MarkSearchGroup9Next :call mark#SearchGroupMark(9, v:count1, 0, 1)
nmap <C-k8> <Plug>MarkSearchGroup8Prev
nnoremap <silent> <Plug>MarkSearchGroup8Prev :call mark#SearchGroupMark(8, v:count1, 1, 1)
nmap <k8> <Plug>MarkSearchGroup8Next
nnoremap <silent> <Plug>MarkSearchGroup8Next :call mark#SearchGroupMark(8, v:count1, 0, 1)
nmap <C-k7> <Plug>MarkSearchGroup7Prev
nnoremap <silent> <Plug>MarkSearchGroup7Prev :call mark#SearchGroupMark(7, v:count1, 1, 1)
nmap <k7> <Plug>MarkSearchGroup7Next
nnoremap <silent> <Plug>MarkSearchGroup7Next :call mark#SearchGroupMark(7, v:count1, 0, 1)
nmap <C-k6> <Plug>MarkSearchGroup6Prev
nnoremap <silent> <Plug>MarkSearchGroup6Prev :call mark#SearchGroupMark(6, v:count1, 1, 1)
nmap <k6> <Plug>MarkSearchGroup6Next
nnoremap <silent> <Plug>MarkSearchGroup6Next :call mark#SearchGroupMark(6, v:count1, 0, 1)
nmap <C-k5> <Plug>MarkSearchGroup5Prev
nnoremap <silent> <Plug>MarkSearchGroup5Prev :call mark#SearchGroupMark(5, v:count1, 1, 1)
nmap <k5> <Plug>MarkSearchGroup5Next
nnoremap <silent> <Plug>MarkSearchGroup5Next :call mark#SearchGroupMark(5, v:count1, 0, 1)
nmap <C-k4> <Plug>MarkSearchGroup4Prev
nnoremap <silent> <Plug>MarkSearchGroup4Prev :call mark#SearchGroupMark(4, v:count1, 1, 1)
nmap <k4> <Plug>MarkSearchGroup4Next
nnoremap <silent> <Plug>MarkSearchGroup4Next :call mark#SearchGroupMark(4, v:count1, 0, 1)
nmap <C-k3> <Plug>MarkSearchGroup3Prev
nnoremap <silent> <Plug>MarkSearchGroup3Prev :call mark#SearchGroupMark(3, v:count1, 1, 1)
nmap <k3> <Plug>MarkSearchGroup3Next
nnoremap <silent> <Plug>MarkSearchGroup3Next :call mark#SearchGroupMark(3, v:count1, 0, 1)
nmap <C-k2> <Plug>MarkSearchGroup2Prev
nnoremap <silent> <Plug>MarkSearchGroup2Prev :call mark#SearchGroupMark(2, v:count1, 1, 1)
nmap <k2> <Plug>MarkSearchGroup2Next
nnoremap <silent> <Plug>MarkSearchGroup2Next :call mark#SearchGroupMark(2, v:count1, 0, 1)
nmap <C-k1> <Plug>MarkSearchGroup1Prev
nnoremap <silent> <Plug>MarkSearchGroup1Prev :call mark#SearchGroupMark(1, v:count1, 1, 1)
nmap <k1> <Plug>MarkSearchGroup1Next
nnoremap <silent> <Plug>MarkSearchGroup1Next :call mark#SearchGroupMark(1, v:count1, 0, 1)
nnoremap <silent> <Plug>MarkSearchGroupPrev :call mark#SearchGroupMark(v:count, 1, 1, 1)
nnoremap <silent> <Plug>MarkSearchGroupNext :call mark#SearchGroupMark(v:count, 1, 0, 1)
nnoremap <silent> <Plug>MarkSearchOrAnyPrev :if !mark#SearchNext(1,'mark#SearchAnyMark')|execute 'normal! #zv'|endif
nnoremap <silent> <Plug>MarkSearchOrAnyNext :if !mark#SearchNext(0,'mark#SearchAnyMark')|execute 'normal! *zv'|endif
nnoremap <silent> <Plug>MarkSearchOrCurPrev :if !mark#SearchNext(1,'mark#SearchCurrentMark')|execute 'normal! #zv'|endif
nnoremap <silent> <Plug>MarkSearchOrCurNext :if !mark#SearchNext(0,'mark#SearchCurrentMark')|execute 'normal! *zv'|endif
nnoremap <silent> <Plug>MarkSearchPrev :if !mark#SearchNext(1)|execute 'normal! #zv'|endif
nnoremap <silent> <Plug>MarkSearchNext :if !mark#SearchNext(0)|execute 'normal! *zv'|endif
nnoremap <silent> <Plug>MarkSearchAnyPrev :call mark#SearchAnyMark(1)
nnoremap <silent> <Plug>MarkSearchAnyNext :call mark#SearchAnyMark(0)
nnoremap <silent> <Plug>MarkSearchCurrentPrev :call mark#SearchCurrentMark(1)
nnoremap <silent> <Plug>MarkSearchCurrentNext :call mark#SearchCurrentMark(0)
nnoremap <silent> <Plug>MarkToggle :call mark#Toggle()
nnoremap <silent> <Plug>MarkAllClear :call mark#ClearAll()
nnoremap <silent> <Plug>MarkClear :if !mark#DoMark(v:count, (v:count ? '' : mark#CurrentMark()[0]))[0]|execute "normal! \<C-\>\<C-n>\<Esc>"|endif
vnoremap <silent> <Plug>MarkRegex :if !mark#MarkRegex(v:count, mark#GetVisualSelectionAsRegexp())|execute "normal! \<C-\>\<C-n>\<Esc>"|if ! empty(v:errmsg)|echoerr v:errmsg|endif|endif
nnoremap <silent> <Plug>MarkRegex :if !mark#MarkRegex(v:count, '')|execute "normal! \<C-\>\<C-n>\<Esc>"|if ! empty(v:errmsg)|echoerr v:errmsg|endif|endif
vnoremap <silent> <Plug>MarkIWhiteSet :if !mark#DoMark(v:count, mark#GetVisualSelectionAsLiteralWhitespaceIndifferentPattern())[0]|execute "normal! \<C-\>\<C-n>\<Esc>"|endif
vnoremap <silent> <Plug>MarkSet :if !mark#DoMark(v:count, mark#GetVisualSelectionAsLiteralPattern())[0]|execute "normal! \<C-\>\<C-n>\<Esc>"|endif
nnoremap <silent> <Plug>MarkSet :if !mark#MarkCurrentWord(v:count)|execute "normal! \<C-\>\<C-n>\<Esc>"|endif
vnoremap <Plug>(emmet-code-pretty) :call emmet#codePretty()
vnoremap <Plug>(emmet-merge-lines) :call emmet#mergeLines()
nnoremap <Plug>(emmet-anchorize-summary) :call emmet#anchorizeURL(1)
nnoremap <Plug>(emmet-anchorize-url) :call emmet#anchorizeURL(0)
nnoremap <Plug>(emmet-remove-tag) :call emmet#removeTag()
nnoremap <Plug>(emmet-split-join-tag) :call emmet#splitJoinTag()
nnoremap <Plug>(emmet-toogle-comment) :call emmet#toggleComment()
nnoremap <Plug>(emmet-image-size) :call emmet#imageSize()
nnoremap <Plug>(emmet-move-prev) :call emmet#moveNextPrev(1)
nnoremap <Plug>(emmet-move-next) :call emmet#moveNextPrev(0)
vnoremap <Plug>(emmet-balance-tag-outword) :call emmet#balanceTag(-2)
nnoremap <Plug>(emmet-balance-tag-outword) :call emmet#balanceTag(-1)
vnoremap <Plug>(emmet-balance-tag-inward) :call emmet#balanceTag(2)
nnoremap <Plug>(emmet-balance-tag-inward) :call emmet#balanceTag(1)
nnoremap <Plug>(emmet-update-tag) :call emmet#updateTag()
nnoremap <Plug>(emmet-expand-word) :call emmet#expandAbbr(1,"")
vnoremap <Plug>(emmet-expand-abbr) :call emmet#expandAbbr(2,"")
nnoremap <Plug>(emmet-expand-abbr) :call emmet#expandAbbr(3,"")
nmap <silent> <Plug>RestoreWinPosn :call RestoreWinPosn()
nmap <silent> <Plug>SaveWinPosn :call SaveWinPosn()
nmap <SNR>15_WE <Plug>AlignMapsWrapperEnd
map <SNR>15_WS <Plug>AlignMapsWrapperStart
nmap <F3> :SrcExplToggle                "打开/闭浏览窗口
nmap <F2> :NERDTreeToggle
nmap <C-F10> :call Link()
nmap <C-F9> :call Compile()
nmap <F9> :call Run()
imap S <Plug>ISurround
imap s <Plug>Isurround
imap  <Left>
inoremap <silent> 	 =TriggerSnippet()
imap <NL> <Down>
imap  <Up>
imap  <Right>
inoremap <silent> 	 =ShowAvailableSnips()
imap  <Plug>Isurround
inoremap <expr>  omni#cpp#maycomplete#Complete()
imap A <Plug>(emmet-anchorize-summary)
imap a <Plug>(emmet-anchorize-url)
imap k <Plug>(emmet-remove-tag)
imap j <Plug>(emmet-split-join-tag)
imap / <Plug>(emmet-toogle-comment)
imap i <Plug>(emmet-image-size)
imap N <Plug>(emmet-move-prev)
imap n <Plug>(emmet-move-next)
imap D <Plug>(emmet-balance-tag-outword)
imap d <Plug>(emmet-balance-tag-inward)
imap u <Plug>(emmet-update-tag)
imap ; <Plug>(emmet-expand-word)
imap , <Plug>(emmet-expand-abbr)
inoremap <expr> . omni#cpp#maycomplete#Dot()
inoremap <expr> : omni#cpp#maycomplete#Scope()
imap ;ihn :IHN
imap ;is :IHS:A
imap ;ih :IHS
inoremap <expr> > omni#cpp#maycomplete#Arrow()
let &cpo=s:cpo_save
unlet s:cpo_save
set autoread
set background=dark
set backspace=2
set completefunc=neocomplcache#complete#manual_complete
set completeopt=menuone
set cscopequickfix=s-,c-,d-,i-,t-,e-
set cscopetag
set cscopeverbose
set expandtab
set fileencodings=ucs-bom,utf-8,gbk,cp936,latin-1
set fileformats=unix,dos,mac
set helplang=cn
set hidden
set hlsearch
set ignorecase
set incsearch
set nomodeline
set mouse=a
set omnifunc=omni#cpp#complete#Main
set runtimepath=~/.vim,~/.vim/bundle/vundle,~/.vim/bundle/a.vim,~/.vim/bundle/Align,~/.vim/bundle/bufexplorer.zip,~/.vim/bundle/ccvext.vim,~/.vim/bundle/cSyntaxAfter,~/.vim/bundle/ctrlp.vim,~/.vim/bundle/emmet-vim,~/.vim/bundle/vim-javacompleteex,~/.vim/bundle/Mark--Karkat,~/.vim/bundle/neocomplcache.vim,~/.vim/bundle/nerdcommenter,~/.vim/bundle/nerdtree,~/.vim/bundle/OmniCppComplete,~/.vim/bundle/repeat.vim,~/.vim/bundle/snipmate.vim,~/.vim/bundle/SrcExpl,~/.vim/bundle/std_c.zip,~/.vim/bundle/vim-surround,~/.vim/bundle/tagbar,~/.vim/bundle/taglist.vim,~/.vim/bundle/TxtBrowser,~/.vim/bundle/ZoomWin,~/.vim/bundle/indentpython.vim,~/.vim/bundle/winmanager,~/.vim/bundle/syntastic,~/.vim/bundle/vim-powerline,~/.vim/bundle/indentLine,/usr/local/share/vim/vimfiles,/usr/local/share/vim/vim74,/usr/local/share/vim/vimfiles/after,~/.vim/after,~/.vim/bundle/vundle/,~/.vim/bundle/vundle/after,~/.vim/bundle/a.vim/after,~/.vim/bundle/Align/after,~/.vim/bundle/bufexplorer.zip/after,~/.vim/bundle/ccvext.vim/after,~/.vim/bundle/cSyntaxAfter/after,~/.vim/bundle/ctrlp.vim/after,~/.vim/bundle/emmet-vim/after,~/.vim/bundle/vim-javacompleteex/after,~/.vim/bundle/Mark--Karkat/after,~/.vim/bundle/neocomplcache.vim/after,~/.vim/bundle/nerdcommenter/after,~/.vim/bundle/nerdtree/after,~/.vim/bundle/OmniCppComplete/after,~/.vim/bundle/repeat.vim/after,~/.vim/bundle/snipmate.vim/after,~/.vim/bundle/SrcExpl/after,~/.vim/bundle/std_c.zip/after,~/.vim/bundle/vim-surround/after,~/.vim/bundle/tagbar/after,~/.vim/bundle/taglist.vim/after,~/.vim/bundle/TxtBrowser/after,~/.vim/bundle/ZoomWin/after,~/.vim/bundle/indentpython.vim/after,~/.vim/bundle/winmanager/after,~/.vim/bundle/syntastic/after,~/.vim/bundle/vim-powerline/after,~/.vim/bundle/indentLine/after
set sessionoptions=blank,buffers,folds,help,options,tabpages,winsize,sesdir,slash,unix
set shiftwidth=4
set smartcase
set smartindent
set smarttab
set tabstop=4
set tags=./tags;
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
exe "cd " . escape(expand("<sfile>:p:h"), ' ')
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +23 ftpipc.c
badd +3 ftpnobody.c
badd +541 ftpproto.c
badd +61 main.c
badd +68 parseconf.c
badd +29 session.c
badd +43 str.c
badd +288 sysutil.c
badd +11 tunable.c
badd +46 common.h
badd +72 ftpcodes.h
badd +1 ftpipc.h
badd +6 ftpnobody.h
badd +7 ftpproto.h
badd +1 parseconf.h
badd +13 session.h
badd +5 str.h
badd +12 sysutil.h
badd +1 tunable.h
badd +213 echoclnt.c
badd +20 miniftpd.conf
badd +118 hash.c
badd +9 hash.h
badd +301 miniftpd
badd +630 tags
args ~/code/linuxstudy/miniftpd/miniftpd.conf
edit main.c
set splitbelow splitright
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
argglobal
map <buffer> ;  :w:make
setlocal keymap=
setlocal noarabic
setlocal noautoindent
setlocal nobinary
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal concealcursor=
setlocal conceallevel=0
setlocal completefunc=neocomplcache#complete#manual_complete
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal define=
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal expandtab
if &filetype != 'c'
setlocal filetype=c
endif
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=croql
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=0
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal nomodeline
setlocal modifiable
setlocal nrformats=octal,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=omni#cpp#complete#Main
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal shiftwidth=4
setlocal noshortname
setlocal smartindent
setlocal softtabstop=0
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'c'
setlocal syntax=c
endif
setlocal tabstop=4
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal noundofile
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let s:l = 58 - ((23 * winheight(0) + 18) / 37)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
58
normal! 05|
tabnext 1
if exists('s:wipebuf')
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToO
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
