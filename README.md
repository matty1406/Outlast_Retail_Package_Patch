# Outlast Retail Package Patch

Lets you save packages in a format the retail (Steam) build of Outlast can load.

This source saves packages at **882 / licensee 3 / engine 12620**. Retail accepts **867 / 3 / 10907**. Turn the mode on and packages get stamped *and* serialized at retail's numbers instead. With it off, packaging is identical to the unpatched engine.

## Install

Copy the `Development` folder into `<engine root>\UnrealEngine3\` and let it merge over these 7 files:

```
Development\Src\Core\Inc\UnObjVer.h
Development\Src\Core\Src\UnObjVer.cpp
Development\Src\Core\Src\UnLinker.cpp
Development\Src\Core\Src\UnMisc.cpp
Development\Src\UnrealEd\Inc\ResourceIDs.h
Development\Src\UnrealEd\Inc\EditorFrame.h
Development\Src\UnrealEd\Src\EditorFrame.cpp
```

Then rebuild the engine, as below.

## Building the engine

### What you need

**[Visual Studio 2012.](https://archive.org/download/en_visual_studio_professional_2012_x86_dvd_2262334)** The build tool finds the compiler through the `VS110COMNTOOLS` environment variable, which only VS2012 sets. VS2010 and anything newer will not be found.

**[DirectX SDK (June 2010).](https://www.microsoft.com/en-gb/download/details.aspx?id=8109)** The last one Microsoft shipped, and the one this engine wants. The build tool finds it through `DXSDK_DIR`. You can tell it is the right version because `Binaries\Win64` ships `D3DX9_43.dll`, which is that SDK's runtime.

**.NET Framework 4.** UnrealBuildTool is a C# program. Windows 8 and later already have it.

> **If the DirectX SDK installer fails with error S1023**, you have a newer Visual C++ 2010 Redistributable than the one it wants to install. Uninstall the x86 and x64 "Microsoft Visual C++ 2010 Redistributable" entries, then install the SDK. Removing redistributables does not break anything and will be restored if needed.

### Build it

1. Open `Development\Src\Outlast.sln` in Visual Studio 2012.
2. Set the configuration to **Release** and the platform to **Win64**.
3. Build the **OLGame Win64** project.
    - If it fails, you may need to rebuild `UnrealBuildTool` first. Right-click `UnrealBuildTool` in the solution explorer and choose **Rebuild**. Then try again.
4. You get `Binaries\Win64\OLGame.exe`.

Or from a command prompt:

```
Binaries\BuildWrap64.bat OLGame Win64 Release
```

Do a **full rebuild** the first time you apply this patch. `UnObjVer.h` and `ResourceIDs.h` are included almost everywhere, so an incremental build can miss things. It takes a while; that is normal for UE3.

## Use

### Compiling and cooking (UCScript and packages)

Nothing else is touched:

```
OLGame.exe make -auto -SaveAsRetail=MyMod
OLGame.exe CookPackages -platform=PCConsole -DLCName=MyMod -SaveAsRetail=MyMod
```

Comma-separate for several, no spaces around the commas:

```
OLGame.exe make -auto -SaveAsRetail=MyMod,MyUI,MyMovers
```

Names are case-insensitive and the extension is optional, so `MyMod`, `MyMod.u` and `MyMod.upk` all mean the same package. It matches the *package* name, not the output filename, so it still works when cooking renames or moves the file.

Every package written in retail format logs a line to the build log.

### In-editor saving

**File → "Save as Outlast Retail Package (v867)"**, a checkable item. While it's checked, *every* package you save is written in retail format, so only switch it on for the job you actually want. It affects the editor process only; make and cook are separate processes and need the command-line switch.

### Blanket switch

A bare `-SaveAsRetail` with no list applies to every package the process saves. Use it only when you mean everything.