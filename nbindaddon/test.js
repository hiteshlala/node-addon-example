let funcs = require( './build/Release/nbind' );

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

funcs.sayHello( 'test' );

function progress( p ) {
  console.log( 'Progress:', p );
}

funcs.longRunFuncWithProgress( 3000, 
  ( err, res ) => {
    if( err ) {
      console.log( 'Error', err );
    }
    else {
      console.log( 'Long Running Function With Progress Done:', res );
    }
  }, 
  progress 
);
