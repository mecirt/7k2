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

		PUBLIC	INITeffect, IMGeffect, IMGmakePixel, IMGdecodePixel
		PUBLIC	pixel_format_flag
		PUBLIC	red_mask, green_mask, blue_mask
		PUBLIC	red_shift, green_shift, blue_shift
		PUBLIC	log_alpha_mask

.DATA

;--------------------------------------------------

pixel_format_flag	dd ?

; eg RGB_555, 0BBBBBGGGGGRRRRR, red_mask = 0000000000011111
red_mask		dw ?
green_mask		dw ?
blue_mask		dw ?

; shift that component to left shift to highest 16-bit
; eg RGB_555, 0BBBBBGGGGGRRRRR, red_shift = 11, green_shift = 6, blue_shift = 1
red_shift		db ?
green_shift		db ?
blue_shift		db ?

; mask for calcuate alpha blending
log_alpha_mask	dw 8 dup (?)

; private function pointers
make_pixel_proc		dd ?
decode_pixel_proc	dd ?
effect_proc		dd	MAX_EFFECT_CODE - MIN_EFFECT_CODE + 1 dup(?)

; private remap table for effect

; shadow effect is shadow color  (R=G=B=0, A=0.75)
; effect 0 is dim (R=G=B=0, A=0.5)
; effect 1 is blue filter   (R=0, G=0, B=1, A=0.5)
; effect 2 is green filter  (R=0, G=1, B=0, A=0.5)
; effect 3 is cyan filter   (R=0, G=1, B=1, A=0.5)
; effect 4 is red filter    (R=1, G=0, B=0, A=0.5)
; effect 5 is purple filter (R=1, G=0, B=1, A=0.5)
; effect 6 is yellow filter (R=1, G=1, B=0, A=0.5)
; effect 7 is mono filter   (R=1, G=1, B=1, A=0.5)


.CODE

; ------ declare functions for PIXFORM_RGB_555 ---------

makePixelRGB555		PROTO
decodePixelRGB555	PROTO
shadowRGB555		PROTO
effect0RGB555		PROTO
effect1RGB555		PROTO
effect2RGB555		PROTO
effect3RGB555		PROTO
effect4RGB555		PROTO
effect5RGB555		PROTO
effect6RGB555		PROTO
effect7RGB555		PROTO

makePixelRGB565		PROTO
decodePixelRGB565	PROTO
shadowRGB565		PROTO
effect0RGB565		PROTO
effect1RGB565		PROTO
effect2RGB565		PROTO
effect3RGB565		PROTO
effect4RGB565		PROTO
effect5RGB565		PROTO
effect6RGB565		PROTO
effect7RGB565		PROTO

makePixelBGR555		PROTO
decodePixelBGR555	PROTO
shadowBGR555		PROTO
effect0BGR555		PROTO
effect1BGR555		PROTO
effect2BGR555		PROTO
effect3BGR555		PROTO
effect4BGR555		PROTO
effect5BGR555		PROTO
effect6BGR555		PROTO
effect7BGR555		PROTO

