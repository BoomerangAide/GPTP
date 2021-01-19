This project is the sequel to https://github.com/BoomerangAide/GPTP-For-VS2008
Now made to work under Visual Studio 2019.Attempting to keep the code files clean from language tricks that would get against backward compatibility, but the "project" and "solution" files are incompatible.It should be possible to figure how to adapt them if needed by comparing the projects.

How to use:

- Modify the hooks you need to gain the effects you want.
- Use the next_Frame() function in hooks/main/game_hooks.cpp to perform tasks on a frame basis to complete.
- To optimize things, you can remove parts you don't use, make sure to delete the hooked functions and their corresponding code in the *_inject.cpp files. if you delete entire hooks files, make sure to remove them in initialize.cpp
- You can edit the plugin's name and ID in definitions.h
- The compiled .qdp file can be used by Starcraft 1.16.1 third party loaders, or turned into an .exe by Firegraft or maybe other applications.

For a more complete information, check out https://github.com/BoomerangAide/GPTP-For-VS2008/blob/Update-4/GPTP/readme.txt

Special thanks to A_of_s_t, RavenWolf, pastelmind, Heinermann, xAngelSpiritx, bajadulce, KYSXD and many others...
Also special thanks to anybody that may have reported a bug or mistake in GPTP 2008 or this one, leading to improvement.
