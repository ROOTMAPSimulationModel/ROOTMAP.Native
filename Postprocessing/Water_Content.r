# R script to calculate Water Content

source(file.path(scriptDir,"Read_Raw_Data.r"))

meanWaterByDepth <- array( ,c(numRows,2))	# Columns: depth, water content
		
for(nz in 1:numRows)
{		
	thisWC <- NULL
	for(nx in 1:numCols)
	{
		for(ny in 1:numSlices)
		{
			thisWC <- c(thisWC, sum(rawData[((numRows+1)*(ny-1)+nz),nx]))	# using numRows + 1 to account for blank delimiter rows				
		}
	}			
	meanWaterByDepth[nz,1] <- zmidpoints[nz]/100
	meanWaterByDepth[nz,2] <- mean(thisWC)

}

setwd(thisOutputDir)

outputFile <- file(paste(sep="", filename, "postprocessed cm^3[water]cm^-3[soil]", fileExt))
write(t(meanWaterByDepth), outputFile, sep=separator, ncolumns=2)
png(paste(sep="",filename," postprocessed cm^3[water]cm^-3[soil].png"), width=plotWidth, height=plotHeight)
plot(meanWaterByDepth[,2], meanWaterByDepth[,1], type="b", ylim=c(max(meanWaterByDepth[,1]),0), main="Mean Water Content by Depth", xlab="Water Content (cm^3(water)/cm^3(soil))", ylab="Depth (m)")
graphics.off()

setwd(thisDir)