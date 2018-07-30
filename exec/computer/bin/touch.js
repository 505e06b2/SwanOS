(function(args) {
	if(args[0]) {
		if(!os.write(shell.resolve(args[0]), "")) throw args[0] + " can't be written to";
	}
})