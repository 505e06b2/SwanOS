(function(args) {
	var filename = args.join(" ");
	os.print(shell.resolve(filename))
	var contents = os.read(shell.resolve(filename));
	if(typeof(contents) != "undefined") os.print(contents);
	else throw filename + " can't be read";
})