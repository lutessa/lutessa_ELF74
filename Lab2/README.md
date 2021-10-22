A variável ui32Loop é declarada como volatile pois desta forma
o compilador entende que não se deve fazer otimizações com ela,
pois ela pode ser modificada sem o conhecimento do programa principal. 