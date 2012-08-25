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

;Filename    : I_CTRL.ASM
;Description : VGA image buffer control routines


INCLUDE all.inc

.DATA

;--------------------------------------------------

public	image_buf, image_width, image_height

image_buf       DD      ?
image_width     DD      ?
image_height    DD      ?

;--------------------------------------------------


.CODE

;------- BEGIN OF FUNCTION IMGinit ------------
;
; Initialize the settings of the image buffer
;
; Syntax : IMGinit(char*, int, int, int)
;
; int  imageWidth  - width and height of the image buffer
; int  imageHeight
;
		PUBLIC  IMGinit
IMGinit         PROC    imageWidth, imageHeight
		STARTPROC

		MOV	EAX, imageWidth
		MOV	image_width, EAX

		MOV	EAX, imageHeight
		MOV	image_height, EAX

		ENDPROC
IMGinit         ENDP

;---------- END OF FUNCTION IMGinit ---------

END
