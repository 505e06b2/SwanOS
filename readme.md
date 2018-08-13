#SwanOS: an ugly duk

The entire project should compile under any environment that [https://wiki.duktape.org/Portability.html](duktape supports)  
To compile any plugins that I've written, go into [https://github.com/505e06b2/SwanOS/tree/master/exec/plugins](/exec/plugins) and run the makefile for any plugins that you want on your system  
To enable a plugin, add the filepath to [https://github.com/505e06b2/SwanOS/tree/master/exec/plugins/](/exec/plugins/enabled.txt) an example is included  

* Features:  
    * Written in portable C
    * Fully scriptable with JS; get started with Javascript
    * Plugin support with a straightforward API (duktape), through dynamic libraries
    * GUI completely optional; suitable for embedded systems
    * Small memory footprint thanks to duktape and C ( < 1mb without GUI, about 10mb with GUI )
    * JS I/O API by default, but extendable through plugins