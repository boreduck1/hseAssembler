;Необходимо разработать программу,
;определяющую число чисел-палиндромов
;(в десятичном представлении) в диапазоне от 1 до ?10?^6
format PE Console
entry start
include 'win32a.inc'

section '.code' code readable executable
start:                                                           \
        mov ecx, 1000000            ; установка счетчика внешнего цикла (начинаем проверку с 10^6)
        mov [palindromes], 0      ; по адресу [polindroms] записываем значение счетчика палиндромов
numsLoop:                           ; внешний цикл для перебора чисел

        mov ebx, ecx                ; в ebx записываем текущее число, которое проверяется

        cmp ebx, 10                 ; если число меньше 10, то оно палиндром
        jl ifPalindrom
        cinvoke wsprintf, tmp_line, formatNum, ebx  ; конвертация числа из ebx в строку tmp_line по формату formatNum
        mov esi, tmp_line           ; указание на начало строки
        invoke lstrlen, tmp_line    ; вычисление длины строки tmp_line (длина в регистре eax)
        mov edi, eax                ; длину строки из eax -> edi
        mov ecx, eax                ; длину строки из eax -> ecx
        shr ecx, 1                  ; сдвиг значения в регистре ecx на 1 разряд вправо == деление на 2 с округлением вниз

      bytesLoop:                    ; внутренний цикл для сравнения симметричных байтов в строке

          mov edx, esi
          add edx, edi
          sub edx, ecx
          mov ah, byte [edx]        ; правый байт/"символ" (значение по адресу esi+edi-ecx) -> ah
          cmp ah, byte [esi+ecx-1]  ; сравнение с байтом/"символом" слева (значение по дресу esi+ecx-1)
          jnz cycle                 ; выход из цикла bytesLoop, если байты не равны

      loopd bytesLoop               ; цикл с командой loopd для декремента регистра ecx

        ifPalindrom:                ; если число палиндром
        mov eax, [palindromes]      ; инкремент счетчика палиндромов
        inc eax
        mov [palindromes], eax

        cycle:
        mov ecx, ebx

loopd numsLoop                      ; цикл с командой loopd для декремента регистра ecx

        push [palindromes]          ; вывод результата в консоль
        push formatResult
        call [printf]

        call [getch]
        push 0
        call [ExitProcess]
 
section '.data' data readable writable

        formatNum         db '%d', 0                                               ; формат для конвертации числа в строку
        formatResult      db 'The number of palindrome numbers in the range [1;10^6] = %d', 0  ; формат для вывода результата программы в консоль
        palindromes       dd ?                                                     ; количество чисел палиндромов
        tmp_line          db ?                                                     ; строка для конвертации

section '.idata' import data readable

        library kernel32,'kernel32.dll',\
               user32,'user32.dll',\
               msvcrt, 'msvcrt.dll'

        import msvcrt,\
               printf, 'printf',\
               getch, '_getch'

        include 'api\kernel32.inc'
        include 'api\user32.inc'