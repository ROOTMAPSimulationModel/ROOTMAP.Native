# Sum_Per_Plant.r

# Important note: all R scripts expect XZ plane slices. Gotta update 'em to read the OutputRules' <specification1>

rm(list=ls())

separator <- ","

# Temporary. Filenames to be parsed and loaded by ROOTMAP_Postprocessor.r
filenames <- c("TapRootLength1.txt", "TapRootLength2.txt", "TapRootLength3.txt", "TapRootLength4.txt")

source(file.path("C:/RM/build/win32_d","Read_Boundaries.r"))

# GAH! Ugly
maxTimesteps <- 0
for(filename in filenames)
{
	filename <- file.path("C:/RM/build/win32_d",filename)
	source(file.path("C:/RM/build/win32_d","Read_Raw_Data.r"))
	
	maxTimesteps <- max(maxTimesteps, numTimesteps)
}

firstFile <- TRUE

for(filename in filenames)
{
	filename <- file.path("C:/RM/build/win32_d",filename)
	source(file.path("C:/RM/build/win32_d","Read_Raw_Data.r"))
	
	maxTimesteps <- max(maxTimesteps, numTimesteps)

	# Array of values per timestep
	sumsPerTimestep <- array(dim=numTimesteps)

	for(iT in 1:numTimesteps)
	{
		sumsPerTimestep[iT] <- 0.0
		for(iSPT in 1:numSlicesPerTimestep)
		{
			start <- 1 + (iT-1)*numSlicesPerTimestep*length(zb) + (iSPT-1)*length(zb)
			end <- start + length(zb)-2
			sumsPerTimestep[iT] <- sumsPerTimestep[iT] + sum(rawData[c(start:end),]) # add the sum of this "slice"
		}
	}
	
	while(length(sumsPerTimestep)<maxTimesteps)
	{
		sumsPerTimestep <- c(sumsPerTimestep, NA) 
	}
	
	if(firstFile==TRUE)
	{
		results <- sumsPerTimestep
		firstFile <- FALSE
	}
	else
	{
		results <- cbind(results, sumsPerTimestep)
	}
}

# Total Aggregate Root Lengths per timestep
outputFile <- file(paste(sep="", "C:/RM/build/win32_d/", "TEMP", ".csv"))
timeStep <- c(1:maxTimesteps)
columns <- c("Timestep",filenames)
write.table(sep=",", col.names=columns, data.frame(timeStep,results), file=outputFile, row.names=FALSE)
