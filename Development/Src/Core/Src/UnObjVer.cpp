/*=============================================================================
	UnObjVer.cpp: Unreal version definitions.
	Copyright 1998-2012 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "CorePrivate.h"

// Used by the build system to set the Windows version number - it is defined as MAJOR.MINOR.ENGINE.PRIVATE
#define MAJOR_VERSION			1
#define MINOR_VERSION			0
#define PRIVATE_VERSION			131

// Defined separately so the build script can get to it easily (DO NOT CHANGE THIS MANUALLY)
#define	ENGINE_VERSION	12620

#define	BUILT_FROM_CHANGELIST	23201


INT	GEngineVersion				= ENGINE_VERSION;
INT	GBuiltFromChangeList		= BUILT_FROM_CHANGELIST;

#if _XBOX
	// Prevent patched and unpatched network clients from seeing each other in system link
	// NOTE: This is not a Live problem, because you must patch to play on Live
	INT	GEngineMinNetVersion		= ENGINE_VERSION;
#else
	INT	GEngineMinNetVersion		= 9189;
#endif
INT	GEngineNegotiationVersion	= 3078;

// @see UnObjVer.h for the list of changes/defines
INT	GPackageFileVersion			= VER_LATEST_ENGINE;
INT	GPackageFileMinVersion		= 491;
INT	GPackageFileLicenseeVersion = VER_LATEST_ENGINE_LICENSEE;
INT GPackageFileCookedContentVersion = VER_LATEST_COOKED_PACKAGE | (VER_LATEST_COOKED_PACKAGE_LICENSEE << 16);

INT GEngineMatchmakingVersion = 66;

// Outlast Retail Package Patch - selectable package versioning; see UnObjVer.h.
// Defaults: toggle off, preloaded with the Outlast retail target numbers.
UBOOL	GbForceRetailPackageVersion		= FALSE;
INT		GForcedPackageFileVersion		= VER_RETAIL_PACKAGE_FILE;
INT		GForcedPackageLicenseeVersion	= VER_RETAIL_PACKAGE_LICENSEE;
INT		GForcedEngineVersion			= VER_RETAIL_ENGINE_VERSION;

// Packages named on -SaveAsRetail=<list>. Empty = no filter = every package saved.
static TArray<FString> GRetailPackageFilter;

// Reduces a user-supplied entry ("..\Src\MyMod.u", "MyMod.u", " MyMod ") to a bare package name.
static FString NormalizeRetailPackageName( const FString& Entry )
{
	FString Result = Entry.TrimQuotes();
	Result = Result.Trim().TrimTrailing();

	// Drop any directory part.
	INT SlashIndex = Result.InStr( TEXT("\\"), TRUE );
	if ( SlashIndex == INDEX_NONE )
	{
		SlashIndex = Result.InStr( TEXT("/"), TRUE );
	}
	if ( SlashIndex != INDEX_NONE )
	{
		Result = Result.Mid( SlashIndex + 1 );
	}

	// Drop any extension (.u / .upk / .umap), so both forms are accepted.
	const INT DotIndex = Result.InStr( TEXT("."), TRUE );
	if ( DotIndex != INDEX_NONE )
	{
		Result = Result.Left( DotIndex );
	}

	return Result;
}

// Builds the filter from a comma-separated list. Passing NULL or "" clears it.
void appSetRetailPackageFilter( const TCHAR* PackageList )
{
	GRetailPackageFilter.Empty();

	if ( PackageList != NULL && *PackageList != 0 )
	{
		TArray<FString> Entries;
		FString( PackageList ).ParseIntoArray( &Entries, TEXT(","), TRUE );

		for ( INT EntryIndex = 0; EntryIndex < Entries.Num(); EntryIndex++ )
		{
			const FString PackageName = NormalizeRetailPackageName( Entries(EntryIndex) );
			if ( PackageName.Len() > 0 )
			{
				GRetailPackageFilter.AddItem( PackageName );
			}
		}
	}
}

// TRUE when this specific package should be written in retail format.
UBOOL appShouldSaveAsRetailPackage( const TCHAR* PackageName )
{
	UBOOL bShouldSaveAsRetail = FALSE;

	if ( GbForceRetailPackageVersion )
	{
		if ( GRetailPackageFilter.Num() == 0 )
		{
			// No filter given - the toggle applies to every save, as it always has.
			bShouldSaveAsRetail = TRUE;
		}
		else if ( PackageName != NULL )
		{
			// FString comparison is case-insensitive, which is what we want for package names.
			const FString CandidateName = NormalizeRetailPackageName( PackageName );
			for ( INT FilterIndex = 0; FilterIndex < GRetailPackageFilter.Num(); FilterIndex++ )
			{
				if ( GRetailPackageFilter(FilterIndex) == CandidateName )
				{
					bShouldSaveAsRetail = TRUE;
					break;
				}
			}
		}
	}

	return bShouldSaveAsRetail;
}
