
SET SOURCEDIR="S:\CDBProductivitySuiteContent"
.\wix311\heat.exe dir %SOURCEDIR% -srd -sreg -cg CDBFiles -dr TargetDir -var var.TargetDir -v -ag -sfrag -out obj\CDBFiles.wxs
.\wix311\candle.exe -v -out obj\ -dDebug -dTargetDir=%SOURCEDIR%\ -arch x64 -ext ".\wix311\WixUIExtension.dll" Product.wxs obj\CDBFiles.wxs
.\wix311\light.exe -out "CDB Productivity Suite.msi" -pdbout ".\obj\CDB Productivity Suite.wixpdb" -v -cultures:null -ext ".\wix311\WixUIExtension.dll" -contentsfile obj\Product.wixproj.BindContentsFileList.txt -outputsfile obj\Product.wixproj.BindOutputsFileList.txt -builtoutputsfile obj\Product.wixproj.BindBuiltOutputsFileList.txt -wixprojectfile Product.wixproj obj\Product.wixobj obj\CDBFiles.wixobj
