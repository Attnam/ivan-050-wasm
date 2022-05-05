Iter Vehemens ad Necem 0.50 in WebAssembly
==========================================

Late to the Party? Never fear! Play IVAN in your browser, [right here](https://attnam.github.io/ivan-050-wasm/).

This is a port of IVAN 0.50 to WebAssembly, via the magic of emscripten.

This was inspired in the first instance by [James Mackenzie's](https://github.com/jamesfmackenzie) [port](https://www.jamesfmackenzie.com/2019/10/28/commander-keen-ported-to-webassembly/) of 
[Commander Keen](http://www.jamesfmackenzie.com/chocolatekeen/) for the web.

If you're interested in the technical details of this port of IVAN, you could read the [git diff](https://github.com/Attnam/ivan-050-wasm/compare/383ffef341f4b247e3edb0cf49344b120d540120...04be011ce313517baba885a53fb48412cc24eb75), 
but honestly you're better off reading about [Connor Clark's](https://hoten.cc/about/) 
[web port](https://hoten.cc/blog/porting-zelda-classic-to-the-web/) of [Zelda Classic](https://hoten.cc/zc/play/).


Building
--------------------------

Building the project just takes a couple of steps. First you should git pull this project to your local directory (say, `ivan-050-wasm`). 
Note that the emscripten repository is added as a submodule to this repository so to get the whole thing you want to do:
```
$ git submodule update --init --recursive
```

Note that if you already have emscripten installed, you'll probably want to stick to that rather than using the git submodule.
Otherwise, you'll want to cd into the `emsdk` folder and set up the emscripten build system.
On Windows that's something like this:
```
PS ../esmdk>git pull
PS ../esmdk>.\emsdk install latest
PS ../esmdk>.\emsdk activate latest
PS ../esmdk>emsdk_env.bat
```

Continuing on Windows, and using MSYS2 (sorry folks...!)
```
$ cd C:/path/to/ivan-050-wasm
$ mkdir embuild
$ cd embuild
$ cmake .. -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=C:/path/to/ivan-050-wasm/eminstall -DWIZARD=ON -DCMAKE_TOOLCHAIN_FILE=C:/path/to/ivan-050-wasm/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_CXX_COMPILER=C:/path/to/ivan-050-wasm/emsdk/upstream/emscripten/em++.bat -DCMAKE_C_COMPILER=C:/path/to/ivan-050-wasm/emsdk/upstream/emscripten/emcc.bat
$ make install
```

This forces cmake to pick up the emscripten llvm compiler and all that gubbins. It will install the emscripten SDL2 port automatically. If you're on another system you can just as well use "Unix Makefiles" or suchlike.


Running locally
--------------------------

Again on Windows, you're probably best off using a Powershell prompt as Administrator. You'll have to pull a few extra tricks right before running it.
```
PS ../eminstall>Set-ExecutionPolicy RemoteSigned
(type 'y' to accept)
PS ../eminstall>esmdk activate latest --permanent
PS ../eminstall>emrun --browser firefox ivan.html
```

Hopefully that should do it. You can equally set the browser to something different, like `chrome`. Omit `--browser` and it will pick up your system's default browser.


Contributing
--------------------------

This project adheres to the Contributor Covenant [code of conduct](CODE_OF_CONDUCT.md).
By participating, you are expected to uphold this code. Please report any unacceptable behavior.

Bugfixes are welcome! Please fork this repository and then once you have
made and tested your changes, submit a pull request. It will be reviewed by
one of the admins as soon as possible.


Forum
--------------------------

Join us in [the Cathedral of Attnam](https://attnam.com/ "Official forums") for discussion of 
gameplay, planned features for the [fan continuation](https://github.com/Attnam/ivan "fan continuation") 
and more (or to yell at us if we neglect your pull request for too long).


More
--------------------------

See README for FAQ and more information about the game.

See LICENSING for copyright notice.

See /Doc for fiction and short stories from the world of IVAN.


--------------------------

FREE SOFTWARE FOREVER!
