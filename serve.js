const koa = require( 'koa' );
const route = require( 'koa-router' );
const body = require( 'koa-body' );
const fs = require( 'fs' );

const addOnNbind = require( './nbindaddon/build/Release/nbind.node' );
const addOnRegular = require( './regularnodeaddon/build/Release/asyncfuncs.node' );

function asyncfunc( lib, time ) {
  let f = addOnRegular;
  
  if ( lib == 'nbind') {
    f = addOnNbind;
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

const html = fs.readFileSync( 'index.html', 'utf8' );

const app = new koa();

app.use( body({ multipart: true }) );

app.use( async ctx => {
  console.log( ctx.method, ctx.path );
  console.log( ctx.request.body );
  if ( ctx.method === 'GET' ) {
    ctx.body = html;
  }
  else if ( ctx.method === 'POST' ) {
    let result = "";
    let start = Date.now();
    let time;
    switch( ctx.path ) {
      case '/nbind':
        result = await asyncfunc( 'nbind', 50 );
        break;
      case '/regular':
        result = await asyncfunc( 'regular', 50 );
        break;
      case '/nan':
      default:
        result = "None"
    }
    time = Date.now() - start;
    ctx.body = { result, time };
  }

  return ctx.body;
});


// app.use( ctx => {
//   console.log( ctx.method, ctx.path );
//   console.log( ctx.request.body );
//   if ( ctx.method === 'GET' ) {
//     ctx.body = html;
//   }
//   else if ( ctx.method === 'POST' ) {
//     let result = "";
//     let start = Date.now();
//     let time;
//     switch( ctx.path ) {
//       case '/nbind':
//       console.log( 'get to nbind')
//         addOnNbind.longRunFunction( 50, ( err, res ) => {
//           ctx.body = {
//             result: res,
//             time: Date.now() - start
//           };
//           console.log( 'callback ', ctx.body.time)
//         })
//         break;
//       case '/regular':
//       console.log( 'get to regular')
//         addOnRegular.longRunFunction( 50, ( err, res ) => {
//           ctx.body = {
//             result: res,
//             time: Date.now() - start
//           };
//         })
//         break;
//       case '/nan':
//       default:
//         ctx.body = {
//           result: 'None',
//           time: Date.now() - start
//         };
//     }
//     // time = Date.now() - start;
//     // ctx.body = { result, time };
//   }

//   return ctx.body;
// });





app.on( 'error', error => {
  console.log( 'Error message:', error.message );
  console.log( 'Error code:', error.code );
  console.log( 'Error stack:', error.stack );
});

app.listen( 7003 );
console.log( 'on port ', 7003 );


