module.exports = {
  
  toggleLed: function(selector, color){
    $(selector).removeClass('red');
    $(selector).removeClass('yellow');
    $(selector).removeClass('green');
    
    $(selector).addClass(color);
  }
  
}