makePixelBGR565		PROTO
decodePixelBGR565	PROTO
shadowBGR565		PROTO
effect0BGR565		PROTO
effect1BGR565		PROTO
effect2BGR565		PROTO
effect3BGR565		PROTO
effect4BGR565		PROTO
effect5BGR565		PROTO
effect6BGR565		PROTO
effect7BGR565		PROTO


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
		MOV	EAX, makePixelRGB555
		MOV	make_pixel_proc, EAX
		MOV	EAX, decodePixelRGB555
		MOV	decode_pixel_proc, EAX
		MOV	EAX, shadowRGB555	; shadowRGB555
		MOV	effect_proc, EAX
		MOV	EAX, effect0RGB555
		MOV	effect_proc[1*4], EAX
		MOV	EAX, effect1RGB555
		MOV	effect_proc[2*4], EAX
		MOV	EAX, effect2RGB555
		MOV	effect_proc[3*4], EAX
		MOV	EAX, effect3RGB555
		MOV	effect_proc[4*4], EAX
		MOV	EAX, effect4RGB555
		MOV	effect_proc[5*4], EAX
		MOV	EAX, effect5RGB555
		MOV	effect_proc[6*4], EAX
		MOV	EAX, effect6RGB555
		MOV	effect_proc[7*4], EAX
		MOV	EAX, effect7RGB555
		MOV	effect_proc[8*4], EAX

		MOV	red_mask, 001fh
		MOV	green_mask, 03e0h
		MOV	blue_mask, 7c00h
		MOV	red_shift, 11
		MOV	green_shift, 6
		MOV	blue_shift, 1

		MOV	log_alpha_mask[0*2], 0111111111111111b
		MOV	log_alpha_mask[1*2], 0011110111101111b
		MOV	log_alpha_mask[2*2], 0001110011100111b
		MOV	log_alpha_mask[3*2], 0000110001100011b
		MOV	log_alpha_mask[4*2], 0000010000100001b
		MOV	log_alpha_mask[5*2], 0000000000000000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		CALL	INITeffectsRGB555
		JMP	@@initEffectEnd

@@initEffect1:	CMP	EAX, PIXFORM_RGB_565
		JNE	@@initEffect2
		MOV	EAX, makePixelRGB565
		MOV	make_pixel_proc, EAX
		MOV	EAX, decodePixelRGB565
		MOV	decode_pixel_proc, EAX
		MOV	EAX, shadowRGB565	; shadowRGB565
		MOV	effect_proc, EAX
		MOV	EAX, effect0RGB565
		MOV	effect_proc[1*4], EAX
		MOV	EAX, effect1RGB565
		MOV	effect_proc[2*4], EAX
		MOV	EAX, effect2RGB565
		MOV	effect_proc[3*4], EAX
		MOV	EAX, effect3RGB565
		MOV	effect_proc[4*4], EAX
		MOV	EAX, effect4RGB565
		MOV	effect_proc[5*4], EAX
		MOV	EAX, effect5RGB565
		MOV	effect_proc[6*4], EAX
		MOV	EAX, effect6RGB565
		MOV	effect_proc[7*4], EAX
		MOV	EAX, effect7RGB565
		MOV	effect_proc[8*4], EAX

		MOV	red_mask, 001fh
		MOV	green_mask, 07e0h
		MOV	blue_mask, 0f800h
		MOV	red_shift, 11
		MOV	green_shift, 5
		MOV	blue_shift, 0

		MOV	log_alpha_mask[0*2], 1111111111111111b
		MOV	log_alpha_mask[1*2], 0111101111101111b
		MOV	log_alpha_mask[2*2], 0011100111100111b
		MOV	log_alpha_mask[3*2], 0001100011100011b
		MOV	log_alpha_mask[4*2], 0000100001100001b
		MOV	log_alpha_mask[5*2], 0000000000100000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		CALL	INITeffectsRGB565
		JMP	@@initEffectEnd

@@initEffect2:	CMP	EAX, PIXFORM_BGR_555
		JNE	@@initEffect3
		MOV	EAX, makePixelBGR555
		MOV	make_pixel_proc, EAX
		MOV	EAX, decodePixelBGR555
		MOV	decode_pixel_proc, EAX
		MOV	EAX, shadowBGR555	; shadowBGR555
		MOV	effect_proc, EAX
		MOV	EAX, effect0BGR555
		MOV	effect_proc[1*4], EAX
		MOV	EAX, effect1BGR555
		MOV	effect_proc[2*4], EAX
		MOV	EAX, effect2BGR555
		MOV	effect_proc[3*4], EAX
		MOV	EAX, effect3BGR555
		MOV	effect_proc[4*4], EAX
		MOV	EAX, effect4BGR555
		MOV	effect_proc[5*4], EAX
		MOV	EAX, effect5BGR555
		MOV	effect_proc[6*4], EAX
		MOV	EAX, effect6BGR555
		MOV	effect_proc[7*4], EAX
		MOV	EAX, effect7BGR555
		MOV	effect_proc[8*4], EAX

		MOV	blue_mask, 001fh
		MOV	green_mask, 03e0h
		MOV	red_mask, 7c00h
		MOV	blue_shift, 11
		MOV	green_shift, 6
		MOV	red_shift, 1

		MOV	log_alpha_mask[0*2], 0111111111111111b
		MOV	log_alpha_mask[1*2], 0011110111101111b
		MOV	log_alpha_mask[2*2], 0001110011100111b
		MOV	log_alpha_mask[3*2], 0000110001100011b
		MOV	log_alpha_mask[4*2], 0000010000100001b
		MOV	log_alpha_mask[5*2], 0000000000000000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		CALL	INITeffectsBGR555
		JMP	@@initEffectEnd

