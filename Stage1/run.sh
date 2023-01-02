lex source.l  
bison -d source.y
cc parse.c lex.yy.c source.tab.c
./a.out <input.txt
# ./a.out -e input.txt -o output.txt