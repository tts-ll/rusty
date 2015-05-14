struct Point{ x: i32 , y: i32}



fn foo(x:i32, y:i32, z:bool)->i32{

	
	return 100;
}


fn main() {

	let x : [ [i32;2] ; 2] = [ [1,2] , [2,3] ];
	let y : i32 = 2;
	let n : i32 = 20;
	let b : bool = true;
	let P : Point = Point{x: 2 , y: 4};	
	
	let hello = x;
	let t1 : Point;
	let t2 : i32;


	while( n < 2 || n == 1 ){

		loop{
		x[1][0] %= y+1+x[0][0];	
		};
	};
	
	if( n == 5){
	
		x[0][1] += 1;

	}	
	else{
		
		x[1][1] += 5;	
	
	};

	
	if(y > 0){
		y += 1;
	};

  
	foo( x[0][1] - 4*y , y , b );
	
	P.x = 4-1;
	y = P.y + 1;	


	
}

