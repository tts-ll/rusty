define void @main() {

%1 = mul i32 2 , 4 ;
%2 = add i32 1 , %1 ;
%3 = sdiv i32 4 , 2 ;
%4 = add i32 1 , %3 ;

br label %loop1 

loop1:

%5 = mul i32 2 , 4 ;
%6 = add i32 1 , %5 ;
%7 = sub i32 1 , 3 ;
%8 = sdiv i32 8 , 2 ;
%9 = sub i32 %8 , 1 ;
%10 = icmp sge i32 3 , %9 ;

br label %loop1 

;printi not yet implemented!!

ret void
}
declare i32 @puts(i8*)
