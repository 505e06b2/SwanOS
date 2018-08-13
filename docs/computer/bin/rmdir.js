(function(args) {
	if(args[0]) {
		if(!os.rmdir(shell.resolve(args[0]))) throw args[0] + " couldn't be removed (directory probably not empty)");
	}
})