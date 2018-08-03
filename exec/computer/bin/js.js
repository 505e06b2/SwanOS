(function(args) {
	os.print("== Javascript interpreter ==\nThis terminal uses the same global object as the rest of the program, so you can totally break stuff\nAnyways, type 'exit' to return");
	
	var line; //just initialise it here because we'll always need it
	while(true) {
		line = os.getline("<js> ").trim();
		if(line === "exit") return;
		try {
			eval(line);
		} catch(error) {
			os.print("> ERROR => " + error);
		}
	}
})