lex source.l  
bison -d source.y
cc parse.c lex.yy.c source.tab.c
./a.out <input.txt >outSmart.txt
./sound/a.out <input.txt >outGun.txt
# ./a.out -e input.txt -o output.txt