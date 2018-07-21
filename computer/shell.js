shell = {
	run: true,
	current_dir: "/",
	
	exec: function() {
		while(this.run) {
			var line = os.stdin(this.current_dir.replace(/^\/+|\/+$/gm, "") + "> ");
			line = line.split(" ");
			
			var runthis = this.functions[line[0]];
			if(typeof(runthis) !== "function") os.print("Not a command");
			else runthis(line.slice(1));
		}
	},
	
	functions: {
		exit: function() {
			shell.run = false;
		},
		cd: function(args) {
			var folder = args.join(" ");
			path = os.chdir(folder);
			if(path) {
				shell.current_dir = path;
			} else {
				os.print("'" + folder + "' is not a directory");
			}
		}
	}
};