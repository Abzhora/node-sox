var Player = require('./build/Release/sox').Player;

var player = new Player;
player.play({
    onend : function () { console.log('end') },
});
