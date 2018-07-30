(function(args) {
	if(args[0]) {
		if(!os.mkdir(shell.resolve(args[0]))) throw args[0] + " couldn't be created";
	}
})