@@initEffect3:	; assume EAX, PIXFORM_BGR_565
		MOV	EAX, makePixelBGR565
		MOV	make_pixel_proc, EAX
		MOV	EAX, decodePixelBGR565
		MOV	decode_pixel_proc, EAX
		MOV	EAX, shadowBGR565	; shadowBGR565
		MOV	effect_proc, EAX
		MOV	EAX, effect0BGR565
		MOV	effect_proc[1*4], EAX
		MOV	EAX, effect1BGR565
		MOV	effect_proc[2*4], EAX
		MOV	EAX, effect2BGR565
		MOV	effect_proc[3*4], EAX
		MOV	EAX, effect3BGR565
		MOV	effect_proc[4*4], EAX
		MOV	EAX, effect4BGR565
		MOV	effect_proc[5*4], EAX
		MOV	EAX, effect5BGR565
		MOV	effect_proc[6*4], EAX
		MOV	EAX, effect6BGR565
		MOV	effect_proc[7*4], EAX
		MOV	EAX, effect7BGR565
		MOV	effect_proc[8*4], EAX

		MOV	blue_mask, 001fh
		MOV	green_mask, 07e0h
		MOV	red_mask, 0f800h
		MOV	blue_shift, 11
		MOV	green_shift, 5
		MOV	red_shift, 0

		MOV	log_alpha_mask[0*2], 1111111111111111b
		MOV	log_alpha_mask[1*2], 0111101111101111b
		MOV	log_alpha_mask[2*2], 0011100111100111b
		MOV	log_alpha_mask[3*2], 0001100011100011b
		MOV	log_alpha_mask[4*2], 0000100001100001b
		MOV	log_alpha_mask[5*2], 0000000000100000b
		MOV	log_alpha_mask[6*2], 0000000000000000b
		MOV	log_alpha_mask[7*2], 0000000000000000b

		CALL	INITeffectsBGR565
		; JMP	@@initEffectEnd

@@initEffectEnd:
		ENDPROC
INITeffect	ENDP


; AL = effect id, [EDI] = pixel to be affected
IMGeffect	PROC
		PUSH	EAX
		PUSH	EBX
		MOVZX	EBX, AL
		MOV	AX, [EDI]
		CALL	effect_proc[EBX*4 - MIN_EFFECT_CODE*4]
		MOV	[EDI], AX
		POP	EBX
		POP	EAX
		RET
IMGeffect	ENDP

; This one is called from C
PUBLIC doIMGeffect
doIMGeffect PROC id,pixel
  PUSH EAX
  PUSH EDI
  MOV EAX, id
  ADD AL, MIN_EFFECT_CODE
  MOV EDI, pixel
  CALL IMGeffect
  POP EDI
  POP EAX
  RET
doIMGeffect ENDP


;------- BEGIN OF FUNCTION IMGmakePixel ------------
;
; convert rgb value to pixel
;
; Syntax : int IMGmakePixel(rgb)
;
; rgb  - 24-bit color value (red in low 8 bit)
; return pixel specified by pixel_format_flag
;
IMGmakePixel	PROC	rgb
		MOV	EAX, rgb
		CALL	make_pixel_proc
		RET
IMGmakePixel	ENDP

