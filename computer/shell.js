shell = {
	run: true,
	current_dir: "/",
	
	exec: function() {
		while(this.run) {
			var line = os.stdin(this.current_dir.replace(/^\/+|\/+$/gm, "") + "> "); //regex for removing excess '/'
			line = line.trim().match(/(?=\S)[^"\s]*(?:"[^\\"]*(?:\\[\s\S][^\\"]*)*"[^"\s]*)*/g); // stolen from https://stackoverflow.com/a/40120309
			for(var i = 0; i < line.length; i++) if(line[i][0] == "\"" && line[i][line[i].length-1] == "\"") line[i] = line[i].slice(1, -1);
			
			var runthis = this.functions[line[0]];
			if(typeof(runthis) !== "function") os.print("'" + line[0] + "' is not a command");
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
		},
		ls: function(args) {
			os.list(args.join(" ")).forEach(function(e) {
				os.print(e);
			});
		},
		cat: function(args) {
			var filename = args.join(" ");
			var contents = os.read(filename);
			if(contents) os.print(contents);
			else os.print("'" + filename + "' can't be read");
		}
	}
};