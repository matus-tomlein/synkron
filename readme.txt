To compile the Synkron source code, you will need Qt 4.3 or
a newer, binary compatible version.

You now have two options:
1. Use the build script to compile Synkron (recommended)
2. Compile manually

	1. Compiling using the build script
	-----------------------------------
	Download and install Squirrel Shell:
		http://sourceforge.net/projects/squirrelsh/
	Under WINDOWS, open the "Qt Command Prompt"
	from the Start menu, change directory to that of
	the Synkron source code and type:
		squirrelsh build.nut
	Under UNIX/LINUX, change directory to that of the
	Synkron source code and type:
		squirrelsh build.nut

	2. Compiling manually
	---------------------
	Regardless of your platform, type in the following
	commands (it is assumed that you have already changed
	directory to that of the Synkron source code):
		lrelease Synkron.pro
		qmake -config release
		make
	(under Windows, you'll probably need to use
	mingw32-make or nmake)

When all is done, you may run the program:
Linux/Unix: ./synkron
Mac OS X: open Synkron.app
Windows: Synkron