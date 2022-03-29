0000: 
       MOVLW  0
       MOVWF  0Ah
       GOTO   L0004
       NOP
L0004  MOVF   3,W
       ANDLW  1Fh
       MOVWF  3
       MOVLW  72h
       BSF    3,5
       MOVWF  0Fh
       CLRF   10h
       MOVF   0Fh,W
       BCF    1Fh,4
       BCF    1Fh,5
       MOVF   1Bh,W
       ANDLW  80h
       MOVWF  1Bh
       MOVLW  7
       MOVWF  1Ch
       BCF    3,7
       MOVLW  0
       MOVWF  6
L0016  BCF    6,3
       BCF    3,5
       BSF    6,3
       BSF    3,5
       CLRF   6
       MOVLW  0FFh
       BCF    3,5
       MOVWF  6
       BSF    3,5
       GOTO   L0016
       SLEEP
2007: 3F30
2008: 3FFF
