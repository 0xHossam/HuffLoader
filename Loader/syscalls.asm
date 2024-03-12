.data
	systemCall WORD 000h
	syscallAddr QWORD 0h

.code

	GetPEB proc
		; read TEB address from GS segment register
		mov rax, gs:[30h]         
		mov rax, [rax + 60h]       
		ret
	GetPEB endp


	AmmunitionPrepare proc
					mov systemCall, cx
					ret
	AmmunitionPrepare endp

	GunPrepare proc
			mov rax, syscallAddr
			test rax, rax
			jnz THEEND
			mov syscallAddr, rcx
			THEEND:
			ret
	GunPrepare endp

	GunFire proc
		mov r10, rcx
		xor rax, rax
		mov ax, systemCall
		test ax, ax
		JZ GunRuined
		jmp	qword ptr syscallAddr
		ret
	GunFire endp

	GunRuined proc
		not rax
		ret
	GunRuined endp


end