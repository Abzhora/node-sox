var Player = require('./build/Release/sox').Player;
var fs = require('fs');

var player = new Player;
player.play({
    ondata : function (buf) {
        console.dir(buf);
    },
    onend : function () { console.log('end') },
});
