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

; Filename    : colcode.inc
; Description : ASM include file for defining special colour
;               see/update also COLCODE.H for C

TRANSPARENT_CODE        = 255

extrn transparent_code_w:word		; key color in pixel format

MANY_TRANSPARENT_CODE   = 0f8h
MIN_TRANSPARENT_CODE    = 0f8h
MAX_TRASNPARENT_CODE    = 0ffh

; never appear color code 0
SHADOW_CODE             =  000h

POST_REMAP MACRO
	MOV	AX, [EBX+2*EAX]
	ENDM

MIN_EFFECT_CODE = 0efh
MAX_EFFECT_CODE = 0f7h

