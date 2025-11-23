; nasm -f win64 calc.asm -o calc.o
default rel

section .data
    CONV_FACTOR: dd 3.6            ; since 1 km/h = 1000m / 3600s = 1/3.6 m/s

section .text
    global calculate_acceleration 

calculate_acceleration:
    push    rbp
    mov     rbp, rsp
    
    ; rcx = ptr to input_matrix (float*)
    ; rdx = ptr to output_array (int*)
    ; r8d = num_cars (int), we'll use r8 (64-bit)
    
    ; load 3.6 into an xmm register
    movss xmm7, [rel CONV_FACTOR]

    ; rbx as the loop counter 'i'. rbx is a callee-saved reg
    ; save/restore it.
    push    rbx
    xor     rbx, rbx            ; i = 0

loop_start:
    ; comp counter 'i' (rbx) with 'num_cars' (r8)
    cmp     rbx, r8
    jge     loop_end            ; jump if i >= num_cars

    ; calc memory offsets
    
    ; rax = i * 12 (input offset)
    mov     rax, rbx
    imul    rax, 12
    
    ; r9 = i * 4 (output offset)
    mov     r9, rbx
    imul    r9, 4

    ; load data for car 'i'
    
    ; xmm0 = Vf (input_matrix[i*3 + 1])
    movss   xmm0, [rcx + rax + 4]  ; rcx = base of input_matrix
    
    ; xmm1 = Vi (input_matrix[i*3 + 0])
    movss   xmm1, [rcx + rax + 0]
    
    ; xmm2 = T (input_matrix[i*3 + 2])
    movss   xmm2, [rcx + rax + 8]

    ; calculate now
    ; xmm0 = Vf - Vi
    subss   xmm0, xmm1
    
    ; xmm0 = (Vf - Vi) / 3.6
    divss   xmm0, xmm7
    
    ; xmm0 = ((Vf - Vi) / 3.6) / T
    divss   xmm0, xmm2
    
    ; convert and store the result
    
    ; conv float in xmm0 to 32-bit int in xmm0 (round-to-nearest)
    cvtps2dq xmm0, xmm0
    
    ; store the 32-bit integer result into the output array
    ; rdx = base of output_array
    movd    [rdx + r9], xmm0

    ; inc loop counter
    inc     rbx
    jmp     loop_start

loop_end:
    ; restore rbx
    pop     rbx

    ; function epilogue
    mov     rsp, rbp
    pop     rbp
    ret