const SerialPort = require('serialport');
const videos = require('./req');
const light = require('./req');
const led = require('./led.js');

let generalLightsBusy = false;
let musicOn = false;
let guardsBusy = false;
const boomSound = document.getElementById("boom");
let cardsPlayingTimer;
const lightsTimeouts = [];
const musicTimeouts = [];
/*
1  - General   - view black
2  - General   - view light
3  - Cards     - Playing
4  - Cards     - Lights on
5  - Cards     - Music on
6  - Cards     - Nobody there black
7  - Cards     - Nobody there lights on
8  - Door      - Breaking the door loop
9  - Door      - guards arrived
10 - Door      - Door
11 - Execution - Process
12 - Execution - Wait
*/

const GENERAL_BLACK = 1;
const GENERAL_LIGHT = 2;
const CARDS_PLAYING = 3;
const CARDS_LIGHTS = 4;
const CARDS_MUSIC = 5;
const CARDS_NOBODY_BLACK = 6;
const CARDS_NOBODY_LIGHTS = 7;
const DOOR_BREAK = 8;
const DOOR_GUARDS = 9;
const DOOR = 10;
const EXEC_PROCESS = 11;
const EXEC_WAIT = 12;

const status = {
  isError: true
};

let port = {};


const serial = {
  
  init: function () {
    
    port = new SerialPort(config.port.name, {
      parser: SerialPort.parsers.readline('\r\n'),
      baudrate: +config.port.speed,
      autoOpen: true
    });
    
    port.on('error', (error) => {
      console.log(error);
      led.toggleLed('.row.main > .status-led', 'red');
    });
    
    port.on('open', () => {
      led.toggleLed('.row.main > .status-led', 'yellow');
    });
    
    port.on('disconnect', (error) => {
      led.toggleLed('.row.main > .status-led', 'red');
    });
    
    port.on('data', data => {
      data = data.replace(/^.+?({.+\})$/, "$1")
      let input = "";
      
      try {
        console.log(data);
        input = JSON.parse(data);
      } catch (error) {
        console.log(error);
        led.toggleLed('.row.main > .status-led', 'red');
      }
      
      console.log(input);
      if (input) {
        if (input.status === 'on air') {
          led.toggleLed('.row.main > .status-led', 'green');
          
        } else if (input.status === 'OK') {
          led.toggleLed('.row.main > .test-led', 'green');
          
        } else if (input.cigarettes === 'opened' || input.cigarettes === 'closed' || input.cigarettes === 'triggered' || input.cigarettes === 'voice' || input.cigarettes === 'auto' || input.cigarettes === 'manual') {
          led.toggleLed('.row.sigarets > .status-led', 'green');
          
        } else if (input.lock === 'opened' || input.lock === 'armed' || input.lock === 'triggered') {
          led.toggleLed('.row.lock > .status-led', 'green');
          
        } else if (input.tape === 'opened' || input.tape === 'closed' || input.tape === 'triggered' || input.tape === 'voice') {
          led.toggleLed('.row.scotch > .status-led', 'green');
          
        } else if (input.cigarettes === 'OK') {
          led.toggleLed('.row.sigarets > .test-led', 'green');
          
        } else if (input.cigarettes === 'error') {
          led.toggleLed('.row.sigarets > .status-led', 'red');
          
        } else if (input.tape === 'OK') {
          led.toggleLed('.row.scotch > .test-led', 'green');
          
        } else if (input.tape === 'error') {
          led.toggleLed('.row.scotch > .status-led', 'red');
          
        } else if (input.lock === 'OK') {
          led.toggleLed('.row.lock > .test-led', 'green');
          
        } else if (input.lock) {
          if (input.lock === 'error') {
            led.toggleLed('.row.lock > .status-led', 'red');
            
          } else {
            led.toggleLed('.row.lock > .status-led', 'green');
          }
          
          if (lockChargeReq == 'battery') {
            $('.row.lock > .battery').text(`Заряд: ${input.lock}%`)
            
          } else if (lockChargeReq == 'gun') {
            $('.row.lock > .gun').text(`/ ${input.lock}%`)
          }
          lockChargeReq = ''
          
        } else if (input.gate === 'opened' || input.gate === 'closed' || input.gate === 'triggered') {
          led.toggleLed('.row.grid > .status-led', 'green');
          
        } else if (input.gate === 'error') {
          led.toggleLed('.row.grid > .status-led', 'red');
          
        } else if (input.gate === 'boom') {
          bossShutDown();
        } else if (input.fan === 'stoped' || input.fan === 'started') {
          led.toggleLed('.row.fan > .status-led', 'green');
          
        } else if (input.fan === 'error') {
          led.toggleLed('.row.fan > .status-led', 'red');
          
        } else if (input.door === 'played' || input.door === 'triggered' || input.door === 'OK') {
          led.toggleLed('.row.door > .status-led', 'green');
          
        } else if (input.door === 'error') {
          led.toggleLed('.row.door > .status-led', 'red');
          
        } else if (input.post === 'triggered' || input.post === 'opened' || input.post === 'closed') {
          led.toggleLed('.row.post > .status-led', 'green');
      
        }  else if (input.post === 'error') {
          led.toggleLed('.row.post > .status-led', 'red');
      
        } else if (input.doorKnocks) {
          // alarma
          musicAlarm();
          
        } else if (input.boss === 'OK') {
          led.toggleLed('.row.grid > .test-led', 'green');
          led.toggleLed('.row.fan > .test-led', 'green');
          led.toggleLed('.row.door > .test-led', 'green');
          led.toggleLed('.row.vynyl > .test-led', 'green');
          led.toggleLed('.row.cam1 > .test-led', 'green');
          led.toggleLed('.row.cam2 > .test-led', 'green');
          led.toggleLed('.row.cam3 > .test-led', 'green');
          led.toggleLed('.row.cam4 > .test-led', 'green');
          
        } else if (input.ceilsBlockLights === 'on') {;
          musicTimeouts.forEach(item => clearTimeout(item));
          ceilBlockLightsOn(musicOn);

        } else if (input.ceilsBlockLights === 'off') {
          musicTimeouts.forEach(item => clearTimeout(item));
          ceilBlockLightsOff(musicOn);
          
        } else if (input.bossBlockLights === 'on') {
          light.lightsBoss(1, (error, res) => {
            console.log(error, res)
          });
          
        } else if (input.bossBlockLights === 'off') {
          light.lightsBoss(0, (error, res) => {
            console.log(error, res)
          });
          
        } else if (input.deathChair === 'on') {
          videos.playCamsVideo(4, EXEC_PROCESS, (error, res) => {
            console.log(error, res)
          })
          
        } else if (input.deathChair === 'off') {
          videos.playCamsVideo(4, EXEC_WAIT, (error, res) => {
            console.log(error, res)
          })
        }  else if (input.led_off === 'off') {
          led.toggleLed('.row.light > .status-led', 'green');
          generalLightsBusy = false;
        }
      }
      return;

    })
  },

  
  write: function (data, callback) {
    console.log("serial.write:", data);
    port.write(data + '\n', (error, data) => {
      callback(error, data);
    });
  },

  
  isOpen: function () {
    return port.isOpen();
  },
  
  
  checkTest: function () {
    if (status.isError) {
      console.log('test failed');
      led.toggleLed('.row.main > .status-led', 'red');
      led.toggleLed('.row.first-video > .status-led', 'red');
      led.toggleLed('.row.sigarets > .status-led', 'red');
      led.toggleLed('.row.scotch > .status-led', 'red');
      led.toggleLed('.row.vinyl > .status-led', 'red');
      led.toggleLed('.row.fan > .status-led', 'red');
      led.toggleLed('.row.grid > .status-led', 'red');
      led.toggleLed('.row.lock > .status-led', 'red');
      led.toggleLed('.row.sw1 > .status-led', 'red');
      led.toggleLed('.row.sw2 > .status-led', 'red');
      led.toggleLed('.row.sw3 > .status-led', 'red');
      led.toggleLed('.row.sw4 > .status-led', 'red');
    }
  }
};


