(function(args) {
	if(args[0]) {
		if(!os.mkdir(shell.resolve(args[0]))) os.print("Couldn't create directory: '" + args[0] + "'");
	}
})