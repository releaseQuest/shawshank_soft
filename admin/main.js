const electron = require('electron')
const {
  app, BrowserWindow
} = electron
app.on('ready', () => {
  let win = new BrowserWindow({
    width: 895
    , height: 768
  })
  win.loadURL(`file://${__dirname}/index.html`)
})