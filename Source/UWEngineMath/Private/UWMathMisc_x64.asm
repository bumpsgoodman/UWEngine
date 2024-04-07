; /*
;  * Math misc x64 어셈
;  * 작성자: bumpsgoodman
;  * 작성일: 2023/12/22
; */

; __vectorcall:
;   매개 변수가 크기가 8바이트로 고정
;   왼쪽에서 오른쪽으로 처음 6개 할당 (xmm0 : xmm5) 

.code

; UWMETHOD(int) Log2Int64(const uint64_t num);
; num = rcx
Log2Int64 PROC EXPORT
    bsr rax, rcx
    ret
Log2Int64 ENDP

END