function ceilBlockLightsOn( music ) {
  clearLightsTimeouts();
  videos.playCamsVideo(1, GENERAL_LIGHT, (error, res) => {
    console.log(error, res)
  })
  
  if( music ) {
    videos.playCamsVideo(3, CARDS_NOBODY_LIGHTS, (error, res) => {
      console.log(error, res)
    })
  } else {
    generalLightsBusy = true;
    lightsTimeouts.push( setTimeout( () => {
      videos.playCamsVideo(3, CARDS_LIGHTS, (error, res) => {
        console.log(error, res)
      })
    }, 200) );
    lightsTimeouts.push( setTimeout( () => {
      serial.write(JSON.stringify({
        led_off: 2
      }), (error, data) => {
        
      });
      videos.playCamsVideo(1, GENERAL_BLACK, (error, res) => {
        console.log(error, res)
      })
    }, 18000) );
    lightsTimeouts.push( cardsPlayingTimer = setTimeout( () => {
      
      videos.playCamsVideo(3, CARDS_PLAYING, (error, res) => {
        console.log(error, res)
      })
    }, 30000) );
  }
}


function ceilBlockLightsOff( music ) {
  clearLightsTimeouts();
  videos.playCamsVideo(1, GENERAL_BLACK, (error, res) => {
    console.log(error, res)
  })
  if( music  ){
    videos.playCamsVideo(3, CARDS_NOBODY_BLACK, (error, res) => {
      console.log(error, res)
    })
  } 
}


function musicAlarm() {
  clearLightsTimeouts();
  guardsBusy = true;
  musicOn = true;
  
  serial.write(JSON.stringify({
    led_off: 2
  }), (error, data) => {

  });
  
  videos.playCamsVideo(1, GENERAL_BLACK, (error, res) => {
    console.log(error, res)
  })
  
  // guards're hearing music
  videos.playCamsVideo(3, CARDS_MUSIC, (error, res) => {
    console.log(error, res)
  })
  
  // nobody in guards room / lights black
  musicTimeouts.push( setTimeout( () => {
    
    videos.playCamsVideo(3, CARDS_NOBODY_BLACK, (error, res) => {
      console.log(error, res)
    })
  }, 11000))
  
  //guards arrived
  setTimeout( () => {
    guardsBusy = false;
    videos.playCamsVideo(2, DOOR_GUARDS, (error, res) => {
      console.log(error, res)
    })
  }, 14000)
  
  // start knocking the door sound
  setTimeout( () => {
    serial.write(JSON.stringify({
      door: 'voice'
    }), (error, data) => {});
  }, 16000)
  
  // breaking cycle
   setTimeout( () => {
    videos.playCamsVideo(2, DOOR_BREAK, (error, res) => {
      console.log(error, res)
    })
  }, 51000)
  
}

function bossShutDown(){
  boomSound.load();
  boomSound.play();
  light.lightsBoss(0, (error, res) => {
    console.log(error, res)
  });
}

function clearLightsTimeouts(){
  lightsTimeouts.forEach(timerId => clearTimeout(timerId))
}
module.exports = serial;