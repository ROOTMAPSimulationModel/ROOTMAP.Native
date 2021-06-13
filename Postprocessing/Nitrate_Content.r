# R script to calculate nitrate content

source(file.path(scriptDir,"Read_Raw_Data.r"))
rawNData <- rawData

nitrateContentByDepth <- array( ,c(numRows,4))	# Columns: depth, ug/cm^3, ug/g, kg/ha
nitrateContentByBox <- array( ,c((numRows+1)*numSlices,numCols,3)) # 3rd dimension: ug/cm^3, ug/g, kg/ha
		
for(nz in 1:numRows)
{		
	thisLayerNDensities <- NULL
	totalLayerNMass <- 0
	totalLayerBulkMass <- 0
	thisLayerVolume <- NULL
	for(nx in 1:numCols)
	{
		for(ny in 1:numSlices)
		{			
			thisLayerVolume <- c(thisLayerVolume, boxVolumes[nz,nx,ny])
			thisBoxNDensity <- sum(rawNData[((numRows+1)*(ny-1)+nz),nx])	# using numRows + 1 to account for blank delimiter rows
			thisLayerNDensities <- c(thisLayerNDensities, thisBoxNDensity)									
			totalLayerNMass <- totalLayerNMass + thisBoxNDensity*boxVolumes[nz,nx,ny]

			nitrateContentByBox[(numRows+1)*(ny-1)+nz,nx,1] <- thisBoxNDensity
			nitrateContentByBox[(numRows+1)*(ny-1)+nz,nx,3] <- thisBoxNDensity*boxVolumes[nz,nx,ny]*1e-9/boxSurfaceAreas[nx,ny]
			
			if(useBulkDensity)
			{
				thisBoxBulkDensity <- sum(rawBulkDensityData[((numRows+1)*(ny-1)+nz),nx])	# using numRows + 1 to account for blank delimiter rows
				totalLayerBulkMass <- totalLayerBulkMass + thisBoxBulkDensity*boxVolumes[nz,nx,ny]
				nitrateContentByBox[(numRows+1)*(ny-1)+nz,nx,2] <- thisBoxBulkDensity*boxVolumes[nz,nx,ny]
			}
		}	
	}
	nitrateContentByDepth[nz,1] <- zmidpoints[nz]/100		# converting depth from cm to m
	nitrateContentByDepth[nz,2] <- mean(thisLayerNDensities)
	if(useBulkDensity)
	{
		nitrateContentByDepth[nz,3] <- totalLayerNMass/totalLayerBulkMass
	}
	nitrateContentByDepth[nz,4] <- totalLayerNMass * 1e-9 / surfaceArea	# converting mass from ug to kg and dividing by number of hectares
}

setwd(thisOutputDir)

outputFile <- file(paste(sep="", filename, " postprocessed ugNcm^-3", fileExt))
write(t(subset(nitrateContentByDepth, select=c(1,2))), outputFile, sep=separator, ncolumns=2)
png(paste(sep="",filename," postprocessed ugNcm^-3.png"), width=plotWidth, height=plotHeight)
plot(nitrateContentByDepth[,2], nitrateContentByDepth[,1], type="b", ylim=c(max(nitrateContentByDepth[,1]),0),  main="Nitrate Content (mass/vol) vs. Depth", xlab="ugN/cm^3", ylab="Depth (m)")
outputFile <- file(paste(sep="", filename, " postprocessed ugNcm^-3 alldata", fileExt))
write(t(nitrateContentByBox[,,1]), outputFile, sep=separator, ncolumns=numCols)

if(useBulkDensity)
{
	outputFile <- file(paste(sep="", filename, " postprocessed ugNg^-1", fileExt))
	write(t(subset(nitrateContentByDepth, select=c(1,3))), outputFile, sep=separator, ncolumns=2)
	png(paste(sep="",filename," postprocessed ugNg^-1.png"), width=plotWidth, height=plotHeight)
	plot(nitrateContentByDepth[,3], nitrateContentByDepth[,1], type="b", ylim=c(max(nitrateContentByDepth[,1]),0),  main="Nitrate Content (mass/mass) vs. Depth", xlab="ugN/g soil", ylab="Depth (m)")
	outputFile <- file(paste(sep="", filename, " postprocessed ugNg^-1 alldata", fileExt))
	write(t(nitrateContentByBox[,,2]), outputFile, sep=separator, ncolumns=numCols)
}

outputFile <- file(paste(sep="", filename, " postprocessed kgNha^-1", fileExt))
write(t(subset(nitrateContentByDepth, select=c(1,4))), outputFile, sep=separator, ncolumns=2)
png(paste(sep="",filename," postprocessed kgNha^-1.png"), width=plotWidth, height=plotHeight)
plot(nitrateContentByDepth[,4], nitrateContentByDepth[,1], type="b", ylim=c(max(nitrateContentByDepth[,1]),0),  main="Nitrate Content (mass/surface area) vs. Depth", xlab="kgN/ha", ylab="Depth (m)")
outputFile <- file(paste(sep="", filename, " postprocessed kgNha^-1 alldata", fileExt))
write(t(nitrateContentByBox[,,3]), outputFile, sep=separator, ncolumns=numCols)
graphics.off()

setwd(thisDir)
