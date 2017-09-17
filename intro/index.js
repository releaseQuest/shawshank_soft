const $ = require('jquery')
const remote = require('electron').remote
const express = require('express')
const app = express()
const fs = require('fs')
const config = require('./config.json')
const videos = require('./videos')
const options = config

app.get('/play', (req, res) => {
  const id = req.query.id
  if ( !!~videos.indexOf(id) ) {
    $('#scr').attr('src', `./videos/${id}.mp4`)
    const scr = $('#scr').get(0)
    scr.load()
    scr.play()
    res.send(JSON.stringify({
    status: 'PLAYING'
    , screen: screen
  }))
      
  } else {
    res.send(JSON.stringify({
    status: 'ERROR'
    , message: 'query params error'
  }))
  }
})

app.get('/reset', (req, res) => {

    $('#scr').attr('src', '')
    const scr = $('#scr').get(0)
    scr.load()
    //scr.play()
    res.send(JSON.stringify( {status: 'CLEARED'} ))
      
})

app.get('/shutdown', (req, res) => {
  res.send(JSON.stringify({
      status: 'OK'
    , message: `shutted down`
  })) 
  require('child_process').exec('sudo /sbin/shutdown -h now', msg => {console.log(msg)} );
})

app.listen(4000);
