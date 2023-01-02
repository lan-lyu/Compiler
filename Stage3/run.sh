lex source.l  
bison -d source.y
cc parse.c lex.yy.c source.tab.c
./a.out <input.txt >gSmartout.txt
./gungun/a.out <input.txt >gungunout.txt
# ./a.out -e input.txt -o output.txt