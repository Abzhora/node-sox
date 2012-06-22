var Player = require('./build/Release/sox').Player;

var player = new Player;
// player.output(file);
// player.ondata = ...
// player.onend = ...
// player.addEffect(...);

player.play({
    //outfile : 'woo.wav',
    /*
    ondata : function (buf) {
        console.log(buf.length);
        ws.write(buf);
    },
    */
    onend : function () {
        console.log('end');
    },
});
