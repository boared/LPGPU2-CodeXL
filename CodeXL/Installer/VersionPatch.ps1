# VersionPatch.ps1
# ================
# Script that patches the file version and product version information embedded in DLL and EXE files.

#Get Parameters from command line 
Param(
  [string]$workspace, # Mandatory parmater
  [string]$VersionNumber, # Mandatory parmater
  [string]$GDT_Build_Suffix = "Public",
  [string]$debug = "False", 
  [String]$Verbose = "False"
)

CLS

IF ($debug -eq "true") {
#Debug variables values - usually will come from command switches / Set test variables values here
	$workspace=$PSScriptRoot+"\..\..\"
	$VersionNumber='1.2.3.4'
	$GDT_Build_Suffix = "internal"
	}

#Set executable location
$VerPatchexe=$workspace+"\Common\DK\bin\win32\VerPatch\VerPatch.exe"

#Set version details
$VerProd='"CodeXL - a comprehensive tool suite for the performance-aware developer"'
$VerCopyright='"Copyright (C) 2016 AMD Inc. All rights reserved."'
$CMD_ALLINFO=&$VerPatchexe $file.FullName $VersionNumber $SetVA /pv $VersionNumber /s ProductName $VerProd /s copyright $VerCopyright

# Patch files according to CSV file
# Import file list from CSV file
$FilesFromCSV = Import-Csv -Path "$workspace\CodeXL\Installer\CodeXL_FileList_VersionPatch.csv"
$ErrorList = @()
foreach ($CurrentRaw in $FilesFromCSV) {
	$FileVerAction = $CurrentRaw.VerPatch
	if ($FileVerAction -ne "No"){
		$TargetDir = $workspace+"\"+$CurrentRaw.Folder+"\"

		if ($GDT_Build_Suffix -ne "Public" -and $CurrentRaw.GDT_Dynamic_Suffix -eq "Yes") {
			#Changing file name - adding GTD suffix before the file extension
			$adjFilename = $CurrentRaw.Name.Replace(".","-"+$GDT_Build_Suffix+".") 
			$File = $TargetDir+"\"+$adjFilename
			}
		# Special case for VSIX files located at diffent folders with no GDT suffix change
		elseif ($GDT_Build_Suffix -ne "Public" -and $CurrentRaw.Folder -like "Release-Public"){
			$TargetDir = $CurrentRaw.Folder.Replace("Release-Public","Release-"+$GDT_Build_Suffix)
		}
		# Special cases for Spies files located at diffent folders with no GDT suffix change
		elseif ($GDT_Build_Suffix -ne "Public" -and $CurrentRaw.Folder -like "\spies\"){
			$TargetDir = $CurrentRaw.Folder.Replace("spies","spies-"+$GDT_Build_Suffix)
		}
		elseif ($GDT_Build_Suffix -ne "Public" -and $CurrentRaw.Folder -like "\spies64\"){
			$TargetDir = $CurrentRaw.Folder.Replace("spies64","spies64-"+$GDT_Build_Suffix)
		}
		else {
			$File = $TargetDir+"\"+$CurrentRaw.Name
		}
		if (Test-Path $file) {
			$DOFile = Get-ChildItem $file
			if ($DoFile.VersionInfo.FileVersion -eq $null) {
				$SetVA='/va'}
			else {
				$SetVA=$null}
			if ($Verbose -eq "True"){
			Write-Host $DoFile
			Write-Host "Current version=" $DoFile.VersionInfo.FileVersion `n "Prodcut Version "$DoFile.VersionInfo.ProductVersion `n "Copyrights " $DoFile.VersionInfo.LegalCopyright
			write-host "Set to="$VersionNumber}
			Set-ItemProperty -Path $DoFile.FullName  -name IsReadOnly -value $false
			if ($FileVerAction -eq "No_Ver") {
				$CMD_NOFileVersion=&$VerPatchexe $DoFile.FullName $SetVA /pv $VersionNumber /s ProductName $VerProd /s copyright $VerCopyright
					if ($LASTEXITCODE -ne 0){
					Write-Host Failed to patch file $DoFile.name
					}
				}
			elseif ($FileVerAction -eq "ALL"){
				$CMD_ALLINFO=&$VerPatchexe $DoFile.FullName $VersionNumber $SetVA /pv $VersionNumber /s ProductName $VerProd /s copyright $VerCopyright
				if ($LASTEXITCODE -ne 0){
					Write-Host Failed to patch file $DoFile.Name
					}
				}
			else {
				Write-host "$DoFile was not set correctly"
				$ErrorList += $File}
			if ($Verbose -eq "True"){
			$NewItemProps=Get-ItemProperty $DoFile
			Write-Host "Current version=" $NewItemProps.VersionInfo.FileVersion `n "Prodcut Version "$NewItemProps.VersionInfo.ProductVersion `n "Copyrights " $NewItemProps.VersionInfo.LegalCopyright}
			}
		else {
			Write-Host "$File was not found"
			$ErrorList += $File
			}
	}
	else {
		if ($Verbose -eq "True"){
			Write-Host "Skipping"$CurrentRaw.Name}
		}
	}
	if ($ErrorList.Count -gt $null){
		Write-Host $ErrorList.Count "issues found"}