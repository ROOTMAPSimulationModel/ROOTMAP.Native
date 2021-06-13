# ROOTMAP Postprocessing automation script

rm(list=ls())			# Clear all pre-existing objects


# USER SETTINGS:_____________________________________________________________________________________

simulationDir <- readline("Please enter complete path of ROOTMAP simulation directory. Use forward slash (/) as a separator, regardless of operating system.\n")
scriptDir <- "C:/RM/build/win32_d"
plotWidth <- 500	#pixels
plotHeight <- 1000	#pixels
# Specify separator character below. Only "," and "\t" (comma and tab, respectively) are supported.
separator <- "\t"		#tab-delimited

#____________________________________________________________________________________________________


fileExt <- ".txt"
if(separator==",")		fileExt <- ".csv" 
setwd(simulationDir)

# Set all simulation-level constants
source(file.path(scriptDir,"Read_Boundaries.r"))
source(file.path(scriptDir,"Calculate_Box_Volumes.r"))
source(file.path(scriptDir,"Calculate_Box_Surface_Areas.r"))
surfaceArea <- (max(xb) - min(xb)) * (max(yb) - min(yb)) * 1e-8	# surface area in hectares

useRootLength <- FALSE
useWaterContent <- FALSE
useNitrate <- FALSE
usePhosphorus <- FALSE
useBulkDensity <- FALSE

outputRulesXML = XML::xmlRoot(XML::xmlTreeParse(file.path(getwd(),"OutputRules.xml")))
rootLengthElement = XML::getNodeSet(outputRulesXML, "/rootmap/initialisation[owner='DataOutputCoordinator']/outputrule[source='PlantCoordinator' and characteristic='Total Root Density']/filename")
for(rl in rootLengthElement)
{	
	rootLengthFilenameFormat <- XML::xmlValue(rl)
	useRootLength <- TRUE
}
waterContentElement = XML::getNodeSet(outputRulesXML, "/rootmap/initialisation[owner='DataOutputCoordinator']/outputrule[source='Water' and characteristic='Water Content']/filename")
for(wc in waterContentElement)
{	
	waterContentFilenameFormat <- XML::xmlValue(wc)
	useWaterContent <- TRUE
}
nitrateElement = XML::getNodeSet(outputRulesXML, "/rootmap/initialisation[owner='DataOutputCoordinator']/outputrule[source='Nitrate' and characteristic='Nitrate Amount']/filename")
for(n in nitrateElement)
{	
	nitrateFilenameFormat <- XML::xmlValue(n)
	useNitrate <- TRUE
}
phosphorusElement = XML::getNodeSet(outputRulesXML, "/rootmap/initialisation[owner='DataOutputCoordinator']/outputrule[source='Phosphorus' and characteristic='Total Labile P']/filename")
for(p in phosphorusElement)
{	
	phosphorusFilenameFormat <- XML::xmlValue(p)
	usePhosphorus <- TRUE
}
bulkDensityElement = XML::getNodeSet(outputRulesXML, "/rootmap/initialisation[owner='DataOutputCoordinator']/outputrule[source='BulkDensity' and characteristic='Bulk Density']/filename")
for(bd in bulkDensityElement)
{	
	bulkDensityFilenameFormat <- XML::xmlValue(bd)
	useBulkDensity <- TRUE
}

# The following regexes match all files containing the phrases "Root Length" etc
# except for R scripts and those weird 1kb files that start with a .
# (R's default dir() behaviour treats those as hidden and doesn't list them)

if(useRootLength)
{
	chunks <- strsplit(rootLengthFilenameFormat,"%")
	rootLengthFilenameBasis <- chunks[[1]][which.max(nchar(chunks[[1]]))]
	rootLengthFilenameRegex <- paste(sep="", ".*", rootLengthFilenameBasis, "((?![Pp]ostprocessed).)*[^\\.Rr]$")
}

if(useWaterContent)
{
	chunks <- strsplit(waterContentFilenameFormat,"%")
	waterContentFilenameBasis <- chunks[[1]][which.max(nchar(chunks[[1]]))]
	waterContentFilenameRegex <- paste(sep="", ".*", waterContentFilenameBasis, "((?![Pp]ostprocessed).)*[^\\.Rr]$")
}

if(useNitrate)
{
	chunks <- strsplit(nitrateFilenameFormat,"%")
	nitrateFilenameBasis <- chunks[[1]][which.max(nchar(chunks[[1]]))]
	nitrateFilenameRegex <- paste(sep="", ".*", nitrateFilenameBasis, "((?![Pp]ostprocessed).)*[^\\.Rr]$")
}

if(usePhosphorus)
{
	chunks <- strsplit(phosphorusFilenameFormat,"%")
	phosphorusFilenameBasis <- chunks[[1]][which.max(nchar(chunks[[1]]))]
	phosphorusFilenameRegex <- paste(sep="", ".*", phosphorusFilenameBasis, "((?![Pp]ostprocessed).)*[^\\.Rr]$")
}

if(useBulkDensity)
{
	chunks <- strsplit(bulkDensityFilenameFormat,"%")
	bulkDensityFilenameBasis <- chunks[[1]][which.max(nchar(chunks[[1]]))]
	bulkDensityFilenameRegex <- paste(sep="", ".*", bulkDensityFilenameBasis, "((?![Pp]ostprocessed).)*[^\\.Rr]$")
	for(f in grep(bulkDensityFilenameRegex, list.files(), value=TRUE, perl=TRUE))
	{
		# This script uses the last Bulk Density file found in the directory.
		# In theory, this should also be the first (or, at least, identical to it)
		filename <- f
	}
	print(paste("Using Bulk Density file:",filename))
	source(file.path(scriptDir,"Read_Raw_Data.r"))
	rawBulkDensityData <- rawData
	rm(filename)
	rm(rawData)
}
if(!useBulkDensity)
{
	print(paste(sep="","No Bulk Density Output Rule specified in ",getwd(),"/OutputRules.xml. Production of N and P mass/mass graphs impossible."))
}


# Now iterate through all subdirectories and process each one's data
directories <- row.names(subset(file.info(dir()), isdir==TRUE))
for(td in directories)
{
	thisDir <- file.path(simulationDir, td)
	setwd(thisDir)
	print(paste("Processing:",getwd()))
	thisOutputDir <- file.path(getwd(),"Postprocessed Data")
	dir.create(thisOutputDir)

	if(useRootLength)
	{
		for(f in grep(rootLengthFilenameRegex, list.files(), value=TRUE, perl=TRUE))
		{
			print(paste("    Calculating RLD from file:",f))
			filename <- f
			source(file.path(scriptDir,"Average_RLD.r"))
		}
	}

	if(useWaterContent)
	{
		for(f in grep(waterContentFilenameRegex, list.files(), value=TRUE, perl=TRUE))
		{	
			print(paste("    Calculating water content from file:",f))
			filename <- f
			source(file.path(scriptDir,"Water_Content.r"))
		}
	}

	if(useNitrate)
	{
		for(f in grep(nitrateFilenameRegex, list.files(), value=TRUE, perl=TRUE))
		{
			print(paste("    Calculating nitrate content from file:",f))
			filename <- f
			source(file.path(scriptDir,"Nitrate_Content.r"))
		}
	}

	if(usePhosphorus)
	{
		for(f in grep(phosphorusFilenameRegex, list.files(), value=TRUE, perl=TRUE))
		{
			print(paste("    Calculating phosphorus content from file:",f))
			filename <- f
			source(file.path(scriptDir,"Phosphorus_Content.r"))
		}
	}

}	#end simulation (subdirectory)

setwd(simulationDir)
print("Postprocessing completed.")


