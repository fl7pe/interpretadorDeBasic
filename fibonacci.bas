10 INPUT "Quantos termos da sequência de Fibonacci você quer ver? ", N
20 LET A = 0
30 LET B = 1
40 PRINT A
50 PRINT B
60 FOR I = 2 TO N - 1
70 LET C = A + B
80 PRINT C
90 LET A = B
100 LET B = C
110 NEXT I
120 HALT
