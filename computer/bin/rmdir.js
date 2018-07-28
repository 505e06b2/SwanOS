(function(args) {
	if(args[0]) {
		if(!os.rmdir(shell.resolve(args[0]))) os.print("Couldn't remove directory: '" + args[0] + "' (probably not empty)");
	}
})