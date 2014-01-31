#------LAB2-Assembly-File------------

	.globl getebp

getebp:			# return stack fram pointer FP
	movl %ebp, %eax
	ret


