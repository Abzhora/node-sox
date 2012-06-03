var sox = require('./build/Release/sox');

var player = sox.play({
    onend : function () { console.log('end') },
});
console.dir(player);
