;Gilazutdinova Adelia
;194 group
;number of the task - 6

;Input an array and a number via keyboard
;and create a new array consisting of numbers
;from the first array that are multiples of the number


format PE console
entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        strVecSize       db 'size of vector? ', 0
        strIncorSize     db 'Incorrect size of vector = %d', 10, 0
        strVecElemI      db '[%d]? ', 0
        strScanInt       db '%d', 0
        strSumValue      db 'Summa = %d', 10, 0
        strVecElemOut    db '[%d] = %d', 10, 0
        strNum           db 'number? ', 0
        strIncorNum      db 'Incorrect number', 10, 0
        strSecondVec     db 'Second vector: ',10, 0
        strFirstVec      db 'First vector: ',10, 0
        res              db 'res = %d', 10, 0


        vec_size     dd 0
        tmp_neg      dd 0
        num          dd 0
        i            dd ?
        vec2_size    dd 0
        tmp          dd ?
        tmp2         dd ?
        tmpStack     dd ?
        vecFirst     rd 100
        vecSecond    rd 100   ;new array

;--------------------------------------------------------------------------
section '.code' code readable executable
start:
;vector and number input
        call DataInput
;creating second array
       call VectorSecond
;first vector out
        push strFirstVec
        call [printf]
        call FirstVectorOut
;second vector out
        push strSecondVec
        call [printf]
        call SecondVectorOut
finish:
        call [getch]

        push 0
        call [ExitProcess]


;--------------------------------------------------------------------------
DataInput:
        push strVecSize            ; size input
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [vec_size]
        cmp eax, 0
        jg  getVector
;incorrect size
        push [vec_size]
        push strIncorSize
        call [printf]

        call [getch]
        push 0
        call [ExitProcess]

getVector:
        xor ecx, ecx               ; ecx = 0
        mov ebx, vecFirst          ; ebx = &vecFirst
getVecLoop:
        mov [tmp], ebx             ; vec input
        cmp ecx, [vec_size]
        je getNum

        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
getNum:
        push strNum                 ;number input
        call [printf]
        add esp, 4

        push num
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [num]
        cmp eax, 0
        jg endInputData

        neg [num]
        jmp endInputData

endInputData:

        ret
;--------------------------------------------------------------------------
VectorSecond:
        xor ecx, ecx                 ; ecx = 0
        mov ebx, vecFirst            ; ebx = &vecFirst
        mov esi, vecSecond           ; esi = &vecSecond
secondVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endSecondVector

        mov [i], ecx

        xor edx, edx                 ;elem div num
        mov eax, [ebx]
        cmp eax, 0
        jg cont
        neg eax
        jmp cont
        cont:
        mov ecx, [num]
        div ecx



        cmp edx, 0                   ;multiplicity check
        jne increment

        inc [vec2_size]

        mov eax, [ebx]


       ; addElem:
        mov [esi], eax
        add esi, 4

        jmp increment

        increment:
        mov ebx, [tmp]
        mov ecx, [i]
        inc ecx
        add ebx, 4

        jmp secondVecLoop
endSecondVector:
        ret
;--------------------------------------------------------------------------
FirstVectorOut:
        mov [tmpStack], esp
        xor ecx, ecx                 ; ecx = 0
        mov ebx, vecFirst            ; ebx = &vecFirst


putFirstVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputFirstVector
        mov [i], ecx

        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putFirstVecLoop
endOutputFirstVector:
        mov esp, [tmpStack]
        ret

;--------------------------------------------------------------------------
SecondVectorOut:
        mov [tmpStack], esp
        xor ecx, ecx             ; ecx = 0
        mov ebx, vecSecond       ; ebx = &vecSecond

       ; push strSecondVec
       ; call [printf]
putSecVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec2_size]
        je endOutputSecondVector
        mov [i], ecx

        ; output element
        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putSecVecLoop
endOutputSecondVector:
        mov esp, [tmpStack]
        ret

;--------------------------------------------------------------------------
                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'