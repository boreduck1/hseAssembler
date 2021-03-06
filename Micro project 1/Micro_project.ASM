;���������� ����������� ���������,
;������������ ����� �����-�����������
;(� ���������� �������������) � ��������� �� 1 �� ?10?^6
format PE Console
entry start
include 'win32a.inc'

section '.code' code readable executable
start:                                                           \
        mov ecx, 1000000            ; ��������� �������� �������� ����� (�������� �������� � 10^6)
        mov [palindromes], 0      ; �� ������ [polindroms] ���������� �������� �������� �����������
numsLoop:                           ; ������� ���� ��� �������� �����

        mov ebx, ecx                ; � ebx ���������� ������� �����, ������� �����������

        cmp ebx, 10                 ; ���� ����� ������ 10, �� ��� ���������
        jl ifPalindrom
        cinvoke wsprintf, tmp_line, formatNum, ebx  ; ����������� ����� �� ebx � ������ tmp_line �� ������� formatNum
        mov esi, tmp_line           ; �������� �� ������ ������
        invoke lstrlen, tmp_line    ; ���������� ����� ������ tmp_line (����� � �������� eax)
        mov edi, eax                ; ����� ������ �� eax -> edi
        mov ecx, eax                ; ����� ������ �� eax -> ecx
        shr ecx, 1                  ; ����� �������� � �������� ecx �� 1 ������ ������ == ������� �� 2 � ����������� ����

      bytesLoop:                    ; ���������� ���� ��� ��������� ������������ ������ � ������

          mov edx, esi
          add edx, edi
          sub edx, ecx
          mov ah, byte [edx]        ; ������ ����/"������" (�������� �� ������ esi+edi-ecx) -> ah
          cmp ah, byte [esi+ecx-1]  ; ��������� � ������/"��������" ����� (�������� �� ����� esi+ecx-1)
          jnz cycle                 ; ����� �� ����� bytesLoop, ���� ����� �� �����

      loopd bytesLoop               ; ���� � �������� loopd ��� ���������� �������� ecx

        ifPalindrom:                ; ���� ����� ���������
        mov eax, [palindromes]      ; ��������� �������� �����������
        inc eax
        mov [palindromes], eax

        cycle:
        mov ecx, ebx

loopd numsLoop                      ; ���� � �������� loopd ��� ���������� �������� ecx

        push [palindromes]          ; ����� ���������� � �������
        push formatResult
        call [printf]

        call [getch]
        push 0
        call [ExitProcess]
 
section '.data' data readable writable

        formatNum         db '%d', 0                                               ; ������ ��� ����������� ����� � ������
        formatResult      db 'The number of palindrome numbers in the range [1;10^6] = %d', 0  ; ������ ��� ������ ���������� ��������� � �������
        palindromes       dd ?                                                     ; ���������� ����� �����������
        tmp_line          db ?                                                     ; ������ ��� �����������

section '.idata' import data readable

        library kernel32,'kernel32.dll',\
               user32,'user32.dll',\
               msvcrt, 'msvcrt.dll'

        import msvcrt,\
               printf, 'printf',\
               getch, '_getch'

        include 'api\kernel32.inc'
        include 'api\user32.inc'