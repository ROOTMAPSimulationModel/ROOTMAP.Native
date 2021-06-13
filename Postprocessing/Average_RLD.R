# R script to calculate Root Length & Root Length Density averages

# MSA 11.05.10 Kludgy way to preload numRows. TODO: Do this better
source(file.path(getwd(),"Read_Raw_Data.r"))


dataStackColumnCount <- 2*length(filenames)+1
dataStack <- array( ,c(numRows,dataStackColumnCount))
dataStack[,1] <- zmidpoints
dscn <- c("Depth Layer Midpoint")
fnn <- 1
for(i in 2:dataStackColumnCount)
{
	dataStack[,i] <- seq(0,0,length=numRows)
	if(i %% 2 == 0)
	{
		dscn <- c(dscn, paste("Root Length Density (cm/cm^3)",filenames[fnn]))
	}
	else
	{
		dscn <- c(dscn, paste("Total Root Length (cm)",filenames[fnn]))
		fnn <- fnn + 1
	}
}
colnames(dataStack) <- dscn

nextCol <- 2

for(filename in filenames)
{
	print(paste("Calculating average Root Length and Root Length Density from file:",filename))
	source(file.path(getwd(),"Read_Raw_Data.r"))

	# Generate 3-dimensional arrays for the Root Length and Root Length Density of each box
	volumeRLs <- array(, c(numRows,numCols,numSlices))	# Volume Root Lengths
	volumeRLDs <- array( ,c(numRows,numCols,numSlices))	# Volume Root Length Densities

	# Here, we use volumes within the VolumeObject(s) if available, otherwise just raw Scoreboard volumes.
	# MSA TODO Implement this (and an OutputRule to produce the VO-wise volumes?)
	if(exists("coincidentVolumeProportions"))
	{
		effectiveVolumes <- boxVolumes
	} else
	{
		effectiveVolumes <- boxVolumes
	}

	for(nx in 1:numCols)
	{		
		for(nz in 1:numRows)
		{
			for(ny in 1:numSlices)
			{
				thisRL <- sum(rawData[((numRows+1)*(ny-1)+nz),nx])	# using numRows + 1 to account for blank delimiter rows
				volumeRLs[nz,nx,ny] <- thisRL
				thisEffectiveVolume <- effectiveVolumes[nz,nx,ny]
				if(thisEffectiveVolume==0.0)
				{
					volumeRLDs[nz,nx,ny] <- 0.0
				} else
				{
					volumeRLDs[nz,nx,ny] <- thisRL / thisEffectiveVolume
				}
			}
		}
	}

	# MSA 11.01.13 Converted this BACK to using cm/cm^3.

	rootsByDepth <- array( ,c(numRows,3))	# Columns: depth, RLD (cm/cm^3), Total RL (cm)

	for(nz in 1:numRows)
	{
		meanRLD <- sum(volumeRLDs[nz,,], na.rm=TRUE)/(numCols*numSlices)
		rootsByDepth[nz,1] <- zmidpoints[nz]				#/100		# Convert depth from cm to m
		rootsByDepth[nz,2] <- meanRLD 					#* (100^3/100)		# Convert RLD from cm/cm^3 to m/m^3
		rootsByDepth[nz,3] <- sum(volumeRLs[nz,,], na.rm=TRUE) 	#/ 100	# Convert RL from cm to m
	}
	if(exists("datasubdir"))
	{
		basewd <- getwd()
		setwd(file.path(basewd,datasubdir))
	}
	
	dataStack[,nextCol] <- rootsByDepth[,2]
	dataStack[,nextCol+1] <- rootsByDepth[,3]
	nextCol <- nextCol + 2

	outputFile <- file(paste(sep="", filename, "postprocessed cm cm^-3", fileExt))
	write(t(subset(rootsByDepth, select=c(1,2))), outputFile, sep=separator, ncolumns=2)
	png(paste(sep="",filename," postprocessed cm cm^-3.png"), width=plotWidth, height=plotHeight)
	plot(rootsByDepth[,2], rootsByDepth[,1], type="b", ylim=c(max(rootsByDepth[,1]),0), main=paste("Mean Root Length Density by Depth:",filename), xlab="RLD (cm/cm^3)", ylab="Depth (cm)")

	outputFile <- file(paste(sep="", filename, "postprocessed cm", fileExt))
	write(t(subset(rootsByDepth, select=c(1,3))), outputFile, sep=separator, ncolumns=2)
	png(paste(sep="",filename," postprocessed cm.png"), width=plotWidth, height=plotHeight)
	plot(rootsByDepth[,3], rootsByDepth[,1], type="b", ylim=c(max(rootsByDepth[,1]),0), main=paste("Aggregate Root Length by Depth:",filename), xlab="RL (cm)", ylab="Depth (cm)")

	graphics.off()

	if(exists("datasubdir"))
	{
		setwd(basewd)
	}

	print("Computed Root Length Density (cm/cm^3) and Total Root Length (cm) per horizontal layer.")
	print("DEPTH OF LAYER MIDPOINT, ROOT LENGTH DENSITY, TOTAL ROOT LENGTH:")
	print(rootsByDepth)
}

outputFile <- file(paste(sep="", "RL_RLD", fileExt))
write.table(dataStack, outputFile, sep=separator, row.names=FALSE, col.names=TRUE)

rm(filename)