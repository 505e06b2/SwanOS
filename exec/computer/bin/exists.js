(function(args) {
	if(args[0]) {
		var exists = os.exists(shell.resolve(args[0]));
		if(exists) os.print(args[0] + " is a " + exists);
		else throw args[0] + " doesn't exist";
	}
})