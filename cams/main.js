const electron = require('electron')
const {
  app, BrowserWindow
} = electron
app.on('ready', () => {
  let win = new BrowserWindow({
    width: 810
    , height: 510
    , frame: false
    , fullscreen: true
    , center: true
  })
  win.loadURL(`file://${__dirname}/index.html`)
})