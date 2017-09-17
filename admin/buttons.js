const serial = require('./serial.js');
const req = require('./req.js');
const led = require('./led.js');
const videos = require('./req');

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

const TEST_COMMAND_DELAY = 500;
let command = {};
serial.init();

$('#test').click(() => {
  led.toggleLed('.row.main > .status-led', 'yellow');
  if (!serial.isOpen()) {
    serial.init();
  }
  else {
    [
      {
        status: 'test'
      },
      {
        tape: 'status'
      },
      {
        cigarettes: 'status'
      },
      {
        lock: 'status'
      },
      {
        boss: 'status'
      }
    ].forEach((item, index) => {
      setTimeout(() => {
        serial.write(JSON.stringify(item), (error, data) => {});
      }, index * TEST_COMMAND_DELAY)
      
    });
  }
});

$('.row.first-video > .fire').click(() => {
  led.toggleLed('.row.first-video > .status-led', 'yellow');
  req.playFirstVideo((error, data) => {
    if (error) {
      console.log(error);
      led.toggleLed('.row.first-video > .status-led', 'red');
    }
    else {
      led.toggleLed('.row.first-video > .status-led', 'green');
    }
  });
});

$('.row.first-video > .reset').click(() => {
  led.toggleLed('.row.first-video > .status-led', 'yellow');
  req.resetFirstVideo((error, data) => {
    if (error) {
      console.log(error);
      led.toggleLed('.row.first-video > .status-led', 'red');
    }
    else {
      led.toggleLed('.row.first-video > .status-led', 'green');
    }
  });
});