;
;------- BEGIN OF FUNCTION IMGdecodePixel ------------
;
; convert pixel back to rgb
;
; Syntax : int IMGdecodePixel(pixel)
;
; pixel  - pixel in pixel format specified by pixel_format_flag
; return rgb value
;
IMGdecodePixel	PROC	pixel
		MOV	EAX, pixel
		CALL	decode_pixel_proc
		RET
IMGdecodePixel	ENDP


makePixelRGB555	PROC
		ROR EAX, 16	; GGGGGggg:RRRRRrrr:........:BBBBBbbb
		SHR AL, 3	; GGGGGggg:RRRRRrrr:........:000BBBBB
		ROL EAX, 8	; RRRRRrrr:........:000BBBBB:GGGGGggg
		SHR AX, 3	; RRRRRrrr:........:000000BB:BBBGGGGG
		ROL EAX, 5	; rrr.....:........:0BBBBBGG:GGGRRRRR
		RET
makePixelRGB555	ENDP

decodePixelRGB555	PROC
		PUSH	EDX
		MOV EDX, EAX	;     ........:........:.BBBBBGG:GGGRRRRR
		SHL EAX, 9	; EAX=........:BBBBBGGG:GGRRRRR0:00000000
		SHL EDX, 6	; EDX=........:...BBBBB:GGGGGRRR:RR000000
		MOV AH, DH	; EAX=........:BBBBBggg:GGGGGrrr:00000000
		SHR EDX, 3	; EDX=........:......BB:BBBGGGGG:RRRRR000
		MOV AL, DL	; EAX=........:BBBBBggg:GGGGGrrr:RRRRR000
		POP	EDX
		RET
decodePixelRGB555	ENDP

INITeffectsRGB555	PROC
		PUSHAD
		MOV	ECX, 0
@@INITeffectsRGB555L1:
		MOV	EAX, ECX
		INC	ECX
		CMP	ECX, 32
		JB	@@INITeffectsRGB555L1
		POPAD
		RET
INITeffectsRGB555	ENDP

shadowRGB555	PROC	; dim filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bits
		RET
shadowRGB555	ENDP

effect0RGB555	PROC	; dim filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b  ; remove last bit
		RET
effect0RGB555	ENDP

effect1RGB555	PROC	; blue filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100000000000000b	; (1-0.5)*(1.0b, 0.0g, 0.0r)
		RET
effect1RGB555	ENDP

effect2RGB555	PROC	; green filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0000001000000000b	; (1-0.5)*(0.0b, 0.0g, 0.0r)
		RET
effect2RGB555	ENDP

effect3RGB555	PROC	; cyan filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100001000000000b	; (1-0.5)*(1.0b, 1.0g, 0.0r)
		RET
effect3RGB555	ENDP

effect4RGB555	PROC	; red filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0000000000010000b	; (1-0.5)*(0.0b, 0.0g, 1.0r)
		RET
effect4RGB555	ENDP

effect5RGB555	PROC		; purple filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100000000010000b	; (1-0.5)*(1.0b, 0.0g, 1.0r)
		RET
effect5RGB555	ENDP

effect6RGB555	PROC		; yellow filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0000001000010000b	; (1-0.5)*(0.0b, 1.0g, 1.0r)
		RET
effect6RGB555	ENDP

effect7RGB555	PROC		; grey scale filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100001000010000b	; (1-0.5)*(1.0b, 1.0g, 1.0r)
		RET
effect7RGB555	ENDP

makePixelRGB565	PROC
		ROR EAX, 16	; GGGGGGgg:RRRRRrrr:........:BBBBBbbb
		SHR AL, 3	; GGGGGGgg:RRRRRrrr:........:000BBBBB
		ROL EAX, 8	; RRRRRrrr:........:000BBBBB:GGGGGGgg
		SHR AX, 2	; RRRRRrrr:........:00000BBB:BBGGGGGG
		ROL EAX, 5	; rrr.....:........:BBBBBGGG:GGGRRRRR
		RET
makePixelRGB565	ENDP

