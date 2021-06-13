# R script to calculate phosphorus content

source(file.path(scriptDir,"Read_Raw_Data.r"))
rawPData <- rawData

surfaceArea <- (max(xb) - min(xb)) * (max(yb) - min(yb)) * 1e-8	# surface area in hectares

phosphorusContentByDepth <- array( ,c(numRows,4))	# Columns: depth, µg/cm^3, µg/g, kg/ha
phosphorusContentByBox <- array( ,c((numRows+1)*numSlices,numCols,3)) # 3rd dimension: µg/cm^3, µg/g, kg/ha
		
for(nz in 1:numRows)
{		
	thisLayerPDensities <- NULL
	totalLayerPMass <- 0
	totalLayerBulkMass <- 0
	thisLayerVolume <- NULL
	for(nx in 1:numCols)
	{
		for(ny in 1:numSlices)
		{
			thisLayerVolume <- c(thisLayerVolume, boxVolumes[nz,nx,ny])
			thisBoxPDensity <- sum(rawPData[((numRows+1)*(ny-1)+nz),nx])	# using numRows + 1 to account for blank delimiter rows				
			thisLayerPDensities <- c(thisLayerPDensities, thisBoxPDensity)					
			totalLayerPMass <- totalLayerPMass + thisBoxPDensity*boxVolumes[nz,nx,ny]
			
			phosphorusContentByBox[(numRows+1)*(ny-1)+nz,nx,1] <- thisBoxPDensity
			phosphorusContentByBox[(numRows+1)*(ny-1)+nz,nx,3] <- thisBoxPDensity*boxVolumes[nz,nx,ny]*1e-9/boxSurfaceAreas[nx,ny]

			if(useBulkDensity)
			{
				thisBoxBulkDensity <- sum(rawBulkDensityData[((numRows+1)*(ny-1)+nz),nx])	# using numRows + 1 to account for blank delimiter rows
				totalLayerBulkMass <- totalLayerBulkMass + thisBoxBulkDensity*boxVolumes[nz,nx,ny]
				phosphorusContentByBox[(numRows+1)*(ny-1)+nz,nx,2] <- thisBoxBulkDensity*boxVolumes[nz,nx,ny]
			}
		}	
	}
	phosphorusContentByDepth[nz,1] <- zmidpoints[nz]/100		# converting depth from cm to m
	phosphorusContentByDepth[nz,2] <- mean(thisLayerPDensities)
	if(useBulkDensity)
	{
		phosphorusContentByDepth[nz,3] <- totalLayerPMass/totalLayerBulkMass
	}
	phosphorusContentByDepth[nz,4] <- totalLayerPMass * 1e-9 / surfaceArea	# converting mass from µg to kg and dividing by number of hectares
}
	
setwd(thisOutputDir)

outputFile <- file(paste(sep="", filename, " postprocessed µgPcm^-3", fileExt))
write(t(subset(phosphorusContentByDepth, select=c(1,2))), outputFile, sep=separator, ncolumns=2)
png(paste(sep="",filename," postprocessed µgPcm^-3.png"), width=plotWidth, height=plotHeight)
plot(phosphorusContentByDepth[,2], phosphorusContentByDepth[,1], type="b", ylim=c(max(phosphorusContentByDepth[,1]),0), main="Phosphorus Content (mass/vol) vs. Depth",  xlab="µgP/cm^3", ylab="Depth (m)")
outputFile <- file(paste(sep="", filename, " postprocessed µgPcm^-3 alldata", fileExt))
write(t(phosphorusContentByBox[,,1]), outputFile, sep=separator, ncolumns=numCols)

if(useBulkDensity)
{
	outputFile <- file(paste(sep="", filename, " postprocessed µgPg^-1", fileExt))
	write(t(subset(phosphorusContentByDepth, select=c(1,3))), outputFile, sep=separator, ncolumns=2)
	png(paste(sep="",filename," postprocessed µgPg^-1.png"), width=plotWidth, height=plotHeight)
	plot(phosphorusContentByDepth[,3], phosphorusContentByDepth[,1], type="b", ylim=c(max(phosphorusContentByDepth[,1]),0), main="Phosphorus Content (mass/mass) vs. Depth",  xlab="µgP/g soil", ylab="Depth (m)")
	outputFile <- file(paste(sep="", filename, " postprocessed µgPg^-1 alldata", fileExt))
	write(t(phosphorusContentByBox[,,2]), outputFile, sep=separator, ncolumns=numCols)
}

outputFile <- file(paste(sep="", filename, " postprocessed kgPha^-1", fileExt))
write(t(subset(phosphorusContentByDepth, select=c(1,4))), outputFile, sep=separator, ncolumns=2)
png(paste(sep="",filename," postprocessed kgPha^-1.png"), width=plotWidth, height=plotHeight)
plot(phosphorusContentByDepth[,4], phosphorusContentByDepth[,1], type="b", ylim=c(max(phosphorusContentByDepth[,1]),0), main="Phosphorus Content (mass/surface area) vs. Depth",  xlab="kgP/ha", ylab="Depth (m)")
outputFile <- file(paste(sep="", filename, " postprocessed kgPha^-1 alldata", fileExt))
write(t(phosphorusContentByBox[,,3]), outputFile, sep=separator, ncolumns=numCols)
graphics.off()

setwd(thisDir)