; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright 2020 Tim Wiederhake

; Default file descriptors:
;    0: stdin
;    1: stdout
;    2: stderr
;
; Syscall reference: http://syscalls.kernelgrok.com/
;    syscall "exit(int error_code)":
;        eax = 0x01
;        ebx = error_code
;        does not return.
;
;    syscall "eax = read(unsigned int fd, char *buf, size_t count)":
;        eax = 0x03
;        ebx = fd
;        ecx = buf
;        edx = count
;        returns number of bytes read; 0 = eof. <0 = failure.
;
;    syscall "eax = write(unsigned int fd, const char *buf, size_t count)":
;        eax = 0x04
;        ebx = fd
;        ecx = buf
;        edx = count
;        returns number of bytes written.
;
; ELF header description:
;    System V Application Binary Interface, Edition 4.1: http://www.sco.com/developers/devspecs/gabi41.pdf
;    System V Application Binary Interface, Draft 2013-06-10: http://www.sco.com/developers/gabi/latest/contents.html
;    System V Application Binary Interface, Intel386 Architecture Processor Supplement http://www.sco.com/developers/devspecs/abi386-4.pdf
;
; White space characters:
;    ' ' (0x20), '\t' (0x09), '\n' (0x0A), '\v' (0x0B), '\f' (0x0C), '\r' (0x0D)

bits    32
org    0x08048000

Elf32_Ehdr:
    dd    0x464C457F            ; e_ident[0 .. 3]:      "\177ELF" = magic number
    db    0x01                  ; e_ident[4]:           0x01 = ELFCLASS32, this is a 32 bit file header
    db    0x01                  ; e_ident[5]:           0x01 = ELFDATA2LSB, data is 2's complement little endian
    db    0x01                  ; e_ident[6]:           0x01 = EI_VERSION, file header version 1
    db    0x00                  ; e_ident[7]:           0x00 = ELFOSABI_SYSV, System V / Linux executable
    dd    0x00000000            ; e_ident[ 8 .. 11]:    padding
    dd    0x00000000            ; e_ident[12 .. 15]:    padding
    dw    0x0002                ; e_type:               0x02 = ET_EXEC, this is an executable file
    dw    0x0003                ; e_machine:            0x03 = EM_386, instruction set is x86
    dd    0x00000001            ; e_version:            0x01 = elf format version, again
    dd    (_start)              ; e_entry:              0x08048054 = _start
    dd    0x00000034            ; e_phoff:              0x00000034 = sizeof(Elf32_Ehdr)
    dd    0x00000000            ; e_shoff:              0x00000000 = no section headers
    dd    0x00000000            ; e_flags:              unused for x86
    dw    0x0034                ; e_ehsize:             0x34 = sizeof(Elf32_Ehdr)
    dw    0x0020                ; e_phentsize:          0x20 = sizeof(Elf32_Phdr)
    dw    0x0001                ; e_phnum:              0x01 = number of program headers
    dw    0x0028                ; e_shentsize:          0x28 = sizeof(Elf32_Shdr)
    dw    0x0000                ; e_shnum:              0x00 = number of section headers
    dw    0x0000                ; e_shstrndx:           0x00 = index of "section names" section

Elf32_Phdr:
    dd    0x00000001            ; p_type:               0x01 = PT_LOAD
    dd    0x00000000            ; p_offset:             0x00 = load from begin of file
    dd    0x08048000            ; p_vaddr:              0x08048000 = default loading address for x86
    dd    0x08048000            ; p_paddr:              0x08048000 = default loading address for x86
    dd    (eof - Elf32_Ehdr)    ; p_filesz:             image size on disk
    dd    (eof - Elf32_Ehdr)    ; p_memsz:              image size in memory
    dd    0x00000007            ; p_flags:              0x07 = PF_X | PW_W | PF_R
    dd    0x00000000            ; p_align:              0x00 = no alignment

_start:
    ; ecx = buffer address = &temp (constant)
    mov     ecx,    temp

    ; edx = buffer size = 1 (constant)
    mov     edx,    0x01

item:
    ; eax = read(stdin, &temp, 1);
    mov     eax,    0x03
    mov     ebx,    0x00
    int     0x80

    ; if (eax == 0) goto exit0;
    cmp     eax,    0x00
    je      exit0

    ; eax = temp;
    mov     eax,    [ecx]

    ; if (eax == ' ') goto item;
    cmp     eax,    0x20
    je      item

    ; if (eax < '\t') goto exit1;
    cmp     eax,    0x09
    jl      exit1

    ; if (eax <= '\r') goto item;
    cmp     eax,    0x0D
    jle     item

    ; if (eax == '#') goto discard_comment;
    cmp     eax,    0x23
    je      discard_comment

    ; if (eax < '0') goto exit1;
    cmp     eax,    0x30
    jl      exit1

    ; if (eax <= '9') goto c1_number;
    cmp     eax,    0x39
    jle     c1_number

    ; if (eax < 'A') goto exit1;
    cmp     eax,    0x41
    jl      exit1

    ; if (eax <= 'F') goto c1_alpha;
    cmp     eax,    0x46
    jle     c1_alpha

    ; ... else goto exit1;
    jmp     exit1

c1_alpha:
    ; eax -= ('A' - '0' - 10);
    sub     eax,    0x07

c1_number:
    ; eax -= '0';
    sub     eax,    0x30

c1_valid:
    ; eax <<= 4;
    shl     eax,    4

    ; esi = eax;
    mov     esi,    eax

    ; eax = read(stdin, &temp, 1);
    mov     eax,    0x03
    mov     ebx,    0x00
    int     0x80

    ; if (eax == 0) goto exit1;
    cmp     eax,    0x00
    je      exit1

    ; eax = temp;
    mov    eax,    [ecx]

    ; if (eax < '0') goto exit1;
    cmp     eax,    0x30
    jl      exit1

    ; if (eax <= '9') goto c2_number;
    cmp     eax,    0x39
    jle     c2_number

    ; if (eax < 'A') goto exit1;
    cmp     eax,    0x41
    jl      exit1

    ; if (eax <= 'F') goto c2_alpha;
    cmp     eax,    0x46
    jle     c2_alpha

    ; ... else goto exit1;
    jmp     exit1

c2_alpha:
    ; eax -= ('A' - '0' - 10);
    sub     eax,    0x07

c2_number:
    ; eax -= '0';
    sub     eax,    0x30

c2_valid:
    ; eax += esi;
    add     eax,    esi

    ; temp = eax;
    mov     [ecx],  eax

    ; write(stdout, &temp, 1);
    mov     eax,    0x04
    mov     ebx,    0x01
    int     0x80

    ; goto item;
    jmp     item

exit0:
    ; exit(0);
    mov     eax,    0x01
    mov     ebx,    0x00
    int     0x80

exit1:
    ; exit(1);
    mov     eax,    0x01
    mov     ebx,    0x01
    int     0x80

discard_comment:
    ; eax = read(stdin, &temp, 1);
    mov     eax,    0x03
    mov     ebx,    0x00
    int     0x80

    ; if (eax == 0) goto exit0;
    cmp     eax,    0x00
    je      exit0

    ; eax = temp;
    mov     eax,    [ecx]

    ; if (eax == '\n') goto item;
    cmp     eax,    0x0A
    je      item

    ; if (eax == '\r') goto item;
    cmp     eax,    0x0D
    je      item

    jmp     discard_comment

temp:
    db      0xff

eof:
