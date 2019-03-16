let funcs = require( './build/Release/asyncfuncs' ); 

console.log( funcs );

console.log( 'Calling longRunFunction with 5s delay.\n' );

funcs.longRunFunction( 5000, ( err, res ) => {
  if( err ) {
    console.log( 'Error', err );
  }
  else {
    console.log( 'Long Running Function Done:', res )
  }
});

console.log( '\nThis should print before the callback is called\n' );

console.log( funcs.sayHello( 'test' ) );