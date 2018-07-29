(function(args) {
	if(args[0]) {
		var exists = os.exists(shell.resolve(args[0]));
		os.print( (exists) ? ("'" + args[0] + "' is a " + exists) : ("'" + args[0] + "' doesn't exist") );
	}
})