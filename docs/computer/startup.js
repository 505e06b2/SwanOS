os.print("> Loading shell...");
var shell = new (os.run('/bin/shell.js'))(false); //false so that exec doesn't run automatically (it's in the bottom of the shell.js)
os.print(">> JsOS Shell >>");
shell.exec();