decodePixelRGB565	PROC
		PUSH	EDX
		MOV EDX, EAX	;     ........:........:BBBBBGGG:GGGRRRRR
		SHL EAX, 8	; EAX=........:BBBBBGGG:GGGRRRRR:00000000
		SHL EDX, 5	; EDX=........:...BBBBB:GGGGGGRR:RRR00000
		MOV AH, DH	; EAX=........:BBBBBggg:GGGGGGrr:00000000
		SHR EDX, 2	; EDX=........:.....BBB:BBGGGGGG:RRRRR000
		MOV AL, DL	; EAX=........:BBBBBggg:GGGGGGrr:RRRRR000
		POP	EDX
		RET
decodePixelRGB565	ENDP

INITeffectsRGB565	PROC
		RET
INITeffectsRGB565	ENDP

shadowRGB565	PROC
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last two bits
		RET
shadowRGB565	ENDP

effect0RGB565	PROC
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		RET
effect0RGB565	ENDP

effect1RGB565	PROC		; blue filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000000000000000b	; (1-0.5)*(1.0b, 0.0g, 0.0r)
		RET
effect1RGB565	ENDP

effect2RGB565	PROC		; green filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 0000010000000000b	; (1-0.5)*(0.0b, 1.0g, 0.0r)
		RET
effect2RGB565	ENDP

effect3RGB565	PROC		; cyan filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000010000000000b	; (1-0.5)*(1.0b, 1.0g, 0.0r)
		RET
effect3RGB565	ENDP

effect4RGB565	PROC	; red filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 0000000000010000b	; (1-0.5)*(0.0b, 0.0g, 1.0r)
		RET
effect4RGB565	ENDP

effect5RGB565	PROC		; purple filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000000000010000b	; (1-0.5)*(1.0b, 0.0g, 1.0r)
		RET
effect5RGB565	ENDP

effect6RGB565	PROC		; yellow filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 0000010000010000b	; (1-0.5)*(0.0b, 1.0g, 1.0r)
		RET
effect6RGB565	ENDP

effect7RGB565	PROC
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000010000010000b	; (1-0.5)*(1.0b, 1.0g, 1.0r)
		RET
effect7RGB565	ENDP

makePixelBGR555	PROC
		BSWAP EAX	; RRRRRrrr:GGGGGggg:BBBBBbbb:........
		ROL EAX, 8	; GGGGGggg:BBBBBbbb:........:RRRRRrrr
		SHR AL, 3	; GGGGGggg:BBBBBbbb:........:000RRRRR
		ROL EAX, 8	; BBBBBbbb:........:000RRRRR:GGGGGggg
		SHR AX, 3	; BBBBBbbb:........:000000RR:RRRGGGGG
		ROL EAX, 5	; bbb.....:........:0RRRRRGG:GGGBBBBB
		RET
makePixelBGR555	ENDP

decodePixelBGR555	PROC
		PUSH	EDX
		MOV EDX, EAX	;     ........:........:.RRRRRGG:GGGBBBBB
		SHL EAX, 19	; EAX=RRRGGGGG:BBBBB000:00000000:00000000
		SHL EDX, 6	; EDX=........:...RRRRR:GGGGGBBB:BB000000
		MOV AH, DH	; EAX=........:BBBBB000:GGGGGbbb:00000000
		SHR EDX, 13	; EDX=........:........:........:RRRRRGGG
		MOV AL, DL	; EAX=........:BBBBB000:GGGGGbbb:RRRRRggg
		POP	EDX
		RET
decodePixelBGR555	ENDP

INITeffectsBGR555	PROC
		RET
INITeffectsBGR555	ENDP

shadowBGR555	PROC		; dim filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		RET
shadowBGR555	ENDP

effect0BGR555	PROC		; dim filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		RET
effect0BGR555	ENDP

effect1BGR555	PROC		; blue filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0000000000010000b	; (1-0.5)*(0.0r, 0.0g, 1.0b)
		RET
effect1BGR555	ENDP

effect2BGR555	PROC		; green filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0000001000000000b	; (1-0.5)*(0.0r, 1.0g, 0.0b)
		RET
