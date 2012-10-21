
# Hello, world! example

text:
	.string "Hello, world!$"

main:
	mov ah 0x09
	mov dx @text
	int 21h
	ret

foo:
	call @cafebabe
	int @deadbezf
