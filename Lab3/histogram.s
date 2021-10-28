  PUBLIC EightBitHistogram

  SECTION .text : CODE (2)
 

THUMB
  Export EightBitHistogram


EightBitHistogram
  PUSH {LR}
  PUSH {R4-R8}
  MUL R4,R0,R1
  CMP R4,#64000   ;Se maior q 64k retorna 0
  BLS  valido
  MOV R0,#0
  POP {R4-R8}
  POP {LR}
  BX LR
valido:
  MOV R0,R4      ;retorna hxw

  MOV R5,#0 
  MOV R8,R3
zera:               ;zera vetor histogram
 
  MOV R6,#0
  STRH R6,[R8],#2
  ADD R5,R5,#1
  CMP R5,#256
  BEQ zerado
  B zera
zerado:
  MOV R5,#0      
pula:
  CMP R5,R4      ;enquanto não acabou a imagem     
  BEQ fim
  LDRB R6,[R2],#1      ;guarda a cor
  LSL R6,R6,#1
  MOV R8,R3           
  ADD R8,R8,R6        ;recupera a qnt da cor atual,incrementa e guarda de volta no indice
  LDRH R7,[R8]
  ADD R7,R7,#1
  STRH R7,[R8]
  ADD R5,R5,#1
  B pula

fim:
  POP {R4-R8}
  POP {LR}
  BX LR
  
  END
  
