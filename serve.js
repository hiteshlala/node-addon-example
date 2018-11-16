const koa = require( 'koa' );
const Router = require( 'koa-router' );
const body = require( 'koa-body' );
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

const app = new koa();

const home = new Router();

home.get( '/', ctx => {
  console.log( ctx.method, ctx.url, ctx.ip, ctx.host, ctx.protocol );
  ctx.body = html;
  return ctx.body;  
});

home.post( '/', async ctx => {
  console.log( ctx.method, ctx.url, ctx.ip, ctx.host, ctx.protocol );
  const lib = ctx.request.body.type;
  let result = "";
  let start = Date.now();
  let time; 
  try {
    switch( lib ) {
      case 'nbind':
        result = await asyncfunc( 'nbind', 50 );
        break;
      case 'regular':
        result = await asyncfunc( 'regular', 50 );
        break;
      case 'nan':
        result = await asyncfunc( 'nan', 50 );
        break;
      default:
        result = "None"
    }
    time = Date.now() - start;
    ctx.body = { result, time, type: lib, async: true };
  }
  catch( e ) {
    time = Date.now() - start;
    ctx.body = { result, time, type: lib, async: true };
  }
  return ctx.body;
});

home.post ( '/sync', ctx => {
  console.log( ctx.method, ctx.url, ctx.ip, ctx.host, ctx.protocol );
  const lib = ctx.request.body.type;
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
    ctx.body = { result, time, type: lib, async: false };
  }
  catch( e ) {
    time = Date.now() - start;
    ctx.body = { result, time, type: lib, async: false };
  }
  return ctx.body;
});

app.use( body({ multipart: true }) );
app.use( home.routes() );
app.use( home.allowedMethods() );

/*
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
*/

app.on( 'error', error => {
  console.log( 'Error message:', error.message );
  console.log( 'Error code:', error.code );
  console.log( 'Error stack:', error.stack );
});

app.listen( 7003 );
console.log( 'on port ', 7003 );


