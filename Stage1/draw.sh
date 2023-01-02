lex source.l  
bison -d source.y
cc parse.c lex.yy.c source.tab.c
./a.out <input.txt >pic.dot
dot pic.dot -T png -o pic.png