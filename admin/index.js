const $ = require('jquery');
const remote = require('electron').remote;
const fs = require('fs');

const config = require('./config.json');
const buttons = require('./buttons.js');

const controls =  [
      '.row.sigarets > .fire'
    , '.row.first-video > .shut'   
    , '.row.first-video > .reset' 
    , '.row.sigarets > .open'
    , '.row.sigarets > .close'
    , '.row.sigarets > .manual'
    , '.row.sigarets > .auto'
    , '.row.scotch > .open'
    , '.row.scotch > .close'
    , '.row.cams > .shut'
    , '.row.cams > .reset'
    , '.row.light > .on'
    , '.row.light > .off'
    , '.row.light > .main_off'
    , '.row.post > .fire'
    , '.row.post > .open'
    , '.row.post > .close'
    , '.row.door > .fire'
    , '.row.door > .stop'
    , '.row.fan > .open'
    , '.row.fan > .close'
    , '.row.grid > .fire'
    , '.row.grid > .open'
    , '.row.grid > .close'
    , '.row.lock > .fire'
    , '.row.lock > .open'
    , '.row.lock > .close' 
    ];
let lockChargeReq = '';

$('#manual').click(() => {
  if ($('#manual').is(':checked')) {
    controls.forEach(selector => $(selector).removeAttr('hidden'))
  } else {
    controls.forEach(selector => $(selector).attr('hidden', true))
  }
});

const helpList = [
'Приветствие',
'Дюфрейн',
'Рэд',
'Брукс',
'Хэйвуд',
'Что творишь',
'Не надо этого делать',
'Ещё раз и в браслетах',
'Хватит крушить',
'Круги на стенах',
'Общаются при помощи чашек',
'Плакат с девицей',
'Контрабанда в тубусах',
'Молнечный зайчик',
'Зеркальце к правой стороне окошка',
'Стуки',
'Есть закурить',
'Послания на сигаретах',
'Знать за какой кирпич дернуть',
'Сорви девку со стены',
'Изресовали стены',
'Подчеркивают буквы в надписях',
'Символ банды и татуировка',
'Вороны самые кровожадные',
'Толпа бычар',
'Библии повсюду',
'Полежи подумай',
'Что прячешь в кровати',
'Скотч',
'Личные вещи',
'Открутить',
'Стену прошибить плечом',
'Сейф за картиной',
'Энди, постирать костюмчик',
'Писать на одежде',
'Сейф с механическим замком',
'Охранникам не дают револьверы',
'Выстрелив из револьвера',
'Пневмопочта',
'Проводка и предохранитель на местах',
'Директор все записывает',
'Рэд, снизу штанины',
'Отмычкой открыть любой замок',
'Граммофон',
'Микрофон подключить к системе',
'Если включить сразу всё',
'В трубу пальцем ткни',
'Нет замка'];


const nameList = helpList.slice(1, 5).map((item, index) => {
  return `<input class="name-item" type="radio" name="name" value="${index + 2}"> ${item}`
});

let actionsList = helpList.slice(5)
actionsList.unshift(helpList[0])
actionsList = actionsList.map((item, index) => {
  return `<input class="action-item" name="actions" type="radio" value="${index === 0 ? index + 1 : index + 5}" ${index === 0 ? 'checked' : ''}> ${item} <br>`
});

nameList.forEach(item => $('form.names').append(item))
actionsList.forEach(item => $('form.actions').append(item))



