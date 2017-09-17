const request = require('request');

module.exports = {
  
  playFirstVideo: function(callback) {
    request( {url:`http://${config.firstVideo}/play`, timeout: +config.reqTimeout, qs: {id: 1 } }, (error, response, body) => {
      if(error) {
        callback(error, null);
      } else if (response.statusCode == 200) {
        callback(null, "OK");
      } else {
        const error = new Error();
        error.message = response ? response.statusCode : 500;
        callback(error, null);
      }
    });    
  },
  
 resetFirstVideo: function(callback) {
    request( {url:`http://${config.firstVideo}/reset`, timeout: +config.reqTimeout}, (error, response, body) => {
      if(error) {
        callback(error, null);
      } else if (response.statusCode == 200) {
        callback(null, "OK");
      } else {
        const error = new Error();
        error.message = response ? response.statusCode : 500;
        callback(error, null);
      }
    });    
  },
  
  testFirstVideo: function(callback) {
    request( {url:`http://${config.firstVideo}/status`, timeout: +config.reqTimeout }, (error, response, body) => {
      try{
        callback(null, JSON.parse(body).status); 
      } catch (e){
        const error = new Error();
        error.message = response ? response.statusCode : 500;
        callback(error, null);
      }
    });
  },
  
  playCamsVideo: function(scr, id, callback) {
    request( {url:`http://${config.camsVideo}/play`, timeout: +config.reqTimeout , qs: { scr: scr, id: id} }, (error, response, body) => {
      if(error) {
        callback(error, null);
      } else if (response.statusCode == 200) {
        callback(null, "OK");
      } else {
        const error = new Error();
        error.message = response ? response.statusCode : 500;
        callback(error, null);
      }
    });
  },
    
  testCamsVideo: function(callback){
    request( {url:`http://${config.camsVideo}/status`, timeout: +config.reqTimeout }, (error, response, body) => {
      try{
        callback(null, JSON.parse(body).status); 
      } catch (e){
        const error = new Error();
        error.message = response ? response.statusCode : 500;
        callback(error, null);
      }
    });    
  },
    
  powerCams: function(callback){
    request( {url:`http://${config.camsVideo}/shutdown`, timeout: +config.reqTimeout }, (error, response, body) => {
      try{
        callback(null, JSON.parse(body).status); 
      } catch (e){
        const error = new Error();
        error.message = response ? response.statusCode : 500;
        callback(error, null);
      }
    });    
  },
    
  lightsBoss: function(state, callback){
    request( {url:`http://${config.camsVideo}/light`, timeout: +config.reqTimeout, qs: { state: state} }, (error, response, body) => {
      try{
        console.log('lights: '+body)
          console.log('lights: '+response)
        callback(null, JSON.parse(body).status); 
      } catch (e){
        //const error = new Error();
        //error.message = response ? response.statusCode : 500;
        callback(e, null);
      }
    });    
  },
    
  powerFirst: function(callback){
    request( {url:`http://${config.firstVideo}/shutdown`, timeout: +config.reqTimeout }, (error, response, body) => {
      try{
        callback(null, JSON.parse(body).status); 
      } catch (e){
        const error = new Error();
        error.message = response ? response.statusCode : 500;
        callback(error, null);
      }
    });    
  }
    
}