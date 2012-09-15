; Seven Kingdoms 2: The Fryhtan War
;
; Copyright 1999 Enlight Software Ltd.
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

; Filename    : I_EFFECT.ASM
; Description : 16-bit color effect

INCLUDE all.inc
INCLUDE colcode.inc
INCLUDE i_effect.inc

		PUBLIC	INITeffect
		PUBLIC	pixel_format_flag
		PUBLIC	log_alpha_mask

.DATA

;--------------------------------------------------

pixel_format_flag	dd ?

; mask for calcuate alpha blending
log_alpha_mask	dw 8 dup (?)

.CODE

;------- BEGIN OF FUNCTION INITeffect ------------
;
; Initialize the settings of the IMGeffect
;
; Syntax : INITeffect(pixelFormatFlag)
;
; int  pixelFormatFlag		pixel format for the primary surface
;
INITeffect	PROC	pixelFormatFlag
		STARTPROC

		MOV	EAX, pixelFormatFlag
		MOV	pixel_format_flag, EAX

		; set functions pointers

@@initEffect0:	CMP	EAX, PIXFORM_RGB_555
		JNE	@@initEffect1

		MOV	log_alpha_mask[0*2], 0111111111111111b
		MOV	log_alpha_mask[1*2], 0011110111101111b
		MOV	log_alpha_mask[2*2], 0001110011100111b
		MOV	log_alpha_mask[3*2], 0000110001100011b
		MOV	log_alpha_mask[4*2], 0000010000100001b
		MOV	log_alpha_mask[5*2], 0000000000000000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		JMP	@@initEffectEnd

@@initEffect1:	CMP	EAX, PIXFORM_RGB_565
		JNE	@@initEffect2

		MOV	log_alpha_mask[0*2], 1111111111111111b
		MOV	log_alpha_mask[1*2], 0111101111101111b
		MOV	log_alpha_mask[2*2], 0011100111100111b
		MOV	log_alpha_mask[3*2], 0001100011100011b
		MOV	log_alpha_mask[4*2], 0000100001100001b
		MOV	log_alpha_mask[5*2], 0000000000100000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		JMP	@@initEffectEnd

@@initEffect2:	CMP	EAX, PIXFORM_BGR_555
		JNE	@@initEffect3

		MOV	log_alpha_mask[0*2], 0111111111111111b
		MOV	log_alpha_mask[1*2], 0011110111101111b
		MOV	log_alpha_mask[2*2], 0001110011100111b
		MOV	log_alpha_mask[3*2], 0000110001100011b
		MOV	log_alpha_mask[4*2], 0000010000100001b
		MOV	log_alpha_mask[5*2], 0000000000000000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		JMP	@@initEffectEnd

@@initEffect3:	; assume EAX, PIXFORM_BGR_565

		MOV	log_alpha_mask[0*2], 1111111111111111b
		MOV	log_alpha_mask[1*2], 0111101111101111b
		MOV	log_alpha_mask[2*2], 0011100111100111b
		MOV	log_alpha_mask[3*2], 0001100011100011b
		MOV	log_alpha_mask[4*2], 0000100001100001b
		MOV	log_alpha_mask[5*2], 0000000000100000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		; JMP	@@initEffectEnd

@@initEffectEnd:
		ENDPROC
INITeffect	ENDP


; called from C
PUBLIC getAlphaMask
getAlphaMask PROC id
  XOR EAX,EAX
  MOV AX, log_alpha_mask[0*2]
  RET
getAlphaMask ENDP

END