$('.row.sigarets > .fire').click(() => {
  led.toggleLed('.row.sigarets > .status-led', 'yellow');
  command = {
    cigarettes: 'fire'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.sigarets > .open').click(() => {
  led.toggleLed('.row.sigarets > .status-led', 'yellow');
  command = {
    cigarettes: 'open'
  };
  $('.row.sigarets > .open').addClass('green')
  $('.row.sigarets > .close').removeClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.sigarets > .close').click(() => {
  led.toggleLed('.row.sigarets > .status-led', 'yellow');
  command = {
    cigarettes: 'close'
  };
  $('.row.sigarets > .open').removeClass('green')
  $('.row.sigarets > .close').addClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.sigarets > .voice').click(() => {
  led.toggleLed('.row.sigarets > .status-led', 'yellow');
  command = {
    cigarettes: 'voice'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.sigarets > .manual').click(() => {
  led.toggleLed('.row.sigarets > .status-led', 'yellow');
  command = {
    cigarettes: 'manual'
  };
  $('.row.sigarets > .manual').addClass('green')
  $('.row.sigarets > .auto').removeClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.sigarets > .auto').click(() => {
  led.toggleLed('.row.sigarets > .status-led', 'yellow');
  command = {
    cigarettes: 'auto'
  };
  $('.row.sigarets > .manual').removeClass('green')
  $('.row.sigarets > .auto').addClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.scotch > .fire').click(() => {
  led.toggleLed('.row.scotch > .status-led', 'yellow');
  command = {
    tape: 'fire'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.scotch > .open').click(() => {
  led.toggleLed('.row.scotch > .status-led', 'yellow');
  command = {
    tape: 'open'
  };
  $('.row.scotch > .open').addClass('green')
  $('.row.scotch > .close').removeClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.scotch > .close').click(() => {
  led.toggleLed('.row.scotch > .status-led', 'yellow');
  command = {
    tape: 'close'
  };
  $('.row.scotch > .open').removeClass('green')
  $('.row.scotch > .close').addClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.scotch > .voice1').click(() => {
  led.toggleLed('.row.scotch > .status-led', 'yellow');
  command = {
    tape: 'voice1'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.scotch > .voice2').click(() => {
  led.toggleLed('.row.scotch > .status-led', 'yellow');
  command = {
    tape: 'voice2'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.light > .on').click(() => {
  led.toggleLed('.row.light > .status-led', 'yellow');
  $('.row.light > .on').addClass('green')
  $('.row.light > .off').removeClass('green')
  req.lightsBoss(1, (error, res) => {
      console.log(error, res)
      if (error) {
          led.toggleLed('.row.light > .status-led', 'red');
        } else {
          led.toggleLed('.row.light > .status-led', 'green'); 
        }
  })
})

$('.row.light > .off').click(() => {
  led.toggleLed('.row.light > .status-led', 'yellow');
  $('.row.light > .on').removeClass('green')
  $('.row.light > .off').addClass('green')
  req.lightsBoss(0, (error, res) => {
      console.log(error, res)
      if (error) {
          led.toggleLed('.row.light > .status-led', 'red');
        } else {
          led.toggleLed('.row.light > .status-led', 'green'); 
        }
  })
})

$('.row.light > .main_off').click(() => {
  led.toggleLed('.row.light > .status-led', 'yellow');
  command = {
    led_off: 2
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.first-video > .shut').click(() => {
  led.toggleLed('.row.first-video > .status-led', 'yellow');
  req.powerFirst((error, res) => {
    console.log(error, res)
    if (error) {
      led.toggleLed('.row.first-video > .status-led', 'red');
    } else {
      led.toggleLed('.row.first-video > .status-led', 'green'); 
    }
  })
})

$('.row.cams > .shut').click(() => {
  led.toggleLed('.row.cams > .status-led', 'yellow');
  req.powerCams((error, res) => {
    console.log(error, res)
    if (error) {
      led.toggleLed('.row.cams > .status-led', 'red');
    } else {
      led.toggleLed('.row.cams > .status-led', 'green'); 
    }
  })
})

$('.row.cams > .reset').click(() => {
  led.toggleLed('.row.cams > .status-led', 'yellow');

  videos.playCamsVideo(1, GENERAL_BLACK, (error, res) => {
    console.log(error, res)
    videos.playCamsVideo(2, DOOR, (error, res) => {
      console.log(error, res)
      videos.playCamsVideo(3, CARDS_PLAYING, (error, res) => {
        console.log(error, res);
        videos.playCamsVideo(4, EXEC_WAIT, (error, res) => {
          console.log(error, res)
          if (error) {
            led.toggleLed('.row.cams > .status-led', 'red');
          } else {
            led.toggleLed('.row.cams > .status-led', 'green'); 
          }
        })
      })
    })  
  })
})

$('.row.door > .fire').click(() => {
  led.toggleLed('.row.door > .status-led', 'yellow');
  command = {
    door: 'voice'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.door > .stop').click(() => {
  led.toggleLed('.row.door > .status-led', 'yellow');
  command = {
    door: 'stop'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.grid > .fire').click(() => {
  led.toggleLed('.row.grid > .status-led', 'yellow');
  command = {
    gate: 'fire'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.grid > .open').click(() => {
  led.toggleLed('.row.grid > .status-led', 'yellow');
  command = {
    gate: 'open'
  };
  $('.row.grid > .open').addClass('green')
  $('.row.grid > .close').removeClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.grid > .close').click(() => {
  led.toggleLed('.row.grid > .status-led', 'yellow');
  command = {
    gate: 'close'
  };
  $('.row.grid > .open').removeClass('green')
  $('.row.grid > .close').addClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})


$('.row.fan > .open').click(() => {
  led.toggleLed('.row.fan > .status-led', 'yellow');
  command = {
    fan: 'start'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})
$('.row.fan > .close').click(() => {
  led.toggleLed('.row.fan > .status-led', 'yellow');
  command = {
    fan: 'stop'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})


$('.row.lock > .fire').click(() => {
  led.toggleLed('.row.lock > .status-led', 'yellow');
  command = {
    lock: 'fire'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})
$('.row.lock > .open').click(() => {
  led.toggleLed('.row.lock > .status-led', 'yellow');
  command = {
    lock: 'open'
  };
  $('.row.lock > .open').addClass('green')
  $('.row.lock > .close').removeClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})
$('.row.lock > .close').click(() => {
  led.toggleLed('.row.lock > .status-led', 'yellow');
  command = {
    lock: 'arm'
  };
  $('.row.lock > .open').removeClass('green')
  $('.row.lock > .close').addClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})
$('.row.lock > .battery').click(() => {
  led.toggleLed('.row.lock > .status-led', 'yellow');
  command = {
    lock: 'battery'
  };
  lockChargeReq = 'battery'
  serial.write(JSON.stringify(command), (error, data) => {});
})
$('.row.lock > .gun').click(() => {
  led.toggleLed('.row.lock > .status-led', 'yellow');
  command = {
    lock: 'gun'
  };
  lockChargeReq = 'gun'
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.post > .fire').click(() => {
  led.toggleLed('.row.post > .status-led', 'yellow');
  command = {
    post: 'fire'
  };
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.post > .open').click(() => {
  led.toggleLed('.row.post > .status-led', 'yellow');
  command = {
    post: 'open'
  };
  $('.row.post > .open').addClass('green')
  $('.row.post > .close').removeClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('.row.post > .close').click(() => {
  led.toggleLed('.row.post > .status-led', 'yellow');
  command = {
    post: 'close'
  };
  $('.row.post > .open').removeClass('green')
  $('.row.post > .close').addClass('green')
  serial.write(JSON.stringify(command), (error, data) => {});
})

$('#warning').click(() => {
  const name = $('.name-item:checked').attr('value');
  const action = $('.action-item:checked').attr('value');
  if(name != 'null'){
    say(name);
  }
  setTimeout(() => {say(action)}, name != 'null' ? 2000 : 0);
})

function say( track ) {
  command = {
    help: track
  };
  serial.write(JSON.stringify(command), (error, data) => {});
}

