if(exists("datasubdir"))
{
	absoluteFilename <- file.path(getwd(),datasubdir,filename)
} else
{
	absoluteFilename <- file.path(getwd(),filename)
}
print("Reading data from:")
print(absoluteFilename)
rawData <- read.delim(absoluteFilename, sep=separator, header=FALSE, blank.lines.skip=FALSE, encoding="Latin-1")

rawLength <- length(rawData[,1])
numCols <- length(rawData[1,])

# Here we must ensure the last column isn't spurious NAs.
# This may occur due to cross-platform differences;
# the newline character differs between Mac, Windows and Unix.
if(all(is.na(rawData[,numCols])))
{
	#print(rawData)
	#print("As shown above, last column is all NAs (due to cross-platform newline character differences). Removing.")
	rawData <- rawData[1:rawLength,1:length(rawData[1,])-1]
	numCols <- numCols - 1
}

# Find the blank lines which delimit planar slices
# TODO: Parse XML to find out the planar dimension. Typically it's the Y=k (or XZ) plane
# (Scoreboard sliced from front to back), but this is NOT GUARANTEED.
planarSliceDelimiters <- NULL
first <- TRUE
for(i in 1:rawLength)
{
	if(is.na(rawData[i,1]))
	{
		planarSliceDelimiters <- c(planarSliceDelimiters,i)
		if(first)
		{
			numRows <- (i - 1)
			first <- FALSE
		}
	}
}
numSlices <- length(planarSliceDelimiters)
# Ensure that the last line is a blank one (i.e. a delimiter)
if(!all(is.na(rawData[rawLength,])))
{
	numSlices <- numSlices+1
	planarSliceDelimiters <- c(planarSliceDelimiters, rawLength+1)
}

numSlicesPerTimestep <- length(yb)-1

# If we output a 2D array for each planar slice of the Scoreboard every output timestep,
# the number of output timesteps is the number of slices divided by the number of planar slices.
# TODO: Generalise this for all slicing configurations, not just XZ planar slices
numTimesteps <- numSlices / numSlicesPerTimestep


