(function(args) {
	if(args[0]) {
		os.write(shell.resolve(args[0]), "")
	}
})