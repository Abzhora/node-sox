var Player = require('./build/Release/sox').Player;
var fs = require('fs');
var ws = fs.createWriteStream('out.wav');

var player = new Player;
player.play({
    ondata : function (buf) {
        console.log(buf.length);
        ws.write(buf);
    },
    onend : function () {
        console.log('end');
        ws.end();
    },
});