effect2BGR555	ENDP

effect3BGR555	PROC		; cyan filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0000001000010000b	; (1-0.5)*(0.0r, 1.0g, 1.0b)
		RET
effect3BGR555	ENDP

effect4BGR555	PROC		; red filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100000000000000b	; (1-0.5)*(1.0r, 0.0g, 0.0b)
		RET
effect4BGR555	ENDP

effect5BGR555	PROC		; purple filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100000000010000b	; (1-0.5)*(1.0r, 0.0g, 1.0b)
		RET
effect5BGR555	ENDP

effect6BGR555	PROC		; yellow filter
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100001000000000b	; (1-0.5)*(1.0r, 1.0g, 0.0b)
		RET
effect6BGR555	ENDP

effect7BGR555	PROC
		SHR	EAX, 1
		AND	EAX, 0011110111101111b	; remove last bit
		ADD	EAX, 0100001000010000b	; (1-0.5)*(1.0r, 1.0g, 1.0b)
		RET
effect7BGR555	ENDP

makePixelBGR565	PROC
		BSWAP EAX	; RRRRRrrr:GGGGGGgg:BBBBBbbb:........
		ROL EAX, 8	; GGGGGGgg:BBBBBbbb:........:RRRRRrrr
		SHR AL, 3	; GGGGGGgg:BBBBBbbb:........:000RRRRR
		ROL EAX, 8	; BBBBBbbb:........:000RRRRR:GGGGGGgg
		SHR AX, 2	; BBBBBbbb:........:00000RRR:RRGGGGGG
		ROL EAX, 5	; bbb.....:........:RRRRRGGG:GGGBBBBB
		RET
makePixelBGR565	ENDP

decodePixelBGR565	PROC
		PUSH	EDX
		MOV EDX, EAX	;     ........:........:RRRRRGGG:GGGBBBBB
		SHL EAX, 19	; EAX=RRGGGGGG:BBBBB000:00000000:00000000
		SHL EDX, 5	; EDX=........:...RRRRR:GGGGGGBB:BBB00000
		MOV AH, DH	; EAX=........:BBBBB000:GGGGGGbb:00000000
		SHR EDX, 13	; EDX=........:........:........:RRRRRGGG
		MOV AL, DL	; EAX=........:BBBBB000:GGGGGGbb:RRRRRggg
		POP	EDX
		RET
decodePixelBGR565	ENDP

INITeffectsBGR565	PROC
		RET
INITeffectsBGR565	ENDP

shadowBGR565	PROC		; dim filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last two bits
		RET
shadowBGR565	ENDP

effect0BGR565	PROC		; dim filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		RET
effect0BGR565	ENDP

effect1BGR565	PROC		; blue filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 0000000000010000b	; (1-0.5)*(0.0r, 0.0g, 1.0b)
		RET
effect1BGR565	ENDP

effect2BGR565	PROC	; green filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 0000010000000000b	; (1-0.5)*(0.0r, 1.0g, 0.0b)
		RET
effect2BGR565	ENDP

effect3BGR565	PROC	; cyan filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 0000010000010000b	; (1-0.5)*(0.0r, 1.0g, 1.0b)
		RET
effect3BGR565	ENDP

effect4BGR565	PROC		; red filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000000000000000b	; (1-0.5)*(1.0r, 0.0g, 0.0b)
		RET
effect4BGR565	ENDP

effect5BGR565	PROC		; purple filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000000000010000b	; (1-0.5)*(1.0r, 0.0g, 1.0b)
		RET
effect5BGR565	ENDP

effect6BGR565	PROC		; yellow filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000010000000000b	; (1-0.5)*(1.0r, 1.0g, 0.0b)
		RET
effect6BGR565	ENDP

effect7BGR565	PROC		; white filter
		SHR	EAX, 1
		AND	EAX, 0111101111101111b	; remove last bit
		ADD	EAX, 1000010000010000b	; (1-0.5)*(1.0r, 1.0g, 1.0b)
		RET
effect7BGR565	ENDP

		END
