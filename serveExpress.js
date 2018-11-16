const Express = require( 'express' );
const bodyParser = require('body-parser');
const fs = require( 'fs' );

const addOnNbind = require( './nbindaddon/build/Release/nbind.node' );
const addOnRegular = require( './regularnodeaddon/build/Release/asyncfuncs.node' );
const addOnNan = require( './nanaddon/build/Release/asyncfuncs.node' );

function asyncfunc( lib, time ) {
  let f = addOnRegular;
  
  if ( lib == 'nbind') {
    f = addOnNbind;
  }
  else if ( lib === 'nan' ) {
    f= addOnNan;
  }

  return new Promise(( resolve, reject ) => {
    f.longRunFunction( time, ( err, res ) => {
      if( err ) {
        reject( err );
      }
      else {
        resolve( res )
      }
    });
  });
}

function syncfunc( lib, time ) {
  let f = addOnRegular;
  
  if ( lib == 'nbind') {
    f = addOnNbind;
  }
  else if ( lib === 'nan' ) {
    f= addOnNan;
  }

  return f.sayHello( lib );
}

const html = fs.readFileSync( 'index.html', 'utf8' );


let app = new Express();

app.use(bodyParser.json());

app.get( '/', ( req, res ) => {
  console.log( req.method, req.baseUrl, req.ip, req.hostname, req.protocol );
  res.send( html );
});

// app.post( '/', async ( req, res ) => {
//   console.log( req.method, req.baseUrl, req.ip, req.hostname, req.protocol );
//   const lib = req.body.type;
//   let result = "";
//   let start = Date.now();
//   let time; 
//   try {
//     switch( lib ) {
//       case 'nbind':
//         result = await asyncfunc( 'nbind', 50 );
//         break;
//       case 'regular':
//         result = await asyncfunc( 'regular', 50 );
//         break;
//       case 'nan':
//         result = await asyncfunc( 'nan', 50 );
//         break;
//       default:
//         result = "None"
//     }
//     time = Date.now() - start;
//     res.json({ result, time, type: lib, async: true } );
//   }
//   catch( e ) {
//     time = Date.now() - start;
//     res.json( { result, time, type: lib, async: true } );
//   }
// });


app.post( '/', ( req, res ) => {
  console.log( req.method, req.baseUrl, req.ip, req.hostname, req.protocol );
  const lib = req.body.type;
  let start = Date.now();
  let time = 50; 
  try {
    switch( lib ) {
      case 'nbind':
        addOnNbind.longRunFunction( time, ( e, r ) => {
          if( e ) {
            res.json({ result: r, time:Date.now() - start, type: lib, async: true } );
          }
          else {
            res.json({ result: r, time:Date.now() - start, type: lib, async: true } );
          }
        });
        break;
      case 'regular':
        addOnRegular.longRunFunction( time, ( e, r ) => {
          if( e ) {
            res.json({ result: r, time:Date.now() - start, type: lib, async: true } );
          }
          else {
            res.json({ result: r, time:Date.now() - start, type: lib, async: true } );
          }
        });
        break;
      case 'nan':
        addOnNan.longRunFunction( time, ( e, r ) => {
          if( e ) {
            res.json({ result: r, time:Date.now() - start, type: lib, async: true } );
          }
          else {
            res.json({ result: r, time:Date.now() - start, type: lib, async: true } );
          }
        });
        break;
      default:
        res.json({ result:"", time:Date.now() - start, type: lib, async: true } );
    }
    
  }
  catch( e ) {
    res.json( { result:e.message, time:Date.now() - start, type: lib, async: true } );
  }
});

app.post ( '/sync', ( req, res ) => {
  console.log( req.method, req.baseUrl, req.ip, req.hostname, req.protocol );
  const lib = req.body.type;
  let result = "";
  let start = Date.now();
  let time; 
  try {
    switch( lib ) {
      case 'nbind':
        result = syncfunc( 'nbind' );
        break;
      case 'regular':
        result = syncfunc( 'regular' );
        break;
      case 'nan':
        result = syncfunc( 'nan' );
        break;
      default:
        result = "None"
    }
    time = Date.now() - start ;
    res.json({ result, time, type: lib, async: false } );
  }
  catch( e ) {
    time = Date.now() - start;
    res.json({ result, time, type: lib, async: false } );
  }
});



app.on('error', ( err, ctx ) => {
  console.log(`Error Message: ${err.message}`);
  console.log(`Error Code: ${err.code}`);
  console.log(`Error Stack: ${err.stack}`);
});


app.listen( 7004, () => console.log( 'listening on port 7004' ) );


