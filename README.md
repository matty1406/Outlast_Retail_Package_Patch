# Outlast Retail Package Patch

Lets you save packages in a format the retail (Steam) build of Outlast can load.

This source saves packages at **882 / licensee 3 / engine 12620**. The retail exe is **868 / 3 / 12046** and refuses anything newer. Turn the mode on and packages get stamped *and* serialized at retail's numbers instead. With it off, packaging is identical to the unpatched engine.

## Version history

- **v1.0** - Initial release, saves packages in retail format.

- **v1.1** - Cooks retail map packages + shaders & materials.
    - Packages now stamp at retail's ACTUAL numbers. Previously, I mistakenly thought it was 867 / 3 / 10907, now 868 / 3 / 12046. Older packages are still compatible, but this is the correct version to use for new packages.
    - Shaders saved in retail format now match retail's layout, so new materials can be used in retail.
    - Map cooks use retail's own cooker data, so new maps share retail's textures instead of copying them.
    - Map cooks will not stop on textures added post-release, which would block the cook.
    - Logs now confirm when retail format is on, and logs every package and shader cache saved in retail format.

## Install

Copy the `Development` folder into `<engine root>\UnrealEngine3\` and let it merge over these 17 files:

```
Development\Src\Core\Inc\UnObjVer.h
Development\Src\Core\Src\UnObjVer.cpp
Development\Src\Core\Src\UnLinker.cpp
Development\Src\Core\Src\UnMisc.cpp
Development\Src\Core\Src\SavePackage.cpp
Development\Src\Engine\Inc\ShaderManager.h
Development\Src\Engine\Inc\LightMapRendering.h
Development\Src\Engine\Src\BasePassRendering.h
Development\Src\Engine\Src\FogVolumeRendering.h
Development\Src\Engine\Src\LightRendering.h
Development\Src\Engine\Src\PointLightComponent.cpp
Development\Src\Engine\Src\ShaderCache.cpp
Development\Src\UnrealEd\Inc\ResourceIDs.h
Development\Src\UnrealEd\Inc\EditorFrame.h
Development\Src\UnrealEd\Inc\UnContentCookers.h
Development\Src\UnrealEd\Src\EditorFrame.cpp
Development\Src\UnrealEd\Src\UnContentCookers.cpp
```

The `Engine` files make shaders saved in retail format match retail's layout. The source contains Xbox One shader parameters that retail does not have, so without them retail throws those shaders away.

The `UnContentCookers` files let a map cook use retail's own cooker data, so your map shares retail's textures instead of copying them. They also flag materials used on skeletal meshes, such as cloth and curtains, so retail draws them instead of the default material. And a script package named on a `-DLCName` cook's command line is now cooked, under its own name and with the content its classes reference inside it, instead of being skipped.

Then rebuild the engine, as below.

## Building the engine

### What you need

**Outlast Source code.** What this patch is designed for. Find the source at your own risk, and do not redistribute it.

**[Visual Studio 2012.](https://archive.org/download/en_visual_studio_professional_2012_x86_dvd_2262334)** The build tool finds the compiler through the `VS110COMNTOOLS` environment variable, which only VS2012 sets. VS2010 and anything newer will not be found. Make sure you include the C++ components when installing.

**[DirectX SDK (June 2010).](https://www.microsoft.com/en-gb/download/details.aspx?id=6812)** The last one Microsoft shipped, and the one this engine wants. The build tool finds it through `DXSDK_DIR`. You can tell it is the right version because `Binaries\Win64` ships `D3DX9_43.dll`, which is that SDK's runtime.

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

Do a **full rebuild** the first time you apply this patch. `UnObjVer.h`, `ShaderManager.h` and `ResourceIDs.h` are included almost everywhere, so an incremental build can miss things. It takes a while; that is normal for UE3.

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

A patched engine logs `Outlast Retail Package Patch: installed` at startup, then whether retail format is on for that run. While it is on, every package and every shader cache saved in retail format logs a line too, so the log always shows whether the patch did its job.

### Cooking a map

1. Copy `OLGame\CookedPCConsole\GlobalPersistentCookerData.upk` from your Outlast install into the engine's `OLGame\CookedPCConsole\`. The cook reads it to know what retail already ships. Do not run a normal cook (one without `-DLCName`) while it is there, or it gets replaced.
2. Cook:

```
OLGame.exe CookPackages -platform=PCConsole -DLCName=MyMap -SaveAsRetail -full MyMap -CookShadersIntoMapsForPCConsole
```

- Use a bare `-SaveAsRetail`. A map is several packages (sublevels, localisation), and a list of names misses them.
- `-CookShadersIntoMapsForPCConsole` puts the shaders for your new materials inside the map, so retail can draw them.
- `-DLCName` names your texture files, such as `Textures_MyMap.tfc`. Pick something unique, never `dlc1` or `Patch`.
- Cook all of one mod's maps in one command. A second cook rewrites the shared texture files.

3. Ship the map packages from the engine's `OLGame\CookedPCConsoleDLC` and the `*_MyMap.tfc` files. Leave out the shader caches, `GuidCache_*`, `GlobalPersistentCookerData.upk` and the Wwise banks.

### In-editor saving

**File → "Save as Outlast Retail Package (v868)"**, a checkable item. While it's checked, *every* package you save is written in retail format, so only switch it on for the job you actually want. It affects the editor process only; make and cook are separate processes and need the command-line switch.

### Blanket switch

A bare `-SaveAsRetail` with no list applies to every package the process saves. Use it only when you mean everything.

## Credits

- **Epic Games** - Unreal Engine 3, the engine this patch modifies.

- **Red Barrels** - Outlast, the game this patch is designed for.

- **[Outlast Modding Community](https://discord.gg/hPYAappJSM)** - The wonderful people in the community who this is for, and for being a great help in learning how to mod Outlast in the first place.