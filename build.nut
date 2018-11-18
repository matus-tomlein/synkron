#!/usr/bin/squirrelsh

local qmakeOpts = [ "-config", "release" ]
local makeTool = "make";

if (platform() == "macosx") {
	qmakeOpts.append("-spec");
	qmakeOpts.append("macx-g++");
}

printl("Running LRELEASE (translations)");
run("lrelease", [ "Synkron.pro" ]);

printl("Running QMAKE");
run("qmake", qmakeOpts);

switch (getenv("QMAKESPEC"))
{
case "win32-g++":
	makeTool = "mingw32-make";
	break;

case "win32-msvc":
case "win32-msvc.net":
case "win32-msvc2005":
	makeTool = "nmake";
}

printl("Running " + makeTool + " (compiling)");
run(makeTool);
