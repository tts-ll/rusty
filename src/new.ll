@c1 = constant [2 x i8] c"h\00"
@c2 = constant [12 x i8] c"hello world\00"
define void @main() {

%2 = call i32 @puts(i8* 1)
%1 = call i32 @puts(i8* getelementptr inbounds ([12 x i8]* @c2, i32 0, i32 0))
ret void
}
declare i32 @puts(i8*)
