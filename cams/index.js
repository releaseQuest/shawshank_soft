const $ = require('jquery')
const remote = require('electron').remote
const express = require('express')
const gpio = require('gpio')
const app = express()
const fs = require('fs')
const config = require('./config.json')
const videos = require('./videos')
const options = config

const gpio21 = gpio.export(options.relayPin, { direction: 'out', ready: () => {
  gpio21.set();	
} });

init();

app.get('/status', (req, res) => {
  res.send(JSON.stringify({
    status: 'OK'
    , playing: [
      isVideoPlaying(`#scr1`)
      , isVideoPlaying(`#scr2`)
      , isVideoPlaying(`#scr3`)
      , isVideoPlaying(`#scr4`)
    ]
  }))
})
app.get('/play', (req, res) => {
  const screen = req.query.scr
  const id = req.query.id
  if ((screen >= 1 && screen <= 4) && !!~videos.indexOf(id)) {
    $(`#scr${screen}`).attr('src', `./videos/${id}.mp4`)
    const scr = $(`#scr${screen}`).get(0)
    scr.load()
    scr.play()
    res.send(JSON.stringify({
    status: 'PLAYING'
    , screen: screen
  }))
      
  }
  else {
    res.send(JSON.stringify({
    status: 'ERROR'
    , message: 'query params error'
  }))
  }
})

app.get('/light', (req, res) => {
  const state = req.query.state

    console.log(`Set to ${state}`)
    gpio21.set(+state);
    res.send(JSON.stringify({
      status: 'OK'
    , message: `Set to ${state}`
    })) 
  
})

app.get('/shutdown', (req, res) => {
  res.send(JSON.stringify({
      status: 'OK'
    , message: `shutted down`
  })) 
  require('child_process').exec('sudo /sbin/shutdown -h now', msg => {console.log(msg)} );
})

app.listen(4000)

function isVideoPlaying(selector) {
  const video = $(selector).get(0)
  return !!(video.currentTime > 0 && !video.paused && !video.ended && video.readyState > 2)
}

function init() {
  let scr = {}
  const screens = [1, 2, 3, 4]
  screens.forEach(screen => {
    scr = $(`#scr${screen}`).get(0);
    scr.load();
    scr.play();
  });
    
}